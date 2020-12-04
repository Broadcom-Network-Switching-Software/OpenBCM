/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Layer 2 Management
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_MIM
#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <bcm/module.h>
#include <bcm/field.h>
#include <bcm_int/petra_dispatch.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/mim.h>
#include <bcm_int/dpp/vswitch.h>
#include <bcm_int/dpp/vlan.h>
#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/alloc_mngr_local_lif.h>
#include <bcm_int/dpp/alloc_mngr_glif.h>
#include <bcm_int/dpp/sw_db.h>
#include <bcm_int/dpp/port.h>
#include <bcm_int/dpp/switch.h>

#include <bcm_int/common/multicast.h>

#include <soc/dpp/PPD/ppd_api_mymac.h>
#include <soc/dpp/PPD/ppd_api_frwrd_bmact.h>
#include <soc/dpp/PPD/ppd_api_frwrd_fec.h>
#include <soc/dpp/PPD/ppd_api_frwrd_mact_mgmt.h>
#include <soc/dpp/PPD/ppd_api_lif.h>
#include <soc/dpp/PPD/ppd_api_vsi.h>
#include <soc/dpp/PPD/ppd_api_llp_parse.h>
#include <soc/dpp/PPD/ppd_api_eg_ac.h>


#include <soc/dpp/ARAD/arad_sw_db.h>

#include <soc/dpp/mbcm.h>

#if defined(BCM_ARAD_SUPPORT) && defined(CRASH_RECOVERY_SUPPORT) && defined(BCM_WARM_BOOT_API_TEST)
#include <soc/dcmn/dcmn_crash_recovery.h>
#endif
/* 
 * Globals
 */

/*
 * MiM Module Helper functions
 */

uint8
  __dpp_mim_initialized_get(int unit)
{
    uint8 initialized = 0;
    if ((unit >= 0) && (unit < BCM_LOCAL_UNITS_MAX)) {
        /* write mim_out_ac to default_sem_index reg */
        if (SOC_DPP_PP_ENABLE(unit)) {
            soc_ppd_frwrd_mact_mim_init_get(unit, &initialized);
        }
    }
    return initialized;
}

SOC_PPC_LIF_ID
  __dpp_mim_lif_ndx_get(int unit)
{
    return SOC_DPP_CONFIG(unit)->pp.mim_global_lif_ndx;
}

SOC_PPC_AC_ID
  __dpp_mim_global_out_ac_get(int unit)
{
    SOC_PPC_AC_ID  mim_local_out_ac = -1;
    MIM_ACCESS.mim_local_out_ac.get(unit, &mim_local_out_ac);

    /* No default mim out ac in AVT mode, so return null out ac */
    return ((SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED(unit)) ?  ARAD_PP_EG_ENCAP_EEDB_INDEX_TO_OUTLIF(unit, SOC_PPC_LIF_NULL_LOCAL_OUTLIF_ID) : mim_local_out_ac);
}

/*
 * Local out lif id for data entry used for EoE
 * Better solution required rather than reseving a local out lif for data entry internally (Improve it when users complain about it)
 */
#define _BCM_DPP_MIM_OUT_LIF            (0x1000);


/*********** SW DBs translation functions *******************/
int
_bcm_dpp_in_lif_mim_match_add(int unit, bcm_mim_port_t *mim_port, int lif)
{
    _bcm_dpp_gport_sw_resources gport_sw_resources;
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
                  
    rv = _bcm_dpp_local_lif_to_sw_resources(unit, lif, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS, &gport_sw_resources);
    BCMDNX_IF_ERR_EXIT(rv);    
                                
    gport_sw_resources.in_lif_sw_resources.criteria = mim_port->criteria;
    gport_sw_resources.in_lif_sw_resources.flags = mim_port->flags;
    gport_sw_resources.in_lif_sw_resources.port = mim_port->port;
    gport_sw_resources.in_lif_sw_resources.match1 = mim_port->match_tunnel_srcmac[0]; /* src_mac LSB */
    gport_sw_resources.in_lif_sw_resources.match2 = mim_port->match_tunnel_srcmac[4]; /* src_mac MSB */
    gport_sw_resources.in_lif_sw_resources.key1 = mim_port->egress_tunnel_service; /* I-SID */
    gport_sw_resources.in_lif_sw_resources.lif_type = _bcmDppLifTypeMim;
    gport_sw_resources.in_lif_sw_resources.gport_id =  mim_port->mim_port_id;

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_local_lif_sw_resources_set(unit, lif, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS, &gport_sw_resources));

    BCM_EXIT;

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_in_lif_mim_match_get(int unit, bcm_mim_port_t *mim_port, int lif)
{
    _bcm_dpp_gport_sw_resources gport_sw_resources;
    int rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_local_lif_to_sw_resources(unit, lif, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS, &gport_sw_resources);
    BCMDNX_IF_ERR_EXIT(rv);

    mim_port->criteria = gport_sw_resources.in_lif_sw_resources.criteria;
    mim_port->flags = gport_sw_resources.in_lif_sw_resources.flags;
    mim_port->port = gport_sw_resources.in_lif_sw_resources.port;
    mim_port->match_tunnel_srcmac[0] = gport_sw_resources.in_lif_sw_resources.match1; /* src_mac LSB */
    mim_port->match_tunnel_srcmac[4] = gport_sw_resources.in_lif_sw_resources.match2; /* src_mac MSB */
    mim_port->egress_tunnel_service = gport_sw_resources.in_lif_sw_resources.key1; /* I-SID */
    mim_port->mim_port_id = gport_sw_resources.in_lif_sw_resources.gport_id;

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_out_lif_mim_match_add(int unit, bcm_mim_port_t *mim_port, int lif)
{
    int rv = BCM_E_NONE;
    _bcm_dpp_gport_sw_resources gport_sw_resources;
    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_local_lif_to_sw_resources(unit, -1, lif, _BCM_DPP_GPORT_SW_RESOURCES_EGRESS, &gport_sw_resources);
    BCMDNX_IF_ERR_EXIT(rv);

    gport_sw_resources.out_lif_sw_resources.lif_type = _bcmDppLifTypeMim;

    rv = _bcm_dpp_local_lif_sw_resources_set(unit, -1, lif, _BCM_DPP_GPORT_SW_RESOURCES_EGRESS, &gport_sw_resources);
    BCMDNX_IF_ERR_EXIT(rv);    

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      dpp_mim_set_global_mim_tpid
 * Purpose:
 *      set the I-tag TPID to MiM TPID
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */
int dpp_mim_set_global_mim_tpid(int unit, uint16 tpid){

    int soc_sand_rv;
    SOC_PPC_LLP_PARSE_TPID_VALUES tpid_vals;
    BCMDNX_INIT_FUNC_DEFS;

    soc_sand_rv = soc_ppd_llp_parse_tpid_values_get(unit, &tpid_vals);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    
    tpid_vals.tpid_vals[4] = tpid;
    
    soc_sand_rv = soc_ppd_llp_parse_tpid_values_set(unit, &tpid_vals);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      dpp_mim_get_global_mim_tpid
 * Purpose:
 *      get the I-tag TPID to MiM TPID
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */
int dpp_mim_get_global_mim_tpid(int unit, uint16 *tpid){

    int soc_sand_rv;
    SOC_PPC_LLP_PARSE_TPID_VALUES tpid_vals;
    BCMDNX_INIT_FUNC_DEFS;

    if (!tpid) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, 
            (_BSL_BCM_MSG("Parameter of tpid should be a valid pointer!")));
    }

    soc_sand_rv = soc_ppd_llp_parse_tpid_values_get(unit, &tpid_vals);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    *tpid = tpid_vals.tpid_vals[4];

exit:
    BCMDNX_FUNC_RETURN;
}



/*
 * Function:
 *      bcm_petra_mim_init
 * Purpose:
 *      Initialize the MIM software module
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_petra_mim_init(int unit)
{
    int rv = BCM_E_NONE;
    uint32 soc_sand_rv;
    uint32 soc_sand_dev_id;
    uint32 profile_enablers;
    SOC_PPC_EG_AC_INFO ac_info;
    bcm_dpp_am_local_inlif_info_t local_inlif_info;
    bcm_dpp_am_local_out_lif_info_t local_out_lif_info;
    SOC_PPC_AC_ID  mim_local_out_ac; 


    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    if (__dpp_mim_initialized_get(unit)) {
         BCM_EXIT;
    }
    /* Enable MiM */

    if (!SOC_WARM_BOOT(unit)) {

        MIM_ACCESS.alloc(unit);

        if (!SOC_IS_ARADPLUS(unit)) {
            if ((SOC_DPP_CONFIG(unit)->pp.next_hop_mac_extension_enable)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("next hop mac extension soc property and mim do not coexist")));
            }
        }

        if (SOC_IS_JERICHO_PLUS(unit)) {
            /* Set the default IN-RIF profile (for VSIs that doesn't have RIF) to accept MIM  */
            profile_enablers = ((1 << SOC_PPC_ROUTING_ENABLE_MIM_UC_BIT) | (1 << SOC_PPC_ROUTING_ENABLE_MIM_MC_BIT));
            if (SOC_DPP_CONFIG(unit)->pp.custom_ip_route) {
                profile_enablers = ((1 << SOC_PPC_ROUTING_ENABLE_IPV4UC_BIT) | (1 << SOC_PPC_ROUTING_ENABLE_IPV4MC_BIT));
                profile_enablers |= ((1 << SOC_PPC_ROUTING_ENABLE_IPV6UC_BIT) | (1 << SOC_PPC_ROUTING_ENABLE_IPV6MC_BIT));
            }
            rv = arad_pp_ihp_map_rif_profile_to_routing_enable_tbl_set_unsafe(unit, 0 /* default in-rif profile*/, profile_enablers);
            BCM_IF_ERROR_RETURN(rv);
        }

        soc_sand_dev_id = (unit);

        if (!SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED(unit)) { /* Standard vlan translate mode has a default ac for MIM, while AVT mode doesn't */

            if (SOC_IS_JERICHO(unit)) {

                sal_memset(&local_inlif_info, 0, sizeof(local_inlif_info));
                sal_memset(&local_out_lif_info, 0, sizeof(local_out_lif_info));

                /* 
                 *  Allocate mim ingress global lif.
                 */

                /* Set the local lif info */
                local_inlif_info.local_lif_flags = BCM_DPP_AM_IN_LIF_FLAG_COMMON;
                local_inlif_info.app_type = bcm_dpp_am_ingress_lif_app_ingress_ac;
                local_inlif_info.counting_profile_id = BCM_DPP_AM_COUNTING_PROFILE_NONE;

                /* Allocate the global and loca lif */
                rv = _bcm_dpp_gport_alloc_global_and_local_lif(unit, BCM_DPP_AM_FLAG_ALLOC_WITH_ID, (int*)&SOC_DPP_CONFIG(unit)->pp.mim_global_lif_ndx, &local_inlif_info, NULL);
                BCM_IF_ERROR_RETURN(rv);

                /* Retrieve the allocated local lif */
                MIM_ACCESS.mim_local_lif_ndx.set(unit, local_inlif_info.base_lif_id);


                /* 
                 * Allocate the egress mim global lif.
                 */                                   

                /* Set the local lif info */
                local_out_lif_info.app_alloc_info.pool_id = dpp_am_res_eg_out_ac;
                local_out_lif_info.app_alloc_info.application_type = bcm_dpp_am_egress_encap_app_out_ac;
                local_out_lif_info.counting_profile_id = BCM_DPP_AM_COUNTING_PROFILE_NONE;

                /* Allocate the global and local lif */
                rv = _bcm_dpp_gport_alloc_global_and_local_lif(unit, BCM_DPP_AM_FLAG_ALLOC_WITH_ID, (int*)&SOC_DPP_CONFIG(unit)->pp.mim_global_out_ac, NULL, &local_out_lif_info);
                BCM_IF_ERROR_RETURN(rv);

                /* Retrieve the allocated local lif */
                MIM_ACCESS.mim_local_out_ac.set(unit, local_out_lif_info.base_lif_id);

            }
            if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
                /* alloc a special lif, used only fof MiM B-VID -> B-VSI */
                rv = bcm_dpp_am_l2_ac_alloc(unit, _BCM_DPP_AM_L2_AC_TYPE_VLAN_EDITING, BCM_DPP_AM_FLAG_ALLOC_WITH_ID, &SOC_DPP_CONFIG(unit)->pp.mim_global_lif_ndx);
                BCMDNX_IF_ERR_EXIT(rv);

                MIM_ACCESS.mim_local_lif_ndx.set(unit, SOC_DPP_CONFIG(unit)->pp.mim_global_lif_ndx);

                /* alloc a special out-ac, used only fof MiM */
                rv = bcm_dpp_am_out_ac_alloc(unit, _BCM_DPP_AM_OUT_AC_TYPE_DEFAULT, BCM_DPP_AM_FLAG_ALLOC_WITH_ID, &SOC_DPP_CONFIG(unit)->pp.mim_global_out_ac);
                BCMDNX_IF_ERR_EXIT(rv);

                MIM_ACCESS.mim_local_out_ac.set(unit, SOC_DPP_CONFIG(unit)->pp.mim_global_out_ac);

            }

            MIM_ACCESS.mim_local_out_ac.get(unit, &mim_local_out_ac); 
            /* set out-ac to point at default profile */
            SOC_PPC_EG_AC_INFO_clear(&ac_info);
            ac_info.edit_info.nof_tags = 2; /* nof tags must be 2 */
            soc_sand_rv = soc_ppd_eg_ac_info_set(soc_sand_dev_id, mim_local_out_ac, &ac_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
        
        soc_sand_rv = soc_ppd_frwrd_bmact_init(soc_sand_dev_id);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* in ARAD the deafult out_ac is per_port and will be done in bcm_petra_port_control_set() with bcmPortControlMacInMac */
        
        /* set the I-tag TPID */
        rv = dpp_mim_set_global_mim_tpid(unit, BCM_PETRA_MIM_ITAG_TPID);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Set the condition to create a data entry for prge program selection,
        data entry is required only when working in arad+ and below, from Jericho on
        we use outlif profile */
        if (SOC_IS_ARADPLUS_AND_BELOW(unit) && (SOC_DPP_CONFIG(unit)->pp.test2 || SOC_DPP_CONFIG(unit)->pp.test1)  ) {
            SOC_PPC_EG_ENCAP_DATA_INFO data_info;
            SOC_PPC_EG_ENCAP_DATA_INFO_clear(&data_info);
 
            SOC_DPP_CONFIG(unit)->pp.mim_local_out_lif = _BCM_DPP_MIM_OUT_LIF;
            rv = bcm_dpp_am_out_ac_alloc(unit, _BCM_DPP_AM_OUT_AC_TYPE_PON_3_TAGS_DATA, BCM_DPP_AM_FLAG_ALLOC_WITH_ID, &SOC_DPP_CONFIG(unit)->pp.mim_local_out_lif);
            BCMDNX_IF_ERR_EXIT(rv);
            SOC_DPP_CONFIG(unit)->pp.mim_local_out_lif_base = 0x80;

            data_info.data_entry[0] = SOC_PPD_EG_ENCAP_TEST2_PROG_VAR;
            soc_sand_rv = soc_ppd_eg_encap_data_lif_entry_add(soc_sand_dev_id, SOC_DPP_CONFIG(unit)->pp.mim_local_out_lif, &data_info, FALSE, 0);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
        soc_sand_rv = soc_ppd_frwrd_mact_mim_init_set(soc_sand_dev_id, TRUE);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_mim_detach(int unit)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);                      

    if (!__dpp_mim_initialized_get(unit)) {
         BCM_EXIT;
    }

exit:
    BCMDNX_FUNC_RETURN;

}

