/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 *
 * QoS module
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_QOS

#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>

#include <sal/core/libc.h>

#include <shared/bitop.h>
#include <shared/gport.h>

#include <soc/defs.h>
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/util.h>
#include <soc/debug.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/mbcm.h>
#include <soc/dpp/mbcm_pp.h>

#include <soc/dpp/PPD/ppd_api_general.h>
#include <soc/dpp/PPD/ppd_api_lif_cos.h>
#include <soc/dpp/PPD/ppd_api_eg_qos.h> 
#include <soc/dpp/PPD/ppd_api_eg_vlan_edit.h>
#include <soc/dpp/PPD/ppd_api_eg_ac.h>
#include <soc/dpp/PPD/ppd_api_lif.h>
#include <soc/dpp/PPD/ppd_api_lif_table.h>
#include <soc/dpp/PPD/ppd_api_frwrd_ilm.h>
#include <soc/dpp/PPD/ppd_api_frwrd_bmact.h>
#include <soc/dpp/PPD/ppd_api_llp_parse.h>
#include <soc/dpp/PPD/ppd_api_port.h>

#include <shared/gport.h>

#include <bcm/l2.h>
#include <bcm/qos.h>
#include <bcm/debug.h>
#include <bcm/error.h>

#include <bcm_int/petra_dispatch.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/alloc_mngr_glif.h>
#include <bcm_int/dpp/qos.h>
#include <bcm_int/dpp/vlan.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dpp/switch.h>


#include <bcm_int/dpp/oam.h>



#define _BCM_ERROR_IF_OUT_OF_RANGE(val, max_limit)  \
    if (val >= max_limit) {                          \
        return BCM_E_PARAM;                         \
    }                                               

#define _DPP_QOS_ENTRY_INDEX_ING_LIF_COS_MPLS(_profile_, _exp_) \
    ((_profile_)*_BCM_QOS_MAP_ING_MPLS_EXP_MAX + (_exp_))
#define _DPP_QOS_ENTRY_INDEX_ING_LIF_COS_L3(_profile_, _dscp_) \
    ((_profile_)*_BCM_QOS_MAP_ING_L3_DSCP_MAX + (_dscp_))
#define _DPP_QOS_ENTRY_INDEX_ING_LIF_COS_L2_INNER(_profile_, _up_, _dei_) \
    ((_profile_)*_BCM_QOS_MAP_ING_L2_PRI_CFI_MAX + ((_up_)<<1) + (_dei_))
#define _DPP_QOS_ENTRY_INDEX_ING_LIF_COS_L2_OUTER(_profile_, _up_, _dei_) \
    ((_profile_)*_BCM_QOS_MAP_ING_L2_PRI_CFI_MAX + ((_up_)<<1) + (_dei_))
#define _DPP_QOS_ENTRY_INDEX_ING_LIF_COS_L2_UNTAG(_profile_, _tc_, _dp_) \
    ((_profile_)*_BCM_QOS_MAP_TC_DP_MAX + ((_tc_)<<2) + (_dp_))
#define _DPP_QOS_ENTRY_INDEX_ING_PCP_VLAN_CTAG(_profile_, _up_) \
    ((_profile_)*_BCM_QOS_MAP_ING_VLAN_PCP_CTAG_MAX + (_up_))
#define _DPP_QOS_ENTRY_INDEX_ING_PCP_VLAN_UTAG(_profile_, _tc_, _dp_) \
    ((_profile_)*_BCM_QOS_MAP_ING_VLAN_PCP_UTAG_MAX + ((_tc_)<<2) + (_dp_))
#define _DPP_QOS_ENTRY_INDEX_ING_PCP_VLAN_STAG(_profile_, _up_, _dei_) \
    ((_profile_)*_BCM_QOS_MAP_ING_VLAN_PCP_STAG_MAX + ((_up_)<<1) + (_dei_))
#define _DPP_QOS_ENTRY_INDEX_EGR_REMARK_ENP_MPLS(_profile_, _exp_) \
    ((_profile_)*_BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX + (_exp_))
#define _DPP_QOS_ENTRY_INDEX_EGR_REMARK_ENP_L3(_profile_, _dscp_) \
    ((_profile_)*_BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX + (_dscp_))
#define _DPP_QOS_ENTRY_INDEX_EGR_REMARK_ENP_L2(_profile_, _pcp_, _dp_) \
    ((_profile_)*_BCM_QOS_MAP_EGR_REMARK_L2_MAX + ((_pcp_)<<2) + (_dp_))
#define _DPP_QOS_ENTRY_INDEX_EGR_REMARK_MPLS(_profile_, _exp_, _dp_) \
    (((_profile_)*_BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX*4) + ((_exp_)<<2) + (_dp_))
#define _DPP_QOS_ENTRY_INDEX_EGR_REMARK_L3(_profile_, _dscp_, _dp_) \
    (((_profile_)*_BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX*4) + ((_dscp_)<<2) + (_dp_))
#define _DPP_QOS_ENTRY_INDEX_EGR_PCP_VLAN_CTAG(_profile_, _up_) \
    ((_profile_)*_BCM_QOS_MAP_EGR_VLAN_PRI_MAX + (_up_))
#define _DPP_QOS_ENTRY_INDEX_EGR_PCP_VLAN_UTAG(_profile_, _tc_, _dp_) \
    (((_profile_)*_BCM_QOS_MAP_EGR_VLAN_PRI_MAX*4) + ((_tc_)<<2) + (_dp_))
#define _DPP_QOS_ENTRY_INDEX_EGR_PCP_VLAN_STAG(_profile_, _up_, _dei_) \
    (((_profile_)*_BCM_QOS_MAP_EGR_VLAN_PRI_MAX*2) + ((_up_)<<1) + (_dei_))        
#define _DPP_QOS_ENTRY_INDEX_EGR_PCP_VLAN_MPLS(_profile_, _exp_) \
    ((_profile_)*_BCM_QOS_MAP_EGR_EXP_MAX + (_exp_))
#define _DPP_QOS_ENTRY_INDEX_EGR_PCP_VLAN_L3(_profile_, _dscp_) \
    ((_profile_)*_BCM_QOS_MAP_EGR_DSCP_MAX + (_dscp_))
#define _DPP_QOS_ENTRY_INDEX_EGR_MPLS_PHP(_profile_, _exp_) \
    ((_profile_)*_BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX + (_exp_))
#define _DPP_QOS_ENTRY_INDEX_EGR_L2_I_TAG(_profile_, _tc_, _dp_) \
    ((_profile_)*_BCM_QOS_MAP_EGR_VLAN_PRI_MAX*4 + ((_tc_)<<2) + (_dp_))
#define _DPP_QOS_ENTRY_INDEX_EGR_DSCP_EXP_MARKING(_profile_, _inlif_profile, _tc_, _dp_) \
    ((_profile_)*(_BCM_QOS_MAP_EGR_RESOLVED_DP_MAX * _BCM_QOS_MAP_TC_MAX * SOC_OCC_MGMT_NOF_INLIF_PROFILES_TRANSFERED_TO_EGRESS) + ((_inlif_profile)<<5)+ ((_tc_)<<2) + (_dp_))
#define _DPP_QOS_ENTRY_INDEX_DP(_profile_, _dp_, pcp_dei) \
    ((_profile_)*_BCM_QOS_MAP_ING_DP_ENTRY_MAX + ((_dp_) * _BCM_QOS_MAP_PCP_DEI_MAX) + pcp_dei)

/* returns the number of set bits in the range of _max_in_profile_*_profile_ to _max_in_profile_*(_profile_+1) for _bitmap_ */
#define _DPP_QOS_ENTRY_USED_CNT(_bitmap_, _profile_, _max_in_profile_, _count_) \
    BCMDNX_IF_ERR_EXIT(QOS_ACCESS._bitmap_.bit_range_count(unit, _max_in_profile_*_profile_, _max_in_profile_, &_count_));

#define QOS_ACCESS                  sw_state_access[unit].dpp.bcm.qos

#define _BCM_DPP_QOS_DEFAULT_EGR_PCP_DEI_PROFILE (0)

#define _BCM_DPP_QOS_PWE_P2P_MAX_PROFILE  (15)

#define _BCM_DPP_QOS_MAP_ENCAP_INTPRI_COLOR_DEFAULT_ENTRY  (0)

/* Initialize the QoS module. */
int 
bcm_petra_qos_init(int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    int profile_ndx;
    uint8 is_allocated;
    bcm_qos_map_t qos_map;
    int qos_map_id;
    int flags;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    if (!SOC_WARM_BOOT(unit)) {
        rv = QOS_ACCESS.is_allocated(unit, &is_allocated);
        BCMDNX_IF_ERR_EXIT(rv);

        if(!is_allocated) {
            rv = QOS_ACCESS.alloc(unit);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /* ing_lif_cos */
        rv = QOS_ACCESS.ing_lif_cos_mpls_bitmap.alloc_bitmap(unit, _BCM_QOS_MAP_ING_MPLS_EXP_MAX * SOC_DPP_CONFIG(unit)->qos.nof_ing_lif_cos); 
        BCMDNX_IF_ERR_EXIT(rv);
    
        rv = QOS_ACCESS.ing_lif_cos_ipv4_bitmap.alloc_bitmap(unit, _BCM_QOS_MAP_ING_L3_DSCP_MAX * SOC_DPP_CONFIG(unit)->qos.nof_ing_lif_cos);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = QOS_ACCESS.ing_lif_cos_ipv6_bitmap.alloc_bitmap(unit, _BCM_QOS_MAP_ING_L3_DSCP_MAX * SOC_DPP_CONFIG(unit)->qos.nof_ing_lif_cos);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = QOS_ACCESS.ing_lif_cos_l2_inner_bitmap.alloc_bitmap(unit, _BCM_QOS_MAP_ING_L2_PRI_CFI_MAX * SOC_DPP_CONFIG(unit)->qos.nof_ing_lif_cos);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = QOS_ACCESS.ing_lif_cos_l2_outer_bitmap.alloc_bitmap(unit, _BCM_QOS_MAP_ING_L2_PRI_CFI_MAX * SOC_DPP_CONFIG(unit)->qos.nof_ing_lif_cos);
        BCMDNX_IF_ERR_EXIT(rv);
    
        rv = QOS_ACCESS.ing_lif_cos_l2_untag_bitmap.alloc_bitmap(unit, _BCM_QOS_MAP_TC_DP_MAX * SOC_DPP_CONFIG(unit)->qos.nof_ing_lif_cos);
        BCMDNX_IF_ERR_EXIT(rv);

        /* ing_pcp_vlan */
        rv = QOS_ACCESS.ing_pcp_vlan_ctag_bitmap.alloc_bitmap(unit, _BCM_QOS_MAP_ING_VLAN_PCP_CTAG_MAX * SOC_DPP_CONFIG(unit)->qos.nof_ing_pcp_vlan);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = QOS_ACCESS.ing_pcp_vlan_utag_bitmap.alloc_bitmap(unit, _BCM_QOS_MAP_ING_VLAN_PCP_UTAG_MAX * SOC_DPP_CONFIG(unit)->qos.nof_ing_pcp_vlan);
        BCMDNX_IF_ERR_EXIT(rv);
    
        rv = QOS_ACCESS.ing_pcp_vlan_stag_bitmap.alloc_bitmap(unit, _BCM_QOS_MAP_ING_VLAN_PCP_STAG_MAX * SOC_DPP_CONFIG(unit)->qos.nof_ing_pcp_vlan);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = QOS_ACCESS.ing_pcp_vlan_color_bitmap.alloc_bitmap(unit, _BCM_QOS_MAP_ING_DP_ENTRY_MAX * SOC_DPP_CONFIG(unit)->qos.nof_ing_dp);
        BCMDNX_IF_ERR_EXIT(rv);

        /* egr_remark */
        rv = QOS_ACCESS.egr_remark_encap_mpls_bitmap.alloc_bitmap(unit, _BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX * SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = QOS_ACCESS.egr_remark_encap_ipv6_bitmap.alloc_bitmap(unit, _BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX * SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = QOS_ACCESS.egr_remark_encap_ipv4_bitmap.alloc_bitmap(unit, _BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX * SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = QOS_ACCESS.egr_remark_encap_l2_bitmap.alloc_bitmap(unit, _BCM_QOS_MAP_EGR_REMARK_L2_MAX * SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = QOS_ACCESS.egr_remark_mpls_bitmap.alloc_bitmap(unit, _BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX*4 * SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = QOS_ACCESS.egr_remark_ipv6_bitmap.alloc_bitmap(unit, _BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX*4 * SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = QOS_ACCESS.egr_remark_ipv4_bitmap.alloc_bitmap(unit, _BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX*4 * SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id);
        BCMDNX_IF_ERR_EXIT(rv);

        /* egr_mpls_php */
        rv = QOS_ACCESS.egr_mpls_php_ipv6_bitmap.alloc_bitmap(unit, _BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX * SOC_DPP_CONFIG(unit)->qos.nof_egr_mpls_php);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = QOS_ACCESS.egr_mpls_php_ipv4_bitmap.alloc_bitmap(unit, _BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX * SOC_DPP_CONFIG(unit)->qos.nof_egr_mpls_php);
        BCMDNX_IF_ERR_EXIT(rv);
    
        /* egr_pcp_vlan */
        rv = QOS_ACCESS.egr_pcp_vlan_ctag_bitmap.alloc_bitmap(unit, _BCM_QOS_MAP_EGR_VLAN_PRI_MAX * SOC_DPP_CONFIG(unit)->qos.nof_egr_pcp_vlan);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = QOS_ACCESS.egr_pcp_vlan_utag_bitmap.alloc_bitmap(unit, _BCM_QOS_MAP_EGR_VLAN_PRI_MAX*4 * SOC_DPP_CONFIG(unit)->qos.nof_egr_pcp_vlan);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = QOS_ACCESS.egr_pcp_vlan_stag_bitmap.alloc_bitmap(unit, _BCM_QOS_MAP_EGR_VLAN_PRI_MAX*2 * SOC_DPP_CONFIG(unit)->qos.nof_egr_pcp_vlan);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = QOS_ACCESS.egr_pcp_vlan_mpls_bitmap.alloc_bitmap(unit, _BCM_QOS_MAP_EGR_EXP_MAX * SOC_DPP_CONFIG(unit)->qos.nof_egr_pcp_vlan);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = QOS_ACCESS.egr_pcp_vlan_ipv6_bitmap.alloc_bitmap(unit, _BCM_QOS_MAP_EGR_DSCP_MAX * SOC_DPP_CONFIG(unit)->qos.nof_egr_pcp_vlan);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = QOS_ACCESS.egr_pcp_vlan_ipv4_bitmap.alloc_bitmap(unit, _BCM_QOS_MAP_EGR_DSCP_MAX * SOC_DPP_CONFIG(unit)->qos.nof_egr_pcp_vlan);
        BCMDNX_IF_ERR_EXIT(rv);
    
        /* egr_l2_i_tag */
        rv = QOS_ACCESS.egr_l2_i_tag_bitmap.alloc_bitmap(unit, _BCM_QOS_MAP_EGR_VLAN_PRI_MAX*4 * SOC_DPP_CONFIG(unit)->qos.nof_egr_l2_i_tag);
        BCMDNX_IF_ERR_EXIT(rv);

    }

#ifdef BCM_88660

    if (SOC_IS_ARADPLUS(unit)) {
        int simple_mode = soc_property_get(unit, spn_BCM886XX_QOS_L3_L2_MARKING, 0);

        if (!SOC_WARM_BOOT(unit)) {
            /* egr_dscp_exp_marking */
            rv = QOS_ACCESS.egr_dscp_exp_marking_bitmap.alloc_bitmap(unit, 
                _BCM_QOS_MAP_EGR_RESOLVED_DP_MAX * _BCM_QOS_MAP_TC_MAX * SOC_OCC_MGMT_NOF_INLIF_PROFILES_TRANSFERED_TO_EGRESS * 
                SOC_DPP_CONFIG(unit)->qos.nof_egr_dscp_exp_marking);
            BCMDNX_IF_ERR_EXIT(rv);

            /* If we are in simple mode, then there is a DSCP marking bit in the inlif profile. */
            /* For all inlif profiles with this bit set, we turn DSCP marking on. */
            if (simple_mode) {
                uint32 mask = 0;
                uint32 inlif_profile_idx;
                uint32 bitmap = 0;
                uint32 soc_sand_rv = 0;
                SOC_PPC_EG_QOS_GLOBAL_INFO info;

                /* Get the mask of the relevant inlif profile */
                rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_set,
                                         (unit, SOC_OCC_MGMT_TYPE_INLIF, SOC_OCC_MGMT_INLIF_APP_SIMPLE_DSCP_MARKING,
                                          1 /* value */, &mask));
                BCMDNX_IF_ERR_EXIT(rv);

                /* Setup the bitmap */
                for (inlif_profile_idx = 0; inlif_profile_idx < SOC_OCC_MGMT_NOF_INLIF_PROFILES_TRANSFERED_TO_EGRESS; inlif_profile_idx++) {
                
                    if ((inlif_profile_idx & mask) == 0) {
                        continue;
                    }
                    bitmap |= (1 << inlif_profile_idx);
                }

                SOC_PPC_EG_QOS_GLOBAL_INFO_clear(&info);
                soc_sand_rv = soc_ppd_eg_qos_global_info_get(unit, &info);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                info.in_lif_profile_bitmap = bitmap;
                soc_sand_rv = soc_ppd_eg_qos_global_info_set(unit, &info);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);        
            }
        }
    }

#endif /* BCM_88660 */
    if(!SOC_WARM_BOOT(unit)) {
        rv = QOS_ACCESS.ing_cos_opcode_flags.is_allocated(unit, &is_allocated);
        BCMDNX_IF_ERR_EXIT(rv);

        if(!is_allocated) {
            rv = QOS_ACCESS.ing_cos_opcode_flags.alloc(unit, SOC_DPP_CONFIG(unit)->qos.nof_ing_cos_opcode);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        rv = QOS_ACCESS.opcode_bmp.alloc_bitmap(unit, SOC_DPP_CONFIG(unit)->qos.nof_ing_cos_opcode);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = QOS_ACCESS.init.set(unit, TRUE);
        BCMDNX_IF_ERR_EXIT(rv);


        /* 
         * Allocate from advance QOS Egress PCP-DEI MAP profile 0.
         * PCP-DEP Map profile 0 is used for the default egress VLAN editing (in case of untagged packets).
         * In order for user to use QOS Egress PCP-DEI Map profile 0 destroy and create it again.
         */
        profile_ndx = _BCM_DPP_QOS_DEFAULT_EGR_PCP_DEI_PROFILE;
        rv = bcm_dpp_am_qos_egr_pcp_vlan_alloc(unit, BCM_DPP_AM_FLAG_ALLOC_WITH_ID, &profile_ndx);
        BCMDNX_IF_ERR_EXIT(rv);        
    }

    /* We catch the first entry in the COS profile table and configure it so it doesn't change the TC and DP*/
    if (!SOC_WARM_BOOT(unit) && SOC_IS_JERICHO(unit))
    {
        
        qos_map_id = _BCM_DPP_QOS_MAP_ENCAP_INTPRI_COLOR_DEFAULT_ENTRY; /* we catch the first entry */
        bcm_qos_map_t_init(&qos_map);
        qos_map.int_pri = BCM_PRIO_PRESERVE;
        qos_map.color = bcmColorPreserve;
        flags = 0;

        rv = bcm_petra_qos_map_create(unit, BCM_QOS_MAP_EGRESS| BCM_QOS_MAP_ENCAP_INTPRI_COLOR | BCM_QOS_MAP_WITH_ID , &qos_map_id);
        BCM_IF_ERROR_RETURN(rv);

        rv = bcm_petra_qos_map_add(unit, flags, &qos_map, qos_map_id);
        BCM_IF_ERROR_RETURN(rv);
        
    }


    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

/* Detach the QoS module. */
int 
bcm_petra_qos_detach(int unit)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
exit:
    BCMDNX_FUNC_RETURN;
}

/* Create a QoS map profile */
int 
bcm_petra_qos_map_create(int unit, uint32 flags, int *map_id)
{
    int rv = BCM_E_NONE;
    int am_flags = 0;
    int profile_ndx = BCM_QOS_MAP_PROFILE_GET(*map_id);
   
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);

    if (flags & BCM_QOS_MAP_WITH_ID) {
        am_flags |= BCM_DPP_AM_FLAG_ALLOC_WITH_ID;
    }

    if (flags & BCM_QOS_MAP_INGRESS) { 

        if (flags & BCM_QOS_MAP_L2_VLAN_PCP) {
            rv = bcm_dpp_am_qos_ing_pcp_vlan_alloc(unit, am_flags, &profile_ndx);
            if (rv != BCM_E_NONE) {
                LOG_ERROR(BSL_LS_BCM_QOS,
                          (BSL_META_U(unit,
                                      "%s: %d is Out of Ingress PCP Profile resources\n"),FUNCTION_NAME(), profile_ndx));
                BCMDNX_IF_ERR_EXIT(rv);
            }
            BCM_INT_QOS_MAP_INGRESS_PCP_VLAN_SET(*map_id, profile_ndx);
        } 
        else if (flags & BCM_QOS_MAP_MPLS_ELSP) {
            rv = bcm_dpp_am_qos_ing_elsp_alloc(unit, am_flags, &profile_ndx);
            if (rv != BCM_E_NONE) {
                LOG_ERROR(BSL_LS_BCM_QOS,
                          (BSL_META_U(unit,
                                      "%s: %d is Out of MPLS ELSP Profile resources\n"),FUNCTION_NAME(), profile_ndx));
                BCMDNX_IF_ERR_EXIT(rv);
            }
            BCM_INT_QOS_MAP_MPLS_ELSP_SET(*map_id, profile_ndx);
        } else if (flags & BCM_QOS_MAP_OPCODE) {
            rv = bcm_dpp_am_qos_ing_cos_opcode_alloc(unit, am_flags, &profile_ndx);
            if (rv != BCM_E_NONE) {
                LOG_ERROR(BSL_LS_BCM_QOS,
                          (BSL_META_U(unit,
                                      "%s: %d is Out of COS OPCODE Profile resources\n"),FUNCTION_NAME(), profile_ndx));
                BCMDNX_IF_ERR_EXIT(rv);
            }
            BCM_INT_QOS_MAP_TYPE_OPCODE_SET(*map_id, profile_ndx);
        }
        else if (flags & BCM_QOS_MAP_POLICER) {/*ingress dp profile*/
            if (SOC_IS_JERICHO_PLUS(unit)) 
             {
                 rv = bcm_dpp_am_qos_ing_color_alloc(unit, am_flags, &profile_ndx);
                 if (rv != BCM_E_NONE)
                 {
                     LOG_ERROR(BSL_LS_BCM_QOS,
                               (BSL_META_U(unit,
                                           "Ingress DP Profile resources\n")));
                     BCMDNX_IF_ERR_EXIT(rv);
                 }
                 BCM_INT_QOS_MAP_INGRESS_COLOR_SET(*map_id, profile_ndx);  
             } 
             else
             {
                     LOG_ERROR(BSL_LS_BCM_QOS,
                               (BSL_META_U(unit,
                                           "%s: Ingress DP Profile available only for QAX.\n"),FUNCTION_NAME()));
                     BCMDNX_IF_ERR_EXIT(BCM_E_PARAM);
             }
            
        }
        else { /* L3 */
            rv = bcm_dpp_am_qos_ing_lif_cos_alloc(unit, am_flags, &profile_ndx);
            if (rv != BCM_E_NONE) {
                LOG_ERROR(BSL_LS_BCM_QOS,
                          (BSL_META_U(unit,
                                      "%s: %d is Out of Ingress Lif/Cos resources\n"),FUNCTION_NAME(), profile_ndx));
                BCMDNX_IF_ERR_EXIT(rv);
            }
            BCM_INT_QOS_MAP_LIF_COS_SET(*map_id, profile_ndx);
        }
    } 
    else if (flags & BCM_QOS_MAP_EGRESS) {

         if (flags & BCM_QOS_MAP_L2_VLAN_PCP) {
            if ((am_flags & BCM_DPP_AM_FLAG_ALLOC_WITH_ID) && 
                profile_ndx == _BCM_DPP_QOS_DEFAULT_EGR_PCP_DEI_PROFILE) {
                /* 
                 * No need to go to Alloc since we allocate it from advance.
                 * We dont verify existance of Profile 0 just returning this profile
                 */
            } else {
                rv = bcm_dpp_am_qos_egr_pcp_vlan_alloc(unit, am_flags, &profile_ndx);
                if (rv != BCM_E_NONE) {
                    LOG_ERROR(BSL_LS_BCM_QOS,
                              (BSL_META_U(unit,
                                          "%s: %d is Out of Egress PCP Profile resources\n"),FUNCTION_NAME(), profile_ndx));
                    BCMDNX_IF_ERR_EXIT(rv);
                }                
            }
            BCM_INT_QOS_MAP_EGRESS_PCP_VLAN_SET(*map_id, profile_ndx);  
         } 
         else if (flags & BCM_QOS_MAP_MIM_ITAG) {
             rv = bcm_dpp_am_qos_egr_l2_i_tag_alloc(unit, am_flags, &profile_ndx);
             if (rv != BCM_E_NONE) {
                 LOG_ERROR(BSL_LS_BCM_QOS,
                           (BSL_META_U(unit,
                                       "%s: %d is Out of Egress PCP Profile resources\n"),FUNCTION_NAME(), profile_ndx));
                 BCMDNX_IF_ERR_EXIT(rv);
             }
             BCM_INT_QOS_MAP_EGRESS_L2_I_TAG_SET(*map_id, profile_ndx);  
         } 
         else if (flags & BCM_QOS_MAP_MPLS_PHP) {
             rv = bcm_dpp_am_qos_egr_mpls_php_id_alloc(unit, am_flags, &profile_ndx);
             if (rv != BCM_E_NONE) {
                 LOG_ERROR(BSL_LS_BCM_QOS,
                           (BSL_META_U(unit,
                                       "%s: %d is Out of Egress MPLS PHP Profile resources\n"),FUNCTION_NAME(), profile_ndx));
                 BCMDNX_IF_ERR_EXIT(rv);
             }
             BCM_INT_QOS_MAP_EGRESS_MPLS_PHP_SET(*map_id, profile_ndx);  
         }
#ifdef BCM_88660
         else if ((flags & BCM_QOS_MAP_L3_L2) && SOC_IS_ARADPLUS(unit)) {
             /* Create a QOS marking profile (each PP port may have a marking profile, */
             /* and each marking profile (along with TC/DP and InLIF) */
             /* determines the TOS/EXP marking at egress).*/
             rv = bcm_dpp_am_qos_egr_dscp_exp_marking_alloc(unit, am_flags, &profile_ndx);
             if (rv != BCM_E_NONE) {
                 LOG_ERROR(BSL_LS_BCM_QOS,
                           (BSL_META_U(unit,
                                       "%s: %d is Out of Egress DSCP/EXP Marking Profile resources\n"),FUNCTION_NAME(), profile_ndx));
                 BCMDNX_IF_ERR_EXIT(rv);
             }
             BCM_INT_QOS_MAP_EGRESS_DSCP_EXP_MARKING_SET(*map_id, profile_ndx);  

         }
#endif
         else if ((flags & BCM_QOS_MAP_OAM_PCP)) {
             if (SOC_IS_JERICHO(unit)) {
                 /* Only verify that the profile is legal and set the map.*/
                 if (profile_ndx > _BCM_OAM_OUTLIF_PROFILE_FOR_PCP_MAX_VALUE) {
                     LOG_ERROR(BSL_LS_BCM_QOS,
                               (BSL_META_U(unit,
                                           "%s: Only %d profiles available. \n"),FUNCTION_NAME(), _BCM_OAM_OUTLIF_PROFILE_FOR_PCP_MAX_VALUE ));
                     BCMDNX_IF_ERR_EXIT(BCM_E_PARAM);
                 }

                 BCM_INT_QOS_MAP_OAM_EGRESS_PROFILE_SET(*map_id, profile_ndx);
             } else {
                     LOG_ERROR(BSL_LS_BCM_QOS,
                               (BSL_META_U(unit,
                                           "%s: OAM PCP available only for Jericho and above.\n"),FUNCTION_NAME()));
                     BCMDNX_IF_ERR_EXIT(BCM_E_PARAM);
             }
         }
         else if (flags & BCM_QOS_MAP_ENCAP_INTPRI_COLOR)
         {
             if (SOC_IS_JERICHO(unit)) 
             {
                 rv = bcm_dpp_am_map_encap_intpri_color_alloc(unit, am_flags, &profile_ndx);
                 if (rv != BCM_E_NONE)
                 {
                     LOG_ERROR(BSL_LS_BCM_QOS,
                               (BSL_META_U(unit,
                                           "%s: %d is Out of COS Profile resources\n"),FUNCTION_NAME(), profile_ndx));
                     BCMDNX_IF_ERR_EXIT(rv);
                 }
             BCM_INT_QOS_MAP_ENCAP_INTPRI_COLOR_SET(*map_id, profile_ndx);  
             } 
             else
             {
                     LOG_ERROR(BSL_LS_BCM_QOS,
                               (BSL_META_U(unit,
                                           "%s: ENCAP INTRI COLOR available only for Jericho and above.\n"),FUNCTION_NAME()));
                     BCMDNX_IF_ERR_EXIT(BCM_E_PARAM);
             }
         } 
         else {
             rv = bcm_dpp_am_qos_egr_remark_id_alloc(unit,am_flags, &profile_ndx);
             if (rv != BCM_E_NONE) {
                 LOG_ERROR(BSL_LS_BCM_QOS,
                           (BSL_META_U(unit,
                                       "%s: %d is Out of Egress Remark ID resources\n"),FUNCTION_NAME(), profile_ndx));
                 BCMDNX_IF_ERR_EXIT(rv);
             }
             BCM_INT_QOS_MAP_REMARK_SET(*map_id, profile_ndx);           
         }
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_petra_qos_map_add_pcp_vlan_params_verify(int unit, uint32 flags, bcm_qos_map_t *map, int map_id){
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    if (flags & BCM_QOS_MAP_MIM_ITAG) { 
      _BCM_ERROR_IF_OUT_OF_RANGE(map->int_pri, _BCM_QOS_MAP_PCP_MAX);/*int_pri = PCP */
    } else if (flags & BCM_QOS_MAP_L2_INNER_TAG) {
        _BCM_ERROR_IF_OUT_OF_RANGE(map->int_pri, _BCM_QOS_MAP_UP_MAX);/*int_pri = UP */ 
    } else if (flags & BCM_QOS_MAP_L2_UNTAGGED) {
        _BCM_ERROR_IF_OUT_OF_RANGE(map->int_pri, _BCM_QOS_MAP_TC_MAX);/*int_pri = TC */ 
    } else if (flags & BCM_QOS_MAP_L2_OUTER_TAG) {  /* OUTER default L2 */
        _BCM_ERROR_IF_OUT_OF_RANGE(map->int_pri, _BCM_QOS_MAP_PCP_MAX);/*int_pri = PCP */
    } else  if (flags & BCM_QOS_MAP_MPLS) {
        if (BCM_INT_QOS_MAP_IS_EGRESS_PCP_VLAN(map_id)){
            _BCM_ERROR_IF_OUT_OF_RANGE(map->exp, _BCM_QOS_MAP_EXP_MAX);
        } else { /* valid only for egress editing */
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("map_id should for egress PCP VLAN"))); 
        }
    } else if (flags & BCM_QOS_MAP_L3) {
        if (BCM_INT_QOS_MAP_IS_EGRESS_PCP_VLAN(map_id)){
            _BCM_ERROR_IF_OUT_OF_RANGE(map->dscp, _BCM_QOS_MAP_DSCP_MAX);
        } else { /* valid only for egress editing */
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("map_id should for egress PCP VLAN"))); 
        }
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("one of the following flags must be set: BCM_QOS_MAP_MIM_ITAG | BCM_QOS_MAP_L2_INNER_TAG | "
                                                "BCM_QOS_MAP_L2_UNTAGGED | BCM_QOS_MAP_L2_OUTER_TAG | BCM_QOS_MAP_MPLS | BCM_QOS_MAP_L3")));
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_petra_qos_map_add_params_verify(int unit, uint32 flags, bcm_qos_map_t *map, int map_id){
    int rv = BCM_E_NONE;
    int dp = 0;
    uint32 dp_map_disabled = 0;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_ERROR_IF_OUT_OF_RANGE(map->pkt_pri, _BCM_QOS_MAP_PRI_MAX);
    _BCM_ERROR_IF_OUT_OF_RANGE(map->pkt_cfi, _BCM_QOS_MAP_CFI_MAX); 
    _BCM_ERROR_IF_OUT_OF_RANGE(map->exp,  _BCM_QOS_MAP_EXP_MAX); 
    _BCM_ERROR_IF_OUT_OF_RANGE(map->dscp, _BCM_QOS_MAP_DSCP_MAX); 

    if (!BCM_INT_QOS_MAP_ENCAP_INTPRI_COLOR(map_id))
    {
        BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_encode(unit,map->color,&dp));
    }

    if (BCM_INT_QOS_MAP_IS_INGRESS(map_id)) {
        if (BCM_INT_QOS_MAP_IS_INGRESS_PCP_VLAN(map_id)){
            _bcm_petra_qos_map_add_pcp_vlan_params_verify(unit, flags, map, map_id);
        } 
        else if (BCM_INT_QOS_MAP_IS_LIF_COS(map_id)) {
            _BCM_ERROR_IF_OUT_OF_RANGE(map->int_pri, _BCM_QOS_MAP_TC_MAX); /*int_pri = TC */

            if ((BCM_QOS_MAP_PROFILE_GET(map_id) > 0xf) && (map->remark_int_pri != 0)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Cannot configure remark_int_pri for map_id > 15")));
            }
            if (flags & BCM_QOS_MAP_L3) {
                _BCM_ERROR_IF_OUT_OF_RANGE(map->remark_int_pri, _BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX); 
            }
            else if (flags & BCM_QOS_MAP_MPLS) {
                _BCM_ERROR_IF_OUT_OF_RANGE(map->remark_int_pri, _BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX); 
            }
        } 
        else if (BCM_INT_QOS_MAP_IS_MPLS_ELSP(map_id)) {
            _BCM_ERROR_IF_OUT_OF_RANGE(map->int_pri, _BCM_QOS_MAP_EXP_MAX); /*int_pri = EXP */
        } 
        else if (BCM_INT_QOS_MAP_IS_OPCODE(map_id)) {
            _BCM_ERROR_IF_OUT_OF_RANGE(map->port_offset, _BCM_QOS_MAP_PORT_OFFSET_MAX); 
        }else if (BCM_INT_QOS_MAP_IS_INGRESS_COLOR(map_id)) {
            _BCM_ERROR_IF_OUT_OF_RANGE(map->remark_int_pri, _BCM_QOS_MAP_ING_L2_PRI_CFI_MAX); 
        _BCM_ERROR_IF_OUT_OF_RANGE(map->color, _BCM_QOS_MAP_DP_MAX); 
    }else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("map_id must be either PCP_VLAN, LIF_COS, MPLS_ELSP OAM or OPCODE")));
        } 
    }
    else if (BCM_INT_QOS_MAP_IS_EGRESS(map_id)) {
        if (BCM_INT_QOS_MAP_IS_EGRESS_PCP_VLAN(map_id)){
            _bcm_petra_qos_map_add_pcp_vlan_params_verify(unit, flags, map, map_id);
        } 
        else if (BCM_INT_QOS_MAP_IS_EGRESS_L2_I_TAG(map_id)) {
            _bcm_petra_qos_map_add_pcp_vlan_params_verify(unit, flags, map, map_id);
        }
        else if (BCM_INT_QOS_MAP_IS_REMARK(map_id)){
            if (flags & BCM_QOS_MAP_ENCAP) { /*in_dscp_exp = map->remark_int_pri, out_dscp = map->dscp, out_exp = map->exp */
                if (flags & BCM_QOS_MAP_MPLS) {
                    _BCM_ERROR_IF_OUT_OF_RANGE(map->remark_int_pri, _BCM_QOS_MAP_EXP_MAX); /* remark_int_pri = in_EXP */    
                } else if (flags & BCM_QOS_MAP_L3) {
                    _BCM_ERROR_IF_OUT_OF_RANGE(map->remark_int_pri, _BCM_QOS_MAP_DSCP_MAX); /* remark_int_pri = in_DSCP */
                } else if (flags & BCM_QOS_MAP_L2) {
                    _BCM_ERROR_IF_OUT_OF_RANGE(map->remark_int_pri, _BCM_QOS_MAP_EXP_MAX); /* remark_int_pri = in_EXP */ 
                } else {
                  BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("when map_id is REMARK and flag BCM_QOS_MAP_ENCAP is set, flag BCM_QOS_MAP_MPLS "
                                                          "or BCM_QOS_MAP_L3 or BCM_QOS_MAP_L2 must be set as well")));
                }
            }
            else { /* Not encap header*/
                if (flags & BCM_QOS_MAP_MPLS) { /*in_dscp_exp = int_pri, out_dscp_exp = remark_int_pri*/
                    _BCM_ERROR_IF_OUT_OF_RANGE(map->int_pri, _BCM_QOS_MAP_EXP_MAX);        /*int_pri = in_EXP */
                    _BCM_ERROR_IF_OUT_OF_RANGE(map->remark_int_pri, _BCM_QOS_MAP_EXP_MAX); /*remark_int_pri = out_EXP */ 
                } else if (flags & BCM_QOS_MAP_L3) {
                    _BCM_ERROR_IF_OUT_OF_RANGE(map->int_pri, _BCM_QOS_MAP_DSCP_MAX);        /*int_pri = in_DSCP */
                    _BCM_ERROR_IF_OUT_OF_RANGE(map->remark_int_pri, _BCM_QOS_MAP_DSCP_MAX); /*remark_int_pri = out_DSCP */ 
                }
            }
        }
        else if (BCM_INT_QOS_MAP_IS_EGRESS_MPLS_PHP(map_id)){
          _BCM_ERROR_IF_OUT_OF_RANGE(map->int_pri, _BCM_QOS_MAP_EXP_MAX);        /*int_pri = in_EXP */
          _BCM_ERROR_IF_OUT_OF_RANGE(map->remark_int_pri, _BCM_QOS_MAP_DSCP_MAX); /*remark_int_pri = out_DSCP */               
          if (!((flags & BCM_QOS_MAP_IPV6) || (flags & BCM_QOS_MAP_IPV4))) {
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("when map_id is MPLS PHP one of the following flags are supported BCM_QOS_MAP_IPV6, BCM_QOS_MAP_L3.")));
          }
        }
#ifdef BCM_88660
        else if (SOC_IS_ARADPLUS(unit) && BCM_INT_QOS_MAP_IS_EGRESS_DSCP_EXP_MARKING(map_id)){
            /** Get the dp map mode, 0: mapped, 1: disable mapping.*/
            if (SOC_IS_JERICHO_B0(unit) || SOC_IS_QMX_B0(unit)) {
                SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_read(unit, EPNI_DP_MAP_FIX_ENABLEDr, SOC_CORE_ALL, 0, DP_MAP_FIX_ENABLEDf, &dp_map_disabled));
            } else if (SOC_IS_JERICHO_PLUS(unit)) {
                SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_read(unit, EPNI_CFG_MARKING_DP_MAPr, SOC_CORE_ALL, 0, CFG_MARKING_DP_MAP_DISABLEf, &dp_map_disabled));
            }

            if (dp_map_disabled) {
                _BCM_ERROR_IF_OUT_OF_RANGE(map->remark_color, _BCM_QOS_MAP_DP_MAX); /*remark_int_pri = dp */
            } else {
                _BCM_ERROR_IF_OUT_OF_RANGE(map->remark_color, _BCM_QOS_MAP_EGR_RESOLVED_DP_MAX); /*remark_int_pri = resolved-dp */
            }

            _BCM_ERROR_IF_OUT_OF_RANGE(map->int_pri, _BCM_QOS_MAP_TC_MAX);        /*int_pri = Egress-TC */
            _BCM_ERROR_IF_OUT_OF_RANGE(map->port_offset, SOC_OCC_MGMT_NOF_INLIF_PROFILES_TRANSFERED_TO_EGRESS); /*remark_int_pri = inlif profile */
            _BCM_ERROR_IF_OUT_OF_RANGE(map->dscp, _BCM_QOS_MAP_DSCP_MAX);
            _BCM_ERROR_IF_OUT_OF_RANGE(map->exp, _BCM_QOS_MAP_EXP_MAX);

            if (flags) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No flags are supported for DSCP/EXP marking.")));
            }
        }
#endif /* BCM_88660 */
        else if (BCM_INT_QOS_MAP_IS_OAM_EGRESS_PROFILE(map_id)) {
            _BCM_ERROR_IF_OUT_OF_RANGE(map->int_pri, _BCM_QOS_MAP_TC_MAX);
            _BCM_ERROR_IF_OUT_OF_RANGE(map->pkt_pri, _BCM_QOS_MAP_PCP_MAX); /* OAM PCP.*/

        } 
        else if (BCM_INT_QOS_MAP_ENCAP_INTPRI_COLOR(map_id))
        {
            if ( map->color != bcmColorPreserve)
            {
                 BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_encode(unit,map->color,&dp));
            }
            if ( map->int_pri != BCM_PRIO_PRESERVE)
            {
                 _BCM_ERROR_IF_OUT_OF_RANGE(map->int_pri, _BCM_QOS_MAP_TC_MAX);
            }
        } 
        else 
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("map_id must be either PCP_VLAN, L2_I_TAG, REMARK or ENCAP_INTRI_COLOR")));
        }
    }
    else { 
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("map_id must be either INGRESS or EGRESS")));
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


/* Destroy a QoS map profile */
int 
bcm_petra_qos_map_destroy(int unit, int map_id)
{
    int sand_rv, rv = BCM_E_NONE;
    int profile_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);
    bcm_qos_map_t qos_map;
    int idx = 0, flags = 0;
    int int_pri = 0, int_cfi = 0, color = 0;
    int dscp_cnt = 0, tc_cnt = 0, dp_cnt = 0, pri_cfi_cnt = 0;
    int remark_int_pri = 0;
    int in_dscp = 0, in_exp = 0;
    int dev_id, dp = 0;
#ifdef BCM_88660
    int resolved_dp_idx, tc_idx, inlif_idx;
#endif /* BCM_88660 */
    SOC_PPC_LIF_COS_OPCODE_TYPE opcode_type;
    bcm_color_t tmp_color;
    uint8 bit_val;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    dev_id = (unit);
    if (BCM_INT_QOS_MAP_IS_INGRESS_PCP_VLAN(map_id)) {
    
    /* Destroy all STAG */
    bcm_qos_map_t_init(&qos_map);
    for (idx=0; idx < _BCM_QOS_MAP_ING_VLAN_PCP_STAG_MAX; idx++) {
        qos_map.int_pri = (idx >> 1);
        BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_decode(unit, (idx & 1), &(qos_map.color)));
        flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_VLAN_PCP | BCM_QOS_MAP_L2_OUTER_TAG;
        rv = QOS_ACCESS.ing_pcp_vlan_stag_bitmap.bit_get(unit,
            _DPP_QOS_ENTRY_INDEX_ING_PCP_VLAN_STAG(profile_ndx, qos_map.int_pri, qos_map.color),
            &bit_val);
        BCMDNX_IF_ERR_EXIT(rv);
        if (!bit_val) {
            continue;
        }
        rv = bcm_petra_qos_map_delete(unit, flags, &qos_map, map_id);
        if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: Vlan PCP Outer Tag Pri %d Dp %d reset %d failed\n"),FUNCTION_NAME(), (idx>>1), (idx & 1), profile_ndx));
        BCMDNX_IF_ERR_EXIT(rv);
        }
    }
    
    /* Destroy all CTAG */
    bcm_qos_map_t_init(&qos_map);
    for (idx=0; idx < _BCM_QOS_MAP_ING_VLAN_PCP_CTAG_MAX; idx++) {
        qos_map.int_pri = idx;
        flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_VLAN_PCP | BCM_QOS_MAP_L2_INNER_TAG;
        rv = QOS_ACCESS.ing_pcp_vlan_ctag_bitmap.bit_get(unit,
            _DPP_QOS_ENTRY_INDEX_ING_PCP_VLAN_CTAG(profile_ndx, qos_map.int_pri),
            &bit_val);
        BCMDNX_IF_ERR_EXIT(rv);
        if (!bit_val) {
            continue;
        }
        rv = bcm_petra_qos_map_delete(unit, flags, &qos_map, map_id);
        if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: Vlan PCP Inner Tag Pri %d reset %d failed\n"),FUNCTION_NAME(), idx, profile_ndx));
        BCMDNX_IF_ERR_EXIT(rv);
        }
    }
    
    /* Destroy all UNTAGGED */
    bcm_qos_map_t_init(&qos_map);
    for (idx=0; idx < _BCM_QOS_MAP_ING_VLAN_PCP_UTAG_MAX; idx++) {
        qos_map.int_pri = (idx >> 2);
        BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_decode(unit, (idx % 4), &(qos_map.color)));
        flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_VLAN_PCP | BCM_QOS_MAP_L2_UNTAGGED;
        rv = QOS_ACCESS.ing_pcp_vlan_utag_bitmap.bit_get(unit,
            _DPP_QOS_ENTRY_INDEX_ING_PCP_VLAN_UTAG(profile_ndx, qos_map.int_pri, qos_map.color),
            &bit_val);
        BCMDNX_IF_ERR_EXIT(rv);
        if (!bit_val) {
            continue;
        }
        rv = bcm_petra_qos_map_delete(unit, flags, &qos_map, map_id);
        if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: Vlan PCP Untagged Pri %d Dp %d reset %d failed\n"),FUNCTION_NAME(), (idx>>2), (idx % 4), profile_ndx));
        BCMDNX_IF_ERR_EXIT(rv);
        }
    }
    
    rv = bcm_dpp_am_qos_ing_pcp_vlan_dealloc(unit, 0, profile_ndx);
    if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: PCP Profile  Resource %d not allocated\n"),FUNCTION_NAME(), profile_ndx));
        BCMDNX_IF_ERR_EXIT(rv);
    }
    } else if (BCM_INT_QOS_MAP_IS_LIF_COS(map_id)) {
    
    /* Destroy all MPLS */
    bcm_qos_map_t_init(&qos_map);
    for (idx = 0; idx < _BCM_QOS_MAP_ING_MPLS_EXP_MAX; idx++) {
        qos_map.exp = idx;
        rv = QOS_ACCESS.ing_lif_cos_mpls_bitmap.bit_get(unit,
            _DPP_QOS_ENTRY_INDEX_ING_LIF_COS_MPLS(profile_ndx, qos_map.exp),
            &bit_val);
        BCMDNX_IF_ERR_EXIT(rv);
        if (!bit_val) {
            continue;
        }
        rv = bcm_petra_qos_map_delete(unit, BCM_QOS_MAP_MPLS, &qos_map, map_id);
        if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: Lif/Cos MPLS %d reset %d failed\n"),FUNCTION_NAME(), idx, profile_ndx));
        BCMDNX_IF_ERR_EXIT(rv);
        }
    }
    
    /* Destroy all L3 */
    bcm_qos_map_t_init(&qos_map);
    for (idx = 0; idx < _BCM_QOS_MAP_ING_L3_DSCP_MAX; idx++) {
        qos_map.dscp = idx;
        rv = QOS_ACCESS.ing_lif_cos_ipv4_bitmap.bit_get(unit,
            _DPP_QOS_ENTRY_INDEX_ING_LIF_COS_L3(profile_ndx, qos_map.dscp),
            &bit_val);
        BCMDNX_IF_ERR_EXIT(rv);
        if (bit_val) {

            rv = bcm_petra_qos_map_delete(unit, BCM_QOS_MAP_L3|BCM_QOS_MAP_IPV4, &qos_map, map_id);
            if (rv != BCM_E_NONE) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: Lif/Cos L3 IPV4 %d reset %d failed\n"),FUNCTION_NAME(), idx, profile_ndx));
            BCMDNX_IF_ERR_EXIT(rv);
            }
        }

        rv = QOS_ACCESS.ing_lif_cos_ipv6_bitmap.bit_get(unit,
           _DPP_QOS_ENTRY_INDEX_ING_LIF_COS_L3(profile_ndx, qos_map.dscp),
           &bit_val);
        BCMDNX_IF_ERR_EXIT(rv);
        if (bit_val) {

            rv = bcm_petra_qos_map_delete(unit, BCM_QOS_MAP_L3|BCM_QOS_MAP_IPV6, &qos_map, map_id);
            if (rv != BCM_E_NONE) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: Lif/Cos L3 IPV4 %d reset %d failed\n"),FUNCTION_NAME(), idx, profile_ndx));
            BCMDNX_IF_ERR_EXIT(rv);
            }
        }
    }

    /* Destroy all L2 */
    bcm_qos_map_t_init(&qos_map);
    for (idx = 0; idx < _BCM_QOS_MAP_ING_L2_PRI_CFI_MAX; idx++) {
        qos_map.pkt_cfi = idx & 1;
        qos_map.pkt_pri = idx >> 1;

        rv = QOS_ACCESS.ing_lif_cos_l2_outer_bitmap.bit_get(unit,
           _DPP_QOS_ENTRY_INDEX_ING_LIF_COS_L2_OUTER(profile_ndx, qos_map.pkt_pri, qos_map.pkt_cfi),
           &bit_val);
        BCMDNX_IF_ERR_EXIT(rv);
        if (bit_val) {

            rv = bcm_petra_qos_map_delete(unit, BCM_QOS_MAP_L2|BCM_QOS_MAP_L2_OUTER_TAG, &qos_map, map_id);
            if (rv != BCM_E_NONE) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: Lif/Cos L2 OUTER TAG %d reset %d failed\n"),FUNCTION_NAME(), idx, profile_ndx));
            BCMDNX_IF_ERR_EXIT(rv);
            }
        }

        rv = QOS_ACCESS.ing_lif_cos_l2_inner_bitmap.bit_get(unit,
           _DPP_QOS_ENTRY_INDEX_ING_LIF_COS_L2_INNER(profile_ndx, qos_map.pkt_pri, qos_map.pkt_cfi),
           &bit_val);
        BCMDNX_IF_ERR_EXIT(rv);
        if (bit_val) {

            rv = bcm_petra_qos_map_delete(unit, BCM_QOS_MAP_L2|BCM_QOS_MAP_L2_INNER_TAG, &qos_map, map_id);
            if (rv != BCM_E_NONE) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: Lif/Cos L2 INNER TAG %d reset %d failed\n"),FUNCTION_NAME(), idx, profile_ndx));
            BCMDNX_IF_ERR_EXIT(rv);
            }
        }
    }

    /* Destroy all UNTAGGED */
    bcm_qos_map_t_init(&qos_map);
    for (idx = 0; idx < _BCM_QOS_MAP_ING_L2_PRI_CFI_MAX; idx++) {
        qos_map.pkt_pri = idx >> 1;
        for (dp=0; dp < SOC_DPP_CONFIG(unit)->qos.dp_max; dp++) {
        BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_decode(unit, dp, &tmp_color));
        qos_map.pkt_cfi = tmp_color;
        flags = BCM_QOS_MAP_L2_UNTAGGED;

        rv = QOS_ACCESS.ing_lif_cos_l2_untag_bitmap.bit_get(unit,
           _DPP_QOS_ENTRY_INDEX_ING_LIF_COS_L2_UNTAG(profile_ndx, qos_map.pkt_pri, qos_map.pkt_cfi),
           &bit_val);
        BCMDNX_IF_ERR_EXIT(rv);
        if (!bit_val) {
            continue;
        }
        rv = bcm_petra_qos_map_delete(unit, flags, &qos_map, map_id);
        if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: Lif/Cos L2 UNTAGGED Pri %d Dp %d reset %d failed\n"),FUNCTION_NAME(), idx>>1, dp, profile_ndx));
        BCMDNX_IF_ERR_EXIT(rv);
        }
        }
    }
        
    rv = bcm_dpp_am_qos_ing_lif_cos_dealloc(unit, 0,  profile_ndx);
    if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: Lif/Cos Resources %d not allocated\n"),FUNCTION_NAME(), profile_ndx));
        BCMDNX_IF_ERR_EXIT(rv);
    }
    } else if (BCM_INT_QOS_MAP_IS_EGRESS_PCP_VLAN(map_id)) {

    /* Destroy all STAG */
    bcm_qos_map_t_init(&qos_map);
    for (int_pri=0; int_pri < _BCM_QOS_MAP_EGR_VLAN_PRI_MAX; int_pri++) {
        qos_map.int_pri = int_pri;
        for (int_cfi=0; int_cfi < _BCM_QOS_MAP_EGR_VLAN_CFI_MAX; int_cfi++) {
        qos_map.color = int_cfi;
        flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_VLAN_PCP | BCM_QOS_MAP_L2_OUTER_TAG;

        rv = QOS_ACCESS.egr_pcp_vlan_stag_bitmap.bit_get(unit,
           _DPP_QOS_ENTRY_INDEX_EGR_PCP_VLAN_STAG(profile_ndx, qos_map.int_pri, qos_map.color),
           &bit_val);
        BCMDNX_IF_ERR_EXIT(rv);
        if (!bit_val) {
            continue;
        }
        rv = bcm_petra_qos_map_delete(unit, flags, &qos_map, map_id);
        if (rv != BCM_E_NONE) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: Egress Vlan PCP Outer Tag PCP %d DEI %d reset %d failed\n"),FUNCTION_NAME(), int_pri, dp, profile_ndx));
            BCMDNX_IF_ERR_EXIT(rv);
        }
        }
    }
    /* Destroy all CTAG */
    bcm_qos_map_t_init(&qos_map);
    for (int_pri=0; int_pri < _BCM_QOS_MAP_EGR_VLAN_PRI_MAX; int_pri++) {
        qos_map.int_pri = int_pri;
        flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_VLAN_PCP | BCM_QOS_MAP_L2_INNER_TAG;

        rv = QOS_ACCESS.egr_pcp_vlan_ctag_bitmap.bit_get(unit,
           _DPP_QOS_ENTRY_INDEX_EGR_PCP_VLAN_CTAG(profile_ndx, qos_map.int_pri),
           &bit_val);
        BCMDNX_IF_ERR_EXIT(rv);
        if (!bit_val) {
            continue;
        }
        rv = bcm_petra_qos_map_delete(unit, flags, &qos_map, map_id);
        if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: Egress Vlan PCP Inner Tag Pri %d reset %d failed\n"),FUNCTION_NAME(), int_pri, profile_ndx));
        BCMDNX_IF_ERR_EXIT(rv);
        }
    }
    
    /* Destroy all UNTAGGED */
    bcm_qos_map_t_init(&qos_map);
    for (int_pri=0; int_pri < _BCM_QOS_MAP_EGR_VLAN_PRI_MAX; int_pri++) {
        qos_map.int_pri = int_pri;
        for (dp=0; dp < SOC_DPP_CONFIG(unit)->qos.dp_max; dp++) {
        BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_decode(unit, dp, &(qos_map.color)));
        flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_VLAN_PCP | BCM_QOS_MAP_L2_UNTAGGED;

        rv = QOS_ACCESS.egr_pcp_vlan_utag_bitmap.bit_get(unit,
           _DPP_QOS_ENTRY_INDEX_EGR_PCP_VLAN_UTAG(profile_ndx, qos_map.int_pri, qos_map.color),
           &bit_val);
        BCMDNX_IF_ERR_EXIT(rv);
        if (!bit_val) {
            continue;
        }
        rv = bcm_petra_qos_map_delete(unit, flags, &qos_map, map_id);
        if (rv != BCM_E_NONE) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: Vlan PCP Untagged Pri %d Dp %d reset %d failed\n"),FUNCTION_NAME(), int_pri, dp, profile_ndx));
            BCMDNX_IF_ERR_EXIT(rv);
        }
        }
    }
    
    if ( profile_ndx == SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_profile_id ) {
        /* Destroy all L3 IPV4 */ 
        bcm_qos_map_t_init(&qos_map);
        for (in_dscp=0; in_dscp < _BCM_QOS_MAP_EGR_DSCP_MAX; in_dscp++) {
        qos_map.dscp = in_dscp;
        flags = BCM_QOS_MAP_L2_VLAN_PCP | BCM_QOS_MAP_ENCAP | BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4; 

        rv = QOS_ACCESS.egr_pcp_vlan_ipv4_bitmap.bit_get(unit,
           _DPP_QOS_ENTRY_INDEX_EGR_PCP_VLAN_L3(profile_ndx, qos_map.dscp),
           &bit_val);
        BCMDNX_IF_ERR_EXIT(rv);
        if (!bit_val) {
            continue;
        }
        rv = bcm_petra_qos_map_delete(unit, flags, &qos_map, map_id);
        if (rv != BCM_E_NONE) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: L3 IPV4 Dscp %d reset %d failed\n"),FUNCTION_NAME(), in_dscp, profile_ndx));
            BCMDNX_IF_ERR_EXIT(rv);
        }
        }
        
        /* Destroy all L3 IPV6 */ 
        bcm_qos_map_t_init(&qos_map);
        for (in_dscp=0; in_dscp < _BCM_QOS_MAP_EGR_DSCP_MAX; in_dscp++) {
        qos_map.dscp = in_dscp;
        flags = BCM_QOS_MAP_L2_VLAN_PCP | BCM_QOS_MAP_ENCAP | BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV6; 

        rv = QOS_ACCESS.egr_pcp_vlan_ipv6_bitmap.bit_get(unit,
           _DPP_QOS_ENTRY_INDEX_EGR_PCP_VLAN_L3(profile_ndx, qos_map.dscp),
           &bit_val);
        BCMDNX_IF_ERR_EXIT(rv);
        if (!bit_val) {
            continue;
        }
        rv = bcm_petra_qos_map_delete(unit, flags, &qos_map, map_id);
        if (rv != BCM_E_NONE) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: L3 IPV6 Dscp %d reset %d failed\n"),FUNCTION_NAME(), in_dscp, profile_ndx));
            BCMDNX_IF_ERR_EXIT(rv);
        }
        }
        
        /* Destroy all MPLS */ 
        bcm_qos_map_t_init(&qos_map);
        for (in_exp=0; in_exp < _BCM_QOS_MAP_EGR_EXP_MAX; in_exp++) {
        qos_map.exp = in_exp;
        flags = BCM_QOS_MAP_L2_VLAN_PCP | BCM_QOS_MAP_ENCAP | BCM_QOS_MAP_MPLS;

        rv = QOS_ACCESS.egr_pcp_vlan_mpls_bitmap.bit_get(unit,
           _DPP_QOS_ENTRY_INDEX_EGR_PCP_VLAN_MPLS(profile_ndx, qos_map.exp),
           &bit_val);
        BCMDNX_IF_ERR_EXIT(rv);
        if (!bit_val) {
            continue;
        }
        rv = bcm_petra_qos_map_delete(unit, flags, &qos_map, map_id);
        if (rv != BCM_E_NONE) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: MPLS Exp %d reset %d failed\n"),FUNCTION_NAME(), int_pri, profile_ndx));
            BCMDNX_IF_ERR_EXIT(rv);
        }
        }
        
    }
    rv = bcm_dpp_am_qos_egr_pcp_vlan_dealloc(unit, 0, profile_ndx);
    if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: PCP Profile Resource %d not allocated\n"),FUNCTION_NAME(),profile_ndx));
        BCMDNX_IF_ERR_EXIT(rv);
    }

    } else if (BCM_INT_QOS_MAP_IS_EGRESS_L2_I_TAG(map_id)) {

        bcm_qos_map_t_init(&qos_map);
        for (int_pri=0; int_pri < _BCM_QOS_MAP_EGR_VLAN_PRI_MAX; int_pri++) {
            qos_map.int_pri = int_pri;
            for (dp=0; dp < SOC_DPP_CONFIG(unit)->qos.dp_max; dp++) {
            BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_decode(unit, dp, &(qos_map.color)));
            flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_MIM_ITAG;

            rv = QOS_ACCESS.egr_l2_i_tag_bitmap.bit_get(unit,
               _DPP_QOS_ENTRY_INDEX_EGR_L2_I_TAG(profile_ndx, qos_map.int_pri, qos_map.color),
               &bit_val);
            BCMDNX_IF_ERR_EXIT(rv);
            if (!bit_val) {
                continue;
            }
            rv = bcm_petra_qos_map_delete(unit, flags, &qos_map, map_id);
            if (rv != BCM_E_NONE) {
                LOG_ERROR(BSL_LS_BCM_QOS,
                          (BSL_META_U(unit,
                                      "%s: Egress L2 I TAG PCP %d DEI %d reset %d failed\n"),FUNCTION_NAME(), int_pri, dp, profile_ndx));
                BCMDNX_IF_ERR_EXIT(rv);
            }
            }
        }
        rv = bcm_dpp_am_qos_egr_l2_i_tag_dealloc(unit, 0, profile_ndx);
        if (rv != BCM_E_NONE) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: Egress L2 I TAG %d not allocated\n"),FUNCTION_NAME(),profile_ndx));
            BCMDNX_IF_ERR_EXIT(rv);
        }
#ifdef BCM_88660
    } else if (SOC_IS_ARADPLUS(unit) && BCM_INT_QOS_MAP_IS_EGRESS_DSCP_EXP_MARKING(map_id)) {

        if (bcm_dpp_am_qos_egr_dscp_exp_marking_is_alloced(unit, profile_ndx) != BCM_E_EXISTS)
        {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: Egress DSCP/EXP marking profile %d not allocated\n"),FUNCTION_NAME(),profile_ndx));
            BCMDNX_IF_ERR_EXIT(rv);
        }

        bcm_qos_map_t_init(&qos_map);

        /* Reset all lines with profile profile_ndx */
        for (resolved_dp_idx = 0; resolved_dp_idx < _BCM_QOS_MAP_EGR_RESOLVED_DP_MAX; resolved_dp_idx++) {
            for (tc_idx = 0; tc_idx < _BCM_QOS_MAP_TC_MAX; tc_idx++) {
                for (inlif_idx = 0; inlif_idx < SOC_OCC_MGMT_NOF_INLIF_PROFILES_TRANSFERED_TO_EGRESS; inlif_idx++) {
                    qos_map.int_pri = tc_idx;
                    qos_map.remark_color = resolved_dp_idx;
                    qos_map.port_offset = inlif_idx;
                    rv = QOS_ACCESS.egr_dscp_exp_marking_bitmap.bit_get(unit,
                       _DPP_QOS_ENTRY_INDEX_EGR_DSCP_EXP_MARKING(profile_ndx, qos_map.port_offset, qos_map.int_pri, qos_map.remark_color),
                       &bit_val);
                    BCMDNX_IF_ERR_EXIT(rv);
                    if (!bit_val) {
                        continue;
                    }
                    rv = bcm_petra_qos_map_delete(unit, flags, &qos_map, map_id);
                    if (rv != BCM_E_NONE) {
                        LOG_ERROR(BSL_LS_BCM_QOS,
                                  (BSL_META_U(unit,
                                              "%s: Egress DSCP/EXP marking profile %d with tc %d, resolved_dp %d, inlif %d reset failed\n"),FUNCTION_NAME(), profile_ndx, tc_idx, resolved_dp_idx, inlif_idx));
                        BCMDNX_IF_ERR_EXIT(rv);
                    }
                }
            }
        }

        rv = bcm_dpp_am_qos_egr_dscp_exp_marking_dealloc(unit, 0, profile_ndx);
        if (rv != BCM_E_NONE) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: Egress DSCP/EXP marking profile %d not allocated\n"),FUNCTION_NAME(),profile_ndx));
            BCMDNX_IF_ERR_EXIT(rv);
        }

#endif /* BCM_88660 */
    } else if (BCM_INT_QOS_MAP_IS_REMARK(map_id)) {
        if (BCM_INT_QOS_MAP_IS_ENCAP(map_id)) {

           /* Destroy all MPLS */
           for (remark_int_pri = 0; remark_int_pri < _BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX; remark_int_pri++) { 
             bcm_qos_map_t_init(&qos_map);
             qos_map.remark_int_pri = remark_int_pri;
             rv = QOS_ACCESS.egr_remark_encap_mpls_bitmap.bit_get(unit,
                _DPP_QOS_ENTRY_INDEX_EGR_REMARK_ENP_MPLS(profile_ndx, qos_map.remark_int_pri),
                &bit_val);
             BCMDNX_IF_ERR_EXIT(rv);
             if (!bit_val) {
                 continue;
             }
             rv = bcm_petra_qos_map_delete(unit, BCM_QOS_MAP_ENCAP | BCM_QOS_MAP_MPLS, &qos_map, map_id);
             if (rv != BCM_E_NONE) {
                 LOG_ERROR(BSL_LS_BCM_QOS,
                           (BSL_META_U(unit,
                                       "%s: Egress Remark MPLS %d reset %d failed\n"),FUNCTION_NAME(), idx, profile_ndx));
                 BCMDNX_IF_ERR_EXIT(rv);
             }
           }
           
           /* Destroy all L3 IPV6 */ 
           for (remark_int_pri = 0; remark_int_pri < _BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX; remark_int_pri++) { 
             bcm_qos_map_t_init(&qos_map);
             qos_map.remark_int_pri = remark_int_pri;
             rv = QOS_ACCESS.egr_remark_encap_ipv4_bitmap.bit_get(unit,
                _DPP_QOS_ENTRY_INDEX_EGR_REMARK_ENP_L3(profile_ndx, qos_map.remark_int_pri),
                &bit_val);
             BCMDNX_IF_ERR_EXIT(rv);
             if (!bit_val) {
                 continue;
             }
             rv = bcm_petra_qos_map_delete(unit, BCM_QOS_MAP_ENCAP | BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV6, &qos_map, map_id);
             if (rv != BCM_E_NONE) {
                 LOG_ERROR(BSL_LS_BCM_QOS,
                           (BSL_META_U(unit,
                                       "%s: Egress Remark L3 IPV6 %d reset %d failed\n"),FUNCTION_NAME(), idx, profile_ndx));
                 BCMDNX_IF_ERR_EXIT(rv);
             }
           }
           /* Destroy all L3 IPV4 */ 
           for (remark_int_pri = 0; remark_int_pri < _BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX; remark_int_pri++) { 
             bcm_qos_map_t_init(&qos_map);
             qos_map.remark_int_pri = remark_int_pri;
             rv = QOS_ACCESS.egr_remark_encap_ipv6_bitmap.bit_get(unit,
                _DPP_QOS_ENTRY_INDEX_EGR_REMARK_ENP_L3(profile_ndx, qos_map.remark_int_pri),
                &bit_val);
             BCMDNX_IF_ERR_EXIT(rv);
             if (!bit_val) {
                 continue;
             }
             rv = bcm_petra_qos_map_delete(unit, BCM_QOS_MAP_ENCAP | BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV6, &qos_map, map_id);
             if (rv != BCM_E_NONE) {
                 LOG_ERROR(BSL_LS_BCM_QOS,
                           (BSL_META_U(unit,
                                       "%s: Egress Remark L3 IPV4 %d reset %d failed\n"),FUNCTION_NAME(), idx, profile_ndx));
                 BCMDNX_IF_ERR_EXIT(rv);
             }
           }
         
      
        } else { /* Not encap header */
            /* Destroy all MPLS */
           for (color = 0; color < SOC_DPP_CONFIG(unit)->qos.dp_max; color++) {
               for (int_pri = 0; int_pri < _BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX; int_pri++) {
             bcm_qos_map_t_init(&qos_map);
             qos_map.color = color;
             qos_map.int_pri = int_pri;
             rv = QOS_ACCESS.egr_remark_mpls_bitmap.bit_get(unit,
                _DPP_QOS_ENTRY_INDEX_EGR_REMARK_MPLS(profile_ndx, qos_map.int_pri, qos_map.color),
                &bit_val);
             BCMDNX_IF_ERR_EXIT(rv);
             if (!bit_val) {
                 continue;
             }
             rv = bcm_petra_qos_map_delete(unit, BCM_QOS_MAP_MPLS, &qos_map, map_id);
             if (rv != BCM_E_NONE) {
                 LOG_ERROR(BSL_LS_BCM_QOS,
                           (BSL_META_U(unit,
                                       "%s: Egress Remark MPLS %d reset %d failed\n"),FUNCTION_NAME(), idx, profile_ndx));
                 BCMDNX_IF_ERR_EXIT(rv);
             }
               }
           }

           /* Destroy all L3 */ 
           for (color = 0; color < SOC_DPP_CONFIG(unit)->qos.dp_max; color++) {
               for (int_pri = 0; int_pri < _BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX; int_pri++) {
             bcm_qos_map_t_init(&qos_map);
             qos_map.color = color;
             qos_map.int_pri = int_pri;
             rv = QOS_ACCESS.egr_remark_ipv6_bitmap.bit_get(unit,
                _DPP_QOS_ENTRY_INDEX_EGR_REMARK_L3(profile_ndx, qos_map.int_pri, qos_map.color),
                &bit_val);
             BCMDNX_IF_ERR_EXIT(rv);
             if (!bit_val) {
                 continue;
             }
             rv = bcm_petra_qos_map_delete(unit, BCM_QOS_MAP_L3|BCM_QOS_MAP_IPV6, &qos_map, map_id);
             if (rv != BCM_E_NONE) {
                 LOG_ERROR(BSL_LS_BCM_QOS,
                           (BSL_META_U(unit,
                                       "%s: Egress Remark L3 %d reset %d failed\n"),FUNCTION_NAME(), idx, profile_ndx));
                 BCMDNX_IF_ERR_EXIT(rv);
             }
               }
           }

           /* Destroy all L3 IPV4 */ 
           for (color = 0; color < SOC_DPP_CONFIG(unit)->qos.dp_max; color++) {
               for (int_pri = 0; int_pri < _BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX; int_pri++) {
             bcm_qos_map_t_init(&qos_map);
             qos_map.color = color;
             qos_map.int_pri = int_pri;
             rv = QOS_ACCESS.egr_remark_ipv4_bitmap.bit_get(unit,
                _DPP_QOS_ENTRY_INDEX_EGR_REMARK_L3(profile_ndx, qos_map.int_pri, qos_map.color),
                &bit_val);
             BCMDNX_IF_ERR_EXIT(rv);
             if (!bit_val) {
                 continue;
             }
             rv = bcm_petra_qos_map_delete(unit, BCM_QOS_MAP_L3, &qos_map, map_id);
             if (rv != BCM_E_NONE) {
                 LOG_ERROR(BSL_LS_BCM_QOS,
                           (BSL_META_U(unit,
                                       "%s: Egress Remark L3 %d reset %d failed\n"),FUNCTION_NAME(), idx, profile_ndx));
                 BCMDNX_IF_ERR_EXIT(rv);
             }
               }
           }
        }
        rv = bcm_dpp_am_qos_egr_remark_id_dealloc(unit,0, profile_ndx);
        if (rv != BCM_E_NONE) {
          LOG_ERROR(BSL_LS_BCM_QOS,
                    (BSL_META_U(unit,
                                "%s: Remark Profile Resource %d not allocated\n"),FUNCTION_NAME(),profile_ndx));
          BCMDNX_IF_ERR_EXIT(rv);
        }
    } else if (BCM_INT_QOS_MAP_IS_MPLS_ELSP(map_id)) {

    /* Destroy all MPLS */
    bcm_qos_map_t_init(&qos_map);
    for (idx = 0; idx < _BCM_QOS_MAP_ING_MPLS_EXP_MAX; idx++) {
        qos_map.exp = idx;
        rv = bcm_petra_qos_map_delete(unit, BCM_QOS_MAP_MPLS_ELSP, &qos_map, map_id);
        if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: MPLS ELSP %d reset %d failed\n"),FUNCTION_NAME(), idx, profile_ndx));
        BCMDNX_IF_ERR_EXIT(rv);
        }
    }

    rv = bcm_dpp_am_qos_ing_elsp_dealloc(unit, 0,  profile_ndx);
    if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: MPLS ELSP Resources %d not allocated\n"),FUNCTION_NAME(), profile_ndx));
        BCMDNX_IF_ERR_EXIT(rv);
    }

    } else if (BCM_INT_QOS_MAP_IS_EGRESS_MPLS_PHP(map_id)) {
        /* Destroy all MPLS */
        bcm_qos_map_t_init(&qos_map);
        for (idx = 0; idx < _BCM_QOS_MAP_EGR_EXP_MAX; idx++) {
            qos_map.int_pri = idx;
            rv = QOS_ACCESS.egr_mpls_php_ipv4_bitmap.bit_get(unit,
                _DPP_QOS_ENTRY_INDEX_EGR_MPLS_PHP(profile_ndx, qos_map.int_pri),
                &bit_val); 
            BCMDNX_IF_ERR_EXIT(rv);
            if (bit_val) {

                rv = bcm_petra_qos_map_delete(unit, BCM_QOS_MAP_IPV4, &qos_map, map_id);
                if (rv != BCM_E_NONE) {
                    LOG_ERROR(BSL_LS_BCM_QOS,
                              (BSL_META_U(unit,
                                          "%s: EGRESS MPLS PHP %d reset %d failed\n"),FUNCTION_NAME(), idx, profile_ndx));
                    BCMDNX_IF_ERR_EXIT(rv);
                }
            }

            rv = QOS_ACCESS.egr_mpls_php_ipv6_bitmap.bit_get(unit,
                _DPP_QOS_ENTRY_INDEX_EGR_MPLS_PHP(profile_ndx, qos_map.int_pri),
                &bit_val); 
            BCMDNX_IF_ERR_EXIT(rv);
            if (bit_val) {

                rv = bcm_petra_qos_map_delete(unit, BCM_QOS_MAP_IPV6, &qos_map, map_id);
                if (rv != BCM_E_NONE) {
                    LOG_ERROR(BSL_LS_BCM_QOS,
                              (BSL_META_U(unit,
                                          "%s: EGRESS MPLS PHP %d reset %d failed\n"),FUNCTION_NAME(), idx, profile_ndx));
                    BCMDNX_IF_ERR_EXIT(rv);
                }
            }
        }

        rv = bcm_dpp_am_qos_egr_mpls_php_id_dealloc(unit, 0,  profile_ndx);
        if (rv != BCM_E_NONE) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: MPLS MPLS PHP Resources %d not allocated\n"),FUNCTION_NAME(), profile_ndx));
            BCMDNX_IF_ERR_EXIT(rv);
        }
    } else if (BCM_INT_QOS_MAP_IS_OPCODE(map_id)) {

        sand_rv = soc_ppd_lif_cos_opcode_types_get(dev_id, profile_ndx, &opcode_type);
        BCM_SAND_IF_ERR_EXIT(sand_rv);

        if (opcode_type & SOC_PPC_LIF_COS_OPCODE_TYPE_L3) {
            /* Destroy all IPV4 */
            for (dscp_cnt = 0; dscp_cnt < _BCM_QOS_MAP_ING_COS_OPCODE_L3_MAX; dscp_cnt++) {
                bcm_qos_map_t_init(&qos_map);
                qos_map.dscp = dscp_cnt;
                rv = bcm_petra_qos_map_delete(unit, BCM_QOS_MAP_IPV4, &qos_map, map_id);
                if (rv != BCM_E_NONE) {
                    LOG_ERROR(BSL_LS_BCM_QOS,
                              (BSL_META_U(unit,
                                          "%s: L3 IPV4 %d reset %d failed\n"),FUNCTION_NAME(), idx, profile_ndx));
                    BCMDNX_IF_ERR_EXIT(rv);
                }
                /* Destroy all IPV6 */
                rv = bcm_petra_qos_map_delete(unit, BCM_QOS_MAP_IPV6, &qos_map, map_id);
                if (rv != BCM_E_NONE) {
                    LOG_ERROR(BSL_LS_BCM_QOS,
                              (BSL_META_U(unit,
                                          "%s: L3 IPV4 %d reset %d failed\n"),FUNCTION_NAME(), idx, profile_ndx));
                    BCMDNX_IF_ERR_EXIT(rv);
                }
                /* Destroy all TC_DP associated with profile_ndx */
                for (tc_cnt = 0; tc_cnt < _BCM_QOS_MAP_TC_MAX; tc_cnt++) {
                    for (dp_cnt = 0; dp_cnt < 4; dp_cnt++) {
                        bcm_qos_map_t_init(&qos_map);
                        qos_map.int_pri = tc_cnt;
                        BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_decode(unit, dp_cnt, &(qos_map.color)));
                        rv = bcm_petra_qos_map_delete(unit, 0, &qos_map, map_id);
                        if (rv != BCM_E_NONE) {
                            LOG_ERROR(BSL_LS_BCM_QOS,
                                      (BSL_META_U(unit,
                                                  "%s: L3 IPV4 %d reset %d failed\n"),FUNCTION_NAME(), idx, profile_ndx));
                            BCMDNX_IF_ERR_EXIT(rv);
                        }
                    }
                }
            }
        }
        else if (opcode_type & SOC_PPC_LIF_COS_OPCODE_TYPE_L2) {   /* L2 */
            for (pri_cfi_cnt = 0; pri_cfi_cnt <  _BCM_QOS_MAP_ING_L2_PRI_CFI_MAX; pri_cfi_cnt++) {
                bcm_qos_map_t_init(&qos_map);
                qos_map.int_pri = pri_cfi_cnt >> 1;
                qos_map.pkt_cfi = pri_cfi_cnt & 1;
                rv = bcm_petra_qos_map_delete(unit, BCM_QOS_MAP_L2, &qos_map, map_id);
                if (rv != BCM_E_NONE) {
                    LOG_ERROR(BSL_LS_BCM_QOS,
                              (BSL_META_U(unit,
                                          "%s: L2|L2_OUTER_TAG %d reset %d failed\n"),FUNCTION_NAME(), idx, profile_ndx));
                    BCMDNX_IF_ERR_EXIT(rv);
                }
                rv = bcm_petra_qos_map_delete(unit, BCM_QOS_MAP_L2|BCM_QOS_MAP_L2_INNER_TAG, &qos_map, map_id);
                if (rv != BCM_E_NONE) {
                    LOG_ERROR(BSL_LS_BCM_QOS,
                              (BSL_META_U(unit,
                                          "%s: L2|L2_INNER_TAG %d reset %d failed\n"),FUNCTION_NAME(), idx, profile_ndx));
                    BCMDNX_IF_ERR_EXIT(rv);
                }
                rv = bcm_petra_qos_map_delete(unit, BCM_QOS_MAP_L2|BCM_QOS_MAP_L2_UNTAGGED, &qos_map, map_id);
                if (rv != BCM_E_NONE) {
                    LOG_ERROR(BSL_LS_BCM_QOS,
                              (BSL_META_U(unit,
                                          "%s: L2|L2_UNTAGGED %d reset %d failed\n"),FUNCTION_NAME(), idx, profile_ndx));
                    BCMDNX_IF_ERR_EXIT(rv);
                }
            }
        } 
        else if (opcode_type & SOC_PPC_LIF_COS_OPCODE_TYPE_TC_DP) { /* TC/DP */
            /* Destroy all TC_DP associated with profile_ndx */
            for (tc_cnt = 0; tc_cnt < _BCM_QOS_MAP_TC_MAX; dscp_cnt++) {
                for (dp_cnt = 0; dp_cnt < 4; dp_cnt++) {
                    bcm_qos_map_t_init(&qos_map);
                    qos_map.int_pri = tc_cnt;
                    BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_decode(unit, dp_cnt, &(qos_map.color)));
                    rv = bcm_petra_qos_map_delete(unit, BCM_QOS_MAP_L2_UNTAGGED, &qos_map, map_id);
                    if (rv != BCM_E_NONE) {
                        LOG_ERROR(BSL_LS_BCM_QOS,
                                  (BSL_META_U(unit,
                                              "%s: L3 IPV4 %d reset %d failed\n"),FUNCTION_NAME(), idx, profile_ndx));
                        BCMDNX_IF_ERR_EXIT(rv);
                    }
                }
            }
        } 
          
        /* Clear bit associated with opcode index */
        rv = QOS_ACCESS.opcode_bmp.bit_clear(unit, profile_ndx);
        BCMDNX_IF_ERR_EXIT(rv);

    }
    else if(BCM_INT_QOS_MAP_ENCAP_INTPRI_COLOR(map_id)) 
    {
        bcm_qos_map_t_init(&qos_map);
       /* first - deleting the entry*/
       BCMDNX_IF_ERR_EXIT( bcm_petra_qos_map_delete(unit, BCM_QOS_MAP_EGRESS| BCM_QOS_MAP_ENCAP_INTPRI_COLOR, &qos_map, map_id));

       BCMDNX_IF_ERR_EXIT(bcm_dpp_am_map_encap_intpri_color_dealloc(unit,flags, profile_ndx));
    }
    else if(BCM_INT_QOS_MAP_IS_INGRESS_COLOR(map_id)) 
    {
        for (color = 0; color < SOC_DPP_CONFIG(unit)->qos.dp_max; color++) {
            for (remark_int_pri = 0; remark_int_pri < _BCM_QOS_MAP_ING_L2_PRI_CFI_MAX; remark_int_pri++){
                 bcm_qos_map_t_init(&qos_map);
                 qos_map.remark_int_pri = remark_int_pri;
                 qos_map.color = color;
                 BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_encode(unit,qos_map.color,&dp));
                 rv = QOS_ACCESS.ing_pcp_vlan_color_bitmap.bit_get(unit,
                    _DPP_QOS_ENTRY_INDEX_DP(profile_ndx, dp, qos_map.remark_int_pri),
                    &bit_val);
                 BCMDNX_IF_ERR_EXIT(rv);
                 if (!bit_val) {
                     continue;
                 }
                 rv = bcm_petra_qos_map_delete(unit, BCM_QOS_MAP_INGRESS| BCM_QOS_MAP_POLICER, &qos_map, map_id);
                 if (rv != BCM_E_NONE) {
                     LOG_ERROR(BSL_LS_BCM_QOS,
                               (BSL_META_U(unit,
                                           "%s: Egress Remark MPLS %d reset %d failed\n"),FUNCTION_NAME(), idx, profile_ndx));
                     BCMDNX_IF_ERR_EXIT(rv);
                 }
            }
        }
        BCMDNX_IF_ERR_EXIT(bcm_dpp_am_qos_ing_color_dealloc(unit,flags, profile_ndx));
    }
   

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


/* Add an entry to a QoS map */

STATIC int 
_bcm_petra_qos_map_ingress_elsp_add(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_NONE;
    int dev_id = 0;
    uint32 profile_ndx = 0;
    SOC_PPC_FRWRD_ILM_GLBL_INFO glbl_info;

    BCMDNX_INIT_FUNC_DEFS;
    dev_id = (unit);
    profile_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);

    if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_ing_elsp) {
    
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: invalid MPLS ELSP profile (%d) out of range\n"),
                              FUNCTION_NAME(), profile_ndx));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid MPLS ELSP profile")));
    }
    
    SOC_PPC_FRWRD_ILM_GLBL_INFO_clear(&glbl_info);
    BCM_SAND_IF_ERR_EXIT(soc_ppd_frwrd_ilm_glbl_info_get(dev_id, &glbl_info));

    glbl_info.elsp_info.exp_map_tbl[map->exp] = map->int_pri;
    BCM_SAND_IF_ERR_EXIT(soc_ppd_frwrd_ilm_glbl_info_set(dev_id, &glbl_info));
    
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int 
_bcm_petra_qos_map_ingress_lif_cos_add(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_NONE;
    int dev_id = 0, dp = 0, tmp_dp = 0;
    uint32 profile_ndx = 0;
    SOC_PPC_LIF_COS_PROFILE_INFO profile_info;
    SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY map_entry;
    SOC_PPC_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY mpls_key;
    SOC_PPC_LIF_COS_PROFILE_MAP_TBL_IP_KEY ip_key;
    SOC_PPC_LIF_COS_PROFILE_MAP_TBL_L2_KEY l2_key;
    SOC_PPC_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY tc_dp_key;

    BCMDNX_INIT_FUNC_DEFS;
    dev_id = (unit);
    profile_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);
    
    if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_ing_lif_cos) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: invalid lif_cos profile (%d) out of range\n"), FUNCTION_NAME(), profile_ndx));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid LIF COS profile")));
    }
    
    SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY_clear(&map_entry);
    SOC_PPC_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY_clear(&mpls_key);
    SOC_PPC_LIF_COS_PROFILE_MAP_TBL_IP_KEY_clear(&ip_key);
    SOC_PPC_LIF_COS_PROFILE_MAP_TBL_L2_KEY_clear(&l2_key);
    SOC_PPC_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY_clear(&tc_dp_key);
    
    BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_encode(unit,map->color,&dp));

    SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_cos_profile_info_get(dev_id,
                              profile_ndx,
                              &profile_info));
    
    if (flags & BCM_QOS_MAP_MPLS) {
    
        /*
        * 'map_when_ip' and 'map_when_mpls' should have the 
        * same value for Soc_petra-B.
        */
        profile_info.map_when_ip = TRUE;
        profile_info.map_when_mpls = TRUE;

        mpls_key.in_exp = map->exp;
        map_entry.tc = map->int_pri;
        map_entry.dp = dp;
        map_entry.dscp = map->remark_int_pri;
        SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_cos_profile_map_mpls_label_info_set(dev_id,
                                     profile_ndx,
                                     &mpls_key,
                                     &map_entry));

        if ((map_entry.tc + map_entry.dp) != 0) {
            /* if entry content is non-zero, the entry is in use.
             * corresponding bit in bitmap ing_lif_cos_mpls_bitmap should be set.
             */
            rv = QOS_ACCESS.ing_lif_cos_mpls_bitmap.bit_set(unit, _DPP_QOS_ENTRY_INDEX_ING_LIF_COS_MPLS(profile_ndx, map->exp));
            BCMDNX_IF_ERR_EXIT(rv);
        }
        else {
            /* if entry content is zero, the entry is not in use.
             * corresponding bit in bitmap ing_lif_cos_mpls_bitmap should be clear.
             */
            rv = QOS_ACCESS.ing_lif_cos_mpls_bitmap.bit_clear(unit, _DPP_QOS_ENTRY_INDEX_ING_LIF_COS_MPLS(profile_ndx, map->exp));
            BCMDNX_IF_ERR_EXIT(rv);
        }
        
    } 

    if (flags & BCM_QOS_MAP_L3) {
#ifdef BCM_88660_A0
        if( !(SOC_IS_ARADPLUS(unit) && (flags & BCM_QOS_MAP_L3_L2))) 
#endif  /* BCM_88660_A0 */
        {
            /*
             * 'map_when_ip' and 'map_when_mpls' should have the 
             * same value for Soc_petra-B.
             */
            profile_info.map_when_ip = TRUE;
            profile_info.map_when_mpls = TRUE;
        }

        ip_key.ip_type = (flags & BCM_QOS_MAP_IPV6) ? SOC_SAND_PP_IP_TYPE_IPV6 : SOC_SAND_PP_IP_TYPE_IPV4;
        ip_key.tos = map->dscp;

        if (map->inherited_dscp_exp == 0) {
            ip_key.dscp_exp = map->dscp;
        } else {
            ip_key.dscp_exp = map->exp;
        }

        map_entry.tc = map->int_pri;
        map_entry.dp = dp;
        map_entry.dscp = map->remark_int_pri;
        SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_cos_profile_map_ip_info_set(dev_id,
                                 profile_ndx,
                                 &ip_key,
                                 &map_entry));

        if ((map_entry.tc + map_entry.dp) != 0) {
            if (flags & BCM_QOS_MAP_IPV6) {
                /* if entry content is non-zero, the entry is in use.
                 * corresponding bit in bitmap ing_lif_cos_ipv6_bitmap should be set.
                 */
                rv = QOS_ACCESS.ing_lif_cos_ipv6_bitmap.bit_set(unit, _DPP_QOS_ENTRY_INDEX_ING_LIF_COS_L3(profile_ndx, map->dscp));
                BCMDNX_IF_ERR_EXIT(rv);
            }
            else {
                /* if entry content is non-zero, the entry is in use.
                 * corresponding bit in bitmap BCM_QOS_MAP_IPV4 should be set.
                 */
                rv = QOS_ACCESS.ing_lif_cos_ipv4_bitmap.bit_set(unit, _DPP_QOS_ENTRY_INDEX_ING_LIF_COS_L3(profile_ndx, map->dscp));
                BCMDNX_IF_ERR_EXIT(rv);
            }
        } else {
            if (flags & BCM_QOS_MAP_IPV6) {
                /* if entry content is zero, the entry is not in use.
                 * corresponding bit in bitmap ing_lif_cos_ipv6_bitmap should be clear..
                 */
                rv = QOS_ACCESS.ing_lif_cos_ipv6_bitmap.bit_clear(unit, _DPP_QOS_ENTRY_INDEX_ING_LIF_COS_L3(profile_ndx, map->dscp));
                BCMDNX_IF_ERR_EXIT(rv);
            }
            else {
                /* if entry content is zero, the entry is not in use.
                 * corresponding bit in bitmap ing_lif_cos_ipv4_bitmap should be clear.
                 */
                rv = QOS_ACCESS.ing_lif_cos_ipv4_bitmap.bit_clear(unit, _DPP_QOS_ENTRY_INDEX_ING_LIF_COS_L3(profile_ndx, map->dscp));
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }
        
    }

    if (flags & BCM_QOS_MAP_L2 && (!(flags & BCM_QOS_MAP_L2_UNTAGGED))) {

        profile_info.map_when_l2 = TRUE;

        l2_key.outer_tpid = (flags & BCM_QOS_MAP_L2_INNER_TAG) ? 1:0;
        l2_key.incoming_up = map->pkt_pri;
        l2_key.incoming_dei = map->pkt_cfi;

        map_entry.tc = map->int_pri;
        map_entry.dp = dp;

        SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_cos_profile_map_l2_info_set(dev_id,
                                     profile_ndx,
                                     &l2_key,
                                     &map_entry));

        if ((map_entry.tc + map_entry.dp) != 0) {
            if (flags & BCM_QOS_MAP_L2_INNER_TAG) {
                /* if entry content is non-zero, the entry is in use, 
                 * corresponding bit in bitmap ing_lif_cos_l2_inner_bitmap should be set.
                 */
                rv = QOS_ACCESS.ing_lif_cos_l2_inner_bitmap.bit_set(unit, _DPP_QOS_ENTRY_INDEX_ING_LIF_COS_L2_INNER(profile_ndx, l2_key.incoming_up, l2_key.incoming_dei));
                BCMDNX_IF_ERR_EXIT(rv);
            }
            else {
                /* if entry content is non-zero, the entry is in use.
                 * corresponding bit in bitmap ing_lif_cos_l2_outer_bitmap should be set.
                 */
                rv = QOS_ACCESS.ing_lif_cos_l2_outer_bitmap.bit_set(unit, _DPP_QOS_ENTRY_INDEX_ING_LIF_COS_L2_OUTER(profile_ndx, l2_key.incoming_up, l2_key.incoming_dei));
                BCMDNX_IF_ERR_EXIT(rv);
            }
        } else {
            if (flags & BCM_QOS_MAP_L2_INNER_TAG) {
                /* if entry content is zero, the entry is not in use.
                 * corresponding bit in bitmap ing_lif_cos_l2_inner_bitmap should be clear..
                 */
                rv = QOS_ACCESS.ing_lif_cos_l2_inner_bitmap.bit_clear(unit, _DPP_QOS_ENTRY_INDEX_ING_LIF_COS_L2_INNER(profile_ndx, l2_key.incoming_up, l2_key.incoming_dei));
                BCMDNX_IF_ERR_EXIT(rv);
            }
            else {
                /* if entry content is zero, the entry is not in use.
                 * corresponding bit in bitmap ing_lif_cos_l2_outer_bitmap should be clear..
                 */
                rv = QOS_ACCESS.ing_lif_cos_l2_outer_bitmap.bit_clear(unit, _DPP_QOS_ENTRY_INDEX_ING_LIF_COS_L2_OUTER(profile_ndx, l2_key.incoming_up, l2_key.incoming_dei));
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }

    }

    if (flags & BCM_QOS_MAP_L2_UNTAGGED) {
        profile_info.map_from_tc_dp = TRUE;

        tc_dp_key.tc = map->pkt_pri;
        BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_encode(unit,map->pkt_cfi,&tmp_dp));         
        tc_dp_key.dp = tmp_dp;

        map_entry.tc = map->int_pri;
        map_entry.dp = dp;

        SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_cos_profile_map_tc_dp_info_set(dev_id,
                                     profile_ndx,
                                     &tc_dp_key,
                                     &map_entry));

        if ((map_entry.tc + map_entry.dp) != 0) {
            /* if entry content is non-zero, the entry is in use.
             * corresponding bit in bitmap ing_lif_cos_l2_untag_bitmap should be set.
             */
            rv = QOS_ACCESS.ing_lif_cos_l2_untag_bitmap.bit_set(unit, _DPP_QOS_ENTRY_INDEX_ING_LIF_COS_L2_UNTAG(profile_ndx, tc_dp_key.tc, tc_dp_key.dp));
            BCMDNX_IF_ERR_EXIT(rv);
        }
        else {
            /* if entry content is zero, the entry is not in use.
             * corresponding bit in bitmap ing_lif_cos_l2_untag_bitmap should be clear.
             */
            rv = QOS_ACCESS.ing_lif_cos_l2_untag_bitmap.bit_clear(unit, _DPP_QOS_ENTRY_INDEX_ING_LIF_COS_L2_UNTAG(profile_ndx, tc_dp_key.tc, tc_dp_key.dp));
            BCMDNX_IF_ERR_EXIT(rv);
        }
        
    }

#ifdef BCM_88660_A0
    if (SOC_IS_ARADPLUS(unit) && (flags & BCM_QOS_MAP_L3_L2)) {
        if (profile_ndx > _BCM_QOS_MAP_ING_USE_PCP_MAX_QOS_PROFILE) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: invalid lif_cos profile (%d) out of range (0-15).\n"), FUNCTION_NAME(), profile_ndx));
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid LIF COS profile for using layer 2 pcp")));
        }
        
        profile_info.use_layer2_pcp = TRUE;
    }
#endif /* BCM_88660_A0 */

    SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_cos_profile_info_set(dev_id,
                              profile_ndx,
                              &profile_info));
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_petra_qos_map_ingress_pcp_vlan_add(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_NONE;
    int dev_id = 0, dp = 0;
    uint32 profile_ndx = 0;
    BCMDNX_INIT_FUNC_DEFS;

    dev_id = (unit);
    profile_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);

    BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_encode(unit,map->color,&dp));

    if ((flags & BCM_QOS_MAP_L2_VLAN_PCP) == 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("flag BCM_QOS_MAP_L2_VLAN_PCP must be set")));
    }

    if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_ing_pcp_vlan) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: invalid ingress vlan pcp profile (%d) out of range\n"), FUNCTION_NAME(), profile_ndx));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid PCP VLAN profile")));
    }

    if (flags & BCM_QOS_MAP_L2) {
        /* ing_vlan_edit */
        if (flags & BCM_QOS_MAP_L2_INNER_TAG) {

            SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_ing_vlan_edit_pcp_map_ctag_set(dev_id, 
                                    profile_ndx, 
                                    map->int_pri,
                                    map->pkt_pri,
                                    map->pkt_cfi));

            if ((map->pkt_pri + map->pkt_cfi) != 0) {
                /* if entry content is non-zero, the entry is in use.
                 * corresponding bit in bitmap ing_pcp_vlan_ctag_bitmap should be set.
                 */
                rv = QOS_ACCESS.ing_pcp_vlan_ctag_bitmap.bit_set(unit, _DPP_QOS_ENTRY_INDEX_ING_PCP_VLAN_CTAG(profile_ndx, map->int_pri));
                BCMDNX_IF_ERR_EXIT(rv);
            }
            else {
                /* if entry content is zero, the entry is not in use.
                 * corresponding bit in bitmap ing_pcp_vlan_ctag_bitmap should be clear.
                 */
                rv = QOS_ACCESS.ing_pcp_vlan_ctag_bitmap.bit_clear(unit, _DPP_QOS_ENTRY_INDEX_ING_PCP_VLAN_CTAG(profile_ndx, map->int_pri));
                BCMDNX_IF_ERR_EXIT(rv);
            }
            
        } else if (flags & BCM_QOS_MAP_L2_UNTAGGED) {

            SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_ing_vlan_edit_pcp_map_untagged_set(dev_id, 
                                        profile_ndx, 
                                        map->int_pri,
                                        dp,
                                        map->pkt_pri,
                                        map->pkt_cfi));

            if ((map->pkt_pri + map->pkt_cfi) != 0) {
                /* if entry content is non-zero, the entry is in use.
                 * corresponding bit in bitmap ing_pcp_vlan_utag_bitmap should be set.
                 */
                rv = QOS_ACCESS.ing_pcp_vlan_utag_bitmap.bit_set(unit, _DPP_QOS_ENTRY_INDEX_ING_PCP_VLAN_UTAG(profile_ndx, map->int_pri, dp));
                BCMDNX_IF_ERR_EXIT(rv);
            }
            else {
                /* if entry content is zero, the entry is not in use.
                 * corresponding bit in bitmap ing_pcp_vlan_utag_bitmap should be clear.
                 */
                rv = QOS_ACCESS.ing_pcp_vlan_utag_bitmap.bit_clear(unit, _DPP_QOS_ENTRY_INDEX_ING_PCP_VLAN_UTAG(profile_ndx, map->int_pri, dp));
                BCMDNX_IF_ERR_EXIT(rv);
            }

        } else { /* OUTER TAG */

            SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_ing_vlan_edit_pcp_map_stag_set(dev_id, 
                                    profile_ndx, 
                                    map->int_pri,
                                    (dp) ? 1 : 0,
                                    map->pkt_pri,
                                    map->pkt_cfi));

            if ((map->pkt_pri + map->pkt_cfi) != 0) {
                /* if entry content is non-zero, the entry is in use.
                 * corresponding bit in bitmap ing_pcp_vlan_stag_bitmap should be set.
                 */
                rv = QOS_ACCESS.ing_pcp_vlan_stag_bitmap.bit_set(unit, _DPP_QOS_ENTRY_INDEX_ING_PCP_VLAN_STAG(profile_ndx, map->int_pri, (dp) ? 1 : 0));
                BCMDNX_IF_ERR_EXIT(rv);
            }
            else {
                /* if entry content is zero, the entry is not in use.
                 * corresponding bit in bitmap ing_pcp_vlan_stag_bitmap should be clear.
                 */
                rv = QOS_ACCESS.ing_pcp_vlan_stag_bitmap.bit_clear(unit, _DPP_QOS_ENTRY_INDEX_ING_PCP_VLAN_STAG(profile_ndx, map->int_pri, (dp) ? 1 : 0));
                BCMDNX_IF_ERR_EXIT(rv);
            }

        }

    }
    
    BCMDNX_IF_ERR_EXIT(rv);    
exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_petra_qos_map_ingress_color_add(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_NONE;
    int dev_id = 0, dp = 0;
    uint32 profile_ndx = 0;
    BCMDNX_INIT_FUNC_DEFS;

    dev_id = (unit);
    profile_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);

    BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_encode(unit,map->color,&dp));

    if ((flags & BCM_QOS_MAP_POLICER) == 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("flag BCM_QOS_MAP_POLICER must be set")));
    }

    if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_ing_dp) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: invalid ingress dp profile (%d) out of range\n"), FUNCTION_NAME(), profile_ndx));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid dp profile")));
    }

	BCMDNX_IF_ERR_EXIT(MBCM_PP_DRIVER_CALL(dev_id, mbcm_pp_lif_ing_vlan_edit_pcp_map_dp_set, (unit, 
                            profile_ndx, 
                            dp,
                            map->remark_int_pri,
                            map->pkt_pri,
                            map->pkt_cfi)));
    if ((map->pkt_pri + map->pkt_cfi) != 0) {
        /* if entry content is non-zero, the entry is in use.
         * corresponding bit in bitmap ing_pcp_vlan_color_bitmap should be set.
         */
        rv = QOS_ACCESS.ing_pcp_vlan_color_bitmap.bit_set(unit, _DPP_QOS_ENTRY_INDEX_DP(profile_ndx, dp, map->remark_int_pri));
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else{
         /* if entry content is zero, the entry is not in use.
             * corresponding bit in bitmap ing_pcp_vlan_color_bitmap should be clear.
             */ 
        rv = QOS_ACCESS.ing_pcp_vlan_color_bitmap.bit_clear(unit, _DPP_QOS_ENTRY_INDEX_DP(profile_ndx, dp, map->remark_int_pri));
        BCMDNX_IF_ERR_EXIT(rv);
    }

    BCMDNX_IF_ERR_EXIT(rv);    
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int 
_bcm_petra_qos_map_ingress_cos_ipv4_add(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    bcm_error_t rv = BCM_E_NONE;
    int dev_id;
    uint32 sand_rv, opcode_ndx;
    SOC_PPC_LIF_COS_OPCODE_ACTION_INFO action_info;
    SOC_PPC_LIF_COS_OPCODE_TYPE opcode_type;
    int ing_cos_opcode_flags;

    BCMDNX_INIT_FUNC_DEFS;
    SOC_PPC_LIF_COS_OPCODE_ACTION_INFO_clear(&action_info);

    dev_id = (unit);
    opcode_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);

    sand_rv = soc_ppd_lif_cos_opcode_types_get(dev_id, opcode_ndx, &opcode_type);
    BCM_SAND_IF_ERR_EXIT(sand_rv);
    if ((opcode_type & SOC_PPC_LIF_COS_OPCODE_TYPE_L3) == 0) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: invalid OPCODE TYPE (0x%x) \n"), FUNCTION_NAME(), opcode_type));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Incorrect Opcode Index used")));
    }

    action_info.is_packet_valid = (flags & BCM_QOS_MAP_PACKET_INVALID) ? FALSE : TRUE;
    action_info.is_qos_only = (flags & BCM_QOS_MAP_IGNORE_OFFSET) ? TRUE : FALSE;
    action_info.ac_offset = map->port_offset;

    sand_rv = soc_ppd_lif_cos_opcode_ipv4_tos_map_set(dev_id, opcode_ndx, map->dscp, &action_info);
    BCM_SAND_IF_ERR_EXIT(sand_rv);
    
    rv = QOS_ACCESS.ing_cos_opcode_flags.get(unit, opcode_ndx, &ing_cos_opcode_flags);
    BCMDNX_IF_ERR_EXIT(rv);
    ing_cos_opcode_flags |= flags;
    rv = QOS_ACCESS.ing_cos_opcode_flags.set(unit, opcode_ndx, ing_cos_opcode_flags);
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int 
_bcm_petra_qos_map_ingress_cos_ipv6_add(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    bcm_error_t rv = BCM_E_NONE;
    int dev_id;
    uint32 sand_rv, opcode_ndx;
    SOC_PPC_LIF_COS_OPCODE_ACTION_INFO action_info;
    SOC_PPC_LIF_COS_OPCODE_TYPE opcode_type;
    int ing_cos_opcode_flags;
    BCMDNX_INIT_FUNC_DEFS;
    dev_id = (unit);
    opcode_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);

    SOC_PPC_LIF_COS_OPCODE_ACTION_INFO_clear(&action_info);

    sand_rv = soc_ppd_lif_cos_opcode_types_get(dev_id, opcode_ndx, &opcode_type);
    BCM_SAND_IF_ERR_EXIT(sand_rv);
    if ((opcode_type & SOC_PPC_LIF_COS_OPCODE_TYPE_L3) == 0) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: invalid OPCODE TYPE (0x%x) \n"), FUNCTION_NAME(), opcode_type));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Incorrect Opcode Index used")));
    }

    action_info.is_packet_valid = (flags & BCM_QOS_MAP_PACKET_INVALID) ? FALSE : TRUE;
    action_info.is_qos_only = (flags & BCM_QOS_MAP_IGNORE_OFFSET) ? TRUE : FALSE;
    action_info.ac_offset = map->port_offset;

    sand_rv = soc_ppd_lif_cos_opcode_ipv6_tos_map_set(dev_id, opcode_ndx, map->dscp, &action_info);
    BCM_SAND_IF_ERR_EXIT(sand_rv);
    rv = QOS_ACCESS.ing_cos_opcode_flags.get(unit, opcode_ndx, &ing_cos_opcode_flags);
    BCMDNX_IF_ERR_EXIT(rv);
    ing_cos_opcode_flags |= flags;
    rv = QOS_ACCESS.ing_cos_opcode_flags.set(unit, opcode_ndx, ing_cos_opcode_flags);
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int 
_bcm_petra_qos_map_ingress_cos_l2_add(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    bcm_error_t rv = BCM_E_NONE;
    int dev_id;
    uint32 opcode_ndx, sand_rv;
    SOC_PPC_LIF_COS_OPCODE_ACTION_INFO action_info;
    SOC_PPC_LLP_PARSE_TPID_INDEX outer_tpid_ndx;
    SOC_PPC_LIF_COS_OPCODE_TYPE opcode_type;
    int ing_cos_opcode_flags;
    BCMDNX_INIT_FUNC_DEFS;
    dev_id = (unit);

    SOC_PPC_LIF_COS_OPCODE_ACTION_INFO_clear(&action_info);
    opcode_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);

    _bcm_petra_qos_map_add_pcp_vlan_params_verify(unit, flags, map, map_id);

    sand_rv = soc_ppd_lif_cos_opcode_types_get(dev_id, opcode_ndx, &opcode_type);
    BCM_SAND_IF_ERR_EXIT(sand_rv);
    if ((opcode_type & SOC_PPC_LIF_COS_OPCODE_TYPE_L2) == 0) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: invalid OPCODE TYPE (0x%x) \n"), FUNCTION_NAME(), opcode_type));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Incorrect Opcode Index used")));
    }

    if (flags & BCM_QOS_MAP_L2_UNTAGGED) {
        outer_tpid_ndx = SOC_PPC_LLP_PARSE_TPID_INDEX_NONE;
    } else if (flags & BCM_QOS_MAP_L2_INNER_TAG) {
        outer_tpid_ndx = SOC_PPC_LLP_PARSE_TPID_INDEX_TPID2;
    } else {
        outer_tpid_ndx = SOC_PPC_LLP_PARSE_TPID_INDEX_TPID1;
    }

    action_info.is_packet_valid = (flags & BCM_QOS_MAP_PACKET_INVALID) ? FALSE : TRUE;
    action_info.is_qos_only = (flags & BCM_QOS_MAP_IGNORE_OFFSET) ? TRUE : FALSE;
    action_info.ac_offset = map->port_offset;

    sand_rv = soc_ppd_lif_cos_opcode_vlan_tag_map_set(dev_id, opcode_ndx, outer_tpid_ndx, map->int_pri, map->pkt_cfi, &action_info);
    BCM_SAND_IF_ERR_EXIT(sand_rv);
    rv = QOS_ACCESS.ing_cos_opcode_flags.get(unit, opcode_ndx, &ing_cos_opcode_flags);
    BCMDNX_IF_ERR_EXIT(rv);
    ing_cos_opcode_flags |= flags;
    rv = QOS_ACCESS.ing_cos_opcode_flags.set(unit, opcode_ndx, ing_cos_opcode_flags);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int 
_bcm_petra_qos_map_ingress_cos_tc_dp_add(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    bcm_error_t rv = BCM_E_NONE;
    int dev_id, dp;
    uint32 opcode_ndx, sand_rv;
    SOC_PPC_LIF_COS_OPCODE_ACTION_INFO action_info;
    SOC_PPC_LIF_COS_OPCODE_TYPE opcode_type;
    int ing_cos_opcode_flags;
    BCMDNX_INIT_FUNC_DEFS;
    dev_id = (unit);
    opcode_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);

    SOC_PPC_LIF_COS_OPCODE_ACTION_INFO_clear(&action_info);

    sand_rv = soc_ppd_lif_cos_opcode_types_get(dev_id, opcode_ndx, &opcode_type);
    BCM_SAND_IF_ERR_EXIT(sand_rv);
    if ((opcode_type & SOC_PPC_LIF_COS_OPCODE_TYPE_TC_DP) == 0) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: invalid OPCODE TYPE (0x%x) \n"), FUNCTION_NAME(), opcode_type));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Incorrect Opcode Index used")));
    }

    BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_encode(unit,map->color,&dp));

    action_info.is_packet_valid = (flags & BCM_QOS_MAP_PACKET_INVALID) ? FALSE : TRUE;
    action_info.is_qos_only = (flags & BCM_QOS_MAP_IGNORE_OFFSET) ? TRUE : FALSE;
    action_info.ac_offset = map->port_offset;

    sand_rv = soc_ppd_lif_cos_opcode_tc_dp_map_set(dev_id, opcode_ndx, map->int_pri, dp, &action_info);
    BCM_SAND_IF_ERR_EXIT(sand_rv);
    rv = QOS_ACCESS.ing_cos_opcode_flags.get(unit, opcode_ndx, &ing_cos_opcode_flags);
    BCMDNX_IF_ERR_EXIT(rv);
    ing_cos_opcode_flags |= flags;
    rv = QOS_ACCESS.ing_cos_opcode_flags.set(unit, opcode_ndx, ing_cos_opcode_flags);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int 
_bcm_petra_qos_map_ingress_cos_opcode_add(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    bcm_error_t rv = BCM_E_NONE;
    int dev_id;
    uint32 opcode_ndx, sand_rv;
    SOC_PPC_LIF_COS_OPCODE_TYPE opcode_type, type;
    uint8 bit_val;
    BCMDNX_INIT_FUNC_DEFS;

    dev_id = (unit);
    opcode_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);

    if ((flags & BCM_QOS_MAP_IPV4) || (flags & BCM_QOS_MAP_IPV6)) {
        type = SOC_PPC_LIF_COS_OPCODE_TYPE_L3 | SOC_PPC_LIF_COS_OPCODE_TYPE_TC_DP;
    } else if (flags & BCM_QOS_MAP_L2) {
        type = SOC_PPC_LIF_COS_OPCODE_TYPE_L2;
    } else if (flags & BCM_QOS_MAP_L2_UNTAGGED) {
        type = SOC_PPC_LIF_COS_OPCODE_TYPE_TC_DP;
    } else {
        type = SOC_PPC_LIF_COS_OPCODE_TYPE_L3 | SOC_PPC_LIF_COS_OPCODE_TYPE_TC_DP;
    }

    rv = QOS_ACCESS.opcode_bmp.bit_get(unit, opcode_ndx, &bit_val);
    BCMDNX_IF_ERR_EXIT(rv); 
    if (bit_val == 0) {
        sand_rv = soc_ppd_lif_cos_opcode_types_set(dev_id, opcode_ndx, type);
        BCM_SAND_IF_ERR_EXIT(sand_rv);
        rv = QOS_ACCESS.opcode_bmp.bit_set(unit, opcode_ndx);
        BCMDNX_IF_ERR_EXIT(rv);


    } else {
        sand_rv = soc_ppd_lif_cos_opcode_types_get(dev_id, opcode_ndx, &opcode_type);
        BCM_SAND_IF_ERR_EXIT(sand_rv);
        if (opcode_type != type) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: invalid OPCODE TYPE (0x%x) \n"), FUNCTION_NAME(), opcode_type));
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Map ID does not match flags.\n")));
        }
    }

    if (type & SOC_PPC_LIF_COS_OPCODE_TYPE_L3) {
        if (flags & BCM_QOS_MAP_IPV4) {
            rv = _bcm_petra_qos_map_ingress_cos_ipv4_add(unit, flags, map, map_id);
        } 
        else if (flags & BCM_QOS_MAP_IPV6) {
            rv = _bcm_petra_qos_map_ingress_cos_ipv6_add(unit, flags, map, map_id);
        } 
        else {
            rv = _bcm_petra_qos_map_ingress_cos_tc_dp_add(unit, flags, map, map_id);
        }
    }
    else if (type & SOC_PPC_LIF_COS_OPCODE_TYPE_L2) {   /* L2 */
        rv = _bcm_petra_qos_map_ingress_cos_l2_add(unit, flags, map, map_id);
    } 
    else if (type & SOC_PPC_LIF_COS_OPCODE_TYPE_TC_DP) { /* TC/DP */
        rv = _bcm_petra_qos_map_ingress_cos_tc_dp_add(unit, flags, map, map_id);
    }

    BCMDNX_IF_ERR_EXIT(rv);


exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_petra_qos_map_egress_remark_add(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_NONE;
    int dev_id = 0, dp = 0;
    uint32 profile_ndx = 0;
    SOC_PPC_EG_QOS_MAP_KEY prm_in_qos_key;
    SOC_PPC_EG_QOS_PARAMS  prm_out_qos_params;  
    SOC_PPC_EG_ENCAP_QOS_MAP_KEY prm_in_encap_qos_key;
    SOC_PPC_EG_ENCAP_QOS_PARAMS  prm_out_encap_qos_params;
    int tmp;

    BCMDNX_INIT_FUNC_DEFS;
    dev_id = (unit);
    profile_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);

    SOC_PPC_EG_QOS_MAP_KEY_clear(&prm_in_qos_key);
    SOC_PPC_EG_QOS_PARAMS_clear(&prm_out_qos_params);

    SOC_PPC_EG_ENCAP_QOS_MAP_KEY_clear(&prm_in_encap_qos_key);
    SOC_PPC_EG_ENCAP_QOS_PARAMS_clear(&prm_out_encap_qos_params);

    BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_encode(unit,map->color,&dp));

    if (flags & BCM_QOS_MAP_ENCAP) {
      
      if (flags & BCM_QOS_MAP_MPLS) {
          if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id) 
          {
              LOG_ERROR(BSL_LS_BCM_QOS,
                        (BSL_META_U(unit,
                                    "%s: invalid remark profile (%d) out of range\n"), FUNCTION_NAME(), profile_ndx));
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid REMARK profile")));
          }

          if ( SOC_DPP_CONFIG(unit)->qos.egr_remark_encap_enable != 1) {
              LOG_ERROR(BSL_LS_BCM_QOS,
                        (BSL_META_U(unit,
                                    "%s: encap remark is disabled. \n"), FUNCTION_NAME()));  
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Encap is disabled")));
          }

          /* internal pri */
          prm_in_encap_qos_key.in_dscp_exp = (uint32)map->remark_int_pri;
          prm_in_encap_qos_key.remark_profile = profile_ndx;
          prm_in_encap_qos_key.pkt_hdr_type = SOC_PPC_PKT_HDR_TYPE_MPLS;


          SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_encap_qos_params_remark_get(dev_id,
                             &prm_in_encap_qos_key,
                             &prm_out_encap_qos_params));
          
          prm_out_encap_qos_params.ip_dscp = map->dscp; 
          prm_out_encap_qos_params.mpls_exp = map->exp; 
          
          SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_encap_qos_params_remark_set(dev_id,
                             &prm_in_encap_qos_key,
                             &prm_out_encap_qos_params));

          if ((map->dscp + map->exp) != 0) {
              /* if entry content is non-zero, the entry is in use.
               * corresponding bit in bitmap egr_remark_encap_mpls_bitmap should be set.
               */
              rv = QOS_ACCESS.egr_remark_encap_mpls_bitmap.bit_set(unit, _DPP_QOS_ENTRY_INDEX_EGR_REMARK_ENP_MPLS(profile_ndx, prm_in_encap_qos_key.in_dscp_exp));
              BCMDNX_IF_ERR_EXIT(rv);
          }
          else {
              /* if entry content is zero, the entry is not in use.
               * corresponding bit in bitmap egr_remark_encap_mpls_bitmap should be clear.
               */
              rv = QOS_ACCESS.egr_remark_encap_mpls_bitmap.bit_clear(unit, _DPP_QOS_ENTRY_INDEX_EGR_REMARK_ENP_MPLS(profile_ndx, prm_in_encap_qos_key.in_dscp_exp));
              BCMDNX_IF_ERR_EXIT(rv);
          }
          
     }
     else if (flags & BCM_QOS_MAP_IPV6) 
     {
          if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id) 
          {
              LOG_ERROR(BSL_LS_BCM_QOS,
                        (BSL_META_U(unit,
                                    "%s: invalid remark profile (%d) out of range\n"), FUNCTION_NAME(), profile_ndx));
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("REMARK profile is out of range")));
          }

          /* In !ARAD-A0 Remark profile for IP Tunnels is only 15 remark ids */
          if (SOC_IS_ARAD_B0_AND_ABOVE(unit) && profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id-1) 
          {
              LOG_ERROR(BSL_LS_BCM_QOS,
                        (BSL_META_U(unit,
                                    "%s: invalid remark profile (%d) out of range\n"), FUNCTION_NAME(), profile_ndx));
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("REMARK profile is out of range. There are only 15 Remark profile for IP tunnels")));
          }

          if ( SOC_DPP_CONFIG(unit)->qos.egr_remark_encap_enable != 1) {
              LOG_ERROR(BSL_LS_BCM_QOS,
                        (BSL_META_U(unit,
                                    "%s: encap remark is disabled. \n"), FUNCTION_NAME()));  
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Encap is disabled")));
          }
          /* internal pri */
          prm_in_encap_qos_key.in_dscp_exp = (uint32)map->remark_int_pri;
          prm_in_encap_qos_key.remark_profile = profile_ndx;
          prm_in_encap_qos_key.pkt_hdr_type = SOC_PPC_PKT_HDR_TYPE_IPV6;


          SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_encap_qos_params_remark_get(dev_id,
                             &prm_in_encap_qos_key,
                             &prm_out_encap_qos_params));
       
          prm_out_encap_qos_params.ip_dscp = map->dscp; 
          prm_out_encap_qos_params.mpls_exp = map->exp; 
           
          
          SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_encap_qos_params_remark_set(dev_id,
                             &prm_in_encap_qos_key,
                             &prm_out_encap_qos_params));

          if ((map->dscp + map->exp) != 0) {
              /* if entry content is non-zero, the entry is in use.
               * corresponding bit in bitmap egr_remark_encap_ipv6_bitmap should be set.
               */
              rv = QOS_ACCESS.egr_remark_encap_ipv6_bitmap.bit_set(unit, _DPP_QOS_ENTRY_INDEX_EGR_REMARK_ENP_L3(profile_ndx, prm_in_encap_qos_key.in_dscp_exp));
              BCMDNX_IF_ERR_EXIT(rv);
          }
          else {
              /* if entry content is zero, the entry is not in use.
               * corresponding bit in bitmap egr_remark_encap_ipv6_bitmap should be clear.
               */
              rv = QOS_ACCESS.egr_remark_encap_ipv6_bitmap.bit_clear(unit, _DPP_QOS_ENTRY_INDEX_EGR_REMARK_ENP_L3(profile_ndx, prm_in_encap_qos_key.in_dscp_exp));
              BCMDNX_IF_ERR_EXIT(rv);
          }

     }
     else if (flags & BCM_QOS_MAP_IPV4) 
     {
          if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id) 
          {
              LOG_ERROR(BSL_LS_BCM_QOS,
                        (BSL_META_U(unit,
                                    "%s: invalid remark profile (%d) out of range\n"), FUNCTION_NAME(), profile_ndx));
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("REMARK profile is out of range")));
          }

          /* In !ARAD-A0 Remark profile for IP Tunnels is only 15 remark ids */
          if (SOC_IS_ARAD_B0_AND_ABOVE(unit) && profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id-1) 
          {
              LOG_ERROR(BSL_LS_BCM_QOS,
                        (BSL_META_U(unit,
                                    "%s: invalid remark profile (%d) out of range\n"), FUNCTION_NAME(), profile_ndx));
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("REMARK profile is out of range. There are only 15 Remark profile for IP tunnels")));
          }

          if ( SOC_DPP_CONFIG(unit)->qos.egr_remark_encap_enable != 1) {
              LOG_ERROR(BSL_LS_BCM_QOS,
                        (BSL_META_U(unit,
                                    "%s: encap remark is disabled. \n"), FUNCTION_NAME()));  
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Encap is disabled")));
          }
          /* internal pri */
          prm_in_encap_qos_key.in_dscp_exp = (uint32)map->remark_int_pri;
          prm_in_encap_qos_key.remark_profile = profile_ndx;
          prm_in_encap_qos_key.pkt_hdr_type = SOC_PPC_PKT_HDR_TYPE_IPV4;


          SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_encap_qos_params_remark_get(dev_id,
                             &prm_in_encap_qos_key,
                             &prm_out_encap_qos_params));
       
          prm_out_encap_qos_params.ip_dscp = map->dscp; 
          prm_out_encap_qos_params.mpls_exp = map->exp; 
           
          
          SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_encap_qos_params_remark_set(dev_id,
                             &prm_in_encap_qos_key,
                             &prm_out_encap_qos_params));

        if ((map->dscp + map->exp) != 0) {
            /* if entry content is non-zero, the entry is in use.
             * corresponding bit in bitmap egr_remark_encap_ipv4_bitmap should be set.
             */
            rv = QOS_ACCESS.egr_remark_encap_ipv4_bitmap.bit_set(unit, _DPP_QOS_ENTRY_INDEX_EGR_REMARK_ENP_L3(profile_ndx, prm_in_encap_qos_key.in_dscp_exp));
            BCMDNX_IF_ERR_EXIT(rv);
        }
        else {
             /* if entry content is zero, the entry is not in use.
              * corresponding bit in bitmap egr_remark_encap_ipv4_bitmap should be clear.
              */
              rv = QOS_ACCESS.egr_remark_encap_ipv4_bitmap.bit_clear(unit, _DPP_QOS_ENTRY_INDEX_EGR_REMARK_ENP_L3(profile_ndx, prm_in_encap_qos_key.in_dscp_exp));
              BCMDNX_IF_ERR_EXIT(rv);
        }

     }
     else if (flags & BCM_QOS_MAP_L2) 
     {
          /* Only used for ARAD_B0_and_above */
          if (SOC_IS_ARAD_A0(unit)) {
              BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("REMARK flag L2 is not supported")));
          }

          if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id) 
          {
              LOG_ERROR(BSL_LS_BCM_QOS,
                        (BSL_META_U(unit,
                                    "%s: invalid remark profile (%d) out of range\n"), FUNCTION_NAME(), profile_ndx));
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("REMARK profile is out of range")));
          }

          /* Ethernet is only 4 remark ids */
          if (profile_ndx >= _BCM_QOS_MAP_EGR_REMARK_PROFILE_L2_MAX) 
          {
              LOG_ERROR(BSL_LS_BCM_QOS,
                        (BSL_META_U(unit,
                                    "%s: invalid remark profile (%d) out of range\n"), FUNCTION_NAME(), profile_ndx));
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("REMARK profile is out of range.")));
          }

          if ( SOC_DPP_CONFIG(unit)->qos.egr_remark_encap_enable != 1) {
              LOG_ERROR(BSL_LS_BCM_QOS,
                        (BSL_META_U(unit,
                                    "%s: encap remark is disabled. \n"), FUNCTION_NAME()));  
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Encap is disabled")));
          }
          /* internal pri */
          prm_in_encap_qos_key.in_dscp_exp = (uint32)(map->remark_int_pri & 0x7);
          prm_in_encap_qos_key.remark_profile = profile_ndx;
          prm_in_encap_qos_key.pkt_hdr_type = SOC_PPC_PKT_HDR_TYPE_ETH;
          tmp = 0;
          BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_encode(unit,map->color,&tmp)); /* dp */
          prm_in_encap_qos_key.dp = tmp;

          SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_encap_qos_params_remark_get(dev_id,
                             &prm_in_encap_qos_key,
                             &prm_out_encap_qos_params));
       
          prm_out_encap_qos_params.ip_dscp = map->dscp; 
          prm_out_encap_qos_params.mpls_exp = map->exp; 
           
          
          SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_encap_qos_params_remark_set(dev_id,
                             &prm_in_encap_qos_key,
                             &prm_out_encap_qos_params));

        if ((map->dscp + map->exp) != 0) {
            /* if entry content is non-zero, the entry is in use.
             * corresponding bit in bitmap egr_remark_encap_l2_bitmap should be set.
             */
            rv = QOS_ACCESS.egr_remark_encap_l2_bitmap.bit_set(unit, _DPP_QOS_ENTRY_INDEX_EGR_REMARK_ENP_L2(profile_ndx, map->remark_int_pri, map->color));
            BCMDNX_IF_ERR_EXIT(rv);
        }
        else {
            /* if entry content is zero, the entry is not in use.
             * corresponding bit in bitmap egr_remark_encap_mpls_bitmap should be clear.
             */
            rv = QOS_ACCESS.egr_remark_encap_l2_bitmap.bit_clear(unit, _DPP_QOS_ENTRY_INDEX_EGR_REMARK_ENP_L2(profile_ndx, map->remark_int_pri, map->color));
            BCMDNX_IF_ERR_EXIT(rv);
        }
     }
    }
    else { /* Not encap header */
        if (flags & BCM_QOS_MAP_MPLS) {
    
            if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id) {
        
                LOG_ERROR(BSL_LS_BCM_QOS,
                          (BSL_META_U(unit,
                                      "%s: invalid remark profile (%d) out of range\n"),
                                      FUNCTION_NAME(), profile_ndx));
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("REMARK profile is out of range")));
            }
            /* internal pri */
            prm_in_qos_key.remark_profile = profile_ndx;
            prm_in_qos_key.dp = dp;
            prm_in_qos_key.in_dscp_exp = (uint32) map->int_pri; 
        
            SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_qos_params_remark_get(dev_id,
                                  &prm_in_qos_key,
                                  &prm_out_qos_params));
    
            prm_out_qos_params.mpls_exp = map->remark_int_pri; 
        
            SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_qos_params_remark_set(dev_id,
                                  &prm_in_qos_key,
                                  &prm_out_qos_params));

            if (map->remark_int_pri != 0) {
                /* if entry content is non-zero, the entry is in use.
                 * corresponding bit in bitmap egr_remark_mpls_bitmap should be set.
                 */
                rv = QOS_ACCESS.egr_remark_mpls_bitmap.bit_set(unit, _DPP_QOS_ENTRY_INDEX_EGR_REMARK_MPLS(profile_ndx, prm_in_qos_key.in_dscp_exp, prm_in_qos_key.dp));
                BCMDNX_IF_ERR_EXIT(rv);
            }
            else {
                /* if entry content is zero, the entry is not in use.
                 * corresponding bit in bitmap egr_remark_mpls_bitmap should be clear.
                 */
                rv = QOS_ACCESS.egr_remark_mpls_bitmap.bit_clear(unit, _DPP_QOS_ENTRY_INDEX_EGR_REMARK_MPLS(profile_ndx, prm_in_qos_key.in_dscp_exp, prm_in_qos_key.dp));
                BCMDNX_IF_ERR_EXIT(rv);
            }

        }
        else if (flags & BCM_QOS_MAP_L3) {
        
            if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id) {
        
                LOG_ERROR(BSL_LS_BCM_QOS,
                          (BSL_META_U(unit,
                                      "%s: invalid remark profile (%d) out of range\n"),
                                      FUNCTION_NAME(), profile_ndx));
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("REMARK profile is out of range")));
            }
            /* internal pri */
            prm_in_qos_key.remark_profile = profile_ndx;
            prm_in_qos_key.dp = dp;
            prm_in_qos_key.in_dscp_exp = (uint32) map->int_pri; 
        
            SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_qos_params_remark_get(dev_id,
                                  &prm_in_qos_key,
                                  &prm_out_qos_params));
        
            /* IPV6 */
            if (flags & BCM_QOS_MAP_IPV6) {
                prm_out_qos_params.ipv6_tc = map->remark_int_pri; 
            }
            else { /* IPV4 */
                prm_out_qos_params.ipv4_tos = map->remark_int_pri; 
            }
    
            SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_qos_params_remark_set(dev_id,
                                  &prm_in_qos_key,
                                  &prm_out_qos_params));


            if (map->remark_int_pri != 0) {
                if (flags & BCM_QOS_MAP_IPV6) {
                    /* if entry content is non-zero, the entry is in use.
                     * corresponding bit in bitmap egr_remark_ipv6_bitmap should be set.
                     */
                    rv = QOS_ACCESS.egr_remark_ipv6_bitmap.bit_set(unit, _DPP_QOS_ENTRY_INDEX_EGR_REMARK_L3(profile_ndx, prm_in_qos_key.in_dscp_exp, prm_in_qos_key.dp));
                    BCMDNX_IF_ERR_EXIT(rv);
                }
                else {
                    /* if entry content is non-zero, the entry is in use.
                     * corresponding bit in bitmap egr_remark_ipv4_bitmap should be set.
                     */                    
                    rv = QOS_ACCESS.egr_remark_ipv4_bitmap.bit_set(unit, _DPP_QOS_ENTRY_INDEX_EGR_REMARK_L3(profile_ndx, prm_in_qos_key.in_dscp_exp, prm_in_qos_key.dp));
                    BCMDNX_IF_ERR_EXIT(rv);
                }
            } else {
                if (flags & BCM_QOS_MAP_IPV6) {
                    /* if entry content is zero, the entry is not in use.
                     * corresponding bit in bitmap egr_remark_ipv6_bitmap should be clear.
                     */
                    rv = QOS_ACCESS.egr_remark_ipv6_bitmap.bit_clear(unit, _DPP_QOS_ENTRY_INDEX_EGR_REMARK_L3(profile_ndx, prm_in_qos_key.in_dscp_exp, prm_in_qos_key.dp));
                    BCMDNX_IF_ERR_EXIT(rv);
                }
                else {
                    /* if entry content is zero, the entry is not in use.
                     * corresponding bit in bitmap egr_remark_ipv4_bitmap should be clear.
                     */
                    rv = QOS_ACCESS.egr_remark_ipv4_bitmap.bit_clear(unit, _DPP_QOS_ENTRY_INDEX_EGR_REMARK_L3(profile_ndx, prm_in_qos_key.in_dscp_exp, prm_in_qos_key.dp));
                    BCMDNX_IF_ERR_EXIT(rv);
                }                
            }

        } 
        else {
            rv = BCM_E_UNAVAIL;
        }
    }
    
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_petra_qos_map_egress_mpls_php_add(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    bcm_error_t rv = BCM_E_NONE;  
    int dev_id = 0;
    uint32 profile_ndx = 0;
    SOC_PPC_EG_QOS_PHP_REMARK_KEY prm_in_php_remark_qos_key;
    

    BCMDNX_INIT_FUNC_DEFS;

    dev_id = (unit);
    profile_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);

    SOC_PPC_EG_QOS_PHP_REMARK_KEY_clear(&prm_in_php_remark_qos_key);

    if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_egr_mpls_php) 
    {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: invalid MPLS PHP profile (%d) out of range\n"), FUNCTION_NAME(), profile_ndx));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid MPLS PHPs profile")));
    }
    
    if (flags & BCM_QOS_MAP_IPV6) {
          
          prm_in_php_remark_qos_key.exp = map->int_pri; /* In-DSCP-EXP before */
          prm_in_php_remark_qos_key.exp_map_profile = profile_ndx;
          prm_in_php_remark_qos_key.php_type = SOC_PPC_EG_QOS_UNIFORM_PHP_POP_INTO_IPV6;                 
          
          SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_qos_params_php_remark_set(dev_id,
                             &prm_in_php_remark_qos_key,
                             map->remark_int_pri)); /* Out-DSCP-EXP after */

        if (map->remark_int_pri != 0) {
            /* if entry content is non-zero, the entry is in use.
             * corresponding bit in bitmap egr_mpls_php_ipv6_bitmap should be set.
             */
            rv = QOS_ACCESS.egr_mpls_php_ipv6_bitmap.bit_set(unit, _DPP_QOS_ENTRY_INDEX_EGR_MPLS_PHP(profile_ndx, map->int_pri));
            BCMDNX_IF_ERR_EXIT(rv);
        }
        else {
             /* if entry content is zero, the entry is not in use.
              * corresponding bit in bitmap egr_mpls_php_ipv6_bitmap should be clear.
              */
            rv = QOS_ACCESS.egr_mpls_php_ipv6_bitmap.bit_clear(unit, _DPP_QOS_ENTRY_INDEX_EGR_MPLS_PHP(profile_ndx, map->int_pri));
            BCMDNX_IF_ERR_EXIT(rv);
        }
        
     }
     else if (flags & BCM_QOS_MAP_IPV4) 
     {
        
          prm_in_php_remark_qos_key.exp = map->int_pri; /* In-DSCP-EXP before */
          prm_in_php_remark_qos_key.exp_map_profile = profile_ndx;
          prm_in_php_remark_qos_key.php_type = SOC_PPC_EG_QOS_UNIFORM_PHP_POP_INTO_IPV4;
          
          SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_qos_params_php_remark_set(dev_id,
                             &prm_in_php_remark_qos_key,
                             map->remark_int_pri)); /* Out-DSCP-EXP after */
          
        if (map->remark_int_pri != 0) {
            /* if entry content is non-zero, the entry is in use.
             * corresponding bit in bitmap egr_mpls_php_ipv4_bitmap should be set.
             */
            rv = QOS_ACCESS.egr_mpls_php_ipv4_bitmap.bit_set(unit, _DPP_QOS_ENTRY_INDEX_EGR_MPLS_PHP(profile_ndx, map->int_pri));
            BCMDNX_IF_ERR_EXIT(rv);
        }
        else {
            /* if entry content is zero, the entry is not in use.
             * corresponding bit in bitmap egr_mpls_php_ipv4_bitmap should be clear.
             */
            rv = QOS_ACCESS.egr_mpls_php_ipv4_bitmap.bit_clear(unit, _DPP_QOS_ENTRY_INDEX_EGR_MPLS_PHP(profile_ndx, map->int_pri));
            BCMDNX_IF_ERR_EXIT(rv);
        }

     }     
     else
     {
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid MPLS PHPs flags")));
     }

     BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

#ifdef BCM_88660
static int
_bcm_petra_qos_map_egress_dscp_exp_marking_add(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    bcm_error_t rv = BCM_E_NONE, sand_rv;  
    uint32 profile_ndx = 0;
    SOC_PPC_EG_QOS_MARKING_KEY key;
    SOC_PPC_EG_QOS_MARKING_PARAMS val;
    int simple_mode;
    uint32 dp_map_disabled = 0;

    BCMDNX_INIT_FUNC_DEFS;

    profile_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);

    if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_egr_dscp_exp_marking) 
    {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: invalid DSCP/EXP marking profile (%d) out of range\n"), FUNCTION_NAME(), profile_ndx));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid DSCP/EXP marking profile")));
    }
    
    /* We don't allow any flags */
    if (flags != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid DSCP/EXP marking flags")));
    }

    if (SOC_IS_JERICHO_B0(unit)) {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_read(unit, EPNI_DP_MAP_FIX_ENABLEDr, SOC_CORE_ALL, 0, DP_MAP_FIX_ENABLEDf, &dp_map_disabled));
    } else if (SOC_IS_JERICHO_PLUS(unit)) {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_read(unit, EPNI_CFG_MARKING_DP_MAPr, SOC_CORE_ALL, 0, CFG_MARKING_DP_MAP_DISABLEf, &dp_map_disabled));
    }

    simple_mode = soc_property_get(unit, spn_BCM886XX_QOS_L3_L2_MARKING, 0);
    if (simple_mode) {
        uint32 inlif_profile_same_dscp_mask = 0;
        uint32 inlif_profile_index;

        SOC_PPC_EG_QOS_MARKING_KEY_clear(&key);
        SOC_PPC_EG_QOS_MARKING_PARAMS_clear(&val);

        key.resolved_dp_ndx = map->remark_color;
        key.tc_ndx = map->int_pri;
        key.marking_profile = profile_ndx;
        key.dp_map_disabled = dp_map_disabled;

        val.ip_dscp = map->dscp;
        val.mpls_exp = map->exp;

        /* get mask of inlif profile with dscp marking set */
        sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_set, (unit, SOC_OCC_MGMT_TYPE_INLIF, SOC_OCC_MGMT_INLIF_APP_SIMPLE_DSCP_MARKING, /*value*/1,&inlif_profile_same_dscp_mask));
        SOCDNX_IF_ERR_EXIT(sand_rv);

        /* Set this mapping for all the relevant bits */
        for (inlif_profile_index = 0; inlif_profile_index < SOC_OCC_MGMT_NOF_INLIF_PROFILES_TRANSFERED_TO_EGRESS; inlif_profile_index++) {
            if (inlif_profile_index & inlif_profile_same_dscp_mask) {
                key.in_lif_profile = inlif_profile_index;

                SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_qos_params_marking_set(unit, &key, &val));
            }
        }
    } else {
        SOC_PPC_EG_QOS_MARKING_KEY_clear(&key);
        SOC_PPC_EG_QOS_MARKING_PARAMS_clear(&val);

        key.resolved_dp_ndx = map->remark_color;
        key.tc_ndx = map->int_pri;
        key.in_lif_profile = map->port_offset;
        key.marking_profile = profile_ndx;
        key.dp_map_disabled = dp_map_disabled;

        val.ip_dscp = map->dscp;
        val.mpls_exp = map->exp;

        SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_qos_params_marking_set(unit, &key, &val));

    }
    if ((map->dscp + map->exp) != 0) {
        /* if entry content is non-zero, the entry is in use.
         * corresponding bit in bitmap egr_dscp_exp_marking_bitmap should be set.
         */
        rv = QOS_ACCESS.egr_dscp_exp_marking_bitmap.bit_set(unit, 
            _DPP_QOS_ENTRY_INDEX_EGR_DSCP_EXP_MARKING(profile_ndx, key.in_lif_profile, key.tc_ndx, key.resolved_dp_ndx));
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else {
        /* if entry content is zero, the entry is not in use.
         * corresponding bit in bitmap egr_dscp_exp_marking_bitmap should be clear.
         */
        rv = QOS_ACCESS.egr_dscp_exp_marking_bitmap.bit_clear(unit, 
            _DPP_QOS_ENTRY_INDEX_EGR_DSCP_EXP_MARKING(profile_ndx, key.in_lif_profile, key.tc_ndx, key.resolved_dp_ndx));
        BCMDNX_IF_ERR_EXIT(rv);
    }



exit:
    BCMDNX_FUNC_RETURN;
}


#endif /* BCM_88660 */


STATIC int _bcm_petra_qos_map_egress_oam_pcp_add(int unit, uint32 flags, bcm_qos_map_t *map, int map_id) {
    int rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;
    

    rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_egress_pcp_set_by_profile_and_tc, (unit,map->int_pri, BCM_QOS_MAP_PROFILE_GET(map_id), map->pkt_pri));
    BCMDNX_IF_ERR_EXIT(rv); 

exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int _bcm_petra_qos_map_encap_intpri_color_add(int unit, uint32 flags, bcm_qos_map_t *map, int map_id) 
{
    int rv = BCM_E_NONE;
    int profile_ndx = 0;
    SOC_PPC_EG_MAP_ENCAP_INTPRI_COLOR_INFO entry_info;
    BCMDNX_INIT_FUNC_DEFS;
    SOC_PPC_EG_MAP_ENCAP_INTPRI_COLOR_INFO_clear(&entry_info);
    
    profile_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);
 
    if(map->color != bcmColorPreserve)
    {
        rv = _bcm_petra_port_color_encode(unit,map->color,(int *)&entry_info.color);
        BCMDNX_IF_ERR_EXIT(rv);
        entry_info.color_valid = 1;
    }
    if(map->int_pri != BCM_PRIO_PRESERVE) 
    {
        entry_info.int_pri = map->int_pri;
        entry_info.int_pri_valid = 1;
    }

    rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_eg_encap_map_encap_intpri_color_set, (unit,profile_ndx, &entry_info));
    BCMDNX_IF_ERR_EXIT(rv); 

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int _bcm_petra_qos_map_encap_intpri_color_get(int unit, uint32 flags, int map_id , bcm_qos_map_t *map) 
{
    int rv = BCM_E_NONE;
    int profile_ndx = 0;
    SOC_PPC_EG_MAP_ENCAP_INTPRI_COLOR_INFO entry_info;
    BCMDNX_INIT_FUNC_DEFS;
    SOC_PPC_EG_MAP_ENCAP_INTPRI_COLOR_INFO_clear(&entry_info);
    
    profile_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);
    
    rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_eg_encap_map_encap_intpri_color_get, (unit,profile_ndx, &entry_info));
    BCMDNX_IF_ERR_EXIT(rv); 


    if(entry_info.color_valid == 0)
    { 
        map->color = bcmColorPreserve;
    }
    else
    {
        rv = _bcm_petra_port_color_decode(unit, entry_info.color_valid, &(map->color));
        BCMDNX_IF_ERR_EXIT(rv); 
    }
    if(entry_info.int_pri_valid == 0) 
    {  
        map->int_pri = BCM_PRIO_PRESERVE;
    }
    else
    {
        map->int_pri = entry_info.int_pri;
    }


exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int _bcm_petra_qos_map_encap_intpri_color_delete(int unit, uint32 flags, int map_id) 
{
    int rv = BCM_E_NONE;
    int profile_ndx = 0;
    SOC_PPC_EG_MAP_ENCAP_INTPRI_COLOR_INFO entry_info;
    BCMDNX_INIT_FUNC_DEFS;

    /* we write zero's into the HW*/
    SOC_PPC_EG_MAP_ENCAP_INTPRI_COLOR_INFO_clear(&entry_info);
    
    profile_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);
    
    rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_eg_encap_map_encap_intpri_color_set, (unit,profile_ndx, &entry_info));
    BCMDNX_IF_ERR_EXIT(rv); 

exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_petra_qos_map_egress_pcp_vlan_add(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    bcm_error_t rv = BCM_E_NONE;
    int dev_id = 0, dp = 0;
    uint32 profile_ndx = 0;
    SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY key_mapping;
    int count1 = 0, count2 = 0, count3 = 0;

    BCMDNX_INIT_FUNC_DEFS;
    dev_id = (unit);
    profile_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);

    BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_encode(unit,map->color,&dp));

    if ((flags & BCM_QOS_MAP_L2_VLAN_PCP) == 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("flag BCM_QOS_MAP_L2_VLAN_PCP must be set")));
    }

    if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_egr_pcp_vlan) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: invalid egress vlan pcp profile (%d) out of range\n"), FUNCTION_NAME(), profile_ndx));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid PCP VLAN profile")));
    }

    if (flags & BCM_QOS_MAP_L2) {

        if (flags & BCM_QOS_MAP_MIM_ITAG) {
            SOC_SAND_IF_ERROR_RETURN(soc_ppd_frwrd_bmact_eg_vlan_pcp_map_set(dev_id, profile_ndx, map->int_pri,
                                     dp, map->pkt_pri, map->pkt_cfi));

        }
        else if (flags & BCM_QOS_MAP_L2_UNTAGGED) {

            key_mapping =  SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY_TC_DP;
            SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_profile_info_set(dev_id, 
                                       profile_ndx, 
                                       key_mapping));

            SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_map_untagged_set(dev_id, 
                                       profile_ndx, 
                                       map->int_pri, 
                                       dp, 
                                       map->pkt_pri, 
                                       map->pkt_cfi));

            if ((map->pkt_pri + map->pkt_cfi) != 0) {
                /* if entry content is non-zero, the entry is in use.
                 * corresponding bit in bitmap egr_pcp_vlan_utag_bitmap should be set.
                 */
                rv = QOS_ACCESS.egr_pcp_vlan_utag_bitmap.bit_set(unit, _DPP_QOS_ENTRY_INDEX_EGR_PCP_VLAN_UTAG(profile_ndx, map->int_pri, dp));
                BCMDNX_IF_ERR_EXIT(rv);
            }
            else {
                /* if entry content is zero, the entry is not in use.
                 * corresponding bit in bitmap egr_pcp_vlan_utag_bitmap should be clear.
                 */
                rv = QOS_ACCESS.egr_pcp_vlan_utag_bitmap.bit_clear(unit, _DPP_QOS_ENTRY_INDEX_EGR_PCP_VLAN_UTAG(profile_ndx, map->int_pri, dp));
                BCMDNX_IF_ERR_EXIT(rv);
            }

        } 
        else if (flags & BCM_QOS_MAP_L2_INNER_TAG) {

            key_mapping =  SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY_PCP;
            SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_profile_info_set(dev_id, 
                                       profile_ndx, 
                                       key_mapping));

            SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_map_ctag_set(dev_id, 
                                   profile_ndx, 
                                   map->int_pri, 
                                   map->pkt_pri, 
                                   map->pkt_cfi));

            if ((map->pkt_pri + map->pkt_cfi) != 0) {
                /* if entry content is non-zero, the entry is in use.
                 * corresponding bit in bitmap egr_pcp_vlan_ctag_bitmap should be set.
                 */
                rv = QOS_ACCESS.egr_pcp_vlan_ctag_bitmap.bit_set(unit, _DPP_QOS_ENTRY_INDEX_EGR_PCP_VLAN_CTAG(profile_ndx, map->int_pri));
                BCMDNX_IF_ERR_EXIT(rv);
            }
            else {
                /* if entry content is zero, the entry is not in use.
                 * corresponding bit in bitmap egr_pcp_vlan_ctag_bitmap should be clear.
                 */
                rv = QOS_ACCESS.egr_pcp_vlan_ctag_bitmap.bit_clear(unit, _DPP_QOS_ENTRY_INDEX_EGR_PCP_VLAN_CTAG(profile_ndx, map->int_pri));
                BCMDNX_IF_ERR_EXIT(rv);
            }
            
        } 
        else { /* OUTER TAG */

            key_mapping =  SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY_PCP;
            SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_profile_info_set(dev_id, 
                                       profile_ndx, 
                                       key_mapping));

        
            SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_map_stag_set(dev_id, 
                                   profile_ndx, 
                                   map->int_pri, 
                                   map->color, 
                                   map->pkt_pri, 
                                   map->pkt_cfi));

            if ((map->pkt_pri + map->pkt_cfi) != 0) {
                /* if entry content is non-zero, the entry is in use.
                 * corresponding bit in bitmap egr_pcp_vlan_ctag_bitmap should be set.
                 */
                rv = QOS_ACCESS.egr_pcp_vlan_stag_bitmap.bit_set(unit, _DPP_QOS_ENTRY_INDEX_EGR_PCP_VLAN_STAG(profile_ndx, map->int_pri, map->color));
                BCMDNX_IF_ERR_EXIT(rv);
            }
            else {
                /* if entry content is zero, the entry is not in use.
                 * corresponding bit in bitmap egr_pcp_vlan_stag_bitmap should be clear.
                 */
                rv = QOS_ACCESS.egr_pcp_vlan_stag_bitmap.bit_clear(unit, _DPP_QOS_ENTRY_INDEX_EGR_PCP_VLAN_STAG(profile_ndx, map->int_pri, map->color));
                BCMDNX_IF_ERR_EXIT(rv);
            }
            
        }
    } 
    else if (flags & BCM_QOS_MAP_MPLS) { 
        /* Arad only */
        if ( SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_enable != 1) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: dscp_exp mapping is disabled. \n"), FUNCTION_NAME()));  
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("DSCP_EXP mapping is disabled")));
        }
        if ( SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_profile_id == -1) {  /* no profile is set to dscp_exp mapping yet */
            SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_profile_id = profile_ndx;
        }
        if (profile_ndx !=  SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_profile_id) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: invalid egress vlan pcp profile (%d). profile ndx for dscp_exp mapping is set to (%d). \n"), FUNCTION_NAME(), SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_enable, profile_ndx)); 
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("Invalid PCP VLAN profile")));
        }
        key_mapping =  SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY_DSCP_EXP;
        SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_profile_info_set(dev_id, 
                                       profile_ndx, 
                                       key_mapping));

        SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_map_dscp_exp_set(dev_id, 
                               SOC_PPC_PKT_FRWRD_TYPE_MPLS, 
                               map->exp,
                               map->pkt_pri, 
                               map->pkt_cfi));
        
        if ((map->pkt_pri + map->pkt_cfi) != 0) {
            /* if entry content is non-zero, the entry is in use.
             * corresponding bit in bitmap egr_pcp_vlan_mpls_bitmap should be set.
             */
            rv = QOS_ACCESS.egr_pcp_vlan_mpls_bitmap.bit_set(unit, _DPP_QOS_ENTRY_INDEX_EGR_PCP_VLAN_MPLS(profile_ndx, map->exp));
            BCMDNX_IF_ERR_EXIT(rv);
        }
        else {
            /* if entry content is zero, the entry is not in use.
             * corresponding bit in bitmap egr_pcp_vlan_mpls_bitmap should be clear.
             */
            rv = QOS_ACCESS.egr_pcp_vlan_mpls_bitmap.bit_clear(unit, _DPP_QOS_ENTRY_INDEX_EGR_PCP_VLAN_MPLS(profile_ndx, map->exp));
            BCMDNX_IF_ERR_EXIT(rv);

            _DPP_QOS_ENTRY_USED_CNT(egr_pcp_vlan_mpls_bitmap, profile_ndx, _BCM_QOS_MAP_EGR_EXP_MAX, count1);
            if (count1 == 0) {
                _DPP_QOS_ENTRY_USED_CNT(egr_pcp_vlan_ipv6_bitmap, profile_ndx, _BCM_QOS_MAP_EGR_DSCP_MAX, count2);
                _DPP_QOS_ENTRY_USED_CNT(egr_pcp_vlan_ipv4_bitmap, profile_ndx, _BCM_QOS_MAP_EGR_DSCP_MAX, count3);
                if ((count2 == 0) && (count3 == 0)) {
                    SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_profile_id = -1; 
                }
            }
        }
        
    } 
    else if ((flags & BCM_QOS_MAP_L3) && (flags & BCM_QOS_MAP_IPV6)) {
          /* Arad only */
          if ( SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_enable != 1) {
              LOG_ERROR(BSL_LS_BCM_QOS,
                        (BSL_META_U(unit,
                                    "%s: dscp_exp mapping is disabled. \n"), FUNCTION_NAME()));  
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("DSCP_EXP mapping is disabled")));
          }
          if ( SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_profile_id == -1) {  /* no profile is set to dscp_exp mapping yet */
              SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_profile_id = profile_ndx;
          }
          if (profile_ndx !=  SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_profile_id) {
                LOG_ERROR(BSL_LS_BCM_QOS,
                          (BSL_META_U(unit,
                                      "%s: invalid egress vlan pcp profile (%d). profile ndx for dscp_exp mapping is set to (%d). \n"), FUNCTION_NAME(), SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_enable, profile_ndx)); 
                BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("Invalid PCP VLAN profile")));
          }
          key_mapping =  SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY_DSCP_EXP;
          SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_profile_info_set(dev_id, 
                           profile_ndx, 
                           key_mapping));

          SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_map_dscp_exp_set(dev_id, 
                     SOC_PPC_PKT_FRWRD_TYPE_IPV6_UC, 
                     map->dscp,
                                   map->pkt_pri, 
                                   map->pkt_cfi));
          SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_map_dscp_exp_set(dev_id, 
                     SOC_PPC_PKT_FRWRD_TYPE_IPV6_MC, 
                     map->dscp,
                                   map->pkt_pri, 
                                   map->pkt_cfi));

          if ((map->pkt_pri + map->pkt_cfi) != 0) {
              /* if entry content is non-zero, the entry is in use.
               * corresponding bit in bitmap egr_pcp_vlan_ipv6_bitmap should be set.
               */
              rv = QOS_ACCESS.egr_pcp_vlan_ipv6_bitmap.bit_set(unit, _DPP_QOS_ENTRY_INDEX_EGR_PCP_VLAN_L3(profile_ndx, map->dscp));
              BCMDNX_IF_ERR_EXIT(rv);
          }
          else {
              /* if entry content is zero, the entry is not in use.
               * corresponding bit in bitmap egr_pcp_vlan_ipv6_bitmap should be clear.
               */
              rv = QOS_ACCESS.egr_pcp_vlan_ipv6_bitmap.bit_clear(unit, _DPP_QOS_ENTRY_INDEX_EGR_PCP_VLAN_L3(profile_ndx, map->dscp));
              BCMDNX_IF_ERR_EXIT(rv);
              
              _DPP_QOS_ENTRY_USED_CNT(egr_pcp_vlan_ipv6_bitmap, profile_ndx, _BCM_QOS_MAP_EGR_DSCP_MAX, count1);
              if (count1 == 0) {
                  _DPP_QOS_ENTRY_USED_CNT(egr_pcp_vlan_ipv4_bitmap, profile_ndx, _BCM_QOS_MAP_EGR_DSCP_MAX, count2);
                  _DPP_QOS_ENTRY_USED_CNT(egr_pcp_vlan_mpls_bitmap, profile_ndx, _BCM_QOS_MAP_EGR_EXP_MAX, count3);
                  if ((count2 == 0) && (count3 == 0)) {
                      SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_profile_id = -1; 
                  }
              }              
          }
          
     }
     else if ((flags & BCM_QOS_MAP_L3) && (flags & BCM_QOS_MAP_IPV4)) {
          /* Arad only */

          if ( SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_enable != 1) {
              LOG_ERROR(BSL_LS_BCM_QOS,
                        (BSL_META_U(unit,
                                    "%s: dscp_exp mapping is disabled. \n"), FUNCTION_NAME()));  
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("DSCP_EXP mapping is disabled")));
          }
          if ( SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_profile_id == -1) {  /*no profile is set to dscp_exp mapping yet*/
              SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_profile_id = profile_ndx;
          }
          if (profile_ndx !=  SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_profile_id) {
                LOG_ERROR(BSL_LS_BCM_QOS,
                          (BSL_META_U(unit,
                                      "%s: invalid egress vlan pcp profile (%d). profile ndx for dscp_exp mapping is set to (%d). \n"), FUNCTION_NAME(), SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_enable, profile_ndx)); 
                BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("Invalid PCP VLAN profile")));
          }
          key_mapping =  SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY_DSCP_EXP;
          SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_profile_info_set(dev_id, 
                           profile_ndx, 
                           key_mapping));

          SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_map_dscp_exp_set(dev_id, 
                     SOC_PPC_PKT_FRWRD_TYPE_IPV4_UC, 
                     map->dscp,
                                   map->pkt_pri, 
                                   map->pkt_cfi));
          SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_map_dscp_exp_set(dev_id, 
                     SOC_PPC_PKT_FRWRD_TYPE_IPV4_MC, 
                     map->dscp,
                                   map->pkt_pri, 
                                   map->pkt_cfi));
          
          if ((map->pkt_pri + map->pkt_cfi) != 0) {
              /* if entry content is non-zero, the entry is in use.
               * corresponding bit in bitmap egr_pcp_vlan_ipv4_bitmap should be set.
               */
              rv = QOS_ACCESS.egr_pcp_vlan_ipv4_bitmap.bit_set(unit, _DPP_QOS_ENTRY_INDEX_EGR_PCP_VLAN_L3(profile_ndx, map->dscp));
              BCMDNX_IF_ERR_EXIT(rv);
          }
          else {
              /* if entry content is zero, the entry is not in use.
               * corresponding bit in bitmap egr_pcp_vlan_ipv4_bitmap should be clear.
               */
              rv = QOS_ACCESS.egr_pcp_vlan_ipv4_bitmap.bit_clear(unit, _DPP_QOS_ENTRY_INDEX_EGR_PCP_VLAN_L3(profile_ndx, map->dscp));
              BCMDNX_IF_ERR_EXIT(rv);

              _DPP_QOS_ENTRY_USED_CNT(egr_pcp_vlan_ipv4_bitmap, profile_ndx, _BCM_QOS_MAP_EGR_DSCP_MAX, count1);
              if (count1 == 0) {
                  _DPP_QOS_ENTRY_USED_CNT(egr_pcp_vlan_ipv6_bitmap, profile_ndx, _BCM_QOS_MAP_EGR_DSCP_MAX, count2);
                  _DPP_QOS_ENTRY_USED_CNT(egr_pcp_vlan_mpls_bitmap, profile_ndx, _BCM_QOS_MAP_EGR_EXP_MAX, count3);
                  if ((count2 == 0) && (count3 == 0)) {
                      SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_profile_id = -1; 
                  }
              }               
          }
     } else {
         rv = BCM_E_PARAM;
     }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_petra_qos_map_egress_l2_i_tag_add(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_NONE;
    int dev_id = 0, dp = 0;
    uint32 profile_ndx = 0;

    BCMDNX_INIT_FUNC_DEFS;
    dev_id = (unit);
    profile_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);

    BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_encode(unit,map->color,&dp));

    if ((flags & BCM_QOS_MAP_L2_VLAN_PCP) == 0) {
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("flag BCM_QOS_MAP_L2_VLAN_PCP must be set")));
    }

    if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_egr_l2_i_tag) {
    
    LOG_ERROR(BSL_LS_BCM_QOS,
              (BSL_META_U(unit,
                          "%s: invalid egress egress l2 i tag profile (%d) out of range\n"),
                          FUNCTION_NAME(), profile_ndx));
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid L2 I-TAG profile")));
    }


    if (flags & BCM_QOS_MAP_MIM_ITAG) {
        SOC_SAND_IF_ERROR_RETURN(soc_ppd_frwrd_bmact_eg_vlan_pcp_map_set(dev_id, profile_ndx, map->int_pri,
                                   dp, map->pkt_pri, map->pkt_cfi));

        if ((map->pkt_pri + map->pkt_cfi) != 0) {
            /* if entry content is non-zero, the entry is in use.
             * corresponding bit in bitmap egr_l2_i_tag_bitmap should be set.
             */
            rv = QOS_ACCESS.egr_l2_i_tag_bitmap.bit_set(unit, _DPP_QOS_ENTRY_INDEX_EGR_L2_I_TAG(profile_ndx, map->int_pri, dp));
            BCMDNX_IF_ERR_EXIT(rv);
        }
        else {
            /* if entry content is zero, the entry is not in use.
             * corresponding bit in bitmap egr_l2_i_tag_bitmap should be clear.
             */
            rv = QOS_ACCESS.egr_l2_i_tag_bitmap.bit_clear(unit, _DPP_QOS_ENTRY_INDEX_EGR_L2_I_TAG(profile_ndx, map->int_pri, dp));
            BCMDNX_IF_ERR_EXIT(rv);
        }

    } else {
        rv = BCM_E_PARAM;
    }
     
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int 
_bcm_petra_qos_map_ingress_add(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    if (BCM_INT_QOS_MAP_IS_LIF_COS(map_id)) { /* L3 */
        rv = _bcm_petra_qos_map_ingress_lif_cos_add(unit, flags, map, map_id);
    } 
    else if (BCM_INT_QOS_MAP_IS_INGRESS_PCP_VLAN(map_id)) { /* L2 */
        rv = _bcm_petra_qos_map_ingress_pcp_vlan_add(unit, flags, map, map_id);
    } 
    else if (BCM_INT_QOS_MAP_IS_MPLS_ELSP(map_id)) {
        rv = _bcm_petra_qos_map_ingress_elsp_add(unit, flags, map, map_id);
    } 
    else if (BCM_INT_QOS_MAP_IS_OPCODE(map_id)) {
        rv = _bcm_petra_qos_map_ingress_cos_opcode_add(unit, flags, map, map_id);
    }
    else if (BCM_INT_QOS_MAP_IS_INGRESS_COLOR(map_id)) { /* DP  */
        rv = _bcm_petra_qos_map_ingress_color_add(unit, flags, map, map_id);
    } 
    else {
        rv = BCM_E_UNAVAIL;
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int 
_bcm_petra_qos_map_egress_add(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    if (BCM_INT_QOS_MAP_IS_REMARK(map_id)) {
        rv = _bcm_petra_qos_map_egress_remark_add(unit, flags, map, map_id);
    BCMDNX_IF_ERR_EXIT(rv);

    } else if (BCM_INT_QOS_MAP_IS_EGRESS_PCP_VLAN(map_id)) {
        rv = _bcm_petra_qos_map_egress_pcp_vlan_add(unit, flags, map, map_id);
    } else if (BCM_INT_QOS_MAP_IS_EGRESS_L2_I_TAG(map_id)) {
        rv = _bcm_petra_qos_map_egress_l2_i_tag_add(unit, flags, map, map_id);
    } else if (BCM_INT_QOS_MAP_IS_EGRESS_MPLS_PHP(map_id)) {
        rv = _bcm_petra_qos_map_egress_mpls_php_add(unit, flags, map, map_id);
#ifdef BCM_88660
    } else if (SOC_IS_ARADPLUS(unit) && BCM_INT_QOS_MAP_IS_EGRESS_DSCP_EXP_MARKING(map_id)) {
        rv = _bcm_petra_qos_map_egress_dscp_exp_marking_add(unit, flags, map, map_id);
#endif /* BCM_88660*/
    }  else if (SOC_IS_JERICHO(unit) && BCM_INT_QOS_MAP_IS_OAM_EGRESS_PROFILE(map_id)) {
        rv = _bcm_petra_qos_map_egress_oam_pcp_add(unit, flags, map, map_id);
    }
    else if (SOC_IS_JERICHO(unit) && BCM_INT_QOS_MAP_ENCAP_INTPRI_COLOR(map_id))
    {
        rv = _bcm_petra_qos_map_encap_intpri_color_add(unit, flags, map, map_id);
    }
    else {
        rv = BCM_E_UNAVAIL;
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


int 
bcm_petra_qos_map_add(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    if (map == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("bcm_qos_map_t * is NULL")));
    }

    rv = _bcm_petra_qos_map_add_params_verify(unit, flags, map, map_id);
    BCMDNX_IF_ERR_EXIT(rv);

    if (BCM_INT_QOS_MAP_IS_INGRESS(map_id))  {
        rv = _bcm_petra_qos_map_ingress_add(unit, flags, map, map_id);
    } else if (BCM_INT_QOS_MAP_IS_EGRESS(map_id)) {
        rv = _bcm_petra_qos_map_egress_add(unit, flags, map, map_id);
    }
    BCMDNX_IF_ERR_EXIT(rv);
 exit:
    BCMDNX_FUNC_RETURN;
}

/* bcm_petra_qos_map_multi_get */
int
bcm_petra_qos_map_multi_get(int unit, uint32 flags,
                int map_id, int array_size, 
                bcm_qos_map_t *array, int *array_count)
{
    int rv = BCM_E_NONE, sand_rv;
    int idx = 0, num_entries = 0, dev_id = 0;
    int exp_cnt = 0, tc_cnt = 0, dp_cnt = 0, dscp_cnt = 0, pri_cfi_cnt = 0, int_pri_cnt = 0, tc_dp_cnt = 0, int_cfi = 0, pcp_dei;
    int exp = 0, dscp = 0, outer_tpid_index = 0, is_pkt_valid = 0, is_qos_only = 0;
    SOC_PPC_PKT_FRWRD_TYPE pkt_hdr_type;
    SOC_PPC_LIF_COS_OPCODE_TYPE opcode_type;
    bcm_color_t tmp_color;
    bcm_qos_map_t *map;
    uint32 profile_ndx = 0;
    uint32 tmp_dscp_exp = 0;
#ifdef BCM_88660
    int resolved_dp_idx, tc_idx, inlif_idx;
    uint32 dp_map_disabled = 0;
#endif /* BCM_88660 */
    SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY map_entry;
    SOC_PPC_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY mpls_key;
    SOC_PPC_LIF_COS_PROFILE_MAP_TBL_IP_KEY ip_key;
    SOC_PPC_LIF_COS_PROFILE_MAP_TBL_L2_KEY l2_key;
    SOC_PPC_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY tc_dp_key;
    SOC_PPC_FRWRD_ILM_GLBL_INFO glbl_info;
    SOC_PPC_EG_QOS_MAP_KEY prm_in_qos_key;
    SOC_PPC_EG_QOS_PARAMS  prm_out_qos_params;
    SOC_PPC_EG_ENCAP_QOS_MAP_KEY prm_in_encap_qos_key;
    SOC_PPC_EG_ENCAP_QOS_PARAMS  prm_out_encap_qos_params;
    SOC_PPC_EG_QOS_PHP_REMARK_KEY prm_in_qos_php_key;
    SOC_PPC_LIF_COS_OPCODE_ACTION_INFO action_info;
#ifdef BCM_88660
    SOC_PPC_EG_QOS_MARKING_KEY marking_key;
    SOC_PPC_EG_QOS_MARKING_PARAMS marking_params;
#endif /* BCM_88660 */

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    
    dev_id = (unit);
    profile_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);
    is_pkt_valid = (flags & BCM_QOS_MAP_PACKET_INVALID) ? FALSE : TRUE;
    is_qos_only = (flags & BCM_QOS_MAP_IGNORE_OFFSET) ? TRUE : FALSE;

    if (array_count == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("array_count is NULL")));
    }
    
    if (BCM_INT_QOS_MAP_IS_INGRESS_PCP_VLAN(map_id)) {
        if (flags & (BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_VLAN_PCP)) {
            if (flags & BCM_QOS_MAP_L2_INNER_TAG) {
                num_entries = _BCM_QOS_MAP_ING_VLAN_PCP_CTAG_MAX;
            } 
            else if (flags & BCM_QOS_MAP_L2_UNTAGGED) {
                num_entries = _BCM_QOS_MAP_ING_VLAN_PCP_UTAG_MAX;
            } 
            else if (flags & BCM_QOS_MAP_L2_OUTER_TAG) {
                num_entries = _BCM_QOS_MAP_ING_VLAN_PCP_STAG_MAX;
            } 
        }
    } 
    else if (BCM_INT_QOS_MAP_IS_LIF_COS(map_id)) {
        if (flags & BCM_QOS_MAP_MPLS) {
            num_entries = _BCM_QOS_MAP_ING_MPLS_EXP_MAX;
        } 
        else if ((flags & BCM_QOS_MAP_L3) && (flags & (BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_IPV6))) {
            num_entries = _BCM_QOS_MAP_ING_L3_DSCP_MAX;
        } 
        else if ((flags & BCM_QOS_MAP_L2) && 
                   (flags & (BCM_QOS_MAP_L2_OUTER_TAG | BCM_QOS_MAP_L2_INNER_TAG))) {
            num_entries = _BCM_QOS_MAP_ING_L2_PRI_CFI_MAX;
        }
        else if ((flags & BCM_QOS_MAP_L2_UNTAGGED)) {
            num_entries = _BCM_QOS_MAP_TC_DP_MAX;
        }
#ifdef BCM_88660_A0
        else if ((SOC_IS_ARADPLUS(unit)) && (flags & BCM_QOS_MAP_L3_L2)) {
            num_entries = 0;
        }
#endif /* BCM_88660_A0 */
    } 
    else if (BCM_INT_QOS_MAP_IS_MPLS_ELSP(map_id)) {
        num_entries = _BCM_QOS_MAP_ING_MPLS_EXP_MAX;
    } 
    else if (BCM_INT_QOS_MAP_IS_EGRESS_PCP_VLAN(map_id)) {
        if (flags & BCM_QOS_MAP_ENCAP) { 
            if (flags & BCM_QOS_MAP_MPLS) {
                num_entries = _BCM_QOS_MAP_EGR_EXP_MAX;
            } 
            else if (flags & BCM_QOS_MAP_L3) {
                num_entries = _BCM_QOS_MAP_EGR_DSCP_MAX;
            }
        }
        else if ((flags & BCM_QOS_MAP_L2) && (flags & BCM_QOS_MAP_L2_VLAN_PCP)) {
            if (flags & BCM_QOS_MAP_L2_INNER_TAG) {
                num_entries = _BCM_QOS_MAP_EGR_VLAN_PRI_MAX;
            } else if (flags & BCM_QOS_MAP_L2_UNTAGGED) {
                num_entries = _BCM_QOS_MAP_EGR_VLAN_PRI_MAX * SOC_DPP_CONFIG(unit)->qos.dp_max;
            } else { /* outer */ 
                num_entries = _BCM_QOS_MAP_EGR_VLAN_PRI_MAX * _BCM_QOS_MAP_EGR_VLAN_CFI_MAX;
            }
        }
    } 
    else if (BCM_INT_QOS_MAP_IS_EGRESS_L2_I_TAG(map_id)) {
        num_entries = _BCM_QOS_MAP_EGR_VLAN_PRI_MAX * SOC_DPP_CONFIG(unit)->qos.dp_max;
    } 
#ifdef BCM_88660
    else if (SOC_IS_ARADPLUS(unit) && BCM_INT_QOS_MAP_IS_EGRESS_DSCP_EXP_MARKING(map_id)) {
        num_entries = _BCM_QOS_MAP_EGR_RESOLVED_DP_MAX * _BCM_QOS_MAP_TC_MAX * SOC_OCC_MGMT_NOF_INLIF_PROFILES_TRANSFERED_TO_EGRESS;
    }
#endif
    else if (BCM_INT_QOS_MAP_IS_EGRESS_MPLS_PHP(map_id)) {
        num_entries = _BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX;
    } 
    else if (BCM_INT_QOS_MAP_IS_REMARK(map_id)) {
        if (flags & BCM_QOS_MAP_ENCAP) {
            if (flags & BCM_QOS_MAP_MPLS) {
                num_entries =  _BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX;
            } 
            else if ((flags & BCM_QOS_MAP_L3) && (flags & (BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_IPV6))) {
                num_entries =  _BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX;
            }
            else if ((flags & BCM_QOS_MAP_L2) && (SOC_IS_ARAD_B0_AND_ABOVE(unit))) { /* L2 */ 
                num_entries = _BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX * SOC_DPP_CONFIG(unit)->qos.dp_max;
            }
        } 
        else {
            if (flags & BCM_QOS_MAP_MPLS) {
                num_entries =  _BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX * SOC_DPP_CONFIG(unit)->qos.dp_max;
            } else if ((flags & (BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4)) ||
                       (flags & (BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV6))) {
                num_entries =  _BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX;
            } 
        }
    } 
    else if (BCM_INT_QOS_MAP_IS_OPCODE(map_id)) {
        sand_rv = soc_ppd_lif_cos_opcode_types_get(dev_id, profile_ndx, &opcode_type);
        BCM_SAND_IF_ERR_EXIT(sand_rv);

        if (opcode_type & SOC_PPC_LIF_COS_OPCODE_TYPE_L3) {
            if ((flags & BCM_QOS_MAP_IPV4) | (flags & BCM_QOS_MAP_IPV6)) {
                num_entries = _BCM_QOS_MAP_ING_COS_OPCODE_L3_MAX;
            } else {
                num_entries = _BCM_QOS_MAP_ING_COS_OPCODE_TC_DP_MAX;
            }
        } else if (opcode_type & SOC_PPC_LIF_COS_OPCODE_TYPE_L2) {
            num_entries = _BCM_QOS_MAP_ING_L2_PRI_CFI_MAX;
        } else if (opcode_type & SOC_PPC_LIF_COS_OPCODE_TYPE_TC_DP) {
            num_entries = _BCM_QOS_MAP_ING_COS_OPCODE_TC_DP_MAX;
        } else {
            num_entries = 0;
        }
    } else if (BCM_INT_QOS_MAP_IS_INGRESS_COLOR(map_id)) {
        num_entries = _BCM_QOS_MAP_ING_DP_ENTRY_MAX;
    } 
    
    if (num_entries == 0 && !(BCM_INT_QOS_MAP_ENCAP_INTPRI_COLOR(map_id))) {
        *array_count = num_entries;
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("For this type of map_id and these flags, there are no entries to get")));
    }
    
    if (array_size == 0) { /* querying the size of map */
        *array_count = num_entries;
        BCM_EXIT;
    }
    
    if (array == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("array is NULL")));
    }
    
    if (BCM_INT_QOS_MAP_IS_LIF_COS(map_id)) {
        if (flags & BCM_QOS_MAP_MPLS) {
            for (exp_cnt = 0; exp_cnt < _BCM_QOS_MAP_ING_MPLS_EXP_MAX; exp_cnt++) {
                map = &array[idx++];
                bcm_qos_map_t_init(map);
                SOC_PPC_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY_clear(&mpls_key);
        
                mpls_key.in_exp = exp_cnt;
                SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_cos_profile_map_mpls_label_info_get(dev_id,
                                         profile_ndx,
                                         &mpls_key,
                                         &map_entry));
                map->exp = exp_cnt;
                map->int_pri = map_entry.tc;
                map->remark_int_pri = map_entry.dscp;
                BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_decode(unit, map_entry.dp, &(map->color)));
        
                if ((idx >= num_entries) || (idx >= array_size)) {
                    break;
                }
            }
        } 
        else if (flags & BCM_QOS_MAP_L3) {
            for (dscp_cnt = 0; dscp_cnt < _BCM_QOS_MAP_ING_L3_DSCP_MAX; dscp_cnt++) {
                map = &array[idx++];
                bcm_qos_map_t_init(map);
                SOC_PPC_LIF_COS_PROFILE_MAP_TBL_IP_KEY_clear(&ip_key);
        
                ip_key.ip_type = (flags & BCM_QOS_MAP_IPV6) ? SOC_SAND_PP_IP_TYPE_IPV6 : SOC_SAND_PP_IP_TYPE_IPV4;
                ip_key.tos = dscp_cnt;
                SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_cos_profile_map_ip_info_get(dev_id,
                                     profile_ndx,
                                     &ip_key,
                                     &map_entry));
                map->dscp = dscp_cnt;
                map->int_pri = map_entry.tc;
                map->remark_int_pri = map_entry.dscp;
                BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_decode(unit, map_entry.dp, &(map->color)));
        
                if ((idx >= num_entries) || (idx >= array_size)) {
                    break;
                }
            }
        } 
        else if (flags & BCM_QOS_MAP_L2 && !(flags & BCM_QOS_MAP_L2_UNTAGGED)) { /* Tag exist */
            for (pri_cfi_cnt = 0; pri_cfi_cnt <  _BCM_QOS_MAP_ING_L2_PRI_CFI_MAX; pri_cfi_cnt++) {
                map = &array[idx++];
                bcm_qos_map_t_init(map);
                SOC_PPC_LIF_COS_PROFILE_MAP_TBL_L2_KEY_clear(&l2_key);
        
                l2_key.outer_tpid = (flags & BCM_QOS_MAP_L2_INNER_TAG) ? 1:0;
                map->pkt_pri = pri_cfi_cnt >> 1;
                map->pkt_cfi = pri_cfi_cnt & 1;
                l2_key.incoming_up = map->pkt_pri;
                l2_key.incoming_dei = map->pkt_cfi;
        
                SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_cos_profile_map_l2_info_get(dev_id,
                                     profile_ndx,
                                     &l2_key,
                                     &map_entry));
                map->int_pri = map_entry.tc;
                BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_decode(unit, map_entry.dp, &(map->color)));
        
                if ((idx >= num_entries) || (idx >= array_size)) {
                    break;
                }
            }
        } 
        else if ((flags & BCM_QOS_MAP_L2_UNTAGGED)) { /* Untagged */
            for (tc_dp_cnt = 0; tc_dp_cnt <  _BCM_QOS_MAP_TC_DP_MAX; tc_dp_cnt++) {
                map = &array[idx++];
                bcm_qos_map_t_init(map);
                SOC_PPC_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY_clear(&tc_dp_key);
        
                map->pkt_pri = tc_dp_cnt >> 2;
                map->pkt_cfi = tc_dp_cnt & 3;
                tc_dp_key.tc = map->pkt_pri;
                tc_dp_key.dp = map->pkt_cfi;
                BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_decode(unit, tc_dp_key.dp, &tmp_color));                 
                map->pkt_cfi = (uint8)tmp_color;

                SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_cos_profile_map_tc_dp_info_get(dev_id,
                                     profile_ndx,
                                     &tc_dp_key,
                                     &map_entry));
                map->int_pri = map_entry.tc;
                BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_decode(unit, map_entry.dp, &(map->color)));
        
                if ((idx >= num_entries) || (idx >= array_size)) {
                    break;
                }
            }
        } else {
            rv = BCM_E_PARAM;
        }
    } 
    else if (BCM_INT_QOS_MAP_IS_MPLS_ELSP(map_id)) {

        SOC_PPC_FRWRD_ILM_GLBL_INFO_clear(&glbl_info);
        BCM_SAND_IF_ERR_EXIT(soc_ppd_frwrd_ilm_glbl_info_get(dev_id, &glbl_info));
    
        for (exp_cnt = 0; exp_cnt < _BCM_QOS_MAP_ING_MPLS_EXP_MAX; exp_cnt++) {
            map = &array[idx++];
            bcm_qos_map_t_init(map);

            map->exp = exp_cnt;
            map->int_pri = glbl_info.elsp_info.exp_map_tbl[exp_cnt];
        
            if ((idx >= num_entries) || (idx >= array_size)) {
                break;
            }
        }
    } 
    else if (BCM_INT_QOS_MAP_IS_INGRESS_PCP_VLAN(map_id)) {
        if (flags & (BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_VLAN_PCP)) {
            if (flags & BCM_QOS_MAP_L2_INNER_TAG) {
                for (int_pri_cnt = 0; int_pri_cnt < _BCM_QOS_MAP_ING_VLAN_PCP_CTAG_MAX; int_pri_cnt++) {
                    map = &array[idx++];
                    bcm_qos_map_t_init(map);
            
                    SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_ing_vlan_edit_pcp_map_ctag_get(dev_id,
                                        profile_ndx,
                                        int_pri_cnt,
                                        &(map->pkt_pri),
                                        &(map->pkt_cfi)));
            
                    map->int_pri = int_pri_cnt;
            
                    if ((idx >= num_entries) || (idx >= array_size)) {
                        break;
                    }
                }
            } 
            else if (flags & BCM_QOS_MAP_L2_UNTAGGED) {
                for (pri_cfi_cnt = 0; pri_cfi_cnt < _BCM_QOS_MAP_ING_VLAN_PCP_UTAG_MAX; pri_cfi_cnt++) {
                    map = &array[idx++];
                    bcm_qos_map_t_init(map);
            
                    SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_ing_vlan_edit_pcp_map_untagged_get(dev_id,
                                            profile_ndx,
                                            (pri_cfi_cnt >> 2),
                                            (pri_cfi_cnt % 4),
                                            &(map->pkt_pri),
                                            &(map->pkt_cfi)));
            
                    map->int_pri = pri_cfi_cnt>>2;          
                    BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_decode(unit, (pri_cfi_cnt % 4), &(map->color)));
            
                    if ((idx >= num_entries) || (idx >= array_size)) {
                        break;
                    }
                }
            }
            else if (flags & BCM_QOS_MAP_L2_OUTER_TAG) {
                for (pri_cfi_cnt = 0; pri_cfi_cnt < _BCM_QOS_MAP_ING_VLAN_PCP_STAG_MAX; pri_cfi_cnt++) {
                    map = &array[idx++];
                    bcm_qos_map_t_init(map);
            
                    SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_ing_vlan_edit_pcp_map_stag_get(dev_id,
                                        profile_ndx,
                                        (pri_cfi_cnt >> 1),
                                        (pri_cfi_cnt & 1),
                                        &(map->pkt_pri),
                                        &(map->pkt_cfi)));

                    map->int_pri = pri_cfi_cnt>>1;          
                    BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_decode(unit, (pri_cfi_cnt & 1), &(map->color)));
            
                    if ((idx >= num_entries) || (idx >= array_size)) {
                        break;
                    }
                }
            } 
            else {
                rv = BCM_E_PARAM;
            }
        }
    } 
    else if (BCM_INT_QOS_MAP_IS_REMARK(map_id)) {
        if (flags & BCM_QOS_MAP_ENCAP) { 
            if (flags & BCM_QOS_MAP_MPLS) {
                for (exp_cnt = 0; exp_cnt < _BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX; exp_cnt++) {
                    map = &array[idx++];
                    bcm_qos_map_t_init(map);
                    SOC_PPC_EG_ENCAP_QOS_MAP_KEY_clear(&prm_in_encap_qos_key);
            
                    prm_in_encap_qos_key.remark_profile = profile_ndx;
                    prm_in_encap_qos_key.in_dscp_exp = exp_cnt;
                    prm_in_encap_qos_key.pkt_hdr_type = SOC_PPC_PKT_HDR_TYPE_MPLS;
            
                    SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_encap_qos_params_remark_get(dev_id,
                                        &prm_in_encap_qos_key,
                                        &prm_out_encap_qos_params));
            
                    map->remark_int_pri = prm_in_encap_qos_key.in_dscp_exp;
                    map->remark_color = prm_in_encap_qos_key.remark_profile; 
                    map->exp  = prm_out_encap_qos_params.mpls_exp;
                    map->dscp = prm_out_encap_qos_params.ip_dscp;
            
                    if ((idx >= num_entries) || (idx >= array_size)) {
                        break;
                    }
                }
            } 
            else if (flags & BCM_QOS_MAP_L3) { /* IPV4/V6 */ 
                for (dscp_cnt = 0; dscp_cnt < _BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX; dscp_cnt++) {
                    map = &array[idx++];
                    bcm_qos_map_t_init(map);
                    SOC_PPC_EG_ENCAP_QOS_MAP_KEY_clear(&prm_in_encap_qos_key);
            
                    prm_in_encap_qos_key.remark_profile = profile_ndx;
                    prm_in_encap_qos_key.in_dscp_exp = dscp_cnt;
            
                    if (flags & BCM_QOS_MAP_IPV6) {
                        prm_in_encap_qos_key.pkt_hdr_type = SOC_PPC_PKT_HDR_TYPE_IPV6;
                    } 
                    else {
                        prm_in_encap_qos_key.pkt_hdr_type = SOC_PPC_PKT_HDR_TYPE_IPV4;
                    }
                    SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_encap_qos_params_remark_get(dev_id,
                                        &prm_in_encap_qos_key,  
                                        &prm_out_encap_qos_params));
            
                    map->remark_int_pri = prm_in_encap_qos_key.in_dscp_exp;
                    map->remark_color = prm_in_encap_qos_key.remark_profile; 
                    map->exp  = prm_out_encap_qos_params.mpls_exp;
                    map->dscp = prm_out_encap_qos_params.ip_dscp;
            
                    if ((idx >= num_entries) || (idx >= array_size)) {
                        break;
                    }
                }
            } 
            else if (flags & BCM_QOS_MAP_L2 && (SOC_IS_ARAD_B0_AND_ABOVE(unit))) { /* L2 */

                /* get at most Color x In-DSCP-EXP entries*/
                for (dscp_cnt = 0; dscp_cnt < _BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX; dscp_cnt++) {
                    for (dp_cnt = 0; dp_cnt < SOC_DPP_CONFIG(unit)->qos.dp_max; dp_cnt++) {
                        map = &array[idx++];
                        bcm_qos_map_t_init(map);
                        SOC_PPC_EG_ENCAP_QOS_MAP_KEY_clear(&prm_in_encap_qos_key);
                
                        prm_in_encap_qos_key.remark_profile = profile_ndx;
                        prm_in_encap_qos_key.in_dscp_exp = dscp_cnt;
                        prm_in_encap_qos_key.pkt_hdr_type = SOC_PPC_PKT_HDR_TYPE_ETH;
                        prm_in_encap_qos_key.dp = dp_cnt;

                        SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_encap_qos_params_remark_get(dev_id,
                                            &prm_in_encap_qos_key,  
                                            &prm_out_encap_qos_params));
                
                        map->remark_int_pri = prm_in_encap_qos_key.in_dscp_exp;
                        map->remark_color = prm_in_encap_qos_key.remark_profile;
                        BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_decode(unit,dp_cnt,&map->color));
                        map->exp  = prm_out_encap_qos_params.mpls_exp;
                        map->dscp = prm_out_encap_qos_params.ip_dscp;
                
                        if ((idx >= num_entries) || (idx >= array_size)) {
                            dscp_cnt = _BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX;
                            break;
                        }
                    }
                }
            } 
            else {
                rv = BCM_E_PARAM; 
            }
        } 
        else{ /*not encapsulated*/
            if (flags & BCM_QOS_MAP_MPLS) {
                for (exp_cnt = 0; exp_cnt < _BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX; exp_cnt++) {
                    for (dp_cnt = 0; dp_cnt < SOC_DPP_CONFIG(unit)->qos.dp_max; dp_cnt++) {
                        map = &array[idx++];
                        bcm_qos_map_t_init(map);
                        SOC_PPC_EG_QOS_MAP_KEY_clear(&prm_in_qos_key);
            
                        prm_in_qos_key.in_dscp_exp = exp_cnt;
                        prm_in_qos_key.dp = dp_cnt;
                        prm_in_qos_key.remark_profile = profile_ndx;
            
                        SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_qos_params_remark_get(dev_id,
                                      &prm_in_qos_key,
                                      &prm_out_qos_params));
            
                        map->remark_int_pri = prm_out_qos_params.mpls_exp;
                        map->exp = prm_in_qos_key.in_dscp_exp;
                        BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_decode(unit, prm_in_qos_key.dp, &(map->color)));
                        if ((idx >= num_entries) || (idx >= array_size)) {
                            exp_cnt = _BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX;
                            break;
                        }
                    }
                }
            } 
            else if (flags & BCM_QOS_MAP_L3) {
                for (dscp_cnt = 0; dscp_cnt < _BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX; dscp_cnt++) {
                    for (dp_cnt = 0; dp_cnt < SOC_DPP_CONFIG(unit)->qos.dp_max; dp_cnt++) {
                        map = &array[idx++];
                        bcm_qos_map_t_init(map);
                        SOC_PPC_EG_QOS_MAP_KEY_clear(&prm_in_qos_key);
            
                        prm_in_qos_key.in_dscp_exp = dscp_cnt;
                        prm_in_qos_key.dp = dp_cnt;
                        prm_in_qos_key.remark_profile = profile_ndx;
                        
                        SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_qos_params_remark_get(dev_id,
                                      &prm_in_qos_key,
                                      &prm_out_qos_params));
                        if (flags & BCM_QOS_MAP_IPV6) {
                            map->remark_int_pri = prm_out_qos_params.ipv6_tc;
                        } else {
                            map->remark_int_pri = prm_out_qos_params.ipv4_tos;
                        }
                        map->int_pri = prm_in_qos_key.in_dscp_exp;
                        BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_decode(unit, prm_in_qos_key.dp, &(map->color)));
                        if ((idx >= num_entries) || (idx >= array_size)) {
                            dscp_cnt = _BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX;
                            break;
                        }
                    }
                }
            } 
            else {
                rv = BCM_E_PARAM;
            }
        }
    } 
    else if (BCM_INT_QOS_MAP_IS_EGRESS_PCP_VLAN(map_id)) {
        if (flags & BCM_QOS_MAP_ENCAP) {
            if (flags & BCM_QOS_MAP_MPLS) {
                for (exp = 0; exp < _BCM_QOS_MAP_EGR_EXP_MAX; exp++){
                    map = &array[idx++];
                    bcm_qos_map_t_init(map);
            
                    map->exp = exp;
            
                    SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_map_dscp_exp_get(dev_id,
                                           SOC_PPC_PKT_FRWRD_TYPE_MPLS,
                                           exp,
                                           &(map->pkt_pri),
                                           &(map->pkt_cfi)));
            
                    if ((idx >= num_entries) || (idx >= array_size)) {
                        break;
                    }
                }
            }
            else if (flags & BCM_QOS_MAP_L3) {
                pkt_hdr_type = (flags & BCM_QOS_MAP_IPV6)? SOC_PPC_PKT_FRWRD_TYPE_IPV6_UC : SOC_PPC_PKT_FRWRD_TYPE_IPV4_UC;
                for (dscp = 0; dscp < _BCM_QOS_MAP_EGR_DSCP_MAX; dscp++){
                    map = &array[idx++];
                    bcm_qos_map_t_init(map);
                    map->dscp = dscp;
            
                    SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_map_dscp_exp_get(dev_id,
                                           pkt_hdr_type,
                                           dscp,
                                           &(map->pkt_pri),
                                           &(map->pkt_cfi)));                  
            
                    if ((idx >= num_entries) || (idx >= array_size)) {
                        break;
                    }
                }
            } 
            else {
                rv = BCM_E_PARAM;
            }
        } 
        else { /*not encapsulated*/
            if (flags & BCM_QOS_MAP_L2_VLAN_PCP) {
                if (flags & BCM_QOS_MAP_L2_INNER_TAG) {
                    for (int_pri_cnt = 0; int_pri_cnt < _BCM_QOS_MAP_EGR_VLAN_PRI_MAX; int_pri_cnt++) {
                        map = &array[idx++];
                        bcm_qos_map_t_init(map);
            
                        SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_map_ctag_get(dev_id,
                                           profile_ndx,
                                           int_pri_cnt,
                                           &(map->pkt_pri),
                                           &(map->pkt_cfi)));
            
                        map->int_pri = int_pri_cnt;
            
                        if ((idx >= num_entries) || (idx >= array_size)) {
                            break;
                        }
                    }
                } else if (flags & BCM_QOS_MAP_L2_UNTAGGED) {
                    for (int_pri_cnt = 0; int_pri_cnt < _BCM_QOS_MAP_EGR_VLAN_PRI_MAX; int_pri_cnt++) {
                        for (dp_cnt=0; dp_cnt < SOC_DPP_CONFIG(unit)->qos.dp_max; dp_cnt++) {
                            map = &array[idx++];
                            bcm_qos_map_t_init(map);
                
                            SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_map_untagged_get(dev_id,
                                               profile_ndx,
                                               int_pri_cnt,
                                               dp_cnt,
                                               &(map->pkt_pri),
                                               &(map->pkt_cfi)));
                
                            map->int_pri = int_pri_cnt;         
                            BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_decode(unit, dp_cnt, &(map->color)));
                
                            if ((idx >= num_entries) || (idx >= array_size)) {
                                int_pri_cnt = _BCM_QOS_MAP_EGR_VLAN_PRI_MAX;
                                break;
                            }
                        }
                    }
                } else  {/* outer */ 
                    for (int_pri_cnt = 0; int_pri_cnt < _BCM_QOS_MAP_EGR_VLAN_PRI_MAX; int_pri_cnt++) {
                        for (int_cfi=0; int_cfi < _BCM_QOS_MAP_EGR_VLAN_CFI_MAX; int_cfi++) {

                            map = &array[idx++];
                            bcm_qos_map_t_init(map);
                
                            SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_map_stag_get(dev_id,
                                           profile_ndx,
                                           int_pri_cnt,
                                           int_cfi,
                                           &(map->pkt_pri),
                                           &(map->pkt_cfi)));
                
                
                            map->color = int_cfi;
                            map->int_pri = int_pri_cnt;
                
                            if ((idx >= num_entries) || (idx >= array_size)) {
                                int_pri_cnt = _BCM_QOS_MAP_EGR_VLAN_PRI_MAX;
                                break;
                            }
                        }
                    }
                }
            }
        }
    } 
    else if (BCM_INT_QOS_MAP_IS_EGRESS_L2_I_TAG(map_id)) {
        for (int_pri_cnt = 0; int_pri_cnt < _BCM_QOS_MAP_EGR_VLAN_PRI_MAX; int_pri_cnt++) {
            for (dp_cnt=0; dp_cnt < SOC_DPP_CONFIG(unit)->qos.dp_max; dp_cnt++) {
        
                map = &array[idx++];
                bcm_qos_map_t_init(map);
        
                SOC_SAND_IF_ERROR_RETURN(soc_ppd_frwrd_bmact_eg_vlan_pcp_map_get(dev_id, profile_ndx, int_pri_cnt,
                                     dp_cnt, &(map->pkt_pri), &(map->pkt_cfi)));

                BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_decode(unit, dp_cnt, &(map->color)));

                map->int_pri = int_pri_cnt;
        
                if ((idx >= num_entries) || (idx >= array_size)) {
                    int_pri_cnt = _BCM_QOS_MAP_EGR_VLAN_PRI_MAX;
                    break;
                }
            }
        }
    } 
#ifdef BCM_88660
    else if (SOC_IS_ARADPLUS(unit) && BCM_INT_QOS_MAP_IS_EGRESS_DSCP_EXP_MARKING(map_id)) {
        /* Reset all lines with profile profile_ndx */
        if (SOC_IS_JERICHO_B0(unit)) {
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_read(unit, EPNI_DP_MAP_FIX_ENABLEDr, SOC_CORE_ALL, 0, DP_MAP_FIX_ENABLEDf, &dp_map_disabled));
        } else if (SOC_IS_JERICHO_PLUS(unit)) {
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_read(unit, EPNI_CFG_MARKING_DP_MAPr, SOC_CORE_ALL, 0, CFG_MARKING_DP_MAP_DISABLEf, &dp_map_disabled));
        }

        idx = 0;
        for (resolved_dp_idx = 0; (resolved_dp_idx < _BCM_QOS_MAP_EGR_RESOLVED_DP_MAX) && (idx < array_size); resolved_dp_idx++) {
            for (tc_idx = 0; (tc_idx < _BCM_QOS_MAP_TC_MAX) && (idx < array_size); tc_idx++) {
                for (inlif_idx = 0; (inlif_idx < SOC_OCC_MGMT_NOF_INLIF_PROFILES_TRANSFERED_TO_EGRESS) && (idx < array_size); inlif_idx++) {
                    SOC_PPC_EG_QOS_MARKING_KEY_clear(&marking_key);
                    SOC_PPC_EG_QOS_MARKING_PARAMS_clear(&marking_params);

                    map = &array[idx];

                    bcm_qos_map_t_init(map);
                    
                    marking_key.resolved_dp_ndx = resolved_dp_idx;
                    marking_key.tc_ndx = tc_idx;
                    marking_key.in_lif_profile = inlif_idx;
                    marking_key.marking_profile = profile_ndx;
                    marking_key.dp_map_disabled = dp_map_disabled;
                    
                    SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_qos_params_marking_get(dev_id, &marking_key, &marking_params));
                    map->remark_color = marking_key.resolved_dp_ndx;
                    map->int_pri = marking_key.tc_ndx;
                    map->port_offset = marking_key.in_lif_profile;
                    map->dscp = marking_params.ip_dscp;
                    map->exp = marking_params.mpls_exp;

                    idx++;
                }
            }
        }

    } 
#endif
    else if (BCM_INT_QOS_MAP_IS_EGRESS_MPLS_PHP(map_id)) {
        for (exp_cnt = 0; exp_cnt < _BCM_QOS_MAP_EGR_EXP_MAX; exp_cnt++) {
            map = &array[idx++];
            bcm_qos_map_t_init(map);

            SOC_PPC_EG_QOS_PHP_REMARK_KEY_clear(&prm_in_qos_php_key);
            
            prm_in_qos_php_key.exp = exp_cnt;
            prm_in_qos_php_key.exp_map_profile = profile_ndx;

            if (flags & BCM_QOS_MAP_IPV6) {
                prm_in_qos_php_key.php_type = SOC_PPC_EG_QOS_UNIFORM_PHP_POP_INTO_IPV6;
            } else {
                prm_in_qos_php_key.php_type = SOC_PPC_EG_QOS_UNIFORM_PHP_POP_INTO_IPV4;
            }

            SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_qos_params_php_remark_get(dev_id,
                          &prm_in_qos_php_key,
                          &tmp_dscp_exp));
            map->remark_int_pri = tmp_dscp_exp;
            map->int_pri = exp_cnt;
            
            if ((idx >= num_entries) || (idx >= array_size)) {
                break;
            }
        }
    } 
    else if (BCM_INT_QOS_MAP_IS_OPCODE(map_id)) {
        if (opcode_type & SOC_PPC_LIF_COS_OPCODE_TYPE_L3) {
            if (flags & BCM_QOS_MAP_IPV6) {
                for (dscp_cnt = 0; dscp_cnt < _BCM_QOS_MAP_ING_COS_OPCODE_L3_MAX; dscp_cnt++) {
                    map = &array[idx++];
                    bcm_qos_map_t_init(map);
                    SOC_PPC_LIF_COS_OPCODE_ACTION_INFO_clear(&action_info);
                    
                    map->dscp = dscp_cnt;
                    
                    sand_rv = soc_ppd_lif_cos_opcode_ipv6_tos_map_get(dev_id, profile_ndx, map->dscp, &action_info);
                    BCM_SAND_IF_ERR_EXIT(sand_rv);
                    if ((action_info.is_packet_valid != is_pkt_valid) ||
                        (action_info.is_qos_only != is_qos_only)) {
                        idx--;
                        continue;
                    }
                    
                    map->port_offset = action_info.ac_offset;
                    
                    if ((idx >= num_entries) || (idx >= array_size)) {
                        break;
                    }
                }
            } 
            else if (flags & BCM_QOS_MAP_IPV4) {
                for (dscp_cnt = 0; dscp_cnt < _BCM_QOS_MAP_ING_COS_OPCODE_L3_MAX; dscp_cnt++) {
                    map = &array[idx++];
                    bcm_qos_map_t_init(map);
                    SOC_PPC_LIF_COS_OPCODE_ACTION_INFO_clear(&action_info);
                    
                    map->dscp = dscp_cnt;
                    
                    sand_rv = soc_ppd_lif_cos_opcode_ipv4_tos_map_get(dev_id, profile_ndx, map->dscp, &action_info);
                    BCM_SAND_IF_ERR_EXIT(sand_rv);
                    if ((action_info.is_packet_valid != is_pkt_valid) ||
                        (action_info.is_qos_only != is_qos_only)) {
                        idx--;
                        continue;
                    }
                    
                    map->port_offset = action_info.ac_offset;
                    
                    if ((idx >= num_entries) || (idx >= array_size)) {
                        break;
                    }
                }
            } else {
                for (tc_cnt = 0; tc_cnt < _BCM_QOS_MAP_TC_MAX; dscp_cnt++) {
                    for (dp_cnt = 0; dp_cnt < 4; dp_cnt++) {
                        map = &array[idx++];
                        bcm_qos_map_t_init(map);
                        SOC_PPC_LIF_COS_OPCODE_ACTION_INFO_clear(&action_info);
                        
                        map->int_pri = tc_cnt;
                        BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_decode(unit, dp_cnt, &(map->color)));
                        
                        sand_rv = soc_ppd_lif_cos_opcode_tc_dp_map_get(dev_id, profile_ndx, map->int_pri, dp_cnt, &action_info);
                        BCM_SAND_IF_ERR_EXIT(sand_rv);
                        if ((action_info.is_packet_valid != is_pkt_valid) ||
                            (action_info.is_qos_only != is_qos_only)) {
                            idx--;
                            continue;
                        }
                        
                        map->port_offset = action_info.ac_offset;
                        
                        if ((idx >= num_entries) || (idx >= array_size)) {
                            dscp_cnt = _BCM_QOS_MAP_TC_MAX;
                            break;
                        }
                    }
                }
            }
        }
        else if (opcode_type & SOC_PPC_LIF_COS_OPCODE_TYPE_L2) {
            for (pri_cfi_cnt = 0; pri_cfi_cnt <  _BCM_QOS_MAP_ING_L2_PRI_CFI_MAX; pri_cfi_cnt++) {
                map = &array[idx++];
                bcm_qos_map_t_init(map);
        
                outer_tpid_index = SOC_PPC_LLP_PARSE_TPID_INDEX_TPID1;
                if (flags & BCM_QOS_MAP_L2_UNTAGGED) {
                    outer_tpid_index = SOC_PPC_LLP_PARSE_TPID_INDEX_NONE;
                } else if (flags & BCM_QOS_MAP_L2_INNER_TAG) {
                    outer_tpid_index = SOC_PPC_LLP_PARSE_TPID_INDEX_TPID2;
                }
        
                map->int_pri = pri_cfi_cnt >> 1;
                map->pkt_cfi = pri_cfi_cnt & 1;
                
                sand_rv = soc_ppd_lif_cos_opcode_vlan_tag_map_get(dev_id, profile_ndx, outer_tpid_index, 
                                                              map->int_pri, map->pkt_cfi, &action_info);
                BCM_SAND_IF_ERR_EXIT(sand_rv);
                if ((action_info.is_packet_valid != is_pkt_valid) ||
                    (action_info.is_qos_only != is_qos_only)) {
                    idx--;
                    continue;
                }

                map->port_offset = action_info.ac_offset;

                if ((idx >= num_entries) || (idx >= array_size)) {
                    break;
                }
            }
        } 
        else if (opcode_type & SOC_PPC_LIF_COS_OPCODE_TYPE_TC_DP) {
            for (tc_cnt = 0; tc_cnt < _BCM_QOS_MAP_TC_MAX; dscp_cnt++) {
                for (dp_cnt = 0; dp_cnt < 4; dp_cnt++) {
                    map = &array[idx++];
                    bcm_qos_map_t_init(map);
                    SOC_PPC_LIF_COS_OPCODE_ACTION_INFO_clear(&action_info);

                    map->int_pri = tc_cnt;
                    BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_decode(unit, dp_cnt, &(map->color)));

                    sand_rv = soc_ppd_lif_cos_opcode_tc_dp_map_get(dev_id, profile_ndx, map->int_pri, dp_cnt, &action_info);
                    BCM_SAND_IF_ERR_EXIT(sand_rv);
                    if ((action_info.is_packet_valid != is_pkt_valid) ||
                        (action_info.is_qos_only != is_qos_only)) {
                        idx--;
                        continue;
                    }

                    map->port_offset = action_info.ac_offset;

                    if ((idx >= num_entries) || (idx >= array_size)) {
                        dscp_cnt = _BCM_QOS_MAP_TC_MAX;
                        break;
                    }
                }
            }
        } 
    }
    else if (BCM_INT_QOS_MAP_ENCAP_INTPRI_COLOR(map_id)) 
    {
        SOC_SAND_IF_ERROR_RETURN(_bcm_petra_qos_map_encap_intpri_color_get(unit,flags,map_id ,array)); 
    }
    else if (BCM_INT_QOS_MAP_IS_INGRESS_COLOR(map_id)) {
        for (dp_cnt = 0; dp_cnt < _BCM_QOS_MAP_DP_MAX; dp_cnt++) {
            for (pcp_dei= 0; pcp_dei < _BCM_QOS_MAP_PCP_DEI_MAX; pcp_dei++) {
                map = &array[idx++];
                bcm_qos_map_t_init(map);

                BCMDNX_IF_ERR_EXIT(MBCM_PP_DRIVER_CALL(dev_id, mbcm_pp_lif_ing_vlan_edit_pcp_map_dp_get, (unit,
                                    profile_ndx,
                                    dp_cnt,
                                    pcp_dei,
                                    &(map->pkt_pri),
                                    &(map->pkt_cfi))));

                map->remark_int_pri = pcp_dei;          
                map->int_pri = dp_cnt;
        
                if ((idx >= num_entries) || (idx >= array_size)) {
                    break;
                }
            }
        }
    }
    /* the idx is not initialized in case of map encap intri color */
    if(BCM_INT_QOS_MAP_ENCAP_INTPRI_COLOR(map_id))
    {
        *array_count = 1;
    }
    else
    {
        *array_count = idx;
    }
    
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


/* Clear an entry from a QoS map */
STATIC int 
_bcm_petra_qos_map_ingress_elsp_delete(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_NONE;
    int dev_id = 0;
    uint32 profile_ndx = 0;
    SOC_PPC_FRWRD_ILM_GLBL_INFO glbl_info;

    BCMDNX_INIT_FUNC_DEFS;
    dev_id = (unit);
    profile_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);

    if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_ing_elsp) {
    
    LOG_ERROR(BSL_LS_BCM_QOS,
              (BSL_META_U(unit,
                          "%s: invalid MPLS ELSP profile (%d) out of range\n"),
                          FUNCTION_NAME(), profile_ndx));
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid ELSP profile")));
    }
    
    SOC_PPC_FRWRD_ILM_GLBL_INFO_clear(&glbl_info);
    BCM_SAND_IF_ERR_EXIT(soc_ppd_frwrd_ilm_glbl_info_get(dev_id, &glbl_info));

    glbl_info.elsp_info.exp_map_tbl[map->exp] = 0;
    BCM_SAND_IF_ERR_EXIT(soc_ppd_frwrd_ilm_glbl_info_set(dev_id, &glbl_info));
    
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int 
_bcm_petra_qos_map_ingress_lif_cos_delete(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_NONE;
    int dev_id = 0, dp = 0, tmp_dp = 0;
    uint32 profile_ndx = 0;
    SOC_PPC_LIF_COS_PROFILE_INFO profile_info;
    SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY map_entry;
    SOC_PPC_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY mpls_key;
    SOC_PPC_LIF_COS_PROFILE_MAP_TBL_IP_KEY ip_key;
    SOC_PPC_LIF_COS_PROFILE_MAP_TBL_L2_KEY l2_key;
    SOC_PPC_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY tc_dp_key;
    int count1 = 0, count2 = 0, count3 = 0;

    BCMDNX_INIT_FUNC_DEFS;
    dev_id = (unit);
    profile_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);
    
    if (BCM_INT_QOS_MAP_IS_LIF_COS(map_id)) {
    
    if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_ing_lif_cos) {
        
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: invalid lif_cos profile (%d) out of range\n"),
                              FUNCTION_NAME(), profile_ndx));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid LIF COS profile")));
    }
    }
    
    SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY_clear(&map_entry);
    SOC_PPC_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY_clear(&mpls_key);
    SOC_PPC_LIF_COS_PROFILE_MAP_TBL_IP_KEY_clear(&ip_key);
    SOC_PPC_LIF_COS_PROFILE_MAP_TBL_L2_KEY_clear(&l2_key);
    SOC_PPC_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY_clear(&tc_dp_key);
    
    BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_encode(unit,map->color,&dp));

    if (flags & BCM_QOS_MAP_MPLS) {
    
    mpls_key.in_exp = map->exp;
    map_entry.tc = 0;
    map_entry.dp = 0;
    map_entry.dscp = 0;

    SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_cos_profile_map_mpls_label_info_set(dev_id,
                                     profile_ndx,
                                     &mpls_key,
                                     &map_entry));
    /* the entry is not in use:
     * corresponding bit in bitmap ing_lif_cos_mpls_bitmap should be clear.
     */
    rv = QOS_ACCESS.ing_lif_cos_mpls_bitmap.bit_clear(unit, _DPP_QOS_ENTRY_INDEX_ING_LIF_COS_MPLS(profile_ndx, map->exp));
    BCMDNX_IF_ERR_EXIT(rv);
    
    _DPP_QOS_ENTRY_USED_CNT(ing_lif_cos_mpls_bitmap, profile_ndx, _BCM_QOS_MAP_ING_MPLS_EXP_MAX, count1);
    if (count1 == 0) {
        _DPP_QOS_ENTRY_USED_CNT(ing_lif_cos_ipv6_bitmap, profile_ndx, _BCM_QOS_MAP_ING_L3_DSCP_MAX, count2);
        _DPP_QOS_ENTRY_USED_CNT(ing_lif_cos_ipv4_bitmap, profile_ndx, _BCM_QOS_MAP_ING_L3_DSCP_MAX, count3);

        if ((count2 == 0) && (count3 == 0)) {
            SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_cos_profile_info_get(dev_id,
                                  profile_ndx,
                                  &profile_info));
            
            /*
             * 'map_when_ip' and 'map_when_mpls' should have the 
             * same value for Soc_petra-B.
             */
            
            profile_info.map_when_ip = FALSE;
            profile_info.map_when_mpls = FALSE;
            
            SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_cos_profile_info_set(dev_id,
                                  profile_ndx,
                                  &profile_info));
        }       
    }    
    } 
    
    if (flags & BCM_QOS_MAP_L3) {
        
    ip_key.ip_type = (flags & BCM_QOS_MAP_IPV6) ? SOC_SAND_PP_IP_TYPE_IPV6 : SOC_SAND_PP_IP_TYPE_IPV4;
    ip_key.tos = map->dscp;
    map_entry.tc = 0;
    map_entry.dp = 0;
    map_entry.dscp = 0;

    SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_cos_profile_map_ip_info_set(dev_id,
                                 profile_ndx,
                                 &ip_key,
                                 &map_entry));
    
    if (flags & BCM_QOS_MAP_IPV6) {
        /* the entry is not in use:
         * corresponding bit in bitmap ing_lif_cos_ipv6_bitmap should be clear.
         */        
        rv = QOS_ACCESS.ing_lif_cos_ipv6_bitmap.bit_clear(unit, _DPP_QOS_ENTRY_INDEX_ING_LIF_COS_L3(profile_ndx, map->dscp));   
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else {
        /* the entry is not in use:
         * corresponding bit in bitmap ing_lif_cos_ipv4_bitmap should be clear.
         */        
        rv = QOS_ACCESS.ing_lif_cos_ipv4_bitmap.bit_clear(unit, _DPP_QOS_ENTRY_INDEX_ING_LIF_COS_L3(profile_ndx, map->dscp));
        BCMDNX_IF_ERR_EXIT(rv);
    }
    
    _DPP_QOS_ENTRY_USED_CNT(ing_lif_cos_ipv6_bitmap, profile_ndx, _BCM_QOS_MAP_ING_L3_DSCP_MAX, count1);
    _DPP_QOS_ENTRY_USED_CNT(ing_lif_cos_ipv4_bitmap, profile_ndx, _BCM_QOS_MAP_ING_L3_DSCP_MAX, count2);
    
    if ((count1 == 0) && (count2 == 0)) {
        _DPP_QOS_ENTRY_USED_CNT(ing_lif_cos_mpls_bitmap, profile_ndx, _BCM_QOS_MAP_ING_MPLS_EXP_MAX, count3);
        
        if (count3 == 0) {
            SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_cos_profile_info_get(dev_id,
                                  profile_ndx,
                                  &profile_info));
#ifdef BCM_88660_A0
            if( !(SOC_IS_ARADPLUS(unit) && (flags & BCM_QOS_MAP_L3_L2)))
#endif  /* BCM_88660_A0 */
            {
                /*
                 * 'map_when_ip' and 'map_when_mpls' should have the 
                 * same value for Soc_petra-B.
                 */
                profile_info.map_when_ip = FALSE;
                profile_info.map_when_mpls = FALSE;
            }
            
            SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_cos_profile_info_set(dev_id,
                                  profile_ndx,
                                  &profile_info));
        }
    }
    
    }

    /* Tag exist */
    if ((flags & BCM_QOS_MAP_L2) && !(flags & BCM_QOS_MAP_L2_UNTAGGED)) {

    l2_key.outer_tpid = (flags & BCM_QOS_MAP_L2_INNER_TAG) ? 1:0;
    l2_key.incoming_up = map->pkt_pri;
    l2_key.incoming_dei = map->pkt_cfi;

    map_entry.tc = 0;
    map_entry.dp = 0;

    SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_cos_profile_map_l2_info_set(dev_id,
                                 profile_ndx,
                                 &l2_key,
                                 &map_entry));

    if (flags & BCM_QOS_MAP_L2_INNER_TAG) {
        /* the entry is not in use:
         * corresponding bit in bitmap ing_lif_cos_l2_inner_bitmap should be clear.
         */        
        rv = QOS_ACCESS.ing_lif_cos_l2_inner_bitmap.bit_clear(unit, _DPP_QOS_ENTRY_INDEX_ING_LIF_COS_L2_INNER(profile_ndx, l2_key.incoming_up, l2_key.incoming_dei));
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else {
        /* the entry is not in use:
         * corresponding bit in bitmap ing_lif_cos_l2_outer_bitmap should be clear.
         */        
        rv = QOS_ACCESS.ing_lif_cos_l2_outer_bitmap.bit_clear(unit, _DPP_QOS_ENTRY_INDEX_ING_LIF_COS_L2_OUTER(profile_ndx, l2_key.incoming_up, l2_key.incoming_dei));
        BCMDNX_IF_ERR_EXIT(rv);
    }
    
    _DPP_QOS_ENTRY_USED_CNT(ing_lif_cos_l2_inner_bitmap, profile_ndx, _BCM_QOS_MAP_ING_L2_PRI_CFI_MAX, count1);
    _DPP_QOS_ENTRY_USED_CNT(ing_lif_cos_l2_outer_bitmap, profile_ndx, _BCM_QOS_MAP_ING_L2_PRI_CFI_MAX, count2);
    if ((count1 == 0) && (count2 == 0)) {
        SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_cos_profile_info_get(dev_id,
                                  profile_ndx,
                                  &profile_info));
            
        profile_info.map_when_l2 = FALSE;
            
        SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_cos_profile_info_set(dev_id,
                                  profile_ndx,
                                  &profile_info));
    }
    }

    if (flags & BCM_QOS_MAP_L2_UNTAGGED) {

    tc_dp_key.tc = map->pkt_pri;
    BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_encode(unit,map->pkt_cfi,&tmp_dp));
    tc_dp_key.dp = tmp_dp;
    map_entry.tc = 0;
    map_entry.dp = 0;

    SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_cos_profile_map_tc_dp_info_set(dev_id,
                                 profile_ndx,
                                 &tc_dp_key,
                                 &map_entry));
    /* the entry is not in use:
     * corresponding bit in bitmap ing_lif_cos_l2_untag_bitmap should be clear.
     */    
    rv = QOS_ACCESS.ing_lif_cos_l2_untag_bitmap.bit_clear(unit, _DPP_QOS_ENTRY_INDEX_ING_LIF_COS_L2_UNTAG(profile_ndx, tc_dp_key.tc, tc_dp_key.dp));
    BCMDNX_IF_ERR_EXIT(rv);

    _DPP_QOS_ENTRY_USED_CNT(ing_lif_cos_l2_untag_bitmap, profile_ndx, _BCM_QOS_MAP_TC_DP_MAX, count1);

    if (count1 == 0) {
        SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_cos_profile_info_get(dev_id,
                                  profile_ndx,
                                  &profile_info));
            
        profile_info.map_from_tc_dp = FALSE;
            
        SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_cos_profile_info_set(dev_id,
                                  profile_ndx,
                                  &profile_info));
    }
    }


    if (flags & BCM_QOS_MAP_L3_L2) {

        SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_cos_profile_info_get(dev_id, profile_ndx, &profile_info));
            
        profile_info.use_layer2_pcp = FALSE;
            
        SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_cos_profile_info_set(dev_id, profile_ndx, &profile_info));
    }

    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int 
_bcm_petra_qos_map_ingress_pcp_vlan_delete(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_NONE;
    int dev_id = 0, dp = 0;
    uint32 profile_ndx = 0;

    BCMDNX_INIT_FUNC_DEFS;
    dev_id = (unit);
    profile_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);

    BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_encode(unit,map->color,&dp));

    if ((flags & BCM_QOS_MAP_L2_VLAN_PCP) == 0) {
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("flag BCM_QOS_MAP_L2_VLAN_PCP must be set")));
    }

    if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_ing_pcp_vlan) {
    
    LOG_ERROR(BSL_LS_BCM_QOS,
              (BSL_META_U(unit,
                          "%s: invalid ingress vlan pcp profile (%d) out of range\n"),
                          FUNCTION_NAME(), profile_ndx));
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid PCP VLAN profile")));
    }

    if (flags & BCM_QOS_MAP_L2) {
    /* ing_vlan_edit */

    map->pkt_pri = 0;
    map->pkt_cfi = 0;

    if (flags & BCM_QOS_MAP_L2_INNER_TAG) {

        SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_ing_vlan_edit_pcp_map_ctag_set(dev_id, 
                                    profile_ndx, 
                                    map->int_pri,
                                    map->pkt_pri,
                                    map->pkt_cfi));
        
        /* the entry is not in use:
         * corresponding bit in bitmap ing_pcp_vlan_ctag_bitmap should be clear.
         */
        rv = QOS_ACCESS.ing_pcp_vlan_ctag_bitmap.bit_clear(unit, _DPP_QOS_ENTRY_INDEX_ING_PCP_VLAN_CTAG(profile_ndx, map->int_pri));
        BCMDNX_IF_ERR_EXIT(rv);
    } else if (flags & BCM_QOS_MAP_L2_UNTAGGED) {

        SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_ing_vlan_edit_pcp_map_untagged_set(dev_id, 
                                        profile_ndx, 
                                        map->int_pri,
                                        dp,
                                        map->pkt_pri,
                                        map->pkt_cfi));

        /* the entry is not in use:
         * corresponding bit in bitmap ing_pcp_vlan_utag_bitmap should be clear.
         */
        rv = QOS_ACCESS.ing_pcp_vlan_utag_bitmap.bit_clear(unit, _DPP_QOS_ENTRY_INDEX_ING_PCP_VLAN_UTAG(profile_ndx, map->int_pri, dp));
        BCMDNX_IF_ERR_EXIT(rv);
    } else { /* OUTER TAG */

        SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_ing_vlan_edit_pcp_map_stag_set(dev_id, 
                                    profile_ndx, 
                                    map->int_pri,
                                    (dp) ? 1 : 0,
                                    map->pkt_pri,
                                    map->pkt_cfi));

        /* the entry is not in use:
         * corresponding bit in bitmap ing_pcp_vlan_stag_bitmap should be clear.
         */
        rv = QOS_ACCESS.ing_pcp_vlan_stag_bitmap.bit_clear(unit, _DPP_QOS_ENTRY_INDEX_ING_PCP_VLAN_STAG(profile_ndx, map->int_pri, (dp) ? 1 : 0));
        BCMDNX_IF_ERR_EXIT(rv);
    }
    }

    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int 
_bcm_petra_qos_map_ingress_color_delete(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_NONE;
    int dev_id = 0, dp = 0;
    uint32 profile_ndx = 0;

    BCMDNX_INIT_FUNC_DEFS;
    dev_id = (unit);
    profile_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);

    BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_encode(unit,map->color,&dp));

    if ((flags & BCM_QOS_MAP_POLICER) == 0) {
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("flag BCM_QOS_MAP_POLICER must be set")));
    }

    if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_ing_dp) {
    
    LOG_ERROR(BSL_LS_BCM_QOS,
              (BSL_META_U(unit,
                          "%s: invalid ingress dp profile (%d) out of range\n"),
                          FUNCTION_NAME(), profile_ndx));
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid dp profile")));
    }


    map->pkt_pri = 0;
    map->pkt_cfi = 0;

	BCMDNX_IF_ERR_EXIT(MBCM_PP_DRIVER_CALL(dev_id, mbcm_pp_lif_ing_vlan_edit_pcp_map_dp_set, (unit, 
                            profile_ndx, 
                            dp,
                            map->remark_int_pri,
                            map->pkt_pri,
                            map->pkt_cfi)));

    /* the entry is not in use:
     * corresponding bit in bitmap ing_pcp_vlan_color_bitmap should be clear.
     */
    rv = QOS_ACCESS.ing_pcp_vlan_color_bitmap.bit_clear(unit, _DPP_QOS_ENTRY_INDEX_DP(profile_ndx, dp, map->remark_int_pri));
    BCMDNX_IF_ERR_EXIT(rv);


exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int 
_bcm_petra_qos_map_ingress_cos_ipv4_delete(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    bcm_error_t rv = BCM_E_NONE;
    int dev_id;
    uint32 sand_rv, opcode_ndx;
    SOC_PPC_LIF_COS_OPCODE_ACTION_INFO action_info;
    SOC_PPC_LIF_COS_OPCODE_TYPE opcode_type;
    uint8 bit_val;

    BCMDNX_INIT_FUNC_DEFS;
    SOC_PPC_LIF_COS_OPCODE_ACTION_INFO_clear(&action_info);

    dev_id = (unit);
    opcode_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);

    rv = QOS_ACCESS.opcode_bmp.bit_get(unit, opcode_ndx, &bit_val);
    BCMDNX_IF_ERR_EXIT(rv);
    if (bit_val == 0) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: OPCODE TYPE not configured \n"), FUNCTION_NAME()));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Opcode Type not configured")));
    } else {
        sand_rv = soc_ppd_lif_cos_opcode_types_get(dev_id, opcode_ndx, &opcode_type);
        BCM_SAND_IF_ERR_EXIT(sand_rv);
        if ((opcode_type & SOC_PPC_LIF_COS_OPCODE_TYPE_L3) == 0) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: invalid OPCODE TYPE (0x%x) \n"), FUNCTION_NAME(), opcode_type));
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Incorrect Opcode Index used")));
        }
    }

    action_info.is_packet_valid = TRUE;
    action_info.is_qos_only = TRUE;
    action_info.ac_offset = 0;

    sand_rv = soc_ppd_lif_cos_opcode_ipv4_tos_map_set(dev_id, opcode_ndx, map->dscp, &action_info);
    BCM_SAND_IF_ERR_EXIT(sand_rv);
    
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int 
_bcm_petra_qos_map_ingress_cos_ipv6_delete(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    bcm_error_t rv = BCM_E_NONE;
    int dev_id;
    uint32 sand_rv, opcode_ndx;
    SOC_PPC_LIF_COS_OPCODE_ACTION_INFO action_info;
    SOC_PPC_LIF_COS_OPCODE_TYPE opcode_type;
    uint8 bit_val;
    BCMDNX_INIT_FUNC_DEFS;
    dev_id = (unit);
    opcode_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);

    SOC_PPC_LIF_COS_OPCODE_ACTION_INFO_clear(&action_info);

    rv = QOS_ACCESS.opcode_bmp.bit_get(unit, opcode_ndx, &bit_val);
    BCMDNX_IF_ERR_EXIT(rv);
    if (bit_val == 0) {
        sand_rv = soc_ppd_lif_cos_opcode_types_set(dev_id, opcode_ndx,(SOC_PPC_LIF_COS_OPCODE_TYPE_L3|SOC_PPC_LIF_COS_OPCODE_TYPE_TC_DP)); 
        BCM_SAND_IF_ERR_EXIT(sand_rv);
    } else {
        sand_rv = soc_ppd_lif_cos_opcode_types_get(dev_id, opcode_ndx, &opcode_type);
        BCM_SAND_IF_ERR_EXIT(sand_rv);
        if ((opcode_type & SOC_PPC_LIF_COS_OPCODE_TYPE_L3) == 0) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: invalid OPCODE TYPE (0x%x) \n"), FUNCTION_NAME(), opcode_type));
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Incorrect Opcode Index used")));
        }
    }

    action_info.is_packet_valid = TRUE;
    action_info.is_qos_only = TRUE;
    action_info.ac_offset = 0;

    sand_rv = soc_ppd_lif_cos_opcode_ipv6_tos_map_set(dev_id, opcode_ndx, map->dscp, &action_info);
    BCM_SAND_IF_ERR_EXIT(sand_rv);
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int 
_bcm_petra_qos_map_ingress_cos_l2_delete(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    bcm_error_t rv = BCM_E_NONE;
    int dev_id;
    uint32 opcode_ndx, sand_rv;
    SOC_PPC_LIF_COS_OPCODE_ACTION_INFO action_info;
    SOC_PPC_LLP_PARSE_TPID_INDEX outer_tpid_ndx;
    SOC_PPC_LIF_COS_OPCODE_TYPE opcode_type;
    uint8 bit_val;
    BCMDNX_INIT_FUNC_DEFS;
    dev_id = (unit);

    SOC_PPC_LIF_COS_OPCODE_ACTION_INFO_clear(&action_info);
    opcode_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);

    rv = QOS_ACCESS.opcode_bmp.bit_get(unit, opcode_ndx, &bit_val);
    BCMDNX_IF_ERR_EXIT(rv);
    if (bit_val == 0) {
        sand_rv = soc_ppd_lif_cos_opcode_types_set(dev_id, opcode_ndx,(SOC_PPC_LIF_COS_OPCODE_TYPE_L2));
        BCM_SAND_IF_ERR_EXIT(sand_rv);
    } else {
        sand_rv = soc_ppd_lif_cos_opcode_types_get(dev_id, opcode_ndx, &opcode_type);
        BCM_SAND_IF_ERR_EXIT(sand_rv);
        if ((opcode_type & SOC_PPC_LIF_COS_OPCODE_TYPE_L2) == 0) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: invalid OPCODE TYPE (0x%x) \n"), FUNCTION_NAME(), opcode_type));
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Incorrect Opcode Index used")));
        }
    }

    if (flags & BCM_QOS_MAP_L2_UNTAGGED) {
        outer_tpid_ndx = SOC_PPC_LLP_PARSE_TPID_INDEX_NONE;
    } else if (flags & BCM_QOS_MAP_L2_INNER_TAG) {
        outer_tpid_ndx = SOC_PPC_LLP_PARSE_TPID_INDEX_TPID2;
    } else {
        outer_tpid_ndx = SOC_PPC_LLP_PARSE_TPID_INDEX_TPID1;
    }

    action_info.is_packet_valid = TRUE;
    action_info.is_qos_only = TRUE;
    action_info.ac_offset = 0;

    sand_rv = soc_ppd_lif_cos_opcode_vlan_tag_map_set(dev_id, opcode_ndx, outer_tpid_ndx, map->int_pri, map->pkt_cfi, &action_info);
    BCM_SAND_IF_ERR_EXIT(sand_rv);
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int 
_bcm_petra_qos_map_ingress_cos_tc_dp_delete(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    bcm_error_t rv = BCM_E_NONE;
    int dev_id, dp;
    uint32 opcode_ndx, sand_rv;
    SOC_PPC_LIF_COS_OPCODE_ACTION_INFO action_info;
    SOC_PPC_LIF_COS_OPCODE_TYPE opcode_type;
    uint8 bit_val;
    BCMDNX_INIT_FUNC_DEFS;
    dev_id = (unit);
    opcode_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);
    
    SOC_PPC_LIF_COS_OPCODE_ACTION_INFO_clear(&action_info);

    rv = QOS_ACCESS.opcode_bmp.bit_get(unit, opcode_ndx, &bit_val);
    BCMDNX_IF_ERR_EXIT(rv);
    if (bit_val == 0) {
        sand_rv = soc_ppd_lif_cos_opcode_types_set(dev_id, opcode_ndx,(SOC_PPC_LIF_COS_OPCODE_TYPE_TC_DP));
        BCM_SAND_IF_ERR_EXIT(sand_rv);
    } else {
        sand_rv = soc_ppd_lif_cos_opcode_types_get(dev_id, opcode_ndx, &opcode_type);
        BCM_SAND_IF_ERR_EXIT(sand_rv);
        if ((opcode_type & SOC_PPC_LIF_COS_OPCODE_TYPE_TC_DP) == 0) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: invalid OPCODE TYPE (0x%x) \n"), FUNCTION_NAME(), opcode_type));
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Incorrect Opcode Index used")));
        }
    }

    BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_encode(unit,map->color,&dp));

    action_info.is_packet_valid = TRUE;
    action_info.is_qos_only = TRUE;
    action_info.ac_offset = 0;

    sand_rv = soc_ppd_lif_cos_opcode_tc_dp_map_set(dev_id, opcode_ndx, map->int_pri, dp, &action_info);
    BCM_SAND_IF_ERR_EXIT(sand_rv);
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int 
_bcm_petra_qos_map_ingress_cos_opcode_delete(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    bcm_error_t rv = BCM_E_NONE;
    int dev_id;
    uint32 opcode_ndx, sand_rv;
    SOC_PPC_LIF_COS_OPCODE_TYPE opcode_type, type;
    uint8 bit_val;
    BCMDNX_INIT_FUNC_DEFS;
 
    dev_id = (unit);
    opcode_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);

    if ((flags & BCM_QOS_MAP_IPV4) || (flags & BCM_QOS_MAP_IPV6)) {
        type = SOC_PPC_LIF_COS_OPCODE_TYPE_L3 | SOC_PPC_LIF_COS_OPCODE_TYPE_TC_DP;
    } else if (flags & BCM_QOS_MAP_L2) {
        type = SOC_PPC_LIF_COS_OPCODE_TYPE_L2;
    } else if (flags & BCM_QOS_MAP_L2_UNTAGGED) {
        type = SOC_PPC_LIF_COS_OPCODE_TYPE_TC_DP;
    } else {
        type = SOC_PPC_LIF_COS_OPCODE_TYPE_L3 | SOC_PPC_LIF_COS_OPCODE_TYPE_TC_DP;
    }

    rv = QOS_ACCESS.opcode_bmp.bit_get(unit, opcode_ndx, &bit_val);
    BCMDNX_IF_ERR_EXIT(rv);
    if (bit_val != 1) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: Invalid Map ID (0x%x), opcode not configured \n"), FUNCTION_NAME(), map_id));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Map ID Opcode not set.\n")));
    } else {
        sand_rv = soc_ppd_lif_cos_opcode_types_get(dev_id, opcode_ndx, &opcode_type);
        BCM_SAND_IF_ERR_EXIT(sand_rv);
        if (opcode_type != type) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: invalid OPCODE TYPE (0x%x) \n"), FUNCTION_NAME(), opcode_type));
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Map ID does not match flags.\n")));
        }
    }

    if (type & SOC_PPC_LIF_COS_OPCODE_TYPE_L3) {
        if (flags & BCM_QOS_MAP_IPV4) {
            rv = _bcm_petra_qos_map_ingress_cos_ipv4_delete(unit, flags, map, map_id);
        } 
        else if (flags & BCM_QOS_MAP_IPV6) {
            rv = _bcm_petra_qos_map_ingress_cos_ipv6_delete(unit, flags, map, map_id);
        } 
        else {
            rv = _bcm_petra_qos_map_ingress_cos_tc_dp_delete(unit, flags, map, map_id);
        }
    }
    else if (type & SOC_PPC_LIF_COS_OPCODE_TYPE_L2) {   /* L2 */
        rv = _bcm_petra_qos_map_ingress_cos_l2_delete(unit, flags, map, map_id);
    } 
    else if (type & SOC_PPC_LIF_COS_OPCODE_TYPE_TC_DP) { /* TC/DP */
        rv = _bcm_petra_qos_map_ingress_cos_tc_dp_delete(unit, flags, map, map_id);
    }

    BCMDNX_IF_ERR_EXIT(rv);


exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int 
_bcm_petra_qos_map_egress_remark_delete(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_NONE;
    int dev_id = 0, dp = 0;
    uint32 profile_ndx = 0;
    SOC_PPC_EG_QOS_MAP_KEY prm_in_qos_key;
    SOC_PPC_EG_QOS_PARAMS  prm_out_qos_params;
    SOC_PPC_EG_ENCAP_QOS_MAP_KEY prm_in_encap_qos_key;
    SOC_PPC_EG_ENCAP_QOS_PARAMS  prm_out_encap_qos_params;

    BCMDNX_INIT_FUNC_DEFS;
    dev_id = (unit);
    profile_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);

    SOC_PPC_EG_QOS_MAP_KEY_clear(&prm_in_qos_key);
    SOC_PPC_EG_QOS_PARAMS_clear(&prm_out_qos_params);
    SOC_PPC_EG_ENCAP_QOS_MAP_KEY_clear(&prm_in_encap_qos_key);
    SOC_PPC_EG_ENCAP_QOS_PARAMS_clear(&prm_out_encap_qos_params);

    BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_encode(unit,map->color,&dp));

    if (flags & BCM_QOS_MAP_ENCAP) {
      /* Currently unavailable, Arad  */ 
      if (flags & BCM_QOS_MAP_MPLS) {
          if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id) 
          {
              LOG_ERROR(BSL_LS_BCM_QOS,
                        (BSL_META_U(unit,
                                    "%s: invalid remark profile (%d) out of range\n"), FUNCTION_NAME(), profile_ndx));
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid REMARK profile")));
          }
          if ( SOC_DPP_CONFIG(unit)->qos.egr_remark_encap_enable != 1) {
              LOG_ERROR(BSL_LS_BCM_QOS,
                        (BSL_META_U(unit,
                                    "%s: encap remark is disabled. \n"), FUNCTION_NAME()));  
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Encap is disabled")));
          }
  
          /* internal pri */
          prm_in_encap_qos_key.in_dscp_exp = (uint32)map->remark_int_pri; 
          prm_in_encap_qos_key.remark_profile = profile_ndx;
          prm_in_encap_qos_key.pkt_hdr_type = SOC_PPC_PKT_HDR_TYPE_MPLS;


          SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_encap_qos_params_remark_get(dev_id,
                             &prm_in_encap_qos_key,
                             &prm_out_encap_qos_params));
          
          prm_out_encap_qos_params.mpls_exp = 0;
          prm_out_encap_qos_params.ip_dscp = 0;

          
          SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_encap_qos_params_remark_set(dev_id,
                             &prm_in_encap_qos_key,
                             &prm_out_encap_qos_params));


         /* the entry is not in use:
          * corresponding bit in bitmap egr_remark_encap_mpls_bitmap should be clear.
          */
         rv = QOS_ACCESS.egr_remark_encap_mpls_bitmap.bit_clear(unit, _DPP_QOS_ENTRY_INDEX_EGR_REMARK_ENP_MPLS(profile_ndx, prm_in_encap_qos_key.in_dscp_exp));
         BCMDNX_IF_ERR_EXIT(rv);
     }
     else if (flags & BCM_QOS_MAP_IPV6) 
     {
         if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id) 
          {
              LOG_ERROR(BSL_LS_BCM_QOS,
                        (BSL_META_U(unit,
                                    "%s: invalid remark profile (%d) out of range\n"), FUNCTION_NAME(), profile_ndx));
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid REMARK profile")));
          }
          if ( SOC_DPP_CONFIG(unit)->qos.egr_remark_encap_enable != 1) {
              LOG_ERROR(BSL_LS_BCM_QOS,
                        (BSL_META_U(unit,
                                    "%s: encap remark is disabled. \n"), FUNCTION_NAME()));  
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Encap is disabled")));
          }

          /* internal pri */
          prm_in_encap_qos_key.in_dscp_exp = (uint32)map->remark_int_pri; 
          prm_in_encap_qos_key.remark_profile = profile_ndx;
          prm_in_encap_qos_key.pkt_hdr_type = SOC_PPC_PKT_HDR_TYPE_IPV6;

          SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_encap_qos_params_remark_get(dev_id,
                             &prm_in_encap_qos_key,
                             &prm_out_encap_qos_params));
       
          prm_out_encap_qos_params.ip_dscp = 0; 
          prm_out_encap_qos_params.mpls_exp = 0;
          
          
          SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_encap_qos_params_remark_set(dev_id,
                             &prm_in_encap_qos_key,
                             &prm_out_encap_qos_params));


          /* the entry is not in use:
           * corresponding bit in bitmap egr_remark_encap_ipv6_bitmap should be clear.
           */
          rv = QOS_ACCESS.egr_remark_encap_ipv6_bitmap.bit_clear(unit, _DPP_QOS_ENTRY_INDEX_EGR_REMARK_ENP_L3(profile_ndx, prm_in_encap_qos_key.in_dscp_exp));
          BCMDNX_IF_ERR_EXIT(rv);
     }
     else if (flags & BCM_QOS_MAP_IPV4) {
        if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: invalid remark profile (%d) out of range\n"), FUNCTION_NAME(), profile_ndx));
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid REMARK profile")));
        }
        if ( SOC_DPP_CONFIG(unit)->qos.egr_remark_encap_enable != 1) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: encap remark is disabled. \n"), FUNCTION_NAME()));  
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Encap is disabled")));
        }

        /* internal pri */
        prm_in_encap_qos_key.in_dscp_exp = (uint32)map->remark_int_pri; 
        prm_in_encap_qos_key.remark_profile = profile_ndx;
        prm_in_encap_qos_key.pkt_hdr_type = SOC_PPC_PKT_HDR_TYPE_IPV4;

        SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_encap_qos_params_remark_get(dev_id,
                             &prm_in_encap_qos_key,
                             &prm_out_encap_qos_params));
       
        prm_out_encap_qos_params.ip_dscp= 0; 
        prm_out_encap_qos_params.mpls_exp = 0;
          
        SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_encap_qos_params_remark_set(dev_id,
                             &prm_in_encap_qos_key,
                             &prm_out_encap_qos_params));

        /* the entry is not in use:
         * corresponding bit in bitmap egr_remark_encap_ipv4_bitmap should be clear.
         */        
        rv = QOS_ACCESS.egr_remark_encap_ipv4_bitmap.bit_clear(unit, _DPP_QOS_ENTRY_INDEX_EGR_REMARK_ENP_L3(profile_ndx, prm_in_encap_qos_key.in_dscp_exp));
        BCMDNX_IF_ERR_EXIT(rv);
     }
     else if (flags & BCM_QOS_MAP_L2) { 
        /* Only used for ARAD_B0_and_above */
        if (SOC_IS_ARAD_A0(unit)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("REMARK flag L2 is not supported")));
        }

        if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id) 
        {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: invalid remark profile (%d) out of range\n"), FUNCTION_NAME(), profile_ndx));
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("REMARK profile is out of range")));
        }

        if (profile_ndx >= _BCM_QOS_MAP_EGR_REMARK_PROFILE_L2_MAX) 
        {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: invalid remark profile (%d) out of range\n"), FUNCTION_NAME(), profile_ndx));
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("REMARK profile is out of range. There are only 4 Remark profile for Ethernet encap remark")));
        }

        if ( SOC_DPP_CONFIG(unit)->qos.egr_remark_encap_enable != 1) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: encap remark is disabled. \n"), FUNCTION_NAME()));  
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Encap is disabled")));
        }

        /* internal pri */
        prm_in_encap_qos_key.in_dscp_exp = (uint32)(map->remark_int_pri & 0x7);
        prm_in_encap_qos_key.remark_profile = profile_ndx;
        prm_in_encap_qos_key.pkt_hdr_type = SOC_PPC_PKT_HDR_TYPE_ETH;
        BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_encode(unit,map->color,&dp)); /* dp */
        prm_in_encap_qos_key.dp = dp;

        SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_encap_qos_params_remark_get(dev_id,
                             &prm_in_encap_qos_key,
                             &prm_out_encap_qos_params));
       
        prm_out_encap_qos_params.ip_dscp= 0; 
        prm_out_encap_qos_params.mpls_exp = 0;
          
        SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_encap_qos_params_remark_set(dev_id,
                             &prm_in_encap_qos_key,
                             &prm_out_encap_qos_params));


        /* the entry is not in use:
         * corresponding bit in bitmap egr_remark_encap_l2_bitmap should be clear.
         */
        rv = QOS_ACCESS.egr_remark_encap_l2_bitmap.bit_clear(unit, _DPP_QOS_ENTRY_INDEX_EGR_REMARK_ENP_L2(profile_ndx, map->remark_int_pri, map->color));
        BCMDNX_IF_ERR_EXIT(rv);
     }
    }
    else { /* Not encap header */
        if (flags & BCM_QOS_MAP_MPLS) {
            if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id) {
        
                LOG_ERROR(BSL_LS_BCM_QOS,
                          (BSL_META_U(unit,
                                      "%s: invalid remark profile (%d) out of range\n"),
                                      FUNCTION_NAME(), profile_ndx));
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid REMARK profile")));
            }
        
            /* internal pri */
            prm_in_qos_key.remark_profile = profile_ndx;
            prm_in_qos_key.dp = dp;
            prm_in_qos_key.in_dscp_exp = (uint32)map->int_pri;
        
            SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_qos_params_remark_get(dev_id,
                                  &prm_in_qos_key,
                                  &prm_out_qos_params));
    
            prm_out_qos_params.mpls_exp = 0; /* set to lowest priority on delete */ 
        
            SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_qos_params_remark_set(dev_id,
                                  &prm_in_qos_key,
                                  &prm_out_qos_params));

            /* the entry is not in use:
             * corresponding bit in bitmap egr_remark_mpls_bitmap should be clear.
             */        
            rv = QOS_ACCESS.egr_remark_mpls_bitmap.bit_clear(unit, _DPP_QOS_ENTRY_INDEX_EGR_REMARK_MPLS(profile_ndx, prm_in_qos_key.in_dscp_exp, prm_in_qos_key.dp));
            BCMDNX_IF_ERR_EXIT(rv);
        }
        else if (flags & BCM_QOS_MAP_L3) {
        
            if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id) {
        
                LOG_ERROR(BSL_LS_BCM_QOS,
                          (BSL_META_U(unit,
                                      "%s: invalid remark profile (%d) out of range\n"),
                                      FUNCTION_NAME(), profile_ndx));
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid REMARK profile")));
            }
        
            /* internal pri */
            prm_in_qos_key.remark_profile = profile_ndx;
            prm_in_qos_key.dp = dp;
            prm_in_qos_key.in_dscp_exp = (uint32) map->int_pri;
        
            SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_qos_params_remark_get(dev_id,
                                  &prm_in_qos_key,
                                  &prm_out_qos_params));
        
            /* IPV6 */
            if (flags & BCM_QOS_MAP_IPV6) {
                prm_out_qos_params.ipv6_tc = 0; /* set to unmapped priority */
            }
            else { /* IPV4 */
                prm_out_qos_params.ipv4_tos = 0; 
            }
        
            SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_qos_params_remark_set(dev_id,
                                  &prm_in_qos_key,
                                  &prm_out_qos_params));

            if (flags & BCM_QOS_MAP_IPV6) {
                /* the entry is not in use:
                 * corresponding bit in bitmap egr_remark_ipv6_bitmap should be clear.
                 */                
                rv = QOS_ACCESS.egr_remark_ipv6_bitmap.bit_clear(unit, _DPP_QOS_ENTRY_INDEX_EGR_REMARK_L3(profile_ndx, prm_in_qos_key.in_dscp_exp, prm_in_qos_key.dp));
                BCMDNX_IF_ERR_EXIT(rv);
            } else {
                /* the entry is not in use:
                 * corresponding bit in bitmap egr_remark_ipv4_bitmap should be clear.
                 */            
                rv = QOS_ACCESS.egr_remark_ipv4_bitmap.bit_clear(unit, _DPP_QOS_ENTRY_INDEX_EGR_REMARK_L3(profile_ndx, prm_in_qos_key.in_dscp_exp, prm_in_qos_key.dp));
                BCMDNX_IF_ERR_EXIT(rv);
            }       
        } else {
            rv = BCM_E_UNAVAIL;
        }
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int 
_bcm_petra_qos_map_egress_mpls_php_delete(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{  
    int rv = BCM_E_NONE;
    int dev_id = 0;
    uint32 profile_ndx = 0;
    SOC_PPC_EG_QOS_PHP_REMARK_KEY prm_in_php_remark_qos_key;

    BCMDNX_INIT_FUNC_DEFS;
    dev_id = (unit);
    profile_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);

    SOC_PPC_EG_QOS_PHP_REMARK_KEY_clear(&prm_in_php_remark_qos_key);

    if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_egr_mpls_php) 
    {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: invalid MPLS PHP profile (%d) out of range\n"), FUNCTION_NAME(), profile_ndx));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid MPLS PHPs profile")));
    }


    if (flags & BCM_QOS_MAP_IPV6) {
          
        prm_in_php_remark_qos_key.exp = map->int_pri; /* In-DSCP-EXP before */
        prm_in_php_remark_qos_key.exp_map_profile = profile_ndx;
        prm_in_php_remark_qos_key.php_type = SOC_PPC_EG_QOS_UNIFORM_PHP_POP_INTO_IPV6;
                   
        SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_qos_params_php_remark_set(dev_id,
                             &prm_in_php_remark_qos_key,
                             0)); /* Out-DSCP-EXP after */

        /* the entry is not in use:
         * corresponding bit in bitmap egr_mpls_php_ipv6_bitmap should be clear.
         */
        rv = QOS_ACCESS.egr_mpls_php_ipv6_bitmap.bit_clear(unit, _DPP_QOS_ENTRY_INDEX_EGR_MPLS_PHP(profile_ndx, map->int_pri));
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else if (flags & BCM_QOS_MAP_IPV4) 
    {
        
        prm_in_php_remark_qos_key.exp = map->int_pri; /* In-DSCP-EXP before */
        prm_in_php_remark_qos_key.exp_map_profile = profile_ndx;
        prm_in_php_remark_qos_key.php_type = SOC_PPC_EG_QOS_UNIFORM_PHP_POP_INTO_IPV4;
          
        SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_qos_params_php_remark_set(dev_id,
                             &prm_in_php_remark_qos_key,
                             0)); /* Out-DSCP-EXP after */
          
        /* the entry is not in use:
         * corresponding bit in bitmap egr_mpls_php_ipv4_bitmap should be clear.
         */          
        rv = QOS_ACCESS.egr_mpls_php_ipv4_bitmap.bit_clear(unit, _DPP_QOS_ENTRY_INDEX_EGR_MPLS_PHP(profile_ndx, map->int_pri));
        BCMDNX_IF_ERR_EXIT(rv);
    }     
    else
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid MPLS PHPs flags")));
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int 
_bcm_petra_qos_map_egress_pcp_vlan_delete(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_NONE;
    int dev_id = 0, dp = 0;
    uint32 profile_ndx = 0;
    int count1 = 0, count2 = 0, count3 = 0;
    
    BCMDNX_INIT_FUNC_DEFS;
    dev_id = (unit);
    profile_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);
    
    BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_encode(unit,map->color,&dp));
    
    if ((flags & BCM_QOS_MAP_L2_VLAN_PCP) == 0) {
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("flag BCM_QOS_MAP_L2_VLAN_PCP must be set")));
    }
    
    if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_egr_pcp_vlan) {
    
    LOG_ERROR(BSL_LS_BCM_QOS,
              (BSL_META_U(unit,
                          "%s: invalid egress vlan pcp profile (%d) out of range\n"),
                          FUNCTION_NAME(), profile_ndx));
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid PCP VLAN profile")));
    }
    
    if (flags & BCM_QOS_MAP_L2) {
    /* eg_vlan_edit */
    
    map->pkt_pri = 0;
    map->pkt_cfi = 0;
    
    if (flags & BCM_QOS_MAP_L2_INNER_TAG) {
        
        SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_map_ctag_set(dev_id, 
                                   profile_ndx, 
                                   map->int_pri, 
                                   map->pkt_pri, 
                                   map->pkt_cfi));

        /* the entry is not in use:
         * corresponding bit in bitmap egr_pcp_vlan_ctag_bitmap should be clear.
         */        
        rv = QOS_ACCESS.egr_pcp_vlan_ctag_bitmap.bit_clear(unit, _DPP_QOS_ENTRY_INDEX_EGR_PCP_VLAN_CTAG(profile_ndx, map->int_pri));
        BCMDNX_IF_ERR_EXIT(rv);
    } else if (flags & BCM_QOS_MAP_L2_UNTAGGED) {
        
        SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_map_untagged_set(dev_id, 
                                       profile_ndx, 
                                       map->int_pri, 
                                       dp, 
                                       map->pkt_pri, 
                                       map->pkt_cfi));

        /* the entry is not in use:
         * corresponding bit in bitmap egr_pcp_vlan_utag_bitmap should be clear.
         */
        rv = QOS_ACCESS.egr_pcp_vlan_utag_bitmap.bit_clear(unit, _DPP_QOS_ENTRY_INDEX_EGR_PCP_VLAN_UTAG(profile_ndx, map->int_pri, dp));
        BCMDNX_IF_ERR_EXIT(rv);
    } else { /* OUTER TAG */
        
        SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_map_stag_set(dev_id, 
                                   profile_ndx, 
                                   map->int_pri, 
                                   map->color, 
                                   map->pkt_pri, 
                                   map->pkt_cfi));

        /* the entry is not in use:
         * corresponding bit in bitmap egr_pcp_vlan_stag_bitmap should be clear.
         */        
        rv = QOS_ACCESS.egr_pcp_vlan_stag_bitmap.bit_clear(unit, _DPP_QOS_ENTRY_INDEX_EGR_PCP_VLAN_STAG(profile_ndx, map->int_pri, map->color));
        BCMDNX_IF_ERR_EXIT(rv);
    }
    
    } else if (flags & BCM_QOS_MAP_MPLS) { 
    /* Arad only */ 
       if ( SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_enable != 1) {
               LOG_ERROR(BSL_LS_BCM_QOS,
                         (BSL_META_U(unit,
                                     "%s: dscp_exp mapping is disabled. \n"), FUNCTION_NAME()));  
               BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("DSCP_EXP mapping is disabled")));
       }
       
       if ( SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_profile_id == -1) {  /* no profile is set to dscp_exp mapping yet */
            SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_profile_id = profile_ndx;
       } 
      
       if (profile_ndx !=  SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_profile_id) {
           LOG_ERROR(BSL_LS_BCM_QOS,
                     (BSL_META_U(unit,
                                 "%s: invalid egress vlan pcp profile (%d). profile ndx for dscp_exp mapping is set to (%d). \n"), FUNCTION_NAME(), SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_enable, profile_ndx)); 
           BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid PCP VLAN profile")));
       }
       SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_map_dscp_exp_set(dev_id, 
               SOC_PPC_PKT_FRWRD_TYPE_MPLS,  
               map->exp, 
               map->pkt_pri, 
               map->pkt_cfi));
       
        /* the entry is not in use:
         * corresponding bit in bitmap egr_pcp_vlan_mpls_bitmap should be clear.
         */   
        rv = QOS_ACCESS.egr_pcp_vlan_mpls_bitmap.bit_clear(unit, _DPP_QOS_ENTRY_INDEX_EGR_PCP_VLAN_MPLS(profile_ndx, map->exp));
        BCMDNX_IF_ERR_EXIT(rv);    

        _DPP_QOS_ENTRY_USED_CNT(egr_pcp_vlan_mpls_bitmap, profile_ndx, _BCM_QOS_MAP_EGR_EXP_MAX, count1);
        if (count1 == 0) {
            _DPP_QOS_ENTRY_USED_CNT(egr_pcp_vlan_ipv6_bitmap, profile_ndx, _BCM_QOS_MAP_EGR_DSCP_MAX, count2);
            _DPP_QOS_ENTRY_USED_CNT(egr_pcp_vlan_ipv4_bitmap, profile_ndx, _BCM_QOS_MAP_EGR_DSCP_MAX, count3);
            if ((count2 == 0) && (count3 == 0)) {
                SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_profile_id = -1; 
            }
        }
    } else if (flags & BCM_QOS_MAP_L3) {
    /* Arad only */ 

       if ( SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_enable != 1) {
               LOG_ERROR(BSL_LS_BCM_QOS,
                         (BSL_META_U(unit,
                                     "%s: dscp_exp mapping is disabled. \n"), FUNCTION_NAME()));  
               BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("DSCP_EXP mapping is disabled")));
       }
      
       if ( SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_profile_id == -1) {  /* no profile is set to dscp_exp mapping yet */
            SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_profile_id = profile_ndx;
       }
 
       if (profile_ndx !=  SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_profile_id) {
           LOG_ERROR(BSL_LS_BCM_QOS,
                     (BSL_META_U(unit,
                                 "%s: invalid egress vlan pcp profile (%d). profile ndx for dscp_exp mapping is set to (%d). \n"), FUNCTION_NAME(), SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_enable, profile_ndx)); 
           BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid PCP VLAN profile")));
       }
       if (flags & BCM_QOS_MAP_IPV6) {
           SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_map_dscp_exp_set(dev_id, 
                  SOC_PPC_PKT_FRWRD_TYPE_IPV6_UC,  
                  map->dscp, 
                  map->pkt_pri, 
                  map->pkt_cfi));
           SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_map_dscp_exp_set(dev_id, 
                  SOC_PPC_PKT_FRWRD_TYPE_IPV6_MC,  
                  map->dscp, 
                  map->pkt_pri, 
                  map->pkt_cfi));

           /* the entry is not in use:
            * corresponding bit in bitmap egr_pcp_vlan_ipv6_bitmap should be clear.
            */ 
           rv = QOS_ACCESS.egr_pcp_vlan_ipv6_bitmap.bit_clear(unit, _DPP_QOS_ENTRY_INDEX_EGR_PCP_VLAN_L3(profile_ndx, map->dscp));
           BCMDNX_IF_ERR_EXIT(rv);           

           _DPP_QOS_ENTRY_USED_CNT(egr_pcp_vlan_ipv6_bitmap, profile_ndx, _BCM_QOS_MAP_EGR_DSCP_MAX, count1);
           if (count1 == 0) {
               _DPP_QOS_ENTRY_USED_CNT(egr_pcp_vlan_ipv4_bitmap, profile_ndx, _BCM_QOS_MAP_EGR_DSCP_MAX, count2);
               _DPP_QOS_ENTRY_USED_CNT(egr_pcp_vlan_mpls_bitmap, profile_ndx, _BCM_QOS_MAP_EGR_EXP_MAX, count3);
               if ((count2 == 0) && (count3 == 0)) {
                   SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_profile_id = -1; 
               }
           }
       } else if (flags & BCM_QOS_MAP_IPV4) {
           SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_map_dscp_exp_set(dev_id, 
                  SOC_PPC_PKT_FRWRD_TYPE_IPV4_UC,  
                  map->dscp, 
                  map->pkt_pri, 
                  map->pkt_cfi));
           SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_map_dscp_exp_set(dev_id, 
                  SOC_PPC_PKT_FRWRD_TYPE_IPV4_MC,  
                  map->dscp, 
                  map->pkt_pri, 
                  map->pkt_cfi));

           /* the entry is not in use:
            * corresponding bit in bitmap egr_pcp_vlan_ipv4_bitmap should be clear.
            */
           rv = QOS_ACCESS.egr_pcp_vlan_ipv4_bitmap.bit_clear(unit, _DPP_QOS_ENTRY_INDEX_EGR_PCP_VLAN_L3(profile_ndx, map->dscp));
           BCMDNX_IF_ERR_EXIT(rv);

           _DPP_QOS_ENTRY_USED_CNT(egr_pcp_vlan_ipv4_bitmap, profile_ndx, _BCM_QOS_MAP_EGR_DSCP_MAX, count1);
           if (count1 == 0) {
               _DPP_QOS_ENTRY_USED_CNT(egr_pcp_vlan_ipv6_bitmap, profile_ndx, _BCM_QOS_MAP_EGR_DSCP_MAX, count2);            
               _DPP_QOS_ENTRY_USED_CNT(egr_pcp_vlan_mpls_bitmap, profile_ndx, _BCM_QOS_MAP_EGR_EXP_MAX, count3);
               if ((count2 == 0) && (count3 == 0)) {
                   SOC_DPP_CONFIG(unit)->qos.egr_pcp_vlan_dscp_exp_profile_id = -1; 
               }
           }
       }      
     }
     else 
     {
        rv = BCM_E_UNAVAIL;
     }
    
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int 
_bcm_petra_qos_map_egress_l2_i_tag_delete(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_NONE;
    int dev_id = 0, dp = 0;
    uint32 profile_ndx = 0;
    
    BCMDNX_INIT_FUNC_DEFS;
    dev_id = (unit);
    profile_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);
    
    BCMDNX_IF_ERR_EXIT(_bcm_petra_port_color_encode(unit,map->color,&dp));
        
    if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_egr_l2_i_tag) {
    
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: invalid egress l2 i tag (%d) out of range\n"),
                              FUNCTION_NAME(), profile_ndx));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid L2 I-TAG profile")));
    }
        
    map->pkt_pri = 0;
    map->pkt_cfi = 0;
    
    SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_vlan_edit_pcp_map_stag_set(dev_id, 
                               profile_ndx, 
                               map->int_pri, 
                               map->color, 
                               map->pkt_pri, 
                               map->pkt_cfi));

    /* the entry is not in use:
     * corresponding bit in bitmap egr_l2_i_tag_bitmap should be clear.
     */
    rv = QOS_ACCESS.egr_l2_i_tag_bitmap.bit_clear(unit, _DPP_QOS_ENTRY_INDEX_EGR_L2_I_TAG(profile_ndx, map->int_pri, dp));
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


#ifdef BCM_88660
STATIC int 
_bcm_petra_qos_map_egress_dscp_exp_marking_delete(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_NONE;
    uint32 profile_ndx = 0;
    
    BCMDNX_INIT_FUNC_DEFS;
    profile_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);
    
    if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_egr_dscp_exp_marking) {
    
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: invalid egress DSCP/EXP marking profile (%d) out of range\n"),
                              FUNCTION_NAME(), profile_ndx));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid DSCP/EXP profile")));
    }

    map->dscp = 0;
    map->exp = 0;

    rv = _bcm_petra_qos_map_egress_dscp_exp_marking_add(unit, flags, map, map_id);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}
#endif /* BCM_88660 */


static int 
_bcm_petra_qos_map_ingress_delete(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;
    if (BCM_INT_QOS_MAP_IS_LIF_COS(map_id)) {
    rv = _bcm_petra_qos_map_ingress_lif_cos_delete(unit, flags, map, map_id);
    } else if (BCM_INT_QOS_MAP_IS_INGRESS_PCP_VLAN(map_id)) {
    rv = _bcm_petra_qos_map_ingress_pcp_vlan_delete(unit, flags, map, map_id);
    } else if (BCM_INT_QOS_MAP_IS_MPLS_ELSP(map_id)) {
    rv = _bcm_petra_qos_map_ingress_elsp_delete(unit, flags, map, map_id);
    } else if (BCM_INT_QOS_MAP_IS_OPCODE(map_id)) {
    rv = _bcm_petra_qos_map_ingress_cos_opcode_delete(unit, flags, map, map_id);
    } else if (BCM_INT_QOS_MAP_IS_INGRESS_COLOR(map_id)) {
    rv = _bcm_petra_qos_map_ingress_color_delete(unit, flags, map, map_id);
    }else {
    rv = BCM_E_UNAVAIL;
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int 
_bcm_petra_qos_map_egress_delete(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_NONE;



    BCMDNX_INIT_FUNC_DEFS;
    if (BCM_INT_QOS_MAP_IS_REMARK(map_id)) {
    rv = _bcm_petra_qos_map_egress_remark_delete(unit, flags, map, map_id);
    BCMDNX_IF_ERR_EXIT(rv);
    BCM_EXIT;

    } else if (BCM_INT_QOS_MAP_IS_EGRESS_PCP_VLAN(map_id)) {
        rv = _bcm_petra_qos_map_egress_pcp_vlan_delete(unit, flags, map, map_id);
    } else if (BCM_INT_QOS_MAP_IS_EGRESS_L2_I_TAG(map_id)) {
        rv = _bcm_petra_qos_map_egress_l2_i_tag_delete(unit, flags, map, map_id);
    } else if (BCM_INT_QOS_MAP_IS_EGRESS_MPLS_PHP(map_id)) {
        rv = _bcm_petra_qos_map_egress_mpls_php_delete(unit, flags, map, map_id);
#ifdef BCM_88660
    } else if (SOC_IS_ARADPLUS(unit) && BCM_INT_QOS_MAP_IS_EGRESS_DSCP_EXP_MARKING(map_id)) {
        rv = _bcm_petra_qos_map_egress_dscp_exp_marking_delete(unit, flags, map, map_id);
#endif /* BCM_88660 */
    }
    else if (!SOC_IS_ARADPLUS_AND_BELOW(unit) && BCM_INT_QOS_MAP_ENCAP_INTPRI_COLOR(map_id)) 
    {
        rv = _bcm_petra_qos_map_encap_intpri_color_delete(unit, flags, map_id);
    }
    else 
    {
        rv = BCM_E_UNAVAIL;
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


int 
bcm_petra_qos_map_delete(int unit, uint32 flags, bcm_qos_map_t *map, int map_id)
{
    int rv = BCM_E_UNAVAIL;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    rv = _bcm_petra_qos_map_add_params_verify(unit, flags, map, map_id);
    BCMDNX_IF_ERR_EXIT(rv);

    if (BCM_INT_QOS_MAP_IS_INGRESS(map_id))  {
        rv = _bcm_petra_qos_map_ingress_delete(unit, flags, map, map_id);
    } else if (BCM_INT_QOS_MAP_IS_EGRESS(map_id)) {
        rv = _bcm_petra_qos_map_egress_delete(unit, flags, map, map_id);
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_petra_qos_ingress_pcp_vlan_gport_map_set(int unit, 
                          bcm_gport_t gport, 
                          int map_id)
{    
    unsigned int dev_id = 0;
    int is_local_conf;
    uint32 soc_sand_rv;
    SOC_PPC_LIF_ENTRY_INFO *lif_entry_info = NULL;
    int rv;
    _bcm_dpp_gport_hw_resources gport_hw_resources;

    BCMDNX_INIT_FUNC_DEFS;
    dev_id = (unit);

    rv = _bcm_dpp_gport_is_local(unit, gport, &is_local_conf);
    BCMDNX_IF_ERR_EXIT(rv);    

    if(is_local_conf) {        
        BCMDNX_ALLOC(lif_entry_info, sizeof(SOC_PPC_LIF_ENTRY_INFO), "_bcm_petra_qos_ingress_pcp_vlan_gport_map_set.lif_entry_info");
        if (lif_entry_info == NULL) {        
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
        }

        /* map gport to used LIF */
        rv = _bcm_dpp_gport_to_hw_resources(unit, gport, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK,  
                                                     &gport_hw_resources);
        BCMDNX_IF_ERR_EXIT(rv);    


        soc_sand_rv = soc_ppd_lif_table_entry_get(dev_id, gport_hw_resources.local_in_lif, lif_entry_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (lif_entry_info->type == SOC_PPC_LIF_ENTRY_TYPE_AC) {
            lif_entry_info->value.ac.ing_edit_info.edit_pcp_profile = map_id;
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("gport LIF is not of type AC")));
        }

        soc_sand_rv = soc_ppd_lif_table_entry_update(dev_id, gport_hw_resources.local_in_lif, lif_entry_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
    BCM_EXIT;
exit:
    BCM_FREE(lif_entry_info);
    BCMDNX_FUNC_RETURN;
}
STATIC int
_bcm_petra_qos_ingress_lif_cos_gport_map_set(int unit, 
                        bcm_gport_t gport, 
                        int map_id)
{    
    unsigned int dev_id = 0;
    int is_local_conf;
    uint32 soc_sand_rv;
    SOC_PPC_LIF_ENTRY_INFO *lif_entry_info = NULL;
    int rv;
    _bcm_dpp_gport_hw_resources gport_hw_resources;

    BCMDNX_INIT_FUNC_DEFS;
    dev_id = (unit);

    rv = _bcm_dpp_gport_is_local(unit, gport, &is_local_conf);
    BCMDNX_IF_ERR_EXIT(rv);    

    if(is_local_conf) {
        BCMDNX_ALLOC(lif_entry_info, sizeof(SOC_PPC_LIF_ENTRY_INFO), "_bcm_petra_qos_ingress_lif_cos_gport_map_set.lif_entry_info");
        if (lif_entry_info == NULL) {        
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
        }

        /* map gport to used LIF */
        rv = _bcm_dpp_gport_to_hw_resources(unit, gport, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, 
                                                     &gport_hw_resources);
        BCMDNX_IF_ERR_EXIT(rv);    


        soc_sand_rv = soc_ppd_lif_table_entry_get(dev_id, gport_hw_resources.local_in_lif, lif_entry_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (lif_entry_info->type == SOC_PPC_LIF_ENTRY_TYPE_PWE){
            lif_entry_info->value.pwe.cos_profile = map_id;
        }
        else if (lif_entry_info->type == SOC_PPC_LIF_ENTRY_TYPE_AC) {
            if ((SOC_DPP_CONFIG(unit)->pp.l3_source_bind_mode != SOC_DPP_L3_SOURCE_BIND_MODE_DISABLE) || 
            (SOC_DPP_CONFIG(unit)->pp.vmac_enable) || (SOC_DPP_CONFIG(unit)->pp.local_switching_enable)) {
                lif_entry_info->value.ac.cos_profile &= 0x20;
                lif_entry_info->value.ac.cos_profile |= map_id;
            } else {
                lif_entry_info->value.ac.cos_profile = map_id; 
            }
        }
        else if (lif_entry_info->type == SOC_PPC_LIF_ENTRY_TYPE_ISID) {
            lif_entry_info->value.isid.cos_profile = map_id; 
        }
        else if (lif_entry_info->type == SOC_PPC_LIF_ENTRY_TYPE_IP_TUNNEL_RIF) {
            lif_entry_info->value.ip_term_info.cos_profile = map_id;
        }
        else if (lif_entry_info->type == SOC_PPC_LIF_ENTRY_TYPE_MPLS_TUNNEL_RIF) {
            lif_entry_info->value.mpls_term_info.cos_profile = map_id;
        }
        else if (lif_entry_info->type == SOC_PPC_LIF_ENTRY_TYPE_EXTENDER) {
            lif_entry_info->value.extender.cos_profile = map_id;
        }
        else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("gport LIF should be of type PWE, AC, ISID or Extender")));
        }
        soc_sand_rv = soc_ppd_lif_table_entry_update(dev_id, gport_hw_resources.local_in_lif, lif_entry_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    } else {

    }
    BCM_EXIT;
exit:
    BCM_FREE(lif_entry_info);
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_petra_qos_ingress_opcode_gport_map_set(int unit, 
                                            bcm_gport_t gport, 
                                            int map_id)
{    
    unsigned int dev_id = 0;
    int is_local_conf;
    uint32 soc_sand_rv;
    SOC_PPC_LIF_ENTRY_INFO *lif_entry_info = NULL;
    SOC_PPC_L2_LIF_AC_KEY  ac_key;
    SOC_PPC_L2_LIF_AC_GROUP_INFO *acs_group_info = NULL;
    int rv, opcode_ndx;
    SOC_PPC_LIF_ID base_lif_index = 0;
    bcm_vlan_port_t vlan_port;
    SOC_SAND_SUCCESS_FAILURE success;
    uint8 found;
    _bcm_dpp_gport_hw_resources gport_hw_resources;

    BCMDNX_INIT_FUNC_DEFS;
    dev_id = (unit);
    opcode_ndx = BCM_QOS_MAP_PROFILE_GET(map_id);
    
    SOC_PPC_L2_LIF_AC_KEY_clear(&ac_key);

    ac_key.inner_vid = SOC_PPC_LIF_IGNORE_INNER_VID;
    ac_key.outer_vid = SOC_PPC_LIF_IGNORE_OUTER_VID;

    bcm_vlan_port_t_init(&vlan_port);

    rv = _bcm_dpp_gport_is_local(unit, gport, &is_local_conf);
    BCMDNX_IF_ERR_EXIT(rv);    

    if(is_local_conf) {

        BCMDNX_ALLOC(lif_entry_info, sizeof(SOC_PPC_LIF_ENTRY_INFO), "_bcm_petra_qos_ingress_opcode_gport_map_set.lif_entry_info");
        if (lif_entry_info == NULL) {        
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
        }

        /* map gport to used LIF */
        rv = _bcm_dpp_gport_to_hw_resources(unit, gport, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK,  
                                                     &gport_hw_resources);
        BCMDNX_IF_ERR_EXIT(rv);   

        soc_sand_rv = soc_ppd_lif_table_entry_get(dev_id, gport_hw_resources.local_in_lif, lif_entry_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (lif_entry_info->type & (SOC_PPC_LIF_ENTRY_TYPE_AC|
                                   SOC_PPC_LIF_ENTRY_TYPE_FIRST_AC_IN_GROUP |
                                   SOC_PPC_LIF_ENTRY_TYPE_MIDDLE_AC_IN_GROUP)) {
            lif_entry_info->value.ac.cos_profile = opcode_ndx; 

            soc_sand_rv = soc_ppd_lif_table_entry_update(dev_id, gport_hw_resources.local_in_lif, lif_entry_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            rv = _bcm_dpp_in_lif_ac_match_get(unit, &vlan_port, gport_hw_resources.local_in_lif);
            BCMDNX_IF_ERR_EXIT(rv);

            rv = _bcm_dpp_gport_fill_ac_key(unit, &vlan_port, BCM_GPORT_VLAN_PORT, &ac_key);
            
            BCMDNX_ALLOC(acs_group_info, sizeof(SOC_PPC_L2_LIF_AC_GROUP_INFO), "_bcm_petra_qos_ingress_opcode_gport_map_set.acs_group_info");
            if (acs_group_info == NULL) {        
                BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
            }
            soc_sand_rv = soc_ppd_l2_lif_ac_with_cos_get(dev_id, &ac_key, &base_lif_index, acs_group_info, &found);

            if (found == TRUE) {
                acs_group_info->opcode_id = opcode_ndx;

                soc_sand_rv = soc_ppd_l2_lif_ac_with_cos_add(dev_id, &ac_key, base_lif_index, acs_group_info, &success);
                BCMDNX_IF_ERR_EXIT(translate_sand_success_failure(success));
            } else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Lif needs to have vlan group configured\n")));
            }
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Lif needs to be type AC\n")));
        }
    }

    BCM_EXIT;

exit:
    BCM_FREE(lif_entry_info);
    BCM_FREE(acs_group_info);
    BCMDNX_FUNC_RETURN;
}

int 
_bcm_petra_qos_ingress_gport_map_validate(int unit, bcm_gport_t gport, int ing_map)
{
    int rv = BCM_E_NONE;
    int is_local_conf;
    uint32 soc_sand_rv;    
    int profile_ndx;
    SOC_PPC_LIF_ENTRY_INFO lif_entry_info;
    int vsi;
    _bcm_dpp_gport_hw_resources gport_hw_resources;

    BCMDNX_INIT_FUNC_DEFS;

    profile_ndx = BCM_QOS_MAP_PROFILE_GET(ing_map);
    
    rv = _bcm_dpp_gport_is_local(unit, gport, &is_local_conf);
    BCMDNX_IF_ERR_EXIT(rv);    

    if(is_local_conf) {
        /* validity check: P2P has only 4 bits instead of 6, user can not configure cos profile that is > 16 for PWE P2P */        
        rv = _bcm_dpp_gport_to_hw_resources(unit, gport, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK,  
                                                     &gport_hw_resources);
        BCMDNX_IF_ERR_EXIT(rv);   

        soc_sand_rv = soc_ppd_lif_table_entry_get(unit, gport_hw_resources.local_in_lif, &lif_entry_info);
        SOC_SAND_IF_ERR_RETURN(soc_sand_rv);

        if (lif_entry_info.type == SOC_PPC_LIF_ENTRY_TYPE_PWE) {
            rv = _bcm_dpp_local_lif_sw_resources_vsi_get(unit, gport_hw_resources.local_in_lif, &vsi);
            BCMDNX_IF_ERR_EXIT(rv);

            if (((vsi==0) || (vsi==-1)) && (profile_ndx > _BCM_DPP_QOS_PWE_P2P_MAX_PROFILE)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("For PWE P2P, cos profile value range is 0-15")));
            }
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}


int 
_bcm_petra_qos_ingress_gport_map_set(int unit, bcm_gport_t gport, int ing_map)
{
    int rv = BCM_E_NONE;
    int profile_ndx;
    
    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_petra_qos_ingress_gport_map_validate(unit, gport, ing_map);
    BCMDNX_IF_ERR_EXIT(rv);

    profile_ndx = BCM_QOS_MAP_PROFILE_GET(ing_map);
    
    if (BCM_INT_QOS_MAP_IS_LIF_COS(ing_map)) {
    if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_ing_lif_cos) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: invalid lif_cos profile (%d) out of range\n"),
                              FUNCTION_NAME(), profile_ndx));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid LIF COS profile")));
    } else {

      rv = _bcm_petra_qos_ingress_lif_cos_gport_map_set(unit, 
                                gport, 
                                profile_ndx);
      if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: error updating qos ingress map id\n"),FUNCTION_NAME()));
        BCMDNX_IF_ERR_EXIT(rv);
      }       
    }   
    } else if (BCM_INT_QOS_MAP_IS_OPCODE(ing_map)) {
    if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_ing_cos_opcode) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: invalid opcode profile (%d) out of range\n"),
                              FUNCTION_NAME(), profile_ndx));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Opcode index out of range\n")));
    } else {

      rv = _bcm_petra_qos_ingress_opcode_gport_map_set(unit, gport, ing_map);
      if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: error updating qos ingress opcode map id\n"),FUNCTION_NAME()));
        BCMDNX_IF_ERR_EXIT(rv);
      }       
    }   
    } else if (BCM_INT_QOS_MAP_IS_PCP_VLAN(ing_map)) {
        if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_ing_pcp_vlan) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: invalid ing pcp vlan profile (%d) out of range\n"),
                              FUNCTION_NAME(), profile_ndx));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid PCP VLAN profile")));
    } else {
        rv = _bcm_petra_qos_ingress_pcp_vlan_gport_map_set(unit, 
                                   gport, 
                                   profile_ndx);
        if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: error updating qos ingress map id\n"),FUNCTION_NAME()));
        BCMDNX_IF_ERR_EXIT(rv);
        }         
    }   
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int 
_bcm_petra_qos_egress_gport_map_set(int unit, bcm_gport_t gport, int egr_map)
{
    int rv = BCM_E_NONE;
    int profile_ndx;
    SOC_PPC_EG_AC_INFO eg_ac_info;    
    int is_local_conf;
    int port_i;
    int tunnel_id, core;
    int is_uniform;
    SOC_PPC_EG_ENCAP_IP_TUNNEL_SRC_IP sip_val;
    int ttl_val, tos_val;
    int sip_template, ttl_template, tos_template, is_allocated;
    SOC_PPC_PORT soc_ppd_port;
    SOC_PPC_EG_QOS_PORT_INFO eg_qos_port_info;
    SOC_PPC_EG_ENCAP_ENTRY_INFO   *encap_entry_info = NULL;
    uint32         next_eep[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX];
    uint32         nof_entries;
    uint32          soc_sand_rv;
    _bcm_dpp_gport_info_t gport_info;
    _bcm_dpp_gport_hw_resources gport_hw_resources;
    _bcm_lif_type_e lif_usage;

    BCMDNX_INIT_FUNC_DEFS;
    
    if (BCM_INT_QOS_MAP_IS_REMARK(egr_map)) {

        profile_ndx = BCM_QOS_MAP_PROFILE_GET(egr_map);
        if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: invalid remark profile (%d) out of range\n"), FUNCTION_NAME(), BCM_QOS_MAP_PROFILE_GET(egr_map)));
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid REMARK profile")));
        }
        if (BCM_GPORT_IS_MPLS_PORT(gport)) { 
            rv = BCM_E_NONE;
        }

        BCMDNX_ALLOC(encap_entry_info, sizeof(*encap_entry_info) * SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX,
            "_bcm_petra_qos_egress_gport_map_set.encap_entry_info");
        if(encap_entry_info == NULL) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failue\n")));
        }

        if (BCM_GPORT_IS_TRILL_PORT(gport)) {
            int encap_id;
            int trill_global_out_lif, trill_local_out_lif;
            encap_id = BCM_GPORT_TRILL_PORT_ID_GET(gport);
            /* get the outlif from the encap */
            trill_global_out_lif = BCM_GPORT_SUB_TYPE_TRILL_OUTLIF_GET(encap_id);

            rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, trill_global_out_lif, &trill_local_out_lif);
            BCMDNX_IF_ERR_EXIT(rv);
            rv = soc_ppd_eg_encap_entry_get(
                   unit, SOC_PPC_EG_ENCAP_EEP_TYPE_TRILL, trill_local_out_lif, 0, encap_entry_info, next_eep, &nof_entries);
            BCM_SAND_IF_ERR_EXIT(rv);

            encap_entry_info[0].entry_val.trill_encap_info.remark_profile = profile_ndx;
            rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_eg_trill_entry_set,(unit,trill_local_out_lif, &encap_entry_info[0].entry_val.trill_encap_info)));
            BCMDNX_IF_ERR_EXIT(rv);

            goto exit;
        }
        if (!BCM_GPORT_IS_TUNNEL(gport)){
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: tunnel_id (%d) is not a legal ipv4 tunnel\n"), FUNCTION_NAME(), gport));
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Gport is not an ipv4 tunnel")));
        }

        rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, BCM_GPORT_TUNNEL_ID_GET(gport), &tunnel_id);
        BCMDNX_IF_ERR_EXIT(rv);

        if(tunnel_id == _SHR_GPORT_INVALID) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: tunnel_id (%d) is not a legal ipv4 tunnel\n"), FUNCTION_NAME(), gport));
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Illegal tunnel_id")));
        } else {
            BCMDNX_IF_ERR_EXIT( _bcm_dpp_local_lif_sw_resources_lif_usage_get(unit, -1,tunnel_id, NULL,&lif_usage) );

            /*for jer/qmx Roo Link layer qos map*/
            if (lif_usage == _bcmDppLifTypeOverlayLinkLayer) {
                  if(SOC_IS_JERICHO(unit) && !SOC_IS_QAX(unit)) {
			    rv = soc_ppd_eg_encap_entry_get(
			       unit, SOC_PPC_EG_ENCAP_EEP_TYPE_ROO_LL, tunnel_id, 0, encap_entry_info, next_eep, &nof_entries);
			    BCM_SAND_IF_ERR_EXIT(rv);

			    /*For LL-Arp  format entry, remark-profile has only 3 bits from EEDB entry*/
			    encap_entry_info[0].entry_val.overlay_arp_encap_info.remark_profile = (BCM_QOS_MAP_PROFILE_GET(egr_map) & BCM_INT_QOS_MAP_LL_ARP_PROFILE_MASK);
			    rv = soc_ppd_eg_encap_overlay_arp_data_entry_add(unit, tunnel_id, &encap_entry_info[0].entry_val.overlay_arp_encap_info);
			    BCMDNX_IF_ERR_EXIT(rv);

                  } else {
                            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Qos mapping for Roo link layer only support on JERICO")));
                  }
	          goto exit;
            }



            rv = _bcm_dpp_am_template_ip_tunnel_data_get(unit, tunnel_id, &sip_val, &ttl_val, &tos_val);
            BCMDNX_IF_ERR_EXIT(rv);
            
            is_uniform = SOC_SAND_PP_TOS_IS_UNIFORM_GET(tos_val);

            if (!is_uniform){
                LOG_ERROR(BSL_LS_BCM_QOS,
                          (BSL_META_U(unit,
                                      "%s: tunnel_id (%d) is not uniform\n"), FUNCTION_NAME(), BCM_QOS_MAP_PROFILE_GET(egr_map)));
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid REMARK profile")));
            }

            SOC_SAND_PP_TOS_SET(tos_val, profile_ndx, is_uniform);
            rv = _bcm_dpp_am_template_ip_tunnel_exchange(unit, tunnel_id, 0, &sip_val, ttl_val, tos_val, &sip_template, &ttl_template, &tos_template, &is_allocated); /*encode tos val and update*/
            BCMDNX_IF_ERR_EXIT(rv);

            if(is_allocated) {
                /* TOS */
                soc_sand_rv = soc_ppd_eg_encap_ipv4_tunnel_glbl_tos_set(unit, tos_template, tos_val);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }

            /* get the encap entry info so we can update the tos value */
            soc_sand_rv =
            soc_ppd_eg_encap_entry_get(unit,
                                   SOC_PPC_EG_ENCAP_EEP_TYPE_TUNNEL_EEP,
                                   tunnel_id, 1,
                                   encap_entry_info, next_eep,
                                   &nof_entries);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            if(encap_entry_info[0].entry_type != SOC_PPC_EG_ENCAP_ENTRY_TYPE_IPV4_ENCAP) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("given Tunnel ID has to point to IP tunnel encapsulation")));
            }
            
            encap_entry_info[0].entry_val.ipv4_encap_info.dest.tos_index = tos_template;
        
            /*return identical encap entry, but with different tos value */
            soc_sand_rv =
                soc_ppd_eg_encap_ipv4_entry_add(unit, tunnel_id, 
                                                                        &(encap_entry_info[0].entry_val.ipv4_encap_info), next_eep[0]);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            
        }
    } 
    else if (BCM_INT_QOS_MAP_IS_EGRESS_MPLS_PHP(egr_map)) {
        if (BCM_QOS_MAP_PROFILE_GET(egr_map) >= SOC_DPP_CONFIG(unit)->qos.nof_egr_mpls_php) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: invalid mpls php profile (%d) out of range\n"), FUNCTION_NAME(), BCM_QOS_MAP_PROFILE_GET(egr_map)));
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid EGRESS MPLS PHP profile")));
        }

        BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, gport, 0, &gport_info));

        BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
            BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port, &core)));

            SOC_PPC_EG_QOS_PORT_INFO_clear(&eg_qos_port_info);
            SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_qos_port_info_get(unit, core, soc_ppd_port, &eg_qos_port_info));

            eg_qos_port_info.exp_map_profile = BCM_QOS_MAP_PROFILE_GET(egr_map);

            SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_qos_port_info_set(unit, core, soc_ppd_port, &eg_qos_port_info));
        }

    }
#ifdef BCM_88660
    else if (SOC_IS_ARADPLUS(unit) && BCM_INT_QOS_MAP_IS_EGRESS_DSCP_EXP_MARKING(egr_map)) {
        if (BCM_QOS_MAP_PROFILE_GET(egr_map) >= SOC_DPP_CONFIG(unit)->qos.nof_egr_mpls_php) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: invalid DSCP/EXP marking profile (%d) out of range\n"), FUNCTION_NAME(), BCM_QOS_MAP_PROFILE_GET(egr_map)));
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid DSCP/EXP marking profile")));
        }

        BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, gport, 0, &gport_info));

        BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
            BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port, &core)));

            SOC_PPC_EG_QOS_PORT_INFO_clear(&eg_qos_port_info);
            SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_qos_port_info_get(unit, core, soc_ppd_port, &eg_qos_port_info));

            eg_qos_port_info.marking_profile = BCM_QOS_MAP_PROFILE_GET(egr_map);
            SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_qos_port_info_set(unit, core, soc_ppd_port, &eg_qos_port_info));
        }

    }
#endif /* BCM_88660 */
    else if (SOC_IS_JERICHO(unit) && BCM_INT_QOS_MAP_IS_OAM_EGRESS_PROFILE(egr_map) ) {
        uint32 full_outlif_profile;
        
        if (SOC_IS_JERICHO_B0_AND_ABOVE(unit)) {
            /* Mapping doesn't work anyways on the above devices*/
            rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_set,
                                     (unit,SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_APP_OAM_PCP , BCM_QOS_MAP_PROFILE_GET(egr_map),&full_outlif_profile));
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }
    else {
        profile_ndx = BCM_QOS_MAP_PROFILE_GET(egr_map);
    
        if (BCM_INT_QOS_MAP_IS_EGRESS_PCP_VLAN(egr_map)) {
            if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_egr_pcp_vlan) {
        
                LOG_ERROR(BSL_LS_BCM_QOS,
                          (BSL_META_U(unit,
                                      "%s: invalid egress pcp vlan profile (%d) out of range\n"), FUNCTION_NAME(), profile_ndx));
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid PCP VLAN profile")));
            } 
            else { 

                rv = _bcm_dpp_gport_is_local(unit, gport, &is_local_conf);
                BCMDNX_IF_ERR_EXIT(rv);    
                
                if(is_local_conf) {
                    /* map gport to used LIF */
                    rv = _bcm_dpp_gport_to_hw_resources(unit, gport, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK,  
                                                                 &gport_hw_resources);
                    BCMDNX_IF_ERR_EXIT(rv);   

                    SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_ac_info_get(unit, gport_hw_resources.local_out_lif, &eg_ac_info));
            
                    eg_ac_info.edit_info.pcp_profile = profile_ndx;
            
                    SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_ac_info_set(unit, gport_hw_resources.local_out_lif, &eg_ac_info));
                }
            }
        } 
        else {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: invalid profile, must be egress pcp vlan type\n"),FUNCTION_NAME()));
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid profile")));
        }
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCM_FREE(encap_entry_info);
    BCMDNX_FUNC_RETURN;
}

/* Attach a QoS map to an object (Gport) */
int 
bcm_petra_qos_port_map_set(int unit, bcm_gport_t port, int ing_map, int egr_map)
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    if (ing_map != -1)  {
        rv = _bcm_petra_qos_ingress_gport_map_set(unit, port, ing_map);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    if (egr_map != -1) {
        rv = _bcm_petra_qos_egress_gport_map_set(unit, port, egr_map);
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_petra_qos_ingress_lif_cos_gport_map_get(int unit, 
                         bcm_gport_t gport, 
                         int *map_id)
{
    unsigned int dev_id = 0;
    int is_local_conf;
    uint32 soc_sand_rv;
    SOC_PPC_LIF_ENTRY_INFO lif_entry_info;
    SOC_PPC_L2_LIF_AC_GROUP_INFO *acs_group_info = NULL;
    bcm_error_t rv;
    int profile_ndx = -1;
    _bcm_dpp_gport_hw_resources gport_hw_resources;

    BCMDNX_INIT_FUNC_DEFS;
    *map_id = -1;

    dev_id = (unit);
   
    rv = _bcm_dpp_gport_is_local(unit, gport, &is_local_conf);
    BCMDNX_IF_ERR_EXIT(rv);    

    if(is_local_conf) {

        /* map gport to used LIF */
        rv = _bcm_dpp_gport_to_hw_resources(unit, gport, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK,  
                                                     &gport_hw_resources);
        BCMDNX_IF_ERR_EXIT(rv);   

        soc_sand_rv = soc_ppd_lif_table_entry_get(dev_id, gport_hw_resources.local_in_lif, &lif_entry_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (lif_entry_info.type == SOC_PPC_LIF_ENTRY_TYPE_PWE){
            profile_ndx = lif_entry_info.value.pwe.cos_profile;         
        }
        else if (lif_entry_info.type == SOC_PPC_LIF_ENTRY_TYPE_AC) {
            if ((SOC_DPP_CONFIG(unit)->pp.l3_source_bind_mode != SOC_DPP_L3_SOURCE_BIND_MODE_DISABLE) || 
            (SOC_DPP_CONFIG(unit)->pp.vmac_enable) || (SOC_DPP_CONFIG(unit)->pp.local_switching_enable)) {
                profile_ndx = lif_entry_info.value.ac.cos_profile & 0x1f;
            } else {
                profile_ndx = lif_entry_info.value.ac.cos_profile;
            }
        }
        else if (lif_entry_info.type == SOC_PPC_LIF_ENTRY_TYPE_ISID) {
            profile_ndx = lif_entry_info.value.isid.cos_profile;
        }
        else if (lif_entry_info.type == SOC_PPC_LIF_ENTRY_TYPE_IP_TUNNEL_RIF) {
            profile_ndx = lif_entry_info.value.ip_term_info.cos_profile;
        }
        else if (lif_entry_info.type == SOC_PPC_LIF_ENTRY_TYPE_MPLS_TUNNEL_RIF) {
            profile_ndx = lif_entry_info.value.mpls_term_info.cos_profile;
        }
        else if (lif_entry_info.type == SOC_PPC_LIF_ENTRY_TYPE_EXTENDER) {
            profile_ndx = lif_entry_info.value.extender.cos_profile;
        }
        else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("gport LIF should be of type PWE, AC, ISID or Extender")));
        }
    }
    else {
        BCM_EXIT;
    }

    if ( (profile_ndx != -1) && (bcm_dpp_am_qos_ing_lif_cos_is_alloced(unit, profile_ndx) == BCM_E_EXISTS)) {
        BCM_INT_QOS_MAP_LIF_COS_SET(*map_id, profile_ndx);
    }
    BCM_EXIT;
exit:
    BCM_FREE(acs_group_info);
    BCMDNX_FUNC_RETURN;
}


int
_bcm_petra_qos_ingress_pcp_vlan_gport_map_get(int unit, 
                          bcm_gport_t gport, 
                          int *map_id)
{
    unsigned int dev_id = 0;
    int is_local_conf;
    uint32 soc_sand_rv;
    SOC_PPC_LIF_ENTRY_INFO lif_entry_info;
    int rv;
    int profile_ndx = -1;
    _bcm_dpp_gport_hw_resources gport_hw_resources;

    BCMDNX_INIT_FUNC_DEFS;
    *map_id = -1;

    dev_id = (unit);

    rv = _bcm_dpp_gport_is_local(unit, gport, &is_local_conf);
    BCMDNX_IF_ERR_EXIT(rv);    

    if(is_local_conf) {

        /* map gport to used LIF */
        rv = _bcm_dpp_gport_to_hw_resources(unit, gport, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK,  
                                                     &gport_hw_resources);
        BCMDNX_IF_ERR_EXIT(rv);   

        soc_sand_rv = soc_ppd_lif_table_entry_get(dev_id, gport_hw_resources.local_in_lif, &lif_entry_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (lif_entry_info.type == SOC_PPC_LIF_ENTRY_TYPE_AC) {
            profile_ndx = lif_entry_info.value.ac.ing_edit_info.edit_pcp_profile;
        }
        else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("gport LIF is not of type AC")));
        }
    }
    if ( (profile_ndx != -1) && (bcm_dpp_am_qos_ing_pcp_vlan_is_alloced(unit, profile_ndx) == BCM_E_EXISTS)) {
        BCM_INT_QOS_MAP_INGRESS_PCP_VLAN_SET(*map_id, profile_ndx);
    }
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_petra_qos_ingress_color_gport_map_get(int unit, uint32 flags,
                          bcm_gport_t gport, 
                          int *map_id)
{
    unsigned int dev_id = 0;
    int is_local_conf;
    uint32 soc_sand_rv;
    SOC_PPC_LIF_ENTRY_INFO lif_entry_info;
    int rv;
    int profile_ndx = -1;
    _bcm_dpp_gport_hw_resources gport_hw_resources;
    int in_lif_profile =0;
    BCMDNX_INIT_FUNC_DEFS;
    *map_id = -1;

    dev_id = (unit);

    rv = _bcm_dpp_gport_is_local(unit, gport, &is_local_conf);
    BCMDNX_IF_ERR_EXIT(rv);    

    if(is_local_conf) {

        /* map gport to used LIF */
        rv = _bcm_dpp_gport_to_hw_resources(unit, gport, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK,  
                                                     &gport_hw_resources);
        BCMDNX_IF_ERR_EXIT(rv);   

        soc_sand_rv = soc_ppd_lif_table_entry_get(dev_id, gport_hw_resources.local_in_lif, &lif_entry_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (lif_entry_info.type == SOC_PPC_LIF_ENTRY_TYPE_AC) {
            in_lif_profile = lif_entry_info.value.ac.lif_profile;
            if (flags & BCM_QOS_MAP_L2_OUTER_TAG) {
                BCMDNX_IF_ERR_EXIT(MBCM_PP_DRIVER_CALL(dev_id, mbcm_pp_lif_ing_vlan_edit_outer_global_info_get, (unit,in_lif_profile,&profile_ndx)));
            }
            else if (flags & BCM_QOS_MAP_L2_INNER_TAG)
            {
				BCMDNX_IF_ERR_EXIT(MBCM_PP_DRIVER_CALL(dev_id, mbcm_pp_lif_ing_vlan_edit_inner_global_info_get, (unit,in_lif_profile,&profile_ndx)));
            }
        }
        else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("gport LIF is not of type AC")));
        }
    }
    if ( (profile_ndx != -1) && (bcm_dpp_am_qos_ing_color_is_alloced(unit, profile_ndx) == BCM_E_EXISTS)) {
        BCM_INT_QOS_MAP_INGRESS_COLOR_SET(*map_id, profile_ndx);
    }
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


/**
 * If (flags & BCM_QOS_MAP_INGRESS) then the flags are used to determine which profile should be returned. 
 */
int 
_bcm_petra_qos_ingress_gport_map_get(int unit, bcm_gport_t gport, int *ing_map, uint32 flags)
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    if (!(flags & BCM_QOS_MAP_INGRESS)) {
      flags = 0;
      /* -1 ingress map indicates user wants LIF COS map */
      if (*ing_map == -1) {
        flags |= BCM_QOS_MAP_L2_VLAN_PCP;
      }
    }

    if (flags & BCM_QOS_MAP_L2_VLAN_PCP) {
        rv = _bcm_petra_qos_ingress_pcp_vlan_gport_map_get(unit, gport, ing_map);
    } else  if (flags & BCM_QOS_MAP_POLICER) {
        rv = _bcm_petra_qos_ingress_color_gport_map_get(unit,flags, gport, ing_map);
    } else {
        rv = _bcm_petra_qos_ingress_lif_cos_gport_map_get(unit, gport, ing_map);
    }

    BCMDNX_IF_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG_STR("%s: error getting qos ingress map id\n"), FUNCTION_NAME()));

exit:
    BCMDNX_FUNC_RETURN;
}

/**
 * If flags & EGRESS then the function uses the flags to determine the profile. 
 * Otherwise the gport is used. 
 *  
 * The profiles are generally only dependant on the gport, however if the port is system/local then 
 * there are two options: MPLS PHP and DSCP/EXP marking. 
 * If flags are present then this is easy. 
 * Otherwise, if a gport has both profiles then the MPLS PHP profile is returned. 
 *  
 * We assume the flags have been validated. 
 */
int 
_bcm_petra_qos_egress_gport_map_get(int unit, bcm_gport_t gport, int *egr_map, uint32 flags)
{
    int profile_ndx = 0; /* only 1 id is allocated for egress SOC_PETRA */
    int rv = BCM_E_NONE, core;
    SOC_PPC_EG_AC_INFO eg_ac_info;
    SOC_PPC_EG_QOS_PORT_INFO eg_qos_port_info;
    int is_local_conf;
    int tunnel_id;
    SOC_PPC_EG_ENCAP_IP_TUNNEL_SRC_IP sip_val;
    int ttl_val, tos_val;
    SOC_PPC_PORT soc_ppd_port;
    _bcm_dpp_gport_info_t gport_info;
    _bcm_dpp_gport_hw_resources gport_hw_resources;
    BCMDNX_INIT_FUNC_DEFS;
    /* Set to invalid map */
    *egr_map = -1;
    
    /* if port is system port / local port */
    if (BCM_GPORT_IS_LOCAL(gport) || BCM_GPORT_IS_SYSTEM_PORT(gport) || BCM_GPORT_IS_MODPORT(gport) || (SOC_PORT_VALID(unit,gport))) {
      BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, gport, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));
    
      BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, gport_info.local_port, &soc_ppd_port, &core)));

      SOC_PPC_EG_QOS_PORT_INFO_clear(&eg_qos_port_info);

      SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_qos_port_info_get(unit, core, soc_ppd_port, &eg_qos_port_info));

      if (flags & BCM_QOS_MAP_EGRESS) {
        if (flags & BCM_QOS_MAP_MPLS_PHP) {
          BCM_INT_QOS_MAP_EGRESS_MPLS_PHP_SET(*egr_map, eg_qos_port_info.exp_map_profile);
#ifdef BCM_88660_A0
        } else if (SOC_IS_ARADPLUS(unit)) {
          if (flags & BCM_QOS_MAP_L3_L2) {
            BCM_INT_QOS_MAP_EGRESS_DSCP_EXP_MARKING_SET(*egr_map, eg_qos_port_info.marking_profile);
          }
#endif /* BCM_88660_A0 */
        }
      } else {
        BCM_INT_QOS_MAP_EGRESS_MPLS_PHP_SET(*egr_map, eg_qos_port_info.exp_map_profile);
      }
    } else {

        if (BCM_GPORT_IS_TUNNEL(gport)) {
            
            rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, BCM_GPORT_TUNNEL_ID_GET(gport), &tunnel_id);
            BCMDNX_IF_ERR_EXIT(rv);


            if(tunnel_id == _SHR_GPORT_INVALID) {
                LOG_ERROR(BSL_LS_BCM_QOS,
                          (BSL_META_U(unit,
                                      "%s: tunnel_id (%d) is not a legal ipv4 tunnel\n"), FUNCTION_NAME(), gport));
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Illegal tunnel_id")));
            } 

            rv = _bcm_dpp_am_template_ip_tunnel_data_get(unit, tunnel_id, &sip_val, &ttl_val, &tos_val);
            BCMDNX_IF_ERR_EXIT(rv);
            
            tos_val = SOC_SAND_PP_TOS_VAL_GET(tos_val);
            BCM_INT_QOS_MAP_REMARK_SET(*egr_map, tos_val);

        } else if (BCM_GPORT_IS_VLAN_PORT(gport)) {
            
            rv = _bcm_dpp_gport_is_local(unit, gport, &is_local_conf);
            BCMDNX_IF_ERR_EXIT(rv);    

            if(is_local_conf) {
                /* map gport to used LIF */
                rv = _bcm_dpp_gport_to_hw_resources(unit, gport, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK,  
                                                     &gport_hw_resources);
                BCMDNX_IF_ERR_EXIT(rv);   
                  
                SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_ac_info_get(unit, gport_hw_resources.local_out_lif, &eg_ac_info));
                  
                profile_ndx = eg_ac_info.edit_info.pcp_profile;
            }
            BCM_INT_QOS_MAP_EGRESS_PCP_VLAN_SET(*egr_map, profile_ndx);      
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid gport type")));
        }
    }


    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

/**
 * Validate that flags are valid and match the gport type. 
 */
int
_bcm_petra_qos_port_map_get_validate_flags(int unit, bcm_gport_t gport, uint32 flags)
{
  BCMDNX_INIT_FUNC_DEFS;

  if (flags & BCM_QOS_MAP_INGRESS) {
    /* Always valid */

  } else if (flags & BCM_QOS_MAP_EGRESS) {
    const char *valid_string = NULL;
    uint32 valid_flags = 0;
    if (BCM_GPORT_IS_LOCAL(gport) || BCM_GPORT_IS_SYSTEM_PORT(gport) || BCM_GPORT_IS_MODPORT(gport) || (SOC_PORT_VALID(unit,gport))) {
      valid_string = "only BCM_QOS_MAP_MPLS_PHP is valid";
      valid_flags = BCM_QOS_MAP_MPLS_PHP;

#ifdef BCM_88660_A0
      if (SOC_IS_ARADPLUS(unit)) {
        valid_string = "one of BCM_QOS_MAP_MPLS_PHP and BCM_QOS_MAP_L3_l2 must be specified";
        valid_flags |= BCM_QOS_MAP_L3_L2 ;
      }
#endif /* BCM_88660_A0 */
    } else if (BCM_GPORT_IS_TUNNEL(gport) || BCM_GPORT_IS_VLAN_PORT(gport)) {
      /* Always valid */
      valid_flags = ~0;
    } else {
      BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG_STR("%s: Illegal gport.\n"), FUNCTION_NAME()));
    }

    if (!(flags & valid_flags)) {
      BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG_STR("%s: Illegal flags (%s).\n"), FUNCTION_NAME(), valid_string));
    }
  } else {
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG_STR("%s: Illegal flags (one of BCM_QOS_MAP_INGERSS and BCM_QOS_MAP_EGRESS must be specified).\n"), FUNCTION_NAME()));
  }

exit:
  BCMDNX_FUNC_RETURN;
  
}

/* bcm_petra_qos_port_map_get */
int
bcm_petra_qos_port_map_get(int unit, bcm_gport_t gport, 
                         int *ing_map, int *egr_map)
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    if (ing_map) {
        rv = _bcm_petra_qos_ingress_gport_map_get(unit, gport, ing_map, 0);
    }
    BCMDNX_IF_ERR_EXIT(rv);
    if (egr_map) {
        rv = _bcm_petra_qos_egress_gport_map_get(unit, gport, egr_map, 0);
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_qos_port_map_type_get(int unit, bcm_gport_t gport, uint32 flags, int *map_id)
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    
    if (!map_id) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG_STR("%s: map_id is NULL\n"),FUNCTION_NAME()));
    }

    BCMDNX_IF_ERR_EXIT(_bcm_petra_qos_port_map_get_validate_flags(unit, gport, flags));

    if (flags & BCM_QOS_MAP_INGRESS) {
        rv = _bcm_petra_qos_ingress_gport_map_get(unit, gport, map_id, flags);
    } else if (flags & BCM_QOS_MAP_EGRESS) {
        rv = _bcm_petra_qos_egress_gport_map_get(unit, gport, map_id, flags);
    }
    BCMDNX_IF_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG_STR("%s: error getting qos map id\n"),FUNCTION_NAME()));

exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_petra_qos_ingress_port_vlan_map_set(int unit,  bcm_port_t port, bcm_vlan_t vid,
                     int ing_map)
{
    int rv = BCM_E_NONE;
    SOC_PPC_L2_LIF_AC_KEY ac_key;
    SOC_PPC_LIF_ID lif_id;
    SOC_PPC_L2_LIF_AC_INFO ac_info;
    uint8 found;
    int profile_ndx, port_i;
    int core;
    SOC_SAND_SUCCESS_FAILURE soc_sand_success;
    SOC_PPC_PORT soc_ppd_port;
    SOC_PPC_PORT_INFO port_info;
    uint32 soc_sand_rv;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    profile_ndx = BCM_QOS_MAP_PROFILE_GET(ing_map);
    
    if (BCM_INT_QOS_MAP_IS_INGRESS_PCP_VLAN(ing_map)) {
        if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_ing_pcp_vlan) {

            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: invalid ingress pcp vlan profile (%d) out of range\n"),
                                  FUNCTION_NAME(), profile_ndx));
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid PCP VLAN profile")));
        }
    } else if (BCM_INT_QOS_MAP_IS_LIF_COS(ing_map)) {
        if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_ing_lif_cos) {

            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: invalid ingress lif cos profile (%d) out of range\n"),
                                  FUNCTION_NAME(), profile_ndx));
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid LIF COS profile")));
        }
    } else {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: invalid map type for this association map_id(0x%08x)\n"),
                              FUNCTION_NAME(), ing_map));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("map type should be PCP VLAN or LIF COS")));

    }

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info));    
    
    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
    
        SOC_PPC_L2_LIF_AC_INFO_clear(&ac_info);
        SOC_PPC_L2_LIF_AC_KEY_clear(&ac_key);

        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port, &core)));

        soc_sand_rv = soc_ppd_port_info_get(unit, core, soc_ppd_port, &port_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        ac_key.key_type     = SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_VLAN;
        ac_key.vlan_domain  = port_info.vlan_domain;
        ac_key.outer_vid    = vid;
        ac_key.inner_vid    = SOC_PPC_LIF_IGNORE_INNER_VID;

        SOC_SAND_IF_ERROR_RETURN(soc_ppd_l2_lif_ac_get(unit, &ac_key, &lif_id, &ac_info, &found));
        
        if (found == FALSE) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: Lif AC not found for vid(%d) port(%d)\n"),
                                  FUNCTION_NAME(), vid, port));
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("AC LIF was not found for these vid and port")));
        }

        if (BCM_INT_QOS_MAP_IS_INGRESS_PCP_VLAN(ing_map)) {
            ac_info.ing_edit_info.edit_pcp_profile = profile_ndx;
        } else {
            if ((SOC_DPP_CONFIG(unit)->pp.l3_source_bind_mode != SOC_DPP_L3_SOURCE_BIND_MODE_DISABLE) || 
            (SOC_DPP_CONFIG(unit)->pp.vmac_enable) || (SOC_DPP_CONFIG(unit)->pp.local_switching_enable)) {
                ac_info.cos_profile &= 0x20;
                ac_info.cos_profile |= profile_ndx;
            } else {
                ac_info.cos_profile = profile_ndx;
            }
        }

        SOC_SAND_IF_ERROR_RETURN(soc_ppd_l2_lif_ac_add(unit, &ac_key, lif_id, &ac_info, &soc_sand_success));
        
        BCMDNX_IF_ERR_EXIT(translate_sand_success_failure(soc_sand_success));
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}




int
_bcm_dpp_qos_egress_port_vlan_map_ac_key_to_local_outlif(int unit, int port, int vid, SOC_PPC_L2_LIF_AC_KEY *ac_key, SOC_PPC_LIF_ID *local_outlif){
    int rv;
    int global_lif_id;
    SOC_PPC_LIF_ID local_inlif_id;
    uint8 found;
    SOC_PPC_L2_LIF_AC_INFO ac_info;

    BCMDNX_INIT_FUNC_DEFS;
    
    rv = soc_ppd_l2_lif_ac_get(unit, ac_key, &local_inlif_id, &ac_info, &found);
    SOC_SAND_IF_ERROR_RETURN(rv);
    if (found == FALSE) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: Lif AC not found for vid(%d) port(%d)\n"), FUNCTION_NAME(), vid, port));
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("AC LIF was not found for these vid and port")));
    }
    
    rv = _bcm_dpp_global_lif_mapping_local_to_global_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS, local_inlif_id, &global_lif_id);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, global_lif_id, (int*) local_outlif);
    BCMDNX_IF_ERR_EXIT(rv);
    
exit:
    BCMDNX_FUNC_RETURN;
}
int
_bcm_petra_qos_egress_port_vlan_map_set(int unit,  bcm_port_t port, bcm_vlan_t vid,
                     int egr_map)
{

    int rv = BCM_E_NONE;
    unsigned int dev_id = 0;
    int profile_ndx;
    SOC_PPC_LIF_ID lif_id;
    uint8 found;
    SOC_PPC_EG_AC_INFO eg_ac_info;
    SOC_PPC_EG_AC_VBP_KEY vbp_key;
    SOC_PPC_EG_AC_CEP_PORT_KEY cep_key;
    uint8 is_cep = 0;
    bcm_vlan_port_t port_vlan;
    uint32 soc_sand_rv;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    profile_ndx = BCM_QOS_MAP_PROFILE_GET(egr_map);
    
    if (BCM_INT_QOS_MAP_IS_EGRESS_PCP_VLAN(egr_map)) {

        if (profile_ndx >= SOC_DPP_CONFIG(unit)->qos.nof_egr_pcp_vlan) {

            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: invalid egress pcp vlan profile (%d) out of range\n"), FUNCTION_NAME(), profile_ndx));
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid PCP VLAN profile")));
        } 
        else { 
            dev_id = (unit);

            BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info));
            if (!_BCM_DPP_GPORT_INFO_IS_LOCAL_PORT(gport_info)) { /* Nothing to do here */
                BCM_EXIT;
            }
            
            bcm_vlan_port_t_init(&port_vlan);
            port_vlan.match_vlan = vid;
            port_vlan.port = port;
            port_vlan.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
            BCMDNX_IF_ERR_EXIT(bcm_vlan_port_find(unit, &port_vlan));

            rv = _bcm_dpp_gport_fill_out_ac_key(unit, &port_vlan, &is_cep, &vbp_key, &cep_key);
            BCMDNX_IF_ERR_EXIT(rv);

            /* Get out ac id */
            if (!is_cep) {
                soc_sand_rv = soc_ppd_eg_ac_port_vsi_info_get(unit, &vbp_key, &lif_id, &eg_ac_info, &found);               
            } else {
                soc_sand_rv = soc_ppd_eg_ac_port_cvid_info_get(unit, &cep_key, &lif_id, &eg_ac_info, &found);                
            }
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            if (found == FALSE) {
                LOG_ERROR(BSL_LS_BCM_QOS,
                          (BSL_META_U(unit,
                                      "%s: Lif AC not found for vid(%d) port(%d)\n"), FUNCTION_NAME(), vid, port));
                BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("AC LIF was not found for these vid and port")));
            }   
            SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_ac_info_get(dev_id, lif_id, &eg_ac_info));

            eg_ac_info.edit_info.pcp_profile = profile_ndx;
    
            SOC_SAND_IF_ERROR_RETURN(soc_ppd_eg_ac_info_set(dev_id, lif_id, &eg_ac_info));
        }
    } 
    else {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: invalid profile, must be egress pcp vlan type\n"),FUNCTION_NAME()));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("profile must be of type PCP VLAN")));
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

/* bcm_petra_qos_port_vlan_map_set */
int
bcm_petra_qos_port_vlan_map_set(int unit,  bcm_port_t port, bcm_vlan_t vid,
                int ing_map, int egr_map)
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    if (ing_map != -1)  {
    rv = _bcm_petra_qos_ingress_port_vlan_map_set(unit, port, vid, ing_map);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    if (egr_map != -1) {
    rv = _bcm_petra_qos_egress_port_vlan_map_set(unit, port, vid, egr_map);
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}
int
_bcm_petra_qos_ingress_lif_cos_port_vlan_map_get(int unit, 
                         bcm_port_t port,
                         bcm_vlan_t vid,
                         int *map_id)
{
    int rv = BCM_E_NONE;
    SOC_PPC_L2_LIF_AC_KEY ac_key;
    SOC_PPC_LIF_ID lif_id;
    SOC_PPC_L2_LIF_AC_INFO ac_info;
    uint8 found;
    int profile_ndx=0, port_i, core;
    SOC_PPC_PORT soc_ppd_port;
    SOC_PPC_PORT_INFO port_info;
    uint32 soc_sand_rv;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info));
        
    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {

        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port, &core)));
        
        SOC_PPC_L2_LIF_AC_INFO_clear(&ac_info);
        SOC_PPC_L2_LIF_AC_KEY_clear(&ac_key);

        soc_sand_rv = soc_ppd_port_info_get(unit, core, soc_ppd_port, &port_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        
        ac_key.key_type     = SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_VLAN;    
        ac_key.vlan_domain  = port_info.vlan_domain;
        ac_key.outer_vid    = vid;    
        ac_key.inner_vid    = SOC_PPC_LIF_IGNORE_INNER_VID;
        
        SOC_SAND_IF_ERROR_RETURN(soc_ppd_l2_lif_ac_get(unit, &ac_key, &lif_id, &ac_info, &found));
        
        if (found == FALSE) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: Lif AC not found for vid(%d) port(%d)\n"), FUNCTION_NAME(), vid, port));
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("AC LIF not found for these vid and port")));
        }

        if ((SOC_DPP_CONFIG(unit)->pp.l3_source_bind_mode != SOC_DPP_L3_SOURCE_BIND_MODE_DISABLE) || 
        (SOC_DPP_CONFIG(unit)->pp.vmac_enable) || (SOC_DPP_CONFIG(unit)->pp.local_switching_enable)) {
            profile_ndx = ac_info.cos_profile & 0x1f;
        } else {
            profile_ndx = ac_info.cos_profile;
        }

    }

    if (bcm_dpp_am_qos_ing_lif_cos_is_alloced(unit, profile_ndx) == BCM_E_EXISTS) {
        BCM_INT_QOS_MAP_LIF_COS_SET(*map_id, profile_ndx);
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_petra_qos_ingress_pcp_vlan_port_vlan_map_get(int unit, 
                          bcm_port_t port,
                          bcm_vlan_t vlan,
                          int *map_id)
{
    int rv = BCM_E_NONE;
    SOC_PPC_L2_LIF_AC_KEY ac_key;
    SOC_PPC_LIF_ID lif_id;
    SOC_PPC_L2_LIF_AC_INFO ac_info;
    uint8 found=0;
    int profile_ndx=0, port_i;
    int core;
    SOC_PPC_PORT soc_ppd_port;
    SOC_PPC_PORT_INFO port_info;
    uint32 soc_sand_rv;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info));
        
    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
    
        SOC_PPC_L2_LIF_AC_INFO_clear(&ac_info);
        SOC_PPC_L2_LIF_AC_KEY_clear(&ac_key);

        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port, &core)));

        soc_sand_rv = soc_ppd_port_info_get(unit, core, soc_ppd_port, &port_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        
        ac_key.key_type     = SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_PORT_VLAN;
        ac_key.vlan_domain  = port_info.vlan_domain;
        ac_key.outer_vid    = vlan;
        ac_key.inner_vid    = SOC_PPC_LIF_IGNORE_INNER_VID;
        
        SOC_SAND_IF_ERROR_RETURN(soc_ppd_l2_lif_ac_get(unit, &ac_key, &lif_id, &ac_info, &found));
        
        if (found == FALSE) {
            LOG_ERROR(BSL_LS_BCM_QOS,
                      (BSL_META_U(unit,
                                  "%s: Lif AC not found for vid(%d) port(%d)\n"), FUNCTION_NAME(), vlan, port));
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("AC LIF not found for these vid and port")));
        }
        
        profile_ndx = ac_info.ing_edit_info.edit_pcp_profile;
    }

    if (bcm_dpp_am_qos_ing_pcp_vlan_is_alloced(unit, profile_ndx) == BCM_E_EXISTS) {
    BCM_INT_QOS_MAP_INGRESS_PCP_VLAN_SET(*map_id, profile_ndx);
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int 
_bcm_petra_qos_ingress_port_vlan_map_get(int unit, bcm_port_t port, bcm_vlan_t vlan, int *ing_map)
{
    int rv = BCM_E_NONE;
    
    BCMDNX_INIT_FUNC_DEFS;
    /* -1 ingress map indicates user wants LIF COS map */
    if (*ing_map == -1) {       
    rv = _bcm_petra_qos_ingress_lif_cos_port_vlan_map_get(unit, 
                                  port,
                                  vlan,
                                  ing_map);
    if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: error getting qos ingress map id\n"),FUNCTION_NAME()));
        BCMDNX_IF_ERR_EXIT(rv);
    }         
    } else {
    rv = _bcm_petra_qos_ingress_pcp_vlan_port_vlan_map_get(unit, 
                                   port,
                                   vlan,
                                   ing_map);
    if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: error getting qos ingress map id\n"),FUNCTION_NAME()));
        BCMDNX_IF_ERR_EXIT(rv);
    }         
    }   
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int 
_bcm_petra_qos_egress_port_vlan_map_get(int unit, bcm_port_t port, bcm_vlan_t vid, int *egr_map)
{
    int rv = BCM_E_NONE;
    int profile_ndx;
    SOC_PPC_LIF_ID lif_id;
    uint8 found;
    SOC_PPC_EG_AC_INFO eg_ac_info;
    SOC_PPC_EG_AC_VBP_KEY vbp_key;
    SOC_PPC_EG_AC_CEP_PORT_KEY cep_key;
    uint8 is_cep = 0;
    bcm_vlan_port_t port_vlan;
    uint32 soc_sand_rv;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info));
    if (!_BCM_DPP_GPORT_INFO_IS_LOCAL_PORT(gport_info)) { /* Nothing to do here */
        BCM_EXIT;
    }
    
    bcm_vlan_port_t_init(&port_vlan);
    port_vlan.match_vlan = vid;
    port_vlan.port = port;
    port_vlan.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    BCMDNX_IF_ERR_EXIT(bcm_vlan_port_find(unit, &port_vlan));

    rv = _bcm_dpp_gport_fill_out_ac_key(unit, &port_vlan, &is_cep, &vbp_key, &cep_key);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Get out ac id */
    if (!is_cep) {
        soc_sand_rv = soc_ppd_eg_ac_port_vsi_info_get(unit, &vbp_key, &lif_id, &eg_ac_info, &found);               
    } else {
        soc_sand_rv = soc_ppd_eg_ac_port_cvid_info_get(unit, &cep_key, &lif_id, &eg_ac_info, &found);                
    }
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    if (found == FALSE) {
        LOG_ERROR(BSL_LS_BCM_QOS,
                  (BSL_META_U(unit,
                              "%s: Lif AC not found for vid(%d) port(%d)\n"), FUNCTION_NAME(), vid, port));
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("AC LIF was not found for these vid and port")));
    }
    
    profile_ndx = eg_ac_info.edit_info.pcp_profile;
        
    if (bcm_dpp_am_qos_egr_pcp_vlan_is_alloced(unit, profile_ndx) == BCM_E_EXISTS) {
        BCM_INT_QOS_MAP_EGRESS_PCP_VLAN_SET(*egr_map, profile_ndx);
    } else {
        rv = BCM_E_NOT_FOUND;   
    }
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}


/* bcm_petra_qos_port_vlan_map_get */
int
bcm_petra_qos_port_vlan_map_get(int unit, bcm_port_t port, bcm_vlan_t vid,
                              int *ing_map, int *egr_map)
{
    int rv = BCM_E_NONE;
    
    BCMDNX_INIT_FUNC_DEFS;
    if (ing_map) {
    rv = _bcm_petra_qos_ingress_port_vlan_map_get(unit, port, vid, ing_map);
    }
    if (egr_map) {
    rv = _bcm_petra_qos_egress_port_vlan_map_get(unit, port, vid, egr_map);
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

/* bcm_petra_qos_multi_get */
int
bcm_petra_qos_multi_get(int unit, int array_size, int *map_ids_array, 
            int *flags_array, int *array_count)
{
    int rv = BCM_E_NONE;
    int idx = 0, count = 0, map_id = 0, map_flags = 0;
    int entry_count = 0;
#ifdef BCM_88660
    SOC_PPC_LIF_COS_PROFILE_INFO profile_info;
#endif /*BCM_88660*/

    BCMDNX_INIT_FUNC_DEFS;
    if (array_size == 0) {
        /* querying the number of map-ids for storage allocation */
        if (array_count == NULL) {
            rv = BCM_E_PARAM;
        }
        if (BCM_SUCCESS(rv)) {
            count = 0;
            *array_count = 0;
        for(idx=0; idx < SOC_DPP_CONFIG(unit)->qos.nof_ing_lif_cos; idx++) {
        if(bcm_dpp_am_qos_ing_lif_cos_is_alloced(unit,idx) == BCM_E_EXISTS) {
            count++;
        }
        }
        for(idx=0; idx < SOC_DPP_CONFIG(unit)->qos.nof_ing_pcp_vlan; idx++) {
        if(bcm_dpp_am_qos_ing_pcp_vlan_is_alloced(unit,idx) == BCM_E_EXISTS) {
            count++;
        }
        }
        for(idx=0; idx < SOC_DPP_CONFIG(unit)->qos.nof_ing_dp; idx++) {
        if(bcm_dpp_am_qos_ing_color_is_alloced(unit,idx) == BCM_E_EXISTS) {
            count++;
        }
        }
        for(idx=0; idx < SOC_DPP_CONFIG(unit)->qos.nof_ing_elsp; idx++) {
        if(bcm_dpp_am_qos_ing_elsp_is_alloced(unit,idx) == BCM_E_EXISTS) {
            count++;
        }
        }
        for(idx=0; idx < SOC_DPP_CONFIG(unit)->qos.nof_ing_cos_opcode; idx++) {
        if(bcm_dpp_am_qos_ing_cos_opcode_is_alloced(unit,idx) == BCM_E_EXISTS) {
            count++;
        }
        }
        for(idx=0; idx < SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id; idx++) {
        if(bcm_dpp_am_qos_egr_remark_id_is_alloced(unit,idx) == BCM_E_EXISTS) {
            count++;
        }
        }
        for(idx=0; idx < SOC_DPP_CONFIG(unit)->qos.nof_egr_mpls_php; idx++) {
        if(bcm_dpp_am_qos_egr_mpls_php_id_is_alloced(unit,idx) == BCM_E_EXISTS) {
            count++;
        }
        }
        for(idx=0; idx < SOC_DPP_CONFIG(unit)->qos.nof_egr_pcp_vlan; idx++) {
        if(bcm_dpp_am_qos_egr_pcp_vlan_is_alloced(unit,idx) == BCM_E_EXISTS) {
            count++;
        }
        }
        for(idx=0; idx < SOC_DPP_CONFIG(unit)->qos.nof_egr_l2_i_tag; idx++) {
        if(bcm_dpp_am_qos_egr_l2_i_tag_is_alloced(unit,idx) == BCM_E_EXISTS) {
            count++;
        }
        }
#ifdef BCM_88660
        if (SOC_IS_ARADPLUS(unit)) {
            for(idx=0; idx < SOC_DPP_CONFIG(unit)->qos.nof_egr_dscp_exp_marking; idx++) {
                if(bcm_dpp_am_qos_egr_dscp_exp_marking_is_alloced(unit,idx) == BCM_E_EXISTS) {
                    count++;
                }
            }
        }
#endif /* BCM_88660 */
        *array_count = count;
        }
    } else {
        if ((map_ids_array == NULL) || (flags_array == NULL) || 
            (array_count == NULL)) {
            rv = BCM_E_PARAM;
        }
        if (BCM_SUCCESS(rv)) {
            count = 0;
            for (idx = 0; ((idx < SOC_DPP_CONFIG(unit)->qos.nof_ing_lif_cos) && 
                         (count < array_size)); idx++) {
                if (bcm_dpp_am_qos_ing_lif_cos_is_alloced(unit, idx) == BCM_E_EXISTS) {
                    BCM_INT_QOS_MAP_LIF_COS_SET(map_id, idx);

                    map_flags = 0;
                    _DPP_QOS_ENTRY_USED_CNT(ing_lif_cos_mpls_bitmap, idx, _BCM_QOS_MAP_ING_MPLS_EXP_MAX, entry_count);
                    if (entry_count != 0) {
                        map_flags |= BCM_QOS_MAP_MPLS;
                    }
                    _DPP_QOS_ENTRY_USED_CNT(ing_lif_cos_ipv6_bitmap, idx, _BCM_QOS_MAP_ING_L3_DSCP_MAX, entry_count);
                    if (entry_count != 0) {
                        map_flags |= (BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV6);
                    }            
                    _DPP_QOS_ENTRY_USED_CNT(ing_lif_cos_ipv4_bitmap, idx, _BCM_QOS_MAP_ING_L3_DSCP_MAX, entry_count);          
                    if (entry_count != 0) {
                        map_flags |= (BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4);
                    }
                    _DPP_QOS_ENTRY_USED_CNT(ing_lif_cos_l2_inner_bitmap, idx, _BCM_QOS_MAP_ING_L2_PRI_CFI_MAX, entry_count);
                    if (entry_count != 0) {
                        map_flags |= (BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_INNER_TAG);
                    }
                    _DPP_QOS_ENTRY_USED_CNT(ing_lif_cos_l2_outer_bitmap, idx, _BCM_QOS_MAP_ING_L2_PRI_CFI_MAX, entry_count);
                    if (entry_count != 0) {
                        map_flags |= (BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_OUTER_TAG);
                    }
                    _DPP_QOS_ENTRY_USED_CNT(ing_lif_cos_l2_untag_bitmap, idx, _BCM_QOS_MAP_TC_DP_MAX, entry_count);
                    if (entry_count != 0) {
                        map_flags |= BCM_QOS_MAP_L2_UNTAGGED;
                    }

#ifdef BCM_88660
                    if (SOC_IS_ARADPLUS(unit)) {
                        SOC_SAND_IF_ERROR_RETURN(soc_ppd_lif_cos_profile_info_get(unit,
                            idx,
                            &profile_info));
                        if (profile_info.use_layer2_pcp == TRUE) {
                            map_flags |= BCM_QOS_MAP_L3_L2;
                        }
                    }
#endif

                    *(map_ids_array + count) = map_id;
                    *(flags_array + count) = (map_flags | BCM_QOS_MAP_INGRESS);
                    count++;
                }
            }
            for (idx = 0; ((idx < SOC_DPP_CONFIG(unit)->qos.nof_ing_pcp_vlan) && 
                         (count < array_size)); idx++) {
                if (bcm_dpp_am_qos_ing_pcp_vlan_is_alloced(unit, idx) == BCM_E_EXISTS) {
                    BCM_INT_QOS_MAP_INGRESS_PCP_VLAN_SET(map_id, idx);

                    map_flags = 0;
                    _DPP_QOS_ENTRY_USED_CNT(ing_pcp_vlan_ctag_bitmap, idx, _BCM_QOS_MAP_ING_VLAN_PCP_CTAG_MAX, entry_count);
                    if (entry_count != 0) {
                        map_flags |= (BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_VLAN_PCP | BCM_QOS_MAP_L2_INNER_TAG);
                    }
                    _DPP_QOS_ENTRY_USED_CNT(ing_pcp_vlan_utag_bitmap, idx, _BCM_QOS_MAP_ING_VLAN_PCP_UTAG_MAX, entry_count);
                    if (entry_count != 0) {
                        map_flags |= (BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_VLAN_PCP | BCM_QOS_MAP_L2_UNTAGGED);
                    }
                    _DPP_QOS_ENTRY_USED_CNT(ing_pcp_vlan_stag_bitmap, idx, _BCM_QOS_MAP_ING_VLAN_PCP_STAG_MAX, entry_count);
                    if (entry_count != 0) {
                        map_flags |= (BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_VLAN_PCP | BCM_QOS_MAP_L2_OUTER_TAG);
                    }

                    *(map_ids_array + count) = map_id;
                    *(flags_array + count) = (map_flags | BCM_QOS_MAP_INGRESS);
                    count++;
                }
            }
            for (idx = 0; ((idx < SOC_DPP_CONFIG(unit)->qos.nof_ing_dp) && 
                         (count < array_size)); idx++) {
                if (bcm_dpp_am_qos_ing_color_is_alloced(unit, idx) == BCM_E_EXISTS) {
                    BCM_INT_QOS_MAP_INGRESS_COLOR_SET(map_id, idx);

                    map_flags = BCM_QOS_MAP_POLICER;
       
                    *(map_ids_array + count) = map_id;
                    *(flags_array + count) = (map_flags | BCM_QOS_MAP_INGRESS);
                    count++;
                }
            }
            for (idx = 0; ((idx < SOC_DPP_CONFIG(unit)->qos.nof_ing_elsp) && 
                       (count < array_size)); idx++) {
                if (bcm_dpp_am_qos_ing_elsp_is_alloced(unit, idx) == BCM_E_EXISTS) {
                    BCM_INT_QOS_MAP_MPLS_ELSP_SET(map_id, idx);
                    map_flags = BCM_QOS_MAP_MPLS_ELSP;
                
                    *(map_ids_array + count) = map_id;
                    *(flags_array + count) = (map_flags | BCM_QOS_MAP_INGRESS);
                    count++;
                }
            }
            for (idx = 0; ((idx < SOC_DPP_CONFIG(unit)->qos.nof_ing_cos_opcode) && 
                       (count < array_size)); idx++) {
                if (bcm_dpp_am_qos_ing_cos_opcode_is_alloced(unit, idx) == BCM_E_EXISTS) {
                    BCM_INT_QOS_MAP_TYPE_OPCODE_SET(map_id, idx);
                    rv = QOS_ACCESS.ing_cos_opcode_flags.get(unit, idx, &map_flags);
                    BCMDNX_IF_ERR_EXIT(rv);
                
                    *(map_ids_array + count) = map_id;
                    *(flags_array + count) = (map_flags | BCM_QOS_MAP_INGRESS);
                    count++;
                }
            }
            for (idx = 0; ((idx < SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id) && 
                         (count < array_size)); idx++) {
                if (bcm_dpp_am_qos_egr_remark_id_is_alloced(unit, idx) == BCM_E_EXISTS) {
                    BCM_INT_QOS_MAP_REMARK_SET(map_id, idx);

                    map_flags = 0;
                    _DPP_QOS_ENTRY_USED_CNT(egr_remark_encap_mpls_bitmap, idx, _BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX, entry_count);
                    if (entry_count != 0) {
                        map_flags |= (BCM_QOS_MAP_ENCAP | BCM_QOS_MAP_MPLS);
                    }
                    _DPP_QOS_ENTRY_USED_CNT(egr_remark_encap_ipv6_bitmap, idx, _BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX, entry_count);
                    if (entry_count != 0) {
                        map_flags |= (BCM_QOS_MAP_ENCAP | BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV6);
                    }            
                    _DPP_QOS_ENTRY_USED_CNT(egr_remark_encap_ipv4_bitmap, idx, _BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX, entry_count);          
                    if (entry_count != 0) {
                        map_flags |= (BCM_QOS_MAP_ENCAP | BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4);
                    }
                    _DPP_QOS_ENTRY_USED_CNT(egr_remark_encap_l2_bitmap, idx, _BCM_QOS_MAP_EGR_REMARK_L2_MAX, entry_count);
                    if (entry_count != 0) {
                        map_flags |= (BCM_QOS_MAP_ENCAP | BCM_QOS_MAP_L2);
                    }
                    _DPP_QOS_ENTRY_USED_CNT(egr_remark_mpls_bitmap, idx, _BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX*4, entry_count);
                    if (entry_count != 0) {
                        map_flags |= BCM_QOS_MAP_MPLS;
                    }
                    _DPP_QOS_ENTRY_USED_CNT(egr_remark_ipv6_bitmap, idx, _BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX*4, entry_count);
                    if (entry_count != 0) {
                        map_flags |= (BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV6);
                    }
                    _DPP_QOS_ENTRY_USED_CNT(egr_remark_ipv4_bitmap, idx, _BCM_QOS_MAP_EGR_REMARK_L3_DSCP_MAX*4, entry_count);
                    if (entry_count != 0) {
                        map_flags |= (BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4);
                    }

                    *(map_ids_array + count) = map_id;
                    *(flags_array + count) = (map_flags | BCM_QOS_MAP_EGRESS);
                    count++;
                }
            }
            for (idx = 0; ((idx < SOC_DPP_CONFIG(unit)->qos.nof_egr_mpls_php) && 
                         (count < array_size)); idx++) {
                if (bcm_dpp_am_qos_egr_mpls_php_id_is_alloced(unit, idx) == BCM_E_EXISTS) {
                    BCM_INT_QOS_MAP_EGRESS_MPLS_PHP_SET(map_id, idx);

                    map_flags = 0;
                    _DPP_QOS_ENTRY_USED_CNT(egr_mpls_php_ipv6_bitmap, idx, _BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX*4, entry_count);
                    if (entry_count != 0) {
                        map_flags |= (BCM_QOS_MAP_MPLS_PHP | BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV6);
                    }
                    _DPP_QOS_ENTRY_USED_CNT(egr_mpls_php_ipv4_bitmap, idx, _BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX*4, entry_count);
                    if (entry_count != 0) {
                        map_flags |= (BCM_QOS_MAP_MPLS_PHP | BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4);
                    }

                    *(map_ids_array + count) = map_id;
                    *(flags_array + count) = (map_flags | BCM_QOS_MAP_EGRESS);
                    count++;
                }
            }
            for (idx = 0; ((idx < SOC_DPP_CONFIG(unit)->qos.nof_egr_pcp_vlan) && 
                         (count < array_size)); idx++) {
                if (bcm_dpp_am_qos_egr_pcp_vlan_is_alloced(unit, idx) == BCM_E_EXISTS) {
                    BCM_INT_QOS_MAP_EGRESS_PCP_VLAN_SET(map_id, idx);

                    map_flags = 0;
                    _DPP_QOS_ENTRY_USED_CNT(egr_pcp_vlan_ctag_bitmap, idx, _BCM_QOS_MAP_EGR_VLAN_PRI_MAX, entry_count);
                    if (entry_count != 0) {
                        map_flags |= (BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_INNER_TAG | BCM_QOS_MAP_L2_VLAN_PCP);
                    }
                    _DPP_QOS_ENTRY_USED_CNT(egr_pcp_vlan_utag_bitmap, idx, _BCM_QOS_MAP_EGR_VLAN_PRI_MAX*4, entry_count);
                    if (entry_count != 0) {
                        map_flags |= (BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_UNTAGGED | BCM_QOS_MAP_L2_VLAN_PCP);
                    }
                    _DPP_QOS_ENTRY_USED_CNT(egr_pcp_vlan_stag_bitmap, idx, _BCM_QOS_MAP_EGR_VLAN_PRI_MAX*4, entry_count);
                    if (entry_count != 0) {
                        map_flags |= (BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_OUTER_TAG | BCM_QOS_MAP_L2_VLAN_PCP);
                    }
                    _DPP_QOS_ENTRY_USED_CNT(egr_pcp_vlan_mpls_bitmap, idx, _BCM_QOS_MAP_EGR_EXP_MAX, entry_count);
                    if (entry_count != 0) {
                        map_flags |= (BCM_QOS_MAP_MPLS | BCM_QOS_MAP_L2_VLAN_PCP);
                    }
                    _DPP_QOS_ENTRY_USED_CNT(egr_pcp_vlan_ipv6_bitmap, idx, _BCM_QOS_MAP_EGR_DSCP_MAX, entry_count);
                    if (entry_count != 0) {
                        map_flags |= (BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV6 | BCM_QOS_MAP_L2_VLAN_PCP);
                    }
                    _DPP_QOS_ENTRY_USED_CNT(egr_pcp_vlan_ipv4_bitmap, idx, _BCM_QOS_MAP_EGR_DSCP_MAX, entry_count);
                    if (entry_count != 0) {
                        map_flags |= (BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_L2_VLAN_PCP);
                    }

                    *(map_ids_array + count) = map_id;
                    /*For Egress pcp vlan flag BCM_QOS_MAP_L2_VLAN_PCP must be set"*/
                    *(flags_array + count) = (map_flags | BCM_QOS_MAP_EGRESS);
                    count++;
                }
            }
            for (idx = 0; ((idx < SOC_DPP_CONFIG(unit)->qos.nof_egr_l2_i_tag) && 
                         (count < array_size)); idx++) {
                if (bcm_dpp_am_qos_egr_l2_i_tag_is_alloced(unit, idx) == BCM_E_EXISTS) {
                    BCM_INT_QOS_MAP_EGRESS_L2_I_TAG_SET(map_id, idx);

                    map_flags = 0;
                    _DPP_QOS_ENTRY_USED_CNT(egr_mpls_php_ipv4_bitmap, idx, _BCM_QOS_MAP_EGR_REMARK_MPLS_EXP_MAX*4, entry_count);
                    if (entry_count != 0) {
                        map_flags |= BCM_QOS_MAP_MIM_ITAG;
                    }

                    *(map_ids_array + count) = map_id;
                    *(flags_array + count) = (map_flags | BCM_QOS_MAP_EGRESS);
                    count++;
                }
            }
            
#ifdef BCM_88660
            if (SOC_IS_ARADPLUS(unit)) {
                for (idx = 0; ((idx < SOC_DPP_CONFIG(unit)->qos.nof_egr_dscp_exp_marking) && 
                             (count < array_size)); idx++) {
                    if (bcm_dpp_am_qos_egr_dscp_exp_marking_is_alloced(unit, idx) == BCM_E_EXISTS) {
                        BCM_INT_QOS_MAP_EGRESS_DSCP_EXP_MARKING_SET(map_id, idx);

                        map_flags = 0;
                        *(map_ids_array + count) = map_id;
                        *(flags_array + count) = (map_flags | BCM_QOS_MAP_EGRESS);
                        count++;
                    }
                }
            }
#endif /* BCM_88660 */
            *array_count = count;
        }
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


/**get qos map id by profile and flags */
int bcm_petra_qos_map_id_get_by_profile(int unit, uint32 flags, int profile, int *map_id)
{
    int rv = BCM_E_NONE;
   
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);

    if (flags & BCM_QOS_MAP_INGRESS) { 
        if (flags & BCM_QOS_MAP_L2_VLAN_PCP) {
            BCM_INT_QOS_MAP_INGRESS_PCP_VLAN_SET(*map_id, profile);
        } 
        else if (flags & BCM_QOS_MAP_MPLS_ELSP) {
            BCM_INT_QOS_MAP_MPLS_ELSP_SET(*map_id, profile);
        } 
        else if (flags & BCM_QOS_MAP_OPCODE) {
            BCM_INT_QOS_MAP_TYPE_OPCODE_SET(*map_id, profile);
        } 
        else if (flags & BCM_QOS_MAP_POLICER) {/*ingress dp profile*/
            if (SOC_IS_JERICHO_PLUS(unit)) 
            {
                BCM_INT_QOS_MAP_INGRESS_COLOR_SET(*map_id, profile);  
            } 
        }
        else { /* L3 */
            BCM_INT_QOS_MAP_LIF_COS_SET(*map_id, profile);
        }
    } 
    else if (flags & BCM_QOS_MAP_EGRESS) {
         if (flags & BCM_QOS_MAP_L2_VLAN_PCP) {
            BCM_INT_QOS_MAP_EGRESS_PCP_VLAN_SET(*map_id, profile);  
         } 
         else if (flags & BCM_QOS_MAP_MIM_ITAG) {
             BCM_INT_QOS_MAP_EGRESS_L2_I_TAG_SET(*map_id, profile);  
         } 
         else if (flags & BCM_QOS_MAP_MPLS_PHP) {
             BCM_INT_QOS_MAP_EGRESS_MPLS_PHP_SET(*map_id, profile);  
         }
#ifdef BCM_88660
         else if ((flags & BCM_QOS_MAP_L3_L2) && SOC_IS_ARADPLUS(unit)) {
             BCM_INT_QOS_MAP_EGRESS_DSCP_EXP_MARKING_SET(*map_id, profile);  

         }
#endif
         else if ((flags & BCM_QOS_MAP_OAM_PCP)) {
             if (SOC_IS_JERICHO(unit)) {
                 BCM_INT_QOS_MAP_OAM_EGRESS_PROFILE_SET(*map_id, profile);
             }
         } 
         else if (flags & BCM_QOS_MAP_ENCAP_INTPRI_COLOR){
             if (SOC_IS_JERICHO(unit)) 
             {
                 BCM_INT_QOS_MAP_ENCAP_INTPRI_COLOR_SET(*map_id, profile);  
             }
         }
         else {
             BCM_INT_QOS_MAP_REMARK_SET(*map_id, profile);           
         }
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