/*
 * Function:
 *      bcm_petra_mim_vpn_create
 * Purpose:
 *      Create a VPN instance
 * Parameters:
 *      unit  - (IN)  Device Number
 *      info  - (IN/OUT) VPN configuration info
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_petra_mim_vpn_create(
    int unit, 
    bcm_mim_vpn_config_t *info)
{
    int rv = BCM_E_NONE;
    uint32 soc_sand_rv;
    uint32 soc_sand_dev_id;
    int bc, bc_type, uc, uc_type, mc, mc_type;
    uint32 flood_vpn, vpn = info->vpn; /* save initial vpn */
    SOC_PPC_BMACT_BVID_INFO bvid_info;
    SOC_PPC_VSI_INFO vsi_info;
    bcm_vlan_control_vlan_t control;
    SOC_PPC_SYS_VSI_ID vsi_id, tmp_vsi;
    SOC_PPC_L2_LIF_ISID_KEY isid_key;
    SOC_SAND_PP_ISID old_lookup_id = 0;
    SOC_PPC_LIF_ID tmp_lif, isid_local_lif_id = 0;
    int isid_global_lif_id = 0;
    SOC_PPC_L2_LIF_ISID_INFO isid_info;
    SOC_SAND_SUCCESS_FAILURE success;
    uint8 found, replaced;
    uint8 update;
    uint8 is_ingress, is_egress;
    uint32 l2_ac_alloc_flags = 0;
    bcm_mim_port_t mim_port;
    bcm_dpp_am_local_inlif_info_t local_inlif_info;    
    uint8 sw_state_is_ingress, sw_state_is_egress, sw_state_is_valid, sw_state_bsa_nickname_valid;
    uint32 sw_state_isid, mim_info_bsa_nickname, sw_state_bsa_nickname;
    SOC_SAND_PP_ISID egress_isid;
    uint32 mim_info_flags;
    uint8 bsa_valid, old_bsa_nickname_valid = 0;
    uint32 bsa_nickname, old_bsa_nickname = 0;

    BCMDNX_INIT_FUNC_DEFS;

#if defined(BCM_ARAD_SUPPORT) && defined(CRASH_RECOVERY_SUPPORT) && defined(BCM_WARM_BOOT_API_TEST)
    soc_dcmn_cr_suppress(unit, dcmn_cr_no_support_in_out_parameter);
#endif
    BCM_DPP_UNIT_CHECK(unit);

    /* make sure MiM is enabled */
    MIM_INIT(unit);

    /* function must be called with VPN id */
    if (!(info->flags & BCM_MIM_VPN_WITH_ID)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_MIM_VPN_WITH_ID must be set")));
    }
    
    /* Can't have both INGRESS_ONLY and EGRESS_ONLY flags */
    if ((info->flags & BCM_MIM_VPN_INGRESS_ONLY) && (info->flags & BCM_MIM_VPN_EGRESS_ONLY)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Can't have both BCM_MIM_VPN_INGRESS_ONLY and BCM_MIM_VPN_EGRESS_ONLY flags")));
    }

    /* INGRESS_ONLY and EGRESS_ONLY flags are valid only for VSI type VPNs, not for BVID */
    if ((info->flags & BCM_MIM_VPN_BVLAN) && ((info->flags & BCM_MIM_VPN_INGRESS_ONLY) || (info->flags & BCM_MIM_VPN_EGRESS_ONLY))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Can't have BCM_MIM_VPN_INGRESS_ONLY or BCM_MIM_VPN_EGRESS_ONLY flags with BCM_MIM_VPN_BVLAN")));
    }
        
    /* make sure I-SID is in range */
    if (info->lookup_id < 0 || info->lookup_id > SOC_SAND_PP_ISID_MAX) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("I-SID must be a 24bit value")));
    }

    update = ((info->flags & BCM_MIM_VPN_REPLACE) ? TRUE : FALSE);
    is_ingress = !(info->flags & BCM_MIM_VPN_EGRESS_ONLY);
    is_egress = !(info->flags & BCM_MIM_VPN_INGRESS_ONLY);
    bsa_valid = ((info->flags & BCM_MIM_VPN_MATCH_SERVICE_SMAC) ? TRUE : FALSE);

    /* SPB must be enabled for BSA+ISID */
    if (bsa_valid && !soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mim_spb_enable", 0)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("SPB must be enabled for BCM_MIM_VPN_MATCH_SERVICE_SMAC")));
    }

    soc_sand_dev_id = (unit);

    if (info->flags & BCM_MIM_VPN_BVLAN) { /* B-VID */

        /* add 0x1111 to B-VID (to distinguish from vlan) */
      info->vpn = _BCM_DPP_VLAN_TO_BVID(unit, info->vpn);

        /* set flooding for B-VID */
        bc_type = _BCM_MULTICAST_TYPE_GET(info->broadcast_group);
        bc = _BCM_MULTICAST_ID_GET(info->broadcast_group);
        mc_type = _BCM_MULTICAST_TYPE_GET(info->unknown_multicast_group);
        mc = _BCM_MULTICAST_ID_GET(info->unknown_multicast_group);
        uc_type = _BCM_MULTICAST_TYPE_GET(info->unknown_unicast_group);
        uc = _BCM_MULTICAST_ID_GET(info->unknown_unicast_group);

        /* all groups should be equal to B-VID+12K and of type MIM */
        flood_vpn = vpn + BCM_PETRA_MIM_BVID_MC_GROUP_BASE;
        if ((bc_type != _BCM_MULTICAST_TYPE_MIM) ||
            (mc_type != _BCM_MULTICAST_TYPE_MIM) ||
            (uc_type != _BCM_MULTICAST_TYPE_MIM) ||
            (bc != flood_vpn) || (mc != flood_vpn) || (uc != flood_vpn)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("broadcast_group, unknown_multicast_group and unknown_unicast_group must be "
                                                    "of type BCM_MULTICAST_TYPE_MIM and equal to vpn+12K")));
        }
     
        /* set B-VID FID class */
        soc_sand_rv = soc_ppd_frwrd_bmact_bvid_info_get(soc_sand_dev_id, vpn, &bvid_info); 
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        bvid_info.b_fid_profile = SOC_PPC_BFID_EQUAL_TO_BVID; /* B-FID = B-VID */
        SOC_PPD_FRWRD_DECISION_MC_GROUP_SET(soc_sand_dev_id, &(bvid_info.uknown_da_dest), flood_vpn, soc_sand_rv);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        soc_sand_rv = soc_ppd_frwrd_bmact_bvid_info_set(soc_sand_dev_id, vpn, &bvid_info); 
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* For Jericho devices due to MiM erratum need to set the unknown_da in the regular VSI table and not the BVID table */
        if (SOC_IS_JERICHO_A0(unit) || SOC_IS_JERICHO_B0(unit) || SOC_IS_QMX(unit)) {
            rv = bcm_dpp_am_l2_vpn_vsi_is_alloced(unit, info->vpn); 
            if (rv != BCM_E_EXISTS) {
                /* allocate the VSI for MIM usage */
                rv = _bcm_dpp_vswitch_vsi_usage_alloc(unit,BCM_DPP_AM_FLAG_ALLOC_WITH_ID,_bcmDppVsiTypeMim,&info->vpn,&replaced);
                BCMDNX_IF_ERR_EXIT(rv);
            }
            soc_sand_rv = soc_ppd_vsi_info_get(soc_sand_dev_id, info->vpn, &vsi_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            SOC_PPD_FRWRD_DECISION_MC_GROUP_SET(soc_sand_dev_id, &(vsi_info.default_forwarding), flood_vpn, soc_sand_rv);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            soc_sand_rv = soc_ppd_vsi_info_set(soc_sand_dev_id, info->vpn, &vsi_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }

        /* set control to independent VLAN Learning */
        bcm_vlan_control_vlan_t_init(&control);

        control.forwarding_vlan = info->vpn;

        rv = bcm_petra_vlan_control_vlan_set(unit, info->vpn, control);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else { /* VSI */
        BCMDNX_IF_ERR_EXIT(MIM_ACCESS.mim_vpn_flags.get(unit,info->vpn,&mim_info_flags));
        BCMDNX_IF_ERR_EXIT(MIM_ACCESS.mim_vpn_bsa_nickname.get(unit,info->vpn,&mim_info_bsa_nickname));

        sw_state_isid = _BCM_DPP_MIM_VPN_ISID_GET(mim_info_flags);
        sw_state_is_valid = _BCM_DPP_MIM_VPN_IS_VALID(mim_info_flags);
        sw_state_is_ingress = _BCM_DPP_MIM_VPN_IS_INGRESS(mim_info_flags);
        sw_state_is_egress = _BCM_DPP_MIM_VPN_IS_EGRESS(mim_info_flags);
        sw_state_bsa_nickname = _BCM_DPP_MIM_VPN_BSA_NICKNAME_GET(mim_info_bsa_nickname);
        sw_state_bsa_nickname_valid = _BCM_DPP_MIM_VPN_IS_BSA_NICKNAME_VALID(mim_info_bsa_nickname);

        bsa_nickname = info->match_service_smac[2] | (info->match_service_smac[1] << 8) | ((info->match_service_smac[0] & 0xf0) << 12); /* bsa_nickname = bsa[47:44],bsa[39:24] */

        /* If the sw_state indicates the VPN is not valid but the application has the REPLACE flag set - return an error */
        if (!sw_state_is_valid && update) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_MIM_VPN_REPLACE flag is set for a VPN that doesn't exist")));
        }

        /* If the sw_state indicates the VPN is valid but the application hasn't set the REPLACE flag - return an error */
        if (sw_state_is_valid && !update) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("BCM_MIM_VPN_REPLACE flag is not set for a VPN that already exists (either in INGRESS or EGRESS)")));
        }

        /* allocate VSI */
        if (!update) {
            /* allocate a new VSI for MIM usage */
                rv = _bcm_dpp_vswitch_vsi_usage_alloc(unit,BCM_DPP_AM_FLAG_ALLOC_WITH_ID,_bcmDppVsiTypeMim,&info->vpn,&replaced);
                BCMDNX_IF_ERR_EXIT(rv);
        }
        vsi_id = info->vpn;

        /* If we create/replace a VPN in the ingress side (InLIF + ISEM entries) we need to check:
           1. For adding a new ingress VPN - The ISID is not used already to map a different VPN
           2. For replacing an existing ingress VPN - The ISID is indeed used already to map some VPN */
        if (is_ingress) {
            if (!update || ((sw_state_is_ingress == 0) && (sw_state_is_egress == 1))) {
                /* 
                * VSI is uniquely identified by I-SID at the ingress side
                * if we create the VPN at the ingress side check if this ISID is already used to map a VPN.
                */
                SOC_PPC_L2_LIF_ISID_KEY_clear(&isid_key);
                isid_key.isid_domain = 0; /* isid_domain is always 0 */
                isid_key.isid_id = info->lookup_id;
                if (bsa_valid) {
                    isid_key.bsa_nickname_valid = 1;
                    isid_key.bsa_nickname = bsa_nickname;
                }

                soc_sand_rv = soc_ppd_l2_lif_isid_get(soc_sand_dev_id, &isid_key, &tmp_lif, &tmp_vsi, &isid_info, &found);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                if (found) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("lookup_id is not unique to this VPN")));
                }
            } else {
                old_lookup_id = sw_state_isid;
                old_bsa_nickname_valid = sw_state_bsa_nickname_valid;
                old_bsa_nickname = sw_state_bsa_nickname;

                SOC_PPC_L2_LIF_ISID_KEY_clear(&isid_key);
                isid_key.isid_domain = 0; /* isid_domain is always 0 */
                isid_key.isid_id = old_lookup_id;
                isid_key.bsa_nickname_valid = old_bsa_nickname_valid;
                isid_key.bsa_nickname = old_bsa_nickname;

                soc_sand_rv = soc_ppd_l2_lif_isid_get(soc_sand_dev_id, &isid_key, &isid_local_lif_id, &vsi_id, &isid_info, &found);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                if (!found) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("lookup_id is not previous configured to this VPN")));
                }            
                isid_global_lif_id = isid_info.global_lif;
            }
        }

        /* set flooding for VSI */
        bcm_vlan_control_vlan_t_init(&control);

        control.forwarding_vlan = info->vpn;

        control.broadcast_group = info->broadcast_group;
        control.unknown_multicast_group = info->unknown_multicast_group;
        control.unknown_unicast_group = info->unknown_unicast_group;

        rv = bcm_petra_vlan_control_vlan_set(unit, info->vpn, control);
        BCMDNX_IF_ERR_EXIT(rv);

        /* 
         * map VSI to I-SID and I-SID to VSI 
         * set default forwarding as drop 
         * update B-tag TPID index 
         */

        /* allocate LIF */
/*
 * COVERITY
 *
 * The variable isid_lif_id is assigned in bcm_dpp_am_l2_ac_alloc
 */
/* coverity[uninit_use_in_call] */
        if(info->flags & BCM_MIM_VPN_SERVICE_ENCAP_WITH_ID) {
            l2_ac_alloc_flags |= BCM_DPP_AM_FLAG_ALLOC_WITH_ID;
            isid_global_lif_id = BCM_GPORT_TUNNEL_ID_GET(info->service_encap_id);
        }

        if (is_ingress) {
            if (!update || ((sw_state_is_ingress == 0) && (sw_state_is_egress == 1))) {
                if (SOC_IS_JERICHO(unit)) {

                    /* Set the inlif parameters */
                    sal_memset(&local_inlif_info, 0, sizeof(local_inlif_info));
                    local_inlif_info.local_lif_flags = DPP_IS_FLAG_SET(info->flags, BCM_MIM_VPN_SERVICE_ENCAP_INGRESS_WIDE) ? 
                                                                    BCM_DPP_AM_IN_LIF_FLAG_WIDE : BCM_DPP_AM_IN_LIF_FLAG_COMMON;
                    local_inlif_info.app_type = bcm_dpp_am_ingress_lif_app_ingress_isid;
                    local_inlif_info.counting_profile_id = BCM_DPP_AM_COUNTING_PROFILE_NONE;

                    /* Allocate the global and local lif */
                    rv = _bcm_dpp_gport_alloc_global_and_local_lif(unit, l2_ac_alloc_flags, &isid_global_lif_id, &local_inlif_info, NULL);
                    BCM_IF_ERROR_RETURN(rv);

                    /* Retrieve the allocated local lif */
                    isid_local_lif_id = local_inlif_info.base_lif_id;

                } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
                    rv = bcm_dpp_am_l2_ac_alloc(unit, _BCM_DPP_AM_L2_AC_TYPE_ISID, l2_ac_alloc_flags, (SOC_PPC_LIF_ID*)&isid_global_lif_id);
                    BCMDNX_IF_ERR_EXIT(rv);
                    isid_local_lif_id = isid_global_lif_id;
                }
            } else { /* update when ingress ISID already exists */
                rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS, isid_global_lif_id, (int*)&isid_local_lif_id);
                BCMDNX_IF_ERR_EXIT(rv);  
            }
        }

        BCM_GPORT_TUNNEL_ID_SET(info->service_encap_id, isid_global_lif_id);
        
        SOC_PPC_L2_LIF_ISID_INFO_clear(&isid_info);
        isid_info.service_type = SOC_PPC_L2_LIF_ISID_SERVICE_TYPE_MP;
        isid_info.learn_enable = TRUE;
        isid_info.orientation = SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB;
        isid_info.default_frwrd.default_frwd_type = SOC_PPC_L2_LIF_DFLT_FRWRD_SRC_VSI;
        isid_info.default_frwrd.default_forwarding.type = SOC_PPC_FRWRD_DECISION_TYPE_DROP;
        isid_info.global_lif = isid_global_lif_id;
        isid_info.is_extended = (info->flags & BCM_MIM_VPN_SERVICE_ENCAP_INGRESS_WIDE) ? TRUE:FALSE;

        if (!update || ((sw_state_is_ingress == 0) && (sw_state_is_egress == 1))) {
            SOC_PPC_L2_LIF_ISID_KEY_clear(&isid_key);
            isid_key.isid_domain = 0; /* isid_domain is always 0 */
            isid_key.isid_id = info->lookup_id;
            if (bsa_valid) {
                isid_key.bsa_nickname_valid = 1;
                isid_key.bsa_nickname = bsa_nickname;
            }
            soc_sand_rv = soc_ppd_l2_lif_isid_add(soc_sand_dev_id, vsi_id, &isid_key, isid_local_lif_id, &isid_info, is_ingress, is_egress, 0, &success);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        } else {
            if ((old_lookup_id != info->lookup_id) || (bsa_valid && (old_bsa_nickname != bsa_nickname))) {
                /* We must first remove the old ISID ISEM and InLIF entries, and then add the new ones */
                /*update the lookup_id */
                SOC_PPC_L2_LIF_ISID_KEY_clear(&isid_key);
                isid_key.isid_domain = 0; /* isid_domain is always 0 */
                isid_key.isid_id = old_lookup_id;
                isid_key.bsa_nickname = old_bsa_nickname;
                isid_key.bsa_nickname_valid = old_bsa_nickname_valid;

                /* remove the isid - Egress ISID shouldn't be removed as it was already overwritten by the soc_ppd_l2_lif_isid_add call */            
                soc_sand_rv = soc_ppd_l2_lif_isid_remove(soc_sand_dev_id, &isid_key, is_ingress, FALSE, vsi_id, FALSE, &isid_local_lif_id);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                                
                /*update the lookup_id */
                SOC_PPC_L2_LIF_ISID_KEY_clear(&isid_key);
                isid_key.isid_domain = 0; /* isid_domain is always 0 */
                isid_key.isid_id = info->lookup_id;
                if (bsa_valid) {
                    isid_key.bsa_nickname_valid = 1;
                    isid_key.bsa_nickname = bsa_nickname;
                }
                
                soc_sand_rv = soc_ppd_l2_lif_isid_add(soc_sand_dev_id, vsi_id, &isid_key, isid_local_lif_id, &isid_info, is_ingress, is_egress, 0, &success);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
        }                

        /* for 32k ISIDs, store the VSI->ISID mapping in the SW DB (will be set in HW when the vlan ports are attached to the VSI) */
        if (soc_property_get(unit, spn_MIM_NUM_VSIS, 0) == 32768) {
            BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.vsi.vsi_to_isid.set(soc_sand_dev_id, vsi_id, info->lookup_id));
        }

        if (is_ingress) {
            /*
             * update TPID index for B-tag TPID
             * this uses the I-SID Lif and therfore must be done after lif_isid_add
             */
            rv = _bcm_petra_mim_tpid_profile_update(unit, isid_local_lif_id, info->match_service_tpid);
            BCMDNX_IF_ERR_EXIT(rv);    


            /* save type and LIF-ID data - for restoration */
            bcm_mim_port_t_init(&mim_port);
            mim_port.mim_port_id = info->service_encap_id;
            rv = _bcm_dpp_in_lif_mim_match_add(unit, &mim_port, isid_local_lif_id);
            BCMDNX_IF_ERR_EXIT(rv);
        }


        /* Before we update the SW state we need to know what is the ISID in the VSI2ISID table in the EPNI. So we can tell if the ingress and egress pipes ISIDs are the same */
        soc_sand_rv = soc_ppd_l2_lif_vsi_to_isid(soc_sand_dev_id, vsi_id, &egress_isid);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        mim_info_bsa_nickname = (bsa_valid) ? _BCM_DPP_MIM_VPN_BSA_NICKNAME_SET(1, bsa_nickname) : 0;

        if (!update) {
            mim_info_flags = _BCM_DPP_MIM_VPN_FLAGS_SET((is_ingress && !is_egress), (is_egress && !is_ingress), 1, info->lookup_id);
        }
        else {
            if (is_ingress & is_egress) { /* Update the VPN to a new ISID that is the same in INGRESS and EGRESS */
                mim_info_flags = _BCM_DPP_MIM_VPN_FLAGS_SET(0, 0, 1, info->lookup_id);
            }
            else if (is_ingress) { /* Update the ISID in the INGRESS VPN only */
                if (sw_state_is_ingress && !sw_state_is_egress) { /* EGRESS is not defined for this VPN, only INGRESS is replaced now */
                    mim_info_flags = _BCM_DPP_MIM_VPN_FLAGS_SET(1, 0, 1, info->lookup_id); 
                }
                else { /* EGRESS is defined for this VPN, INGRESS might be defined or not. Need to update/add the ISID to the INGRESS and check if it's the same as in EGRESS */
                    mim_info_flags = _BCM_DPP_MIM_VPN_FLAGS_SET((info->lookup_id != egress_isid), (info->lookup_id != egress_isid), 1, info->lookup_id); 
                }
            }
            else if (is_egress) { /* Update the ISID in the EGRESS VPN only. ISID in the SW_STATE doesn't change */
                if (!sw_state_is_ingress && sw_state_is_egress) { /* INGRESS is not defined for this VPN, only EGRESS is replaced now */
                    mim_info_flags = _BCM_DPP_MIM_VPN_FLAGS_SET(0, 1, 1, sw_state_isid);
                }
                else { /* INGRESS is defined for this VPN, EGRESS might be defined or not. Need to check if the INGRESS ISID is the same as the new EGRESS one */
                    mim_info_flags = _BCM_DPP_MIM_VPN_FLAGS_SET((sw_state_isid != egress_isid), (sw_state_isid != egress_isid), 1, sw_state_isid); 
                }
                mim_info_bsa_nickname = _BCM_DPP_MIM_VPN_BSA_NICKNAME_SET(sw_state_bsa_nickname_valid, sw_state_bsa_nickname);
            }
        }

        BCMDNX_IF_ERR_EXIT(MIM_ACCESS.mim_vpn_flags.set(unit, info->vpn, mim_info_flags));
        BCMDNX_IF_ERR_EXIT(MIM_ACCESS.mim_vpn_bsa_nickname.set(unit, info->vpn, mim_info_bsa_nickname));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_mim_vpn_get(
    int unit, 
    bcm_mim_vpn_t vpn, 
    bcm_mim_vpn_config_t *info)
{
    int rv = BCM_E_NONE;
    bcm_mim_vpn_t vid, flood_vid;
    uint32 soc_sand_rv;
    uint32 soc_sand_dev_id;
    bcm_vlan_control_vlan_t control;
    SOC_PPC_SYS_VSI_ID vsi_id;
    SOC_SAND_PP_ISID isid;
    SOC_PPC_L2_LIF_ISID_KEY isid_key;
    SOC_PPC_LIF_ID local_lif_index;
    int global_lif_index;
    SOC_PPC_L2_LIF_ISID_INFO isid_info;
    uint8 found;
    uint8 is_wide_entry;
    uint8 sw_state_is_ingress, sw_state_is_egress, sw_state_bsa_nickname_valid;
    uint32 sw_state_isid, sw_state_bsa_nickname;
    uint8 is_ingress, is_egress;
    uint32 mim_info_flags, mim_info_bsa_nickname;


    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    SOC_PPC_L2_LIF_ISID_KEY_clear(&isid_key);        

    /* make sure MiM is enabled */
    MIM_INIT(unit);

    soc_sand_dev_id = (unit);

    /* Can't have both INGRESS_ONLY and EGRESS_ONLY flags */
    if ((info->flags & BCM_MIM_VPN_INGRESS_ONLY) && (info->flags & BCM_MIM_VPN_EGRESS_ONLY)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Can't have both BCM_MIM_VPN_INGRESS_ONLY and BCM_MIM_VPN_EGRESS_ONLY flags set")));
    }

    is_ingress = !(info->flags & BCM_MIM_VPN_EGRESS_ONLY);
    is_egress = !(info->flags & BCM_MIM_VPN_INGRESS_ONLY);

    if (_BCM_DPP_VLAN_IS_BVID(unit, vpn)) { /* B-VID */
        /* remove 0x1111 from B-VID */
        vid = _BCM_DPP_BVID_TO_VLAN(vpn);

        /* set the groups */
        flood_vid = vid + BCM_PETRA_MIM_BVID_MC_GROUP_BASE;
        _BCM_MULTICAST_GROUP_SET(info->broadcast_group, _BCM_MULTICAST_TYPE_MIM, flood_vid);
        _BCM_MULTICAST_GROUP_SET(info->unknown_multicast_group, _BCM_MULTICAST_TYPE_MIM, flood_vid);
        _BCM_MULTICAST_GROUP_SET(info->unknown_unicast_group, _BCM_MULTICAST_TYPE_MIM, flood_vid);
    }
    else { /* VSI */
        BCMDNX_IF_ERR_EXIT(MIM_ACCESS.mim_vpn_flags.get(unit, vpn, &mim_info_flags));
        BCMDNX_IF_ERR_EXIT(MIM_ACCESS.mim_vpn_bsa_nickname.get(unit, vpn, &mim_info_bsa_nickname));

        sw_state_isid = _BCM_DPP_MIM_VPN_ISID_GET(mim_info_flags);
        sw_state_is_ingress = _BCM_DPP_MIM_VPN_IS_INGRESS(mim_info_flags);
        sw_state_is_egress = _BCM_DPP_MIM_VPN_IS_EGRESS(mim_info_flags);
        sw_state_bsa_nickname = _BCM_DPP_MIM_VPN_BSA_NICKNAME_GET(mim_info_bsa_nickname);
        sw_state_bsa_nickname_valid = _BCM_DPP_MIM_VPN_IS_BSA_NICKNAME_VALID(mim_info_bsa_nickname);

        /* If the VPN has different ISIDs for INGRESS and EGRESS sides, the application must tell us which ISID it wants to get through the flags */
        if ((sw_state_is_ingress && sw_state_is_egress) && is_ingress && is_egress) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VPN has different ISIDs for INGRESS and EGRESS sides. Must specify in flags INGRESS_ONLY or EGRESS_ONLY")));
        }

        /* If the VPN has ISID defined only for INGRESS side, and the application requested EGRESS - return an error */
        if (sw_state_is_ingress && !sw_state_is_egress && is_egress) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VPN has ISID defined only for INGRESS. Must specify in flags INGRESS_ONLY")));
        }

        /* If the VPN has ISID defined only for EGRESS side, and the application requested INGRESS - return an error */
        if (!sw_state_is_ingress && sw_state_is_egress && is_ingress) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VPN has ISID defined only for EGRESS. Must specify in flags EGRESS_ONLY")));
        }

        /* get flooding for VSI */
        rv = bcm_petra_vlan_control_vlan_get(unit, vpn, &control);
        BCMDNX_IF_ERR_EXIT(rv);

        info->broadcast_group = control.broadcast_group;
        info->unknown_multicast_group = control.unknown_multicast_group;
        info->unknown_unicast_group = control.unknown_unicast_group;

        /* get the I-SID from the VSI */
        vsi_id = vpn;
        if (is_egress) {
            soc_sand_rv = soc_ppd_l2_lif_vsi_to_isid(soc_sand_dev_id, vsi_id, &isid);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
        else { /*Ingress side ISID */
            isid = sw_state_isid;
        }

        info->lookup_id = isid;
        if (is_ingress) {
            isid_key.isid_domain = 0; /* isid_domain is always 0 */
            isid_key.isid_id = isid;
            isid_key.bsa_nickname_valid = sw_state_bsa_nickname_valid;
            isid_key.bsa_nickname = sw_state_bsa_nickname;

            soc_sand_rv = soc_ppd_l2_lif_isid_get(soc_sand_dev_id, &isid_key, &local_lif_index, &vsi_id, &isid_info, &found);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            rv = _bcm_dpp_global_lif_mapping_local_to_global_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS, local_lif_index, &global_lif_index);
            BCMDNX_IF_ERR_EXIT(rv);

            /* Get wide entry info */
            if (SOC_IS_JERICHO(unit)) {

                rv = MBCM_PP_DRIVER_CALL(unit,mbcm_pp_lif_is_wide_entry,(unit, local_lif_index, TRUE, &is_wide_entry, NULL));
                BCMDNX_IF_ERR_EXIT(rv);

                info->flags |= (is_wide_entry) ? BCM_MIM_VPN_SERVICE_ENCAP_INGRESS_WIDE : 0;
            }

            BCM_GPORT_TUNNEL_ID_SET(info->service_encap_id, global_lif_index);
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_mim_vpn_destroy(
    int unit, 
    bcm_mim_vpn_t vpn)
{
    int rv = BCM_E_NONE;
    uint32 soc_sand_rv;
    uint32 soc_sand_dev_id;
    SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO profile_info;
    SOC_PPC_L2_LIF_ISID_KEY isid_key;
    bcm_pbmp_t pbmp_sum, pbmp,ubmp;
    SOC_PPC_LIF_ID isid_local_lif_id;
    int isid_global_lif_id;
    bcm_if_t service_encap_id; 
    uint8 sw_state_is_ingress, sw_state_is_egress, sw_state_bsa_nickname_valid;
    uint32 sw_state_isid, sw_state_bsa_nickname;
    uint32 mim_info_flags, mim_info_bsa_nickname;
    SOC_PPC_L2_LIF_ISID_INFO isid_info;
    uint8 found = 0;
    SOC_PPC_SYS_VSI_ID tmp_vsi;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    /* make sure MiM is enabled */
    MIM_INIT(unit);

    soc_sand_dev_id = (unit);

    /* reset the flooding */
    if (_BCM_DPP_VLAN_IS_BVID(unit, vpn)) { /* B-VID */

        /* set the trap profile to default settings */
        SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO_clear(&profile_info);

        profile_info.bitmap_mask |= SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DEST;
        profile_info.dest_info.frwrd_dest.type = SOC_PPC_FRWRD_DECISION_TYPE_UC_FLOW;    
        profile_info.dest_info.add_vsi = FALSE;
        profile_info.dest_info.frwrd_dest.dest_id = 0;

    }
    else { /* VSI */
        BCMDNX_IF_ERR_EXIT(MIM_ACCESS.mim_vpn_flags.get(unit, vpn, &mim_info_flags));
        BCMDNX_IF_ERR_EXIT(MIM_ACCESS.mim_vpn_bsa_nickname.get(unit, vpn, &mim_info_bsa_nickname));
        sw_state_isid = _BCM_DPP_MIM_VPN_ISID_GET(mim_info_flags);
        sw_state_is_ingress = _BCM_DPP_MIM_VPN_IS_INGRESS(mim_info_flags);
        sw_state_is_egress = _BCM_DPP_MIM_VPN_IS_EGRESS(mim_info_flags);
        sw_state_bsa_nickname = _BCM_DPP_MIM_VPN_BSA_NICKNAME_GET(mim_info_bsa_nickname);
        sw_state_bsa_nickname_valid = _BCM_DPP_MIM_VPN_IS_BSA_NICKNAME_VALID(mim_info_bsa_nickname);

        /* BCM_DPP_VLAN_CHK_ID(unit, vpn);*/

        /* Validate VSI does not exist */
        rv = bcm_dpp_am_l2_vpn_vsi_is_alloced(unit, vpn);
        if (rv == BCM_E_NOT_FOUND) {
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VSI does not exists")));
        }
        if (vpn <= BCM_VLAN_MAX) {
            /* Clear vlan membership */
            BCM_PBMP_CLEAR(pbmp);
            BCM_PBMP_CLEAR(ubmp);
            BCM_PBMP_CLEAR(pbmp_sum);

            rv = bcm_petra_vlan_port_get(unit, vpn, &pbmp, &ubmp);
            BCMDNX_IF_ERR_EXIT(rv);
    
            BCM_PBMP_OR(pbmp_sum, ubmp);
            BCM_PBMP_OR(pbmp_sum, pbmp);

            rv = bcm_petra_vlan_port_remove(unit, vpn, pbmp);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /* If ISID is defined on the INGRESS side for this VPN - delete it. Use the ISID stored in the sw_state */
        SOC_PPC_L2_LIF_ISID_KEY_clear(&isid_key);
        isid_key.isid_domain = 0; /* isid_domain is always 0 */
        isid_key.isid_id = sw_state_isid;
        isid_key.bsa_nickname_valid = sw_state_bsa_nickname_valid;
        isid_key.bsa_nickname = sw_state_bsa_nickname;

        /* 
         * If ISID was defined for the VPN on the INGRESS side (either it's equal to the egress side or not - we don't care) 
         * We'll delete the ISEM, InLIF and EPNI_ISID table by calling soc_ppd_l2_lif_isid_remove 
         */ 
        if ((sw_state_is_ingress == sw_state_is_egress) || (sw_state_is_ingress)) {
            soc_sand_rv = soc_ppd_l2_lif_isid_get(soc_sand_dev_id, &isid_key, &isid_local_lif_id, &tmp_vsi, &isid_info, &found);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            rv = _bcm_dpp_global_lif_mapping_local_to_global_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS, isid_local_lif_id, &isid_global_lif_id);
            BCMDNX_IF_ERR_EXIT(rv);


            if(SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED(unit)) {
                BCM_GPORT_TUNNEL_ID_SET(service_encap_id, isid_global_lif_id);
                rv = bcm_petra_port_tpid_delete_all(unit, service_encap_id);
                BCMDNX_IF_ERR_EXIT(rv);
            }
            else {
                /* isid_lif_id should no longer point at TPID index for B-tag TPID */
                rv = _bcm_petra_mim_tpid_profile_update(unit, isid_local_lif_id, 0);
                BCMDNX_IF_ERR_EXIT(rv);  
            }

            soc_sand_rv = soc_ppd_l2_lif_isid_remove(soc_sand_dev_id, &isid_key, TRUE, TRUE, vpn, FALSE, &isid_local_lif_id);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            /* Deallocate isid_lif_id */
            if (SOC_IS_JERICHO(unit)) {
                rv = _bcm_dpp_gport_delete_global_and_local_lif(unit, isid_global_lif_id, isid_local_lif_id, _BCM_GPORT_ENCAP_ID_LIF_INVALID);
                BCMDNX_IF_ERR_EXIT(rv);
            } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
                rv = bcm_dpp_am_l2_ac_dealloc(unit, 0, isid_local_lif_id);
                BCMDNX_IF_ERR_EXIT(rv);
            }

            /* remove mim lif match SW */
            rv = _bcm_dpp_local_lif_sw_resources_delete(unit, isid_local_lif_id, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS);
            BCMDNX_IF_ERR_EXIT(rv); 
        }

        /* Else ISID defined only on the egress side of the VPN - remove the ISID in the egress only, using the VSI provided by the application */
        else {
            soc_sand_rv = soc_ppd_l2_lif_isid_remove(soc_sand_dev_id, &isid_key, FALSE, TRUE, vpn, FALSE, &isid_local_lif_id);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }

        rv =  _bcm_dpp_vswitch_vsi_usage_dealloc(unit, _bcmDppVsiTypeMim, vpn);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Update the SW state */
        mim_info_flags = _BCM_DPP_MIM_VPN_FLAGS_SET(0, 0, 0, 0);
        BCMDNX_IF_ERR_EXIT(MIM_ACCESS.mim_vpn_flags.set(unit, vpn, mim_info_flags));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_mim_port_add
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_petra_mim_port_add(
    int unit,
    bcm_mim_vpn_t vpn,
    bcm_mim_port_t * mim_port)
{
    int rv, soc_sand_dev_id;
    uint32 soc_sand_rv;
    SOC_SAND_PP_MAC_ADDRESS src_bmac;
    SOC_PPC_FEC_ID fec_id /* work */, protect_fec_id;
    uint32 fec_flags = 0, eep_flags = 0, lif_flags = 0;
    int value, sec_fec = 0, is_protected, update = 0;
    uint32 nof_entries, next_eep[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX];
    SOC_PPC_EG_ENCAP_LL_INFO ll_encap_info, *ll_encap_info_ptr;
    SOC_PPC_EG_ENCAP_OVERLAY_ARP_ENCAP_INFO ll_overlay_encap_info;
    SOC_PPC_EG_ENCAP_ENTRY_INFO *encap_entry_info = NULL;
    SOC_PPC_BMACT_ENTRY_KEY bmac_key;
    SOC_PPC_BMACT_ENTRY_INFO bmact_entry_info;
    uint8 found;
    SOC_SAND_SUCCESS_FAILURE success;
    _BCM_GPORT_PHY_PORT_INFO phy_port;
    _BCM_GPORT_PHY_PORT_INFO remove_phy_port;
    uint32 mac_addr_arr[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S];
    SOC_PPC_LIF_ID local_in_lif_id;
    int global_in_lif_id, global_out_lif_id = 0, local_out_lif_id;
    SOC_PPC_L2_LIF_ISID_KEY isid_key;
    SOC_PPC_L2_LIF_ISID_INFO isid_info;
    SOC_PPC_SYS_VSI_ID vsi_index;
    SOC_SAND_PP_VLAN_ID out_vlan = mim_port->egress_tunnel_vlan;    
    uint32  soc_ppd_port;
    int local_port, core;
    _bcm_dpp_gport_info_t gport_info;
    _bcm_lif_type_e
        usage;
    bcm_dpp_am_local_inlif_info_t local_inlif_info;
    bcm_dpp_am_local_out_lif_info_t local_out_lif_info;
    int spb_is_supported = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mim_spb_enable", 0);

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    MIM_INIT(unit);

    /* check invalid flags and match criteria */

    soc_sand_dev_id = (unit);

    /* network_group_id validity check */
    if (spb_is_supported) {
        if (((soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1) == 0 ||
              soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1) == 3) && (mim_port->network_group_id > 1)) ||
            (mim_port->network_group_id > 3)) {
           BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG(
               "network_group_id valid values are 0-3 for split_horizon_forwarding_groups_mode=1/2 and 0-1 for split_horizon_forwarding_groups_mode=0/3")));
        }
    } else {
        if (((soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1) != 2) && (mim_port->network_group_id > 1)) ||
            (mim_port->network_group_id > 3)) {
           BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG(
               "network_group_id valid values are 0-1 for split_horizon_forwarding_groups_mode=0/1/3 and 0-3 for split_horizon_forwarding_groups_mode=2")));
        }
    }

    /* check that port is PBP enabled */
    if (!BCM_GPORT_IS_TRUNK(mim_port->port)) {
        rv = bcm_petra_port_control_get(
            unit,
            mim_port->port,
            bcmPortControlMacInMac,
            &value);
        BCMDNX_IF_ERR_EXIT(rv);
        if (!value) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("port is not set as bcmPortControlMacInMac")));
        }
    }

    rv = _bcm_dpp_gport_to_phy_port(unit, mim_port->port, 0, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rv);

    is_protected = _BCM_PPD_IS_VALID_FAILOVER_ID(mim_port->failover_id);

    fec_id = BCM_GPORT_MIM_PORT_ID_GET(mim_port->mim_port_id);

    if (is_protected && mim_port->failover_gport_id != BCM_GPORT_TYPE_NONE) {

        /* if this is working fec, failover-gport has to be protected otherwise return error */
        rv = _bcm_dpp_gport_is_protected(unit, mim_port->failover_gport_id, &is_protected);
        BCMDNX_IF_ERR_EXIT(rv);
        if (!is_protected) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("failover_id is valid but there is no failover_gport_id")));
        }

        /* this gport is second gport in protection */
        protect_fec_id =  BCM_GPORT_MIM_PORT_ID_GET(mim_port->failover_gport_id);
        fec_id = _BCM_PPD_GPORT_PROTECT_TO_WORK_FEC(protect_fec_id);
        sec_fec = 1;
    }

    if (mim_port->flags & BCM_MIM_PORT_REPLACE){
        if (mim_port->flags & BCM_MIM_PORT_WITH_ID) {

            /* fec is already allocated - get info */
            if (!BCM_GPORT_IS_MIM_PORT((mim_port->mim_port_id))) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("gport mim_port_id is not MIM")));
            }
        }
        else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Cannot replace port when port ID is not supplied")));
        }
        update = 1;
    }
    else {
        /* allocate FEC */
        if (mim_port->flags & BCM_MIM_PORT_WITH_ID) {
            fec_flags |= BCM_DPP_AM_FLAG_ALLOC_WITH_ID;                    
        }
        if (is_protected) {
            if (!sec_fec) {
                /* first fec in protection - allocate 2 fec entries */
                rv = bcm_dpp_am_fec_alloc(unit, fec_flags, 0, 2, &fec_id);
                BCMDNX_IF_ERR_EXIT(rv);
            }
            /* if sec_fec then work & protect fec are already allocated */
        }
        else {
            _bcm_dpp_am_fec_alloc_usage usage;
            usage = (mim_port->flags & BCM_MIM_PORT_CASCADED) ? _BCM_DPP_AM_FEC_ALLOC_USAGE_CASCADED : _BCM_DPP_AM_FEC_ALLOC_USAGE_STANDARD;
            /* allocate 1 fec entry */
            rv = bcm_dpp_am_fec_alloc(unit, fec_flags, usage, 1, &fec_id);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /* return FEC to user */
        BCM_GPORT_MIM_PORT_ID_SET(mim_port->mim_port_id, fec_id);
    }

    if (!update) {
        /* allocate EEP and return to user */
        if (mim_port->flags & BCM_MIM_PORT_ENCAP_WITH_ID) {
            eep_flags |= BCM_DPP_AM_FLAG_ALLOC_WITH_ID;
            global_out_lif_id = mim_port->encap_id;
        }
        if (SOC_IS_JERICHO(unit)) {
            sal_memset(&local_out_lif_info, 0, sizeof(local_out_lif_info));
            local_out_lif_info.app_alloc_info.pool_id = dpp_am_res_eep_global;
            local_out_lif_info.app_alloc_info.application_type = 0;
            local_out_lif_info.counting_profile_id = BCM_DPP_AM_COUNTING_PROFILE_NONE;

            if (mim_port->flags & BCM_MIM_PORT_ENCAP_OVERLAY) {
                local_out_lif_info.local_lif_flags |= BCM_DPP_AM_OUT_LIF_FLAG_WIDE;
                local_out_lif_info.app_alloc_info.application_type = bcm_dpp_am_egress_encap_app_linker_layer;
            }

            /* Allocate the global and local lif */
            rv = _bcm_dpp_gport_alloc_global_and_local_lif(unit, eep_flags, &global_out_lif_id, NULL, &local_out_lif_info);
            BCM_IF_ERROR_RETURN(rv);

            /* Retrieve the allocated local lif */
            local_out_lif_id = local_out_lif_info.base_lif_id;


        } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
            if (SOC_DPP_CONFIG(unit)->pp.test2 || SOC_DPP_CONFIG(unit)->pp.test1) {
                rv = bcm_dpp_am_l3_eep_alloc(unit, _BCM_DPP_AM_L3_EEP_TYPE_ROO_LINKER_LAYER, eep_flags, &global_out_lif_id);
                BCMDNX_IF_ERR_EXIT(rv);
            } else {
                rv = bcm_dpp_am_l3_eep_alloc(unit, _BCM_DPP_AM_L3_EEP_TYPE_DEFAULT, eep_flags, &global_out_lif_id);
                BCMDNX_IF_ERR_EXIT(rv); 
            }     
            local_out_lif_id = global_out_lif_id;
        }

        mim_port->encap_id = global_out_lif_id;
    }

    if ((_BCM_DPP_GPORT_INFO_IS_LOCAL_PORT(gport_info)) && (!(mim_port->flags & BCM_MIM_PORT_NO_LEARN_ENTRY))) { /* Only Local settings MAC Tunnel identification and no flag*/
        BCM_PBMP_ITER(gport_info.pbmp_local_ports, local_port) {

            BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, local_port, &soc_ppd_port, &core)));

            /* Translate parameters to SOC_SAND parameters */
            rv = _bcm_petra_mac_to_sand_mac(mim_port->match_tunnel_srcmac, &src_bmac);
            BCMDNX_IF_ERR_EXIT(rv);

            /* add B-MACT entry, mapping <B-VID, B-SA> --> FEC. Only for LEARN. Forwarding is done in bcm_petra_l2_addr_add */
            SOC_PPC_BMACT_ENTRY_KEY_clear(&bmac_key);

            bmac_key.b_mac_addr = src_bmac;
            bmac_key.b_vid = _BCM_DPP_BVID_TO_VLAN(mim_port->match_tunnel_vlan);

            bmac_key.flags = SOC_PPC_BMACT_ENTRY_TYPE_LEARN;
            bmac_key.local_port_ndx = soc_ppd_port;
            if (SOC_IS_JERICHO(unit) && !SOC_IS_QAX(unit)) {
                bmac_key.core = core;
            }

            if (SOC_DPP_CONFIG(unit)->pp.test2) {
                bmac_key.b_vid=vpn;
                /*
                 * In EoE, port is not part of the key, we reset the port so it will pas the verify test
                 */
                bmac_key.local_port_ndx =0;
            }
            

            soc_sand_rv = soc_ppd_frwrd_bmact_entry_get(soc_sand_dev_id, &bmac_key, &bmact_entry_info, &found);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            bmact_entry_info.i_sid_domain = 0;
            bmact_entry_info.sa_learn_fec_id = fec_id;   

            soc_sand_rv = soc_ppd_frwrd_bmact_entry_add(soc_sand_dev_id, &bmac_key, &bmact_entry_info, &success);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            SOC_SAND_IF_FAIL_RETURN(success);
        }
    }

    /* add FEC entry that points at EEP */
    rv = _bcm_dpp_l2_gport_fill_fec(unit, mim_port, BCM_GPORT_MIM_PORT, fec_id, NULL);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Update FEC->GPORT SW DB */
    if (fec_id != -1) {
        rv = bcm_dpp_gport_mgmt_sw_resources_fec_to_gport_set(unit, fec_id, mim_port->mim_port_id);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /*Create Overlay LL */
    if (mim_port->flags & BCM_MIM_PORT_ENCAP_OVERLAY) {
        SOC_PPC_EG_ENCAP_ETHER_TYPE_INDEX_INFO          eth_type_index_entry ;
        _bcm_dpp_gport_sw_resources gport_sw_resources;
        uint32 next_eep[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX];
        SOC_PPC_EG_ENCAP_ENTRY_INFO encap_entry_info[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX];
        uint32 nof_entries;
        int template_is_new;
        int old_eth_type_index_ndx = 0;
        int new_eth_type_index_ndx = 0;
        int template_is_last = 0;
        int spb_da_bit_0 = 0;
        int spb_da_bit_1 = 0;

        if (!(SOC_IS_JERICHO_B0(unit) || SOC_IS_QMX_B0(unit))) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Flag: BCM_MIM_PORT_ENCAP_OVERLAY, MIM LL overlay is only available for Jericho_B0")));
        }

        /* Following l2_egress_create */
        /* 2. convert overlay arp infos, from bcm to soc */
        SOC_PPC_EG_ENCAP_OVERLAY_ARP_ENCAP_INFO_clear(&ll_overlay_encap_info);

        /* dest mac */
        rv = _bcm_petra_mac_to_sand_mac(
           mim_port->egress_tunnel_dstmac,
           &ll_overlay_encap_info.dest_mac);
        BCMDNX_IF_ERR_EXIT(rv);
        /* src mac */
        rv = _bcm_petra_mac_to_sand_mac(
           mim_port->egress_tunnel_srcmac,
           &ll_overlay_encap_info.src_mac);
        BCMDNX_IF_ERR_EXIT(rv);

        /* outer-vid and PCP-DEI are both encode in outer-vlan field: encoding: {PCP DEI [15:12] VID[11:0]} */
        ll_overlay_encap_info.out_vid = VLAN_CTRL_ID(mim_port->egress_tunnel_vlan);
        /* PCP DEI
        * extract outer PCP DEI from outer-vlan */
        ll_overlay_encap_info.pcp_dei = (VLAN_CTRL_PRIO(mim_port->egress_tunnel_vlan) << 1) | VLAN_CTRL_CFI(mim_port->egress_tunnel_vlan);

        soc_sand_os_memset(&eth_type_index_entry, 0x0, sizeof(SOC_PPC_EG_ENCAP_ETHER_TYPE_INDEX_INFO));

        /* inner_vid in case of MIM is irrelevant, the I-TAG is handled by the HW */
        ll_overlay_encap_info.inner_vid = 0;
        ll_overlay_encap_info.inner_pcp_dei = 0;

        /*PCP DEI profile */
        ll_overlay_encap_info.pcp_dei_profile = mim_port->int_pri;

        /* number of tags in MIM ROO LL overlay is always 2 */
        ll_overlay_encap_info.nof_tags = 2;

        /* Edit the dest_mac in entry for spb split horizon mode 1 -
         * Replace B-DA[47], B-DA[43] with the split horizon orientation bits */
        if (spb_is_supported && (soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1) == 1))
        {
            spb_da_bit_0 = (ll_overlay_encap_info.dest_mac.address[5] >> 3) & 1;
            spb_da_bit_1 = (ll_overlay_encap_info.dest_mac.address[5] >> 7) & 1;
            switch (mim_port->network_group_id) {
            case 0:
                ll_overlay_encap_info.dest_mac.address[5] &= ~(1 << 3);
                ll_overlay_encap_info.dest_mac.address[5] &= ~(1 << 7);
                break;
            case 1:
                ll_overlay_encap_info.dest_mac.address[5] |= 1 << 3;
                ll_overlay_encap_info.dest_mac.address[5] &= ~(1 << 7);
                break;
            case 2:
                ll_overlay_encap_info.dest_mac.address[5] &= ~(1 << 3);
                ll_overlay_encap_info.dest_mac.address[5] |= 1 << 7;
                break;
            case 3:
                ll_overlay_encap_info.dest_mac.address[5] |= 1 << 3;
                ll_overlay_encap_info.dest_mac.address[5] |= 1 << 7;
                break;
            }
        }

        /* allocate entry for CfgEtherTypeIndex
           (additional table for eedb of type roo link layer format) */
        /* HW stamps 0x88e7 in case of MIM */
        eth_type_index_entry.ether_type = 0;

        eth_type_index_entry.tpid_0 = mim_port->egress_service_tpid;
        eth_type_index_entry.tpid_1 = 0;

        /* For split horizon mode 1 in spb - replace MSB of tpid in EtherTypeIndex entry with (2'b10, B-DA[47], B-DA[43]) */
        if (spb_is_supported && (soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1) == 1)) {
            eth_type_index_entry.tpid_0 &= 0x0fff;
            eth_type_index_entry.tpid_0 |= (spb_da_bit_0 << 12 | spb_da_bit_1 << 13 | 1 << 15);
        }

        /* (sw) allocate entry for CfgEtherTypeIndex */

        /* replace mode: get the current index for CfgEtherTypeIndex from eedb entry
           Note: outlif != 0 since we've tested that replace and with_id flags are both required */
        if (update) {
            /* get the current index for CfgEtherTypeIndex from eedb entry */
            rv = soc_ppd_eg_encap_entry_get(
               unit, SOC_PPC_EG_ENCAP_EEP_TYPE_ROO_LL, local_out_lif_id, 0, encap_entry_info, next_eep, &nof_entries);
            BCM_SAND_IF_ERR_EXIT(rv);
            old_eth_type_index_ndx = encap_entry_info[0].entry_val.overlay_arp_encap_info.eth_type_index;

            /* clear sw allocation for current eth_type_index */
            rv = bcm_dpp_am_template_eedb_roo_ll_format_eth_type_index_free(unit, old_eth_type_index_ndx, &template_is_last);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /* sw allocation for new eth_type_index */
        rv = bcm_dpp_am_template_eedb_roo_ll_format_eth_type_index_alloc(
           unit, 0, &eth_type_index_entry, &template_is_new, &new_eth_type_index_ndx);
        BCMDNX_IF_ERR_EXIT(rv);
        ll_overlay_encap_info.eth_type_index = new_eth_type_index_ndx;

        /* in case we have removed last entry and add a different new entry, hw remove of the eth_type_index entry    */
        if (update && template_is_last && (old_eth_type_index_ndx != new_eth_type_index_ndx)) {
            rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_eg_encap_ether_type_index_clear,
                      (unit, old_eth_type_index_ndx)));
            BCMDNX_IF_ERR_EXIT(rv);
        }

        if (template_is_new) {
            /* hw allocation of eth_type_index entry */
            rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_eg_encap_ether_type_index_set,
                                      (unit, ll_overlay_encap_info.eth_type_index, &eth_type_index_entry)));
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /* 3. build and insert overlay arp eedb entry */

        /* 3. insert */

        /* build eedb entry, and insert the entry in allocated place */
        rv = soc_ppd_eg_encap_overlay_arp_data_entry_add(unit, local_out_lif_id, &ll_overlay_encap_info);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Update SW DB */
        rv = _bcm_dpp_local_lif_to_sw_resources(unit, -1, local_out_lif_id, _BCM_DPP_GPORT_SW_RESOURCES_EGRESS, &gport_sw_resources);
        BCMDNX_IF_ERR_EXIT(rv);

        gport_sw_resources.out_lif_sw_resources.lif_type = _bcmDppLifTypeOverlayLinkLayer;
        gport_sw_resources.out_lif_sw_resources.flags |= 0;

        rv = _bcm_dpp_local_lif_sw_resources_set(unit, -1, local_out_lif_id, _BCM_DPP_GPORT_SW_RESOURCES_EGRESS, &gport_sw_resources);
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        if (update) {

            /* Check that mim_port->encap_id is an the global lif of a MIM OutLIF (in SW DB) */
            global_out_lif_id = mim_port->encap_id;
            rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, global_out_lif_id, &local_out_lif_id);
            BCMDNX_IF_ERR_EXIT(rv);
        BCMDNX_IF_ERR_EXIT( _bcm_dpp_local_lif_sw_resources_lif_usage_get(unit, -1, local_out_lif_id, NULL, &usage) );
            if (usage != _bcmDppLifTypeMim) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Can't replace OutLIF to MIM, because it's a different type (%d)"),usage));
            }

            BCMDNX_ALLOC(encap_entry_info, sizeof(SOC_PPC_EG_ENCAP_ENTRY_INFO) * SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX, "bcm_petra_mim_port_add.encap_entry_info");
            if (encap_entry_info == NULL) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failure")));
            }
            /* get existing EEP entry */
            soc_sand_rv = soc_ppd_eg_encap_entry_get(soc_sand_dev_id, SOC_PPC_EG_ENCAP_EEP_TYPE_LL, local_out_lif_id, 1, encap_entry_info,
                                             next_eep, &nof_entries);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            if (nof_entries <= 0 || encap_entry_info[0].entry_type != SOC_PPC_EG_ENCAP_ENTRY_TYPE_LL_ENCAP) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("encap_id does not exist or is of the wrong type")));
            }
            ll_encap_info_ptr = &(encap_entry_info[0].entry_val.ll_info);
        }
        else {
            /* add EEP entry */
            SOC_PPC_EG_ENCAP_LL_INFO_clear(&ll_encap_info);
            ll_encap_info_ptr = &ll_encap_info;
        }

        rv = _bcm_petra_mac_to_sand_mac(mim_port->egress_tunnel_dstmac, &(ll_encap_info_ptr->dest_mac));
        BCMDNX_IF_ERR_EXIT(rv);

        if (_BCM_DPP_VLAN_IS_BVID(unit, out_vlan)) { /* egress B-VID */
            /* remove 0x1111 from egress B-VID */
            out_vlan = _BCM_DPP_BVID_TO_VLAN(out_vlan);
        }
        ll_encap_info_ptr->out_vid = out_vlan;
        ll_encap_info_ptr->out_vid_valid = (mim_port->egress_tunnel_vlan == BCM_VLAN_INVALID ? FALSE : TRUE);

        if (SOC_IS_JERICHO(unit) && ((soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1) >= 2) || spb_is_supported)) {
                    soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_set,
                                     (unit, SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_APP_ORIENTATION,
                                      mim_port->flags & BCM_MIM_PORT_TYPE_PEER ? 0 : mim_port->network_group_id,
                                      &(ll_encap_info_ptr->outlif_profile)));
                    BCMDNX_IF_ERR_EXIT(soc_sand_rv);
        } else if (SOC_IS_JERICHO(unit) && (soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1) == 1)) {
            soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_set,
                                     (unit, SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_APP_ORIENTATION,
                                      mim_port->flags & BCM_MIM_PORT_TYPE_PEER ? 0 : 1,
                                      &(ll_encap_info_ptr->outlif_profile)));
            BCMDNX_IF_ERR_EXIT(soc_sand_rv);
        }

        /* update is_l2_lif in outlif profile.
           Used in ROO application to build native LL in case a L2_LIF is in the encapsulation stack */
        if (SOC_IS_JERICHO_PLUS(unit)) {
            /* set entry to be L2LIF */
            soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_set,
                              (unit, SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_APP_ROO_IS_L2_LIF, 1,
                                  &(ll_encap_info_ptr->outlif_profile)));
            BCMDNX_IF_ERR_EXIT(soc_sand_rv);
        }

       /* in Jericho, set MIM_ENCAP bit of outlif profile to 0x1 */
       if (SOC_IS_JERICHO(unit) && (SOC_DPP_CONFIG(unit)->pp.test2 || SOC_DPP_CONFIG(unit)->pp.test1) ) {
            /* set entry to be MIM ENCAP */
            soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_set,
                              (unit, SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_MIM_ENCAP, 1,
                                  &(ll_encap_info_ptr->outlif_profile)));
            BCMDNX_IF_ERR_EXIT(soc_sand_rv);
        } else if(SOC_DPP_CONFIG(unit)->pp.test2 || SOC_DPP_CONFIG(unit)->pp.test1){ /* arad+ and below, next entry is data entry */
            ll_encap_info_ptr->out_ac_lsb = ((uint8)SOC_DPP_CONFIG(unit)->pp.mim_local_out_lif);
            ll_encap_info_ptr->out_ac_valid = 1;
        }

        if (SOC_IS_JERICHO_PLUS(unit) && !SOC_IS_QAX_A0(unit)) {
            ll_encap_info_ptr->native_ll = 0;
        }

        soc_sand_rv = soc_ppd_eg_encap_ll_entry_add(soc_sand_dev_id, local_out_lif_id, ll_encap_info_ptr);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* Add outlif to SW DB */
        rv = _bcm_dpp_out_lif_mim_match_add(unit,mim_port,local_out_lif_id);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    if (vpn == BCM_PETRA_MIM_VPN_INVALID(unit)) { /* P2P */

        /* I-SID is the identifier of the MIM port
           make sure there's no other port with the same I-SID */
        SOC_PPC_L2_LIF_ISID_KEY_clear(&isid_key);

        isid_key.isid_domain = 0;
        isid_key.isid_id = mim_port->egress_tunnel_service;

        soc_sand_rv = soc_ppd_l2_lif_isid_get(soc_sand_dev_id, &isid_key, &local_in_lif_id, &vsi_index, &isid_info, &found);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        if (found && !update) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("egress_tunnel_service must be unique to this gport")));
        }

        if (!update) {
            if(mim_port->flags & BCM_MIM_PORT_SERVICE_ENCAP_WITH_ID) {
                lif_flags |= BCM_DPP_AM_FLAG_ALLOC_WITH_ID;
                global_in_lif_id = BCM_GPORT_TUNNEL_ID_GET(mim_port->service_encap_id);
            }
            /* allocate LIF */
            if (SOC_IS_JERICHO(unit)) {

                /* Allocate the local and global lifs */
                sal_memset(&local_inlif_info, 0, sizeof(local_inlif_info));

                local_inlif_info.local_lif_flags = DPP_IS_FLAG_SET(mim_port->flags, BCM_MIM_PORT_SERVICE_ENCAP_INGERSS_WIDE) ? 
                                                                BCM_DPP_AM_IN_LIF_FLAG_WIDE : BCM_DPP_AM_IN_LIF_FLAG_COMMON;
                local_inlif_info.app_type = bcm_dpp_am_ingress_lif_app_ingress_isid;
                local_inlif_info.counting_profile_id = BCM_DPP_AM_COUNTING_PROFILE_NONE;

                rv = _bcm_dpp_gport_alloc_global_and_local_lif(unit, lif_flags, &global_in_lif_id, &local_inlif_info, NULL);
                BCM_IF_ERROR_RETURN(rv);

                /* Retrieve the allocated local in lif id */
                local_in_lif_id = local_inlif_info.base_lif_id;

            } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
                rv = bcm_dpp_am_l2_ac_alloc(unit, _BCM_DPP_AM_L2_AC_TYPE_ISID, lif_flags, (SOC_PPC_LIF_ID*)&global_in_lif_id);
                BCMDNX_IF_ERR_EXIT(rv);
                local_in_lif_id = global_in_lif_id;
            }

            BCM_GPORT_TUNNEL_ID_SET(mim_port->service_encap_id, global_in_lif_id);

            /* add I-SID entry */
            SOC_PPC_L2_LIF_ISID_INFO_clear(&isid_info);

            /* in PB, LIF destination cannot be drop so use trap-code */
            /* this forwarding is temporary and will be replaced when the port is cross-connected to another port */
            isid_info.default_frwrd.default_forwarding.type = SOC_PPC_FRWRD_DECISION_TYPE_TRAP;
            isid_info.default_frwrd.default_frwd_type = SOC_PPC_L2_LIF_DFLT_FRWRD_SRC_LIF;
        } else {
            /* We already get the local inlif index from the HW, now get the global lif from the isid info. */
            global_in_lif_id = isid_info.global_lif ? isid_info.global_lif : BCM_GPORT_TUNNEL_ID_GET(mim_port->service_encap_id);
        }

        isid_info.service_type = SOC_PPC_L2_LIF_ISID_SERVICE_TYPE_P2P;

        if (mim_port->flags & BCM_MIM_PORT_TYPE_PEER) {
            isid_info.orientation = SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE;
        }
        else {
            isid_info.orientation = SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB;
        }
        isid_info.global_lif = global_in_lif_id;
        isid_info.is_extended = DPP_IS_FLAG_SET(mim_port->flags, BCM_MIM_PORT_SERVICE_ENCAP_INGERSS_WIDE);

        vsi_index = SOC_DPP_CONFIG(unit)->arad->pp_op_mode.p2p_info.mim_vsi;

        soc_sand_rv = soc_ppd_l2_lif_isid_add(soc_sand_dev_id, vsi_index, &isid_key, local_in_lif_id, &isid_info, TRUE, TRUE, 0, &success);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        SOC_SAND_IF_FAIL_RETURN(success);

        if (update) {
            /* get a ptr to previous db entry */
            rv = _bcm_dpp_sw_db_hash_vlan_find(unit, (sw_state_htb_key_t) &mim_port->mim_port_id, (sw_state_htb_data_t) &phy_port, TRUE);        
            if (GPORT_HASH_VLAN_NOT_FOUND(phy_port,rv)) {
                BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Updating Gport Dbase (0x%x)"), bcm_errmsg(rv), BCM_GPORT_MIM_PORT_ID_GET(mim_port->mim_port_id)));
            }
        }
        else {
            sal_memset(&phy_port, 0, sizeof(_BCM_GPORT_PHY_PORT_INFO));
        }

        /* update previous entry / fill new entry */
        phy_port.type = _bcmDppGportResolveTypeMimP2P;
        phy_port.phy_gport = global_in_lif_id; /* LIF */
        phy_port.encap_id = mim_port->match_tunnel_vlan; /* B-VID */

        
        /* add new db entry */
        rv = _bcm_dpp_sw_db_hash_vlan_insert(unit, (sw_state_htb_key_t) &mim_port->mim_port_id, (sw_state_htb_data_t) &phy_port);        
        if (BCM_FAILURE(rv)) {
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Updating Gport Dbase (0x%x)"), bcm_errmsg(rv), BCM_GPORT_MIM_PORT_ID_GET(mim_port->mim_port_id)));
        }
 

        /* save B-SA, I-SID and other mim_port data as lif_info - for restoration */
        rv = _bcm_dpp_in_lif_mim_match_add(unit, mim_port, local_in_lif_id);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else { /* MP */

        if (update) {
            /* get a ptr to previous db entry */
            rv = _bcm_dpp_sw_db_hash_vlan_find(unit, (sw_state_htb_key_t) &mim_port->mim_port_id, (sw_state_htb_data_t) &phy_port, TRUE);        
            if (GPORT_HASH_VLAN_NOT_FOUND(phy_port,rv)) {
                BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Updating Gport Dbase (0x%x)"), bcm_errmsg(rv), BCM_GPORT_MIM_PORT_ID_GET(mim_port->mim_port_id)));
            }
        }
        else {
            /* add db entry, mapping gport -> B-SA + B-VID */
            sal_memset(&phy_port, 0, sizeof(_BCM_GPORT_PHY_PORT_INFO));
        }

        /* update previous entry / fill new entry */
        phy_port.type = ((mim_port->flags & BCM_MIM_PORT_NO_LEARN_ENTRY) ? _bcmDppGportResolveTypeMimMPNoLearn : _bcmDppGportResolveTypeMimMP);

        soc_sand_rv = soc_sand_pp_mac_address_struct_to_long(&bmac_key.b_mac_addr, mac_addr_arr);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        phy_port.phy_gport = mac_addr_arr[0];  /* B-SA LSB */
        phy_port.encap_id = mac_addr_arr[1] | (mim_port->match_tunnel_vlan << 16); /* B-SA MSB (16 bits) + B-VID (16 bits) */


        /* add/replace new db entry */
        rv = _bcm_dpp_sw_db_hash_vlan_insert(unit, (sw_state_htb_key_t) &mim_port->mim_port_id, (sw_state_htb_data_t) &phy_port);        
        if (BCM_FAILURE(rv)) {
            BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Updating Gport Dbase (0x%x)"), bcm_errmsg(rv), BCM_GPORT_MIM_PORT_ID_GET(mim_port->mim_port_id)));
        }

        else { /* update, remove & add */
            rv = _bcm_dpp_sw_db_hash_vlan_find(unit, (sw_state_htb_key_t) &mim_port->mim_port_id, (sw_state_htb_data_t) &remove_phy_port,TRUE);        
            if (GPORT_HASH_VLAN_NOT_FOUND(phy_port,rv)) {
                BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) remove Gport Dbase (0x%x)"), bcm_errmsg(rv), BCM_GPORT_MIM_PORT_ID_GET(mim_port->mim_port_id)));
            }

            rv = _bcm_dpp_sw_db_hash_vlan_insert(unit, (sw_state_htb_key_t) &mim_port->mim_port_id, (sw_state_htb_data_t) &phy_port);        
            if (BCM_FAILURE(rv)) {
                BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Updating Gport Dbase (0x%x)"), bcm_errmsg(rv), BCM_GPORT_MIM_PORT_ID_GET(mim_port->mim_port_id)));
            }
        }
    }

exit:
    BCM_FREE(encap_entry_info);
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_mim_port_get(
    int unit,
    bcm_mim_vpn_t vpn,
    bcm_mim_port_t * mim_port)
{
    int rv, soc_sand_dev_id;
    uint32 soc_sand_rv, nof_entries;
    SOC_SAND_PP_MAC_ADDRESS src_bmac;
    SOC_PPC_FEC_ID fec_id;
    SOC_PPC_EG_ENCAP_ENTRY_INFO *encap_entry_info = NULL;
    uint32 next_eep[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX];
    _BCM_GPORT_PHY_PORT_INFO phy_port;
    uint32 mac_addr_arr[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S];
    SOC_PPC_LIF_ID local_inlif;
    int global_inlif, local_outlif;
    SOC_PPC_L2_LIF_ISID_KEY isid_key;
    SOC_PPC_SYS_VSI_ID vsi_index;
    SOC_PPC_L2_LIF_ISID_INFO isid_info;
    uint8 found;
    _bcm_lif_type_e lif_usage;
    uint8 is_wide_entry;
    uint32 profile_orientation;
	int status = BCM_E_NONE;
    int spb_is_supported = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mim_spb_enable", 0);

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    MIM_INIT(unit);

    /* Translate parameters to SOC_SAND parameters */
    soc_sand_dev_id = (unit);

    /* get B-SA/LIF and B-VID from db (by port) */
    rv = _bcm_dpp_sw_db_hash_vlan_find(unit, (sw_state_htb_key_t) &mim_port->mim_port_id, (sw_state_htb_data_t) &phy_port, FALSE);
    if (GPORT_HASH_VLAN_NOT_FOUND(phy_port,rv)) {
        BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Searching Gport Dbase (0x%x)"), bcm_errmsg(rv), BCM_GPORT_MIM_PORT_ID_GET(mim_port->mim_port_id)));
    }
    if (phy_port.type != _bcmDppGportResolveTypeMimMP && phy_port.type != _bcmDppGportResolveTypeMimP2P && phy_port.type != _bcmDppGportResolveTypeMimMPNoLearn) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("gport is not MIM")));
    }

    if (phy_port.type == _bcmDppGportResolveTypeMimP2P) { /* P2P */

        global_inlif = phy_port.phy_gport;  /* LIF */
        mim_port->match_tunnel_vlan = phy_port.encap_id >> 16; /* B-VID */

        rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS, global_inlif, (int*)&local_inlif);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Get wide entry info */
        if (SOC_IS_JERICHO(unit)) {

            rv = MBCM_PP_DRIVER_CALL(unit,mbcm_pp_lif_is_wide_entry,(unit, local_inlif, TRUE, &is_wide_entry, NULL));
            BCMDNX_IF_ERR_EXIT(rv);

            mim_port->flags |= (is_wide_entry) ? BCM_MIM_PORT_SERVICE_ENCAP_INGERSS_WIDE : 0;
        }


        /* get B-SA, I-SID and other mim_port data from lif_info */
        rv = _bcm_dpp_in_lif_mim_match_get(unit, mim_port, local_inlif);
        BCMDNX_IF_ERR_EXIT(rv);

        SOC_PPC_L2_LIF_ISID_KEY_clear(&isid_key);
        isid_key.isid_domain = 0;
        isid_key.isid_id = mim_port->egress_tunnel_service;

        soc_sand_rv = soc_ppd_l2_lif_isid_get(soc_sand_dev_id, &isid_key, &local_inlif, &vsi_index, &isid_info, &found);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        BCM_GPORT_TUNNEL_ID_SET(mim_port->service_encap_id, global_inlif);
    }
    else {

        mac_addr_arr[0] = phy_port.phy_gport;  /* B-SA LSB */
        mac_addr_arr[1] = phy_port.encap_id & 0xffff; /* B-SA MSB (16 bits) */
        mim_port->match_tunnel_vlan = phy_port.encap_id >> 16; /* B-VID (16 bits) */

        soc_sand_rv = soc_sand_pp_mac_address_long_to_struct(mac_addr_arr, &src_bmac);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        rv = _bcm_petra_mac_from_sand_mac(mim_port->match_tunnel_srcmac, &src_bmac);
        BCMDNX_IF_ERR_EXIT(rv);

        if (phy_port.type == _bcmDppGportResolveTypeMimMPNoLearn) {
            mim_port->flags |= BCM_MIM_PORT_NO_LEARN_ENTRY;
        }
    }

    /* get FEC */
    fec_id = BCM_GPORT_MIM_PORT_ID_GET(mim_port->mim_port_id);

	/* In JERICHO possibly set the CASCADED flag. */
	if (SOC_IS_JERICHO(unit)) {
		_bcm_dpp_am_fec_alloc_usage usage;
		status = bcm_dpp_am_fec_get_usage(unit, fec_id, &usage);
        BCMDNX_IF_ERR_EXIT(status);

		if (usage == _BCM_DPP_AM_FEC_ALLOC_USAGE_CASCADED) {
			mim_port->flags |= BCM_MIM_PORT_CASCADED;
        }
    }

    /* restore failover data and encap_id from FEC */
    rv = _bcm_dpp_l2_fec_fill_gport(unit, fec_id, mim_port, BCM_GPORT_MIM_PORT);
    BCMDNX_IF_ERR_EXIT(rv);
    
    /* Verify out LIF is MIM */
    rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, mim_port->encap_id, &local_outlif);
    BCMDNX_IF_ERR_EXIT(rv);   

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_local_lif_sw_resources_lif_usage_get(soc_sand_dev_id, -1, local_outlif, NULL, &lif_usage));
    if ((lif_usage != _bcmDppLifTypeMim) && (lif_usage != _bcmDppLifTypeOverlayLinkLayer)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("OutLIF is not MIM or overlay LL")));
    }

    BCMDNX_ALLOC(encap_entry_info, sizeof(*encap_entry_info) * SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX,
        "bcm_petra_mim_port_get.encap_entry_info");
    if(encap_entry_info == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failue\n")));
    }

    if (lif_usage == _bcmDppLifTypeMim) {

        /* get EEP entry */
        soc_sand_rv = soc_ppd_eg_encap_entry_get(soc_sand_dev_id, SOC_PPC_EG_ENCAP_EEP_TYPE_LL, local_outlif, 1, encap_entry_info, 
                                         next_eep, &nof_entries);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (encap_entry_info[0].entry_type != SOC_PPC_EG_ENCAP_ENTRY_TYPE_LL_ENCAP) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("invalid outlif setting. expect LL entry")));
        }
        rv = _bcm_petra_mac_from_sand_mac(mim_port->egress_tunnel_dstmac, &encap_entry_info[0].entry_val.ll_info.dest_mac);
        BCMDNX_IF_ERR_EXIT(rv);

        if (encap_entry_info[0].entry_val.ll_info.out_vid_valid) {
            mim_port->egress_tunnel_vlan = encap_entry_info[0].entry_val.ll_info.out_vid;
        }
        else {
            mim_port->egress_tunnel_vlan = BCM_VLAN_INVALID;
        }

        /* network group id */
        if (SOC_IS_JERICHO(unit) && (soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1))) {
            soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_get,
                                              (unit, SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_APP_ORIENTATION,
                                               &(encap_entry_info[0].entry_val.ll_info.outlif_profile),&profile_orientation));
            BCMDNX_IF_ERR_EXIT(soc_sand_rv); 

            if ((soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1) >= 2) || spb_is_supported) {
                mim_port->network_group_id = profile_orientation;
            }
            else {
                if (!profile_orientation) {
                    mim_port->flags &= BCM_MIM_PORT_TYPE_PEER;
                }
            }
        }
    } else if (lif_usage == _bcmDppLifTypeOverlayLinkLayer) {
        SOC_PPC_EG_ENCAP_OVERLAY_ARP_ENCAP_INFO ll_encap_info;
        SOC_PPC_EG_ENCAP_ETHER_TYPE_INDEX_INFO eth_type_index_entry;

        rv = soc_ppd_eg_encap_entry_get(
           unit, SOC_PPC_EG_ENCAP_EEP_TYPE_ROO_LL, local_outlif, 0, encap_entry_info, next_eep, &nof_entries);
        BCM_SAND_IF_ERR_EXIT(rv);

        ll_encap_info = encap_entry_info[0].entry_val.overlay_arp_encap_info; 


        /* 3. convert overlay arp infos, from soc to bcm  */
        /* dest mac */
        rv = _bcm_petra_mac_from_sand_mac(mim_port->egress_tunnel_dstmac, &ll_encap_info.dest_mac);
        BCMDNX_IF_ERR_EXIT(rv);

        /* src mac */
        rv = _bcm_petra_mac_from_sand_mac(mim_port->egress_tunnel_srcmac, &ll_encap_info.src_mac);
        BCMDNX_IF_ERR_EXIT(rv);

        /* outer-vid and PCP-DEI are both encode in outer-vlan field: encoding: {PCP DEI [15:12] VID[11:0]} */
        mim_port->egress_tunnel_vlan = VLAN_CTRL((ll_encap_info.pcp_dei >> 1), ll_encap_info.pcp_dei, ll_encap_info.out_vid);

        /*PCP DEI profile */
        mim_port->int_pri = ll_encap_info.pcp_dei_profile;

        /* get additional info drom table CfgEtherTypeIndex.
            Get the entry from SW */
        rv = bcm_dpp_am_template_eedb_roo_ll_format_eth_type_index_get(unit,ll_encap_info.eth_type_index, &eth_type_index_entry); 
        BCMDNX_IF_ERR_EXIT(rv);

        /* I-TAG is automatically constructed by HW */
        mim_port->egress_service_tpid = eth_type_index_entry.tpid_0;

        /* Fix fields for SPB split horizon mode 1 feature */
        if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mim_spb_enable", 0) && soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1) == 1) {
            int spb_da_bit_0 = 1 & (mim_port->egress_service_tpid >> 12);
            int spb_da_bit_1 = 1 & (mim_port->egress_service_tpid >> 13);
            mim_port->egress_service_tpid &= 0x0fff;
            mim_port->egress_service_tpid |= 0x8000;
            mim_port->network_group_id = ((mim_port->egress_tunnel_dstmac[5] & 0x80) ? 10 : 0) | ((mim_port->egress_tunnel_dstmac[5] & 0x08) ? 1 : 0);
            mim_port->egress_tunnel_dstmac[5] &= 0x77;
            mim_port->egress_tunnel_dstmac[5] |= (spb_da_bit_0 << 3 & spb_da_bit_1 << 7);
        }
    }

exit:
    BCM_FREE(encap_entry_info);
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_mim_port_delete(
    int unit, 
    bcm_mim_vpn_t vpn, 
    bcm_gport_t mim_port_id)
{
    int rv;
    uint32 soc_sand_rv;
    SOC_SAND_PP_MAC_ADDRESS src_bmac;
    SOC_PPC_FEC_ID fec_id /* work */, protect_fec_id;
    uint32 fec_flags = 0;
    SOC_PPC_BMACT_ENTRY_KEY bmac_key;
    _BCM_GPORT_PHY_PORT_INFO phy_port;
    SOC_PPC_LIF_ID tmp_local_lif;
    bcm_mim_port_t mim_port;        
    uint32  soc_ppd_port;
    int local_port, core;
    SOC_PPC_L2_LIF_ISID_KEY isid_key;
    SOC_PPC_LIF_ID isid_local_lif_id;
    int isid_global_lif_id, global_outlif_id, local_outlif_id;
    _bcm_dpp_gport_info_t gport_info;
    _bcm_lif_type_e lif_usage;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    MIM_INIT(unit);

    bcm_mim_port_t_init(&mim_port);

    mim_port.mim_port_id = mim_port_id;

    rv = bcm_petra_mim_port_get(unit, vpn, &mim_port);
    BCMDNX_IF_ERR_EXIT(rv);

    global_outlif_id = mim_port.encap_id;

    rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, global_outlif_id, &local_outlif_id);
    BCMDNX_IF_ERR_EXIT(rv);
    

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_local_lif_sw_resources_lif_usage_get(unit, -1, local_outlif_id, NULL, &lif_usage));
    if ((lif_usage != _bcmDppLifTypeMim) && (lif_usage != _bcmDppLifTypeOverlayLinkLayer)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("OutLIF is not a MIM or Overlay LL")));
    }

    rv = _bcm_petra_mac_to_sand_mac(mim_port.match_tunnel_srcmac, &src_bmac);
    BCMDNX_IF_ERR_EXIT(rv);

    /* get FEC */
    fec_id = BCM_GPORT_MIM_PORT_ID_GET(mim_port.mim_port_id);
    if (fec_id == -1) {
        rv = BCM_E_PARAM;
        BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) GPORT (0x%x) not of type MIM"), bcm_errmsg(rv), BCM_GPORT_MIM_PORT_ID_GET(mim_port.mim_port_id)));
    }

    /* remove FEC entry */
    soc_sand_rv = soc_ppd_frwrd_fec_remove(unit, fec_id);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if (lif_usage == _bcmDppLifTypeMim) {
        /* remove EEP entry */
        soc_sand_rv = soc_ppd_eg_encap_entry_remove(unit, SOC_PPC_EG_ENCAP_EEP_TYPE_LL, local_outlif_id);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    } else if (lif_usage == _bcmDppLifTypeOverlayLinkLayer) {
        SOC_PPC_EG_ENCAP_ENTRY_INFO encap_entry_info[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX];
        uint32 next_eep[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX];
        uint32 nof_entries; 
        int eth_type_index; 
        int is_last; 

        /* get current entry to access eth-type-index */
        rv = soc_ppd_eg_encap_entry_get(
           unit, SOC_PPC_EG_ENCAP_EEP_TYPE_ROO_LL, local_outlif_id, 0, encap_entry_info, next_eep, &nof_entries);
        BCM_SAND_IF_ERR_EXIT(rv);
        eth_type_index  = encap_entry_info[0].entry_val.overlay_arp_encap_info.eth_type_index; 

        /* remove eedb entry in HW*/
        rv = soc_ppd_eg_encap_entry_remove(unit, SOC_PPC_EG_ENCAP_EEP_TYPE_ROO_LL, local_outlif_id);
        BCM_SAND_IF_ERR_EXIT(rv);

        /* free template mgr entry (sw db). */
        rv = bcm_dpp_am_template_eedb_roo_ll_format_eth_type_index_free(
           unit, eth_type_index, &is_last); 
        BCM_SAND_IF_ERR_EXIT(rv);

        /* remove additional table: CfgEtherTypeIndex table */
        /* remove from HW 
           (if this entry was the last one pointing on CfgEtherTypeIndex table) */
        if (is_last) {
            rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_eg_encap_ether_type_index_clear,(unit, eth_type_index)));
            BCMDNX_IF_ERR_EXIT(rv);
       }
    }

    /* get db entry to check if P2P port */
    rv = _bcm_dpp_sw_db_hash_vlan_find(unit, (sw_state_htb_key_t) &(mim_port.mim_port_id), (sw_state_htb_data_t) &phy_port, FALSE);
    if (GPORT_HASH_VLAN_NOT_FOUND(phy_port,rv)) {
        BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Searching Gport Dbase (0x%x)"), bcm_errmsg(rv), BCM_GPORT_MIM_PORT_ID_GET(mim_port.mim_port_id)));
    }

    if (phy_port.type == _bcmDppGportResolveTypeMimP2P) { /* P2P */

        isid_global_lif_id = phy_port.phy_gport;  /* LIF */

        rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS, isid_global_lif_id, (int*)&tmp_local_lif);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = bcm_petra_port_tpid_delete_all(unit, mim_port.service_encap_id);
        BCMDNX_IF_ERR_EXIT(rv);

        SOC_PPC_L2_LIF_ISID_KEY_clear(&isid_key);
        isid_key.isid_domain = 0; /* isid_domain is always 0 */
        isid_key.isid_id = mim_port.egress_tunnel_service;

        /* delete lif from HW */
        soc_sand_rv = soc_ppd_l2_lif_isid_remove(unit, &isid_key, TRUE, FALSE, 0, FALSE, &isid_local_lif_id);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (isid_local_lif_id != tmp_local_lif) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("isid lif id doesn't match lif index")));
        }

        /* de-allocate LIF */
        if (SOC_IS_JERICHO(unit)) {
            rv = _bcm_dpp_gport_delete_global_and_local_lif(unit, isid_global_lif_id, isid_local_lif_id, _BCM_GPORT_ENCAP_ID_LIF_INVALID);
            BCMDNX_IF_ERR_EXIT(rv);
        } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
            rv = bcm_dpp_am_l2_ac_dealloc(unit, 0, isid_local_lif_id);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /* remove mim lif match SW */
        rv = _bcm_dpp_local_lif_sw_resources_delete(unit, isid_local_lif_id, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS);
        BCMDNX_IF_ERR_EXIT(rv); 

    }
    else { /* MP */

        rv = _bcm_dpp_gport_to_phy_port(unit, mim_port.port, 0, &gport_info); 
        BCMDNX_IF_ERR_EXIT(rv);

        if ((_BCM_DPP_GPORT_INFO_IS_LOCAL_PORT(gport_info)) && (!(mim_port.flags & BCM_MIM_PORT_NO_LEARN_ENTRY))) { /* Only Local settings MAC Tunnel identification and no flag */
            BCM_PBMP_ITER(gport_info.pbmp_local_ports, local_port) {
                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, local_port, &soc_ppd_port, &core)));

                /* remove B-MACT entry */
                SOC_PPC_BMACT_ENTRY_KEY_clear(&bmac_key);

                bmac_key.b_mac_addr = src_bmac;
                bmac_key.b_vid = _BCM_DPP_BVID_TO_VLAN(mim_port.match_tunnel_vlan);

                bmac_key.flags |= SOC_PPC_BMACT_ENTRY_TYPE_LEARN;
                bmac_key.local_port_ndx = soc_ppd_port;

                if (SOC_IS_JERICHO(unit) && !SOC_IS_QAX(unit)) {
                    bmac_key.core = core;
                }

                soc_sand_rv = soc_ppd_frwrd_bmact_entry_remove(unit, &bmac_key);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
        }
    }

    /* remove db entry */
    rv = _bcm_dpp_sw_db_hash_vlan_find(unit, (sw_state_htb_key_t) &(mim_port.mim_port_id), (sw_state_htb_data_t) &phy_port, TRUE);
    if (GPORT_HASH_VLAN_NOT_FOUND(phy_port,rv)) {
        BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("error(%s) Searching Gport Dbase (0x%x)"), bcm_errmsg(rv), BCM_GPORT_MIM_PORT_ID_GET(mim_port.mim_port_id)));
    }

    /* de-allocate FEC */    
    fec_id = BCM_GPORT_MIM_PORT_ID_GET(mim_port.mim_port_id);

    if (_BCM_PPD_IS_VALID_FAILOVER_ID(mim_port.failover_id)) {
        /* if failover is valid then second member in protection already allocated, get info */
        if (mim_port.failover_gport_id != BCM_GPORT_TYPE_NONE) {
            protect_fec_id =  BCM_GPORT_MIM_PORT_ID_GET(mim_port.failover_gport_id);
            fec_id = _BCM_PPD_GPORT_PROTECT_TO_WORK_FEC(protect_fec_id);
        }
        else
        {
            /* de-allocate 2 fec entries */
            rv = bcm_dpp_am_fec_dealloc(unit, fec_flags, 2, fec_id);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }
    else {
        /* de-allocate 1 fec entry */
        rv = bcm_dpp_am_fec_dealloc(unit, fec_flags, 1, fec_id);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* de-allocate EEP */
    if (SOC_IS_JERICHO(unit)) {
        rv = _bcm_dpp_gport_delete_global_and_local_lif(unit, global_outlif_id, _BCM_GPORT_ENCAP_ID_LIF_INVALID, local_outlif_id);
        BCMDNX_IF_ERR_EXIT(rv);
    } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        rv = bcm_dpp_am_l3_eep_dealloc(unit, global_outlif_id);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* Remove entry from SW DB */
    rv = _bcm_dpp_local_lif_sw_resources_delete(unit, -1, local_outlif_id, _BCM_DPP_GPORT_SW_RESOURCES_EGRESS);
    BCMDNX_IF_ERR_EXIT(rv); 

exit:
    BCMDNX_FUNC_RETURN;
}


