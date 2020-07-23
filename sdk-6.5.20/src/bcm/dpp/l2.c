/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Soc_petra-B Layer 2 Management
 */

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_L2

#include <shared/bsl.h>

#include "bcm_int/common/debug.h"

#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/vswitch.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/alloc_mngr_local_lif.h>
#include <bcm_int/dpp/alloc_mngr_glif.h>
#include <bcm_int/dpp/trill.h>
#include <bcm_int/dpp/rx.h>
#include <bcm_int/dpp/l2.h>
#include <bcm_int/dpp/l3.h>
#include <bcm_int/dpp/oam.h>
#include <bcm_int/dpp/pon.h>

#include <bcm_int/control.h>
#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm/l2.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/petra_dispatch.h>
#include <bcm_int/dpp/mim.h>
#include <bcm_int/dpp/switch.h>

#include <soc/dpp/mbcm.h>

#include <soc/dpp/PPD/ppd_api_llp_parse.h>
#include <soc/dpp/PPD/ppd_api_lif.h>
#include <soc/dpp/PPD/ppd_api_vsi.h>
#include <soc/dpp/PPD/ppd_api_eg_vlan_edit.h>
#include <soc/dpp/PPD/ppd_api_frwrd_mact_mgmt.h>
#include <soc/dpp/PPD/ppd_api_frwrd_mact.h>
#include <soc/dpp/PPD/ppd_api_frwrd_bmact.h>
#include <soc/dpp/PPD/ppd_api_llp_trap.h>
#include <soc/dpp/PPD/ppd_api_rif.h>
#include <soc/dpp/PPD/ppd_api_mymac.h>
#include <soc/dpp/PPD/ppd_api_llp_sa_auth.h>
#include <soc/dpp/PPD/ppd_api_lif_table.h>

#include <soc/dcmn/dcmn_wb.h>
#include <soc/dpp/mbcm_pp.h>
#include <shared/swstate/access/sw_state_access.h>
#include <shared/swstate/sw_state_sync_db.h>
#include <shared/swstate/sw_state_cb_db.h>


#ifdef BCM_88660_A0
#include <soc/dpp/PPD/ppd_api_slb.h>
#endif /* BCM_88660_A0 */

#include <soc/dpp/SAND/Utils/sand_framework.h>

#include <soc/dpp/TMC/tmc_api_header_parsing_utils.h>
#include <soc/dpp/TMC/tmc_api_ports.h>

#include <soc/dpp/pkt.h>

#include <soc/dpp/ARAD/arad_header_parsing_utils.h>
#include <soc/dpp/ARAD/arad_interrupts.h>

#ifdef BCM_CMICM_SUPPORT
#include <soc/uc_msg.h>
#include <soc/shared/llm_msg.h>
#include <soc/shared/llm_pack.h>
#include <soc/shared/llm_appl.h>
#include <soc/uc.h>
#include <soc/cm.h>
#endif

#include <soc/dpp/JER/JER_PP/jer_pp_eg_encap_access.h>
#include <soc/dpp/JER/JER_PP/jer_pp_eg_encap.h>


/* numnber of port profile for reserved- MC */
#define _BCM_PETRA_L2_CACHE_RM_PORT_PROFILES (4)

/* defines */
#define _BCM_PETRA_L2_FLAGS_SUPPORTED_ARAD                              \
    (BCM_L2_DISCARD_SRC             | BCM_L2_DISCARD_DST                | \
     BCM_L2_STATIC                  | BCM_L2_TRUNK_MEMBER               | \
     BCM_L2_MCAST                   | BCM_L2_ENTRY_OVERFLOW             | \
     BCM_L2_HIT                     | BCM_L2_SET_ENCAP_VALID            | \
     BCM_L2_SET_ENCAP_INVALID)


/* Supported flags for L2 */
#define _BCM_PETRA_L2_FLAGS_SUPPORTED(unit)                             \
    (_BCM_PETRA_L2_FLAGS_SUPPORTED_ARAD)


#define _BCM_PETRA_L2_LIMIT_FLAGS_SUPPORTED_PB                  \
    (BCM_L2_LEARN_LIMIT_VLAN    | BCM_L2_LEARN_LIMIT_TUNNEL)

#define _BCM_PETRA_L2_LIMIT_FLAGS_SUPPORTED_ARAD                    \
    (BCM_L2_LEARN_LIMIT_VLAN    | BCM_L2_LEARN_LIMIT_TUNNEL     |   \
     BCM_L2_LEARN_LIMIT_SYSTEM)

#define _BCM_PETRA_L2_LIMIT_FLAGS_SUPPORTED_ARAD_PLUS               \
    (BCM_L2_LEARN_LIMIT_VLAN    | BCM_L2_LEARN_LIMIT_TUNNEL     |   \
     BCM_L2_LEARN_LIMIT_SYSTEM  | BCM_L2_LEARN_LIMIT_PORT)

/* unsuported flags for L2-limit*/
#define _BCM_PETRA_L2_LIMIT_FLAGS_SUPPORTED(unit)                       \
    (SOC_IS_ARADPLUS(unit) ? _BCM_PETRA_L2_LIMIT_FLAGS_SUPPORTED_ARAD_PLUS : _BCM_PETRA_L2_LIMIT_FLAGS_SUPPORTED_ARAD)

/* unsupported flag for L2_egress */
#define _BCM_L2_EGRESS_UNSUPPORTED_FLAGS (BCM_L2_EGRESS_DEST_MAC_PREFIX5_REPLACE | BCM_L2_EGRESS_WIDE)


/*
 * when traversing the MACT, to perform action on each entry how many 
 * entries to return in each iteration 
 */
#define _BCM_PETRA_L2_TRVRS_ITER_BLK_SIZE (130)

/* Clear bit 15 to get mac limit number */
#define _BCM_PETRA_L2_MAC_LIMIT_GET(limit) ((limit) & (~SOC_PPC_FRWRD_MACT_LEARN_VMAC_LIMIT))
/* 1 -- vmac, 0--mac limit per tunnel */

#define  BCM_L2CP_PROFILE_LEN_IN_BYTES 8
#define  BCM_L2CP_MAX_PROFILE_IDX 3


#define _BCM_DPP_MYMAC_VSI_LSB_MAX0  ((1<<8)-1)
#define _BCM_DPP_MYMAC_VSI_LSB_MAX1  ((1<<4)-1)

/* static variables */
SOC_PPC_FRWRD_MACT_ENTRY_KEY                
*_bcm_l2_traverse_mact_keys[BCM_LOCAL_UNITS_MAX] = {NULL};
SOC_PPC_FRWRD_MACT_ENTRY_VALUE              
*_bcm_l2_traverse_mact_vals[BCM_LOCAL_UNITS_MAX] = {NULL};
SOC_SAND_PP_MAC_ADDRESS
*_bcm_l2_auth_traverse_mac_addrs[BCM_LOCAL_UNITS_MAX] = {NULL};
SOC_PPC_LLP_SA_AUTH_MAC_INFO              
*_bcm_l2_auth_traverse_mac_infos[BCM_LOCAL_UNITS_MAX] = {NULL};


/****************L2 Cache*************/
/*Enum of index type which used in bcm_l2_cache_set/get*/
typedef enum _bcm_l2_cache_index_type_s
{
    _BCM_L2_CACHE_INDEX_TYPE_INVALID,           /*Invalid*/
    _BCM_L2_CACHE_INDEX_TYPE_VSI_RESERVED_MC = 1,  /* For L2CP traps which is same as MC reserved just configured per VSI*/
    _BCM_L2_CACHE_INDEX_TYPE_RESERVED_MC = 2,      /*Virtual trap reserved Multi-cast IHP_RESERVED_MC*/
    _BCM_L2_CACHE_INDEX_TYPE_PROG = 3,              /*Virtual Traps programable IHP_GENERAL_TRAP*/
    _BCM_L2_CACHE_INDEX_TYPE_MAX
}_bcm_l2_cache_index_type_e;

/*3 bit for types see _bcm_l2_cache_index_type_e*/
#define _BCM_L2_CACHE_INDEX_TYPE_OFFSET   (0) 
#define _BCM_L2_CACHE_INDEX_TYPE_MASK     (0x7) 
#define _BCM_L2_CACHE_INDEX_TYPE_NOF_BITS (3)

#define _BCM_L2_INVALID_TPID                        0xFFFF   
#define _BCM_L2_INVALID_ETH_TYPE               0xFFFF   
#define _BCM_L2_IS_VALID_FIELD(a)           (a != _BCM_L2_INVALID_ETH_TYPE)
#define ALLOC_MNGR_LOCAL_LIF_ACCESS                     sw_state_access[unit].dpp.bcm.alloc_mngr_local_lif


/*8 bits for MAC LSB or Programable/General Traps*/
#define _BCM_L2_CACHE_INDEX_MAC_OR_PROG_PROFILE_OFFSET      (_BCM_L2_CACHE_INDEX_TYPE_NOF_BITS)
#define _BCM_L2_CACHE_INDEX_MAC_OR_PROG_PROFILE_MASK        (0xFF)
#define _BCM_L2_CACHE_INDEX_MAC_OR_PROG_PROFILE_NOF_BITS    (8)

/*19 bit for port or VSI*/
#define _BCM_L2_CACHE_INDEX_PORT_OR_VSI_OFFSET      ((_BCM_L2_CACHE_INDEX_MAC_OR_PROG_PROFILE_OFFSET) + (_BCM_L2_CACHE_INDEX_MAC_OR_PROG_PROFILE_NOF_BITS))
#define _BCM_L2_CACHE_INDEX_PORT_OR_VSI_MASK        (0x7FFFF)
#define _BCM_L2_CACHE_INDEX_PORT_OR_VSI_NOF_BITS    (19)

/*1 bits if TRAP
    bcmRxTrapL2Cache can be created by 2 ways
    1)  bcm_rx_trap_create/set in this case bit will be set to 1
    2) bcm_l2_cache_set in this case bit will be set to 0
    Note bcm_l2_cache_set will be called in 1st way also but just to configure the condition of the trap not the action*/
#define _BCM_L2_CACHE_INDEX_TRAP_CREATED_OFFSET     ((_BCM_L2_CACHE_INDEX_PORT_OR_VSI_OFFSET) + (_BCM_L2_CACHE_INDEX_PORT_OR_VSI_NOF_BITS))
#define _BCM_L2_CACHE_INDEX_TRAP_CREATED_MASK       (0x1)
#define _BCM_L2_CACHE_INDEX_TRAP_CREATED_NOF_BITS   (1)


#define _BCM_L2_CACHE_INDEX_MAX_VALUE (0x7FFFFFFF)
#define _BCM_L2_CACHE_INDEX_INVALID   (0) 


/*The following Will hold the encoding of MC RESERVED port profile
  The data for each profile is trap_porfile * LSB MAC address (the last 6 bits of MAC address = 64 options)
  For MAC address N the bits [N,N+8] will represent that MAC action profile
  The bits [N,N+8] are encoded as follows: Trap_index 3 bit | trap_strength 3 bit | snoop Strength 2 bit
  Each prot profile data is bit stream of 64*8  = MAC options * Action Profile */
#define _BCM_L2_MC_RESERVED_TRAP_CODE_OFFSET         (0)
#define _BCM_L2_MC_RESERVED_TRAP_CODE_MASK           (0x7)
#define _BCM_L2_MC_RESERVED_TRAP_CODE_NOF_BITS       (3)  /* SOC_PPC_TRAP_CODE_RESERVED_MC_0 to 7*/

#define _BCM_L2_MC_RESERVED_TRAP_STRENGTH_OFFSET    (_BCM_L2_MC_RESERVED_TRAP_CODE_OFFSET + _BCM_L2_MC_RESERVED_TRAP_CODE_NOF_BITS)
#define _BCM_L2_MC_RESERVED_TRAP_STRENGTH_MASK      (0x7)
#define _BCM_L2_MC_RESERVED_TRAP_STRENGTH_NOF_BITS  (3)  /* Trap strength 0-7*/

#define _BCM_L2_MC_RESERVED_SNOOP_CODE_OFFSET       (_BCM_L2_MC_RESERVED_TRAP_STRENGTH_OFFSET + _BCM_L2_MC_RESERVED_TRAP_STRENGTH_NOF_BITS)
#define _BCM_L2_MC_RESERVED_SNOOP_CODE_MASK         (0x3)
#define _BCM_L2_MC_RESERVED_SNOOP_CODE_NOF_BITS     (2)  /* Snoop Strength 0-3*/

#define _BCM_L2_MC_RESERVED_NOF_MAC_COMBINATIONS    (64) /*2^6 = 64*/




/* MACROs to check if module inited*/
#define _DPP_L2_INIT_SET(unit, val)             \
    BCMDNX_IF_ERR_EXIT(L2_ACCESS.l2_initialized.set(unit, val));

#define _DPP_L2_INIT_CHECK(unit)                                                             \
    {                                                                                        \
        int _l2_initialized;                                                                 \
        uint8 _is_allocated;                                                                 \
        BCMDNX_IF_ERR_EXIT(L2_ACCESS.is_allocated(unit, &_is_allocated));                    \
        if(!_is_allocated) {                                                                 \
            BCMDNX_ERR_EXIT_MSG(BCM_E_INIT, (_BSL_BCM_MSG("L2 module is not initialized"))); \
        }                                                                                    \
        BCMDNX_IF_ERR_EXIT(L2_ACCESS.l2_initialized.get(unit, &_l2_initialized));            \
        if (_l2_initialized == FALSE) {                                                      \
            BCMDNX_ERR_EXIT_MSG(BCM_E_INIT, (_BSL_BCM_MSG("L2 module is not initialized"))); \
        }                                                                                    \
    }


/* Execute registered callback routines for given unit */
#define _BCM_PETRA_L2_CB_RUN(unit, l2addr, insert)                      \
    do {                                                                \
        int _i;                                                         \
        for (_i = 0; _i < _BCM_PETRA_L2_CB_MAX; _i++) {                 \
            if (_BCM_PETRA_L2_CB_ENTRY(unit, _i).fn) {                  \
                _BCM_PETRA_L2_CB_ENTRY(unit, _i).fn((unit), (l2addr), (insert), \
                                                    _BCM_PETRA_L2_CB_ENTRY((unit), _i).fn_data); \
            }                                                           \
        }                                                               \
    } while (0)

/* SW DB for mac-limit*/

/*  profile used for unlimited-learning*/


#define _BCM_PETRA_L2_IS_RM_MAC(_mac_)  (((_mac_)[0] == 0x01) && ((_mac_)[1] == 0x80) && \
                                         ((_mac_)[2] == 0xc2) && ((_mac_)[3] == 0x0)   && \
                                         ((_mac_)[4] == 0x0)  && ((_mac_)[5] <= 0x3f))

#define _BCM_PETRA_L2_RM_MAC_FILL(_mac_) (_mac_)[0] = 0x01; (_mac_)[1] = 0x80; \
                                                (_mac_)[2] = 0xc2; (_mac_)[3] = 0x0; \
                                                (_mac_)[4] = 0x0; (_mac_)[5] = 0;

#define _BCM_PETRA_L2_IS_MAC_ALL_ONES(_mac_)            \
    (((_mac_)[0] & (_mac_)[1] & (_mac_)[2] &            \
      (_mac_)[3] & (_mac_)[4] & (_mac_)[5]) == 0xff) 

#define _BCM_PETRA_L2_IS_MAC_EQUAL(m1,m2)       \
    ((((m1)[0] == (m2)[0]) &&                   \
      ((m1)[1] == (m2)[1]) &&                   \
      ((m1)[2] == (m2)[2]) &&                   \
      ((m1)[3] == (m2)[3]) &&                   \
      ((m1)[4] == (m2)[4]) &&                   \
      ((m1)[5] == (m2)[5])) ? 1:0)


/* number of reserved MAC addresses */
#define _BCM_PETRA_L2_NOF_RM_MAC (64)

#define _BCM_PETRA_L2_NOF_PROG_TRAPS (4)

#define _BCM_PETRA_L2_EVENT_DIST_IS_DEFAULT(vid) ((vid) == ((bcm_vlan_t) - 1))

#ifdef BCM_CMICM_SUPPORT
/* Disable MAC limit per tunnel function */
#define LLM_PER_TUNNEL_DISABLE(unit) \
    if (SOC_MAC_LIMIT_PER_TUNNEL_ENABLE_GET(unit)) { \
        LLM_TUNNEL_LOCK(unit); \
        rv = shr_llm_msg_mac_limit_enable(unit, FALSE, FALSE); \
        restore_arm = TRUE; \
        BCMDNX_IF_ERR_EXIT(rv); \
    } 
    
/* Disable MAC limit per tunnel function and enable the reply fifo*/
#define LLM_PER_TUNNEL_ENABLE_REPLY(unit) \
    if (SOC_MAC_LIMIT_PER_TUNNEL_ENABLE_GET(unit)) { \
        if (hw_acc) { \
            reply_fifo_enable = TRUE; \
        } \
        LLM_TUNNEL_LOCK(unit); \
        rv = shr_llm_msg_mac_limit_enable(unit, FALSE, reply_fifo_enable); \
        restore_arm = TRUE; \
        BCMDNX_IF_ERR_EXIT(rv); \
    } 

/* Enable MAC limit per tunnel function */
#define LLM_PER_TUNNEL_ENABLE(unit) \
    if (SOC_MAC_LIMIT_PER_TUNNEL_ENABLE_GET(unit) && (restore_arm == TRUE)) { \
        restore_arm = FALSE; \
        rv = shr_llm_msg_mac_limit_enable(unit, TRUE, FALSE); \
        LLM_TUNNEL_UNLOCK(unit); \
        BCMDNX_IF_ERR_EXIT(rv); \
    }

#define _L2_LEARN_LIMIT_PER_TUNNEL_IS_ENABLED(unit, limit) \
    (SOC_MAC_LIMIT_PER_TUNNEL_ENABLE_GET((unit)) && \
     ((limit)->flags & BCM_L2_LEARN_LIMIT_TUNNEL))

/* Disable MAC limit per tunnel function */
#define LLM_PER_TUNNEL_DISABLE_NO_MOS(unit) \
    if (SOC_MAC_LIMIT_PER_TUNNEL_ENABLE_GET(unit)) { \
        LLM_TUNNEL_LOCK(unit); \
    } 

/* Enable MAC limit per tunnel function */
#define LLM_PER_TUNNEL_ENABLE_NO_MOS(unit) \
    if (SOC_MAC_LIMIT_PER_TUNNEL_ENABLE_GET(unit)) { \
        LLM_TUNNEL_UNLOCK(unit); \
    }
#else
#define LLM_PER_TUNNEL_DISABLE(unit)
#define LLM_PER_TUNNEL_ENABLE_REPLY(unit)
#define LLM_PER_TUNNEL_ENABLE(unit)
#define _L2_LEARN_LIMIT_PER_TUNNEL_IS_ENABLED(unit, limit)
#define LLM_PER_TUNNEL_DISABLE_NO_MOS(unit)
#define LLM_PER_TUNNEL_ENABLE_NO_MOS(unit)
#endif

#define L2_ACCESS sw_state_access[unit].dpp.bcm.l2
/* Set the frozen value from Warmboot value */
int _bcm_petra_l2_freeze_set_no_wb(int unit, int frozen)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(L2_ACCESS.bcm_petra_l2_freeze_state.frozen.set(unit, frozen));

    goto exit;
exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_petra_l2_freeze_inc(int unit, int *frozen)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(frozen);
    BCMDNX_IF_ERR_EXIT(L2_ACCESS.bcm_petra_l2_freeze_state.frozen.get(unit, frozen));
    (*frozen)++;
    BCMDNX_IF_ERR_EXIT(L2_ACCESS.bcm_petra_l2_freeze_state.frozen.set(unit, *frozen));

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_petra_l2_freeze_dec(int unit, int *frozen)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(frozen);
    /* if no valid freeze then return indication for error*/
    BCMDNX_IF_ERR_EXIT(L2_ACCESS.bcm_petra_l2_freeze_state.frozen.get(unit, frozen));
    if (*frozen == 0) {
        return -1;
    }
    (*frozen)--;
    BCMDNX_IF_ERR_EXIT(L2_ACCESS.bcm_petra_l2_freeze_state.frozen.set(unit, *frozen));
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_petra_l2_is_freezed(int unit, uint8 *is_freezed)
{
    int frozen;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(L2_ACCESS.bcm_petra_l2_freeze_state.frozen.get(unit, &frozen));
    
    *is_freezed = (frozen > 0);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_petra_l2_freeze_info_set(int unit, _bcm_petra_l2_freeze_t* freeze_info)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(freeze_info);
    BCMDNX_IF_ERR_EXIT(L2_ACCESS.bcm_petra_l2_freeze_state.save_age_ena.set(unit, freeze_info->save_age_ena));
    BCMDNX_IF_ERR_EXIT(L2_ACCESS.bcm_petra_l2_freeze_state.save_learn_ena.set(unit, freeze_info->save_learn_ena));
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_petra_l2_freeze_info_get(int unit, _bcm_petra_l2_freeze_t* freeze_info)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(L2_ACCESS.bcm_petra_l2_freeze_state.get(unit, freeze_info));
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * SW DB}
 */

int _bcm_dpp_out_lif_overlay_ll_match_add(int unit, bcm_l2_egress_t *egr, int lif)
{
    _bcm_dpp_gport_sw_resources gport_sw_resources;
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_local_lif_to_sw_resources(unit, -1, lif, _BCM_DPP_GPORT_SW_RESOURCES_EGRESS, &gport_sw_resources);
    BCMDNX_IF_ERR_EXIT(rv);

    gport_sw_resources.out_lif_sw_resources.lif_type = _bcmDppLifTypeOverlayLinkLayer;
    gport_sw_resources.out_lif_sw_resources.flags |= egr->mpls_extended_label_value  ? _BCM_DPP_OUTLIF_MATCH_INFO_MPLS_EXTENDED_LABEL : 0;
    gport_sw_resources.out_lif_sw_resources.flags |= egr->outer_tpid? _BCM_DPP_OUTLIF_MATCH_INFO_MPLS_EXTENDED_LABEL_TAGGED : 0;

    rv = _bcm_dpp_local_lif_sw_resources_set(unit, -1, lif, _BCM_DPP_GPORT_SW_RESOURCES_EGRESS, &gport_sw_resources);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


/* In L2_EGRESS_CREATE, outer_vlan field is encoded as follow: {PCP DEI [15:12] VID[11:0]} */
#define L2_EGRESS_CREATE_PCP_DEI_FROM_VLAN(pcp_dei, vlan) \
         pcp_dei = (VLAN_CTRL_PRIO(vlan) << 1) | VLAN_CTRL_CFI(vlan)

/* outer_vlan/inner_vlan field is encoded as follow: {PCP DEI [15:12] VID[11:0]} */
#define L2_EGRESS_GET_VLAN_FROM_VID_PCP_DEI(vlan , vid, pcp_dei) \
        vlan = VLAN_CTRL((pcp_dei >> 1), (pcp_dei & 0x1), vid)

/* pcp dei profile*/
#define L2_EGRESS_PCP_DEI_PROFILE_MAX  0x3
#define L2_EGRESS_PCP_DEI_PROFILE_SET(pcp_dei_profile, qos_map_id) \
        (pcp_dei_profile) = (qos_map_id & 0x7)

/* MPLS extended label macros */

#define L2_EGRESS_CREATE_OUTER_VID_FROM_MPLS_EXTENDED_LABEL(out_vid, mpls_extended_label_value) \
        (out_vid) = (VLAN_CTRL_ID(mpls_extended_label_value))

#define L2_EGRESS_CREATE_OUTER_PCP_DEI_FROM_MPLS_EXTENDED_LABEL(pcp_dei,mpls_extended_label_value) \
        (pcp_dei) = (((VLAN_CTRL_PRIO(mpls_extended_label_value)) << 1) | (VLAN_CTRL_CFI(mpls_extended_label_value)))

#define L2_EGRESS_CREATE_ETHER_TYPE_INDEX_NDX_FROM_MPLS_EXTENDED_LABEL(ehter_type_index_ndx,mpls_extended_label_value) \
        (ehter_type_index_ndx) = ((0xf0000 & (mpls_extended_label_value)) >> 16)

#define L2_EGRESS_CREATE_INNER_TPID_FROM_MPLS_EXTENDED_LABEL(inner_tpid,mpls_extended_label_value,outer_tpid) \
        (inner_tpid) = outer_tpid?(((0xf0000 & (mpls_extended_label_value)) >> 16) | 0x80):(((0xf0000 & (mpls_extended_label_value)) >> 16) | 0x40)


#define L2_EGRESS_GET_MPLS_EXTENDED_LABEL_FROM_OUTER_VID(mpls_extended_label_value, out_vid) \
        (mpls_extended_label_value) = ((out_vid & 0xfff) | ((mpls_extended_label_value) & 0xff000))

#define L2_EGRESS_GET_MPLS_EXTENDED_LABEL_FROM_OUTER_PCP_DEI(mpls_extended_label_value, pcp_dei) \
        (mpls_extended_label_value) = (((pcp_dei & 0xf) << 12) | ((mpls_extended_label_value) & 0xf0fff))

#define L2_EGRESS_GET_MPLS_EXTENDED_LABEL_FROM_OUTER_TPID(mpls_extended_label_value, outer_tpid) \
        (mpls_extended_label_value) = (((outer_tpid & 0xf) << 16) | ((mpls_extended_label_value) & 0x0ffff))

/*internal functions */

int
_bcm_petra_l2_mymac_msb_is_set_set(int unit, uint8 val)
{
    BCMDNX_INIT_FUNC_DEFS;

    if (unit < BCM_LOCAL_UNITS_MAX) {
        BCMDNX_IF_ERR_EXIT(L2_ACCESS.mymac_msb_is_set.set(unit, val));
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_petra_l2_mymac_msb_is_set_get(int unit, uint8 *is_set)
{
    BCMDNX_INIT_FUNC_DEFS;

    if (unit < BCM_LOCAL_UNITS_MAX) {
        BCMDNX_IF_ERR_EXIT(L2_ACCESS.mymac_msb_is_set.get(unit, is_set));
    }
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_petra_l2_mpls_extended_label_ether_type_index_table_is_set_set(int unit, uint16 val)
{
    BCMDNX_INIT_FUNC_DEFS;

    if (unit < BCM_LOCAL_UNITS_MAX) {
        BCMDNX_IF_ERR_EXIT(L2_ACCESS.mpls_extended_label_ether_type_index_table_is_set.set(unit, val));
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_petra_l2_mpls_extended_label_ether_type_index_table_is_set_get(int unit, uint16 *is_set)
{
    BCMDNX_INIT_FUNC_DEFS;

    if (unit < BCM_LOCAL_UNITS_MAX) {
        BCMDNX_IF_ERR_EXIT(L2_ACCESS.mpls_extended_label_ether_type_index_table_is_set.get(unit, is_set));
    }
exit:
    BCMDNX_FUNC_RETURN;
}


void
_bcm_petra_l2_entry_print(bcm_l2_addr_t *l2e)
{
    LOG_CLI((BSL_META("%02x:%02x:%02x:%02x:%02x:%02x |"),
l2e->mac[0],l2e->mac[1],l2e->mac[2],
             l2e->mac[3],l2e->mac[4],l2e->mac[5]));
    LOG_CLI((BSL_META("VLAN_TAG=0x%x| PORT=%d, |\n"),
l2e->vid, l2e->port));
}

/*
 * Function:
 *      _bcm_petra_vid_to_vsi
 * Purpose:
 *      Convert an L2 BCM-VID to Soc_petra-VSI
 * Parameters:
 *      vid         (IN) BCM VID
 *      soc_ppd_vsi     (OUT) PPD VSI used for the given VID
 */
int
_bcm_petra_vid_to_vsi(int unit, bcm_vlan_t vid, SOC_PPC_VSI_ID *soc_ppd_vsi)
{
    BCMDNX_INIT_FUNC_DEFS;
    *soc_ppd_vsi = vid;

    BCM_EXIT;
 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_vid_to_rif
 * Purpose:
 *      Convert an L2 BCM-VID to Soc_petra-RIF
 * Parameters:
 *      vid         (IN) BCM VID
 *      rif     (OUT) PPD RIF
 */
int
_bcm_petra_vid_to_rif(int unit, bcm_vlan_t vid, SOC_PPC_RIF_ID *rif)
{
    SOC_PPC_VSI_ID
        vsi;
    int rv;


    BCMDNX_INIT_FUNC_DEFS;
    rv = _bcm_petra_vid_to_vsi(unit,vid,&vsi);
    BCMDNX_IF_ERR_EXIT(rv);

    *rif = vsi;
     
    BCM_EXIT;
 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_vid_from_vsi
 * Purpose:
 *      Convert Soc_petra-VSI to L2 BCM-VID
 * Parameters:
 *      vid         (IN) BCM VID
 *      soc_ppd_vsi     (OUT) PPD VSI used for the given VID
 */
int
_bcm_petra_vid_from_vsi(int unit, SOC_PPC_VSI_ID soc_ppd_vsi, bcm_vlan_t* vid)
{
    BCMDNX_INIT_FUNC_DEFS;
    *vid = soc_ppd_vsi;    

    BCM_EXIT;
 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_vsi_to_fid
 * Purpose:
 *      map VSI to FID
 * Parameters:
 *      vid         (IN) BCM VID
 *      soc_ppd_vsi     (OUT) PPD VSI used for the given VID
 */
int
_bcm_petra_vsi_to_fid(int unit, SOC_PPC_VSI_ID vsi, SOC_PPC_FID *fid)
{
    BCMDNX_INIT_FUNC_DEFS;
    *fid = vsi;

    /* 
     * Assumption is that VSI == FID
     */

    BCM_EXIT;
 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_fid_to_vsi
 * Purpose:
 *      map VSI to FID
 * Parameters:
 *      soc_ppd_vsi     (IN) PPD VSI used for the given VID
 *      vid         (OUT) BCM VID
 */
int
_bcm_petra_fid_to_vsi(int unit, SOC_PPC_FID fid, SOC_PPC_VSI_ID *vsi)
{
    BCMDNX_INIT_FUNC_DEFS;
    *vsi = fid;
    /* 
     * Assumption is that FID == VSI
     */

    BCM_EXIT;
 
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_petra_mc_id_to_ppd_val
 * Purpose:
 *      map mc-index to PPD-MC-id
 * Parameters:
 *      l2mc_index         (IN) l2mc_index
 *      soc_ppd_mc_id       (OUT) PPD VSI used for the given VID
 */
int
_bcm_petra_mc_id_to_ppd_val(int unit, int l2mc_index, SOC_TMC_MULT_ID *soc_ppd_mc_id)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_group_to_id(l2mc_index,soc_ppd_mc_id));
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_petra_mc_id_from_ppd_val
 * Purpose:
 *      map mc-index to PPD-MC-id
 * Parameters:
 *      l2mc_index         (IN) l2mc_index
 *      soc_ppd_mc_id       (OUT) PPD VSI used for the given VID
 */
int
_bcm_petra_mc_id_from_ppd_val(int unit, int *l2mc_index, SOC_TMC_MULT_ID soc_ppd_mc_id)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_group_from_id(l2mc_index,_BCM_MULTICAST_TYPE_L2, soc_ppd_mc_id));
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_petra_l2_to_petra_entry
 * Purpose:
 *      Convert an L2 API data structure to a Soc_petra L2 entry
 * Parameters:
 *      unit        Unit number
 *      l2_entry   (OUT) Soc_petra L2 entry
 *      l2addr      L2 API data structure
 *      key_only    Only construct key portion
 */
int
_bcm_petra_l2_to_petra_entry(int unit, bcm_l2_addr_t *l2addr, int key_only, SOC_PPC_FRWRD_MACT_ENTRY_KEY *l2_entry_key, SOC_PPC_FRWRD_MACT_ENTRY_VALUE *l2_entry_val)
{ 
    SOC_TMC_MULT_ID     mc_group_id;
    int     rv = BCM_E_NONE;
    int     is_outlif = 1;
    SOC_PPC_VSI_ID vsi;

    bcm_gport_t     trill_port;
    uint32 soc_sand_rv;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(l2addr);
    BCMDNX_NULL_CHECK(l2_entry_key);
    if(!key_only) {
        BCMDNX_NULL_CHECK(l2_entry_val);
    }

    /* Check for valid flags */
    {
        uint32 temp_flags = _BCM_PETRA_L2_FLAGS_SUPPORTED(unit);
        if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ipmc_l2_ssm_mode_lem", 0)) {
            temp_flags |= BCM_L2_SRC_HIT;
        }

        if (l2addr->flags & (~temp_flags)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unsupported flags")));
        }
    }

    if (l2addr->cos_dst < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("cos_dst has to be zero")));
    }

    rv = _bcm_dpp_vlan_check(unit, l2addr->vid);
    BCMDNX_IF_ERR_EXIT(rv);

    /* encap_id 0 is not supported */
    if (l2addr->encap_id == 0) {
        l2addr->encap_id = BCM_FORWARD_ENCAP_ID_INVALID;
    }

    /* clear structures */
    SOC_PPC_FRWRD_MACT_ENTRY_KEY_clear(l2_entry_key);

    if (l2addr->flags & BCM_L2_SRC_HIT) {
        if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ipmc_l2_ssm_mode_lem", 0)) {
            l2_entry_key->key_type = SOC_PPC_FRWRD_MACT_KEY_TYPE_IPV6_MC_SSM_EUI;
       } else {
           BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_L2_SRC_HIT is only supported when soc property custom_feature_ipmc_l2_ssm_mode_lem=1")));
       }
    } else {
        /* map key */
        l2_entry_key->key_type = SOC_PPC_FRWRD_MACT_KEY_TYPE_MAC_ADDR;
    }

    /* vid to vsi*/
    rv = _bcm_petra_vid_to_vsi(unit, l2addr->vid,&(vsi));
    BCMDNX_IF_ERR_EXIT(rv);

    /* MAC from BCM to PPD strucutre*/
    rv = _bcm_petra_mac_to_sand_mac(l2addr->mac,&(l2_entry_key->key_val.mac.mac));
    BCMDNX_IF_ERR_EXIT(rv);

    /* VSI to FID*/
    rv = _bcm_petra_vsi_to_fid(unit, vsi, &l2_entry_key->key_val.mac.fid);
    BCMDNX_IF_ERR_EXIT(rv);
 
    if (key_only) {
        BCM_EXIT;
    }

    SOC_PPC_FRWRD_MACT_ENTRY_VALUE_clear(l2_entry_val);

    /* is it Multicast entry */
    if (l2addr->flags & BCM_L2_MCAST || l2addr->l2mc_group > 0) { 
        /* old implementation that use sw db for mapping mc-id to nickname*/
        if (SOC_DPP_CONFIG(unit)->trill.mc_id){
            rv = _bcm_dpp_mc_to_trill_get (unit, l2addr->l2mc_group , &trill_port );
            if (rv == BCM_E_NOT_FOUND) {
                rv = BCM_E_NONE;

                _bcm_petra_mc_id_to_ppd_val(unit, l2addr->l2mc_group ,&mc_group_id);
                if (mc_group_id > BCM_DPP_MAX_MC_ID(unit)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("l2mc_index is out of range")));
                }
                SOC_PPD_FRWRD_DECISION_MC_GROUP_SET(unit, &(l2_entry_val->frwrd_info.forward_decision), mc_group_id, soc_sand_rv);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            } else {           
                rv = _bcm_dpp_gport_to_fwd_decision(unit,trill_port,&(l2_entry_val->frwrd_info.forward_decision));
                BCMDNX_IF_ERR_EXIT(rv);
            }
        } else {
            /*convert trill gport to forwarding decision (destination + editing information)*/
            if (BCM_GPORT_IS_TRILL_PORT(l2addr->port)) {
                rv = _bcm_dpp_trill_gport_to_fwd_decision(unit, l2addr->port, l2addr->l2mc_group, &(l2_entry_val->frwrd_info.forward_decision));
                BCMDNX_IF_ERR_EXIT(rv);
            } else {
                _bcm_petra_mc_id_to_ppd_val(unit, l2addr->l2mc_group ,&mc_group_id);
                if (mc_group_id > BCM_DPP_MAX_MC_ID(unit)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("l2mc_index is out of range")));
                }
                SOC_PPD_FRWRD_DECISION_MC_GROUP_SET(unit, &(l2_entry_val->frwrd_info.forward_decision), mc_group_id, soc_sand_rv);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
        }
    } else if(l2addr->flags & BCM_L2_DISCARD_DST) {
        SOC_PPD_FRWRD_DECISION_TRAP_SET(unit, &(l2_entry_val->frwrd_info.forward_decision), _BCM_PETRA_UD_DROP_TRAP, 5 ,0, soc_sand_rv);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    } else {
        if ((l2addr->flags & BCM_L2_SRC_HIT) && (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ipmc_l2_ssm_mode_lem", 0))) {
            if (BCM_GPORT_IS_FORWARD_PORT(l2addr->port)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("FEC is not supported with BCM_L2_SRC_HIT")));
            }
            else {
                /* gport is just physical gport. map it using generic APi */
                rv = _bcm_dpp_gport_to_fwd_decision(unit,l2addr->port,&(l2_entry_val->frwrd_info.forward_decision));
                BCMDNX_IF_ERR_EXIT(rv);
           }
           if (BCM_FORWARD_ENCAP_ID_IS_OUTLIF(l2addr->encap_id)) {
                l2_entry_val->frwrd_info.forward_decision.additional_info.outlif.type = SOC_PPC_OUTLIF_ENCODE_TYPE_RAW;
                l2_entry_val->frwrd_info.forward_decision.additional_info.outlif.val = BCM_FORWARD_ENCAP_ID_VAL_GET(l2addr->encap_id);
           } else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("encap_id must be OUTLIF")));
           }
       } else {
             rv = _bcm_dpp_port_encap_to_fwd_decision(unit,l2addr->port,l2addr->encap_id ,&(l2_entry_val->frwrd_info.forward_decision));
             BCMDNX_IF_ERR_EXIT(rv);
       }
       if(l2addr->flags & BCM_L2_TRUNK_MEMBER) {
           if ((l2addr->tgid > BCM_DPP_MAX_TRUNK_ID(unit)) || (l2addr->tgid < 0)) {
               return BCM_E_PARAM;
           }
           l2_entry_val->frwrd_info.forward_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_UC_LAG;
           l2_entry_val->frwrd_info.forward_decision.dest_id = l2addr->tgid;
       }
    }

    if ((l2_entry_val->frwrd_info.forward_decision.additional_info.eei.type >
        SOC_PPC_EEI_TYPE_EMPTY) &&
        (l2_entry_val->frwrd_info.forward_decision.additional_info.eei.type <
        SOC_PPC_NOF_EEI_TYPES_ARAD)) {
        is_outlif = 0;
    } else {
        is_outlif = 1;
    }

    if ((l2addr->flags & BCM_L2_SET_ENCAP_VALID) && (is_outlif == 1)) {
        l2_entry_val->frwrd_info.forward_decision.additional_info.outlif.type = SOC_PPC_OUTLIF_ENCODE_TYPE_RAW;
        l2_entry_val->frwrd_info.forward_decision.additional_info.outlif.val = 0;
    }

    if ((l2addr->flags & BCM_L2_SET_ENCAP_INVALID) && (is_outlif == 1)) {
        l2_entry_val->frwrd_info.forward_decision.additional_info.outlif.type = SOC_PPC_OUTLIF_ENCODE_TYPE_RAW_INVALID;
    }

    /* Setting flag */
    l2_entry_val->frwrd_info.drop_when_sa_is_known = (l2addr->flags & BCM_L2_DISCARD_SRC)?TRUE:FALSE;
    l2_entry_val->aging_info.is_dynamic = !(l2addr->flags & BCM_L2_STATIC);

    /* group setting */
    if (l2addr->group != 0) {
        if (l2_entry_val->frwrd_info.forward_decision.additional_info.eei.type != SOC_PPC_EEI_TYPE_EMPTY){
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("group ID must be zero for given gport type")));
        }
        l2_entry_val->group = l2addr->group;
    }
    if (SOC_IS_ARADPLUS_A0(unit))
    {
        /* Only in ARAD+ :
         * in case of static entry add, outlif should be set to valid (value not set), 
         * in which case the signal learn_or_transplant will reflect reality.
         */
        if(l2addr->flags & BCM_L2_STATIC)
        {
            if (l2_entry_val->frwrd_info.forward_decision.additional_info.eei.type == SOC_PPC_EEI_TYPE_EMPTY &&
                l2_entry_val->frwrd_info.forward_decision.additional_info.outlif.type == SOC_PPC_OUTLIF_ENCODE_TYPE_NONE) {
                l2_entry_val->frwrd_info.forward_decision.additional_info.outlif.type = SOC_PPC_OUTLIF_ENCODE_TYPE_RAW;
            }
        }
        
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_l2_from_petra_entry
 * Purpose:
 *      Convert an L2 API data structure to a Soc_petra L2 entry
 * Parameters:
 *      unit        Unit number
 *      l2_entry   (OUT) Soc_petra L2 entry
 *      l2addr      L2 API data structure
 *      key_only    Only construct key portion
 */
int
_bcm_petra_l2_from_petra_entry(int unit, bcm_l2_addr_t *l2addr, int key_only, SOC_PPC_FRWRD_MACT_ENTRY_KEY *l2_entry_key, SOC_PPC_FRWRD_MACT_ENTRY_VALUE *l2_entry_val)
{ 
    int     rv = BCM_E_NONE;
    SOC_PPC_FID fid;
    SOC_PPC_VSI_ID vsi;
    bcm_mac_t mac_t;
    bcm_vlan_t vlan;

    BCMDNX_INIT_FUNC_DEFS;
    /* Check params and get device handler */
    if (l2_entry_key->key_type != SOC_PPC_FRWRD_MACT_KEY_TYPE_MAC_ADDR) {
        if ((soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ipmc_l2_ssm_mode_lem", 0)) && 
            (l2_entry_key->key_type != SOC_PPC_FRWRD_MACT_KEY_TYPE_IPV6_MC_SSM_EUI)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unexpected key type has to be MAC entry")));
       }
    }

    fid = l2_entry_key->key_val.mac.fid;

    /* FID to VSI*/
    rv = _bcm_petra_fid_to_vsi(unit, fid, &(vsi));
    BCMDNX_IF_ERR_EXIT(rv);

    /* VSI to VID*/
    rv = _bcm_petra_vid_from_vsi(unit, (vsi), &(vlan));
    BCMDNX_IF_ERR_EXIT(rv);


    /* MAC from PPD to BCM structure*/
    rv = _bcm_petra_mac_from_sand_mac(mac_t,&(l2_entry_key->key_val.mac.mac));
    BCMDNX_IF_ERR_EXIT(rv);

    bcm_l2_addr_t_init(l2addr,mac_t,vlan);

    if (key_only) {
        BCM_EXIT;
    }

    /* is it Multicast entry */
    if (l2_entry_val->frwrd_info.forward_decision.type == SOC_PPC_FRWRD_DECISION_TYPE_MC){
        l2addr->flags |= BCM_L2_MCAST;

        rv = _bcm_petra_mc_id_from_ppd_val(unit, &l2addr->l2mc_group,  l2_entry_val->frwrd_info.forward_decision.dest_id);
        BCMDNX_IF_ERR_EXIT(rv); 
                 
        if (!SOC_DPP_CONFIG(unit)->trill.mc_id) {
            bcm_gport_t gport_dummy;

            rv = _bcm_dpp_gport_and_encap_from_fwd_decision(unit, 
                                                            &gport_dummy, 
                                                            &(l2addr->encap_id), 
                                                            &(l2_entry_val->frwrd_info.forward_decision), 
                                                            BCM_FORWARD_ENCAP_ID_OUTLIF_USAGE_GENERAL,
                                                            0/*force_destination*/); 
            BCMDNX_IF_ERR_EXIT(rv);            
        }
    }
    else if (l2_entry_val->frwrd_info.forward_decision.type == SOC_PPC_FRWRD_DECISION_TYPE_TRAP){
        if(l2_entry_val->frwrd_info.forward_decision.additional_info.trap_info.action_profile.trap_code == _BCM_PETRA_UD_DROP_TRAP){
            l2addr->flags |= BCM_L2_DISCARD_DST;
        }
        else{
            rv = _bcm_dpp_gport_and_encap_from_fwd_decision(unit,&(l2addr->port),NULL,&(l2_entry_val->frwrd_info.forward_decision), -1/*encap_usage*/,0/*force_destination*/);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }
    else
    {
        /* for MACT encap-usage is not relevant, ignored in set, and not returned in get */
        rv = _bcm_dpp_gport_and_encap_from_fwd_decision(unit,
                                                        &(l2addr->port),
                                                        &(l2addr->encap_id),
                                                        &(l2_entry_val->frwrd_info.forward_decision),
                                                        BCM_FORWARD_ENCAP_ID_OUTLIF_USAGE_GENERAL,
                                                        0/*force_destination*/);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    if (l2_entry_val->frwrd_info.forward_decision.additional_info.outlif.type == SOC_PPC_OUTLIF_ENCODE_TYPE_RAW_INVALID) {
        l2addr->flags |= BCM_L2_SET_ENCAP_INVALID;
    }

    /* Setting flag */
    if(l2_entry_val->frwrd_info.drop_when_sa_is_known) {
        l2addr->flags |= BCM_L2_DISCARD_SRC;
    }
    
    if (!l2_entry_val->aging_info.is_dynamic) {
        l2addr->flags |= BCM_L2_STATIC;
    }

    if (l2_entry_val->accessed) {
        l2addr->flags |= BCM_L2_HIT;
    }

    l2addr->age_state = l2_entry_val->aging_info.age_status;

    /* if lag, then set also trunk info */
    if(l2_entry_val->frwrd_info.forward_decision.type == SOC_PPC_FRWRD_DECISION_TYPE_UC_LAG) {
        l2addr->flags |= BCM_L2_TRUNK_MEMBER;
        l2addr->tgid = l2_entry_val->frwrd_info.forward_decision.dest_id;
    }

    /* indication that outlif valid bit is set, but outlif is NULL */
    if ((l2_entry_val->frwrd_info.forward_decision.additional_info.outlif.type == SOC_PPC_OUTLIF_ENCODE_TYPE_RAW) && 
        (l2_entry_val->frwrd_info.forward_decision.additional_info.outlif.val == 0)) {
        l2addr->flags |= BCM_L2_SET_ENCAP_VALID;
        l2addr->encap_id = BCM_FORWARD_ENCAP_ID_INVALID;
    }

    l2addr->group = l2_entry_val->group;

    BCMDNX_IF_ERR_EXIT(rv);
  
exit:
    BCMDNX_FUNC_RETURN;
}

/* Identical to _bcm_petra_l2_from_petra_entry but always uses arad format as input */
int
_bcm_petra_l2_from_petra_entry_arad_format(int unit, bcm_l2_addr_t *l2addr, int key_only, SOC_PPC_FRWRD_MACT_ENTRY_KEY *l2_entry_key, SOC_PPC_FRWRD_MACT_ENTRY_VALUE *l2_entry_val)
{ 
    int     rv = BCM_E_NONE;
    SOC_PPC_FID fid;
    SOC_PPC_VSI_ID vsi;
    bcm_mac_t mac_t;
    bcm_vlan_t vlan;

    BCMDNX_INIT_FUNC_DEFS;
    /* Check params and get device handler */
    if (l2_entry_key->key_type != SOC_PPC_FRWRD_MACT_KEY_TYPE_MAC_ADDR) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unexpected key type has to be MAC entry")));
    }

    fid = l2_entry_key->key_val.mac.fid;

    /* FID to VSI*/
    rv = _bcm_petra_fid_to_vsi(unit, fid, &(vsi));
    BCMDNX_IF_ERR_EXIT(rv);

    /* VSI to VID*/
    rv = _bcm_petra_vid_from_vsi(unit, (vsi), &(vlan));
    BCMDNX_IF_ERR_EXIT(rv);


    /* MAC from PPD to BCM structure*/
    rv = _bcm_petra_mac_from_sand_mac(mac_t,&(l2_entry_key->key_val.mac.mac));
    BCMDNX_IF_ERR_EXIT(rv);

    bcm_l2_addr_t_init(l2addr,mac_t,vlan);

    if (key_only) {
        BCM_EXIT;
    }

    /* is it Multicast entry */
    if (l2_entry_val->frwrd_info.forward_decision.type == SOC_PPC_FRWRD_DECISION_TYPE_MC){
        l2addr->flags |= BCM_L2_MCAST;

        rv = _bcm_petra_mc_id_from_ppd_val(unit, &l2addr->l2mc_group,  l2_entry_val->frwrd_info.forward_decision.dest_id);
        BCMDNX_IF_ERR_EXIT(rv); 
                 
   /*     if (!SOC_DPP_CONFIG(unit)->trill.mc_id) { 
            rv = _bcm_dpp_encap_from_fwd_decision(unit, &(l2addr->encap_id), &(l2_entry_val->frwrd_info.forward_decision), _BCM_DPP_GPORT_ENCAP_USAGE_GENERAL); 
            BCMDNX_IF_ERR_EXIT(rv);            
        } */
    }
    else if (l2_entry_val->frwrd_info.forward_decision.type == SOC_PPC_FRWRD_DECISION_TYPE_TRAP){
        if(l2_entry_val->frwrd_info.forward_decision.additional_info.trap_info.action_profile.trap_code == _BCM_PETRA_UD_DROP_TRAP){
            l2addr->flags |= BCM_L2_DISCARD_DST;
        }
        else{
            rv = _bcm_dpp_gport_and_encap_from_fwd_decision(unit,&(l2addr->port),NULL,&(l2_entry_val->frwrd_info.forward_decision), -1/*encap_usage*/,0/*force_destination*/);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }
    else
    {

        /* for MACT encap-usage is not relevant, ignored in set, and not returned in get */
        rv = _bcm_dpp_gport_and_encap_from_fwd_decision(unit,
                                                        &(l2addr->port),
                                                        &(l2addr->encap_id),
                                                        &(l2_entry_val->frwrd_info.forward_decision),
                                                        BCM_FORWARD_ENCAP_ID_OUTLIF_USAGE_GENERAL,
                                                        0/*force_destination*/);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* Setting flag */
    if(l2_entry_val->frwrd_info.drop_when_sa_is_known) {
        l2addr->flags |= BCM_L2_DISCARD_SRC;
    }
    
    if (!l2_entry_val->aging_info.is_dynamic) {
        l2addr->flags |= BCM_L2_STATIC;
    }

    if (l2_entry_val->accessed) {
        l2addr->flags |= BCM_L2_HIT;
    }

    l2addr->age_state = l2_entry_val->aging_info.age_status;

    /* if lag, then set also trunk info */
    if(l2_entry_val->frwrd_info.forward_decision.type == SOC_PPC_FRWRD_DECISION_TYPE_UC_LAG) {
        l2addr->flags |= BCM_L2_TRUNK_MEMBER;
        l2addr->tgid = l2_entry_val->frwrd_info.forward_decision.dest_id;
    }
    l2addr->group = l2_entry_val->group;

    BCMDNX_IF_ERR_EXIT(rv);
  
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_petra_l2_from_petra_bmact_entry(int unit, bcm_l2_addr_t *l2addr, int key_only, SOC_PPC_BMACT_ENTRY_KEY *l2_entry_key, SOC_PPC_BMACT_ENTRY_INFO *l2_entry_val)
{ 
    int     rv = BCM_E_NONE;
    bcm_mac_t mac_t;
    bcm_vlan_t vlan;
    bcm_module_t modid;
    bcm_port_t mode_port;

    BCMDNX_INIT_FUNC_DEFS;
    /* convert VID to B-VID */
    vlan = _BCM_DPP_VLAN_TO_BVID(unit, l2_entry_key->b_vid);
    
    /* MAC from PPD to BCM strucutre */
    rv = _bcm_petra_mac_from_sand_mac(mac_t,&(l2_entry_key->b_mac_addr));
    BCMDNX_IF_ERR_EXIT(rv);

    bcm_l2_addr_t_init(l2addr, mac_t, vlan);

    if (key_only) {
        BCM_EXIT;
    }
    
    /* Setting flag */
    if (l2_entry_val->drop_sa) {
        l2addr->flags |= BCM_L2_DISCARD_SRC;
    }

    if (l2_entry_val->mc_id != 0) {
        rv = _bcm_petra_mc_id_from_ppd_val(unit, &l2addr->l2mc_group, l2_entry_val->mc_id);
    } else if (l2_entry_key->flags & SOC_PPC_BMACT_ENTRY_TYPE_LAG) {
        l2addr->flags |= BCM_L2_TRUNK_MEMBER;
        l2addr->tgid = l2_entry_val->sys_port_id;
    } else {
        /* map system port to mod-port*/
        rv = _bcm_dpp_port_mod_port_from_sys_port(unit, &modid, &mode_port, l2_entry_val->sys_port_id);
        BCMDNX_IF_ERR_EXIT(rv);
        BCM_GPORT_MODPORT_SET(l2addr->port, modid, mode_port);
    }

    if (!l2_entry_val->is_dynamic) {
        l2addr->flags |= BCM_L2_STATIC;
    }
    l2addr->age_state = l2_entry_val->age_status;

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_petra_l2_to_petra_bmact_entry(int unit, bcm_l2_addr_t *l2addr, int key_only, SOC_PPC_BMACT_ENTRY_KEY *l2_entry_key, SOC_PPC_BMACT_ENTRY_INFO *l2_entry_val)
{ 
    int     rv = BCM_E_NONE;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    uint8 found;
    int local_port;
    _bcm_dpp_gport_info_t gport_info;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(l2addr);
    BCMDNX_NULL_CHECK(l2_entry_key);
    if(!key_only) {
        BCMDNX_NULL_CHECK(l2_entry_val);
    } 

    /* Check for valid flags */
    if (l2addr->flags & (~(_BCM_PETRA_L2_FLAGS_SUPPORTED(unit)))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Unsupported L2 Flags")));
    }

    /* Due to Jericho MiM erratum the BVID is in the range of 28-32K and will cause the fail of this check. So this check is masked in Jericho */
    if (!(SOC_IS_JERICHO_A0(unit) | SOC_IS_JERICHO_B0(unit) | SOC_IS_QMX(unit))) {
        rv = _bcm_dpp_vlan_check(unit, l2addr->vid);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    

    soc_sand_dev_id = (unit);

    /* clear structures */
    SOC_PPC_BMACT_ENTRY_KEY_clear(l2_entry_key);

    l2_entry_key->b_vid = l2addr->vid;

    /* MAC from BCM to PPD strucutre*/
    rv = _bcm_petra_mac_to_sand_mac(l2addr->mac,&(l2_entry_key->b_mac_addr));
    BCMDNX_IF_ERR_EXIT(rv);
 
    l2_entry_key->flags |= SOC_PPC_BMACT_ENTRY_TYPE_FRWRD;

    if (key_only) {
        BCM_EXIT;
    }

    /* get entry, if exists */
    soc_sand_rv = soc_ppd_frwrd_bmact_entry_get(soc_sand_dev_id, l2_entry_key, l2_entry_val, &found);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    
    l2_entry_val->drop_sa = (l2addr->flags & BCM_L2_DISCARD_SRC) ? TRUE : FALSE;
    l2_entry_val->is_dynamic = !(l2addr->flags & BCM_L2_STATIC);
    l2_entry_val->age_status = l2addr->age_state;
 
    /* is it Multicast entry */
    if (l2addr->flags & BCM_L2_MCAST || l2addr->l2mc_group > 0) {
        _bcm_petra_mc_id_to_ppd_val(unit, l2addr->l2mc_group ,&l2_entry_val->mc_id);
        if (l2_entry_val->mc_id > BCM_DPP_MAX_MC_ID(unit)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("l2mc_index is out of range")));
        }
        l2_entry_key->flags |= SOC_PPC_BMACT_ENTRY_TYPE_MC_DEST;
        l2_entry_val->sys_port_id = l2addr->encap_id << 1; /*nibble align for extraction*/
    } else if(l2addr->flags & BCM_L2_TRUNK_MEMBER) {
        if ((l2addr->tgid > BCM_DPP_MAX_TRUNK_ID(unit)) || (l2addr->tgid < 0)) {
            return BCM_E_PARAM;
        }
        l2_entry_key->flags |= SOC_PPC_BMACT_ENTRY_TYPE_LAG;
        l2_entry_val->sys_port_id = l2addr->tgid;
    } else {
        rv = _bcm_dpp_gport_to_phy_port(unit, l2addr->port, 0, &gport_info); 
        BCMDNX_IF_ERR_EXIT(rv);

        l2_entry_val->sys_port_id = gport_info.sys_port;

        if (_BCM_DPP_GPORT_INFO_IS_LOCAL_PORT(gport_info)) {
            /* get port from port bit map */
            BCM_PBMP_ITER(gport_info.pbmp_local_ports, local_port)
                l2_entry_val->sys_port_id = local_port;
        }
    }  

    if (SOC_IS_ARADPLUS_A0(unit))
    {
        /* Only in ARAD+ :
         * in case of static entry add, outlif should be set to valid (value not set), 
         * in which case the signal learn_or_transplant will reflect reality.
         */
        if(l2addr->flags & BCM_L2_STATIC)
        {
            l2_entry_key->flags |= SOC_PPC_BMACT_ENTRY_TYPE_STATIC;
        }
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_petra_l2_mpls_extended_label_fill_ether_type_index_table
 * Purpose:
 *      Fill the ether type index table for mpls extended label feature (see bcm_petra_l2_egress_create).
 *
 * Parameters:
 *      eth_type_index_entry      (IN) Information for the entry: outer tpid and ether type. These fields will be inserted into each of the entries
 *                                     with only inner tpid varying (it is equal to the entry's index).
 */
int _bcm_petra_l2_mpls_extended_label_fill_ether_type_index_table(int unit,SOC_PPC_EG_ENCAP_ETHER_TYPE_INDEX_INFO eth_type_index_entry)
{
    int rv = BCM_E_NONE;
    int entry_index;
    int size_of_table = 16;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    for (entry_index = 0; entry_index < size_of_table; entry_index++) {
        /* Inner tpid value is index of entry */
        eth_type_index_entry.tpid_0 = entry_index;
        rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_eg_encap_ether_type_index_set,
                                      (unit, entry_index, &eth_type_index_entry)));
        BCMDNX_IF_ERR_EXIT(rv);
    }    

exit:
    BCMDNX_FUNC_RETURN;
}

#ifdef BCM_CMICM_SUPPORT
/*
 * Function:
 *      _bcm_petra_L2_llm_callback_thread
 * Purpose:
 *      Thread to listen for event messages from ukernel.
 *
 * Returns:
 *      None
 */
STATIC void
_bcm_petra_l2_llm_callback_thread(int unit)
{
    int rv;
    llm_appl_info_t *llm_info = &(llm_appl_info[unit]);      
    mos_msg_data_t event_msg;
    bcm_l2_addr_t old_l2addr;
    bcm_l2_addr_t new_l2addr;
    shr_llm_msg_pon_att_mac_move_event_t mac_move_event;
    uint8 *buffer;
    int gport, gport_id;
    int i = 0;
    char thread_name[SAL_THREAD_NAME_MAX_LEN];
    uint32 enable = 0;
    
    llm_info->event_thread_kill = 0;
    thread_name[0] = 0;
    sal_thread_name(llm_info->event_thread_id, thread_name, sizeof (thread_name));

    while (1) {
        /* Wait on notifications from uController */
        sal_memset(&event_msg, 0x0, sizeof(mos_msg_data_t));
        rv = soc_cmic_uc_msg_receive(unit, llm_info->uc_num,
                                     MOS_MSG_CLASS_LLM_EVENT, &event_msg,
                                     sal_sem_FOREVER);

        if (BCM_FAILURE(rv) || (llm_info->event_thread_kill)) {
            break;   /* Thread exit */
        }
        rv = shr_llm_mac_move_event_get(unit, _LLM_EVENT_MAC_MOVE, &enable);
        if (BCM_FAILURE(rv)) {
            break;   /* Thread exit */
        }

        if (event_msg.s.subclass == MOS_MSG_SUBCLASS_LLM_EVENT_MAC_MOVE
            && enable) {
            sal_memset(&mac_move_event, 0x0, sizeof(shr_llm_msg_pon_att_mac_move_event_t));
            buffer = llm_info->dma_buffer;
            shr_llm_msg_pon_att_mac_move_event_unpack(buffer, &mac_move_event);
            bcm_l2_addr_t_init(&old_l2addr, mac_move_event.mac, mac_move_event.vsi);
            BCM_GPORT_SUB_TYPE_LIF_SET( gport_id ,0 ,mac_move_event.existing_ac);
            BCM_GPORT_VLAN_PORT_ID_SET(gport, gport_id);
            old_l2addr.port = gport;
            bcm_l2_addr_t_init(&new_l2addr, mac_move_event.mac, mac_move_event.vsi);
            BCM_GPORT_SUB_TYPE_LIF_SET( gport_id ,0, mac_move_event.incoming_ac);
            BCM_GPORT_VLAN_PORT_ID_SET(gport, gport_id);
            new_l2addr.port = gport;

            /* Fill new L2 entry to user_data */
            for (i = 0; i < _BCM_PETRA_L2_CB_MAX; i++) { 
                if (_BCM_PETRA_L2_CB_ENTRY(unit, i).fn && 
                    _BCM_PETRA_L2_CB_ENTRY((unit), i).fn_data) { 
                    sal_memcpy(_BCM_PETRA_L2_CB_ENTRY((unit), i).fn_data, &new_l2addr, sizeof(bcm_l2_addr_t));
                }                                                        
            } 

            _BCM_PETRA_L2_CB_RUN(unit, &old_l2addr, BCM_L2_CALLBACK_MOVE_EVENT);
        }
    }

    llm_info->event_thread_kill = 0;
    llm_info->event_thread_id   = NULL;
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_COMMON,
                  (BSL_META_U(unit,
                              "AbnormalThreadExit:%s\n"), thread_name)); 
    }
    sal_thread_exit(0);
}


/*
 * Function:
 *      _bcm_petra_l2_llm_thread_init
 * Purpose:
 *      L2 LLM thread init.
 * Parameters:
 *      unit       (IN) unit number
 */
STATIC int
_bcm_petra_l2_llm_thread_init(int unit)
{
    llm_appl_info_t *llm_info = shr_llm_appl_info(unit);
    
    llm_info->event_thread_kill = 0;
    if (llm_info->event_thread_id == NULL) {
        llm_info->event_thread_id = sal_thread_create("bcmLLM", SAL_THREAD_STKSZ, 
                              SHR_LLM_EVENT_THREAD_PRIORTY,
                              (void (*)(void*))_bcm_petra_l2_llm_callback_thread,
                              INT_TO_PTR(unit));
        if (llm_info->event_thread_id == SAL_THREAD_ERROR) {
            if (SAL_BOOT_QUICKTURN) {
                /* If emulation, then wait */
                sal_usleep(1000000);
            }
        }

        if (llm_info->event_thread_id == NULL) {
            BCM_IF_ERROR_RETURN(bcm_petra_l2_detach(unit));
            return BCM_E_MEMORY;
        }
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_petra_l2_llm_thread_deinit
 * Purpose:
 *      L2 LLM thread init.
 * Parameters:
 *      unit       (IN) unit number
 */
STATIC int
_bcm_petra_l2_llm_thread_deinit(int unit)
{    
    sal_usecs_t timeout = 0;
    llm_appl_info_t *llm_info = shr_llm_appl_info(unit);

    /* Event Handler thread exit signal */
    if (llm_info->event_thread_id) {
        soc_cmic_uc_msg_receive_cancel(unit, llm_info->uc_num,
                                       MOS_MSG_CLASS_LLM_EVENT);

        /*
         * Wait some time at here to lensure event callback thread not-existed
         */
        timeout = sal_time_usecs() + 1000000;
        while (llm_info->event_thread_id != NULL) {
            if (sal_time_usecs() < timeout) {
                /*give some time to already running bfd thread to schedule and exit*/
                sal_usleep(1000);
            } else {
                /*timeout*/
                LOG_ERROR(BSL_LS_BCM_COMMON,
                          (BSL_META_U(unit,
                                      "LLM event thread did not exit.\n")));
                return BCM_E_INTERNAL;
            }
        }
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_petra_l2_service_type_get
 * Purpose:
 *      Get service type base gport.
 *
 * Parameters:
 *      gport_id      (IN) gport ID
 *      serv_type     (IN/OUT) service type, vmac or mac limit
 */
int _bcm_petra_l2_service_type_get(int         unit,
                                   bcm_gport_t gport_id,
                                   SHR_LLM_SERVICE_TYPE *serv_type)
{
    int rv = BCM_E_NONE;
    uint32    soc_sand_rv;
    SOC_PPC_LIF_ENTRY_INFO *lif_info = NULL;
    uint32 vmac_enable_in_ac = 0;
    _bcm_dpp_gport_hw_resources gport_hw_resources;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);


    rv = _bcm_dpp_gport_to_hw_resources(unit, gport_id, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK, 
                                                 &gport_hw_resources);
    BCMDNX_IF_ERR_EXIT(rv);    
    
    BCMDNX_ALLOC(lif_info, sizeof(*lif_info), "_bcm_petra_l2_service_type_get.lif_info");
    if (lif_info == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("unit %d, failed to allocate memory"), unit));
    }
    SOC_PPC_LIF_ENTRY_INFO_clear(lif_info);
    soc_sand_rv = soc_ppd_lif_table_entry_get(unit, gport_hw_resources.local_in_lif, lif_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    
    if (lif_info->type != SOC_PPC_LIF_ENTRY_TYPE_AC) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid lif index.")));
    }
    
    soc_sand_rv = soc_sand_bitstream_get_field(&(lif_info->value.ac.cos_profile),
                                               SOC_PPC_LIF_AC_VAMC_COS_PROFILE_LSB,
                                               SOC_PPC_LIF_AC_VAMC_COS_PROFILE_LSB_NOF_BITS,
                                               &vmac_enable_in_ac);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if (vmac_enable_in_ac == SOC_PPC_LIF_AC_VAMC_COS_PROFILE_ENABLE &&
        SOC_DPP_CONFIG(unit)->pp.vmac_enable) {
        *serv_type = LLM_SERVICE_VMAC;
    }
    else {
        *serv_type = LLM_SERVICE_MAC_LIMIT;
    }

exit:
    BCM_FREE(lif_info);
    BCMDNX_FUNC_RETURN;
}

#endif

int
bcm_petra_l2_addr_add(
                      int unit,
                      bcm_l2_addr_t *l2addr)
{

    SOC_SAND_SUCCESS_FAILURE
        failure_indication = SOC_SAND_SUCCESS;
    int rv = BCM_E_NONE;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
#ifdef BCM_CMICM_SUPPORT
    uint8 data_len = 0;
    shr_llm_msg_pon_att_l2_entry_t l2_entry;
    SHR_LLM_SERVICE_TYPE serv_type;
#endif
    BCMDNX_INIT_FUNC_DEFS;
    /* check parameters*/
    BCM_DPP_UNIT_CHECK(unit);
    _DPP_L2_INIT_CHECK(unit);
    BCMDNX_NULL_CHECK(l2addr);
    /* check rest of parameters in internal conversion functions*/
    soc_sand_dev_id = (unit);
    
    LLM_PER_TUNNEL_DISABLE_NO_MOS(unit);
    if ((_BCM_DPP_VLAN_IS_BVID(unit, l2addr->vid)) || ((SOC_DPP_CONFIG(unit)->pp.test2) && (l2addr->vid & 0x8000))) { /* vid is B-VID */

        SOC_PPC_BMACT_ENTRY_KEY l2_entry_key;
        SOC_PPC_BMACT_ENTRY_INFO l2_entry_val;
        bcm_l2_addr_t l2addr_cpy;

        MIM_INIT(unit);

        /* create a copy of l2_addr */
        sal_memcpy(&l2addr_cpy, l2addr, sizeof(bcm_l2_addr_t));

        if ((((SOC_DPP_CONFIG(unit)->pp.test2) && (l2addr->vid & 0x8000)))) {
            /* 
             * In EoE remove 0x8000 so b-vid can be used as vid
             */
            l2addr_cpy.vid= (l2addr_cpy.vid & 0x7FFF);
        } else {
            /* remove 0xf000 so b-vid can be used as vid, but not for Jericho due to Jericho MiM erratum*/
            if (!(SOC_IS_JERICHO_A0(unit) | SOC_IS_JERICHO_B0(unit) | SOC_IS_QMX(unit))) {
                l2addr_cpy.vid = _BCM_DPP_BVID_TO_VLAN(l2addr_cpy.vid);
            }
        }

        if (l2addr->flags2 & BCM_L2_FLAGS2_BMACT_LEARN) {
            uint32 soc_ppd_port;
            int local_port, core;
            _bcm_dpp_gport_info_t gport_info;
            uint8 found;

            SOC_PPC_BMACT_ENTRY_KEY_clear(&l2_entry_key);
            SOC_PPC_BMACT_ENTRY_INFO_clear(&l2_entry_val);

            rv = _bcm_dpp_gport_to_phy_port(unit, l2addr->modid, 0, &gport_info);
            BCMDNX_IF_ERR_EXIT(rv);

            BCM_PBMP_ITER(gport_info.pbmp_local_ports, local_port) {

                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, local_port, &soc_ppd_port, &core)));

                rv = _bcm_petra_mac_to_sand_mac(l2addr->mac, &(l2_entry_key.b_mac_addr));
                    BCMDNX_IF_ERR_EXIT(rv);
                l2_entry_key.b_vid = _BCM_DPP_BVID_TO_VLAN(l2addr->vid);

                l2_entry_key.flags = SOC_PPC_BMACT_ENTRY_TYPE_LEARN;
                l2_entry_key.local_port_ndx = soc_ppd_port;
                if (SOC_IS_JERICHO(unit) && !SOC_IS_QAX(unit)) {
                    l2_entry_key.core = core;
                }

                soc_sand_rv = soc_ppd_frwrd_bmact_entry_get(soc_sand_dev_id, &l2_entry_key, &l2_entry_val, &found);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                if (found) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("MiM learn entry already exists for this key")));
                }

                l2_entry_val.i_sid_domain = 0;
                l2_entry_val.sa_learn_fec_id = BCM_GPORT_MIM_PORT_ID_GET(l2addr->port);

                soc_sand_rv = soc_ppd_frwrd_bmact_entry_add(soc_sand_dev_id, &l2_entry_key, &l2_entry_val, &failure_indication);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                SOC_SAND_IF_FAIL_RETURN(failure_indication);
            }

        } else { /* BMACT FWD entry */
            /* map BCM MACT key/entry to Soc_petra MACT key/entry*/
            soc_sand_rv = _bcm_petra_l2_to_petra_bmact_entry(unit, &l2addr_cpy, FALSE, &l2_entry_key, &l2_entry_val);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
        /* add bmact entry */
        soc_sand_rv = soc_ppd_frwrd_bmact_entry_add(soc_sand_dev_id, &l2_entry_key, &l2_entry_val, &failure_indication);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
    else {
        SOC_PPC_FRWRD_MACT_ENTRY_KEY l2_entry_key;
        SOC_PPC_FRWRD_MACT_ENTRY_VALUE l2_entry_val;

        /* map BCM MACT key/entry to Soc_petra MACT key/entry*/
        rv = _bcm_petra_l2_to_petra_entry(unit, l2addr, FALSE, &l2_entry_key, &l2_entry_val);
        BCMDNX_IF_ERR_EXIT(rv);

        /* add to mact entry */
#ifdef BCM_CMICM_SUPPORT
        /* For ukernel MAC limit model, it doesn't update MAC limit database
         * when add/delete a L2 entry for multicast group. so in this case, needn't send updated 
         * message to ukernel.
         */
        if (SOC_MAC_LIMIT_PER_TUNNEL_ENABLE_GET(unit) &&
            (0 == (l2addr->flags & BCM_L2_MCAST))) {
            /* LCOV_EXCL_START */
            /* in PON application, needn't add a SA in physical port. 
             * now ukernel don't care add a SA in physical port 
             * because of it cann't update mac limit database in this case
             */
            if (BCM_GPORT_IS_SET(l2addr->port)) {
                rv = _bcm_petra_l2_service_type_get(unit, l2addr->port, &serv_type);
                BCMDNX_IF_ERR_EXIT(rv);
                soc_sand_rv = soc_ppd_frwrd_mact_entry_pack(soc_sand_dev_id,
                                                            TRUE,
                                                            SOC_PPC_FRWRD_MACT_ADD_TYPE_INSERT,
                                                            &l2_entry_key,
                                                            &l2_entry_val,
                                                            l2_entry.entry,
                                                            &data_len);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                rv = shr_llm_msg_pon_attr_set(unit, serv_type, SHR_LLM_MSG_PON_ATT_UPDATE_MAC_ENTRY, &l2_entry);
            }
            else {
                rv = BCM_E_PARAM;
            }
            BCMDNX_IF_ERR_EXIT(rv);
            /* LCOV_EXCL_STOP */
        }
        else
#endif
        {
            soc_sand_rv = soc_ppd_frwrd_mact_entry_add(soc_sand_dev_id,SOC_PPC_FRWRD_MACT_ADD_TYPE_INSERT,&l2_entry_key,&l2_entry_val,&failure_indication);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }
        
    SOC_SAND_IF_FAIL_RETURN(failure_indication);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    LLM_PER_TUNNEL_ENABLE_NO_MOS(unit);
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_l2_addr_delete(
                         int unit,
                         bcm_mac_t mac,
                         bcm_vlan_t vid)
{
    int rv = BCM_E_NONE;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    bcm_l2_addr_t 
        l2addr;
#ifdef BCM_CMICM_SUPPORT
    uint8 data_len = 0;
    shr_llm_msg_pon_att_l2_entry_t l2_entry;
    SHR_LLM_SERVICE_TYPE serv_type;
#endif

    BCMDNX_INIT_FUNC_DEFS;
    /* check parameters*/
    BCM_DPP_UNIT_CHECK(unit);
    _DPP_L2_INIT_CHECK(unit);
    
    soc_sand_dev_id = (unit);
    
    LLM_PER_TUNNEL_DISABLE_NO_MOS(unit);
    /* set l2addr*/
    bcm_l2_addr_t_init(&l2addr,mac,vid);

    rv = bcm_petra_l2_addr_get(unit, mac, vid, &l2addr);

    if (rv == BCM_E_NOT_FOUND) {
        LOG_VERBOSE(BSL_LS_BCM_L2, 
                    (BSL_META_U(unit,
                                "bcm_l2_addr_delete address not found\n")));
    }

    BCMDNX_IF_ERR_NOT_E_NOT_FOUND_EXIT(rv);

    if (_BCM_DPP_VLAN_IS_BVID(unit, l2addr.vid)) { /* vid is B-VID */

        SOC_PPC_BMACT_ENTRY_KEY l2_entry_key;
        SOC_PPC_BMACT_ENTRY_INFO l2_entry_val;

        MIM_INIT(unit);

        /* For Non Jericho devices remove 0xf000 so b-vid can be used as vid (Jericho uses BVID range of 28-32K so FID stays the same is done */
        if (!(SOC_IS_JERICHO_A0(unit) | SOC_IS_JERICHO_B0(unit) | SOC_IS_QMX(unit))) {
            l2addr.vid = _BCM_DPP_BVID_TO_VLAN(l2addr.vid);
        }

        /* map BCM MACT key/entry to Soc_petra MACT key/entry*/
        rv = _bcm_petra_l2_to_petra_bmact_entry(unit, &l2addr, FALSE, &l2_entry_key, &l2_entry_val);
        BCMDNX_IF_ERR_EXIT(rv);

        /* remove bmact entry */
        soc_sand_rv = soc_ppd_frwrd_bmact_entry_remove(soc_sand_dev_id,&l2_entry_key);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
    else {
        SOC_PPC_FRWRD_MACT_ENTRY_KEY l2_entry_key;
        SOC_PPC_FRWRD_MACT_ENTRY_VALUE l2_entry_val;

        /* remove mact entry */
#ifdef BCM_CMICM_SUPPORT
        if (SOC_MAC_LIMIT_PER_TUNNEL_ENABLE_GET(unit) &&
            (0 == (l2addr.flags & BCM_L2_MCAST))) {
            /* LCOV_EXCL_START */
            if (BCM_GPORT_IS_SET(l2addr.port)) {
                /* map BCM MACT key/entry to Soc_petra MACT key/entry 
                 * uKernel need key and value to update mac limit data base.
                 */
                rv = _bcm_petra_l2_to_petra_entry(unit, &l2addr, FALSE, &l2_entry_key, &l2_entry_val);
                BCMDNX_IF_ERR_EXIT(rv);
                rv = _bcm_petra_l2_service_type_get(unit, l2addr.port, &serv_type);
                BCMDNX_IF_ERR_EXIT(rv);
                soc_sand_rv = soc_ppd_frwrd_mact_entry_pack(soc_sand_dev_id,
                                                            FALSE,
                                                            0,/* needn't care this variable in remove operation */
                                                            &l2_entry_key,
                                                            &l2_entry_val,
                                                            l2_entry.entry,
                                                            &data_len);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                rv = shr_llm_msg_pon_attr_set(unit, serv_type, SHR_LLM_MSG_PON_ATT_UPDATE_MAC_ENTRY, &l2_entry);
            } 
            else {
                rv = BCM_E_PARAM;
            }
            BCMDNX_IF_ERR_EXIT(rv);
            /* LCOV_EXCL_STOP */
        }
        else
#endif
        {
            /* map BCM MACT key/entry to Soc_petra MACT key/entry*/
            rv = _bcm_petra_l2_to_petra_entry(unit, &l2addr, TRUE, &l2_entry_key, &l2_entry_val);
            BCMDNX_IF_ERR_EXIT(rv);
            soc_sand_rv = soc_ppd_frwrd_mact_entry_remove(soc_sand_dev_id,&l2_entry_key, 0);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }

    BCMDNX_IF_ERR_EXIT(rv);

exit:
    LLM_PER_TUNNEL_ENABLE_NO_MOS(unit);
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_l2_addr_multi_add(
                      int unit,
                      bcm_l2_addr_t *l2addr,
                      int count)
{

    int rv = BCM_E_NONE;
    int i;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    uint8 data_len = 0;
    shr_llm_msg_pon_att_multi_l2_entry_t multi_l2_entry;
    SHR_LLM_SERVICE_TYPE serv_type = LLM_SERVICE_NOT_DEFINED;
    SOC_PPC_FRWRD_MACT_ENTRY_KEY l2_entry_key;
    SOC_PPC_FRWRD_MACT_ENTRY_VALUE l2_entry_val;
    BCMDNX_INIT_FUNC_DEFS;
    /* check parameters*/
    BCM_DPP_UNIT_CHECK(unit);
    _DPP_L2_INIT_CHECK(unit);
    BCMDNX_NULL_CHECK(l2addr);
    /* check rest of parameters in internal conversion functions*/
    soc_sand_dev_id = (unit);
    
    LLM_PER_TUNNEL_DISABLE_NO_MOS(unit);
    if (count > SHR_LLM_MSG_PON_ATT_UPDATE_MULTI_MAC_ENTRY_NUM) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("bcm_l2_addr_multi_add() max support 32 MACs configuration")));
    }

#ifdef BCM_CMICM_SUPPORT
    if (SOC_MAC_LIMIT_PER_TUNNEL_ENABLE_GET(unit)) {
        for (i = 0; i < count; i++) {
            if ((0 == (l2addr[i].flags & BCM_L2_MCAST)) && BCM_GPORT_IS_SET(l2addr[i].port)) {
                rv = _bcm_petra_l2_to_petra_entry(unit, &l2addr[i], FALSE, &l2_entry_key, &l2_entry_val);
                BCMDNX_IF_ERR_EXIT(rv);
                rv = _bcm_petra_l2_service_type_get(unit, l2addr[i].port, &serv_type);
                BCMDNX_IF_ERR_EXIT(rv);
                soc_sand_rv = soc_ppd_frwrd_mact_entry_pack(soc_sand_dev_id,
                                                            TRUE,
                                                            SOC_PPC_FRWRD_MACT_ADD_TYPE_INSERT,
                                                            &l2_entry_key,
                                                            &l2_entry_val,
                                                            multi_l2_entry.l2_entry[i].entry,
                                                            &data_len);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }                
        }
        multi_l2_entry.l2_entry_count = count;
        rv = shr_llm_msg_pon_attr_set(unit, serv_type, SHR_LLM_MSG_PON_ATT_UPDATE_MULTI_MAC_ENTRY, &multi_l2_entry);
        BCMDNX_IF_ERR_EXIT(rv);     
    } else 
#endif
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("bcm_l2_addr_multi_add() is only supported for PON application")));
    }

exit:
    LLM_PER_TUNNEL_ENABLE_NO_MOS(unit);
    BCMDNX_FUNC_RETURN;
}


int
bcm_petra_l2_addr_multi_delete(
                      int unit,
                      bcm_l2_addr_t *l2addr,
                      int count)
{

    int rv = BCM_E_NONE;
    int i;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    uint8 data_len = 0;
    shr_llm_msg_pon_att_multi_l2_entry_t multi_l2_entry;
    SHR_LLM_SERVICE_TYPE serv_type = LLM_SERVICE_NOT_DEFINED;
    SOC_PPC_FRWRD_MACT_ENTRY_KEY l2_entry_key;
    SOC_PPC_FRWRD_MACT_ENTRY_VALUE l2_entry_val;
    BCMDNX_INIT_FUNC_DEFS;
    /* check parameters*/
    BCM_DPP_UNIT_CHECK(unit);
    _DPP_L2_INIT_CHECK(unit);
    BCMDNX_NULL_CHECK(l2addr);
    /* check rest of parameters in internal conversion functions*/
    soc_sand_dev_id = (unit);
    
    LLM_PER_TUNNEL_DISABLE_NO_MOS(unit);
    if (count > SHR_LLM_MSG_PON_ATT_UPDATE_MULTI_MAC_ENTRY_NUM) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("bcm_l2_addr_multi_delete() max support 16 MACs configuration")));
    }

#ifdef BCM_CMICM_SUPPORT
    if (SOC_MAC_LIMIT_PER_TUNNEL_ENABLE_GET(unit)) {
        for (i = 0; i < count; i++) {
            if ((0 == (l2addr[i].flags & BCM_L2_MCAST)) && BCM_GPORT_IS_SET(l2addr[i].port)) {
                rv = _bcm_petra_l2_to_petra_entry(unit, &l2addr[i], FALSE, &l2_entry_key, &l2_entry_val);
                BCMDNX_IF_ERR_EXIT(rv);
                rv = _bcm_petra_l2_service_type_get(unit, l2addr[i].port, &serv_type);
                BCMDNX_IF_ERR_EXIT(rv);
                soc_sand_rv = soc_ppd_frwrd_mact_entry_pack(soc_sand_dev_id,
                                                            FALSE,
                                                            0,/* needn't care this variable in remove operation */
                                                            &l2_entry_key,
                                                            &l2_entry_val,
                                                            multi_l2_entry.l2_entry[i].entry,
                                                            &data_len);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }                
        }
        multi_l2_entry.l2_entry_count = count;
        rv = shr_llm_msg_pon_attr_set(unit, serv_type, SHR_LLM_MSG_PON_ATT_UPDATE_MULTI_MAC_ENTRY, &multi_l2_entry);
        BCMDNX_IF_ERR_EXIT(rv);     
    } else 
#endif
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("bcm_l2_addr_multi_delete() is only supported for PON application")));
    }

exit:
    LLM_PER_TUNNEL_ENABLE_NO_MOS(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * flags used for traverse action
 */

#define _BCM_PETRA_TRAVERSE_MATCH_MAC       (0x00000001)
#define _BCM_PETRA_TRAVERSE_MATCH_PORT      (0x00000002)
#define _BCM_PETRA_TRAVERSE_MATCH_TRUNK     (0x00000004)
#define _BCM_PETRA_TRAVERSE_MATCH_VLAN      (0x00000008)
#define _BCM_PETRA_TRAVERSE_ACTION_DEL      (0x00000010)
#define _BCM_PETRA_TRAVERSE_ACTION_REPLACE  (0x00000020)
#define _BCM_PETRA_TRAVERSE_ACTION_NEW_TRUNK  (0x00000040)

#define _BCM_PETRA_TRAVERSE_MATCH_IGNORE_DISCARD_SRC    (0x00000080)
#define _BCM_PETRA_TRAVERSE_ACTION_DISCARD_SRC_SET      (0x00000100)
#define _BCM_PETRA_TRAVERSE_ACTION_DISCARD_SRC_CLEAR    (0x00000200)
#define _BCM_PETRA_TRAVERSE_MATCH_IGNORE_DES_HIT        (0x00000400)
#define _BCM_PETRA_TRAVERSE_ACTION_DES_HIT_CLEAR        (0x00000800)
#define _BCM_PETRA_TRAVERSE_MATCH_PROTECTION_RING       (0x00001000)    /* Match according to a FEC */
#define _BCM_PETRA_TRAVERSE_ACTION_MIGRATE_PORT         (0x00002000)    /* update destination */
#define _BCM_PETRA_TRAVERSE_ACTION_DYNAMIC_SET          (0x00004000)    /* update entries to be dynamic */
#define _BCM_PETRA_TRAVERSE_ACTION_DYNAMIC_CLEAR        (0x00008000)    /* update entries to be static */
#define _BCM_PETRA_TRAVERSE_MATCH_AGE                   (0x00010000)    /* Match according to entry age */


/* traverse entry by entry for match rule and delete the entries from the SW shadow table*/
STATIC int _remove_sw_mact_entries(int unit,
                                   SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE *rule,
                                   SOC_PPC_FRWRD_MACT_TABLE_TYPE table_type)
{
    SOC_SAND_TABLE_BLOCK_RANGE block_range;
    bcm_l2_addr_t l2addr;
    int indx;
    uint32 nof_entries;
    int rv = 0;
    uint32 soc_sand_rv = SOC_SAND_OK;
    
    BCMDNX_INIT_FUNC_DEFS;
    
    /* verify input parameters */
    BCM_DPP_UNIT_CHECK(unit);
    _DPP_L2_INIT_CHECK(unit);
    
    /* Check if the SW shadow is enabled */
    if (!SOC_DPP_IS_LEM_SIM_ENABLE(unit))
    {
        BCM_EXIT;
    }

    soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&block_range);

    rule->key_type = SOC_PPC_FRWRD_MACT_KEY_TYPE_MAC_ADDR;

    SOC_SAND_TBL_ITER_SET_BEGIN(&(block_range.iter));
    block_range.entries_to_act = _BCM_PETRA_L2_TRVRS_ITER_BLK_SIZE;
    block_range.entries_to_scan = SOC_SAND_TBL_ITER_SCAN_ALL;
    while(!SOC_SAND_TBL_ITER_IS_END(&(block_range.iter))) {
        soc_sand_rv = soc_ppd_frwrd_mact_get_block(unit,rule, table_type,&block_range,_bcm_l2_traverse_mact_keys[unit],_bcm_l2_traverse_mact_vals[unit],&nof_entries);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        if(nof_entries == 0) {
            break;
        }

        /* Delete the block of found entries from the shadow SW mact */
        for (indx = 0; indx < nof_entries; ++indx) {
            rv = _bcm_petra_l2_from_petra_entry(unit,&l2addr, FALSE, &(_bcm_l2_traverse_mact_keys[unit][indx]),&(_bcm_l2_traverse_mact_vals[unit][indx]));
            BCMDNX_IF_ERR_EXIT(rv);

            soc_sand_rv = soc_ppd_frwrd_mact_entry_remove(unit,&(_bcm_l2_traverse_mact_keys[unit][indx]), 1);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }

exit:
    if (rv!=BCM_E_NONE || soc_sand_get_error_code_from_error_word(soc_sand_rv)!=SOC_SAND_OK) 
    {
        arad_pp_frwrd_mact_clear_flush_operation(unit);
    }

    BCMDNX_FUNC_RETURN;
}

/* internal function to perform action on match entries*/
/* used for
 *  - l2_replace        (if flags & _BCM_PETRA_TRAVERSE_ACTION_REPLACE)
 *  - delete_by         (if flags & _BCM_PETRA_TRAVERSE_ACTION_DEL)
 *  - traverse          (if trav_fn is not null)
 *  - traverse-matched, (if trvrs-flags, indicate some checks)
 */
STATIC int
bcm_petra_l2_addr_traverse_by_rule(     
                                   int unit,
                                   bcm_l2_addr_t  *match_l2addr,
                                   uint32 flags,
                                   uint32 trvrs_flags,
                                   bcm_l2_addr_t  *match_mask_l2addr,
                                   bcm_l2_addr_t  *replace_l2addr,
                                   bcm_l2_addr_t  *replace_mask_l2addr,
                                   int         *restore,
                                   bcm_l2_traverse_cb trav_fn,
                                   void *user_data
                                        )
{
    SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE  rule;
    SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION      action;
    SOC_PPC_FRWRD_MACT_TABLE_TYPE table_type;
    uint8 wait_till_finish = TRUE;
    bcm_module_t new_module;
    bcm_port_t new_port;
    bcm_trunk_t new_trunk;
    uint32 nof_matches;
    uint8 hw_acc=TRUE;
    uint8 is_freezed;
    int rv = BCM_E_NONE;

    uint32 soc_sand_rv;
    bcm_module_t    _mymodid = 0, modid;
    int             core =0;
    soc_port_t      local_port, mod_port;
#ifdef BCM_CMICM_SUPPORT
    uint8 restore_arm = FALSE;
    uint8 reply_fifo_enable = FALSE;
    uint8 data_len = 0;
    shr_llm_msg_pon_att_l2_entry_t l2_entry;
    SHR_LLM_SERVICE_TYPE serv_type;
#endif
    BCMDNX_INIT_FUNC_DEFS;
    unit = (unit);

    *restore = 0;

    /* verify input parameters */
    BCM_DPP_UNIT_CHECK(unit);
    _DPP_L2_INIT_CHECK(unit);
    BCMDNX_NULL_CHECK(match_l2addr);
    BCMDNX_NULL_CHECK(restore);

    rv = bcm_petra_stk_my_modid_get(unit, &_mymodid);
    BCMDNX_IF_ERR_EXIT(rv);

    if (replace_l2addr != NULL) {
        new_module = replace_l2addr->modid; 
        new_port = replace_l2addr->port;
        new_trunk = replace_l2addr->tgid;
    }
    else {
        new_module = 0;
        new_port = 0;
        new_trunk = 0;
    }

    /* pending entries are not supported*/
    if(flags & BCM_L2_DELETE_PENDING){
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_L2_DELETE_PENDING flag not supported")));
    }
    if (trvrs_flags & _BCM_PETRA_TRAVERSE_MATCH_VLAN) {
        rv = _bcm_dpp_vlan_check(unit, match_l2addr->vid);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    if (trvrs_flags & _BCM_PETRA_TRAVERSE_ACTION_NEW_TRUNK) {
        if (new_trunk < 0 || (new_trunk > BCM_DPP_MAX_TRUNK_ID(unit))) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("New Trunk ID out of range")));
        }
    }

    if (trvrs_flags & _BCM_PETRA_TRAVERSE_MATCH_TRUNK) {
        if (match_l2addr->tgid < 0 || (match_l2addr->tgid > BCM_DPP_MAX_TRUNK_ID(unit))) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Match Trunk ID out of range")));
        }
    }

    /* if MACT not freezed, freeze it */
    BCMDNX_IF_ERR_EXIT(_bcm_petra_l2_is_freezed(unit, &is_freezed));
    if(!is_freezed)
    {
        rv = bcm_petra_l2_addr_freeze(unit);
        BCMDNX_IF_ERR_EXIT(rv);

        *restore = 1;
    }

    /* set rule for traverse */
    SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE_clear(&rule);
    SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_clear(&action);
    if (match_l2addr->flags & BCM_L2_SRC_HIT) {
        if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ipmc_l2_ssm_mode_lem", 0)) {
            rule.key_type = SOC_PPC_FRWRD_MACT_KEY_TYPE_IPV6_MC_SSM_EUI;
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_L2_SRC_HIT is only supported when soc property custom_feature_ipmc_l2_ssm_mode_lem=1")));
        }
    } else {
         rule.key_type = SOC_PPC_FRWRD_MACT_KEY_TYPE_MAC_ADDR;
    }


    /* if masked match supply use val-mask for high resolution of selection */
    if (match_mask_l2addr == NULL) {
        /* rule.value_rule.compare_mask = SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_USE_MASK;*/
    }
    /* MAC in rule*/
    if (trvrs_flags & _BCM_PETRA_TRAVERSE_MATCH_MAC) {
        rv = _bcm_petra_mac_to_sand_mac(match_l2addr->mac,&(rule.key_rule.mac.mac));
        BCMDNX_IF_ERR_EXIT(rv);
        /* consider all mac*/

        if (match_mask_l2addr == NULL) {
            soc_sand_rv = soc_sand_pp_mac_address_string_parse("FFFFFFFFFFFF", &(rule.key_rule.mac.mac_mask));
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
        else { /* use mask if available */
            rv = _bcm_petra_mac_to_sand_mac(match_mask_l2addr->mac,&(rule.key_rule.mac.mac_mask));
            BCMDNX_IF_ERR_EXIT(rv);
        }
        /* MAC address cannot be part of HW rule, so do it manually*/
        hw_acc = FALSE;
    }
    /* Age status in rule */
    if (trvrs_flags & _BCM_PETRA_TRAVERSE_MATCH_AGE) {
        rule.value_rule.val.aging_info.age_status = match_l2addr->age_state;
        hw_acc = FALSE;
    }
    else {
        rule.value_rule.val.aging_info.age_status = _BCM_PETRA_L2_ILLEGAL_AGE_STATUS;
    }
    /* VLAN in rule*/
    if (trvrs_flags & _BCM_PETRA_TRAVERSE_MATCH_VLAN) {
        rule.key_rule.mac.fid = match_l2addr->vid;
        rule.key_rule.mac.fid_mask = (match_mask_l2addr == NULL)?0x7FFF:match_mask_l2addr->vid;
    }
    /* TRUNK in rule*/
    if (trvrs_flags & _BCM_PETRA_TRAVERSE_MATCH_TRUNK) {
        SOC_PPD_FRWRD_DECISION_LAG_SET(unit, &(rule.value_rule.val.frwrd_info.forward_decision), match_l2addr->tgid, soc_sand_rv);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        rule.value_rule.compare_mask |= SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DEST_VAL | SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DEST_TYPE;
        /* mask trunk */
        if (match_mask_l2addr != NULL) {
            rule.value_rule.val_mask.frwrd_info.forward_decision.dest_id = match_mask_l2addr->tgid;
        }
        else {
            rule.value_rule.val_mask.frwrd_info.forward_decision.dest_id = 0xffff;
        } 
    }
    /* port/gport in rule, has to be after trunk */
    if (trvrs_flags & _BCM_PETRA_TRAVERSE_MATCH_PORT) {

        if (match_l2addr->encap_id != BCM_FORWARD_ENCAP_ID_INVALID) {
            if ((match_l2addr->flags & BCM_L2_SRC_HIT) && (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ipmc_l2_ssm_mode_lem", 0))) {
                if (BCM_GPORT_IS_FORWARD_PORT(match_l2addr->port)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("FEC is not supported with BCM_L2_SRC_HIT")));
                }
                else {
                    /* gport is just physical gport. map it using generic APi */
                    rv = _bcm_dpp_gport_to_fwd_decision(unit,match_l2addr->port,&(rule.value_rule.val.frwrd_info.forward_decision));
                    BCMDNX_IF_ERR_EXIT(rv);
                }
                if (BCM_FORWARD_ENCAP_ID_IS_OUTLIF(match_l2addr->encap_id)) {
                    rule.value_rule.val.frwrd_info.forward_decision.additional_info.outlif.type = SOC_PPC_OUTLIF_ENCODE_TYPE_RAW;
                    rule.value_rule.val.frwrd_info.forward_decision.additional_info.outlif.val = BCM_FORWARD_ENCAP_ID_VAL_GET(match_l2addr->encap_id);
                } else {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("encap_id must be OUTLIF")));
                }
            } else {
                rv = _bcm_dpp_port_encap_to_fwd_decision(unit, match_l2addr->port, match_l2addr->encap_id, &(rule.value_rule.val.frwrd_info.forward_decision));
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }
        else {
            local_port = match_l2addr->port;

            if(BCM_GPORT_IS_MODPORT(match_l2addr->port)) {
               modid = BCM_GPORT_MODPORT_MODID_GET(match_l2addr->port);
               mod_port = BCM_GPORT_MODPORT_PORT_GET(match_l2addr->port);

               if(SOC_DPP_IS_MODID_AND_BASE_MODID_ON_SAME_FAP(unit, modid, _mymodid)){ /* mod-port*/
   
                    core = SOC_DPP_MODID_TO_CORE(unit, _mymodid, modid);
   
                    rv = soc_port_sw_db_tm_to_local_port_get(unit, core, mod_port, (soc_port_t *)&local_port);
                    BCMDNX_IF_ERR_EXIT(rv);
               } 
            }
            rv = _bcm_dpp_gport_to_fwd_decision(unit,local_port, &(rule.value_rule.val.frwrd_info.forward_decision));
            BCMDNX_IF_ERR_EXIT(rv);
        }
        rule.value_rule.compare_mask |= (SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DEST_VAL | SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DEST_TYPE);
        /* set if additional info is set */
        if (rule.value_rule.val.frwrd_info.forward_decision.additional_info.outlif.type != SOC_PPC_OUTLIF_ENCODE_TYPE_NONE ||
            rule.value_rule.val.frwrd_info.forward_decision.additional_info.eei.type != SOC_PPC_EEI_TYPE_EMPTY) {
            rule.value_rule.compare_mask |= SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_ADDITIONAL_INFO;
        }
        /* set masking info if available */
        if (match_mask_l2addr != NULL) {
            /* encap id mask should be used in case it's different than 0 */
            if (match_mask_l2addr->encap_id != 0) {
                if ((match_mask_l2addr->encap_id & 0xFC000000) != 0) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("encap_id mask must be 26 bit width.")));
                }
                if (rule.value_rule.val.frwrd_info.forward_decision.additional_info.outlif.type != SOC_PPC_OUTLIF_ENCODE_TYPE_NONE){
                    rule.value_rule.val_mask.frwrd_info.forward_decision.additional_info.outlif.val = match_mask_l2addr->encap_id; 
                    rule.value_rule.val_mask.frwrd_info.forward_decision.dest_id = match_mask_l2addr->port; 
                }
                else if (rule.value_rule.val.frwrd_info.forward_decision.additional_info.eei.type == SOC_PPC_EEI_TYPE_MPLS){
                    rule.value_rule.val_mask.frwrd_info.forward_decision.additional_info.eei.val.outlif = match_mask_l2addr->encap_id; 
                    rule.value_rule.val_mask.frwrd_info.forward_decision.dest_id = match_mask_l2addr->port;               
                }
                else if (rule.value_rule.val.frwrd_info.forward_decision.additional_info.eei.type != SOC_PPC_EEI_TYPE_EMPTY){
                    rule.value_rule.val_mask.frwrd_info.forward_decision.additional_info.eei.val.isid = match_mask_l2addr->encap_id; 
                    rule.value_rule.val_mask.frwrd_info.forward_decision.dest_id = match_mask_l2addr->port; 
                }
            }
            /* if additional info exist (EEI/outlif) mask.port set EEI/outlif*/
            else if (rule.value_rule.val.frwrd_info.forward_decision.additional_info.outlif.type != SOC_PPC_OUTLIF_ENCODE_TYPE_NONE){
                rule.value_rule.val_mask.frwrd_info.forward_decision.additional_info.outlif.val = match_mask_l2addr->port; 
                rule.value_rule.val_mask.frwrd_info.forward_decision.dest_id = match_mask_l2addr->tgid; 
            }
            else if (rule.value_rule.val.frwrd_info.forward_decision.additional_info.eei.type == SOC_PPC_EEI_TYPE_MPLS){
                rule.value_rule.val_mask.frwrd_info.forward_decision.additional_info.eei.val.outlif = match_mask_l2addr->port; 
                rule.value_rule.val_mask.frwrd_info.forward_decision.dest_id = match_mask_l2addr->tgid;               
            }
            else if (rule.value_rule.val.frwrd_info.forward_decision.additional_info.eei.type != SOC_PPC_EEI_TYPE_EMPTY){
                rule.value_rule.val_mask.frwrd_info.forward_decision.additional_info.eei.val.isid = match_mask_l2addr->port; 
                rule.value_rule.val_mask.frwrd_info.forward_decision.dest_id = match_mask_l2addr->tgid; 
            }
            else {
                rule.value_rule.val_mask.frwrd_info.forward_decision.dest_id = match_mask_l2addr->port; 
            }
        }
        else{

            rule.value_rule.val_mask.frwrd_info.forward_decision.dest_id = 0xffffff; 
            if (rule.value_rule.val.frwrd_info.forward_decision.additional_info.outlif.type != SOC_PPC_OUTLIF_ENCODE_TYPE_NONE){
                rule.value_rule.val_mask.frwrd_info.forward_decision.additional_info.outlif.val = 0xffffff; 
                
            }
            else if (rule.value_rule.val.frwrd_info.forward_decision.additional_info.eei.type == SOC_PPC_EEI_TYPE_MPLS) {              
                rule.value_rule.val_mask.frwrd_info.forward_decision.additional_info.eei.val.outlif = 0xffffff;
            }
            else if (rule.value_rule.val.frwrd_info.forward_decision.additional_info.eei.type != SOC_PPC_EEI_TYPE_EMPTY){
                rule.value_rule.val_mask.frwrd_info.forward_decision.additional_info.eei.val.isid = 0xffffff;
            }
        }


    }
    /* Protection Ring match is done for a FEC value with no regard to the Outlif. The FEC is located in the same
       location as the physical port destination is located in Port match */
    if (trvrs_flags & _BCM_PETRA_TRAVERSE_MATCH_PROTECTION_RING) {
        rv = _bcm_dpp_gport_to_fwd_decision(unit,match_l2addr->port, &(rule.value_rule.val.frwrd_info.forward_decision));
        BCMDNX_IF_ERR_EXIT(rv);
        rule.value_rule.compare_mask |= (SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DEST_VAL | SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DEST_TYPE);
        if (match_mask_l2addr != NULL) {
            rule.value_rule.val_mask.frwrd_info.forward_decision.dest_id = match_mask_l2addr->port;
        }
        else {
            rule.value_rule.val_mask.frwrd_info.forward_decision.dest_id = 0xffffff;
        }
    }

    /* consider flags into rule */
    /* dynamic logic:
       flags & BCM_L2_REPLACE_MATCH_STATIC == 0 && match_addr->flags & BCM_L2_STATIC == 0 --> match dynamic entry only
       flags & BCM_L2_REPLACE_MATCH_STATIC == 0 && match_addr->flags & BCM_L2_STATIC == 1 -->match static entry only
       flags & BCM_L2_REPLACE_MATCH_STATIC == 1 -->match both static and dynamic entries
    */
    /* if dynamic only, then mark it, otherwise is_dynamic is not part of the compare rule*/
    if ((flags & BCM_L2_DELETE_STATIC)) {/* consider both dynamic + static */
    }
    else if ((match_l2addr->flags & BCM_L2_STATIC) == 0) {/* just consider dynamic entries*/
        rule.value_rule.compare_mask |= SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_IS_DYNAMIC;
        rule.value_rule.val.aging_info.is_dynamic = TRUE;
    }
    else {/* just consider static entries*/
        rule.value_rule.compare_mask |= SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_IS_DYNAMIC;
        rule.value_rule.val.aging_info.is_dynamic = FALSE;
    }
    /* sa-discard in match */
    if ((trvrs_flags & _BCM_PETRA_TRAVERSE_MATCH_IGNORE_DISCARD_SRC)) {/* consider both SA-Discard 1/0*/
    }
    else if ((match_l2addr->flags & BCM_L2_DISCARD_SRC) == 0) {/* just consider SA-discard 0*/
        rule.value_rule.compare_mask |= SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DROP_SA;
        rule.value_rule.val.frwrd_info.drop_when_sa_is_known = FALSE;
    }
    else {/* just consider SA-discard 1*/
        rule.value_rule.compare_mask |= SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DROP_SA;
        rule.value_rule.val.frwrd_info.drop_when_sa_is_known = TRUE;
    }

    /* DES-hit in match */
    if ((trvrs_flags & _BCM_PETRA_TRAVERSE_MATCH_IGNORE_DES_HIT)) {/* consider both DES-hit 1/0*/
    }
    else if ((match_l2addr->flags & BCM_L2_DES_HIT) == 0) {/* just consider DES-hit 0*/
        rule.value_rule.compare_mask |= SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_ACCESSED;
        rule.value_rule.val.accessed = FALSE;
    }
    else if ((match_l2addr->flags & BCM_L2_DES_HIT)) {/* just consider DES-hit 1*/
        rule.value_rule.compare_mask |= SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_ACCESSED;
        rule.value_rule.val.accessed = TRUE;
    }

    /* group */
    /* group setting */
    if (match_l2addr->group != 0) {
            if (rule.value_rule.val.frwrd_info.forward_decision.additional_info.eei.type != SOC_PPC_EEI_TYPE_EMPTY){
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("group ID must be zero for given gport type")));
        }
            rule.value_rule.compare_mask |= SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_GROUP;
        rule.value_rule.val.group = match_l2addr->group;

        /* mask group */
        if (match_mask_l2addr != NULL) {
            rule.value_rule.val_mask.group = match_mask_l2addr->group;
        }
        else{
            rule.value_rule.val_mask.group = 0xf;
        }
    }

   
    if (trvrs_flags & _BCM_PETRA_TRAVERSE_ACTION_DEL) {
        /* set action to be remove */
        action.type = SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_REMOVE;
    }
    else if(trvrs_flags & _BCM_PETRA_TRAVERSE_ACTION_REPLACE) {
        /* set action to be update */
        action.type = SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_UPDATE;

        /* if replace destination with TRUNK*/
        if (trvrs_flags & _BCM_PETRA_TRAVERSE_ACTION_NEW_TRUNK) {
            if (new_trunk > BCM_DPP_MAX_TRUNK_ID(unit)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("New Trunk ID out of range")));
            }
            SOC_PPD_FRWRD_DECISION_LAG_SET(unit, &(action.updated_val.frwrd_info.forward_decision), new_trunk, soc_sand_rv);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            action.update_mask |= (SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DEST_VAL|SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DEST_VAL);
        }
        else if (trvrs_flags & _BCM_PETRA_TRAVERSE_ACTION_MIGRATE_PORT) {
            /* replace destination with port/gport*/
            /* use only new-port*/
            rv = _bcm_dpp_gport_to_fwd_decision(unit,new_port,&(action.updated_val.frwrd_info.forward_decision));
            BCMDNX_IF_ERR_EXIT(rv);
            action.update_mask |= (SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DEST_VAL|SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DEST_VAL|SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_ADDITIONAL_INFO);

            /* mask dest change */
            if (replace_mask_l2addr != NULL) {
                /* if additional info exist (EEI/outlif) mask.port set EEI/outlif*/
                if (action.updated_val.frwrd_info.forward_decision.additional_info.outlif.type != SOC_PPC_OUTLIF_ENCODE_TYPE_NONE){
                    action.updated_val_mask.frwrd_info.forward_decision.additional_info.outlif.val = replace_mask_l2addr->port; 
                    action.updated_val_mask.frwrd_info.forward_decision.dest_id = replace_mask_l2addr->tgid; 
                }
                else if (action.updated_val.frwrd_info.forward_decision.additional_info.eei.type != SOC_PPC_EEI_TYPE_EMPTY){
                    action.updated_val_mask.frwrd_info.forward_decision.additional_info.eei.val.isid = replace_mask_l2addr->port; 
                    action.updated_val_mask.frwrd_info.forward_decision.dest_id = replace_mask_l2addr->tgid; 
                }
                else {
                    action.updated_val_mask.frwrd_info.forward_decision.dest_id = replace_mask_l2addr->port; 
                }
            }
            else {
                action.updated_val_mask.frwrd_info.forward_decision.dest_id = 0xffffff;
                /* if additional info exist (EEI/outlif) mask.port set EEI/outlif*/
                if (action.updated_val.frwrd_info.forward_decision.additional_info.outlif.type != SOC_PPC_OUTLIF_ENCODE_TYPE_NONE){
                    action.updated_val_mask.frwrd_info.forward_decision.additional_info.outlif.val = 0xffffff;  
                }
                else if (action.updated_val.frwrd_info.forward_decision.additional_info.eei.type != SOC_PPC_EEI_TYPE_EMPTY){
                    action.updated_val_mask.frwrd_info.forward_decision.additional_info.eei.val.isid = 0xffffff;
                }
            }

        }
        /* sa-discard in action */
        if ((trvrs_flags & _BCM_PETRA_TRAVERSE_ACTION_DISCARD_SRC_SET)) {/* set SA-discard */
            action.update_mask |= SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DROP_SA;
            action.updated_val.frwrd_info.drop_when_sa_is_known = TRUE;
        }
        else if ((trvrs_flags & _BCM_PETRA_TRAVERSE_ACTION_DISCARD_SRC_CLEAR)) {/* reset SA-discard */
            action.update_mask |= SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_DROP_SA;
            action.updated_val.frwrd_info.drop_when_sa_is_known = FALSE;
        }

        /* DES-hit in action, only clear possible*/
        if ((trvrs_flags & _BCM_PETRA_TRAVERSE_ACTION_DES_HIT_CLEAR)) {/* set SA-discard */
            action.update_mask |= SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_ACCESSED;
        }
                
        /* group change setting */
        if (replace_l2addr != NULL && replace_l2addr->group != 0) {
                if (rule.value_rule.val.frwrd_info.forward_decision.additional_info.eei.type != SOC_PPC_EEI_TYPE_EMPTY){
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("group ID must be zero for given gport type")));
            }
                action.update_mask |= SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_GROUP;
            action.updated_val.group = replace_l2addr->group;
            /* mask group */
            if (replace_mask_l2addr != NULL) {
                action.updated_val_mask.group = replace_mask_l2addr->group;
            }
        }
    }

    /* if need callback in each entry then don't activate HW traverse*/
    if (!(flags & BCM_L2_DELETE_NO_CALLBACKS) || trav_fn != NULL) {
        hw_acc = FALSE;
    }
    
    /* Select mact type for the traverse. (SW or HW+SW)*/
    if (hw_acc || !SOC_DPP_IS_EM_HW_ENABLE) {
        table_type = SOC_PPC_FRWRD_MACT_TABLE_SW_ONLY; 
    }
    else
    {
        table_type = SOC_PPC_FRWRD_MACT_TABLE_SW_HW;
    }

    /* 
     * If update MACT table by FLUSH DB, send LLM request message to ARM CPU to 
     * disable MAC limiting function and enable read reply-FIFO for flush DB.
     * Otherwise, send LLM request message to ARM CPU to disable MAC limiting function
     * and disable read reply-FIFO for flush DB.
 */
    LLM_PER_TUNNEL_ENABLE_REPLY(unit);

    if (hw_acc) {
        /* activate traverse */
#ifdef BCM_CMICM_SUPPORT
        if (SOC_MAC_LIMIT_PER_TUNNEL_ENABLE_GET(unit)) {
         /* LCOV_EXCL_START */
            soc_sand_rv = soc_ppd_frwrd_mact_traverse_by_mac_limit_per_tunnel(unit, &rule, &action, &nof_matches);
        }
        else
#endif
        {
            soc_sand_rv = soc_ppd_frwrd_mact_traverse(unit, &rule, &action, wait_till_finish, &nof_matches);
        }
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* Run over the SW shadow mac table and delete the sw entries one by one */
        soc_sand_rv = _remove_sw_mact_entries(unit, &rule, SOC_PPC_FRWRD_MACT_TABLE_SW_ONLY);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* LCOV_EXCL_STOP */        
    }
    else /* do it one by one, not in ARAD this is get-by rule is hw-accelerated */
    {
        /* traverse entry by entry for match rule*/
        SOC_SAND_SUCCESS_FAILURE
            failure_indication = SOC_SAND_SUCCESS;
        SOC_SAND_TABLE_BLOCK_RANGE                  
            block_range;
        bcm_l2_addr_t 
            l2addr;
        int indx;
        uint32                                
            nof_entries = 0;
        SOC_PPC_FRWRD_MACT_ADD_TYPE add_type = SOC_PPC_FRWRD_MACT_ADD_TYPE_TRANSPLANT;
        
        soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&block_range);
    
        /* excluding IPMC compatible entries*/
        if (match_l2addr->flags & BCM_L2_SRC_HIT) {
            if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ipmc_l2_ssm_mode_lem", 0)) {
                rule.key_type = SOC_PPC_FRWRD_MACT_KEY_TYPE_IPV6_MC_SSM_EUI;
           }
        } else {
            rule.key_type = SOC_PPC_FRWRD_MACT_KEY_TYPE_MAC_ADDR;
        }
    
        SOC_SAND_TBL_ITER_SET_BEGIN(&(block_range.iter));
        block_range.entries_to_act = _BCM_PETRA_L2_TRVRS_ITER_BLK_SIZE;
        block_range.entries_to_scan = SOC_SAND_TBL_ITER_SCAN_ALL;
        while(!SOC_SAND_TBL_ITER_IS_END(&(block_range.iter))) {
            soc_sand_rv = soc_ppd_frwrd_mact_get_block(unit,&rule, table_type,&block_range,_bcm_l2_traverse_mact_keys[unit],_bcm_l2_traverse_mact_vals[unit],&nof_entries);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            if(nof_entries == 0) {
                break;
            }
            for (indx = 0; indx < nof_entries; ++indx) {
                /* act on entry*/
                /* call for call-back */
                if (trav_fn != NULL) {
                    /* map PPD key and value to BCM*/
                    rv = _bcm_petra_l2_from_petra_entry(unit,&l2addr, FALSE, &(_bcm_l2_traverse_mact_keys[unit][indx]),&(_bcm_l2_traverse_mact_vals[unit][indx]));
                    BCMDNX_IF_ERR_EXIT(rv);
                    rv = trav_fn(unit,&l2addr, user_data);
                    BCMDNX_IF_ERR_EXIT(rv);
                }

                /* delete*/
                if (trvrs_flags & _BCM_PETRA_TRAVERSE_ACTION_DEL) {
#ifdef BCM_CMICM_SUPPORT
                    rv = _bcm_petra_l2_from_petra_entry(unit,&l2addr, FALSE, &(_bcm_l2_traverse_mact_keys[unit][indx]),&(_bcm_l2_traverse_mact_vals[unit][indx]));
                    BCMDNX_IF_ERR_EXIT(rv);

                    if (SOC_MAC_LIMIT_PER_TUNNEL_ENABLE_GET(unit) && BCM_GPORT_IS_SET(l2addr.port)) {
                        /* LCOV_EXCL_START */
                        rv = _bcm_petra_l2_service_type_get(unit, l2addr.port, &serv_type);
                        BCMDNX_IF_ERR_EXIT(rv);
                        soc_sand_rv = soc_ppd_frwrd_mact_entry_pack(unit,
                                                                    FALSE,
                                                                    0,/* needn't care this variable in remove operation */
                                                                    &(_bcm_l2_traverse_mact_keys[unit][indx]),
                                                                    &(_bcm_l2_traverse_mact_vals[unit][indx]),
                                                                    l2_entry.entry,
                                                                    &data_len);
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                        rv = shr_llm_msg_pon_attr_set(unit, LLM_SERVICE_MAC_LIMIT, SHR_LLM_MSG_PON_ATT_UPDATE_MAC_ENTRY,&l2_entry);
                        BCMDNX_IF_ERR_EXIT(rv);
                        /* LCOV_EXCL_STOP */
                    }
                    else
#endif
                    {
                        soc_sand_rv = soc_ppd_frwrd_mact_entry_remove(unit,&(_bcm_l2_traverse_mact_keys[unit][indx]), 0);
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    }
                    /* invoke call back */
                    if (!(flags & BCM_L2_DELETE_NO_CALLBACKS)) {
                        /* map PPD key and value to BCM*/
                        rv = _bcm_petra_l2_from_petra_entry(unit,&l2addr, FALSE, &(_bcm_l2_traverse_mact_keys[unit][indx]),&(_bcm_l2_traverse_mact_vals[unit][indx]));
                        BCMDNX_IF_ERR_EXIT(rv);
                        _BCM_PETRA_L2_CB_RUN(unit,&l2addr, BCM_L2_CALLBACK_DELETE);
                    }

                }
                /* replace dest */
                if (trvrs_flags & _BCM_PETRA_TRAVERSE_ACTION_REPLACE) {
                    /* map PPD key and value to BCM*/
                    rv = _bcm_petra_l2_from_petra_entry(unit,&l2addr, FALSE, &(_bcm_l2_traverse_mact_keys[unit][indx]),&(_bcm_l2_traverse_mact_vals[unit][indx]));
                    BCMDNX_IF_ERR_EXIT(rv);
        
                    /* invoke call back */
                    if (!(flags & BCM_L2_DELETE_NO_CALLBACKS)) {
                        _BCM_PETRA_L2_CB_RUN(unit,&l2addr, BCM_L2_CALLBACK_DELETE);
                    }

                    /* update port/trunk*/
                    if (trvrs_flags & _BCM_PETRA_TRAVERSE_ACTION_NEW_TRUNK) {
                        l2addr.modid = new_module;
                        l2addr.tgid = new_trunk;
                        l2addr.flags |= BCM_L2_TRUNK_MEMBER;
                    }
                    else{
                        l2addr.modid = new_module;
                        l2addr.port = new_port;
                        l2addr.flags &= ~BCM_L2_TRUNK_MEMBER;
                    }
                    /* sa-discard in action */
                    if ((trvrs_flags & _BCM_PETRA_TRAVERSE_ACTION_DISCARD_SRC_SET)) {/* set SA-discard */
                        l2addr.flags |= BCM_L2_DISCARD_SRC;
                    }
                    else if ((trvrs_flags & _BCM_PETRA_TRAVERSE_ACTION_DISCARD_SRC_CLEAR)) {/* reset SA-discard */
                        l2addr.flags &= ~BCM_L2_DISCARD_SRC;
                    }
                    /* DES-hit in action, only clear possible*/
                    if ((trvrs_flags & _BCM_PETRA_TRAVERSE_ACTION_DES_HIT_CLEAR)) {/* set SA-discard */
                        l2addr.flags &= ~BCM_L2_DES_HIT;
                    }

                    if (trvrs_flags & _BCM_PETRA_TRAVERSE_ACTION_DYNAMIC_SET) { /* Set dynamic bit */
                        l2addr.flags &= ~BCM_L2_STATIC;
                        add_type = SOC_PPC_FRWRD_MACT_ADD_TYPE_INSERT;
                    }
                    else if (trvrs_flags & _BCM_PETRA_TRAVERSE_ACTION_DYNAMIC_CLEAR) { /* Unset dynamic bit */
                        l2addr.flags |= BCM_L2_STATIC;
                        add_type = SOC_PPC_FRWRD_MACT_ADD_TYPE_INSERT;
                    }
                    

                    /* map value back to PPD*/
                    rv = _bcm_petra_l2_to_petra_entry(unit,&l2addr,FALSE,&(_bcm_l2_traverse_mact_keys[unit][indx]),&(_bcm_l2_traverse_mact_vals[unit][indx]));
                    BCMDNX_IF_ERR_EXIT(rv);
        
                    /* add entry after update */
#ifdef BCM_CMICM_SUPPORT
                    rv = _bcm_petra_l2_from_petra_entry(unit,&l2addr, FALSE, &(_bcm_l2_traverse_mact_keys[unit][indx]),&(_bcm_l2_traverse_mact_vals[unit][indx]));
                    BCMDNX_IF_ERR_EXIT(rv);

                    if (SOC_MAC_LIMIT_PER_TUNNEL_ENABLE_GET(unit) && BCM_GPORT_IS_SET(l2addr.port)) {
                        /* LCOV_EXCL_START */
                        soc_sand_rv = soc_ppd_frwrd_mact_entry_pack(unit,
                                                                    TRUE,
                                                                    add_type,
                                                                    &(_bcm_l2_traverse_mact_keys[unit][indx]),
                                                                    &(_bcm_l2_traverse_mact_vals[unit][indx]),
                                                                    l2_entry.entry,
                                                                    &data_len);
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                        rv = _bcm_petra_l2_service_type_get(unit, l2addr.port, &serv_type);
                        BCMDNX_IF_ERR_EXIT(rv);
                        rv = shr_llm_msg_pon_attr_set(unit, serv_type, SHR_LLM_MSG_PON_ATT_UPDATE_MAC_ENTRY, &l2_entry);
                        BCMDNX_IF_ERR_EXIT(rv);
                        /* LCOV_EXCL_STOP */
                    }
                    else
#endif
                    {
                        soc_sand_rv = soc_ppd_frwrd_mact_entry_add(unit,add_type,&(_bcm_l2_traverse_mact_keys[unit][indx]),&(_bcm_l2_traverse_mact_vals[unit][indx]),&failure_indication);
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    }

                    SOC_SAND_IF_FAIL_RETURN(failure_indication);
                    /* Clear Hit Bit */
                    if ((trvrs_flags & _BCM_PETRA_TRAVERSE_ACTION_DES_HIT_CLEAR)) {
                        SOC_SAND_PP_MAC_ADDRESS mac_address_key;
                        /* convert BCM mac to PPD mac type */
                        rv = _bcm_petra_mac_to_sand_mac(l2addr.mac, &mac_address_key);
                        BCM_IF_ERROR_RETURN(rv);
                        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_frwrd_mact_clear_access_bit, (unit, l2addr.vid,&mac_address_key));
                        BCMDNX_IF_ERR_EXIT(rv);
                    }
                    /* invoke call back */
                    if (!(flags & BCM_L2_DELETE_NO_CALLBACKS)) {
                        _BCM_PETRA_L2_CB_RUN(unit,&l2addr, BCM_L2_CALLBACK_ADD);
                    }

                }
            }
        }
    }

    BCMDNX_IF_ERR_EXIT(rv);

exit:
    if (_rv != BCM_E_NONE) 
    {
        arad_pp_frwrd_mact_clear_flush_operation(unit);
    }

    LLM_PER_TUNNEL_ENABLE(unit);
    BCMDNX_FUNC_RETURN;
}


int
bcm_petra_l2_addr_delete_by_mac(
                                int unit,
                                bcm_mac_t mac,
                                uint32 flags)
{
    bcm_l2_addr_t  match_l2addr;
    uint32 trvrs_flag;
    int         restore;
    int rv = BCM_E_NONE;
    int restore_rv = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS
    BCM_DPP_UNIT_CHECK(unit);
    _DPP_L2_INIT_CHECK(unit);
    /* set relevant fields in l2addr*/
    bcm_l2_addr_t_init(&match_l2addr,mac,0);

    /* set flags to remove according to port+mac */
    trvrs_flag = _BCM_PETRA_TRAVERSE_MATCH_MAC|_BCM_PETRA_TRAVERSE_ACTION_DEL;

    /* ignore hits and SA discard status for ARAD */
    trvrs_flag |= _BCM_PETRA_TRAVERSE_MATCH_IGNORE_DES_HIT|_BCM_PETRA_TRAVERSE_MATCH_IGNORE_DISCARD_SRC;

    rv = bcm_petra_l2_addr_traverse_by_rule(unit,&match_l2addr,flags,trvrs_flag,NULL,NULL,NULL,&restore,NULL,NULL);

    /* if need to restore aging/learning do it*/
    if(restore) {
        restore_rv = bcm_petra_l2_addr_thaw(unit);
        BCMDNX_IF_ERR_EXIT(restore_rv);
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_l2_addr_delete_by_mac_port(
                                     int unit,
                                     bcm_mac_t mac,
                                     bcm_module_t mod,
                                     bcm_port_t port,
                                     uint32 flags)
{
    bcm_l2_addr_t  match_l2addr;
    uint32 trvrs_flag;
    int         restore;
    int rv = BCM_E_NONE;
    int restore_rv = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _DPP_L2_INIT_CHECK(unit);
    /* set relevant fields in l2addr*/
    bcm_l2_addr_t_init(&match_l2addr,mac,0);
    match_l2addr.modid = mod;
    match_l2addr.port = port;

    /* set flags to remove according to port+mac */
    trvrs_flag = _BCM_PETRA_TRAVERSE_MATCH_PORT|_BCM_PETRA_TRAVERSE_MATCH_MAC|_BCM_PETRA_TRAVERSE_ACTION_DEL;

    /* ignore hits and SA discard status for ARAD */
    trvrs_flag |= _BCM_PETRA_TRAVERSE_MATCH_IGNORE_DES_HIT|_BCM_PETRA_TRAVERSE_MATCH_IGNORE_DISCARD_SRC;

    rv = bcm_petra_l2_addr_traverse_by_rule(unit,&match_l2addr,flags,trvrs_flag,NULL,NULL,NULL,&restore,NULL,NULL);

    /* if need to restore aging/learning do it*/
    if(restore) {
        restore_rv = bcm_petra_l2_addr_thaw(unit);
        BCMDNX_IF_ERR_EXIT(restore_rv);
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_l2_addr_delete_by_port(
                                 int unit,
                                 bcm_module_t mod,
                                 bcm_port_t port,
                                 uint32 flags)
{
    bcm_l2_addr_t  match_l2addr;
    uint32 trvrs_flag;
    bcm_mac_t mac;
    int         restore;
    int rv = BCM_E_NONE;
    int restore_rv = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _DPP_L2_INIT_CHECK(unit);

    /* set relevant fields in l2addr*/
    sal_memset(mac, 0x0, sizeof(bcm_mac_t));
    bcm_l2_addr_t_init(&match_l2addr,mac,0);
    match_l2addr.modid = mod;
    match_l2addr.port = port;

    /* set flags to remove according to port */
    trvrs_flag = _BCM_PETRA_TRAVERSE_MATCH_PORT|_BCM_PETRA_TRAVERSE_ACTION_DEL;

    /* ignore hits and SA discard status for ARAD */
    trvrs_flag |= _BCM_PETRA_TRAVERSE_MATCH_IGNORE_DES_HIT|_BCM_PETRA_TRAVERSE_MATCH_IGNORE_DISCARD_SRC;

    rv = bcm_petra_l2_addr_traverse_by_rule(unit,&match_l2addr,flags,trvrs_flag,NULL,NULL,NULL,&restore,NULL,NULL);

    /* if need to restore aging/learning do it*/
    if(restore) {
        restore_rv = bcm_petra_l2_addr_thaw(unit);
        BCMDNX_IF_ERR_EXIT(restore_rv);
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_l2_addr_delete_by_trunk(
                                  int unit,
                                  bcm_trunk_t tid,
                                  uint32 flags)
{
    bcm_l2_addr_t  match_l2addr;
    uint32 trvrs_flag;
    int         restore;
    bcm_mac_t mac;
    int rv = BCM_E_NONE;
    int restore_rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _DPP_L2_INIT_CHECK(unit);

    /* set relevant fields in l2addr*/
    sal_memset(mac, 0x0, sizeof(bcm_mac_t));
    bcm_l2_addr_t_init(&match_l2addr,mac,0);
    match_l2addr.tgid = tid;

    /* set flags to remove according to port+mac */
    trvrs_flag = _BCM_PETRA_TRAVERSE_MATCH_TRUNK|_BCM_PETRA_TRAVERSE_ACTION_DEL;

    /* ignore hits and SA discard status for ARAD */
    trvrs_flag |= _BCM_PETRA_TRAVERSE_MATCH_IGNORE_DES_HIT|_BCM_PETRA_TRAVERSE_MATCH_IGNORE_DISCARD_SRC;

    rv = bcm_petra_l2_addr_traverse_by_rule(unit,&match_l2addr,flags,trvrs_flag,NULL,NULL,NULL,&restore,NULL,NULL);

    /* if need to restore aging/learning do it*/
    if(restore) {
        restore_rv = bcm_petra_l2_addr_thaw(unit);
        BCMDNX_IF_ERR_EXIT(restore_rv);
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}
int 
bcm_petra_l2_addr_delete_by_vlan(
                                 int unit, 
                                 bcm_vlan_t vid,
                                 uint32 flags)
{
    bcm_l2_addr_t  match_l2addr;
    uint32 trvrs_flag;
    int         restore;
    bcm_mac_t mac;
    int rv = BCM_E_NONE;
    int restore_rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _DPP_L2_INIT_CHECK(unit);


    sal_memset(mac, 0x0, sizeof(bcm_mac_t));
    /* set relevant fields in l2addr*/
    bcm_l2_addr_t_init(&match_l2addr,mac,vid);

    /* set flags to remove according to vlan */
    trvrs_flag = _BCM_PETRA_TRAVERSE_MATCH_VLAN|_BCM_PETRA_TRAVERSE_ACTION_DEL;

    /* ignore hits and SA discard status for ARAD */
    trvrs_flag |= _BCM_PETRA_TRAVERSE_MATCH_IGNORE_DES_HIT|_BCM_PETRA_TRAVERSE_MATCH_IGNORE_DISCARD_SRC;

    rv = bcm_petra_l2_addr_traverse_by_rule(unit,&match_l2addr,flags,trvrs_flag,NULL,NULL,NULL,&restore,NULL,NULL);

    /* if need to restore aging/learning do it*/
    if(restore) {
        restore_rv = bcm_petra_l2_addr_thaw(unit);
        BCMDNX_IF_ERR_EXIT(restore_rv);
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}
    
int 
bcm_petra_l2_addr_delete_by_vlan_port(
                                      int unit, 
                                      bcm_vlan_t vid,
                                      bcm_module_t mod,
                                      bcm_port_t port,
                                      uint32 flags)
{
    bcm_l2_addr_t  match_l2addr;
    uint32 trvrs_flag;
    bcm_mac_t mac;
    int         restore;
    int rv = BCM_E_NONE;
    int restore_rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _DPP_L2_INIT_CHECK(unit);
    /* set relevant fields in l2addr*/
    sal_memset(mac, 0x0, sizeof(bcm_mac_t));
    bcm_l2_addr_t_init(&match_l2addr,mac,vid);
    match_l2addr.modid = mod;
    match_l2addr.port = port;

    /* set flags to remove according to port+mac */
    trvrs_flag = _BCM_PETRA_TRAVERSE_MATCH_VLAN|_BCM_PETRA_TRAVERSE_MATCH_PORT|_BCM_PETRA_TRAVERSE_ACTION_DEL;

    /* ignore hits and SA discard status for ARAD */
    trvrs_flag |= _BCM_PETRA_TRAVERSE_MATCH_IGNORE_DES_HIT|_BCM_PETRA_TRAVERSE_MATCH_IGNORE_DISCARD_SRC;

    rv = bcm_petra_l2_addr_traverse_by_rule(unit,&match_l2addr,flags,trvrs_flag,NULL,NULL,NULL,&restore,NULL,NULL);

    /* if need to restore aging/learning do it*/
    if(restore) {
        restore_rv = bcm_petra_l2_addr_thaw(unit);
        BCMDNX_IF_ERR_EXIT(restore_rv);
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_l2_addr_delete_by_vlan_trunk(
                                       int unit,
                                       bcm_vlan_t vid,
                                       bcm_trunk_t tid,
                                       uint32 flags)
{
    bcm_l2_addr_t  match_l2addr;
    uint32 trvrs_flag;
    bcm_mac_t mac;
    int         restore;
    int rv = BCM_E_NONE;
    int restore_rv = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _DPP_L2_INIT_CHECK(unit);
    /* set relevant fields in l2addr*/
    sal_memset(mac, 0x0, sizeof(bcm_mac_t));
    bcm_l2_addr_t_init(&match_l2addr,mac,vid);
    match_l2addr.tgid = tid;

    /* set flags to remove according to port+mac */
    trvrs_flag = _BCM_PETRA_TRAVERSE_MATCH_VLAN|_BCM_PETRA_TRAVERSE_MATCH_TRUNK|_BCM_PETRA_TRAVERSE_ACTION_DEL;

    /* ignore hits and SA discard status for ARAD */
    trvrs_flag |= _BCM_PETRA_TRAVERSE_MATCH_IGNORE_DES_HIT|_BCM_PETRA_TRAVERSE_MATCH_IGNORE_DISCARD_SRC;

    rv = bcm_petra_l2_addr_traverse_by_rule(unit,&match_l2addr,flags,trvrs_flag,NULL,NULL,NULL,&restore,NULL,NULL);

    /* if need to restore aging/learning do it*/
    if(restore) {
        restore_rv = bcm_petra_l2_addr_thaw(unit);
        BCMDNX_IF_ERR_EXIT(restore_rv);
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_l2_addr_by_struct_delete(
                                   int unit,
                                   bcm_l2_addr_t *l2addr)
{
    int rv = BCM_E_NONE;
    uint32 soc_sand_rv;
    uint32 soc_ppd_port;
    int local_port, core;
    uint8 found;
    _bcm_dpp_gport_info_t gport_info;
    SOC_PPC_BMACT_ENTRY_KEY l2_entry_key;
    SOC_PPC_BMACT_ENTRY_INFO l2_entry_value;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _DPP_L2_INIT_CHECK(unit);

    if (!(l2addr->flags2 & BCM_L2_FLAGS2_BMACT_LEARN)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("l2_addr_by_struct_delete only supported for MIM Learn entries (BCM_L2_FLAGS2_BMACT_LEARN)")));
    }

    rv = _bcm_dpp_gport_to_phy_port(unit, l2addr->modid, 0, &gport_info);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_PBMP_ITER(gport_info.pbmp_local_ports, local_port) {
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, local_port, &soc_ppd_port, &core)));

        /* remove B-MACT entry */
        SOC_PPC_BMACT_ENTRY_KEY_clear(&l2_entry_key);
        SOC_PPC_BMACT_ENTRY_INFO_clear(&l2_entry_value);

        rv = _bcm_petra_mac_to_sand_mac(l2addr->mac, &(l2_entry_key.b_mac_addr));
            BCMDNX_IF_ERR_EXIT(rv);
        l2_entry_key.b_vid = _BCM_DPP_BVID_TO_VLAN(l2addr->vid);

        l2_entry_key.flags |= SOC_PPC_BMACT_ENTRY_TYPE_LEARN;
        l2_entry_key.local_port_ndx = soc_ppd_port;

        if (SOC_IS_JERICHO(unit) && !SOC_IS_QAX(unit)) {
            l2_entry_key.core = core;
        }

        soc_sand_rv = soc_ppd_frwrd_bmact_entry_get(unit, &l2_entry_key, &l2_entry_value, &found);
        if (!found) {
            LOG_VERBOSE(BSL_LS_BCM_L2,
                    (BSL_META_U(unit,
                                "bcm_l2_addr_get address not found\n")));
            BCMDNX_IF_ERR_EXIT(BCM_E_NOT_FOUND);
        }

        soc_sand_rv = soc_ppd_frwrd_bmact_entry_remove(unit, &l2_entry_key);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


int
bcm_petra_l2_addr_by_struct_get(
                                   int unit,
                                   bcm_l2_addr_t *l2addr)
{
    int rv = BCM_E_NONE;
    uint32 soc_sand_rv;
    uint32 soc_ppd_port;
    int local_port, core;
    _bcm_dpp_gport_info_t gport_info;
    uint8 found;
    SOC_PPC_BMACT_ENTRY_KEY l2_entry_key;
    SOC_PPC_BMACT_ENTRY_INFO l2_entry_value;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _DPP_L2_INIT_CHECK(unit);

    if (!(l2addr->flags2 & BCM_L2_FLAGS2_BMACT_LEARN)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("l2_addr_by_struct_get only supported for MIM Learn entries (BCM_L2_FLAGS2_BMACT_LEARN)")));
    }

    rv = _bcm_dpp_gport_to_phy_port(unit, l2addr->modid, 0, &gport_info);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_PBMP_ITER(gport_info.pbmp_local_ports, local_port) {
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, local_port, &soc_ppd_port, &core)));

        /* get B-MACT entry */
        SOC_PPC_BMACT_ENTRY_KEY_clear(&l2_entry_key);
        SOC_PPC_BMACT_ENTRY_INFO_clear(&l2_entry_value);

        rv = _bcm_petra_mac_to_sand_mac(l2addr->mac, &(l2_entry_key.b_mac_addr));
            BCMDNX_IF_ERR_EXIT(rv);
        l2_entry_key.b_vid = _BCM_DPP_BVID_TO_VLAN(l2addr->vid);

        l2_entry_key.flags |= SOC_PPC_BMACT_ENTRY_TYPE_LEARN;
        l2_entry_key.local_port_ndx = soc_ppd_port;

        if (SOC_IS_JERICHO(unit) && !SOC_IS_QAX(unit)) {
            l2_entry_key.core = core;
        }

        soc_sand_rv = soc_ppd_frwrd_bmact_entry_get(unit, &l2_entry_key, &l2_entry_value, &found);
        if (!found) {
            LOG_VERBOSE(BSL_LS_BCM_L2,
                    (BSL_META_U(unit,
                                "bcm_l2_addr_get address not found\n")));
            BCMDNX_IF_ERR_EXIT(BCM_E_NOT_FOUND);
        }

        l2addr->port = l2_entry_value.sa_learn_fec_id;

        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


int
bcm_petra_l2_addr_freeze(
                         int unit)
{
    int freeze_cnt;
    int rv = BCM_E_NONE;
    SOC_PPC_FRWRD_MACT_AGING_INFO  aging_info;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    _bcm_petra_l2_freeze_t freeze_info;

    BCMDNX_INIT_FUNC_DEFS;
    /* check parameters*/
    BCM_DPP_UNIT_CHECK(unit);
    _DPP_L2_INIT_CHECK(unit);

    soc_sand_dev_id = (unit);
    sal_memset(&freeze_info, 0x0, sizeof(_bcm_petra_l2_freeze_t));

    /* map BCM MACT key/entry to Soc_petra MACT key/entry*/
    rv = _bcm_petra_l2_freeze_inc(unit, &freeze_cnt);
    BCMDNX_IF_ERR_EXIT(rv);
    /* if this is first freeze, do the freeze*/
    if (freeze_cnt == 1) {
        /* store state before freeze*/
        SOC_PPC_FRWRD_MACT_AGING_INFO_clear(&aging_info);
        soc_sand_rv = soc_ppd_frwrd_mact_aging_info_get(soc_sand_dev_id,&aging_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        freeze_info.save_age_ena = aging_info.enable_aging;

        aging_info.enable_aging = FALSE;

        soc_sand_rv = soc_ppd_frwrd_mact_aging_info_set(soc_sand_dev_id,&aging_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        _bcm_petra_l2_freeze_info_set(unit,&freeze_info);
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}
 
int 
bcm_petra_l2_addr_get(
                      int unit, 
                      bcm_mac_t mac_addr,
                      bcm_vlan_t vid, 
                      bcm_l2_addr_t *l2addr)
{
    uint8
        found;
    bcm_l2_addr_t l2addr_find;
    int rv = BCM_E_NONE;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
#ifdef BCM_CMICM_SUPPORT
    uint8 restore_arm = FALSE;
#endif

    BCMDNX_INIT_FUNC_DEFS;
    /* check parameters*/
    BCM_DPP_UNIT_CHECK(unit);
    _DPP_L2_INIT_CHECK(unit);
    BCMDNX_NULL_CHECK(l2addr);
    /* check rest of parameters in internal conversion functions*/
    soc_sand_dev_id = (unit);

    LLM_PER_TUNNEL_DISABLE(unit);
    if (_BCM_DPP_VLAN_IS_BVID(unit, vid)) { /* vid is B-VID */

        SOC_PPC_BMACT_ENTRY_KEY l2_entry_key;
        SOC_PPC_BMACT_ENTRY_INFO l2_entry_val;

        MIM_INIT(unit);

        /* For Non Jericho devices remove 0xf000 so b-vid can be used as vid (Jericho uses BVID range of 28-32K so FID stays the same is done */
        if (!(SOC_IS_JERICHO_A0(unit) | SOC_IS_JERICHO_B0(unit) | SOC_IS_QMX(unit))) {
            vid = _BCM_DPP_BVID_TO_VLAN(vid);
        }

        bcm_l2_addr_t_init(&l2addr_find,mac_addr,vid);

        /* map BCM MACT key/entry to Soc_petra MACT key/entry*/
        rv = _bcm_petra_l2_to_petra_bmact_entry(unit, &l2addr_find, TRUE, &l2_entry_key, &l2_entry_val);
        BCMDNX_IF_ERR_EXIT(rv);

        /* get entry */
        soc_sand_rv = soc_ppd_frwrd_bmact_entry_get(soc_sand_dev_id, &l2_entry_key, &l2_entry_val, &found);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        if (!found) {
            
            LOG_VERBOSE(BSL_LS_BCM_L2, 
                    (BSL_META_U(unit,
                                "bcm_l2_addr_get address not found\n")));
            BCMDNX_IF_ERR_NOT_E_NOT_FOUND_EXIT(BCM_E_NOT_FOUND);
        }

        /* convert type to BCM*/
        rv = _bcm_petra_l2_from_petra_bmact_entry(unit, l2addr, FALSE, &l2_entry_key, &l2_entry_val);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else {
        SOC_PPC_FRWRD_MACT_ENTRY_KEY l2_entry_key;
        SOC_PPC_FRWRD_MACT_ENTRY_VALUE l2_entry_val;

        bcm_l2_addr_t_init(&l2addr_find,mac_addr,vid);

        /* If custom_feature_ipmc_l2_ssm_mode_lem is set, l2addr.flags is input/output */
        if (l2addr->flags & BCM_L2_SRC_HIT) {
            if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ipmc_l2_ssm_mode_lem", 0)) {
            l2addr_find.flags = BCM_L2_SRC_HIT;
            }
        }

        /* map BCM MACT key/entry to Soc_petra MACT key/entry*/
        rv = _bcm_petra_l2_to_petra_entry(unit, &l2addr_find, TRUE, &l2_entry_key, &l2_entry_val);
        BCMDNX_IF_ERR_EXIT(rv);

        /* get entry*/
        soc_sand_rv = soc_ppd_frwrd_mact_entry_get(soc_sand_dev_id,&l2_entry_key,&l2_entry_val,&found);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        if (!found) {

            LOG_VERBOSE(BSL_LS_BCM_L2, 
                    (BSL_META_U(unit,
                                "bcm_l2_addr_get address not found\n")));

            BCMDNX_IF_ERR_NOT_E_NOT_FOUND_EXIT(BCM_E_NOT_FOUND);
        }

        /* convert type to BCM*/
        rv = _bcm_petra_l2_from_petra_entry(unit, l2addr, FALSE, &l2_entry_key, &l2_entry_val);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    LLM_PER_TUNNEL_ENABLE(unit);
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_l2_addr_register(
                           int unit,
                           bcm_l2_addr_callback_t fn,
                           void *fn_data)
{
    int  i;    

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _DPP_L2_INIT_CHECK(unit);
    
    /* Check if given function is already registered (with same data) */
    for (i = 0; i < _BCM_PETRA_L2_CB_MAX; i++) {
        if ((_BCM_PETRA_L2_CB_ENTRY(unit, i).fn == fn) &&
            (_BCM_PETRA_L2_CB_ENTRY(unit, i).fn_data == fn_data)) {
            BCM_EXIT;
        }
    }

    /* Add to list */
    if (_BCM_PETRA_L2_CB_COUNT(unit) >= _BCM_PETRA_L2_CB_MAX) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("No resource to register callback ")));
    }

    for (i = 0; i < _BCM_PETRA_L2_CB_MAX; i++) {
        if (_BCM_PETRA_L2_CB_ENTRY(unit, i).fn == NULL) {
            _BCM_PETRA_L2_CB_ENTRY(unit, i).fn = fn;
            _BCM_PETRA_L2_CB_ENTRY(unit, i).fn_data = fn_data;
            _BCM_PETRA_L2_CB_COUNT(unit)++;
            break;
        }
    }    

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_l2_addr_thaw(
                       int unit)
{
    int freeze_cnt;
    int rv = BCM_E_NONE;
    SOC_PPC_FRWRD_MACT_AGING_INFO  aging_info;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    _bcm_petra_l2_freeze_t freeze_info;

    BCMDNX_INIT_FUNC_DEFS;
    /* check parameters*/
    BCM_DPP_UNIT_CHECK(unit);
    _DPP_L2_INIT_CHECK(unit);

    soc_sand_dev_id = (unit);

    /* map BCM MACT key/entry to Soc_petra MACT key/entry*/
    rv = _bcm_petra_l2_freeze_dec(unit, &freeze_cnt);
    BCMDNX_IF_ERR_EXIT(rv);
    
    /* thaw without freeze is illegal*/
    if (freeze_cnt < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Call thaw withouth freeze")));
    }
    /* if this is thaw last freeze*/
    if (freeze_cnt == 0) {
        /* restore state before freeze*/
        _bcm_petra_l2_freeze_info_get(unit,&freeze_info);
        SOC_PPC_FRWRD_MACT_AGING_INFO_clear(&aging_info);
        soc_sand_rv = soc_ppd_frwrd_mact_aging_info_get(soc_sand_dev_id,&aging_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        aging_info.enable_aging = freeze_info.save_age_ena;
        soc_sand_rv = soc_ppd_frwrd_mact_aging_info_set(soc_sand_dev_id,&aging_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_l2_addr_unregister(
                             int unit, 
                             bcm_l2_addr_callback_t fn,
                             void *fn_data)
{
    int  i;

    BCMDNX_INIT_FUNC_DEFS;
    /* Check params */
    /*UNIT_INIT_CHECK(unit);*/
    /*PARAM_NULL_CHECK(fn);*/
    BCM_DPP_UNIT_CHECK(unit);    

    if (_BCM_PETRA_L2_CB_COUNT(unit) == 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Callback doesn't exist")));
    }        

    for (i = 0; i < _BCM_PETRA_L2_CB_MAX; i++) {
        if ((_BCM_PETRA_L2_CB_ENTRY(unit, i).fn == fn) &&
            (_BCM_PETRA_L2_CB_ENTRY(unit, i).fn_data == fn_data)) {
            _BCM_PETRA_L2_CB_ENTRY(unit, i).fn = NULL;
            _BCM_PETRA_L2_CB_ENTRY(unit, i).fn_data = NULL;
            _BCM_PETRA_L2_CB_COUNT(unit)--;
            break;
        }
    }    

    if (i >= _BCM_PETRA_L2_CB_MAX) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Callback doesn't exist")));
    }

    BCM_EXIT;
exit:    
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_l2_age_timer_get(
                           int unit, 
                           int *age_seconds)
{
    int rv = BCM_E_NONE;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    SOC_PPC_FRWRD_MACT_AGING_INFO
        aging_info;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);
    BCM_DPP_UNIT_CHECK(unit);
    _DPP_L2_INIT_CHECK(unit);

    /* read from PPD*/
    soc_sand_rv = soc_ppd_frwrd_mact_aging_info_get(soc_sand_dev_id,&aging_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    *age_seconds = aging_info.aging_time.sec;

    if (aging_info.aging_time.mili_sec>500) {
        *age_seconds +=1;
    }
    /* in case not enabled, set to zero*/
    if(!aging_info.enable_aging) {
        *age_seconds = 0;
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_l2_age_timer_set(
                           int unit,
                           int age_seconds)
{
    int rv = BCM_E_NONE;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    SOC_PPC_FRWRD_MACT_AGING_INFO
        aging_info;
    _bcm_petra_l2_freeze_t freeze_info;
    uint8 is_freezed;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _DPP_L2_INIT_CHECK(unit);
    SOC_PPC_FRWRD_MACT_AGING_INFO_clear(&aging_info);
    soc_sand_dev_id = (unit);

    /* max value is check per device inside PPD*/
    if (age_seconds < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("age_seconds has to be >= 0 ")));
    }

    aging_info.enable_aging = (age_seconds)?1:0;
    aging_info.aging_time.sec = age_seconds;

    /* if MACT procs are freezed then store new aging-ebale in SW and still disable in hw*/
    BCMDNX_IF_ERR_EXIT(_bcm_petra_l2_is_freezed(unit, &is_freezed));
    if(is_freezed){
        _bcm_petra_l2_freeze_info_get(unit,&freeze_info);
        freeze_info.save_age_ena = aging_info.enable_aging;
        _bcm_petra_l2_freeze_info_set(unit,&freeze_info);
        aging_info.enable_aging = 0;
    }

    soc_sand_rv = soc_ppd_frwrd_mact_aging_info_set(soc_sand_dev_id,&aging_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

#ifdef BCM_88660_A0
    /* Refresh the SLB final age. */
    if (SOC_IS_ARADPLUS(unit) && soc_property_get(unit, spn_RESILIENT_HASH_ENABLE, 0)) {
          uint32 soc_sand_rv;
          uint32 soc_sand_dev_id; 
          SOC_PPC_SLB_CONFIGURATION_ITEM_SLB_ENTRY_AGING_TIME_IN_SECONDS aging;

          soc_sand_dev_id = (unit);
          SOC_PPC_SLB_CLEAR(&aging, SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_SLB_ENTRY_AGING_TIME_IN_SECONDS);

          rv = sw_state_access[unit].dpp.soc.arad.pp.slb_config.age_time_in_seconds.get(soc_sand_dev_id, &(aging.age_time_in_seconds));
          BCMDNX_IF_ERR_EXIT(rv);

          BCMDNX_VERIFY(aging.age_time_in_seconds > 0);

          soc_sand_rv = soc_ppd_slb_set_global_cfg(soc_sand_dev_id, SOC_PPC_SLB_DOWNCAST(&aging, SOC_PPC_SLB_CONFIGURATION_ITEM));
          BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    }
#endif /* BCM_88660_A0 */

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}



int 
bcm_petra_l2_matched_traverse(
                              int unit, 
                              uint32 flags, 
                              bcm_l2_addr_t *match_addr,
                              bcm_l2_traverse_cb trav_fn, 
                              void *user_data)
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _DPP_L2_INIT_CHECK(unit);
    
    BCMDNX_NULL_CHECK(match_addr);
    BCMDNX_NULL_CHECK(trav_fn);

    /* call traverse API, which will work on rule and trav_fn*/
    rv = bcm_petra_l2_match_masked_traverse(unit, flags, match_addr, NULL, trav_fn, user_data);
    BCMDNX_IF_ERR_EXIT(rv);
exit:
  _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
  BCMDNX_FUNC_RETURN;
}

/* Traverse the SW shadow table*/
int _bcm_petra_l2_sw_traverse(int unit, 
                    bcm_l2_traverse_cb trav_fn, 
                    void *user_data)
{
    SOC_SAND_TABLE_BLOCK_RANGE block_range;
    SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE      rule;
    bcm_l2_addr_t l2addr;
    int indx;
    uint32 nof_entries;
    uint32 soc_sand_rv;
    
    BCMDNX_INIT_FUNC_DEFS;
    
    SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE_clear(&rule);
    soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&block_range);

    soc_sand_rv = soc_sand_pp_mac_address_string_parse("000000000000", &(rule.key_rule.mac.mac_mask));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    rule.key_type = SOC_PPC_FRWRD_MACT_KEY_TYPE_MAC_ADDR;
  /*  rule.key_rule.mac.fid_mask = 0x0;
    rule.value_rule.compare_mask = 1;
*/
    SOC_SAND_TBL_ITER_SET_BEGIN(&(block_range.iter));
    block_range.entries_to_act = _BCM_PETRA_L2_TRVRS_ITER_BLK_SIZE;
    block_range.entries_to_scan = SOC_SAND_TBL_ITER_SCAN_ALL;
    while(!SOC_SAND_TBL_ITER_IS_END(&(block_range.iter))) {
        soc_sand_rv = soc_ppd_frwrd_mact_get_block(unit,&rule, SOC_PPC_FRWRD_MACT_TABLE_SW_ONLY, 
                                                   &block_range,_bcm_l2_traverse_mact_keys[unit],_bcm_l2_traverse_mact_vals[unit],&nof_entries);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        if(nof_entries == 0) {
            break;
        }

        /* Dump the found entries to the output */
        for (indx = 0; indx < nof_entries; ++indx) {
            soc_sand_rv = _bcm_petra_l2_from_petra_entry(unit,&l2addr, FALSE, &(_bcm_l2_traverse_mact_keys[unit][indx]),&(_bcm_l2_traverse_mact_vals[unit][indx]));
            BCMDNX_IF_ERR_EXIT(soc_sand_rv);

            trav_fn(unit, &l2addr, user_data);
        }
    }

exit:
    if (soc_sand_get_error_code_from_error_word(soc_sand_rv)!=SOC_SAND_OK) 
    {
        arad_pp_frwrd_mact_clear_flush_operation(unit);
    }

    BCMDNX_FUNC_RETURN;
}

int bcm_petra_l2_traverse(
    int unit, 
    bcm_l2_traverse_cb trav_fn, 
    void *user_data)
{
    bcm_l2_addr_t  match_addr;
    uint32 flags=0;
    int rv = BCM_E_NONE;
    bcm_mac_t mac_t;
    bcm_vlan_t vlan=0;

    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);
    _DPP_L2_INIT_CHECK(unit);

    BCMDNX_NULL_CHECK(trav_fn);

    /* match all rule */
    sal_memset(mac_t, 0x0, sizeof(bcm_mac_t));
    bcm_l2_addr_t_init(&match_addr,mac_t,vlan);

    flags = BCM_L2_TRAVERSE_IGNORE_DISCARD_SRC | BCM_L2_TRAVERSE_IGNORE_DES_HIT|BCM_L2_TRAVERSE_MATCH_STATIC|BCM_L2_TRAVERSE_MATCH_DYNAMIC ;

    /* call traverse API, which will work on rule and trav_fn*/
    rv = bcm_petra_l2_match_masked_traverse(unit, flags, &match_addr, NULL, trav_fn, user_data);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_l2_match_masked_traverse(
                                       int unit, 
                                       uint32 flags, 
                                       bcm_l2_addr_t *match_addr, 
                                       bcm_l2_addr_t *mask_addr, 
                                       bcm_l2_traverse_cb trav_fn, 
                                       void *user_data)
{
    uint32 trvrs_flag=0;
    uint32 normal_flags=0;
    int     restore;
    int rv = BCM_E_NONE;
    int restore_rv = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _DPP_L2_INIT_CHECK(unit);
    
    
    BCMDNX_NULL_CHECK(trav_fn);

    /* check params*/
    /* unsupported flags in dune devices */

    /*
     * convert flags to traverse flag
     */
    if (flags & BCM_L2_TRAVERSE_MATCH_STATIC) {
        normal_flags |= BCM_L2_DELETE_STATIC;
    }
    if (flags & BCM_L2_TRAVERSE_MATCH_MAC) {
        trvrs_flag |= _BCM_PETRA_TRAVERSE_MATCH_MAC;
    }
    if (flags & BCM_L2_TRAVERSE_MATCH_VLAN) {
        trvrs_flag |= _BCM_PETRA_TRAVERSE_MATCH_VLAN;
    }

    /* no need to call delete/add call-backs*/
    normal_flags |= BCM_L2_DELETE_NO_CALLBACKS;

    if (flags & BCM_L2_TRAVERSE_MATCH_DEST) {
        trvrs_flag |= (_BCM_PETRA_TRAVERSE_MATCH_PORT);
    }

    if (flags & BCM_L2_TRAVERSE_IGNORE_DISCARD_SRC) {
        trvrs_flag |= _BCM_PETRA_TRAVERSE_MATCH_IGNORE_DISCARD_SRC;
    }

    if (flags & BCM_L2_TRAVERSE_IGNORE_DES_HIT) {
        trvrs_flag |= _BCM_PETRA_TRAVERSE_MATCH_IGNORE_DES_HIT;
    }

    /* call traverse API, which will work on rule and trav_fn*/
    rv = bcm_petra_l2_addr_traverse_by_rule(unit,match_addr,normal_flags,trvrs_flag,mask_addr,NULL,NULL,&restore,trav_fn,user_data);

    /* if need to restore aging/learning do it*/
    if(restore) {
        restore_rv = bcm_petra_l2_addr_thaw(unit);
        BCMDNX_IF_ERR_EXIT(restore_rv);
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_l2_learn_limit_enable(int unit)
{
    int rv = BCM_E_NONE;
#ifdef BCM_CMICM_SUPPORT
    uint32 soc_sand_rv;
    shr_llm_msg_pon_att_vmac_encode_t vmac_encode;
    shr_llm_msg_pon_att_vmac_prefix_t vmac_prefix;
#endif

    BCMDNX_INIT_FUNC_DEFS;

#ifdef BCM_CMICM_SUPPORT
    /* MAC limit init settings */
    /* Set L2 learn limit mode. 0-per VLAN, 1-VLAN_PORT , 2-PON port and Tunnel-ID */ 
    /* MAC limit per PON port and Tunnel-ID */
    if (SOC_MAC_LIMIT_PER_TUNNEL_ENABLE_GET(unit)) {
        rv = shr_llm_appl_init(unit);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Send LLM VMAC encode message to uC */
        if (SOC_DPP_CONFIG(unit)->pp.vmac_enable) {
            /* Send vmac encoding to ukernel */
            /* The function soc_sand_pp_mac_address_struct_to_long writes to indecies 0 and 1 of the second parameter only */
            /* coverity[overrun-buffer-val : FALSE] */   
            soc_sand_rv = soc_sand_pp_mac_address_struct_to_long(&(SOC_DPP_CONFIG(unit)->pp.vmac_encoding_val),
                                                                 vmac_encode.value);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            /* The function soc_sand_pp_mac_address_struct_to_long writes to indecies 0 and 1 of the second parameter only */
            /* coverity[overrun-buffer-val : FALSE] */   
            soc_sand_rv = soc_sand_pp_mac_address_struct_to_long(&(SOC_DPP_CONFIG(unit)->pp.vmac_encoding_mask),
                                                                 vmac_encode.mask);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            
            rv = shr_llm_msg_pon_attr_set(unit, LLM_SERVICE_VMAC, SHR_LLM_MSG_PON_ATT_UPDATE_VMAC_ENCODE, &vmac_encode);
            BCMDNX_IF_ERR_EXIT(rv);

            /* Send vmac prefix to ukernel */
            soc_sand_rv = soc_ppd_frwrd_mact_app_to_prefix_get(unit, 
                                                               SOC_PPC_FRWRD_MACT_PREFIX_APP_ID_OMAC, 
                                                               &(vmac_prefix.prefix[0]));
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            soc_sand_rv = soc_ppd_frwrd_mact_app_to_prefix_get(unit, 
                                                               SOC_PPC_FRWRD_MACT_PREFIX_APP_ID_OMAC_2_VMAC, 
                                                               &(vmac_prefix.prefix[1]));
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            
            soc_sand_rv = soc_ppd_frwrd_mact_app_to_prefix_get(unit, 
                                                               SOC_PPC_FRWRD_MACT_PREFIX_APP_ID_VMAC, 
                                                               &(vmac_prefix.prefix[2]));
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            soc_sand_rv = soc_ppd_frwrd_mact_app_to_prefix_get(unit, 
                                                               SOC_PPC_FRWRD_MACT_PREFIX_APP_ID_VMAC_2_OMAC, 
                                                               &(vmac_prefix.prefix[3]));
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            rv = shr_llm_msg_pon_attr_set(unit, LLM_SERVICE_VMAC, SHR_LLM_MSG_PON_ATT_UPDATE_VMAC_PREFIX, &vmac_prefix);
            BCMDNX_IF_ERR_EXIT(rv);
        }
        /* Start a event thread to recieve MAC move event from ukernel */
        rv = _bcm_petra_l2_llm_thread_init(unit);
        BCMDNX_IF_ERR_EXIT(rv);
    }
#endif /*BCM_CMICM_SUPPORT*/

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_l2_learn_limit_disable(int unit)
{
    int rv = BCM_E_NONE;
    
    BCMDNX_INIT_FUNC_DEFS;

#ifdef BCM_CMICM_SUPPORT
    if (SOC_MAC_LIMIT_PER_TUNNEL_ENABLE_GET(unit)) {
        (void) _bcm_petra_l2_llm_thread_deinit(unit);

        rv = shr_llm_appl_deinit(unit);
        BCMDNX_IF_ERR_EXIT(rv);
    }
#endif

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_l2_learn_limit_get(
                             int unit, 
                             bcm_l2_learn_limit_t *limit)
{
    SOC_PPC_VSI_ID vsi = 0;
    SOC_PPC_VSI_INFO vsi_info;
    SOC_PPC_FRWRD_MACT_MAC_LIMIT_INFO limit_info;
    int rv = BCM_E_NONE;
    uint32 soc_sand_rv;
    unsigned int soc_sand_dev_id;
    SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO  mac_limit_glb;    
    _bcm_dpp_gport_hw_resources gport_hw_resources;
    uint32 limit_tbl_idx;
    uint32 is_limit_tbl_idx_reserved;
#ifdef BCM_CMICM_SUPPORT
    uint8 llm_tunnel_lock_taken = FALSE;
    shr_llm_msg_pon_att_t msg;
    shr_llm_msg_pon_att_t rsp;
#endif /* BCM_CMICM_SUPPORT */

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _DPP_L2_INIT_CHECK(unit);
    
    BCMDNX_NULL_CHECK(limit);
      
    soc_sand_dev_id = (unit);
    
    /* Check for valid flags */
    if (limit->flags & (~(_BCM_PETRA_L2_LIMIT_FLAGS_SUPPORTED(unit)))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unsupported LIMIT flags")));
    }

    /* system limit, Arad only */
    if (limit->flags & BCM_L2_LEARN_LIMIT_SYSTEM) {
        soc_sand_rv = soc_ppd_frwrd_mact_mac_limit_glbl_info_get(soc_sand_dev_id,&mac_limit_glb);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if(mac_limit_glb.glbl_limit == SOC_PPC_FRWRD_MACT_NO_GLOBAL_LIMIT){
            limit->limit = -1;
        }
        else{
            limit->limit = mac_limit_glb.glbl_limit;
        }
        BCM_EXIT;
    }

    /* Validate the MACT Limit mode flags with the SOC property */
    if (limit->flags & BCM_L2_LEARN_LIMIT_PORT) {
        if (SOC_PPC_FRWRD_MACT_LEARN_LIMIT_MODE != SOC_PPC_FRWRD_MACT_LEARN_LIMIT_TYPE_LIF) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("LIF MACT Limit doesn't match the SOC property L2_LEARN_LIMIT_MODE")));
        }
    } else if (limit->flags & BCM_L2_LEARN_LIMIT_VLAN) {
        if (SOC_PPC_FRWRD_MACT_LEARN_LIMIT_MODE != SOC_PPC_FRWRD_MACT_LEARN_LIMIT_TYPE_VSI) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VSI MACT Limit doesn't match the SOC property L2_LEARN_LIMIT_MODE")));
        }
    }

#ifdef BCM_CMICM_SUPPORT
    if (_L2_LEARN_LIMIT_PER_TUNNEL_IS_ENABLED(unit, limit)) {

        _bcm_dpp_gport_info_t gport_info;

        LLM_TUNNEL_LOCK(unit);
        llm_tunnel_lock_taken = TRUE;
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, limit->port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info)); 

        if (!_BCM_PPD_IS_PON_PP_PORT(gport_info.local_port, unit)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("BCM_L2_LEARN_LIMIT_TUNNEL is not supported in a non PON port")));
        }

        /* Send LLM GET message to uC */
        sal_memset(&msg, 0x0, sizeof(msg));
        sal_memset(&rsp, 0x0, sizeof(rsp));
        msg.port = gport_info.local_port;
        msg.tunnel = limit->tunnel_id;
        msg.tunnel_count = 1;
        rv = shr_llm_msg_mac_limit_get(unit, &msg, &rsp);
        BCMDNX_IF_ERR_EXIT(rv);
        
        limit->limit = _BCM_PETRA_L2_MAC_LIMIT_GET(rsp.bitmap);
        BCM_EXIT;
    }
#endif

    if (limit->flags & BCM_L2_LEARN_LIMIT_VLAN) {
        /* per vlan limit */
        rv = _bcm_dpp_vlan_check(unit, limit->vlan);
        BCMDNX_IF_ERR_EXIT(rv);
        
        /* map VID to VSI*/
        rv = _bcm_petra_vid_to_vsi(unit,limit->vlan,&vsi);
        BCMDNX_IF_ERR_EXIT(rv);
    } 

    /* In case of LIF limit, the existing VSI limit table is used. Therefore, the LIF value should be
       mapped to the VSI value, according to exact mapping that is done by the HW */
    else if (limit->flags & BCM_L2_LEARN_LIMIT_PORT) {
        /* Convert from gport to LIF */
        rv = _bcm_dpp_gport_to_hw_resources(unit, limit->port, _BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK, 
                                                 &gport_hw_resources);
        BCMDNX_IF_ERR_EXIT(rv);    

        /* Map LIF limit to the common limit table */
        soc_sand_rv = soc_ppd_frwrd_mact_limit_mapped_val_to_table_index_get(soc_sand_dev_id,
                                                                             gport_hw_resources.global_in_lif, &limit_tbl_idx, &is_limit_tbl_idx_reserved);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* Currnetly the limit table is treated as a per VSI table. Therefore, the Limit table index is used as
           a VSI value */
        vsi = limit_tbl_idx;
    }

    /* This code is common for all MACT Limit modes that use the common Limit table 
       for hystorical reasons, the table is treated per VSI, and hence a vsi index
       is used */
    soc_sand_rv = soc_ppd_vsi_info_get(soc_sand_dev_id, vsi, &vsi_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    soc_sand_rv = soc_ppd_frwrd_mact_learn_profile_limit_info_get(soc_sand_dev_id,vsi_info.mac_learn_profile_id,&limit_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if(limit_info.is_limited == 0) {
        limit->limit = -1;
    }
    else if(limit_info.nof_entries == SOC_DPP_DEFS_GET(unit, max_mact_limit)) {
        limit->limit = -1;
    }
    else{
        limit->limit = limit_info.nof_entries;
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
#ifdef BCM_CMICM_SUPPORT    
    if (llm_tunnel_lock_taken) {
        LLM_TUNNEL_UNLOCK(unit);
    } 
#endif
    BCMDNX_FUNC_RETURN;
}


int 
bcm_petra_l2_learn_limit_set(
                             int unit, 
                             bcm_l2_learn_limit_t *limit)
{
    SOC_PPC_VSI_ID vsi;
    SOC_PPC_VSI_INFO vsi_info;
    SOC_PPC_FRWRD_MACT_MAC_LIMIT_INFO limit_info;
    int new_learn_profile;
    int learn_profile_first_appear;
    int rv = BCM_E_NONE;
    uint32 soc_sand_rv;
    int  old_limit, old_learn_profile,old_handle_profile,old_aging_profile;
    unsigned int soc_sand_dev_id;
    int new_limit,is_limit_active;
    SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO  mac_limit_glb;
    int is_last;
    int set_using_common_limits = FALSE;
    _bcm_dpp_gport_hw_resources gport_hw_resources;
    uint32 limit_tbl_idx;
    uint32 is_limit_tbl_idx_reserved;
#ifdef BCM_CMICM_SUPPORT
    uint8 llm_tunnel_lock_taken = FALSE;
#endif /* BCM_CMICM_SUPPORT */

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _DPP_L2_INIT_CHECK(unit);
    soc_sand_dev_id = (unit);
    
    BCMDNX_NULL_CHECK(limit);

    /* Check for valid flags */
    if (limit->flags & (~(_BCM_PETRA_L2_LIMIT_FLAGS_SUPPORTED(unit)))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unsupported LIMIT flags")));
    }

    /* Above max limit table size*/
    if ((limit->flags & (BCM_L2_LEARN_LIMIT_VLAN | BCM_L2_LEARN_LIMIT_PORT)) &&
        (limit->limit != -1) && (limit->limit > SOC_DPP_DEFS_GET(unit, max_mact_limit))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("MACT Limit is out of range")));
    }
    /* above max total limit */
    if (limit->flags & BCM_L2_LEARN_LIMIT_SYSTEM && limit->limit != -1 && limit->limit > SOC_DPP_CONFIG(unit)->l2.mac_size-1) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("System MACT Limit is out of range")));
    }
    
    /*  system limit, Arad only */
    if(limit->flags & BCM_L2_LEARN_LIMIT_SYSTEM) {
        soc_sand_rv = soc_ppd_frwrd_mact_mac_limit_glbl_info_get(soc_sand_dev_id,&mac_limit_glb);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if( limit->limit == -1){
            mac_limit_glb.glbl_limit = SOC_PPC_FRWRD_MACT_NO_GLOBAL_LIMIT;
        }
        else{
            mac_limit_glb.glbl_limit = limit->limit;
        }
        soc_sand_rv = soc_ppd_frwrd_mact_mac_limit_glbl_info_set(soc_sand_dev_id,&mac_limit_glb);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

    /* Non system limit: if unlimited, set to device max limit */
    if(limit->limit == -1){
        new_limit = SOC_DPP_DEFS_GET(unit, max_mact_limit);
    }
    else{
        new_limit = limit->limit;
    }

    /* if set limit for some FID or LIF then enable limit mechanism*/
    if((limit->flags & (BCM_L2_LEARN_LIMIT_VLAN | BCM_L2_LEARN_LIMIT_PORT)) && limit->limit != -1) {
        /* globally enable mac-limit mechanism*/
        /* once user activate limit, it cannot be shut down*/
        soc_sand_rv = soc_ppd_frwrd_mact_mac_limit_glbl_info_get(soc_sand_dev_id,&mac_limit_glb);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        mac_limit_glb.enable = 1;
        mac_limit_glb.static_may_exceed = 0;
        soc_sand_rv = soc_ppd_frwrd_mact_mac_limit_glbl_info_set(soc_sand_dev_id,&mac_limit_glb);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

    /* In case of LIF limit, the existing VSI limit table is used. Therefore, the LIF value should be
       mapped to the VSI value, according to exact mapping that is done by the HW */
    if (limit->flags & BCM_L2_LEARN_LIMIT_PORT) {
        /* Validate the LIF MACT Limit with a SOC property */
        if (SOC_PPC_FRWRD_MACT_LEARN_LIMIT_MODE != SOC_PPC_FRWRD_MACT_LEARN_LIMIT_TYPE_LIF) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("LIF MACT Limit doesn't match the SOC property L2_LEARN_LIMIT_MODE")));
        }

        /* Convert from gport to LIF */
        rv = _bcm_dpp_gport_to_hw_resources(unit, limit->port, _BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK,
                                                 &gport_hw_resources);
        BCMDNX_IF_ERR_EXIT(rv);    

        /* Set a flag to use the common limit table */
        set_using_common_limits = TRUE;

        /* Map LIF limit to the common limit table */
        soc_sand_rv = soc_ppd_frwrd_mact_limit_mapped_val_to_table_index_get(soc_sand_dev_id,
                                                                             gport_hw_resources.global_in_lif, &limit_tbl_idx, &is_limit_tbl_idx_reserved);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* Don't allow the set, if the mapping leads to the reserved entry for invalid mappings.
           There's no way to know if it was an explicit limit for the invalid mapping or misconfiguration. */
        if (is_limit_tbl_idx_reserved) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("The LIF is out of range for MACT Limit setting")));
        }

        /* Currently the limit table is treated as a per VSI table. Therefore, the Limit table index is used as
           a VSI value */
        vsi = limit_tbl_idx;
    }

    /* setting mac limit for vlan */
    if (limit->flags & BCM_L2_LEARN_LIMIT_VLAN) {
        /* Validate the VSI MACT Limit with a SOC property */
        if (SOC_PPC_FRWRD_MACT_LEARN_LIMIT_MODE != SOC_PPC_FRWRD_MACT_LEARN_LIMIT_TYPE_VSI) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VSI MACT Limit doesn't match the SOC property L2_LEARN_LIMIT_MODE")));
        }

        /* check vlan-id */
        rv = _bcm_dpp_vlan_check(unit, limit->vlan);
        BCMDNX_IF_ERR_EXIT(rv);

        /* map VID to VSI*/
        rv = _bcm_petra_vid_to_vsi(unit,limit->vlan,&vsi);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Set a flag to use the common limit table */
        set_using_common_limits = TRUE;
    }

    /* This code is common for all MACT Limit modes that use the common Limit table 
       for historical reasons, the table is treated per VSI, and hence a vsi index
       is used */
    if (set_using_common_limits == TRUE) {
        soc_sand_rv = soc_ppd_vsi_info_get(soc_sand_dev_id, vsi, &vsi_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* get old information attached with given vsi*/
        rv = _bcm_dpp_am_template_l2_learn_profile_data_get(unit, vsi, &old_limit, &old_handle_profile, &old_aging_profile);
        BCMDNX_IF_ERR_EXIT(rv); 


        /* allocate fid-profile with old learn event and new-limit */
        rv = _bcm_dpp_am_template_l2_learn_profile_exchange(unit, 0, vsi, limit->limit, old_handle_profile, old_aging_profile, &old_learn_profile, &is_last, &new_learn_profile,&learn_profile_first_appear);
        BCMDNX_IF_ERR_EXIT(rv);

        /* if first appear then configure profile*/
        if (learn_profile_first_appear) {
            SOC_PPC_FRWRD_MACT_MAC_LIMIT_INFO_clear(&limit_info);
            limit_info.is_limited = 1;
            limit_info.nof_entries = new_limit;
            /* for, PB, max limit --> no limit, as max limit = mac size */
            limit_info.action_when_exceed = SOC_PPC_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE_INTERRUPT; /* when mac limit exceed threshold interrupt is expected */
            soc_sand_rv = soc_ppd_frwrd_mact_learn_profile_limit_info_set(soc_sand_dev_id,new_learn_profile,&limit_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            /* update fid-learn-profile to event-handle profile */
            soc_sand_rv = soc_ppd_frwrd_mact_event_handle_profile_set(soc_sand_dev_id,new_learn_profile,old_handle_profile);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            /* update fid-learn-profile to fid-aging profile */
            soc_sand_rv = soc_ppd_frwrd_mact_fid_aging_profile_set(soc_sand_dev_id,new_learn_profile,old_aging_profile);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }

        vsi_info.mac_learn_profile_id = new_learn_profile;
        soc_sand_rv = soc_ppd_vsi_info_set(soc_sand_dev_id, vsi, &vsi_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

    /* check if limit disable on all FID, if so disable it totally */
    if (set_using_common_limits && (limit->limit == -1)) {
        rv = _bcm_dpp_am_template_l2_learn_limit_active(unit, &is_limit_active);
        BCMDNX_IF_ERR_EXIT(rv);

        if (!is_limit_active) {
            soc_sand_rv = soc_ppd_frwrd_mact_mac_limit_glbl_info_get(soc_sand_dev_id,&mac_limit_glb);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            mac_limit_glb.enable = 0;
            mac_limit_glb.static_may_exceed = 1;
            soc_sand_rv = soc_ppd_frwrd_mact_mac_limit_glbl_info_set(soc_sand_dev_id,&mac_limit_glb);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }

#ifdef BCM_CMICM_SUPPORT
    if (_L2_LEARN_LIMIT_PER_TUNNEL_IS_ENABLED(unit, limit)) {
        shr_llm_msg_pon_att_t msg;
        shr_llm_msg_pon_att_t rsp;
        int new_limit = 0;
        _bcm_dpp_gport_info_t gport_info;

        LLM_TUNNEL_LOCK(unit);
        llm_tunnel_lock_taken = TRUE;
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, limit->port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

        if (!_BCM_PPD_IS_PON_PP_PORT(gport_info.local_port, unit)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("BCM_L2_LEARN_LIMIT_TUNNEL is not supported in a non PON port")));
        }

        new_limit = limit->limit;
        if (new_limit > ARAD_PP_MAX_MAC_LIMIT_VAL_PER_TUNNEL) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid mac limit value")));
        }

        sal_memset(&msg, 0x0, sizeof(msg));
        sal_memset(&rsp, 0x0, sizeof(rsp));
        msg.port = gport_info.local_port;
        msg.tunnel = limit->tunnel_id;
        msg.tunnel_count = 1;
        
        /* Send LLM GET message to uC */
        rv = shr_llm_msg_mac_limit_get(unit, &msg, &rsp);
        BCMDNX_IF_ERR_EXIT(rv);

        sal_memset(&msg, 0x0, sizeof(msg));
        msg.port = gport_info.local_port;
        msg.tunnel = limit->tunnel_id;
        msg.tunnel_count = 1;
        msg.bitmap = new_limit;
        if (rsp.type_of_service == LLM_SERVICE_VMAC) {
            msg.type_of_service = LLM_SERVICE_VMAC;
        } else {
            msg.type_of_service = LLM_SERVICE_MAC_LIMIT;
        }            

        /* Send LLM SET message to uC */
        rv = shr_llm_msg_mac_limit_set(unit, SHR_LLM_MSG_PON_ATT_MAC_BITMAP_SET, &msg);
        BCMDNX_IF_ERR_EXIT(rv);
    }
#endif

exit:
#ifdef BCM_CMICM_SUPPORT    
    if (llm_tunnel_lock_taken) {
        LLM_TUNNEL_UNLOCK(unit);
    } 
#endif
    BCMDNX_FUNC_RETURN;
}



STATIC
int
_bcm_l2_pkt_eth_header_build(
                             bcm_l2_learn_msgs_config_t     *header_info,
                             uint32                       start_bit_msb,
                             uint32                       header[SOC_PPC_FRWRD_MACT_MSG_HDR_SIZE]
                             )
{
    uint32    mac_long[2],    tmp,    start_bit;
    SOC_SAND_PP_MAC_ADDRESS   tmp_mac;
    uint32 soc_sand_rv;
    int     rv = BCM_E_NONE;
    

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;
    start_bit = start_bit_msb;

    /* DA */
    rv = _bcm_petra_mac_to_sand_mac(header_info->dst_mac_addr,&(tmp_mac));
    BCMDNX_IF_ERR_EXIT(rv);
    
    /* The function soc_sand_pp_mac_address_struct_to_long writes to indecies 0 and 1 of the second parameter only */
    /* coverity[overrun-buffer-val : FALSE] */   
    soc_sand_rv = soc_sand_pp_mac_address_struct_to_long(
                                                         &(tmp_mac),
                                                         mac_long
                                                         );
    BCM_SAND_IF_ERR_EXIT_NO_UNIT(soc_sand_rv);
    
    start_bit -= SOC_SAND_PP_MAC_ADDRESS_NOF_BITS;
    soc_sand_rv = soc_sand_bitstream_set_any_field(
                                                   mac_long,
                                                   start_bit,
                                                   SOC_SAND_PP_MAC_ADDRESS_NOF_BITS,
                                                   header
                                                   );
    BCM_SAND_IF_ERR_EXIT_NO_UNIT(soc_sand_rv);
    
    /* SA */
    rv = _bcm_petra_mac_to_sand_mac(header_info->src_mac_addr,&(tmp_mac));
    BCMDNX_IF_ERR_EXIT(rv);
    
    /* The function soc_sand_pp_mac_address_struct_to_long writes to indecies 0 and 1 of the second parameter only */
    /* coverity[overrun-buffer-val : FALSE] */   
    soc_sand_rv = soc_sand_pp_mac_address_struct_to_long(
                                                         &(tmp_mac),
                                                         mac_long
                                                         );
    BCM_SAND_IF_ERR_EXIT_NO_UNIT(soc_sand_rv);
    
    start_bit -= SOC_SAND_PP_MAC_ADDRESS_NOF_BITS;
    soc_sand_rv = soc_sand_bitstream_set_any_field(
                                                   mac_long,
                                                   start_bit,
                                                   SOC_SAND_PP_MAC_ADDRESS_NOF_BITS,
                                                   header
                                                   );
    BCM_SAND_IF_ERR_EXIT_NO_UNIT(soc_sand_rv);

    /*
     * if Ethernet header include VLAN tag then set it.
     */
    if(BCM_VLAN_VALID(header_info->vlan)) {
        /* Ether-type */
        start_bit -= SOC_SAND_PP_TPID_NOF_BITS;
        tmp = header_info->tpid;
        soc_sand_rv = soc_sand_bitstream_set_any_field(
                                                       &tmp,
                                                       start_bit,
                                                       SOC_SAND_PP_TPID_NOF_BITS,
                                                       header
                                                       );
        BCM_SAND_IF_ERR_EXIT_NO_UNIT(soc_sand_rv);
        
        /* vlan-prio */
        tmp = header_info->vlan_prio;
        
        start_bit -= SOC_SAND_PP_UP_NOF_BITS;
        soc_sand_rv = soc_sand_bitstream_set_any_field(
                                                       &tmp,
                                                       start_bit,
                                                       SOC_SAND_PP_UP_NOF_BITS,
                                                       header
                                                       );
        BCM_SAND_IF_ERR_EXIT_NO_UNIT(soc_sand_rv);
        
        
        /* DEI set to zero */
        tmp = 0;
        start_bit -= SOC_SAND_PP_CFI_NOF_BITS;
        soc_sand_rv = soc_sand_bitstream_set_any_field(
                                                       &tmp,
                                                       start_bit,
                                                       SOC_SAND_PP_CFI_NOF_BITS,
                                                       header
                                                       );
        BCM_SAND_IF_ERR_EXIT_NO_UNIT(soc_sand_rv);
        
        /* VID */
        tmp = header_info->vlan;
        start_bit -= SOC_SAND_PP_VID_NOF_BITS;
        soc_sand_rv = soc_sand_bitstream_set_any_field(
                                                       &tmp,
                                                       start_bit,
                                                       SOC_SAND_PP_VID_NOF_BITS,
                                                       header
                                                       );
        BCM_SAND_IF_ERR_EXIT_NO_UNIT(soc_sand_rv);
    }
    /* Ether-type */
    start_bit -= SOC_SAND_PP_ETHER_TYPE_NOF_BITS;
    tmp = header_info->ether_type;
    soc_sand_rv = soc_sand_bitstream_set_any_field(
                                                   &tmp,
                                                   start_bit,
                                                   SOC_SAND_PP_ETHER_TYPE_NOF_BITS,
                                                   header
                                                   );
    BCM_SAND_IF_ERR_EXIT_NO_UNIT(soc_sand_rv);

  
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


STATIC
int
_bcm_l2_pkt_eth_header_parse(
                             bcm_l2_learn_msgs_config_t     *header_info,
                             uint32                       start_bit_msb,
                             uint32                       header[SOC_PPC_FRWRD_MACT_MSG_HDR_SIZE],
                             int header_type)
{
    uint32    mac_long[2],    tmp,    start_bit;
    SOC_SAND_PP_MAC_ADDRESS   tmp_mac;
    uint32 soc_sand_rv;
    int     rv = BCM_E_NONE;
    

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;
    start_bit = start_bit_msb;
    /* DA */
    
    start_bit -= SOC_SAND_PP_MAC_ADDRESS_NOF_BITS;
    soc_sand_rv = soc_sand_bitstream_get_any_field(
                                                   header,
                                                   start_bit,
                                                   SOC_SAND_PP_MAC_ADDRESS_NOF_BITS,
                                                   mac_long
                                                   );
    BCM_SAND_IF_ERR_EXIT_NO_UNIT(soc_sand_rv);
    
    /* The function soc_sand_pp_mac_address_long_to_struct reads from indecies 0 and 1 of the first parameter only */
    /* coverity[overrun-buffer-val : FALSE] */   
    soc_sand_rv = soc_sand_pp_mac_address_long_to_struct(
                                                         mac_long,
                                                         &(tmp_mac)
                                                         );
    BCM_SAND_IF_ERR_EXIT_NO_UNIT(soc_sand_rv);

    rv = _bcm_petra_mac_from_sand_mac(header_info->dst_mac_addr,&(tmp_mac));
    BCMDNX_IF_ERR_EXIT(rv);


    /* SA */
    start_bit -= SOC_SAND_PP_MAC_ADDRESS_NOF_BITS;
    soc_sand_rv = soc_sand_bitstream_get_any_field(
                                                   header,
                                                   start_bit,
                                                   SOC_SAND_PP_MAC_ADDRESS_NOF_BITS,
                                                   mac_long
                                                   );
    BCM_SAND_IF_ERR_EXIT_NO_UNIT(soc_sand_rv);

    /* The function soc_sand_pp_mac_address_long_to_struct reads from indecies 0 and 1 of the first parameter only */
    /* coverity[overrun-buffer-val : FALSE] */   
    soc_sand_rv = soc_sand_pp_mac_address_long_to_struct(
                                                         mac_long,
                                                         &(tmp_mac)
                                                         );
    BCM_SAND_IF_ERR_EXIT_NO_UNIT(soc_sand_rv);

    rv = _bcm_petra_mac_from_sand_mac(header_info->src_mac_addr,&(tmp_mac));
    BCMDNX_IF_ERR_EXIT(rv);

    /*
     * if Ethernet header include VLAN tag then set it.
     */
    if(header_type == SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_ETH_VLAN_O_ITMH) {
        /* tpid */
        start_bit -= SOC_SAND_PP_TPID_NOF_BITS;
        tmp = 0;
        soc_sand_rv = soc_sand_bitstream_get_any_field(
                                                       header,
                                                       start_bit,
                                                       SOC_SAND_PP_TPID_NOF_BITS,
                                                       &tmp
                                                       );
        BCM_SAND_IF_ERR_EXIT_NO_UNIT(soc_sand_rv);
        header_info->tpid = (uint16)tmp;
    
        /* vlan-prio */
        start_bit -= SOC_SAND_PP_UP_NOF_BITS;
        tmp = 0;
        soc_sand_rv = soc_sand_bitstream_get_any_field(
                                                       header,
                                                       start_bit,
                                                       SOC_SAND_PP_UP_NOF_BITS,
                                                       &tmp
                                                       );
        BCM_SAND_IF_ERR_EXIT_NO_UNIT(soc_sand_rv);
    
        header_info->vlan_prio = (uint16)tmp;
        
        /* DEI set to zero */
        start_bit -= SOC_SAND_PP_CFI_NOF_BITS;
        tmp = 0;
        
        /* VID */
        start_bit -= SOC_SAND_PP_VID_NOF_BITS;
        tmp = 0;
        soc_sand_rv = soc_sand_bitstream_get_any_field(
                                                       header,
                                                       start_bit,
                                                       SOC_SAND_PP_VID_NOF_BITS,
                                                       &tmp
                                                       );
        BCM_SAND_IF_ERR_EXIT_NO_UNIT(soc_sand_rv);
        header_info->vlan = tmp;
    }
    /* ether-type */
    start_bit -= SOC_SAND_PP_ETHER_TYPE_NOF_BITS;
    tmp = 0;
    soc_sand_rv = soc_sand_bitstream_get_any_field(
                                                   header,
                                                   start_bit,
                                                   SOC_SAND_PP_ETHER_TYPE_NOF_BITS,
                                                   &tmp
                                                   );
    BCM_SAND_IF_ERR_EXIT_NO_UNIT(soc_sand_rv);


    header_info->ether_type = (uint16)tmp;
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}



int bcm_petra_l2_learn_msgs_config_set(
                                       int unit,
                                       bcm_l2_learn_msgs_config_t *learn_msg_config)
{
    SOC_TMC_PORTS_ITMH olp_port_itmh;
    SOC_TMC_HPU_ITMH_HDR olp_itmh_stream;
    SOC_TMC_PORTS_OTMH_EXTENSIONS_EN otmh_extensions_en;
    int start_bit = 0;
    uint8 use_vlan = 0;
    SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO distr_info;
    SOC_TMC_MULT_ID mc_id;
    bcm_error_t     rv = BCM_E_NONE;
    int soc_sand_rv;
    unsigned int soc_sand_dev_id;
    soc_info_t  *si;
    SOC_TMC_PORT_HEADER_TYPE olp_receive_header_type;
    SOC_TMC_PORT_HEADER_TYPE olp_transmit_header_type;
    uint32 user_header_size, user_header_0_size, user_header_1_size;
    uint32 user_header_egress_pmf_offset_0_dummy, user_header_egress_pmf_offset_1_dummy;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _DPP_L2_INIT_CHECK(unit);
    BCMDNX_NULL_CHECK(learn_msg_config);

    si  = &SOC_INFO(unit);

    /* check params */

    /* has to update at least one message queue*/
    if(!(learn_msg_config->flags & BCM_L2_LEARN_MSG_LEARNING) && !(learn_msg_config->flags & BCM_L2_LEARN_MSG_SHADOW)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Need to set at least one of flags BCM_L2_LEARN_MSG_SHADOW BCM_L2_LEARN_MSG_LEARNING")));
    }

    if (learn_msg_config->color > 3) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Color should be in the range 0..3")));
    }

    if (learn_msg_config->priority > 7) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Priority should be in the range 0..7")));
    }

    if(BCM_VLAN_VALID(learn_msg_config->vlan))
    {
        use_vlan = 1;
    }

    SOC_TMC_PORTS_ITMH_clear(&olp_port_itmh);
    SOC_TMC_HPU_ITMH_HDR_clear(&olp_itmh_stream);
    SOC_TMC_PORTS_OTMH_EXTENSIONS_EN_clear(&otmh_extensions_en);
    SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO_clear(&distr_info);

    soc_sand_dev_id = (unit);

    /* destination is multicast */
    if(learn_msg_config->flags & BCM_L2_LEARN_MSG_DEST_MULTICAST) {

        /* set type and value */
        olp_port_itmh.base.destination.type = SOC_TMC_DEST_TYPE_MULTICAST;
        rv = _bcm_petra_multicast_group_to_id(learn_msg_config->dest_group,&mc_id);
        BCMDNX_IF_ERR_EXIT(rv);

        olp_port_itmh.base.destination.id = mc_id;
    }
    else /* gport destination */
    {
        /* map gport to tm destination */
        rv = _bcm_dpp_gport_to_tm_dest_info(unit,learn_msg_config->dest_port,&(olp_port_itmh.base.destination));
        BCMDNX_IF_ERR_EXIT(rv);
    }

    olp_port_itmh.base.dp = learn_msg_config->color;
    olp_port_itmh.base.tr_cls = learn_msg_config->priority;

    /* build the ITMH header to be attached to learning messages */
    soc_sand_rv = arad_hpu_itmh_build(
                                      soc_sand_dev_id, 
                                      &olp_port_itmh,
                                      &olp_itmh_stream
                                      );
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    start_bit = SOC_PPC_FRWRD_MACT_MSG_HDR_SIZE * 32; /* SOC_SAND_U32_NOF_BITS; */

    distr_info.header[SOC_PPC_FRWRD_MACT_MSG_HDR_SIZE - 1] = olp_itmh_stream.base;
    start_bit -= 32; /* tm header base size */

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "injection_with_user_header_enable", 0) == 1) {
        soc_sand_rv = arad_pmf_db_fes_user_header_sizes_get(
                 unit,
                 &user_header_0_size,
                 &user_header_1_size,
                 &user_header_egress_pmf_offset_0_dummy,
                 &user_header_egress_pmf_offset_1_dummy
                 );
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        user_header_size = (user_header_0_size + user_header_1_size);
        start_bit -= user_header_size;
    }

    rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.header_type_out.get(unit,si->olp_port[0],&olp_receive_header_type);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.header_type_in.get(unit,si->olp_port[0],&olp_transmit_header_type);
    BCMDNX_IF_ERR_EXIT(rv);

    /* set L2 encapsulation if exist */
    if(learn_msg_config->flags & BCM_L2_LEARN_MSG_ETH_ENCAP)
    {
        rv = _bcm_l2_pkt_eth_header_build(learn_msg_config,start_bit,distr_info.header);
        BCMDNX_IF_ERR_EXIT(rv);

        if (olp_receive_header_type == SOC_TMC_PORT_HEADER_TYPE_RAW)
        {
            distr_info.olp_receive_header_type = SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_ETH_O_RAW;
        }
        else
        {
            distr_info.olp_receive_header_type = SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_ETH_O_OTMH;
        }

        if (olp_transmit_header_type == SOC_TMC_PORT_HEADER_TYPE_RAW)
        {
            distr_info.header_type = SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_ETH_O_RAW;
        }
        else
        {
            if (use_vlan) 
            {
                distr_info.header_type = SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_ETH_VLAN_O_ITMH; 
            }
            else
            {
                distr_info.header_type = SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_ETH_O_ITMH; 
            }
        }
    }
    else
    {
        if (olp_receive_header_type == SOC_TMC_PORT_HEADER_TYPE_RAW)
        {
            distr_info.olp_receive_header_type = SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_RAW;
        }
        else
        {
            distr_info.olp_receive_header_type = SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_OTMH;
        }

        if (olp_transmit_header_type == SOC_TMC_PORT_HEADER_TYPE_RAW)
        {
            distr_info.header_type = SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_RAW;
        }
        else
        {
            distr_info.header_type = SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_ITMH;
        }
    }

    /* set information for learning queue */
    if(learn_msg_config->flags & BCM_L2_LEARN_MSG_LEARNING) {
        soc_sand_rv = soc_ppd_frwrd_mact_learn_msgs_distribution_info_set(soc_sand_dev_id,&distr_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
    /* set information for shadow queue */
    if(learn_msg_config->flags & BCM_L2_LEARN_MSG_SHADOW) {
        soc_sand_rv = soc_ppd_frwrd_mact_shadow_msgs_distribution_info_set(soc_sand_dev_id,&distr_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }


    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}



int bcm_petra_l2_learn_msgs_config_get(
                                       int unit,
                                       bcm_l2_learn_msgs_config_t *learn_msg_config)
{
    SOC_TMC_PORTS_ITMH olp_port_itmh;
    SOC_TMC_HPU_ITMH_HDR olp_itmh_stream;
    SOC_TMC_PORTS_OTMH_EXTENSIONS_EN otmh_extensions_en;
    int start_bit = 0;
    int     rv = BCM_E_NONE;
    unsigned int soc_sand_dev_id;
    SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO distr_info;
    int soc_sand_rv;
    uint32 user_header_size, user_header_0_size, user_header_1_size;
    uint32 user_header_egress_pmf_offset_0_dummy, user_header_egress_pmf_offset_1_dummy;
    
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _DPP_L2_INIT_CHECK(unit);
    BCMDNX_NULL_CHECK(learn_msg_config);

    /* check params */

    /* can get exactly one queue */
    if(!(learn_msg_config->flags & BCM_L2_LEARN_MSG_LEARNING) && !(learn_msg_config->flags & BCM_L2_LEARN_MSG_SHADOW)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Need to set exactly one of the flags BCM_L2_LEARN_MSG_SHADOW BCM_L2_LEARN_MSG_LEARNING")));
    }
    if((learn_msg_config->flags & BCM_L2_LEARN_MSG_LEARNING) && (learn_msg_config->flags & BCM_L2_LEARN_MSG_SHADOW)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Need to set exactly one of the flags BCM_L2_LEARN_MSG_SHADOW BCM_L2_LEARN_MSG_LEARNING")));
    }

    SOC_TMC_PORTS_ITMH_clear(&olp_port_itmh);
    SOC_TMC_HPU_ITMH_HDR_clear(&olp_itmh_stream);
    SOC_TMC_PORTS_OTMH_EXTENSIONS_EN_clear(&otmh_extensions_en);
    SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO_clear(&distr_info);

    soc_sand_dev_id = (unit);

    /* get information for learning queue */
    if(learn_msg_config->flags & BCM_L2_LEARN_MSG_LEARNING) {
        soc_sand_rv = soc_ppd_frwrd_mact_learn_msgs_distribution_info_get(soc_sand_dev_id,&distr_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
    /* get information for shadow queue */
    if(learn_msg_config->flags & BCM_L2_LEARN_MSG_SHADOW) {
        soc_sand_rv = soc_ppd_frwrd_mact_shadow_msgs_distribution_info_get(soc_sand_dev_id,&distr_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
    /* get ITMH header */

    start_bit = SOC_PPC_FRWRD_MACT_MSG_HDR_SIZE * 32; /* SOC_SAND_U32_NOF_BITS */

    olp_itmh_stream.base = distr_info.header[SOC_PPC_FRWRD_MACT_MSG_HDR_SIZE - 1];
    start_bit -= 32; /* tm header base size */

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "injection_with_user_header_enable", 0) == 1) {
        soc_sand_rv = arad_pmf_db_fes_user_header_sizes_get(
                 unit,
                 &user_header_0_size,
                 &user_header_1_size,
                 &user_header_egress_pmf_offset_0_dummy,
                 &user_header_egress_pmf_offset_1_dummy
                 );
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        user_header_size = (user_header_0_size + user_header_1_size);
        start_bit -= user_header_size;
    }

    /* parse the ITMH header to be attached to learning messages */
    soc_sand_rv = arad_hpu_itmh_parse(
                                      soc_sand_dev_id, 
                                      &olp_itmh_stream,
                                      &olp_port_itmh
                                      );
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    learn_msg_config->priority = olp_port_itmh.base.tr_cls;
    learn_msg_config->color = olp_port_itmh.base.dp;


    /* get vlan information if relevant */
    if(distr_info.header_type == SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_ETH_O_ITMH ||
       distr_info.header_type == SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_ETH_VLAN_O_ITMH)
    {
        learn_msg_config->flags |= BCM_L2_LEARN_MSG_ETH_ENCAP;
        rv = _bcm_l2_pkt_eth_header_parse(learn_msg_config, start_bit, distr_info.header, distr_info.header_type);
        BCMDNX_IF_ERR_EXIT(rv);
    }


    /* destination is multicast */
    if(olp_port_itmh.base.destination.type == SOC_TMC_DEST_TYPE_MULTICAST) {
        learn_msg_config->flags |= BCM_L2_LEARN_MSG_DEST_MULTICAST;

        /* set type and value */
        olp_port_itmh.base.destination.type = SOC_TMC_DEST_TYPE_MULTICAST;
        rv = _bcm_petra_multicast_group_from_id(&learn_msg_config->dest_group, _BCM_PETRA_MULTICAST_TM_TYPE, olp_port_itmh.base.destination.id);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else /* gport destination */
    {
        /* map gport to tm destination */
        rv = _bcm_dpp_gport_from_tm_dest_info(unit,&learn_msg_config->dest_port,&(olp_port_itmh.base.destination));
        BCMDNX_IF_ERR_EXIT(rv);
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * from learn flags build buffer, 
 * assuming each flags up to 3 bits. 
 */
STATIC
int _bcm_petra_l2_event_handle_buff_build(
                                          int learn_flags, int trans_flags, int del_flags, int refresh_flags, int *buf)
{
    int tmp;
    int action_flags = (BCM_L2_ADDR_DIST_SET_LEARN_DISTRIBUTER|BCM_L2_ADDR_DIST_SET_SHADOW_DISTRIBUTER|
                        BCM_L2_ADDR_DIST_SET_CPU_DMA_DISTRIBUTER);
    int bits_per_event = 8;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;
    tmp = learn_flags & action_flags;
    tmp = tmp << bits_per_event;
    tmp |= (trans_flags & action_flags);
    tmp = tmp << bits_per_event;
    tmp |= (del_flags & action_flags);
    tmp = tmp << bits_per_event;
    tmp |= (refresh_flags & action_flags);

    /* buf[0-4] = del_flags  buf[5-9] = trans_flags  buf[10-14] = learn_flags */
    *buf = tmp;

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_petra_l2_event_handle_profile_alloc(
                                             int unit, int check, int vsi, int learn_flags, int trans_flags,
                                             int del_flags, int refresh_flags, int *new_learn_profile, int *first_appear)
{
    int buf;
    int alloc_flags = 0;
    int is_last;
    int rv = BCM_E_NONE;
    int old_profile;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _DPP_L2_INIT_CHECK(unit);

    if(check != 0) {
        alloc_flags |= BCM_DPP_AM_TEMPLATE_FLAG_CHECK;
    }
    /* build buffer */

    rv = _bcm_petra_l2_event_handle_buff_build(learn_flags,trans_flags,del_flags, refresh_flags, &buf);
    BCMDNX_IF_ERR_EXIT(rv);

    /* exchange for behavior for given fid-profile */
    rv = _bcm_dpp_am_template_l2_event_handle_exchange(unit,alloc_flags,vsi,buf,&old_profile,&is_last,new_learn_profile,first_appear);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_l2_addr_msg_distribute_set(
                                         int unit,
                                         bcm_l2_addr_distribute_t *distribution)
{
    SOC_PPC_VSI_ID vsi;
    SOC_PPC_VSI_INFO vsi_info;
    int first_handle_appear = 0;
    int rv = BCM_E_NONE;
    uint32 soc_sand_rv;
    int  old_limit, old_learn_profile;
    unsigned int soc_sand_dev_id;
    bcm_l2_addr_distribute_t
        old_distribution;
    int learn_flags = 0, trans_flags = 0, del_flags = 0, age_refresh_flags = 0, new_handle_profile = 0;
    int update_without_add_flags = 0;
    SOC_PPC_FRWRD_MACT_EVENT_HANDLE_KEY handle_key;
    SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO handle_info;
    int learn_profile_first_appear = 0;
    SOC_PPC_FRWRD_MACT_MAC_LIMIT_INFO limit_info;
    int old_handle_profile,old_aging_profile;
    int new_learn_profile;
    int is_last;
    uint8 distribute_per_lif = 0;
    _bcm_dpp_gport_hw_resources gport_hw_resources;
    uint32 limit_tbl_idx;
    uint32 is_limit_tbl_idx_reserved;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _DPP_L2_INIT_CHECK(unit);
    soc_sand_dev_id = (unit);

    SOC_PPC_FRWRD_MACT_EVENT_HANDLE_KEY_clear(&handle_key);
    SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO_clear(&handle_info);
    
    BCMDNX_NULL_CHECK(distribution);

    distribute_per_lif = (distribution->flags & BCM_L2_ADDR_DIST_GPORT) ? 1 : 0;
    sal_memset(&gport_hw_resources, 0, sizeof(_bcm_dpp_gport_hw_resources));

    if (distribute_per_lif)
    {
        if (SOC_PPC_FRWRD_MACT_LEARN_LIMIT_MODE != SOC_PPC_FRWRD_MACT_LEARN_LIMIT_TYPE_LIF)
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("LIF distribution doesn't match the SOC property L2_LEARN_LIMIT_MODE")));
        }

        /* Convert from gport to LIF */
        rv = _bcm_dpp_gport_to_hw_resources(unit, distribution->gport, _BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK,
                                                 &gport_hw_resources);
        BCMDNX_IF_ERR_EXIT(rv);
        soc_sand_rv = soc_ppd_frwrd_mact_limit_mapped_val_to_table_index_get(soc_sand_dev_id,
                                                                             gport_hw_resources.global_in_lif, &limit_tbl_idx, &is_limit_tbl_idx_reserved);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        /*
         * Don't allow the set, if the mapping leads to the reserved entry for invalid mappings.
         * There's no way to know if it was an explicit limit for the invalid mapping or misconfiguration.
         */
        if (is_limit_tbl_idx_reserved)
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("The LIF is out of range for MACT Limit setting")));
        }
        /*
         * Currently the limit table is treated as a per VSI table.
         * Therefore, the Limit table index is used as a VSI value.
         */
        vsi = limit_tbl_idx;
        soc_sand_rv = soc_ppd_vsi_info_get(soc_sand_dev_id, vsi, &vsi_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* get old information attached with given vsi*/
        rv = _bcm_dpp_am_template_l2_learn_profile_data_get(unit, vsi, &old_limit, &old_handle_profile, &old_aging_profile);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else if (!_BCM_PETRA_L2_EVENT_DIST_IS_DEFAULT(distribution->vid) && !distribute_per_lif)
    {
        if (SOC_PPC_FRWRD_MACT_LEARN_LIMIT_MODE == SOC_PPC_FRWRD_MACT_LEARN_LIMIT_TYPE_LIF)
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("LIF distribution doesn't match the SOC property L2_LEARN_LIMIT_MODE")));
        }

        rv = _bcm_dpp_vlan_check(unit, distribution->vid);
        BCMDNX_IF_ERR_EXIT(rv);

        /* map VID to VSI*/
        rv = _bcm_petra_vid_to_vsi(unit,distribution->vid,&vsi);
        BCMDNX_IF_ERR_EXIT(rv);

        soc_sand_rv = soc_ppd_vsi_info_get(soc_sand_dev_id, vsi, &vsi_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* get old information attached with given vsi*/
        rv = _bcm_dpp_am_template_l2_learn_profile_data_get(unit, vsi, &old_limit, &old_handle_profile, &old_aging_profile);
        BCMDNX_IF_ERR_EXIT(rv); 
    }
    else
    {
        /* For global event dist, always update event dist for default fid_profole */
        first_handle_appear = 1;
        learn_profile_first_appear = 1;
        old_learn_profile = 0;
        new_learn_profile = 0;
        old_handle_profile = 0;
        old_aging_profile = 0;
        new_handle_profile = 0;
    }

    sal_memcpy(&old_distribution,distribution, sizeof(bcm_l2_addr_distribute_t));

    /* get old setting if not valid in current configuration */
    if(!(distribution->flags & BCM_L2_ADDR_DIST_LEARN_EVENT)) {
        old_distribution.flags = BCM_L2_ADDR_DIST_LEARN_EVENT;
        if (distribute_per_lif)
        {
            old_distribution.flags |= BCM_L2_ADDR_DIST_GPORT; 
        }
        rv = bcm_petra_l2_addr_msg_distribute_get(unit,&old_distribution);
        BCMDNX_IF_ERR_EXIT(rv);

        learn_flags = old_distribution.flags;
    }
    else{
        learn_flags = distribution->flags;
    }

    if(!(distribution->flags & BCM_L2_ADDR_DIST_STATION_MOVE_EVENT)) {
        old_distribution.flags = BCM_L2_ADDR_DIST_STATION_MOVE_EVENT;
        if (distribute_per_lif)
        {
            old_distribution.flags |= BCM_L2_ADDR_DIST_GPORT; 
        }
        rv = bcm_petra_l2_addr_msg_distribute_get(unit,&old_distribution);
        BCMDNX_IF_ERR_EXIT(rv);

        trans_flags = old_distribution.flags;
    }
    else{
        trans_flags = distribution->flags;
    }

    if(!(distribution->flags & BCM_L2_ADDR_DIST_AGED_OUT_EVENT)) {
        old_distribution.flags = BCM_L2_ADDR_DIST_AGED_OUT_EVENT;
        if (distribute_per_lif)
        {
            old_distribution.flags |= BCM_L2_ADDR_DIST_GPORT; 
        }
        rv = bcm_petra_l2_addr_msg_distribute_get(unit,&old_distribution);
        BCMDNX_IF_ERR_EXIT(rv);

        del_flags = old_distribution.flags;
    }
    else{
        del_flags = distribution->flags;
    }

    if(!(distribution->flags & BCM_L2_ADDR_DIST_REFRESH_EVENT)) {
        old_distribution.flags = BCM_L2_ADDR_DIST_REFRESH_EVENT;
        if (distribute_per_lif)
        {
            old_distribution.flags |= BCM_L2_ADDR_DIST_GPORT; 
        }
        rv = bcm_petra_l2_addr_msg_distribute_get(unit,&old_distribution);
        BCMDNX_IF_ERR_EXIT(rv);

        age_refresh_flags = old_distribution.flags;
    }
    else{
        age_refresh_flags = distribution->flags;
    }

    if(!(distribution->flags & BCM_L2_ADDR_DIST_UPDATE_WITHOUT_ADD_EVENT)) {
        old_distribution.flags = BCM_L2_ADDR_DIST_UPDATE_WITHOUT_ADD_EVENT;
        if (distribute_per_lif)
        {
            old_distribution.flags |= BCM_L2_ADDR_DIST_GPORT; 
        }
        rv = bcm_petra_l2_addr_msg_distribute_get(unit,&old_distribution);
        BCMDNX_IF_ERR_EXIT(rv);

        update_without_add_flags = old_distribution.flags;
    }
    else{
        /* change-fail event flag is always attached to transplant or refresh event */
        if(!(distribution->flags & (BCM_L2_ADDR_DIST_STATION_MOVE_EVENT | BCM_L2_ADDR_DIST_REFRESH_EVENT))) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Cannot set distributer only for change-fail event type ")));
        }
        update_without_add_flags = distribution->flags;
    }


    if (!_BCM_PETRA_L2_EVENT_DIST_IS_DEFAULT(distribution->vid))
    {
        /* only check if there is available event handle profile */
        /* for vsi allocate event handle profile with given learning info */
        rv = _bcm_petra_l2_event_handle_profile_alloc(unit, TRUE, vsi,learn_flags, trans_flags, del_flags, age_refresh_flags,
                                                                                        &new_handle_profile, &first_handle_appear);
        BCMDNX_IF_ERR_EXIT(rv);

        /* allocate fid-profile with new learn event, old-limit and old aging-profile */
        rv = _bcm_dpp_am_template_l2_learn_profile_exchange(unit, 0, vsi, old_limit, new_handle_profile,
                                                                                     old_aging_profile, &old_learn_profile, &is_last,
                                                                                     &new_learn_profile,&learn_profile_first_appear);
        BCMDNX_IF_ERR_EXIT(rv);

        /* now that all available allocate event profile */
        rv = _bcm_petra_l2_event_handle_profile_alloc(unit, FALSE, vsi, learn_flags, trans_flags, del_flags, age_refresh_flags,
                                                                                           &new_handle_profile, &first_handle_appear);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* change-fail configuration shouldn't be set without changing event handle profile */
    if((distribution->flags & BCM_L2_ADDR_DIST_UPDATE_WITHOUT_ADD_EVENT) && (first_handle_appear == 0)) {
        if(update_without_add_flags & BCM_L2_ADDR_DIST_SET_LEARN_DISTRIBUTER) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Cannot set learn distributer only for change-fail event type")));
        }
        if(update_without_add_flags & BCM_L2_ADDR_DIST_SET_SHADOW_DISTRIBUTER) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Cannot set shadow distributer only for change-fail event type")));
        }
        if(update_without_add_flags & BCM_L2_ADDR_DIST_SET_CPU_DMA_DISTRIBUTER) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Cannot set cpu dma distributer only for change-fail event type")));
        }
    }

    /*
     * if first appear of event handle profile update hw
     */
    if(first_handle_appear != 0) {
    
        handle_key.vsi_event_handle_profile = new_handle_profile;
        for (handle_key.is_lag = 0; handle_key.is_lag <= 1; ++handle_key.is_lag)
        {
            /* learn */
            handle_key.event_type = SOC_PPC_FRWRD_MACT_EVENT_TYPE_LEARN;
            soc_sand_rv = soc_ppd_frwrd_mact_event_handle_info_get(soc_sand_dev_id,&handle_key,&handle_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            handle_info.send_to_learning_fifo = (learn_flags & BCM_L2_ADDR_DIST_SET_LEARN_DISTRIBUTER)?1:0;
            handle_info.send_to_shadow_fifo = (learn_flags & BCM_L2_ADDR_DIST_SET_SHADOW_DISTRIBUTER)?1:0;
            handle_info.send_to_cpu_dma_fifo = (learn_flags & BCM_L2_ADDR_DIST_SET_CPU_DMA_DISTRIBUTER)?1:0;
            soc_sand_rv = soc_ppd_frwrd_mact_event_handle_info_set(soc_sand_dev_id,&handle_key,&handle_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            /* transplant */
            handle_key.event_type = SOC_PPC_FRWRD_MACT_EVENT_TYPE_TRANSPLANT;
            soc_sand_rv = soc_ppd_frwrd_mact_event_handle_info_get(soc_sand_dev_id,&handle_key,&handle_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            handle_info.send_to_learning_fifo = (trans_flags & BCM_L2_ADDR_DIST_SET_LEARN_DISTRIBUTER)?1:0;
            handle_info.send_to_shadow_fifo = (trans_flags & BCM_L2_ADDR_DIST_SET_SHADOW_DISTRIBUTER)?1:0;
            handle_info.send_to_cpu_dma_fifo = (trans_flags & BCM_L2_ADDR_DIST_SET_CPU_DMA_DISTRIBUTER)?1:0;
            soc_sand_rv = soc_ppd_frwrd_mact_event_handle_info_set(soc_sand_dev_id,&handle_key,&handle_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            /* aged-out*/
            handle_key.event_type = SOC_PPC_FRWRD_MACT_EVENT_TYPE_AGED_OUT;
            soc_sand_rv = soc_ppd_frwrd_mact_event_handle_info_get(soc_sand_dev_id,&handle_key,&handle_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            handle_info.send_to_learning_fifo = (del_flags & BCM_L2_ADDR_DIST_SET_LEARN_DISTRIBUTER)?1:0;
            handle_info.send_to_shadow_fifo = (del_flags & BCM_L2_ADDR_DIST_SET_SHADOW_DISTRIBUTER)?1:0;
            handle_info.send_to_cpu_dma_fifo = (del_flags & BCM_L2_ADDR_DIST_SET_CPU_DMA_DISTRIBUTER)?1:0;
            soc_sand_rv = soc_ppd_frwrd_mact_event_handle_info_set(soc_sand_dev_id,&handle_key,&handle_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            /* learning age refresh */
            handle_key.event_type = SOC_PPC_FRWRD_MACT_EVENT_TYPE_REFRESH;
            soc_sand_rv = soc_ppd_frwrd_mact_event_handle_info_get(soc_sand_dev_id,&handle_key,&handle_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            handle_info.send_to_learning_fifo = (age_refresh_flags & BCM_L2_ADDR_DIST_SET_LEARN_DISTRIBUTER)?1:0;
            handle_info.send_to_shadow_fifo = (age_refresh_flags & BCM_L2_ADDR_DIST_SET_SHADOW_DISTRIBUTER)?1:0;
            handle_info.send_to_cpu_dma_fifo = (age_refresh_flags & BCM_L2_ADDR_DIST_SET_CPU_DMA_DISTRIBUTER)?1:0;
            soc_sand_rv = soc_ppd_frwrd_mact_event_handle_info_set(soc_sand_dev_id,&handle_key,&handle_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            /* change-fail */
            handle_key.event_type = SOC_PPC_FRWRD_MACT_EVENT_TYPE_REQ_FAIL;
            soc_sand_rv = soc_ppd_frwrd_mact_event_handle_info_get(soc_sand_dev_id,&handle_key,&handle_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            handle_info.send_to_learning_fifo = (update_without_add_flags & BCM_L2_ADDR_DIST_SET_LEARN_DISTRIBUTER)?1:0;
            handle_info.send_to_shadow_fifo = (update_without_add_flags & BCM_L2_ADDR_DIST_SET_SHADOW_DISTRIBUTER)?1:0;
            handle_info.send_to_cpu_dma_fifo = (update_without_add_flags & BCM_L2_ADDR_DIST_SET_CPU_DMA_DISTRIBUTER)?1:0;
            soc_sand_rv = soc_ppd_frwrd_mact_event_handle_info_set(soc_sand_dev_id,&handle_key,&handle_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }


    /* if first appear of fid-learn-profile then configure profile*/
    if (learn_profile_first_appear) {

        /* update fid-learn-profile to event-handle profile */
        soc_sand_rv = soc_ppd_frwrd_mact_event_handle_profile_set(soc_sand_dev_id,new_learn_profile,new_handle_profile);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        
        /* update fid-learn-profile to fid-aging profile */
        soc_sand_rv = soc_ppd_frwrd_mact_fid_aging_profile_set(soc_sand_dev_id,new_learn_profile,old_aging_profile);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* update fid-learn-profile to limit, copy from old index */
        soc_sand_rv = soc_ppd_frwrd_mact_learn_profile_limit_info_get(soc_sand_dev_id,old_learn_profile,&limit_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        soc_sand_rv = soc_ppd_frwrd_mact_learn_profile_limit_info_set(soc_sand_dev_id,new_learn_profile,&limit_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

    /* update vsi */
    if (!_BCM_PETRA_L2_EVENT_DIST_IS_DEFAULT(distribution->vid))
    {
        vsi_info.mac_learn_profile_id = new_learn_profile;
        soc_sand_rv = soc_ppd_vsi_info_set(soc_sand_dev_id, vsi, &vsi_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


int bcm_petra_l2_addr_msg_distribute_get(
                                         int unit,
                                         bcm_l2_addr_distribute_t *distribution)
{
    SOC_PPC_VSI_ID vsi;
    SOC_PPC_VSI_INFO vsi_info;
    SOC_PPC_FRWRD_MACT_EVENT_HANDLE_KEY handle_key;
    SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO handle_info;
    int event_set=0;
    int rv = BCM_E_NONE;
    uint32 soc_sand_rv;
    unsigned int soc_sand_dev_id;

    uint8 distribute_per_lif = 0;
    _bcm_dpp_gport_hw_resources gport_hw_resources;
    uint32 limit_tbl_idx;
    uint32 is_limit_tbl_idx_reserved;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _DPP_L2_INIT_CHECK(unit);
    BCMDNX_NULL_CHECK(distribution);

    soc_sand_dev_id = (unit);
    SOC_PPC_FRWRD_MACT_EVENT_HANDLE_KEY_clear(&handle_key);
    SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO_clear(&handle_info);
    SOC_PPC_VSI_INFO_clear(&vsi_info);

    distribute_per_lif = (distribution->flags & BCM_L2_ADDR_DIST_GPORT) ? 1 : 0;
    sal_memset(&gport_hw_resources, 0, sizeof(_bcm_dpp_gport_hw_resources));

    if (distribute_per_lif)
    {
        if (SOC_PPC_FRWRD_MACT_LEARN_LIMIT_MODE != SOC_PPC_FRWRD_MACT_LEARN_LIMIT_TYPE_LIF)
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("LIF distribution doesn't match the SOC property L2_LEARN_LIMIT_MODE")));
        }

        /* Convert from gport to LIF */
        rv = _bcm_dpp_gport_to_hw_resources(unit, distribution->gport, _BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK,
                                                 &gport_hw_resources);
        BCMDNX_IF_ERR_EXIT(rv);
        soc_sand_rv = soc_ppd_frwrd_mact_limit_mapped_val_to_table_index_get(soc_sand_dev_id,
                                                                             gport_hw_resources.global_in_lif, &limit_tbl_idx, &is_limit_tbl_idx_reserved);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        /* 
         * Don't allow the set, if the mapping leads to the reserved entry for invalid mappings.
         * There's no way to know if it was an explicit limit for the invalid mapping or misconfiguration.
         */
        if (is_limit_tbl_idx_reserved)
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("The LIF is out of range for MACT Limit setting")));
        }
        /*
         * Currently the limit table is treated as a per VSI table.
         * Therefore, the Limit table index is used as a VSI value.
         */
        vsi = limit_tbl_idx;
        soc_sand_rv = soc_ppd_vsi_info_get(soc_sand_dev_id, vsi, &vsi_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
    else if (!_BCM_PETRA_L2_EVENT_DIST_IS_DEFAULT(distribution->vid) && !distribute_per_lif)
    {
        if (SOC_PPC_FRWRD_MACT_LEARN_LIMIT_MODE == SOC_PPC_FRWRD_MACT_LEARN_LIMIT_TYPE_LIF)
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("LIF distribution doesn't match the SOC property L2_LEARN_LIMIT_MODE")));
        }

        rv = _bcm_dpp_vlan_check(unit, distribution->vid);
        BCMDNX_IF_ERR_EXIT(rv);

        /* get exist event handle profile */
        /* map VID to VSI*/
        rv = _bcm_petra_vid_to_vsi(unit,distribution->vid,&vsi);
        BCMDNX_IF_ERR_EXIT(rv);

        soc_sand_rv = soc_ppd_vsi_info_get(soc_sand_dev_id, vsi, &vsi_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

    soc_sand_rv = soc_ppd_frwrd_mact_event_handle_profile_get(soc_sand_dev_id, vsi_info.mac_learn_profile_id, &handle_key.vsi_event_handle_profile);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* learn  */
    if(distribution->flags & BCM_L2_ADDR_DIST_LEARN_EVENT) {
        handle_key.event_type = SOC_PPC_FRWRD_MACT_EVENT_TYPE_LEARN;
        event_set = 1;
    }

    /* transplant  */
    if(distribution->flags & BCM_L2_ADDR_DIST_STATION_MOVE_EVENT) {
        if(event_set == 1) {
            /* in get only one event type should be set, to avoid conflict between different types */
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Cannot get configuration only for one event type "))); 
        }
        handle_key.event_type = SOC_PPC_FRWRD_MACT_EVENT_TYPE_TRANSPLANT;
        event_set = 1;
    }

    /* aged-out */
    if(distribution->flags & BCM_L2_ADDR_DIST_AGED_OUT_EVENT) {
        if(event_set == 1) {
            /* in get only one event type should be set, to avoid conflict between different types */
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Cannot get configuration only for one event type "))); 
        }
        handle_key.event_type = SOC_PPC_FRWRD_MACT_EVENT_TYPE_AGED_OUT;
        event_set = 1;
    }

    /* Aging of learning refreshed */
    if(distribution->flags & BCM_L2_ADDR_DIST_REFRESH_EVENT) {
        if(event_set == 1) {
            /* in get only one event type should be set, to avoid conflict between different types */
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Cannot get configuration only for one event type ")));
        }
        handle_key.event_type = SOC_PPC_FRWRD_MACT_EVENT_TYPE_REFRESH;
        event_set = 1;
    }

    /* change-fail */
    if(distribution->flags & BCM_L2_ADDR_DIST_UPDATE_WITHOUT_ADD_EVENT) {
        if(event_set == 1) {
            /* in get only one event type should be set, to avoid conflict between different types */
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Cannot get configuration only for one event type ")));
        }
        handle_key.event_type = SOC_PPC_FRWRD_MACT_EVENT_TYPE_REQ_FAIL;
        event_set = 1;
    }

    soc_sand_rv = soc_ppd_frwrd_mact_event_handle_info_get(soc_sand_dev_id,&handle_key,&handle_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if(handle_info.send_to_learning_fifo) {
        distribution->flags |= BCM_L2_ADDR_DIST_SET_LEARN_DISTRIBUTER;
    }
    if(handle_info.send_to_shadow_fifo) {
        distribution->flags |= BCM_L2_ADDR_DIST_SET_SHADOW_DISTRIBUTER;
    }
    if(handle_info.send_to_cpu_dma_fifo) {
        distribution->flags |= BCM_L2_ADDR_DIST_SET_CPU_DMA_DISTRIBUTER;
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_l2_gport_control_info_set(
                                         int unit,
                                         bcm_l2_gport_control_info_t *control_info)
{
    SOC_PPC_VSI_ID vsi;
    SOC_PPC_VSI_INFO vsi_info;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    int rv;
    int old_learn_profile,old_limit, old_handle_profile, old_aging_profile;
    int update_aging_profile = 1; /* This flag gets 0 when entropy_id and aging_cycles are 0. In this case the aging configuration is skipped */
    int update_aging_config_tbl = 1;
    int is_last;
    int learn_profile_first_appear;
    int new_learn_profile,new_aging_profile;
    uint32 limit_tbl_idx;
    uint32 is_limit_tbl_idx_reserved;
    _bcm_dpp_gport_hw_resources gport_hw_resources;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _DPP_L2_INIT_CHECK(unit);
    BCMDNX_NULL_CHECK(control_info);

    soc_sand_dev_id = (unit);
    sal_memset(&gport_hw_resources, 0, sizeof(_bcm_dpp_gport_hw_resources));

    if (SOC_PPC_FRWRD_MACT_LEARN_LIMIT_MODE != SOC_PPC_FRWRD_MACT_LEARN_LIMIT_TYPE_LIF)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("LIF Ageing configuration doesn't match the SOC property L2_LEARN_LIMIT_MODE")));
    }

    /* Convert from gport to LIF */
    rv = _bcm_dpp_gport_to_hw_resources(unit, control_info->gport, _BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK,
                                             &gport_hw_resources);
    BCMDNX_IF_ERR_EXIT(rv);
    soc_sand_rv = soc_ppd_frwrd_mact_limit_mapped_val_to_table_index_get(soc_sand_dev_id,
                                                                         gport_hw_resources.global_in_lif, &limit_tbl_idx, &is_limit_tbl_idx_reserved);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    /*
     * Don't allow the set, if the mapping leads to the reserved entry for invalid mappings.
     * There's no way to know if it was an explicit limit for the invalid mapping or misconfiguration.
     */
    if (is_limit_tbl_idx_reserved)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("The LIF is out of range for MACT Limit setting")));
    }
    /*
     * Currently the limit table is treated as a per VSI table.
     * Therefore, the Limit table index is used as a VSI value.
     */
    vsi = limit_tbl_idx;
    soc_sand_rv = soc_ppd_vsi_info_get(soc_sand_dev_id, vsi, &vsi_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* In this case skip the aging configuration */
    if (control_info->entropy_id == 0 && control_info->aging_cycles == 0)
    {
        update_aging_profile = 0;
        update_aging_config_tbl = 0;
    }
    /* A special case of freezing the aging of the given LIF */
    else if (control_info->entropy_id == 0 && control_info->aging_cycles == DPP_AGING_CYCLES_IGNORE_VALUE) 
    {
        update_aging_profile = 1;
        control_info->aging_cycles = 0;
        update_aging_config_tbl = 0;
    }

    if (control_info->entropy_id <= DPP_AGING_PROFILE_LAST_CONFIGURABLE && 
        control_info->entropy_id >= DPP_AGING_PROFILE_FIRST_CONFIGURABLE) 
    {
        if (control_info->aging_cycles > DPP_AGING_CYCLES_MAX || 
            control_info->aging_cycles < 0)
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Aging cycles is out of range [0, 6]")));
        }

        /* Configure to not do aging for aging_cycles = 0 */
        if (control_info->aging_cycles == 0)
        {
            control_info->aging_cycles = DPP_AGING_CYCLES_MAX + 1;
        }
    }
    else if (control_info->entropy_id == 0 && control_info->aging_cycles != 0) 
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("The given (entropy_id, aging_cycles) pair is illegal")));
    }

    /* Get old information attached with given vsi */
    rv = _bcm_dpp_am_template_l2_learn_profile_data_get(unit, vsi, &old_limit, &old_handle_profile, &old_aging_profile);
    BCMDNX_IF_ERR_EXIT(rv);
    /* Update the aging profile only if the aging profile is 1,2 or 3 */
    if (update_aging_profile)
    {
        /* Check if fid-aging profile is available */
        new_aging_profile =  control_info->entropy_id;
    }
    else
    {
        new_aging_profile = old_aging_profile;
    }
    /* Allocate fid-profile with new fid-aging-profile, old-limit and old handle event */
    rv = _bcm_dpp_am_template_l2_learn_profile_exchange(unit, FALSE, vsi, old_limit, old_handle_profile, new_aging_profile, &old_learn_profile, &is_last, &new_learn_profile,&learn_profile_first_appear);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Configure hw of fid aging profile */
    if (update_aging_config_tbl)
    {
        soc_sand_rv = soc_ppd_frwrd_mact_aging_profile_config(soc_sand_dev_id, new_aging_profile, control_info->aging_cycles);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
    /* If first appear of fid-learn-profile then configure profile */
    if (learn_profile_first_appear)
    {
        SOC_SAND_OUT SOC_PPC_FRWRD_MACT_MAC_LIMIT_INFO      limit_info;

        /* Update fid-learn-profile to fid-aging profile */
        soc_sand_rv = soc_ppd_frwrd_mact_fid_aging_profile_set(unit, new_learn_profile, new_aging_profile);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* Update fid-learn-profile to event-handle profile */
        soc_sand_rv = soc_ppd_frwrd_mact_event_handle_profile_set(soc_sand_dev_id, new_learn_profile, old_handle_profile);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* Update fid-learn-profile to limit, copy from old index */
        soc_sand_rv = soc_ppd_frwrd_mact_learn_profile_limit_info_get(soc_sand_dev_id, old_learn_profile, &limit_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        soc_sand_rv = soc_ppd_frwrd_mact_learn_profile_limit_info_set(soc_sand_dev_id, new_learn_profile, &limit_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
    /* Clean the old profile now that it is not in use. Use default values */
    if (is_last && old_learn_profile != new_learn_profile) 
    {
        SOC_SAND_OUT SOC_PPC_FRWRD_MACT_MAC_LIMIT_INFO      limit_info;
        /* Update fid-learn-profile to fid-aging profile */
        soc_sand_rv = soc_ppd_frwrd_mact_fid_aging_profile_set(unit,old_learn_profile, 0);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* Update fid-learn-profile to event-handle profile */
        soc_sand_rv = soc_ppd_frwrd_mact_event_handle_profile_set(soc_sand_dev_id,old_learn_profile, 0);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* Set default limit */
        SOC_PPC_FRWRD_MACT_MAC_LIMIT_INFO_clear(&limit_info);
        limit_info.action_when_exceed = SOC_PPC_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE_NONE;
        limit_info.nof_entries = 0;
        limit_info.is_limited = TRUE;
        soc_sand_rv = soc_ppd_frwrd_mact_learn_profile_limit_info_set(soc_sand_dev_id,old_learn_profile,&limit_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

    vsi_info.mac_learn_profile_id = new_learn_profile; 
    soc_sand_rv = soc_ppd_vsi_info_set(soc_sand_dev_id, vsi, &vsi_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_l2_gport_control_info_get(
                                         int unit,
                                         bcm_l2_gport_control_info_t *control_info)
{
    SOC_PPC_VSI_ID vsi;
    SOC_PPC_VSI_INFO vsi_info;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    int rv;
    uint32 limit_tbl_idx;
    uint32 is_limit_tbl_idx_reserved;
    _bcm_dpp_gport_hw_resources gport_hw_resources;
    int fid_aging_cycles = 0;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _DPP_L2_INIT_CHECK(unit);
    BCMDNX_NULL_CHECK(control_info);

    soc_sand_dev_id = (unit);
    sal_memset(&gport_hw_resources, 0, sizeof(_bcm_dpp_gport_hw_resources));

    if (SOC_PPC_FRWRD_MACT_LEARN_LIMIT_MODE != SOC_PPC_FRWRD_MACT_LEARN_LIMIT_TYPE_LIF)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("LIF Ageing configuration doesn't match the SOC property L2_LEARN_LIMIT_MODE")));
    }

    /* Convert from gport to LIF */
    rv = _bcm_dpp_gport_to_hw_resources(unit, control_info->gport, _BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK,
                                             &gport_hw_resources);
    BCMDNX_IF_ERR_EXIT(rv); 
    soc_sand_rv = soc_ppd_frwrd_mact_limit_mapped_val_to_table_index_get(soc_sand_dev_id,
                                                                         gport_hw_resources.global_in_lif, &limit_tbl_idx, &is_limit_tbl_idx_reserved);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    /*
     * Don't allow the set, if the mapping leads to the reserved entry for invalid mappings.
     * There's no way to know if it was an explicit limit for the invalid mapping or misconfiguration.
     */
    if (is_limit_tbl_idx_reserved)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("The LIF is out of range for MACT Limit setting")));
    }
    /*
     * Currently the limit table is treated as a per VSI table.
     * Therefore, the Limit table index is used as a VSI value.
     */
    vsi = limit_tbl_idx;
    soc_sand_rv = soc_ppd_vsi_info_get(soc_sand_dev_id, vsi, &vsi_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    SOC_SAND_IF_ERR_RETURN(soc_ppd_frwrd_mact_fid_aging_profile_get(
                                unit,
                                vsi_info.mac_learn_profile_id,
                                (uint32*)&control_info->entropy_id));
    /* Entropy_id must be specified in order to get a valid aging_cycles value */
    if (control_info->entropy_id > DPP_AGING_PROFILE_LAST_CONFIGURABLE ||
        control_info->entropy_id < DPP_AGING_PROFILE_FIRST_CONFIGURABLE)
    {
        control_info->aging_cycles = 0;
    }
    else
    {
        /* Get aging cycles */
        SOC_SAND_IF_ERR_RETURN(MBCM_PP_DRIVER_CALL(unit, mbcm_pp_aging_num_of_cycles_get, (unit, control_info->entropy_id, &fid_aging_cycles)));
        control_info->aging_cycles = fid_aging_cycles;
    }

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int 
bcm_petra_l2_replace_internal(
                              int unit, 
                              uint32 flags, 
                              bcm_l2_addr_t *match_addr,
                              bcm_l2_addr_t *match_mask_addr,
                              bcm_l2_addr_t *replace_addr,
                              bcm_l2_addr_t *replace_mask_addr
                              )
{
    uint32 trvrs_flag=0;
    uint32 normal_flags=0;
    int         restore;
    int rv = BCM_E_NONE;
    int restore_rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _DPP_L2_INIT_CHECK(unit);
    /* check params*/
    /* unsupported flags in dune devices */
    if (flags & (BCM_L2_REPLACE_PENDING|BCM_L2_REPLACE_LEARN_LIMIT)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("unsupported flags")));
    }

    /*
     * convert flags
     */
    if (flags & BCM_L2_REPLACE_MATCH_STATIC) {
        normal_flags |= BCM_L2_DELETE_STATIC;
    }
    if (flags & BCM_L2_REPLACE_MATCH_MAC) {
        trvrs_flag |= _BCM_PETRA_TRAVERSE_MATCH_MAC;
    }
    if (flags & BCM_L2_REPLACE_MATCH_VLAN) {
        trvrs_flag |= _BCM_PETRA_TRAVERSE_MATCH_VLAN;
    }
    if (flags & BCM_L2_REPLACE_MATCH_DEST) {
        trvrs_flag |= (_BCM_PETRA_TRAVERSE_MATCH_PORT|_BCM_PETRA_TRAVERSE_MATCH_TRUNK);
    }
    if (flags & BCM_L2_REPLACE_PROTECTION_RING) {
        trvrs_flag |= _BCM_PETRA_TRAVERSE_MATCH_PROTECTION_RING;
    }
    if (flags & BCM_L2_REPLACE_DELETE) {
        trvrs_flag |= _BCM_PETRA_TRAVERSE_ACTION_DEL;
    }
    else
    {
        trvrs_flag |= _BCM_PETRA_TRAVERSE_ACTION_REPLACE;
    }
    if (flags & BCM_L2_REPLACE_NEW_TRUNK) {
        trvrs_flag |= _BCM_PETRA_TRAVERSE_ACTION_NEW_TRUNK;
    }
    if (flags & BCM_L2_REPLACE_NO_CALLBACKS) {
        normal_flags |= BCM_L2_DELETE_NO_CALLBACKS;
    }

    if (flags & BCM_L2_REPLACE_IGNORE_DISCARD_SRC) {
        trvrs_flag |= _BCM_PETRA_TRAVERSE_MATCH_IGNORE_DISCARD_SRC;
    }

    if (flags & BCM_L2_REPLACE_DISCARD_SRC_SET) {
        trvrs_flag |= _BCM_PETRA_TRAVERSE_ACTION_DISCARD_SRC_SET;
    }

    if (flags & BCM_L2_REPLACE_DISCARD_SRC_CLEAR) {
        trvrs_flag |= _BCM_PETRA_TRAVERSE_ACTION_DISCARD_SRC_CLEAR;
    }

    if (flags & BCM_L2_REPLACE_IGNORE_DES_HIT) {
        trvrs_flag |= _BCM_PETRA_TRAVERSE_MATCH_IGNORE_DES_HIT;
    }

    if (flags & BCM_L2_REPLACE_DES_HIT_CLEAR) {
        trvrs_flag |= _BCM_PETRA_TRAVERSE_ACTION_DES_HIT_CLEAR;
    }
    if (flags & BCM_L2_REPLACE_MIGRATE_PORT) {
        trvrs_flag |= _BCM_PETRA_TRAVERSE_ACTION_MIGRATE_PORT;
    }

    if (flags & BCM_L2_REPLACE_DYNAMIC_CLEAR) {
        trvrs_flag |= _BCM_PETRA_TRAVERSE_ACTION_DYNAMIC_CLEAR;
    }

    if (flags & BCM_L2_REPLACE_DYNAMIC_SET) {
        trvrs_flag |= _BCM_PETRA_TRAVERSE_ACTION_DYNAMIC_SET;
    }
    
    if (flags & BCM_L2_REPLACE_MATCH_AGE) {
        trvrs_flag |= _BCM_PETRA_TRAVERSE_MATCH_AGE;
    }

    rv = bcm_petra_l2_addr_traverse_by_rule(unit,match_addr,normal_flags,trvrs_flag,match_mask_addr,replace_addr,replace_mask_addr,&restore,NULL,NULL);

    /* if need to restore aging/learning do it*/
    if(restore) {
        restore_rv = bcm_petra_l2_addr_thaw(unit);
        BCMDNX_IF_ERR_EXIT(restore_rv);
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_l2_replace(
                     int unit, 
                     uint32 flags, 
                     bcm_l2_addr_t *match_addr, 
                     bcm_module_t new_module, 
                     bcm_port_t new_port, 
                     bcm_trunk_t new_trunk)
{
    bcm_l2_addr_t replace_addr;

    sal_memset(&replace_addr,0x0,sizeof(replace_addr));
    replace_addr.modid = new_module;
    replace_addr.port = new_port;
    replace_addr.tgid = new_trunk;
    /* use common replace function */
    return bcm_petra_l2_replace_internal(unit,flags,match_addr,NULL,&replace_addr,NULL);
}

int bcm_petra_l2_replace_match(
                               int unit, 
                               uint32 flags, 
                               bcm_l2_addr_t *match_addr, 
                               bcm_l2_addr_t *mask_addr, 
                               bcm_l2_addr_t *replace_addr, 
                               bcm_l2_addr_t *replace_mask_addr){

    return bcm_petra_l2_replace_internal(unit,flags,match_addr,mask_addr,replace_addr,replace_mask_addr);
}



STATIC int
_bcm_petra_l2_rsrvd_mac_buff_update(
                                    int indx,
                                    uint32 buff[_BCM_PETRA_L2_RM_BUF_NOF_U32],
                                    SOC_PPC_ACTION_PROFILE *action_profile
                                    )
{
    uint32
        new_val;
    uint32
        tmp;
    uint32 soc_sand_rv;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;
    new_val = 0;

    /* trap-code */
    tmp = (action_profile->trap_code - SOC_PPC_TRAP_CODE_RESERVED_MC_0);
    new_val |= tmp  << _BCM_L2_MC_RESERVED_TRAP_CODE_OFFSET;
    
    /* trap_strength */
    tmp = (action_profile->frwrd_action_strength);
    new_val |= (tmp) << _BCM_L2_MC_RESERVED_TRAP_STRENGTH_OFFSET;
    
    /* snoop_strength */
    tmp = (action_profile->snoop_action_strength);
    new_val |= (tmp) << _BCM_L2_MC_RESERVED_SNOOP_CODE_OFFSET;

    soc_sand_rv = soc_sand_bitstream_set_any_field(&new_val,indx * _BCM_PETRA_L2_RM_BUF_INDEX_OFFSET, _BCM_PETRA_L2_RM_BUF_INDEX_LEN, buff);
    BCM_SAND_IF_ERR_EXIT_NO_UNIT(soc_sand_rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


int 
_bcm_petra_l2_cache_allocate_trap_fill(
                                       int unit, 
                                       bcm_l2_cache_addr_t                *addr, 
                                       SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO *trap_info,
                                       SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO *snoop_info
                                       )
{
    int rv = BCM_E_NONE;
    int dest_update=FALSE;
    uint32 soc_sand_rv;

    BCMDNX_INIT_FUNC_DEFS;

    SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO_clear(trap_info);
    SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO_clear(snoop_info);

    if(addr->flags & BCM_L2_CACHE_DISCARD) {
        dest_update = TRUE;
        SOC_PPD_FRWRD_DECISION_DROP_SET(unit, &(trap_info->dest_info.frwrd_dest), soc_sand_rv);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        trap_info->bitmap_mask |= SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DEST;
        trap_info->strength = (SOC_DPP_CONFIG(unit)->pp.default_trap_strength);
    }

   else if  ((addr->dest_port != BCM_GPORT_INVALID && BCM_GPORT_IS_SET(addr->dest_port))) {
        dest_update = TRUE;
        trap_info->strength = (SOC_DPP_CONFIG(unit)->pp.default_trap_strength);
        trap_info->bitmap_mask |= SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DEST;
        rv = _bcm_dpp_gport_to_fwd_decision(unit,addr->dest_port,&(trap_info->dest_info.frwrd_dest));
        BCMDNX_IF_ERR_EXIT(rv);
    }

    if(addr->flags & BCM_L2_CACHE_TRUNK) {
        if(dest_update == TRUE) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unsupported flags combinations"))); /* unsupported flags combinations*/
        }
        dest_update = TRUE;
        SOC_PPD_FRWRD_DECISION_LAG_SET(unit, &(trap_info->dest_info.frwrd_dest), addr->dest_trunk, soc_sand_rv);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        trap_info->bitmap_mask |= SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DEST;
        trap_info->strength = (SOC_DPP_CONFIG(unit)->pp.default_trap_strength);
    }
    if(addr->flags & BCM_L2_CACHE_MULTICAST) {
        if(dest_update == TRUE) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unsupported flags combinations"))); /* unsupported flags combinations*/
        }
        dest_update = TRUE;
        SOC_PPD_FRWRD_DECISION_MC_GROUP_SET(unit, &(trap_info->dest_info.frwrd_dest), addr->group, soc_sand_rv);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        trap_info->bitmap_mask |= SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DEST;
        trap_info->strength = (SOC_DPP_CONFIG(unit)->pp.default_trap_strength);
    }
    if(addr->flags & BCM_L2_CACHE_LOOKUP) {
        if(dest_update == TRUE) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unsupported flags combinations"))); /* unsupported flags combinations*/
        }
        if ((addr->group != 0) || (addr->dest_trunk != 0) || (addr->dest_port != BCM_GPORT_INVALID)) {
             BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("When BCM_L2_CACHE_LOOKUP is set, addr->group, addr->dest_trunk and addr->dest_port have to be invalid."))); 
        }
        dest_update = TRUE;
        /* Don't update destination so it will be updated according to L2 lookup*/
        trap_info->bitmap_mask &= ~SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DEST;
        trap_info->strength = (SOC_DPP_CONFIG(unit)->pp.default_trap_strength);
    }
    
    if(addr->flags & BCM_L2_CACHE_SETPRI) {
        trap_info->cos_info.tc = addr->prio;
        trap_info->bitmap_mask |= SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_TC;
        trap_info->strength = (SOC_DPP_CONFIG(unit)->pp.default_trap_strength);
    }

    if(addr->flags & BCM_L2_CACHE_BPDU) {
        trap_info->processing_info.is_trap = TRUE;
        trap_info->strength = (SOC_DPP_CONFIG(unit)->pp.default_trap_strength);
    }

    if(addr->flags & BCM_L2_CACHE_LEARN_DISABLE) {
        trap_info->processing_info.enable_learning = FALSE;
        trap_info->strength = (SOC_DPP_CONFIG(unit)->pp.default_trap_strength);
    }
    else
    {
        trap_info->processing_info.enable_learning = TRUE;
        
    }

    if(addr->flags & BCM_L2_CACHE_CPU) {
        snoop_info->snoop_cmnd = _BCM_DPP_SNOOP_CMD_TO_CPU;
        snoop_info->strength = (SOC_DPP_CONFIG(unit)->pp.default_snoop_strength);
    }
    /* destination was not updated set to drop, it will not affect packet */
    if(trap_info->dest_info.frwrd_dest.type == SOC_PPC_NOF_FRWRD_DECISION_TYPES) {
        SOC_PPD_FRWRD_DECISION_DROP_SET(unit, &(trap_info->dest_info.frwrd_dest), soc_sand_rv);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


int 
_bcm_petra_l2_cache_allocate_trap_get(
                                      int unit, 
                                      bcm_l2_cache_addr_t                *addr, 
                                      SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO *trap_info,
                                      SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO *snoop_info,
                                      int is_virtual_trap)
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    if(trap_info->strength != 0)
    {
        /* if trap set to update destination then resolve destination */
        if(trap_info->bitmap_mask & SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DEST &&
           is_virtual_trap == FALSE)
        { 
            switch(trap_info->dest_info.frwrd_dest.type)
            {
                case SOC_PPC_FRWRD_DECISION_TYPE_MC:
                    addr->flags |= BCM_L2_CACHE_MULTICAST;
                    addr->group = trap_info->dest_info.frwrd_dest.dest_id;
                    break;
                case SOC_PPC_FRWRD_DECISION_TYPE_DROP:  /* if this drop trap, set discard flag */
                    addr->flags |= BCM_L2_CACHE_DISCARD;
                    break;
                case SOC_PPC_FRWRD_DECISION_TYPE_UC_LAG:
                    addr->flags |= BCM_L2_CACHE_TRUNK; /* no break - still want to update the port */
                default:
                    /* prefer return in gport format instead of flags, (yet when set accept flags) --> asymmetric set/get */
                    rv = _bcm_dpp_gport_and_encap_from_fwd_decision(unit,&(addr->dest_port),NULL,&(trap_info->dest_info.frwrd_dest), -1/*encap_usage*/,0/*force_destination*/);
                    BCMDNX_IF_ERR_EXIT(rv);
                    break;
            }
        }

        /* In case of dest trunk port is gport, update to dest trunk */
        if ((addr->flags & BCM_L2_CACHE_TRUNK) && BCM_GPORT_IS_SET(addr->dest_port))
        {
            addr->dest_trunk = BCM_GPORT_TRUNK_GET(addr->dest_port);
            addr->dest_port = 0;
        }

        if(trap_info->bitmap_mask & SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_TC)
        {
            addr->flags |= BCM_L2_CACHE_SETPRI;
            addr->prio = trap_info->cos_info.tc;
        }

        if(!trap_info->processing_info.enable_learning) 
        {
            addr->flags |= BCM_L2_CACHE_LEARN_DISABLE;
        }
        
        if(trap_info->processing_info.is_trap) 
        {
            addr->flags |= BCM_L2_CACHE_BPDU;
            trap_info->strength = (SOC_DPP_CONFIG(unit)->pp.default_trap_strength);
        }
    }
    
    if(snoop_info->strength != 0 && (snoop_info->snoop_cmnd == _BCM_DPP_SNOOP_CMD_TO_CPU)) {
        addr->flags |= BCM_L2_CACHE_CPU;
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}



STATIC int
bcm_petra_l2_cache_mask_length(uint32 mask, int start_bit, int *length, int *continuos)
{
    int masklen, i;
    int broken = FALSE;

    /* Convert netmask to number of bits */
    masklen = 0;
    *continuos = TRUE;

    if(start_bit > 31 || start_bit < 0) {
        return BCM_E_PARAM;
    }


    mask = mask << (31 - start_bit);

    for (i = start_bit; i >= 0; i--) {
        if (!(mask & 0x80000000)) {
            /* if one bit is set after 1's broken then stop */
            if(broken) {
                *continuos = FALSE;
                return (masklen);
            }
            masklen++;
            mask = mask << 1;
        } else {
            broken = TRUE;
        }
    }
    *length = masklen;

    return BCM_E_NONE;
}


STATIC int
bcm_petra_l2_cache_mac_mask_check(bcm_mac_t mac_mask, int *nof_bits, int *continuos)
{
    SOC_SAND_PP_MAC_ADDRESS
        soc_sand_mac;
    uint32            
        mac_add_U32[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S];
    int len1 = 0, len2=0;
    int rv = BCM_E_NONE;
    int soc_sand_rv;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;
    soc_sand_rv = _bcm_petra_mac_to_sand_mac(mac_mask,&(soc_sand_mac));
    BCM_SAND_IF_ERR_EXIT_NO_UNIT(soc_sand_rv);

    /* The function soc_sand_pp_mac_address_struct_to_long writes to indices 0 and 1 of the second parameter only */
    /* coverity[overrun-buffer-val : FALSE] */   
    soc_sand_rv = soc_sand_pp_mac_address_struct_to_long(&soc_sand_mac,mac_add_U32);
    BCM_SAND_IF_ERR_EXIT_NO_UNIT(soc_sand_rv);

    rv = bcm_petra_l2_cache_mask_length(~mac_add_U32[1],15,&len1,continuos);
    BCMDNX_IF_ERR_EXIT(rv);
    *nof_bits = len1;

    /* not cont 1's */
    if(!*continuos) {
        BCM_EXIT;
    }

    /* low 32 bits */
    rv = bcm_petra_l2_cache_mask_length(~mac_add_U32[0],31,&len2,continuos);
    BCMDNX_IF_ERR_EXIT(rv);

    /* low is not cont or high is not fill */
    if((len1 < 16 && len2 > 0) || !*continuos) {
        *continuos = FALSE;
        BCM_EXIT;
    }

    *nof_bits += len2;
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
bcm_petra_l2_cache_mac_mask_fill(bcm_mac_t mac_mask, int nof_bits)
{
    SOC_SAND_PP_MAC_ADDRESS
        soc_sand_mac;
    uint32            
        mac_add_U32[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S],
        mac_all_ones[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S];
    int rv = BCM_E_NONE;
    int soc_sand_rv;


    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;
    sal_memset(mac_all_ones,0xff,sizeof(mac_all_ones));
    sal_memset(mac_add_U32,0x0,sizeof(mac_add_U32));
    
    soc_sand_rv = soc_sand_bitstream_set_any_field(mac_all_ones,48 - nof_bits, nof_bits, mac_add_U32);
    BCM_SAND_IF_ERR_EXIT_NO_UNIT(soc_sand_rv);

    /* The function soc_sand_pp_mac_address_long_to_struct reads from indices 0 and 1 of the first parameter only */
    /* coverity[overrun-buffer-val : FALSE] */   
    soc_sand_rv = soc_sand_pp_mac_address_long_to_struct(mac_add_U32,&soc_sand_mac);
    BCM_SAND_IF_ERR_EXIT_NO_UNIT(soc_sand_rv);

    rv = _bcm_petra_mac_from_sand_mac(mac_mask,&(soc_sand_mac));
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}


int 
bcm_petra_l2_cache_delete(
                          int unit, 
                          int index)
{
    bcm_l2_cache_addr_t
        empty_addr;
    int rv, index_lcl = index;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    /* No need verify init if only using l2_cache_init , without initializaing the entire L2 module */
    if (SOC_DPP_PP_ENABLE(unit)) {
        _DPP_L2_INIT_CHECK(unit);
    }

    bcm_l2_cache_addr_t_init(&empty_addr);



    rv = bcm_petra_l2_cache_get(unit, index, &empty_addr);
    BCMDNX_IF_ERR_EXIT(rv);

    /* add with empty rule at the given index */
    rv = bcm_petra_l2_cache_set_unset(
            unit, 
            index, 
            &empty_addr, 
            &index_lcl,
            TRUE /* is-for-delete */);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_l2_cache_delete_all(
                              int unit)
{
    int port_i;
    SOC_PPC_PORT soc_ppd_port = 0;
    int trap_code, port_profile;
    SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO
        trap_info;
    SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO 
        snoop_info;
    SOC_PPC_LLP_TRAP_PORT_INFO
        port_trap_info;
    _bcm_petra_l2_rsrvd_mc_profile_info_t init_data;
    bcm_dpp_user_defined_traps_t trap_init_data;
    SOC_PPC_LLP_TRAP_RESERVED_MC_KEY
        mc_key;
    SOC_PPC_ACTION_PROFILE
        action_profile;
    SOC_PPC_LLP_TRAP_PROG_TRAP_QUALIFIER 
        prog_init_data;
    int soc_sand_rv, soc_sand_dev_id;
    int mac_lsb;
    int rv, core;
    uint32 flags;


    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    /* No need verify init if only using l2_cache_init , without initializaing the entire L2 module */
    if (SOC_DPP_PP_ENABLE(unit)) {
        _DPP_L2_INIT_CHECK(unit);
    }

    soc_sand_dev_id = (unit);

    if (!SOC_WARM_BOOT(unit)) {
        SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO_clear(&trap_info);
        SOC_PPC_LLP_TRAP_PROG_TRAP_QUALIFIER_clear(&prog_init_data);
        
        trap_info.strength = 0;
        /* these init has no affect, as strength = 0 and not enabled in the trap */
        /* need to bypass verify */
        trap_info.dest_info.frwrd_dest.type = SOC_PPC_FRWRD_DECISION_TYPE_DROP; /* not affective as strength/mask is zero */
        trap_info.processing_info.da_type = SOC_SAND_PP_ETHERNET_DA_TYPE_TYPE_UC;
        trap_info.processing_info.frwrd_type = SOC_TMC_PKT_FRWRD_TYPE_BRIDGE;
        
        SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO_clear(&snoop_info);
        snoop_info.strength = 0;
        snoop_info.snoop_cmnd = 0;
        
        /* clear reserved MC */
        
        /* disable traps */
        for(trap_code = SOC_PPC_TRAP_CODE_RESERVED_MC_0; trap_code <= SOC_PPC_TRAP_CODE_RESERVED_MC_7;  ++trap_code) {
            
            /* disable traps */
            soc_sand_rv = soc_ppd_trap_frwrd_profile_info_set(soc_sand_dev_id,trap_code,&(trap_info),BCM_CORE_ALL);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            
            soc_sand_rv = soc_ppd_trap_snoop_profile_info_set(soc_sand_dev_id,trap_code,&snoop_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }

        /* set for all ports + MAC-lsb no action */
        SOC_PPC_ACTION_PROFILE_clear(&action_profile);
        action_profile.trap_code = SOC_PPC_TRAP_CODE_RESERVED_MC_0;

        
        for(port_profile = 0; port_profile < _BCM_PETRA_L2_CACHE_RM_PORT_PROFILES; ++port_profile) { 
            for(mac_lsb = 0; mac_lsb < _BCM_PETRA_L2_NOF_RM_MAC; ++mac_lsb) {
                
                SOC_PPC_LLP_TRAP_RESERVED_MC_KEY_clear(&mc_key);
                
                mc_key.reserved_mc_profile = port_profile;
                mc_key.da_mac_address_lsb = mac_lsb;
                
                soc_sand_rv = soc_ppd_llp_trap_reserved_mc_info_set(soc_sand_dev_id,&mc_key,&action_profile);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
        }
    }

    /* clear reserved traps  */
    rv = _bcm_dpp_am_template_l2_cache_rm_clear(unit);
    BCMDNX_IF_ERR_EXIT(rv);

    /* init over again */
    sal_memset(&init_data,0x0, sizeof(_bcm_petra_l2_rsrvd_mc_profile_info_t));
     
    rv = _bcm_dpp_am_template_l2_cache_rm_init(unit, 0, &init_data);
    BCMDNX_IF_ERR_EXIT(rv);

    /* clear reserved buffers  */
    rv = _bcm_dpp_am_template_l2_trap_rm_clear(unit);
    BCMDNX_IF_ERR_EXIT(rv);

    /* init over again */
    sal_memset(&trap_init_data,0x0, sizeof(bcm_dpp_user_defined_traps_t));

    
    trap_init_data.gport_trap_id = _BCM_DPP_TRAP_PREVENT_SWAPPING;

    rv = _bcm_dpp_am_template_l2_trap_rm_init(unit, 0, &trap_init_data);
    BCMDNX_IF_ERR_EXIT(rv);

    

    /*
      rv = _bcm_dpp_am_template_l2_prog_trap_init(unit, 0, &prog_init_data);
      BCM_IF_ERROR_RETURN(rv);*/

    if (!SOC_WARM_BOOT(unit)) {
        /* clear prog traps: */
        for(trap_code = SOC_PPC_TRAP_CODE_PROG_TRAP_0; trap_code <= SOC_PPC_TRAP_CODE_PROG_TRAP_3;  ++trap_code) {

            /* disable traps */
            soc_sand_rv = soc_ppd_trap_frwrd_profile_info_set(soc_sand_dev_id,trap_code,&(trap_info),BCM_CORE_ALL);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            
            soc_sand_rv = soc_ppd_trap_snoop_profile_info_set(soc_sand_dev_id,trap_code,&snoop_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }

    /* clear trap status */
    rv = _bcm_dpp_am_template_l2_prog_trap_clear(unit);
    BCMDNX_IF_ERR_EXIT(rv);

    if (!SOC_WARM_BOOT(unit)) {
        SOC_PPC_LLP_TRAP_PORT_INFO_clear(&port_trap_info);

        /* run over all ports and invalidate  trap, reserved mc- profile is 0*/
        BCM_PBMP_ITER(PBMP_E_ALL(unit),port_i){  
            BCMDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port_i, &flags)); 
            if (SOC_PORT_IS_STAT_INTERFACE(flags) || SOC_PORT_IS_ELK_INTERFACE(flags))
            {
                continue;
            }
            BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &soc_ppd_port, &core)));
            soc_sand_rv = soc_ppd_llp_trap_port_info_set(soc_sand_dev_id, core, soc_ppd_port, &port_trap_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


/*****************************************************************************
* Function:  _bcm_dpp_l2_cache_index_values_get
* Purpose:   Decode the index (port/vsi | mac/prog_profile | type)
* Params:
* unit           - which unit is it
* index_type_p   -(OUT) Type of index (VSI/MC/PROG..)
* mac_lsb_p      - (OUT) The last 8 bits of mac da address for which API was called
* port_p         -  (OUT) entrance port of the packet
* prog_profile_p -(OUT)  profile of programable traps
* index    - (IN) The encoded index
* Return:    (int) ERROR type in case of error otherwise BCM_E_NONE
 */
STATIC int _bcm_dpp_l2_cache_index_values_get(int unit,_bcm_l2_cache_index_type_e *index_type_p,
                                            int *mac_lsb_p,int *port_p, int *prog_profile_p,SOC_PPC_VSI_ID *vsi_p,int index)
{
   
    BCMDNX_INIT_FUNC_DEFS;

    if(index <= _BCM_L2_CACHE_INDEX_INVALID)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("index %d is not valid"), index));
    }
    
    if(index_type_p)
    {
        *index_type_p = ((index >> _BCM_L2_CACHE_INDEX_TYPE_OFFSET) & 
                                    _BCM_L2_CACHE_INDEX_TYPE_MASK);
    }
    if(mac_lsb_p)
    {
       *mac_lsb_p =  ((index >> _BCM_L2_CACHE_INDEX_MAC_OR_PROG_PROFILE_OFFSET) &
                                    _BCM_L2_CACHE_INDEX_MAC_OR_PROG_PROFILE_MASK);
    }
    if(prog_profile_p)
    {
       *prog_profile_p =  ((index >> _BCM_L2_CACHE_INDEX_MAC_OR_PROG_PROFILE_OFFSET) &
                                    _BCM_L2_CACHE_INDEX_MAC_OR_PROG_PROFILE_MASK);
    }
    if(vsi_p)
    {
       *vsi_p = ((index >> _BCM_L2_CACHE_INDEX_PORT_OR_VSI_OFFSET) & 
                _BCM_L2_CACHE_INDEX_PORT_OR_VSI_MASK); 
    }
    if(port_p)
    {
       *port_p = ((index >> _BCM_L2_CACHE_INDEX_PORT_OR_VSI_OFFSET) & 
                _BCM_L2_CACHE_INDEX_PORT_OR_VSI_MASK);
    }

    BCM_EXIT;
    
exit:
    BCMDNX_FUNC_RETURN; 
}


/*****************************************************************************
* Function:  bcm_petra_l2_cache_get
* Purpose:   Receive an index to SW DB which hold the port MAC lsb and Type, and fill the addr parametrs from registers
* Params:
* unit  - which unit is it
* index - (IN) index to SW DB which hold the port MAC lsb and Type
* addr  - (OUT) addr structure which is field from appropriate registers  
* Return:    (int) ERROR type in case of error otherwise BCM_E_NONE
 */
int bcm_petra_l2_cache_get(
                       int unit, 
                       int index, 
                       bcm_l2_cache_addr_t *addr)
{

    int port, prog_trap_profile;
    _bcm_l2_cache_index_type_e indx_type;
    int mac_lsb = 0;
    SOC_PPC_VSI_ID vsi = (uint32)-1;
    SOC_PPC_LLP_TRAP_RESERVED_MC_KEY
        mc_key;
    SOC_PPC_ACTION_PROFILE
        action_profile;
    SOC_PPC_LLP_TRAP_PORT_INFO
        port_trap_info;
    bcm_dpp_user_defined_traps_t
        _bcm_trap_info;
    int trap_code;
    SOC_PPC_LLP_TRAP_PROG_TRAP_QUALIFIER
        trap_qual;
    SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO snoop_info;
    bcm_dpp_vsi_ingress_profile_t vsi_ingress_profile_data;
    SOC_PPC_VSI_L2CP_HANDLE_TYPE handle_type;
    int rv = BCM_E_NONE;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    uint32  pp_port;
    int     core;
    int                         virtual_trap_id = 0;
    bcm_dpp_rx_virtual_traps_t  virtual_trap_str;
    int is_virtual_trap = FALSE;
    int trap_id = 0;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    /* No need verify init if only using l2_cache_init , without initializaing the entire L2 module */
    if (SOC_DPP_PP_ENABLE(unit)) {
        _DPP_L2_INIT_CHECK(unit);
    }
    BCMDNX_NULL_CHECK(addr);


    soc_sand_dev_id = (unit);

    bcm_l2_cache_addr_t_init(addr);
    
    BCMDNX_IF_ERR_EXIT(
    _bcm_dpp_l2_cache_index_values_get(unit,&indx_type,&mac_lsb,&port,&prog_trap_profile,&vsi,index));

  
    if(indx_type == _BCM_L2_CACHE_INDEX_TYPE_PROG || indx_type == _BCM_L2_CACHE_INDEX_TYPE_RESERVED_MC)
    {
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port, &pp_port, &core)));

        soc_sand_rv = soc_ppd_llp_trap_port_info_get(soc_sand_dev_id, core, pp_port, &port_trap_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
    
    if(indx_type == _BCM_L2_CACHE_INDEX_TYPE_PROG)
    {  
        if(prog_trap_profile >= _BCM_PETRA_L2_NOF_PROG_TRAPS)
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("index is not valid")));
        }
    } 
    else /* reserved MC */ 
    {
        if(mac_lsb  >= _BCM_PETRA_L2_NOF_RM_MAC)
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("index is not valid")));
        }
    }

    /* read ppd configuration */
    if(indx_type == _BCM_L2_CACHE_INDEX_TYPE_PROG) 
    {
        addr->vlan_mask = 0;

        /* this trap is enabled in the given trap, get it's content*/
        if(port_trap_info.trap_enable_mask & (SOC_PPC_LLP_TRAP_PORT_ENABLE_PROG_TRAP_0 << prog_trap_profile)) {
            rv = _bcm_dpp_am_template_l2_prog_trap_data_get(unit, port, prog_trap_profile, &trap_qual, &_bcm_trap_info);
            BCMDNX_IF_ERR_EXIT(rv);

            /* get snoop info */
            soc_sand_rv = soc_ppd_trap_snoop_profile_info_get(soc_sand_dev_id,prog_trap_profile + SOC_PPC_TRAP_CODE_PROG_TRAP_0,&snoop_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            /* in case trap is virtual, update dest port to be trap gport */
            is_virtual_trap = (index >> (_BCM_L2_CACHE_INDEX_TRAP_CREATED_OFFSET));
            if (is_virtual_trap)
            {
                trap_id = _BCM_TRAP_VIRTUAL_TO_ID(_bcm_trap_info.gport_trap_id);
                BCM_GPORT_TRAP_SET(addr->dest_port, trap_id, _bcm_trap_info.trap.strength, snoop_info.strength);
            }
            
            rv = _bcm_petra_l2_cache_allocate_trap_get(unit, addr, &_bcm_trap_info.trap, &snoop_info, is_virtual_trap);
            BCMDNX_IF_ERR_EXIT(rv);
        }
        else
        {
            BCM_EXIT;
        }

        /* fill qualifier if relevant */
        if(snoop_info.strength != 0 || _bcm_trap_info.trap.strength != 0) 
        {

            addr->src_port_mask = 0;
            BCM_GPORT_LOCAL_SET(addr->src_port,port);

            /* verify rest of parameters */
            rv = bcm_petra_l2_cache_mac_mask_fill(addr->mac_mask,trap_qual.l2_info.dest_mac_nof_bits);
            BCMDNX_IF_ERR_EXIT(rv);
            
            rv = _bcm_petra_mac_from_sand_mac(addr->mac,&(trap_qual.l2_info.dest_mac));
            BCMDNX_IF_ERR_EXIT(rv);
            addr->subtype = trap_qual.l2_info.sub_type;

            if (trap_qual.enable_bitmap & SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_ETHER_TYPE) 
            {
                addr->ethertype = trap_qual.l2_info.ether_type;
                addr->ethertype_mask = 0xffff;
            }
            if (trap_qual.enable_bitmap & SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_SUB_TYPE)
            {
                addr->flags |= BCM_L2_CACHE_SUBTYPE;
            }

        }
        else 
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Trap not exist")));
        }
    }

    else if (indx_type == _BCM_L2_CACHE_INDEX_TYPE_RESERVED_MC)
    {
        addr->vlan_mask = 0;
        /* get trapping/snooping information for the given port and MAC */
        rv = _bcm_dpp_am_template_l2_trap_rm_data_get(unit, port, mac_lsb, &_bcm_trap_info);
        BCMDNX_IF_ERR_EXIT(rv);

        /* get strength */
        SOC_PPC_LLP_TRAP_RESERVED_MC_KEY_clear(&mc_key);

        mc_key.reserved_mc_profile = port_trap_info.reserved_mc_profile;
        mc_key.da_mac_address_lsb = mac_lsb;

        soc_sand_rv = soc_ppd_llp_trap_reserved_mc_info_get(soc_sand_dev_id,&mc_key,&action_profile);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        _bcm_trap_info.trap.strength = action_profile.frwrd_action_strength;
        trap_code = action_profile.trap_code;

        /* get snoop info */
        soc_sand_rv = soc_ppd_trap_snoop_profile_info_get(soc_sand_dev_id,trap_code,&snoop_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        snoop_info.strength = action_profile.snoop_action_strength;

        /* in case trap is virtual, update dest port to be trap gport */
        is_virtual_trap = (index >> (_BCM_L2_CACHE_INDEX_TRAP_CREATED_OFFSET));
        if (is_virtual_trap)
        {
            trap_id = _BCM_TRAP_VIRTUAL_TO_ID(_bcm_trap_info.gport_trap_id);
            BCM_GPORT_TRAP_SET(addr->dest_port, trap_id, _bcm_trap_info.trap.strength, snoop_info.strength);
        }

        rv = _bcm_petra_l2_cache_allocate_trap_get(unit, addr, &_bcm_trap_info.trap, &snoop_info, is_virtual_trap);
        BCMDNX_IF_ERR_EXIT(rv);

        /* fill qualifier if relevant */
        if(snoop_info.strength != 0 || _bcm_trap_info.trap.strength != 0) 
        {
            _BCM_PETRA_L2_RM_MAC_FILL(addr->mac);
            addr->mac[5] += mac_lsb;
            sal_memset(&addr->mac_mask,0xff, sizeof(bcm_mac_t));
            addr->src_port_mask = 0;
            BCM_GPORT_LOCAL_SET(addr->src_port,port);
        }
        else
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Trap not exist")));
        }
    }
    else if (indx_type == _BCM_L2_CACHE_INDEX_TYPE_VSI_RESERVED_MC) 
    {
        addr->vlan_mask = -1;

        rv = _bcm_dpp_am_template_vsi_ingress_profile_data_get(unit, vsi, &vsi_ingress_profile_data);
        BCMDNX_IF_ERR_EXIT(rv);

        L2CP_DA_LSBS_BITMAP_INGRESS_GET(vsi_ingress_profile_data, mac_lsb, handle_type);

        switch (handle_type) 
        {
            case SOC_PPC_VSI_L2CP_HANDLE_TYPE_TUNNEL:
                addr->flags |= BCM_L2_CACHE_TUNNEL;
                trap_code = -1;
                break;
            case SOC_PPC_VSI_L2CP_HANDLE_TYPE_PEER:
                trap_code = SOC_PPC_TRAP_CODE_ETH_L2CP_PEER;
                break;
            case SOC_PPC_VSI_L2CP_HANDLE_TYPE_DROP:
                trap_code = SOC_PPC_TRAP_CODE_ETH_L2CP_DROP;
                break;
            default:
                trap_code = -1;
                break;
        }

        if (trap_code!=-1) 
        {
            BCM_GPORT_TRAP_SET(addr->dest_port, trap_code, 0, 0); /*trap_id = BCM_GPORT_TRAP_GET_ID (addr->dest_port);*/
        }

        rv = _bcm_petra_vid_from_vsi(unit, vsi, &addr->vlan);
        BCMDNX_IF_ERR_EXIT(rv);

        _BCM_PETRA_L2_RM_MAC_FILL(addr->mac);
        addr->mac[5] += mac_lsb;
        sal_memset(&addr->mac_mask,0xff, sizeof(bcm_mac_t));
        if (SOC_IS_JERICHO_PLUS(unit)) {
            addr->encap_id = port;
        }
    }
    else 
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Invalid index")));
    }

    rv = _bcm_dpp_am_template_l2_prog_trap_ppc_index_get(unit, prog_trap_profile, &virtual_trap_id);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_dpp_rx_virtual_trap_get(unit,virtual_trap_id,&virtual_trap_str);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Restore the BCM_L2_CACHE_LOOKUP. It is hard to recover it in the get function, thus we save it in the swstate.  */
    if (virtual_trap_str.flags & BCM_L2_CACHE_LOOKUP)
        addr->flags |= BCM_L2_CACHE_LOOKUP;

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_l2_cache_init(
                        int unit)
{
    int rv;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    /* No need verify init if only using l2_cache_init , without initializaing the entire L2 module */
    if (SOC_DPP_PP_ENABLE(unit)) {
        _DPP_L2_INIT_CHECK(unit);
    }


    rv = bcm_petra_l2_cache_delete_all(unit);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*****************************************************************************
* Function:  _bcm_dpp_l2_cache_index_set
* Purpose:   Encode the values into index, 
* Params:
* unit         - 
* index_type   - Type of index which is saved to SW DB (MC/PROG..)
* mac_lsb_or_prog_profile      - The last 6 bits of mac da address for which API was called or profile of programable traps
* port_or_vsi         -  entrance port of the packet or its VSI depends o the type
* is_trap  - is bcm_rx_trap_create which created the trap or not see _BCM_L2_CACHE_INDEX_TRAP_CREATED_OFFSET for more explanation
* index -  Encoded index (port/vsi | mac/prog_profile | type)
* Return:    (int) ERROR type in case of error otherwise BCM_E_NONE
 */
STATIC int _bcm_dpp_l2_cache_index_values_set(int unit,_bcm_l2_cache_index_type_e index_type,
                                            int mac_lsb_or_prog_profile,int port_or_vsi,int is_trap,int *index)
{
    BCMDNX_INIT_FUNC_DEFS;
    
    if(index)
    {
        if((index_type > _BCM_L2_CACHE_INDEX_TYPE_INVALID) && 
            (index_type < _BCM_L2_CACHE_INDEX_TYPE_MAX))
        {
            *index = 0; 
            *index |= (index_type << _BCM_L2_CACHE_INDEX_TYPE_OFFSET);
            *index |= (mac_lsb_or_prog_profile << _BCM_L2_CACHE_INDEX_MAC_OR_PROG_PROFILE_OFFSET);
            *index |= (port_or_vsi <<_BCM_L2_CACHE_INDEX_PORT_OR_VSI_OFFSET);
            *index |= (is_trap <<_BCM_L2_CACHE_INDEX_TRAP_CREATED_OFFSET);
        }
        else
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid index type")));             
        }
    }
    else
    {
       BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Index is NULL")));
    }

    BCM_EXIT;

exit:
    BCMDNX_FUNC_RETURN;
}

/*****************************************************************************
* Function:  _bcm_dpp_update_virtual_trap_if_needed
* Purpose:   Virtual trap can be created by 2 ways (unfortunately)
                    1) use of  bcm_rx_trap_type_create and set by bcm_rx_trap_set
                    2) direct use of bcm_l2_cache_set
                    In first case we must update the new trap code that was allocated by exchange template (MC or PROG)
                    In second case we must update all parameters of virtual trap
                   Since virtual trap must have SW db to hold its parameters (since we can't get trap strength for MC reserved)
                   We need to keep the SW DB updated and check by which way the trap was created to sync it in correct way
* Params:
* unit                 - Device Number
* _bcm_trap_info       - Trap info
* snoop_info           - Snoop Info
* new_trap_allocated   - In new PPC_TRAP_CODE was allocated by template manager
* virtual_trap_was_set - If the virtual_trap_p actually hold data, means the trap created by 1st way
* ppc_trap_code        - The PPC_TRAP_CODE of the trap
* is_for_delete        - Being called from delete cache
* virtual_traps_p      - vrtual_trap_p data, has valid in case of 1st way and has zero data in case of 2nd way
* Return:    (int)
 */
STATIC int _bcm_dpp_update_virtual_trap_if_needed(int unit,
                bcm_dpp_user_defined_traps_t *_bcm_trap_info,
                SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO *snoop_info,
                int new_trap_allocated,int virtual_trap_was_set,SOC_PPC_TRAP_CODE ppc_trap_code,
                int is_for_delete,bcm_dpp_rx_virtual_traps_t *virtual_traps_p)
{
    int rv = BCM_E_NONE;
    int virtual_trap_id = 0;
    BCMDNX_INIT_FUNC_DEFS;
    /*If virtual trap was not set means that trap is beeing created without calling the bcm_rx_trap_type_create 
          need to alloca resource for the trap*/
    if(virtual_trap_was_set == FALSE)
    {
        if (new_trap_allocated) /*In new trap was allocated by template exchange of MC reserved or Prog trap*/
        {
            /*is_allocated means first time in use, there for allocate only the first time
                             allocation without ID*/

            rv = dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_trap_virtual,0,1,&virtual_trap_id);
            BCMDNX_IF_ERR_EXIT(rv);

            /*New Trap - no need to get the written content from the SW DB*/

            virtual_traps_p->soc_ppc_trap_code = ppc_trap_code;

            sal_memcpy(&(virtual_traps_p->trap_info),&(_bcm_trap_info->trap), sizeof(SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO));
            sal_memcpy(&(virtual_traps_p->snoop_info),snoop_info, sizeof(SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO));

  
        }
    }
    else /*Only update new trap code*/
    {
        /*Means trap was created by bcm_rx_trap_type_create and set by bcm_rx_trap_set
                in bcm_rx_trap_set the data was written to SW DB
                Note no need to call _bcm_dpp_rx_virtual_trap_get since virtual_traps_p was filled in the calling function */
        virtual_trap_id = _bcm_trap_info->gport_trap_id;
        virtual_traps_p->soc_ppc_trap_code = ppc_trap_code;
    }

    /* write the local copy  to sw_db 
            only in case the function was called from delete cache and the trap was created in bcm_rx_trap_type_create
            Only then dont delete the content of the trap action*/
    if(((is_for_delete) == TRUE && (virtual_trap_was_set == TRUE)) == FALSE)
    {
        if (_bcm_dpp_rx_virtual_trap_set(unit,virtual_trap_id,virtual_traps_p) != BCM_E_NONE)
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("Function _bcm_dpp_rx_virtual_trap_get failed, on unit:%d \n"), unit));
        }
    }
    
    BCM_EXIT;

exit:
    BCMDNX_FUNC_RETURN; 
}

/*****************************************************************************
* Function:  _bcm_dpp_l2_cache_update_mc_reserved_table_with_new_profile
* Purpose:   Run on the data string which has encoded trap code and strength per MAC (MAC is index in the string)
                and update MC_RESERVED table according to the new profile
* Params:
* unit   - Device Number
* new_profile   - new profile of mc_reserved
* buff - "data" of the new profile, for MAC n bits [n,n+8] represent the encoded data (snoop_str | trap_str | trap_code)
* Return:    (int)
 */
STATIC int _bcm_dpp_l2_cache_update_mc_reserved_table_with_new_profile(
                                    int unit,
                                    int new_profile,
                                    uint32 buff[_BCM_PETRA_L2_RM_BUF_NOF_U32])
{
    int idx_mac = 0; /*Index in the bit stream which is actually MAC*/
    uint32 entry_val = 0;
    uint32 trap_strength = 0;
    uint32 snoop_strength = 0;
    uint32 trap_code = 0;
    uint32 soc_sand_rv = 0;
    SOC_PPC_LLP_TRAP_RESERVED_MC_KEY
        mc_key;
    SOC_PPC_ACTION_PROFILE 
        action_info;
    BCMDNX_INIT_FUNC_DEFS;
    
    /*Init values*/
    SOC_PPC_LLP_TRAP_RESERVED_MC_KEY_clear(&mc_key);
    SOC_PPC_ACTION_PROFILE_clear(&action_info);
    
    mc_key.reserved_mc_profile = new_profile; /*Profile is same for all entries*/
    
    for(idx_mac = 0; idx_mac < _BCM_L2_MC_RESERVED_NOF_MAC_COMBINATIONS ; idx_mac++)
    {
        soc_sand_rv = soc_sand_bitstream_get_any_field(buff,idx_mac*_BCM_PETRA_L2_RM_BUF_INDEX_OFFSET,
                                        _BCM_PETRA_L2_RM_BUF_INDEX_LEN,&entry_val);
        BCM_SAND_IF_ERR_EXIT_NO_UNIT(soc_sand_rv);

        trap_code = ((entry_val >> _BCM_L2_MC_RESERVED_TRAP_CODE_OFFSET) &
                                    _BCM_L2_MC_RESERVED_TRAP_CODE_MASK);
        trap_strength = ((entry_val >> _BCM_L2_MC_RESERVED_TRAP_STRENGTH_OFFSET) &
                                         _BCM_L2_MC_RESERVED_TRAP_STRENGTH_MASK);
        snoop_strength = ((entry_val >> _BCM_L2_MC_RESERVED_SNOOP_CODE_OFFSET) &
                                         _BCM_L2_MC_RESERVED_SNOOP_CODE_MASK);

        if(trap_strength != 0 || snoop_strength != 0)
        {
            action_info.trap_code = trap_code + SOC_PPC_TRAP_CODE_RESERVED_MC_0;
            mc_key.da_mac_address_lsb = idx_mac;
            action_info.frwrd_action_strength = trap_strength;
            action_info.snoop_action_strength = snoop_strength;
            soc_sand_rv = soc_ppd_llp_trap_reserved_mc_info_set(unit,&mc_key,&action_info);
            BCM_SAND_IF_ERR_EXIT_NO_UNIT(soc_sand_rv);
        }  
    }

    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;   
}

/*****************************************************************************
* Function:  _bcm_dpp_l2_cache_clear_mc_reserved_entry_clear
* Purpose:   Clear certain profile from the table
* Params:
* unit             - Device number
* profile_to_clean - profile to clean
* mac_lsb - mac to clean
* Return:    (int)
 */
STATIC int _bcm_dpp_l2_cache_clear_mc_reserved_entry_clear(int unit, 
                                                                             int profile_to_clean,
                                                                             int mac_lsb)
{
    SOC_PPC_LLP_TRAP_RESERVED_MC_KEY
        mc_key;
    SOC_PPC_ACTION_PROFILE 
        action_info;
    int32 soc_sand_rv = 0;
    BCMDNX_INIT_FUNC_DEFS;

    /*Init values*/
    SOC_PPC_LLP_TRAP_RESERVED_MC_KEY_clear(&mc_key);
    SOC_PPC_ACTION_PROFILE_clear(&action_info);
    /*Set entry to delete*/
    mc_key.reserved_mc_profile = profile_to_clean;
    mc_key.da_mac_address_lsb = mac_lsb;
    soc_sand_rv = soc_ppd_llp_trap_reserved_mc_info_set(unit,&mc_key,&action_info);
    BCM_SAND_IF_ERR_EXIT_NO_UNIT(soc_sand_rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;    
}


/*****************************************************************************
* Function:  _bcm_dpp_l2_cache_clear_mc_reserved_table
* Purpose:   Clear certain profile from the table
* Params:
* unit             - Device number
* profile_to_clean - profile to clean
* Return:    (int)
 */
STATIC int _bcm_dpp_l2_cache_clear_mc_reserved_table(int unit, int profile_to_clean)
{
    int idx_mac = 0; /*Index which is actually represent MAC*/
    BCMDNX_INIT_FUNC_DEFS;

    for(idx_mac = 0; idx_mac < _BCM_L2_MC_RESERVED_NOF_MAC_COMBINATIONS ; idx_mac++)
    {
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_l2_cache_clear_mc_reserved_entry_clear(unit,profile_to_clean,idx_mac));
    }

exit:
    BCMDNX_FUNC_RETURN;    
}
/*****************************************************************************
* Function:  bcm_petra_l2_cache_set_unset
* Purpose:   Set or unset (delete) the virtual traps paramter
* Params:
* unit          - which unit is it
* index         - only valid in case of delete otherwise its -1
* addr          - Structure which hold mac addrress and port etc. which configure the virtual traps
* index_used    - index of SW DB which type of _bcm_l2_cache_index_control_t
* is_for_delete - TRUE if the API was called for deletion (unset), otherwise its set
* Return:    (int) ERROR type in case of error otherwise BCM_E_NONE
 */
int bcm_petra_l2_cache_set_unset(
                       int unit, 
                       int index, 
                       bcm_l2_cache_addr_t *addr, 
                       int *index_used,
                       int is_for_delete)
{
    int port_i;
    _bcm_petra_l2_rsrvd_mc_profile_info_t
        rsrvd_info;
    SOC_PPC_ACTION_PROFILE 
        action_info;
    int mac_lsb;
    SOC_PPC_LLP_TRAP_RESERVED_MC_KEY
        mc_key;
    SOC_PPC_LLP_TRAP_PORT_INFO
        port_trap_info;
    bcm_dpp_user_defined_traps_t
        _bcm_trap_info;
    SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO 
        snoop_info;
    SOC_PPC_LLP_TRAP_PROG_TRAP_QUALIFIER
        trap_qual;
    int nof_bits=0, cont = 0;
    int new_trap_code = 0;
    int old_trap_code, trap_is_last, trap_is_first;
    int old_rm_port_profile=0, rm_is_last=0, new_rm_port_profile = 0, rm_is_first=0;
    int prog_is_first,prog_old_profile, prog_is_last = 0,prog_new_profile=0;
    bcm_dpp_vsi_egress_profile_t vsi_egress_profile_data, vsi_egress_profile_data_clean;
    bcm_dpp_vsi_ingress_profile_t vsi_ingress_profile_data, vsi_ingress_profile_data_clean;
    int egress_profile, vsi_egress_profile_is_last,vsi_egress_old_profile, vsi_egress_profile_is_first;
    int ingress_profile, vsi_ingress_profile_is_last,vsi_ingress_old_profile, vsi_ingress_profile_is_first;
    int soc_sand_dev_id = 0, soc_sand_rv = 0;
    int rv = BCM_E_NONE,trap_id=0, virtual_trap_id=0;
    bcm_dpp_rx_virtual_traps_t virtual_traps_p;
    uint32 alloc_trap_flag = 0; /*If virtual trap was created with id then template manager must alloc with id*/
    SOC_PPC_VSI_ID vsi;
    SOC_PPC_VSI_INFO vsi_info;
    SOC_PPC_VSI_L2CP_HANDLE_TYPE handle_type;
    int is_virtual_traps_p_set = FALSE;
    SOC_PPC_TRAP_CODE trap_code;
    SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO trap_clean;
    SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO snoop_info_clean;
    _bcm_dpp_gport_info_t gport_info;
    uint32  pp_port;
    int     core;
    uint32 new_val = 0;
    int trap_sw_id = 0;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    /* No need verify init if only using l2_cache_init , without initializaing the entire L2 module */
    if (SOC_DPP_PP_ENABLE(unit)) {
        _DPP_L2_INIT_CHECK(unit);
    }
    BCMDNX_NULL_CHECK(addr);
    BCMDNX_NULL_CHECK(index_used);

    soc_sand_dev_id = (unit);

    /*Reset all local varaibales*/
    sal_memset(&_bcm_trap_info,0x0, sizeof(bcm_dpp_user_defined_traps_t));
    sal_memset(&virtual_traps_p,0x0, sizeof(bcm_dpp_rx_virtual_traps_t));
    SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO_clear(&_bcm_trap_info.trap);
    SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO_clear(&trap_clean);
    trap_clean.dest_info.frwrd_dest.type = SOC_PPC_FRWRD_DECISION_TYPE_DROP;
    SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO_clear(&snoop_info);
    SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO_clear(&snoop_info_clean);
    SOC_PPC_ACTION_PROFILE_clear(&action_info);
    SOC_PPC_LLP_TRAP_RESERVED_MC_KEY_clear(&mc_key);



    if((is_for_delete == FALSE) && (index != -1)) {
        /* index has to be -1*/
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("index has to be -1")));
    }

    /* verify parameters */
    /* Soc_petra architecture can't support multiple destinations */
    if (BCM_PBMP_NOT_NULL(addr->dest_ports)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("dest_ports not supported")));
    }
    if ((addr->dest_port < 0) && (addr->dest_port != BCM_GPORT_INVALID)) {
        /* When deleting action set is called with BCM_GPORT_INVALID */
        BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("Illegal dest_port")));
    }

    /* both dest and discard flag is set */
    if (addr->dest_port != BCM_GPORT_TYPE_NONE && (addr->flags & BCM_L2_CACHE_DISCARD)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
            (_BSL_BCM_MSG("Illegal both are set designation and discard flag")));
    }

    if(addr->src_port_mask != 0 || addr->lookup_class != 0 ||
       ((addr->encap_id != 0) && !SOC_IS_JERICHO_PLUS(unit))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Not supported mask values")));
    }

    /* unsupported flags */
    if(addr->flags & (BCM_L2_CACHE_MIRROR |
                      BCM_L2_CACHE_L3 |
                      BCM_L2_CACHE_REMOTE_LOOKUP |
                      BCM_L2_CACHE_DESTPORTS |
                      BCM_L2_CACHE_PROTO_PKT)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not supported flags")));
    }
    /* Erroneous flags combinations are checked in _bcm_petra_l2_cache_allocate_trap_fill*/

    trap_id = BCM_GPORT_TRAP_GET_ID(addr->dest_port);
    /*In case the virtual trap was created with id trap_id will hold HW id, in this case need to change it to virtual*/
    rv = _bcm_dpp_rx_trap_sw_id_to_virtual_id(unit,trap_id,&virtual_trap_id);
    BCM_IF_ERROR_RETURN(rv);
    
    /* fill trap information */
    if (BCM_GPORT_IS_TRAP(addr->dest_port) && _BCM_TRAP_ID_IS_VIRTUAL(virtual_trap_id)) {

        /*Clears virtual bit before allocation,range 0-11*/
        virtual_trap_id = _BCM_TRAP_ID_TO_VIRTUAL(virtual_trap_id);
        is_virtual_traps_p_set = TRUE;
        if (_bcm_dpp_rx_virtual_trap_get(unit,virtual_trap_id,&virtual_traps_p) != BCM_E_NONE)
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS,
                (_BSL_BCM_MSG("Function _bcm_dpp_rx_virtual_trap_get failed, on unit:%d \n"), unit));
        }
        sal_memcpy(&_bcm_trap_info.trap,&(virtual_traps_p.trap_info), sizeof(_bcm_trap_info.trap));
        sal_memcpy(&snoop_info,&(virtual_traps_p.snoop_info), sizeof(snoop_info));

        _bcm_trap_info.gport_trap_id = virtual_trap_id;
        /*If virtual trap was created with_id flag need to sync with the template manager*/
        if(virtual_traps_p.flags & BCM_DPP_AM_FLAG_ALLOC_WITH_ID)
        {
            /*Template manager flag should be set with id since we know wich trap to allocate*/
            alloc_trap_flag = BCM_DPP_AM_FLAG_ALLOC_WITH_ID;
            new_trap_code = virtual_traps_p.soc_ppc_trap_code;
            if(new_trap_code >= SOC_PPC_TRAP_CODE_RESERVED_MC_0 && new_trap_code <= SOC_PPC_TRAP_CODE_RESERVED_MC_7)
            {
                /*In case of MC_RESERVED trap we need to take 1-7 since template manager use exchange*/
                new_trap_code = new_trap_code - SOC_PPC_TRAP_CODE_RESERVED_MC_0 + 1;
            }
            else if(new_trap_code >= SOC_PPC_TRAP_CODE_PROG_TRAP_0 && new_trap_code <= SOC_PPC_TRAP_CODE_PROG_TRAP_3)
            {
                /*In case of PROG trap template manager uses alloc, hence the first allocation is 0*/
                 new_trap_code = new_trap_code - SOC_PPC_TRAP_CODE_PROG_TRAP_0;
            }
        }
    } 
    else if (!(addr->flags & BCM_L2_CACHE_TUNNEL)) 
    {
        rv = _bcm_petra_l2_cache_allocate_trap_fill(unit,addr,&_bcm_trap_info.trap,&snoop_info);
        BCM_IF_ERROR_RETURN(rv);
        /*set -1 to avoid data overwrite by virtual trap (bcmRxTrapL2Cache)*/
        _bcm_trap_info.gport_trap_id = _BCM_DPP_TRAP_PREVENT_SWAPPING;

    }

    _bcm_trap_info.snoop_cmd = snoop_info.snoop_cmnd;

    /* Retrieve local PP ports */
    rv = _bcm_dpp_gport_to_phy_port(unit, addr->src_port, 0, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rv);


    /*is reserved MC and VSI:ARAD-only */
    /*For jericho plus and above, set l2cp egress transparency*/
    if(_BCM_PETRA_L2_IS_RM_MAC(addr->mac) && 
       _BCM_PETRA_L2_IS_MAC_ALL_ONES(addr->mac_mask) && 
       (addr->vlan_mask == (bcm_vlan_t)(-1))) {
        int nof_l2cp_egress_profiles = -1;
        int glb_outlif = -1;

        if (SOC_IS_JERICHO_PLUS(unit)) {
            nof_l2cp_egress_profiles = 
                    soc_property_get(unit, spn_NOF_L2CP_EGRESS_PROFILES_MAX, 1);

            if (addr->encap_id != BCM_FORWARD_ENCAP_ID_INVALID) {
                glb_outlif = BCM_L3_ITF_VAL_GET(addr->encap_id);
            } else if (nof_l2cp_egress_profiles != 1) {
                /*No map from outlif profile to l2cp profile is needed
                 *if nof_l2cp_egress_profiles = 1.
                 */
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("A valid encap_id is needed")));
            }
        }

        /*If SOC property of exposed HW id is on, SW id is used for following checks*/
        BCMDNX_IF_ERR_EXIT(
                _bcm_dpp_rx_trap_hw_id_to_sw_id(unit, trap_id, &trap_sw_id));
        mac_lsb = addr->mac[5];

        /* map VID to VSI */
        rv = _bcm_petra_vid_to_vsi(unit, addr->vlan, &vsi);
        BCMDNX_IF_ERR_EXIT(rv);

        if (addr->flags & BCM_L2_CACHE_TUNNEL) {
            handle_type = SOC_PPC_VSI_L2CP_HANDLE_TYPE_TUNNEL;
            if ((trap_sw_id == SOC_PPC_TRAP_CODE_ETH_L2CP_PEER) ||
                (trap_sw_id == SOC_PPC_TRAP_CODE_ETH_L2CP_DROP)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                    (_BSL_BCM_MSG("Single action per API is supported")));
            }
        } else if (BCM_GPORT_IS_TRAP(addr->dest_port) &&
                   (trap_sw_id == SOC_PPC_TRAP_CODE_ETH_L2CP_PEER)) {
            handle_type = SOC_PPC_VSI_L2CP_HANDLE_TYPE_PEER;
        } else if (BCM_GPORT_IS_TRAP(addr->dest_port) &&
                   (trap_sw_id == SOC_PPC_TRAP_CODE_ETH_L2CP_DROP)) {
            handle_type = SOC_PPC_VSI_L2CP_HANDLE_TYPE_DROP;
        } else {
            handle_type = SOC_PPC_VSI_L2CP_HANDLE_TYPE_NORMAL;
        }

        /* Both Ingress and egress Profiles are always updated together */
        sal_memset(&vsi_egress_profile_data, 0x0, sizeof(bcm_dpp_vsi_egress_profile_t));
        sal_memset(&vsi_ingress_profile_data, 0x0, sizeof(bcm_dpp_vsi_ingress_profile_t));

        if (!is_for_delete) {
            rv = _bcm_dpp_am_template_vsi_ingress_profile_data_get(unit, vsi, &vsi_ingress_profile_data);
            BCMDNX_IF_ERR_EXIT(rv);

            if (nof_l2cp_egress_profiles == 1) {
                /*One l2cp profile only, Nothing need to be done*/
                sal_memcpy(&(vsi_egress_profile_data.eve_da_lsbs_bitmap), 
                           &(vsi_ingress_profile_data.ive_da_lsbs_bitmap), 8);
            } else if (nof_l2cp_egress_profiles != -1) {
                rv = _bcm_dpp_am_template_l2cp_egress_profile_data_get(unit, glb_outlif, &vsi_egress_profile_data);
            } else {
                rv = _bcm_dpp_am_template_vsi_egress_profile_data_get(unit, vsi, &vsi_egress_profile_data);
            }
            BCMDNX_IF_ERR_EXIT(rv);
            
            L2CP_DA_LSBS_BITMAP_EGRESS_SET(vsi_egress_profile_data, mac_lsb, handle_type);
            L2CP_DA_LSBS_BITMAP_INGRESS_SET(vsi_ingress_profile_data, mac_lsb,handle_type);
        }
        if (nof_l2cp_egress_profiles == 1) {
            egress_profile = 0;
            vsi_egress_old_profile = 0;
            vsi_egress_profile_is_last = 0;
            vsi_egress_profile_is_first = 0;
        } else if (nof_l2cp_egress_profiles != -1) {
            rv = _bcm_dpp_am_template_l2cp_egress_profile_exchange(unit, 0/*flags*/, vsi,
                        &vsi_egress_profile_data, &vsi_egress_old_profile,
                        &vsi_egress_profile_is_last, &egress_profile, &vsi_egress_profile_is_first);
        } else {
            rv = _bcm_dpp_am_template_vsi_egress_profile_exchange(unit, 0/*flags*/, vsi,
                        &vsi_egress_profile_data, &vsi_egress_old_profile,
                        &vsi_egress_profile_is_last, &egress_profile, &vsi_egress_profile_is_first);
        }
        BCMDNX_IF_ERR_EXIT(rv);
        egress_profile = egress_profile & 0x3;

        rv = _bcm_dpp_am_template_vsi_ingress_profile_exchange(unit, 0/*flags*/, vsi,
                        &vsi_ingress_profile_data, &vsi_ingress_old_profile,
                        &vsi_ingress_profile_is_last, &ingress_profile, &vsi_ingress_profile_is_first);
        BCMDNX_IF_ERR_EXIT(rv);

        if (vsi_ingress_profile_is_last || vsi_egress_profile_is_last) {
            COMPILER_64_ZERO(vsi_egress_profile_data_clean.eve_da_lsbs_bitmap);
            COMPILER_64_ZERO(vsi_ingress_profile_data_clean.drop_da_lsbs_bitmap);
            COMPILER_64_ZERO(vsi_ingress_profile_data_clean.peer_da_lsbs_bitmap);
            COMPILER_64_ZERO(vsi_ingress_profile_data_clean.ive_da_lsbs_bitmap);
            vsi_egress_profile_data_clean.mtu = vsi_egress_profile_data.mtu;
            rv = _bcm_dpp_l2_vsi_profile_update(unit, vsi_egress_old_profile,
                        ((nof_l2cp_egress_profiles != -1) ?  \
                                _BCM_L2_MC_RESERVED_EGRESS_PROFILE_TYPE_L2CP :\
                                _BCM_L2_MC_RESERVED_EGRESS_PROFILE_TYPE_VSI),
                        vsi_ingress_old_profile, vsi_egress_profile_data_clean,
                        vsi_ingress_profile_data_clean);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        if (vsi_ingress_profile_is_first || vsi_egress_profile_is_first) {
            rv = _bcm_dpp_l2_vsi_profile_update(unit, egress_profile,
                        ((nof_l2cp_egress_profiles != -1) ?  \
                                _BCM_L2_MC_RESERVED_EGRESS_PROFILE_TYPE_L2CP :\
                                _BCM_L2_MC_RESERVED_EGRESS_PROFILE_TYPE_VSI),
                        ingress_profile, vsi_egress_profile_data,
                        vsi_ingress_profile_data);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        soc_sand_rv = soc_ppd_vsi_info_get(soc_sand_dev_id, vsi, &vsi_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* profile is only 2 lsbits of range 0-15 */
        vsi_info.profile_ingress = (vsi_info.profile_ingress & 0xFFFFFFFC) | (ingress_profile & 0x3);

        if (nof_l2cp_egress_profiles == 1) {
            /*L2CP profile 0 is used by default.
             *No bits in outlif_profile will be allocated.
             */
        } else if (nof_l2cp_egress_profiles != -1) {
            /* 1. update outlif-profile
             * 2. update EEDB with new outlif-profile
             */
            /*Format outlif to local_outlif.*/
            uint32 local_outlif_id;
            rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit,
                _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, glb_outlif, (int *)&local_outlif_id);
            BCMDNX_IF_ERR_EXIT(rv);
            BCMDNX_IF_ERR_EXIT(
                _bcm_petra_outlif_profile_set(unit, local_outlif_id, egress_profile,
                                SOC_OCC_MGMT_OUTLIF_APP_L2CP_EGRESS_PROFILE));
        } else {
            vsi_info.profile_egress = egress_profile;
        }

        soc_sand_rv = soc_ppd_vsi_info_set(soc_sand_dev_id, vsi, &vsi_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if(is_for_delete == FALSE) {
            int port_or_vsi;
            if (nof_l2cp_egress_profiles != -1) {
                port_or_vsi = addr->encap_id;
            } else {
                port_or_vsi = vsi;
            }
            BCM_SAND_IF_ERR_EXIT(
                _bcm_dpp_l2_cache_index_values_set(unit,
                         _BCM_L2_CACHE_INDEX_TYPE_VSI_RESERVED_MC, mac_lsb,
                         port_or_vsi, is_virtual_traps_p_set, index_used));
        }
        /*In case of deletion the index is deleted in bcm_petra_l2_cache_delete*/
    }
    /*is reserved MC and port, for reserved MC profile has to be -1 */
    else if(_BCM_PETRA_L2_IS_RM_MAC(addr->mac) &&
        _BCM_PETRA_L2_IS_MAC_ALL_ONES(addr->mac_mask) && 
        !(addr->flags & BCM_L2_CACHE_SUBTYPE) && 
        (addr->ethertype_mask == 0x0) && (addr->vlan_mask == 0)) {

        mac_lsb = addr->mac[5];

        if (is_for_delete) 
        {
            sal_memset(&_bcm_trap_info, 0x0, sizeof(bcm_dpp_user_defined_traps_t));
            _bcm_trap_info.gport_trap_id = _BCM_DPP_TRAP_PREVENT_SWAPPING;
            SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO_clear(&snoop_info);
        }

        /* check if it possible to allocate this trap */
        BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) 
        {
            rv  = _bcm_dpp_am_template_l2_trap_rm_exchange(unit, BCM_DPP_AM_TEMPLATE_FLAG_CHECK , port_i, mac_lsb, &_bcm_trap_info,
                                                           &old_trap_code, &trap_is_last, &new_trap_code, &trap_is_first,alloc_trap_flag);
            BCMDNX_IF_ERR_EXIT(rv);
            break;
        }

        mc_key.da_mac_address_lsb = mac_lsb;
        SOC_PPC_ACTION_PROFILE_clear(&action_info);
        action_info.trap_code = (new_trap_code-1) + SOC_PPC_TRAP_CODE_RESERVED_MC_0;
        action_info.frwrd_action_strength = _bcm_trap_info.trap.strength;
        action_info.snoop_action_strength = snoop_info.strength;

        /* allocate port-mac to trap code */
        BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) 
        {
            if (is_for_delete) 
            {
                new_val = 0;
                rv  = _bcm_dpp_am_template_l2_cache_rm_data_get(unit, port_i, &rsrvd_info);
                BCMDNX_IF_ERR_EXIT(rv);

                /* In order to delete, we maintain the previous lsb*action buff except the desired lsb section*/
                rv = soc_sand_bitstream_set_any_field(&new_val, mac_lsb * _BCM_PETRA_L2_RM_BUF_INDEX_OFFSET, _BCM_PETRA_L2_RM_BUF_INDEX_LEN, rsrvd_info.buff);
                BCM_SAND_IF_ERR_EXIT_NO_UNIT(rv);

                
            } 
            else
            {
                rv  = _bcm_dpp_am_template_l2_cache_rm_data_get(unit, port_i, &rsrvd_info);
                BCMDNX_IF_ERR_EXIT(rv);
                /*mac_buff is a mask of  snoop_str 2bit| trap_str 3bit| trap_code 3bit*/
                rv = _bcm_petra_l2_rsrvd_mac_buff_update(mac_lsb, rsrvd_info.buff, &action_info);
                BCMDNX_IF_ERR_EXIT(rv);
            }
            /*manage dpp_am_template_reserved_mc, has up to 4 profiles the data is te rervd.info*/
            rv  = _bcm_dpp_am_template_l2_cache_rm_exchange(unit, 0, port_i, &rsrvd_info,
                                                         &old_rm_port_profile, &rm_is_last, 
                                                         &new_rm_port_profile, &rm_is_first);
            BCMDNX_IF_ERR_EXIT(rv);


            
            /*When the profile is beeing changed all entries that already installed were updated with old profile
                        In this case all entries in MC_RESERVED table needs to be updated according to new profile
                        In case the new_profile is not first allocation then no need to do the update (its allready allocated)*/
            if((old_rm_port_profile != new_rm_port_profile) &&
                (new_rm_port_profile != 0) && rm_is_first)
            {
                rv = _bcm_dpp_l2_cache_update_mc_reserved_table_with_new_profile(unit,
                                                        new_rm_port_profile,rsrvd_info.buff);
                BCMDNX_IF_ERR_EXIT(rv);
            }
            
            break;
        }

        /* if success: alloc trap + update configuration */
        BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) 
        {
            /* now allocate trap (previous call only was for check of availability */

            rv  = _bcm_dpp_am_template_l2_trap_rm_exchange(unit, 0, port_i, mac_lsb, &_bcm_trap_info, 
                                                &old_trap_code, &trap_is_last, &new_trap_code, &trap_is_first,alloc_trap_flag);
            BCMDNX_IF_ERR_EXIT(rv);

            action_info.trap_code = SOC_PPC_TRAP_CODE_RESERVED_MC_0 + (new_trap_code-1);

            /*If trap entry cache entry beeing deleted no need to update virtual trap array*/
            if(is_for_delete == TRUE)
            {
                is_virtual_traps_p_set = (index >> (_BCM_L2_CACHE_INDEX_TRAP_CREATED_OFFSET));
            }
            rv = _bcm_dpp_update_virtual_trap_if_needed(unit,&_bcm_trap_info,&snoop_info,
                                                    trap_is_first,is_virtual_traps_p_set,action_info.trap_code,
                                                    is_for_delete,&virtual_traps_p);
            BCMDNX_IF_ERR_EXIT(rv);
            

            

            if ( (is_for_delete == FALSE) && (BCM_GPORT_IS_TRAP(addr->dest_port)) && (_BCM_TRAP_ID_IS_VIRTUAL(trap_id)) )
            {
                /*virtual_traps_p.soc_ppc_trap_code should be updated in alloc function above*/
                rv =  soc_ppd_trap_frwrd_profile_info_set(soc_sand_dev_id,virtual_traps_p.soc_ppc_trap_code,&_bcm_trap_info.trap,BCM_CORE_ALL);
                BCM_SAND_IF_ERR_EXIT(rv);
                /*Connect snoop to trap virtual_traps_p.soc_ppc_trap_code should be updated in alloc function above*/
                rv =  soc_ppd_trap_snoop_profile_info_set(soc_sand_dev_id,virtual_traps_p.soc_ppc_trap_code,&snoop_info);
                BCM_SAND_IF_ERR_EXIT(rv);
            }

            if (trap_is_last && (old_trap_code != 0)) 
            {
                trap_code = SOC_PPC_TRAP_CODE_RESERVED_MC_0 + (old_trap_code-1);
                soc_sand_rv = soc_ppd_trap_frwrd_profile_info_set(soc_sand_dev_id, trap_code, &trap_clean,BCM_CORE_ALL);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                soc_sand_rv = soc_ppd_trap_snoop_profile_info_set(soc_sand_dev_id, trap_code, &snoop_info_clean);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }

            /* update trap and snoop info */
            if(trap_is_first && (new_trap_code != 0)) 
            {
                soc_sand_rv = soc_ppd_trap_frwrd_profile_info_set(soc_sand_dev_id, action_info.trap_code, &(_bcm_trap_info.trap),BCM_CORE_ALL);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                soc_sand_rv = soc_ppd_trap_snoop_profile_info_set(soc_sand_dev_id, action_info.trap_code, &snoop_info);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }

            if(rm_is_first && (new_trap_code != 0) &&
                (is_for_delete == FALSE)) 
            {
                /* update port configuration */
                mc_key.reserved_mc_profile = new_rm_port_profile;
                soc_sand_rv = soc_ppd_llp_trap_reserved_mc_info_set(soc_sand_dev_id, &mc_key, &action_info);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }

            
            BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &pp_port, &core)));
            
            /* update port configuration */
            soc_sand_rv = soc_ppd_llp_trap_port_info_get(soc_sand_dev_id, core, pp_port, &port_trap_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            port_trap_info.reserved_mc_profile = new_rm_port_profile;

            soc_sand_rv = soc_ppd_llp_trap_port_info_set(soc_sand_dev_id, core, pp_port, &port_trap_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            /*In case none of the port hold the old profile. 
                        The MC_RESERVED table must be cleaned from leftover of the old profile
                        But only in case its not the same profile*/
            if(rm_is_last && (old_rm_port_profile != 0) &&
               (old_rm_port_profile != new_rm_port_profile))
            {
               rv = _bcm_dpp_l2_cache_clear_mc_reserved_table(unit,old_rm_port_profile);
               BCMDNX_IF_ERR_EXIT(rv);
            }

            /*In case of delete, If the old port porifle is not 0 and the new port profile is the same as the old
                        then the entry must be cleared, 
                        its already was removed from template need to clear HW*/
            if((old_rm_port_profile != 0) && (old_rm_port_profile == new_rm_port_profile) && is_for_delete)
            {
                BCMDNX_IF_ERR_EXIT(_bcm_dpp_l2_cache_clear_mc_reserved_entry_clear(unit,old_rm_port_profile,mac_lsb));
            }

            if(is_for_delete == FALSE)
            {
                BCMDNX_IF_ERR_EXIT(
                _bcm_dpp_l2_cache_index_values_set(unit,_BCM_L2_CACHE_INDEX_TYPE_RESERVED_MC,mac_lsb,port_i,is_virtual_traps_p_set,index_used));
            }/*In case of deletion the index is deleted in bcm_petra_l2_cache_delete*/
            if (addr->flags & BCM_L2_CACHE_LOOKUP)
            {
                /* Save the flags for this trap. The BCM_L2_CACHE_LOOKUP is hard to recover in the get function, thus we save it in the swstate.  */
                rv  = _bcm_dpp_rx_virtual_trap_get(unit,virtual_trap_id,&virtual_traps_p);
                BCMDNX_IF_ERR_EXIT(rv);

                virtual_traps_p.flags |= BCM_L2_CACHE_LOOKUP;

                rv = _bcm_dpp_rx_virtual_trap_set(unit,virtual_trap_id, &virtual_traps_p);
                BCMDNX_IF_ERR_EXIT(rv);
            }
            break;
        }
        BCM_EXIT;
    }

    /*is programmable trap */
    else {
        /* verify rest of parameters */
        rv = bcm_petra_l2_cache_mac_mask_check(addr->mac_mask,&nof_bits,&cont);
        BCMDNX_IF_ERR_EXIT(rv);
        if(!cont) {
            /* 1 bits in mask has to be continues from msb */
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid mac_mask, 1 bits has to be continues from msb")));
        }

        /* or fully masked, or exact ethertype*/
        if((addr->ethertype_mask != 0xffff) && (addr->ethertype_mask != 0)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("support only fully masked or exact ethertype value")));
        }

        SOC_PPC_LLP_TRAP_PROG_TRAP_QUALIFIER_clear(&trap_qual);
        
        rv = _bcm_petra_mac_to_sand_mac(addr->mac,&(trap_qual.l2_info.dest_mac));
        BCMDNX_IF_ERR_EXIT(rv);

        trap_qual.l2_info.dest_mac_nof_bits = nof_bits;
        trap_qual.l2_info.sub_type = addr->subtype;
        if(nof_bits > 0) {
            trap_qual.enable_bitmap |= SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_DA;
        }
        if(addr->flags & BCM_L2_CACHE_SUBTYPE) {
            trap_qual.l2_info.sub_type_bitmap = 0xf;
            trap_qual.enable_bitmap |= SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_SUB_TYPE;
        }

        if((addr->ethertype_mask == 0xffff)) {
            trap_qual.l2_info.ether_type = addr->ethertype;
            trap_qual.enable_bitmap |= SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_ETHER_TYPE;
        }

        BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
            BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port_i, &pp_port, &core)));

            if (is_for_delete == FALSE) {
                /* find programmable trap, that fit condition and result */
                rv  = _bcm_dpp_am_template_l2_prog_trap_alloc(unit, alloc_trap_flag, &trap_qual, &_bcm_trap_info, &prog_new_profile, &prog_is_first, &virtual_traps_p, is_virtual_traps_p_set);
                BCMDNX_IF_ERR_EXIT(rv);

                /* verify the new profile */                
                if (prog_new_profile > 1)
                {
                    uint32 enable_bitmap_test, inverse_bitmap_test;
                    /* trap profile 0,1 are valid for all tests, Other indices are only valid for DA and ETHER_TYPE */
                    enable_bitmap_test =
                        (trap_qual.enable_bitmap & (~(SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_DA | SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_ETHER_TYPE)));
                    inverse_bitmap_test =
                        (trap_qual.inverse_bitmap & (~(SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_DA | SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_ETHER_TYPE)));
              
                    if (enable_bitmap_test || inverse_bitmap_test)
                    {
                        rv  = _bcm_dpp_am_template_l2_prog_trap_free(unit, is_virtual_traps_p_set, prog_new_profile, &prog_old_profile, &prog_is_last);
                        BCM_IF_ERROR_RETURN(rv);
                        BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("trap profile 0,1 are valid for all tests, others are only valid for DA and ETHER_TYPE")));
                    }
                }

                if (is_virtual_traps_p_set == TRUE) /*(if == FALSE) saved in previously called prog_trap_alloc() */
                {
                    /* write local data back to sw_db in case template manager modified it */
                    rv =  _bcm_dpp_rx_virtual_trap_set(unit,virtual_trap_id, &virtual_traps_p);
                    BCMDNX_IF_ERR_EXIT(rv);
                }
            } else {

                /* De-alloc */
                _bcm_dpp_l2_cache_index_values_get(unit,0,0,0,&prog_new_profile,0,index);

                /*If trap was created with bcm_rx_trap_type_create and cache entry is deleted, no need to update virtual trap array*/
                if(is_for_delete == TRUE)
                {
                    is_virtual_traps_p_set = (index >> (_BCM_L2_CACHE_INDEX_TRAP_CREATED_OFFSET));
                }

                /* this trap is enabled in the given trap, get it's content*/
                soc_sand_rv = soc_ppd_llp_trap_port_info_get(soc_sand_dev_id, core, pp_port, &port_trap_info);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                if(port_trap_info.trap_enable_mask & (SOC_PPC_LLP_TRAP_PORT_ENABLE_PROG_TRAP_0 << prog_new_profile)) {
                    rv  = _bcm_dpp_am_template_l2_prog_trap_free(unit, is_virtual_traps_p_set, prog_new_profile, &prog_old_profile, &prog_is_last);
                    BCM_IF_ERROR_RETURN(rv);
                } else {
                    /* unexpected index */
                    BCM_EXIT;
                }

            }

            new_trap_code = prog_new_profile + SOC_PPC_TRAP_CODE_PROG_TRAP_0;


            if ( (BCM_GPORT_IS_TRAP(addr->dest_port)) && (_BCM_TRAP_ID_IS_VIRTUAL(trap_id)) ){
                /*virtual_traps_p.soc_ppc_trap_code should be updated in alloc function above*/
                rv =  soc_ppd_trap_frwrd_profile_info_set(soc_sand_dev_id,virtual_traps_p.soc_ppc_trap_code,&_bcm_trap_info.trap,BCM_CORE_ALL);
                BCM_SAND_IF_ERR_EXIT(rv);

                /*Connect snoop to trap virtual_traps_p.soc_ppc_trap_code should be updated in alloc function above*/
                rv = soc_ppd_trap_snoop_profile_info_set(soc_sand_dev_id,virtual_traps_p.soc_ppc_trap_code,&snoop_info);
                BCM_SAND_IF_ERR_EXIT(rv);            
            }


            if(((is_for_delete == FALSE) && prog_is_first) 
               || (is_for_delete && prog_is_last)) {
                if (is_for_delete) {
                   SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO_clear(&(_bcm_trap_info.trap));
                   _bcm_trap_info.trap.dest_info.frwrd_dest.type = SOC_PPC_FRWRD_DECISION_TYPE_DROP;
                   SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO_clear(&snoop_info);
                   SOC_PPC_LLP_TRAP_PROG_TRAP_QUALIFIER_clear(&trap_qual);
                }

                /* update trap result */
                soc_sand_rv = soc_ppd_trap_frwrd_profile_info_set(soc_sand_dev_id,new_trap_code,&(_bcm_trap_info.trap),BCM_CORE_ALL);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                soc_sand_rv = soc_ppd_trap_snoop_profile_info_set(soc_sand_dev_id,new_trap_code,&snoop_info);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                /* update trap condition */
                soc_sand_rv = soc_ppd_llp_trap_prog_trap_info_set(soc_sand_dev_id,prog_new_profile,&trap_qual);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
            /* update port config */
            soc_sand_rv = soc_ppd_llp_trap_port_info_get(soc_sand_dev_id,core,pp_port,&port_trap_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            /* if this trap already used then decrement counter */
            if ((is_for_delete == FALSE) && (port_trap_info.trap_enable_mask & (SOC_PPC_LLP_TRAP_PORT_ENABLE_PROG_TRAP_0 << prog_new_profile))) {
                rv  = _bcm_dpp_am_template_l2_prog_trap_free(unit, is_virtual_traps_p_set, prog_new_profile, &prog_old_profile, &prog_is_last);
                BCM_IF_ERROR_RETURN(rv);
            } else {
                if (is_for_delete == FALSE) {
                    port_trap_info.trap_enable_mask |= (SOC_PPC_LLP_TRAP_PORT_ENABLE_PROG_TRAP_0 << prog_new_profile);
                } else {
                    port_trap_info.trap_enable_mask &= ~(SOC_PPC_LLP_TRAP_PORT_ENABLE_PROG_TRAP_0 << prog_new_profile);
                }
                soc_sand_rv = soc_ppd_llp_trap_port_info_set(soc_sand_dev_id,core,pp_port,&port_trap_info);
                SOC_SAND_IF_ERR_RETURN(soc_sand_rv);
            }
            if(is_for_delete == FALSE)
            {
                BCM_SAND_IF_ERR_EXIT(
                _bcm_dpp_l2_cache_index_values_set(unit,_BCM_L2_CACHE_INDEX_TYPE_PROG,prog_new_profile,port_i,is_virtual_traps_p_set,index_used));
            }/*In case of is_for_delete the index is deleted in bcm_petra_l2_cache_delete*/

            if (addr->flags & BCM_L2_CACHE_LOOKUP)
            {
                /* Save the flags for this trap. The BCM_L2_CACHE_LOOKUP is hard to recover in the get function, thus we save it in the swstate.  */
                rv  = _bcm_dpp_rx_virtual_trap_get(unit,virtual_trap_id,&virtual_traps_p);
                BCM_SAND_IF_ERR_EXIT(rv);

                virtual_traps_p.flags |= BCM_L2_CACHE_LOOKUP;

                rv = _bcm_dpp_rx_virtual_trap_set(unit,virtual_trap_id, &virtual_traps_p);
                BCM_SAND_IF_ERR_EXIT(rv);
            }

            break;
        }
        return BCM_E_NONE;
    }

exit:
    BCMDNX_FUNC_RETURN;

}

int /* rv */
bcm_petra_l2_cache_profile_set (
        int unit,
        int trap_type,
        uint32   profile_idx, /*0 - 3 */
        uint32 flags,
        bcm_l2cp_profile_info_t *l2cp_profile_info)
/* Each MAC address in the last structure has to contain the standard value for L2CP traps in 42 MSBs.
 * They are specified as following: DA = 01-80-c2-00-00-XX, where XX & 0xC0 == 0x00
 */
{
    int32 rv = BCM_E_PARAM, i;
    uint64 bit_mask, new_bitmap;
    uint32 hi, low;
    uint32 idx;
    bcm_mac_t  *curr_macp;
    soc_reg_above_64_val_t  reg_above_64;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    /* No need verify init if only using l2_cache_init , without initializaing the entire L2 module */
    if (SOC_DPP_PP_ENABLE(unit)) {
        _DPP_L2_INIT_CHECK(unit);
    }

    if ((trap_type != bcmRxTrapL2cpDrop) && (trap_type != bcmRxTrapL2cpPeer))
    {
        LOG_ERROR(BSL_LS_SOC_COMMON,
            (BSL_META("ERROR: invalid trapType %d.\n Only bcmRxTrapL2cpDrop or bcmRxTrapL2cpPeer are accepted\n"),
            (int)trap_type));
        goto exit;
    }
    if (profile_idx > BCM_L2CP_MAX_PROFILE_IDX)
    {
        LOG_ERROR(BSL_LS_SOC_COMMON,
            (BSL_META("ERROR: invalid profile idx %d.\n Only values 0 - 3 are accepted.\n"),
            profile_idx));
        goto exit;
    }
    if (l2cp_profile_info->nof_entries > BCM_RX_TRAP_L2CP_NOF_ENTRIES)
    {
        LOG_ERROR(BSL_LS_SOC_COMMON,
            (BSL_META("ERROR: invalid number of MAC entries in list %d.\n. Values up to %d are accepted.\n"),
            l2cp_profile_info->nof_entries, BCM_RX_TRAP_L2CP_NOF_ENTRIES));
        goto exit;
    }

    COMPILER_64_ZERO(bit_mask);
    if (l2cp_profile_info->nof_entries > 0)
    {
        for (i=0; i < l2cp_profile_info->nof_entries; i++)
        {
            curr_macp = &(l2cp_profile_info->mac_add[i]);
            /* the following macro checks if the first 42 bits in MAC address
             *  are equal to the fixed combination for L2CP drop and peer traps */
            if (!_BCM_PETRA_L2_IS_RM_MAC(*curr_macp))
            {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                    (BSL_META("ERROR: invalid MAC entry #%d in l2cp_profile_info input structure\n"), i));
                goto exit;
            }
            /* The following code fragment takes the 6 LSBs of the MAC address.
             * If this value is equal to N a new bitmap is build with bit #N set
             * (the rest of bits are zeroes). The bit_mask accumulates (performs OR)
             *  for all such new_bitmaps
             */
            idx = (* curr_macp)[5] & 0x3f;
            /* Instead of shift by up to 63 positions here is used shift by
             *  up to 32 positions for high or for low part of the mask */
            if (idx < 32)
            {
                hi = 0;
                low = 1 << idx;
            }
            else
            {
                hi = 1 << (idx - 32);
                low =0;
            }
            COMPILER_64_SET(new_bitmap, hi, low);
            COMPILER_64_OR(bit_mask, new_bitmap);
        }
    }
    /* here the built bit_mask is written to the relevant HW register */
    rv = BCM_E_NONE;
    SOC_REG_ABOVE_64_CLEAR(reg_above_64);
    if (trap_type == bcmRxTrapL2cpDrop)
    {
        /* read the 256-bit register from the HW, containing 4 L2CP-drop profiles */
        rv = READ_IHP_MEF_L_2_CP_DROP_BITMAPr(unit, 0, reg_above_64);
        BCM_SAND_IF_ERR_EXIT(rv);
        /* the buffer is defined as 20 units of uint32, each profile occupies 2 units of uint32, offset in buffer is used according to profile index. */
        /* the low and hi are swapped intentionally according to HW requirements */
        COMPILER_64_TO_32_LO(reg_above_64[(profile_idx<<1)], bit_mask);
        COMPILER_64_TO_32_HI(reg_above_64[(profile_idx<<1)+1], bit_mask);
        rv = WRITE_IHP_MEF_L_2_CP_DROP_BITMAPr(unit, SOC_CORE_ALL, reg_above_64);
        BCM_SAND_IF_ERR_EXIT(rv);
    }
    else {
        rv = READ_IHP_MEF_L_2_CP_PEER_BITMAPr(unit, 0, reg_above_64);
        BCM_SAND_IF_ERR_EXIT(rv);
        COMPILER_64_TO_32_LO(reg_above_64[(profile_idx<<1)], bit_mask);
        COMPILER_64_TO_32_HI(reg_above_64[(profile_idx<<1)+1], bit_mask);
        rv = WRITE_IHP_MEF_L_2_CP_PEER_BITMAPr(unit, SOC_CORE_ALL, reg_above_64);
        BCM_SAND_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int /* rv */
bcm_petra_l2_cache_profile_get (
        int unit,
        int trap_type,
        uint32   profile_idx, /*0 - 3 */
        uint32 flags,
        bcm_l2cp_profile_info_t *l2cp_profile_info)

{
    int32 rv = BCM_E_PARAM, i, nof_entries=0;
    uint64 bit_mask;
    uint32 hi, low, mask, shift;
    soc_reg_above_64_val_t  reg_above_64;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    /* No need verify init if only using l2_cache_init , without initializaing the entire L2 module */
    if (SOC_DPP_PP_ENABLE(unit)) {
        _DPP_L2_INIT_CHECK(unit);
    }

    if ((trap_type != bcmRxTrapL2cpDrop) && (trap_type != bcmRxTrapL2cpPeer))
    {
        LOG_ERROR(BSL_LS_SOC_COMMON,
            (BSL_META("ERROR: invalid trapType %d.\n Only bcmRxTrapL2cpDrop or bcmRxTrapL2cpPeer are accepted\n"),
            (int)trap_type));
        goto exit;
    }
    if (profile_idx > BCM_L2CP_MAX_PROFILE_IDX)
    {
        LOG_ERROR(BSL_LS_SOC_COMMON,
            (BSL_META("ERROR: invalid profile idx %d.\n Only values 0 - 3 are accepted.\n"),
            profile_idx));
        goto exit;
    }

    SOC_REG_ABOVE_64_CLEAR(reg_above_64);
    if (trap_type == bcmRxTrapL2cpDrop)
    {
        rv = READ_IHP_MEF_L_2_CP_DROP_BITMAPr(unit, 0, reg_above_64);
        BCM_SAND_IF_ERR_EXIT(rv);
    }
    else {
        rv = READ_IHP_MEF_L_2_CP_PEER_BITMAPr(unit, 0, reg_above_64);
        BCM_SAND_IF_ERR_EXIT(rv);
    }
    /* update in the buffer only one profile; the buffer is defined as 20 units of uint32;
     * each profile occupies 2 units of uint32;
     * offset in buffer is used according to profile index.*/
    sal_memcpy(&bit_mask, (uint32 *)(&(reg_above_64))+(profile_idx<<1), BCM_L2CP_PROFILE_LEN_IN_BYTES);
    /* Swap hi and low values during taking off uint64 because they are kept swapped in the HW register */
    low = COMPILER_64_HI(bit_mask);
    hi  = COMPILER_64_LO(bit_mask);

    for (i=0; i < BCM_RX_TRAP_L2CP_NOF_ENTRIES; i++)
    {
        if (i< 32)
        {
            mask = low;
            shift = i;
        }
        else
        {
            mask = hi;
            shift = i-32;
        }
        if ((mask>>shift) & 1)
        {
            /* the following is filling with standard destMAC for L2CP trap features. */
            _BCM_PETRA_L2_RM_MAC_FILL (l2cp_profile_info->mac_add[nof_entries]);
            l2cp_profile_info->mac_add[nof_entries][5] = i;
            nof_entries++;
        }
    }
    l2cp_profile_info->nof_entries = nof_entries;
    BCMDNX_FUNC_RETURN;

    exit:
        BCMDNX_FUNC_RETURN;
}

int /* rv */
bcm_petra_l2_cache_vpn_to_profile_map_set (
        int unit,
        SOC_PPC_VSI_ID vsi,
        uint32   profile_idx /*0 - 3 */)
/* This procedure links the specified VSI to the given profile index.
 * The same profile index is linked to VSI for bcmRxTrapL2cpDrop and
 * for bcmRxTrapL2cpPeer. This is HW limitation. Therefore trap type
 * is not considered in this procedure.
 */
{
    int soc_sand_rv;
    SOC_PPC_VSI_INFO vsi_info;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    /* No need verify init if only using l2_cache_init , without initializaing the entire L2 module */
    if (SOC_DPP_PP_ENABLE(unit)) {
        _DPP_L2_INIT_CHECK(unit);
    }

    if (profile_idx > BCM_L2CP_MAX_PROFILE_IDX)
    {
        LOG_ERROR(BSL_LS_SOC_COMMON,
            (BSL_META("ERROR: invalid profile idx %d.\n Only values 0 - 3 are accepted.\n"),
            profile_idx));
       goto exit;
    }
    soc_sand_rv = bcm_dpp_am_l2_vpn_vsi_is_alloced(unit, vsi);
    if (soc_sand_rv != BCM_E_EXISTS)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("VPN %d does not exist"), vsi));
    }
    soc_sand_rv = soc_ppd_vsi_info_get(unit, vsi, &vsi_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    vsi_info.profile_ingress = (vsi_info.profile_ingress & 0xFFFFFFFC) | (profile_idx & 0x3); /* modify only the 2 ls bits */

    soc_sand_rv = soc_ppd_vsi_info_set(unit, vsi, &vsi_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

 exit:
     BCMDNX_FUNC_RETURN;
}


int /* rv */
bcm_petra_l2_cache_vpn_to_profile_map_get (
        int unit,
        SOC_PPC_VSI_ID vsi,
        uint32   *profile_idx /*0 - 3 */)

{
    int soc_sand_rv;
    SOC_PPC_VSI_INFO vsi_info;

   BCMDNX_INIT_FUNC_DEFS;
   BCM_DPP_UNIT_CHECK(unit);
   /* No need verify init if only using l2_cache_init , without initializaing the entire L2 module */
   if (SOC_DPP_PP_ENABLE(unit)) {
       _DPP_L2_INIT_CHECK(unit);
   }

   soc_sand_rv = bcm_dpp_am_l2_vpn_vsi_is_alloced(unit, vsi);
   if (soc_sand_rv != BCM_E_EXISTS)
   {
       BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("VPN does not exist")));
   }
   soc_sand_rv = soc_ppd_vsi_info_get(unit, vsi, &vsi_info);
   BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

   *profile_idx =vsi_info.profile_ingress & 0x3; /* take only the 2 ls bits */

exit:
   BCMDNX_FUNC_RETURN;
}

int
bcm_petra_l2_cache_set(
                       int unit, 
                       int index, 
                       bcm_l2_cache_addr_t *addr, 
                       int *index_used)
{
    int rv;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    /* No need verify init if only using l2_cache_init , without initializaing the entire L2 module */
    if (SOC_DPP_PP_ENABLE(unit)) {
        _DPP_L2_INIT_CHECK(unit);
    }

    rv = bcm_petra_l2_cache_set_unset(
            unit, 
            index, 
            addr, 
            index_used,
            FALSE);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*egress_profile_type: 0 --Indicates egress_profile is a VSI profile.
 *                     1 --Indicates egress_profile is a L2CP profile.
 */
int
_bcm_dpp_l2_vsi_profile_update(
                               int unit,
                               uint32 egress_profile,
                               uint32 egress_profile_type,
                               uint32 ingress_profile,
                               bcm_dpp_vsi_egress_profile_t egress_profile_data, 
                               bcm_dpp_vsi_ingress_profile_t ingress_profile_data)
{
    SOC_PPC_VSI_L2CP_KEY  l2cp_key;
    uint32 soc_sand_rv;
    unsigned int soc_sand_dev_id;
    uint32 mac_lsb;
    SOC_PPC_VSI_L2CP_HANDLE_TYPE handle_type;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    soc_sand_dev_id = (unit);

    /* Only configure table if custom_feature_mtu_advanced_mode is disabled */
    if (!soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mtu_advanced_mode", 0) &&
        (egress_profile_type == _BCM_L2_MC_RESERVED_EGRESS_PROFILE_TYPE_VSI)) {
        if (egress_profile > 0) {
            soc_sand_rv = soc_ppd_vsi_egress_mtu_set(soc_sand_dev_id, FALSE, egress_profile, egress_profile_data.mtu);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }

    SOC_PPC_VSI_L2CP_KEY_clear(&l2cp_key);
    l2cp_key.l2cp_profile_egress = egress_profile;
    l2cp_key.l2cp_profile_ingress = ingress_profile;

    for (mac_lsb = 0; mac_lsb < 64; mac_lsb ++) {
        l2cp_key.da_mac_address_lsb = mac_lsb;
        L2CP_DA_LSBS_BITMAP_INGRESS_GET(ingress_profile_data, mac_lsb, handle_type);
        soc_sand_rv = soc_ppd_vsi_l2cp_trap_set(soc_sand_dev_id, &l2cp_key, handle_type);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_l2_cache_size_get(
                            int unit, 
                            int *size)
{
    BCMDNX_INIT_FUNC_DEFS;
    *size = _BCM_L2_CACHE_INDEX_MAX_VALUE;
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * internal function given vlan, return MAC address set in this VLAN if any
 */
/* LCOV_EXCL_START */
int 
_bcm_petra_l2_tunnel_get(
                         int unit, 
                         bcm_vlan_t vlan,
                         bcm_mac_t mac, 
                         int *enable_flags)
{
    SOC_PPC_VSI_ID vsi;
    SOC_PPC_RIF_ID rif_id;
    SOC_PPC_RIF_INFO
        rif_info;
    int indx;
    SOC_SAND_PP_MAC_ADDRESS
        soc_ppd_mac_msb, soc_ppd_mac_lsb,soc_ppd_mac;
    uint32 soc_sand_rv;
    int     rv = BCM_E_NONE;
    unsigned int soc_sand_dev_id;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    soc_sand_dev_id = (unit);

    soc_sand_SAND_PP_MAC_ADDRESS_clear(&soc_ppd_mac_msb);
    soc_sand_SAND_PP_MAC_ADDRESS_clear(&soc_ppd_mac_lsb);
    soc_sand_SAND_PP_MAC_ADDRESS_clear(&soc_ppd_mac);

    /* map VID to VSI and RIF */
    rv = _bcm_petra_vid_to_vsi(unit,vlan,&vsi);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_petra_vid_to_rif(unit,vlan,&rif_id);
    BCMDNX_IF_ERR_EXIT(rv);

    /* get RIF info */
    soc_sand_rv = soc_ppd_rif_vsid_map_get(soc_sand_dev_id,rif_id,&rif_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    *enable_flags = (int) rif_info.routing_enablers_bm;

    /* get mac address */
    soc_sand_rv = soc_ppd_mymac_msb_get(soc_sand_dev_id,&soc_ppd_mac_msb);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    
    soc_sand_rv = soc_ppd_mymac_vsi_lsb_get(soc_sand_dev_id,vsi,&soc_ppd_mac_lsb);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* total mac is OR of two mac */
    for(indx = 0; indx < SOC_SAND_PP_MAC_ADDRESS_NOF_U8; ++indx) {
        soc_ppd_mac.address[indx] = soc_ppd_mac_msb.address[indx] | soc_ppd_mac_lsb.address[indx];
    }
    
    rv = _bcm_petra_mac_from_sand_mac(mac, &soc_ppd_mac);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}
/* LCOV_EXCL_STOP */



int 
bcm_petra_l2_tunnel_add(
                        int unit, 
                        bcm_mac_t mac, 
                        bcm_vlan_t vlan)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("l2_tunnel APIs are not supported in ARAD"))); 
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_l2_tunnel_delete(
                           int unit, 
                           bcm_mac_t mac,
                           bcm_vlan_t vlan)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("l2_tunnel APIs are not supported in ARAD"))); 
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_l2_tunnel_delete_all(
                               int unit)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("l2_tunnel APIs are not supported in ARAD"))); 
exit:
    BCMDNX_FUNC_RETURN;
}


int 
bcm_petra_l2_clear(
                   int unit)
{   
    int rv;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _DPP_L2_INIT_CHECK(unit);

    rv = bcm_petra_l2_detach(unit);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = bcm_petra_l2_init(unit);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_l2_detach(
                    int unit)
{
    int  rv = BCM_E_NONE;
    int  i;
    _bcm_petra_l2_freeze_t freeze_info;
    
    BCMDNX_INIT_FUNC_DEFS;
    /*UNIT_INIT_CHECK(unit);*/
    BCM_DPP_UNIT_CHECK(unit);    

    /* init freeze info*/
    sal_memset(&freeze_info,0x0,sizeof(_bcm_petra_l2_freeze_t));
    if (!SOC_IS_DETACHING(unit)) {
        BCMDNX_IF_ERR_EXIT(L2_ACCESS.bcm_petra_l2_freeze_state.set(unit, &freeze_info));
    }


    /* Clear L2 callback routines */
    for (i = 0; i < _BCM_PETRA_L2_CB_MAX; i++) {
        _BCM_PETRA_L2_CB_ENTRY(unit, i).fn = NULL;
        _BCM_PETRA_L2_CB_ENTRY(unit, i).fn_data = NULL;
    }
    _BCM_PETRA_L2_CB_COUNT(unit) = 0;

    /* for traverse */
    if(_bcm_l2_traverse_mact_keys[unit] != NULL)
    {
        BCM_FREE(_bcm_l2_traverse_mact_keys[unit]);
    }

    if(_bcm_l2_traverse_mact_vals[unit] != NULL)
    {
        BCM_FREE(_bcm_l2_traverse_mact_vals[unit]);
    }

    /* clear learn-profile */
    rv = _bcm_dpp_am_template_l2_learn_profile_clear(unit);
    BCMDNX_IF_ERR_CONT(rv);

    /* clear l2cp ergress profile */
    rv = _bcm_dpp_am_template_l2cp_egress_profile_clear(unit);
    BCMDNX_IF_ERR_CONT(rv);

    /* clear event handle */
    rv = _bcm_dpp_am_template_l2_event_handle_clear(unit);
    BCMDNX_IF_ERR_CONT(rv);


    /* clear l2 mymac info */
    if (!SOC_IS_DETACHING(unit)) {
        BCMDNX_IF_ERR_EXIT(L2_ACCESS.mymac_msb_is_set.set(unit,0));
    }

    if(_bcm_l2_auth_traverse_mac_addrs[unit] != NULL)
    {
        BCM_FREE(_bcm_l2_auth_traverse_mac_addrs[unit]);
    }

    if(_bcm_l2_auth_traverse_mac_infos[unit] != NULL)
    {
        BCM_FREE(_bcm_l2_auth_traverse_mac_infos[unit]);
    }

    /* cleanup the PON MACT application */
    rv = bcm_petra_l2_learn_limit_disable(unit);
    BCMDNX_IF_ERR_CONT(rv);

    if (!SOC_IS_DETACHING(unit)) {
        _DPP_L2_INIT_SET(unit,0);
    }

    BCMDNX_IF_ERR_CONT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_l2_init(
                  int unit)
{
    
    int  i;
    bcm_error_t  rv = BCM_E_NONE;
    int buf;
    uint32 soc_sand_rv;
    unsigned int soc_sand_dev_id;
    SOC_PPC_FRWRD_MACT_OPER_MODE_INFO oper_mode_info;
    soc_info_t  *si;
    SOC_TMC_PORT_HEADER_TYPE olp_receive_header_type = SOC_TMC_PORT_NOF_HEADER_TYPES;
    SOC_TMC_PORT_HEADER_TYPE olp_transmit_header_type = SOC_TMC_PORT_NOF_HEADER_TYPES;
    bcm_dpp_l2cp_egress_profile_t l2cp_egress_profile_init_data = { 0 };

    int l2_initialized;
    _bcm_petra_l2_freeze_t freeze_info;
    uint8 is_allocated;
    uint32 disable_hard_reset;
    SOC_PPC_LLP_PARSE_TPID_VALUES tpid_vals;

    BCMDNX_INIT_FUNC_DEFS;

    SOC_PPC_LLP_PARSE_TPID_VALUES_clear(&tpid_vals);

    /* Clear L2 callback routines */
    for (i = 0; i < _BCM_PETRA_L2_CB_MAX; i++) {
        _BCM_PETRA_L2_CB_ENTRY(unit, i).fn = NULL;
        _BCM_PETRA_L2_CB_ENTRY(unit, i).fn_data = NULL;
    }
    _BCM_PETRA_L2_CB_COUNT(unit) = 0;

    BCM_DPP_UNIT_CHECK(unit);
    soc_sand_dev_id = (unit);

    /*
     * MAC-limit
     */
    /* by default:
       - FID MAC limit is disabled, --> set in first call limit any fid 
       - global MAC limit is disabled (Arad only)
    */

    /* 
     * L2 SW state init \
     */        

    /* if already initialize, first detach then init (continue)*/
    if (!SOC_WARM_BOOT(unit))
    {
        BCMDNX_IF_ERR_EXIT(L2_ACCESS.is_allocated(unit, &is_allocated));
        if(!is_allocated) {
            BCMDNX_IF_ERR_EXIT(L2_ACCESS.alloc(unit));
        }
        BCMDNX_IF_ERR_EXIT(L2_ACCESS.l2_initialized.get(unit, &l2_initialized));
        if(l2_initialized) {
            rv = bcm_petra_l2_detach(unit);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

    si  = &SOC_INFO(unit);

    /* init freeze info*/
    if (!SOC_WARM_BOOT(unit))
    {
        sal_memset(&freeze_info,0x0,sizeof(_bcm_petra_l2_freeze_t));
        BCMDNX_IF_ERR_EXIT(L2_ACCESS.bcm_petra_l2_freeze_state.set(unit, &freeze_info));
    }

    /* for traverse */
    BCMDNX_ALLOC(_bcm_l2_traverse_mact_keys[unit], sizeof(SOC_PPC_FRWRD_MACT_ENTRY_KEY) * _BCM_PETRA_L2_TRVRS_ITER_BLK_SIZE,"l2 traverse keys");
    if(_bcm_l2_traverse_mact_keys[unit] == NULL)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Malloc Fail")));
    }

    BCMDNX_ALLOC(_bcm_l2_traverse_mact_vals[unit], sizeof(SOC_PPC_FRWRD_MACT_ENTRY_VALUE) * _BCM_PETRA_L2_TRVRS_ITER_BLK_SIZE,"l2 traverse info");
    if(_bcm_l2_traverse_mact_vals[unit] == NULL)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Malloc Fail")));
    }

    BCMDNX_ALLOC(_bcm_l2_auth_traverse_mac_addrs[unit], sizeof(SOC_SAND_PP_MAC_ADDRESS) * _BCM_PETRA_L2_TRVRS_ITER_BLK_SIZE,"l2 auth traverse keys");
    if(_bcm_l2_auth_traverse_mac_addrs[unit] == NULL)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Malloc Fail")));
    }

    BCMDNX_ALLOC(_bcm_l2_auth_traverse_mac_infos[unit], sizeof(SOC_PPC_LLP_SA_AUTH_MAC_INFO) * _BCM_PETRA_L2_TRVRS_ITER_BLK_SIZE,"l2 auth traverse info");
    if(_bcm_l2_auth_traverse_mac_infos[unit] == NULL)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Malloc Fail")));
    }

    /* init learn-profile, with event handle zero no limit */
    rv = _bcm_dpp_am_template_l2_learn_profile_init(unit,0,-1,0,0);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_petra_l2_event_handle_buff_build(BCM_L2_ADDR_DIST_SET_LEARN_DISTRIBUTER,
                                               BCM_L2_ADDR_DIST_SET_LEARN_DISTRIBUTER,
                                               BCM_L2_ADDR_DIST_SET_NO_DISTRIBUTER,
                                               BCM_L2_ADDR_DIST_SET_LEARN_DISTRIBUTER,
                                               &buf);
    BCMDNX_IF_ERR_EXIT(rv);

    /* init event handle -profile, for ignore all events */
    rv = _bcm_dpp_am_template_l2_event_handle_init(unit,0,buf);
    BCMDNX_IF_ERR_EXIT(rv);

    /*init l2cp egress profile template*/
    l2cp_egress_profile_init_data.mtu = 0;
    COMPILER_64_ZERO(l2cp_egress_profile_init_data.eve_da_lsbs_bitmap);
    rv = _bcm_dpp_am_template_l2cp_egress_profile_init(unit, &l2cp_egress_profile_init_data);
    BCMDNX_IF_ERR_EXIT(rv);

    if (!SOC_WARM_BOOT(unit)) {
        /* set default learning, for egress independent */
        SOC_PPC_FRWRD_MACT_OPER_MODE_INFO_clear(&oper_mode_info);

        /*Check if OLP Ports were initialized*/
        if (NUM_OLP_PORT(unit) > 0) {
            rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.header_type_out.get(unit,si->olp_port[0],&olp_receive_header_type);
            BCMDNX_IF_ERR_EXIT(rv);
            rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.header_type_in.get(unit,si->olp_port[0],&olp_transmit_header_type);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        if (olp_receive_header_type == SOC_TMC_PORT_HEADER_TYPE_RAW)
        {
            oper_mode_info.shadow_msgs_info.info.olp_receive_header_type = SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_RAW;
            oper_mode_info.learn_msgs_info.info.olp_receive_header_type = SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_RAW;
        }
        else
        {
            oper_mode_info.shadow_msgs_info.info.olp_receive_header_type = SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_OTMH;
            oper_mode_info.learn_msgs_info.info.olp_receive_header_type = SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_OTMH;
        }

        if (olp_transmit_header_type == SOC_TMC_PORT_HEADER_TYPE_RAW)
        {
            oper_mode_info.shadow_msgs_info.info.header_type = SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_RAW;
            oper_mode_info.learn_msgs_info.info.header_type = SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_RAW;
        }
        else
        {
            oper_mode_info.shadow_msgs_info.info.header_type = SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_ITMH;
            oper_mode_info.learn_msgs_info.info.header_type = SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_ITMH;
        }

        if (soc_property_suffix_num_get(unit,-1, spn_CUSTOM_FEATURE, "egress_independent_default_mode", 0) && (!SOC_IS_QUX(unit))) {
            /* Default value for tests that do not want extra DSP messages */
            oper_mode_info.learning_mode = SOC_PPC_FRWRD_MACT_LEARNING_MODE_EGRESS_INDEPENDENT;
        }
        else
        {
            /* Learn default value */
            oper_mode_info.learning_mode = SOC_PPC_FRWRD_MACT_LEARNING_MODE_INGRESS_DISTRIBUTED;
        }

        oper_mode_info.shadow_mode = SOC_PPC_FRWRD_MACT_SHADOW_MODE_NONE;
        oper_mode_info.learn_msgs_info.type = SOC_PPC_FRWRD_MACT_EVENT_PATH_TYPE_MSG_AGGR;
        oper_mode_info.shadow_msgs_info.type = SOC_PPC_FRWRD_MACT_EVENT_PATH_TYPE_DIRECT_ACCESS;
        
        soc_sand_rv = soc_ppd_frwrd_mact_oper_mode_info_set(soc_sand_dev_id,&oper_mode_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* Set registers for trill and MiM to default values so that even if they're not initialised, they will not interfere with
        802.3 protocol */
        rv = _bcm_dpp_trill_ethertype_set(unit, _BCM_PETRA_TRILL_ETHERTYPE_DEFAULT);
        BCMDNX_IF_ERR_EXIT(rv);

        disable_hard_reset = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "init_without_device_hard_reset", 0);
        /* ECI_GLOBAL_PP_2 is the mirror of IHP_GLOBAL_PP_2, and the value will be difference when custom_feature_init_without_device_hard_reset=1
         * So do sync with ECI_GLOBAL_PP_2 and IHP_GLOBAL_PP_2
         */
        if (disable_hard_reset) {
            soc_sand_rv = soc_ppd_llp_parse_tpid_values_get(unit, &tpid_vals);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            soc_sand_rv = soc_ppd_llp_parse_tpid_values_set(unit, &tpid_vals);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }


    /* init l2 cache */
    if (!SOC_WARM_BOOT(unit)) {
        /* mark initialized*/
        _DPP_L2_INIT_SET(unit,1);
 
        rv = bcm_petra_l2_cache_init(unit);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* This code is registering callback for handling learn events without DSP messages. When the DMA is implemented remove this code!!! */
    if (SOC_IS_JERICHO(unit)) {
            rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_frwrd_mact_event_handler_callback_register, (unit, _bcm_dpp_l2_event_fifo_interrupt_handler));
            BCMDNX_IF_ERR_EXIT(rv);
    }

    BCMDNX_IF_ERR_EXIT(rv);

exit:    
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_l2_key_dump(
                      int unit, 
                      char *pfx, 
                      bcm_l2_addr_t *entry, 
                      char *sfx)
{
    BCMDNX_INIT_FUNC_DEFS;
    if ((pfx == NULL) || (entry == NULL) || (sfx == NULL)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Null key")));
    }

    LOG_CLI((BSL_META_U(unit,
                        "l2: %sVLAN=0x%03x MAC=0x%02x%02x%02x"
             "%02x%02x%02x%s"), pfx, entry->vid,
             entry->mac[0], entry->mac[1], entry->mac[2],
             entry->mac[3], entry->mac[4], entry->mac[5], sfx));

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_l2_station_add(
                         int unit, 
                         int *station_id, 
                         bcm_l2_station_t *station)
{
    int rv = BCM_E_NONE;
    uint32 soc_sand_rv;
    SOC_SAND_PP_MAC_ADDRESS my_mac, my_mac_mask;
    SOC_SAND_PP_MAC_ADDRESS mac_mask;
    uint8 is_equal;
    int is_pbp;
    SOC_SAND_PP_SYS_PORT_ID sys_port_id;
    SOC_PPC_VSI_ID vsi;
    uint8 oam_is_init;
    _bcm_dpp_gport_info_t gport_info;
    int core;
    uint32 pp_port;

#ifdef BCM_88660_A0
    uint8 is_vrrp;
    uint32 protocol_flags;
    SOC_PPC_VRRP_CAM_INFO cam_info;
#endif /* BCM_88660_A0 */

    BCMDNX_INIT_FUNC_DEFS;
    /* check parameters*/
    BCM_DPP_UNIT_CHECK(unit);
    _DPP_L2_INIT_CHECK(unit);
    BCMDNX_NULL_CHECK(station_id);
    BCMDNX_NULL_CHECK(station);


    if (station->flags & BCM_L2_STATION_WITH_ID) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_L2_STATION_WITH_ID flag is not supported")));
    }
    if (station->flags & BCM_L2_STATION_REPLACE) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_L2_STATION_REPLACE flag is not supported")));
    }

    /* MAC from BCM to PPD strucutre */
    rv = _bcm_petra_mac_to_sand_mac(station->dst_mac, &my_mac);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = _bcm_petra_mac_to_sand_mac(station->dst_mac_mask, &my_mac_mask);
    BCMDNX_IF_ERR_EXIT(rv);

    if ((station->flags & BCM_L2_STATION_OAM) && !station->vlan_mask) { /* OAM endpoint */

        /* Make sure OAM is enabled. */
        _BCM_OAM_ENABLED_GET(oam_is_init);
        if (!oam_is_init) {
            BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
        }

        /* Input validation */
        if (station->src_port_mask != -1) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("For OAM cfm mymac, src_port_mask must be all ones.")));
        }

        soc_sand_rv = soc_sand_pp_mac_address_string_parse("FFFFFFFFFFFF", &mac_mask);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        soc_sand_rv = soc_sand_pp_mac_address_are_equal(&mac_mask, &my_mac_mask, &is_equal);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        if (!is_equal) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("For OAM cfm mymac, mac mask must be all ones.")));
        }

        rv = _bcm_dpp_gport_to_phy_port(unit, station->src_port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info); 
        BCMDNX_IF_ERR_EXIT(rv);

        rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, gport_info.local_port, &pp_port, &core));
        BCMDNX_IF_ERR_EXIT(rv);

        /* Configure oam my cfm mac table.*/
        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_my_cfm_mac_set, (unit, core, &my_mac, pp_port));
        BCMDNX_IF_ERR_EXIT(rv);

        /* Set station_id */
        BCM_PETRA_L2_STATION_OAM_CFM_SET(*station_id, station->src_port, my_mac.address[0]);
        
    } else if (station->flags & BCM_L2_STATION_MIM && !station->vlan_mask) { /* my-B-Mac */

        MIM_INIT(unit);

        if (station->src_port_mask == 0) { /* port is not valid - global configuration */

            /* check mac mask - MSB/LSB */
            soc_sand_rv = soc_sand_pp_mac_address_string_parse("FFFFFFFFFF00", &mac_mask);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            soc_sand_rv = soc_sand_pp_mac_address_are_equal(&mac_mask, &my_mac_mask, &is_equal);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            if (is_equal) { /* set my-B-Mac global MSB */
                
                /* id should be MiM bit indication + global MSB flag */
                *station_id = BCM_PETRA_L2_STATION_GLOBAL_MSB | BCM_PETRA_L2_STATION_MIM_BIT;

                /* set global MyB-mac MSB */
                soc_sand_rv = soc_ppd_lif_my_bmac_msb_set(unit, &my_mac);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
            else { 
                soc_sand_rv = soc_sand_pp_mac_address_string_parse("0000000000FF", &mac_mask);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                soc_sand_rv = soc_sand_pp_mac_address_are_equal(&mac_mask, &my_mac_mask, &is_equal);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                if (is_equal) { /* set my-B-Mac global ingress LSB */
                    
                    /* set an invalid pp_port */
                    sys_port_id.sys_port_type = SOC_SAND_PP_NOF_SYS_PORT_TYPES;

                    /* id should be my-B-Mac LSB + MiM flag + global LSB flag */
                    *station_id = (my_mac.address[0] << 8) | BCM_PETRA_L2_STATION_GLOBAL_LSB | BCM_PETRA_L2_STATION_MIM_BIT;
                    
                    /* set port MyB-mac LSB */
                    soc_sand_rv = soc_ppd_lif_my_bmac_port_lsb_set(unit, &sys_port_id, &my_mac, TRUE);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                }
                else {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("mac_mask must be either FFFFFFFFFF00 or 0000000000FF")));
                }
            }
        }
        else { /* port is valid - set per-port egress my-B-Mac LSB */

            /* check port mask is correct */
            if (station->src_port_mask != -1) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("src_port_mask is invalid. can be either -1 or 0.")));
            }

            /* check mac mask is correct - LSB */
            soc_sand_rv = soc_sand_pp_mac_address_string_parse("0000000000FF", &mac_mask);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            soc_sand_rv = soc_sand_pp_mac_address_are_equal(&mac_mask, &my_mac_mask, &is_equal);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            if (!is_equal) {    
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("mac_mask must be 0000000000FF")));
            }
            
            /* port must be customer port (B-SA LSB in ecapsultaion is determined by the src port) */
            rv = bcm_petra_port_control_get(unit, station->src_port, bcmPortControlMacInMac, &is_pbp);
            BCMDNX_IF_ERR_EXIT(rv);
            if (is_pbp && !(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mim_spb_enable", 0))) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("When BCM_L2_STATION_MIM is set, port cannot be PBP")));
            }

            /* id should be port + MiM flag */
            *station_id = station->src_port | BCM_PETRA_L2_STATION_MIM_BIT;

            /* Translate BCM port to SOC_SAND pp_port */
            soc_sand_SAND_PP_SYS_PORT_ID_clear(&sys_port_id);
            rv = _bcm_dpp_gport_to_phy_port(unit, station->src_port, _BCM_DPP_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, &gport_info); 
            BCMDNX_IF_ERR_EXIT(rv);

            sys_port_id.sys_id = gport_info.sys_port;

            if (_BCM_DPP_GPORT_INFO_IS_LAG(gport_info)) {
                sys_port_id.sys_port_type = SOC_SAND_PP_SYS_PORT_TYPE_LAG;
            }
            else {
                sys_port_id.sys_port_type = SOC_SAND_PP_SYS_PORT_TYPE_SINGLE_PORT;
            }

            /* set port MyB-mac LSB */
            soc_sand_rv = soc_ppd_lif_my_bmac_port_lsb_set(unit, &sys_port_id, &my_mac, TRUE);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }
    else { /* my-Mac */

        /* verify port is not valid */
        if (station->src_port_mask != 0) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("src port is relevent only if flag BCM_L2_STATION_MIM/OAM is set")));
        }

        /* Verify flags are legal. Protocol flags are always legal on multiple mymac protocol group mode. */
        if (station->flags && !_BCM_PETRA_L3_MULTIPLE_MYMAC_MODE_PROTOCOL_GROUP_IS_ACTIVE(unit)) {

            if (station->flags == BCM_L2_STATION_OAM || station->flags == BCM_L2_STATION_MIM) {
                /* Vlan mask was set even though it wasn't supposed to */
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Vlan cannot be valid if flag BCM_L2_STATION_MIM or BCM_L2_STATION_OAM is set")));
            }

            if (station->flags == BCM_L2_STATION_IPV4 && !_BCM_PETRA_L3_MULTIPLE_MYMAC_MODE_IS_IPV4_DISTINCT(unit)) {
                /* Only in IPv4 distinct mode this flag is supported. */
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_L2_STATION_IPV4 flag can't be used if chip is not in multiple mymac IPv4 distinct mode.")));
            }
        }

        if (station->vlan_mask == 0) { /* vsi is not valid - set my-Mac global MSB */

            /* check mac mask is correct - MSB */
            soc_sand_rv = soc_sand_pp_mac_address_string_parse("FFFFFFFFFFFF", &mac_mask);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            soc_sand_rv = soc_sand_pp_mac_address_are_equal(&mac_mask, &my_mac_mask, &is_equal);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            if (!is_equal) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("mac_mask must be FFFFFFFFFFFF")));
            }

            /* id should be global MSB flag */
            *station_id = BCM_PETRA_L2_STATION_GLOBAL_MSB;

            /* set global My-mac MSB */
            soc_sand_rv = soc_ppd_mymac_msb_set(unit, &my_mac);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            /* mark My-Mac MSB as set, so that other BCM APIs won't change it
               once My-Mac MSB is set, it can only be changed using this API */
            BCMDNX_IF_ERR_EXIT(_bcm_petra_l2_mymac_msb_is_set_set(unit, 1));

            {
                /* Try to set global MyMAC 16 MSBits to reflector PRGE program var */
                uint64 arg64;

                COMPILER_64_SET(arg64, 0, (my_mac.address[5] << 8) | (my_mac.address[4]));
                soc_sand_rv = arad_pp_prge_program_var_set(unit, ARAD_EGR_PROG_EDITOR_PROG_REFLECTOR_ETH, arg64);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }
        }
        else { /* vsi is valid */

            /* check vsi mask is correct */
            if (station->vlan_mask != 0xffff) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("vlan_mask is invalid. can be either 0xffff or 0.")));
            }

#ifdef BCM_88660_A0
            /* Multiple mymac mode */
            if (SOC_IS_ARADPLUS(unit) && _BCM_L3_MULTIPLE_MYMAC_ENABLED(unit)) {

                
                if (_BCM_PETRA_L3_MULTIPLE_MYMAC_MODE_PROTOCOL_GROUP_IS_ACTIVE(unit)) {
                    /* In case multiple mymac mode supports groups, it also supports different mac masks. To
                       create a VRRP station, at least one flag must be set. */
                    is_equal = (station->flags) ? TRUE : FALSE;

                    if (is_equal && (station->flags & ~BCM_PETRA_L3_MULTIPLE_MYMAC_STATION_SUPPORTED_PROTOCOL_FLAGS)) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Given protocol flags are illegal. Flag(s) 0x%x is illegal."), 
                                                          (station->flags & ~BCM_PETRA_L3_MULTIPLE_MYMAC_STATION_SUPPORTED_PROTOCOL_FLAGS)));
                    }
                } else {
                    /* Check if the mac mask is right for the multiple mymac termination mode.
                    Should be all 1s */
                    soc_sand_rv = soc_sand_pp_mac_address_string_parse("FFFFFFFFFFFF", &mac_mask);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    soc_sand_rv = soc_sand_pp_mac_address_are_equal(&mac_mask, &my_mac_mask, &is_equal);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                }

                if (is_equal) {

                    /* Check that vsi is valid */
                    if (station->vlan >= _BCM_PETRA_L3_VRRP_MAX_VSI(unit)) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Given vsi (vlan) is too high.")));
                    }

                    /* Check that mac address is not vrrp address (should be called from vrrp APIs) */
                    if (!_BCM_L3_MULTIPLE_MYMAC_EXCLUSIVE(unit)) {
                        _bcm_l3_sand_mac_is_vrrp_mac(&my_mac, &is_vrrp);
                    
                        if (is_vrrp) {
                            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("To configure VRRP mymac addresses, please use bcm_l3_vrrp_config_add.")));
                        }
                    }

                    SOC_PPC_VRRP_CAM_INFO_clear(&cam_info);

                    rv = _bcm_petra_mac_to_sand_mac(station->dst_mac, &(cam_info.mac_addr));
                    BCMDNX_IF_ERR_EXIT(rv);

                    cam_info.is_ipv4_entry = _BCM_PETRA_L3_MULTIPLE_MYMAC_MODE_IS_IPV4_DISTINCT(unit) && (station->flags & BCM_L2_STATION_IPV4);

                    if (SOC_IS_JERICHO(unit)) {

                        /* Copy mac mask. */
                        rv = _bcm_petra_mac_to_sand_mac(station->dst_mac_mask, &(cam_info.mac_mask));
                        BCMDNX_IF_ERR_EXIT(rv);

                        /* Configure protocol group, and protocol group mask.
                         If we're in IPV4 distinct mode (first two cases), we need to config either ipv4 or all the rest. */
                        if (cam_info.is_ipv4_entry) {   /* Ipv4 distinct mode and ipv4 entry */
                            protocol_flags = SOC_PPC_L3_VRRP_PROTOCOL_GROUP_IPV4; 
                        } else if (_BCM_PETRA_L3_MULTIPLE_MYMAC_MODE_IS_IPV4_DISTINCT(unit)) { /* Ipv4 distinct and non-ipv4 entry. */
                            protocol_flags = SOC_PPC_L3_VRRP_PROTOCOL_GROUP_ALL_VALID & ~SOC_PPC_L3_VRRP_PROTOCOL_GROUP_IPV4;
                        } else if (_BCM_PETRA_L3_MULTIPLE_MYMAC_MODE_IS_NONE(unit)) {   /* Set all protocols */
                            protocol_flags = SOC_PPC_L3_VRRP_PROTOCOL_GROUP_ALL_VALID;
                        }else {    /* Not ipv4 distinct, just read the flags. */
                            BCM_PETRA_L2_STATION_TRANSLATE_API_FLAGS_TO_INTERNAL_PROTOCOL_GROUP_FLAGS(station->flags, protocol_flags);
                        }

                        rv = bcm_dpp_switch_l3_protocol_group_get_group_by_members(unit, protocol_flags, &(cam_info.protocol_group), 
                                                                                   &(cam_info.protocol_group_mask), TRUE);
                        BCMDNX_IF_ERR_EXIT(rv);
                    } 
                    
                    rv = _bcm_l3_vrrp_setting_add(unit, &cam_info, station->vlan);
                    BCMDNX_IF_ERR_EXIT(rv);

                    BCM_PETRA_L2_STATION_VRRP_SET(*station_id, station->vlan, cam_info.vrrp_cam_index);
                    BCM_EXIT;
                } /* Otherwise, continue with the normal procedure. */

            } 
#endif /* BCM_88660_A0 */

            /* id should be vsi */
            *station_id = station->vlan;
            vsi = *station_id;

            /* check mac mask is correct - LSB */
            soc_sand_rv = soc_sand_pp_mac_address_string_parse("000000000FFF", &mac_mask);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            soc_sand_rv = soc_sand_pp_mac_address_are_equal(&mac_mask, &my_mac_mask, &is_equal);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            if (!is_equal) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("mac_mask must be 000000000FFF")));
            }

            my_mac.address[1] &= _BCM_DPP_MYMAC_VSI_LSB_MAX1;

            /* set vsi My-mac LSB */
            soc_sand_rv = soc_ppd_mymac_vsi_lsb_set(unit, vsi, &my_mac);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_l2_station_get(
                         int unit, 
                         int station_id,
                         bcm_l2_station_t *station)
{
    int rv = BCM_E_NONE;
    uint32 soc_sand_rv;
    SOC_SAND_PP_MAC_ADDRESS my_mac, my_mac_mask;
    bcm_port_t port;
    SOC_PPC_VSI_ID vsi;
    SOC_SAND_PP_SYS_PORT_ID sys_port_id;
    uint8 enable, oam_is_init, is_set;
    int i;
    _bcm_dpp_gport_info_t gport_info;
    uint32 pp_port;
    int core;

#ifdef BCM_88660_A0
    uint32 internal_flags;
    SOC_PPC_VRRP_CAM_INFO cam_info;
#endif /* BCM_88660_A0 */


    BCMDNX_INIT_FUNC_DEFS;
    /* check parameters*/
    BCM_DPP_UNIT_CHECK(unit);
    _DPP_L2_INIT_CHECK(unit);
    BCMDNX_NULL_CHECK(station);

    bcm_l2_station_t_init(station);

    if (BCM_PETRA_L2_STATION_IS_OAM_CFM(station_id)) { /* OAM endpoint */

        /* Make sure OAM is enabled. */
        _BCM_OAM_ENABLED_GET(oam_is_init);
        if (!oam_is_init) {
            BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
        }

        /* Get mymac lsb and port from station id. */
        BCM_PETRA_L2_STATION_OAM_PORT_GET(port, station_id);

        soc_sand_SAND_PP_MAC_ADDRESS_clear(&my_mac);

        BCM_PETRA_L2_STATION_OAM_MYMAC_LSB_GET(my_mac.address[0], station_id);

        rv = _bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info); 
        BCMDNX_IF_ERR_EXIT(rv);

        rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, gport_info.local_port, &pp_port, &core));
        BCMDNX_IF_ERR_EXIT(rv);


        /* Get oam my cfm mac table entry.*/
        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_my_cfm_mac_get, (unit, core, &my_mac, pp_port));
        BCMDNX_IF_ERR_EXIT(rv);

        /* Set port and port mask */
        station->src_port = port;
        station->src_port_mask = -1;

        /* Set station mac and mac mask */
        rv = _bcm_petra_mac_from_sand_mac(station->dst_mac, &my_mac);
        BCMDNX_IF_ERR_EXIT(rv);

        soc_sand_rv = soc_sand_pp_mac_address_string_parse("FFFFFFFFFFFF", &my_mac_mask);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        rv = _bcm_petra_mac_from_sand_mac(station->dst_mac_mask, &my_mac_mask);
        BCMDNX_IF_ERR_EXIT(rv);

        station->flags = BCM_L2_STATION_OAM;
        
    } else if (station_id & BCM_PETRA_L2_STATION_MIM_BIT) { /* my-B-Mac */

        MIM_INIT(unit);

        /* get port from station_id */
        port = station_id & (~BCM_PETRA_L2_STATION_MIM_BIT);

        if (port == BCM_PETRA_L2_STATION_GLOBAL_MSB) { /* get my-B-Mac global MSB */

            /* get global MyB-mac MSB */
            soc_sand_rv = soc_ppd_lif_my_bmac_msb_get(unit, &my_mac);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            /* set MSB mask */
            soc_sand_rv = soc_sand_pp_mac_address_string_parse("FFFFFFFFFF00", &my_mac_mask);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            rv = _bcm_petra_mac_from_sand_mac(station->dst_mac_mask, &my_mac_mask);
            BCMDNX_IF_ERR_EXIT(rv);

            station->src_port_mask = 0;
            station->src_port = port;
        }
        else {
            if (port & BCM_PETRA_L2_STATION_GLOBAL_LSB){ /* get global ingress my-B-Mac LSB */
        
                /* set an invalid pp_port */
                sys_port_id.sys_port_type = SOC_SAND_PP_NOF_SYS_PORT_TYPES;

                /* init mac address, except for LSB */
                for (i = 1; i < SOC_SAND_PP_MAC_ADDRESS_NOF_U8; i++) {
                    my_mac.address[i] = 0;
                }

                /* get the LSB from station_id - to check if it is enabled */
                my_mac.address[0] = (port >> 8);

                station->src_port_mask = 0;
            }
            else { /* get per-port egress my-B-Mac LSB */

                /* Translate BCM port to SOC_SAND pp_port */
                soc_sand_SAND_PP_SYS_PORT_ID_clear(&sys_port_id);
                rv = _bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, &gport_info); 
                BCMDNX_IF_ERR_EXIT(rv);

                sys_port_id.sys_id = gport_info.sys_port;

                if (_BCM_DPP_GPORT_INFO_IS_LAG(gport_info)) {
                    sys_port_id.sys_port_type = SOC_SAND_PP_SYS_PORT_TYPE_LAG;
                }
                else {
                    sys_port_id.sys_port_type = SOC_SAND_PP_SYS_PORT_TYPE_SINGLE_PORT;
                }

                station->src_port_mask = -1;
                station->src_port = port;
            }
            /* get port MyB-mac LSB */
            soc_sand_rv = soc_ppd_lif_my_bmac_port_lsb_get(unit, &sys_port_id, &my_mac, &enable);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            if (!enable) {
                /* entry has been removed */
                BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Entry not exist")));
            }

            /* set LSB mask */
            soc_sand_rv = soc_sand_pp_mac_address_string_parse("0000000000FF", &my_mac_mask);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            rv = _bcm_petra_mac_from_sand_mac(station->dst_mac_mask, &my_mac_mask);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }
    else { /* my-Mac */

        /* get vsi from station_id */
        vsi = station_id;

        if (vsi == BCM_PETRA_L2_STATION_GLOBAL_MSB) { /* vsi is not valid */

            /* check if global MSB is set */
            BCMDNX_IF_ERR_EXIT(_bcm_petra_l2_mymac_msb_is_set_get(unit, &is_set));
            if (is_set) {

                /* get global my-mac MSB */
                soc_sand_rv = soc_ppd_mymac_msb_get(unit, &my_mac);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                /* set MSB mask */
                soc_sand_rv = soc_sand_pp_mac_address_string_parse("FFFFFFFFFFFF", &my_mac_mask);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                rv = _bcm_petra_mac_from_sand_mac(station->dst_mac_mask, &my_mac_mask);
                BCMDNX_IF_ERR_EXIT(rv);
            }
            else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Global MSB is not set")));
            }
        }
#ifdef BCM_88660_A0
        else if (SOC_IS_ARADPLUS(unit) && BCM_PETRA_L2_STATION_IS_VRRP(station_id)) { /* Get the mac from the vrrp cam table */

            SOC_PPC_VRRP_CAM_INFO_clear(&cam_info);
            
            BCM_PETRA_L2_STATION_VRRP_CAM_INDEX_GET(cam_info.vrrp_cam_index, station_id);
            BCM_PETRA_L2_STATION_VRRP_VSI_GET(vsi, station_id); /* vsi will be put into vlan field below */

            /* Check that this cam table entry exists for this vsi before getting it. */
            rv = _bcm_l3_vrrp_setting_exists(unit, cam_info.vrrp_cam_index, vsi);
            BCMDNX_IF_ERR_EXIT(rv);

            /* Get the mac from the cam table */
            rv = _bcm_l3_vrrp_setting_get(unit, &cam_info);
            BCMDNX_IF_ERR_EXIT(rv); 

            /* Copy the mac address to mymac because it will be copied from mymac
               to the l2_station at the end of the function. */
            sal_memcpy(&my_mac, &(cam_info.mac_addr), sizeof(SOC_SAND_PP_MAC_ADDRESS));
            
            if (SOC_IS_JERICHO(unit)) {
                /* Get mask from cam entry. */
                rv = _bcm_petra_mac_from_sand_mac(station->dst_mac_mask, &cam_info.mac_mask);
                BCMDNX_IF_ERR_EXIT(rv);

                /* Get the protocol group by the protocol flags. */
                rv = bcm_dpp_switch_l3_protocol_group_get_members_by_group(unit, cam_info.protocol_group, cam_info.protocol_group_mask, 
                                                                           &internal_flags);
                BCMDNX_IF_ERR_EXIT(rv);

                /* If in, IPV4 distinct mode, the flags are either BCM_L2_STATION_IPV4 or none. */
                if (_BCM_PETRA_L3_MULTIPLE_MYMAC_MODE_IS_IPV4_DISTINCT(unit)) {
                    station->flags = (internal_flags & SOC_PPC_L3_VRRP_PROTOCOL_GROUP_IPV4) ? BCM_L2_STATION_IPV4 : 0;
                } else if (_BCM_PETRA_L3_MULTIPLE_MYMAC_MODE_IS_NONE(unit)){ /* In mode none, flags are not used. */
                    station->flags = 0;
                } else {
                    BCM_PETRA_L2_STATION_TRANSLATE_PROTOCOL_GROUP_INTERNAL_FLAGS_TO_API(internal_flags, station->flags);
                }

            } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {    
                station->flags = (cam_info.is_ipv4_entry) ? BCM_L2_STATION_IPV4 : 0;

                /* Using all bits in multiple mymac mode */
                soc_sand_rv = soc_sand_pp_mac_address_string_parse("FFFFFFFFFFFF", &my_mac_mask);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                rv = _bcm_petra_mac_from_sand_mac(station->dst_mac_mask, &my_mac_mask);
                BCMDNX_IF_ERR_EXIT(rv);
            }

            /* Set the vlan and vlan mask */
            station->vlan_mask = -1;
        }
#endif

        else { /* vsi is valid */

            /* get vsi my-mac LSB */
            soc_sand_rv = soc_ppd_mymac_vsi_lsb_get(unit, vsi, &my_mac);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            /* set LSB mask */
            soc_sand_rv = soc_sand_pp_mac_address_string_parse("000000000FFF", &my_mac_mask);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            rv = _bcm_petra_mac_from_sand_mac(station->dst_mac_mask, &my_mac_mask);
            BCMDNX_IF_ERR_EXIT(rv);

            station->vlan_mask = -1;
        }

        station->vlan = vsi;
    }

    /* BCM MAC from PPD strucutre */
    rv = _bcm_petra_mac_from_sand_mac(station->dst_mac, &my_mac);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_l2_station_delete(
                            int unit, 
                            int station_id)
{
    int rv = BCM_E_NONE;
    uint32 soc_sand_rv;
    SOC_SAND_PP_MAC_ADDRESS my_mac;
    bcm_port_t port;
    SOC_SAND_PP_SYS_PORT_ID sys_port_id;
    SOC_PPC_VSI_ID vsi;
    int i;
    uint8 oam_is_init;
    _bcm_dpp_gport_info_t gport_info;
    uint32 pp_port;
    int core;

#ifdef BCM_88660_A0
    SOC_PPC_VRRP_CAM_INFO cam_info;
    int cam_index, vlan;
#endif /* BCM_88660_A0 */

    BCMDNX_INIT_FUNC_DEFS;
    /* check parameters*/
    BCM_DPP_UNIT_CHECK(unit);
    _DPP_L2_INIT_CHECK(unit);

    /* create an empty PPD mac address */
    soc_sand_rv = soc_sand_pp_mac_address_string_parse("000000000000", &my_mac);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if (BCM_PETRA_L2_STATION_IS_OAM_CFM(station_id)) { /* OAM endpoint */

        /* Make sure OAM is enabled. */
        _BCM_OAM_ENABLED_GET(oam_is_init);
        if (!oam_is_init) {
            BCM_ERR_EXIT_NO_MSG(BCM_E_INIT);
        }

        /* Get mymac lsb and port from station id. */
        BCM_PETRA_L2_STATION_OAM_PORT_GET(port, station_id);

        soc_sand_SAND_PP_MAC_ADDRESS_clear(&my_mac);

        BCM_PETRA_L2_STATION_OAM_MYMAC_LSB_GET(my_mac.address[0], station_id);

        rv = _bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info); 
        BCMDNX_IF_ERR_EXIT(rv);

        rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, gport_info.local_port, &pp_port, &core));
        BCMDNX_IF_ERR_EXIT(rv);

        /* Make sure entry exists.*/
        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_my_cfm_mac_get, (unit, core, &my_mac, pp_port));
        BCMDNX_IF_ERR_EXIT(rv);

        /* Delete entry.*/
        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_my_cfm_mac_delete, (unit, core, &my_mac, pp_port));
        BCMDNX_IF_ERR_EXIT(rv);
        
    } else if (station_id & BCM_PETRA_L2_STATION_MIM_BIT) { /* my-B-Mac */

        MIM_INIT(unit);

        /* get port from station_id */
        port = station_id & (~BCM_PETRA_L2_STATION_MIM_BIT);

        if (port == BCM_PETRA_L2_STATION_GLOBAL_MSB) { /* delete my-B-Mac global MSB */

            /* set global MyB-mac MSB to 0 */
            soc_sand_rv = soc_ppd_lif_my_bmac_msb_set(unit, &my_mac);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
        else {
            if (port & BCM_PETRA_L2_STATION_GLOBAL_LSB){ /* delete global ingress my-B-Mac LSB */
        
                /* set an invalid pp_port */
                sys_port_id.sys_port_type = SOC_SAND_PP_NOF_SYS_PORT_TYPES;

                /* init mac address, except for LSB */
                for (i = 1; i < SOC_SAND_PP_MAC_ADDRESS_NOF_U8; i++) {
                    my_mac.address[i] = 0;
                }

                /* get the LSB from station_id - to diseble it */
                my_mac.address[0] = (port >> 8);
            }
            else { /* delete per-port egress my-B-Mac LSB */

                /* Translate BCM port to SOC_SAND pp_port */
                soc_sand_SAND_PP_SYS_PORT_ID_clear(&sys_port_id);
                rv = _bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, &gport_info); 
                BCMDNX_IF_ERR_EXIT(rv);

                sys_port_id.sys_id = gport_info.sys_port;

                if (_BCM_DPP_GPORT_INFO_IS_LAG(gport_info)) {
                    sys_port_id.sys_port_type = SOC_SAND_PP_SYS_PORT_TYPE_LAG;
                }
                else {
                    sys_port_id.sys_port_type = SOC_SAND_PP_SYS_PORT_TYPE_SINGLE_PORT;
                }
            }

            /* set per-port MyB-mac LSB to 0 or disable global LSB */
            soc_sand_rv = soc_ppd_lif_my_bmac_port_lsb_set(unit, &sys_port_id, &my_mac, FALSE);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }
    else { /* my-Mac */

        /* get vsi from station_id */
        vsi = station_id;

        if (vsi == BCM_PETRA_L2_STATION_GLOBAL_MSB) { /* vsi is not valid */

            /* set global My-mac MSB to 0 */
            soc_sand_rv = soc_ppd_mymac_msb_set(unit, &my_mac);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            /* mark My-Mac MSB as not set */
            BCMDNX_IF_ERR_EXIT(_bcm_petra_l2_mymac_msb_is_set_set(unit, 0));
        }

#ifdef BCM_88660_A0
        else if (SOC_IS_ARADPLUS(unit) && BCM_PETRA_L2_STATION_IS_VRRP(station_id)) {

            SOC_PPC_VRRP_CAM_INFO_clear(&cam_info);

            BCM_PETRA_L2_STATION_VRRP_VSI_GET(vlan, station_id);

            BCM_PETRA_L2_STATION_VRRP_CAM_INDEX_GET(cam_index, station_id);

            rv = _bcm_l3_vrrp_setting_delete(unit, cam_index, NULL, vlan);
            BCMDNX_IF_ERR_EXIT(rv);
        }
#endif /* BCM_88660_A0 */

        else { /* vsi is valid */

            /* set port My-mac LSB to 0 and disable */
            soc_sand_rv = soc_ppd_mymac_vsi_lsb_set(unit, vsi, &my_mac);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_petra_l2_auth_from_sand_auth_info(
                                           int unit,
                                           bcm_l2_auth_addr_t *addr,
                                           SOC_PPC_LLP_SA_AUTH_MAC_INFO *mac_auth_info)
{
    int rv = BCM_E_NONE;
    bcm_module_t modid;
    bcm_port_t mode_port;

    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(addr);
    BCMDNX_NULL_CHECK(mac_auth_info);

    if (mac_auth_info->expect_system_port.sys_id != SOC_PPC_LLP_SA_AUTH_ACCEPT_ALL_PORTS) {
        addr->flags |= BCM_L2_AUTH_CHECK_IN_PORT;
        rv = _bcm_dpp_port_mod_port_from_sys_port(unit, &modid, &mode_port, mac_auth_info->expect_system_port.sys_id);
        BCMDNX_IF_ERR_EXIT(rv);
        BCM_GPORT_MODPORT_SET(addr->src_port, modid, mode_port);
    }

     

    if (mac_auth_info->expect_tag_vid != SOC_PPC_LLP_SA_AUTH_ACCEPT_ALL_VIDS) {
        addr->flags |= BCM_L2_AUTH_CHECK_VLAN;
        addr->vlan = mac_auth_info->expect_tag_vid;
    }

    if (mac_auth_info->tagged_only)
    {
        addr->flags |= BCM_L2_AUTH_PERMIT_TAGGED_ONLY;
    }
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_petra_l2_auth_to_sand_auth_info(
                                         int unit,
                                         bcm_l2_auth_addr_t *addr,
                                         SOC_PPC_LLP_SA_AUTH_MAC_INFO *mac_auth_info)
{
    int rv = BCM_E_NONE;
    _bcm_dpp_gport_info_t gport_info;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(addr);
    BCMDNX_NULL_CHECK(mac_auth_info);

    SOC_PPC_LLP_SA_AUTH_MAC_INFO_clear(mac_auth_info);

    if((addr->flags & (~(BCM_L2_AUTH_CHECK_IN_PORT|BCM_L2_AUTH_CHECK_VLAN|BCM_L2_AUTH_PERMIT_TAGGED_ONLY)))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid flags parameter")));
    }
    
    if (addr->flags & BCM_L2_AUTH_CHECK_IN_PORT) {
        rv = _bcm_dpp_gport_to_phy_port(unit, addr->src_port, _BCM_DPP_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, &gport_info); 
        BCM_IF_ERROR_RETURN(rv);

        mac_auth_info->expect_system_port.sys_id = gport_info.sys_port;

        if (_BCM_DPP_GPORT_INFO_IS_LAG(gport_info)) {
            mac_auth_info->expect_system_port.sys_port_type = SOC_SAND_PP_SYS_PORT_TYPE_LAG;
        }
        else {
            mac_auth_info->expect_system_port.sys_port_type = SOC_SAND_PP_SYS_PORT_TYPE_SINGLE_PORT;
        }
    } 
    else if (SOC_IS_ARAD_A0(unit)) { /* ARAD A0 And CHECK_IN_PORT is not set */
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("ARAD-A0 does not support Permit all ports")));
    }

    if (addr->flags & BCM_L2_AUTH_CHECK_VLAN) {
        rv = _bcm_dpp_vlan_check(unit, addr->vlan);
        BCMDNX_IF_ERR_EXIT(rv);
        mac_auth_info->expect_tag_vid = addr->vlan;
    }

    if (addr->flags & BCM_L2_AUTH_PERMIT_TAGGED_ONLY) {
        mac_auth_info->tagged_only = 1;
    }
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_l2_auth_add(
                      int unit, 
                      bcm_l2_auth_addr_t *addr)
{
    SOC_SAND_SUCCESS_FAILURE
        failure_indication;
    int rv = BCM_E_NONE;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    SOC_SAND_PP_MAC_ADDRESS  mac_address_key;
    SOC_PPC_LLP_SA_AUTH_MAC_INFO mac_auth_info;
    uint8 mac_auth_enable;

    BCMDNX_INIT_FUNC_DEFS;
    /* check parameters */
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(addr);

    soc_sand_dev_id = (unit);

/* convert BCM mac  to PPD mac type */
    rv = _bcm_petra_mac_to_sand_mac(addr->sa_mac, &mac_address_key);
    BCM_IF_ERROR_RETURN(rv);

/* convert BCM auth info to PPD type */
    rv = _bcm_petra_l2_auth_to_sand_auth_info(unit, addr, &mac_auth_info);
    BCM_IF_ERROR_RETURN(rv);

/* set mac_auth_enable to 1, when adding mac authentication address */
    mac_auth_enable = 1;

/* set authentication information for a MAC address */
    soc_sand_rv = soc_ppd_llp_sa_auth_mac_info_set(soc_sand_dev_id, &mac_address_key, 
                                                   &mac_auth_info, mac_auth_enable, &failure_indication);
    SOC_SAND_IF_ERR_RETURN(soc_sand_rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_l2_auth_delete(
                         int unit, 
                         bcm_l2_auth_addr_t *addr)
{
    SOC_SAND_SUCCESS_FAILURE
        failure_indication;
    int rv = BCM_E_NONE;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    SOC_SAND_PP_MAC_ADDRESS  mac_address_key;
    SOC_PPC_LLP_SA_AUTH_MAC_INFO mac_auth_info;
    uint8 mac_auth_enable;

    BCMDNX_INIT_FUNC_DEFS;

/* check parameters */
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(addr);

    soc_sand_dev_id = (unit);

/* convert BCM mac  to PPD mac type */
    rv = _bcm_petra_mac_to_sand_mac(addr->sa_mac, &mac_address_key);
    BCM_IF_ERROR_RETURN(rv);

/* convert BCM auth info to PPD type */
    rv = _bcm_petra_l2_auth_to_sand_auth_info(unit, addr, &mac_auth_info);
    BCM_IF_ERROR_RETURN(rv);

/* set mac_auth_enable to 0, when deleting mac authentication address */
    mac_auth_enable = 0;

/* set authentication information for a MAC address */
    soc_sand_rv = soc_ppd_llp_sa_auth_mac_info_set(soc_sand_dev_id, &mac_address_key, 
                                                   &mac_auth_info, mac_auth_enable, &failure_indication);
    SOC_SAND_IF_ERR_RETURN(soc_sand_rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_l2_auth_delete_all(
                             int unit)
{
    SOC_SAND_SUCCESS_FAILURE
        failure_indication;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    SOC_PPC_LLP_SA_AUTH_MATCH_RULE rule;
    SOC_SAND_TABLE_BLOCK_RANGE block_range;
    uint32 nof_entries=0;
    int indx;

    BCMDNX_INIT_FUNC_DEFS;
/* check parameters*/
    BCM_DPP_UNIT_CHECK(unit);

    soc_sand_dev_id = (unit);

/* traverse entry by entry */
/* for deleting all, ignore port and vid to match every entry */
    SOC_PPC_LLP_SA_AUTH_MATCH_RULE_clear(&rule);
    rule.port.sys_id = SOC_PPD_IGNORE_VAL;
    rule.vid = SOC_PPD_IGNORE_VAL;
    rule.rule_type = SOC_PPC_LLP_SA_MATCH_RULE_TYPE_ALL;

    soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&block_range);
    block_range.entries_to_act = _BCM_PETRA_L2_TRVRS_ITER_BLK_SIZE;
    block_range.entries_to_scan = SOC_SAND_TBL_ITER_SCAN_ALL;

    for (indx = 0; indx < _BCM_PETRA_L2_TRVRS_ITER_BLK_SIZE; indx++) {
        soc_sand_SAND_PP_MAC_ADDRESS_clear(&_bcm_l2_auth_traverse_mac_addrs[unit][indx]);
        SOC_PPC_LLP_SA_AUTH_MAC_INFO_clear(&_bcm_l2_auth_traverse_mac_infos[unit][indx]);
    }

    while (!SOC_SAND_TBL_ITER_IS_END(&(block_range.iter))) {
        soc_sand_rv = soc_ppd_llp_sa_auth_get_block(soc_sand_dev_id, &rule, &block_range, 
                                                    _bcm_l2_auth_traverse_mac_addrs[unit], _bcm_l2_auth_traverse_mac_infos[unit], &nof_entries);
        SOC_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (nof_entries == 0) {
            break;
        }

        for (indx = 0; indx < nof_entries; indx++) {
/* set mac_auth_enable to 0, for deleting mac authentication address */
            soc_sand_rv = soc_ppd_llp_sa_auth_mac_info_set(soc_sand_dev_id, &_bcm_l2_auth_traverse_mac_addrs[unit][indx], 
                                                           &_bcm_l2_auth_traverse_mac_infos[unit][indx], 0, &failure_indication);
            SOC_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_l2_auth_get(
                      int unit, 
                      bcm_l2_auth_addr_t *addr)
{
    int rv = BCM_E_NONE;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    SOC_SAND_PP_MAC_ADDRESS  mac_address_key;
    SOC_PPC_LLP_SA_AUTH_MAC_INFO mac_auth_info;
    uint8 mac_auth_enable;

    BCMDNX_INIT_FUNC_DEFS;

/* check parameters */
    BCM_DPP_UNIT_CHECK(unit);

    soc_sand_dev_id = (unit);

/* convert BCM mac  to PPD mac type */
    rv = _bcm_petra_mac_to_sand_mac(addr->sa_mac, &mac_address_key);
    BCM_IF_ERROR_RETURN(rv);

    SOC_PPC_LLP_SA_AUTH_MAC_INFO_clear(&mac_auth_info);

/*  call GET function */
    soc_sand_rv  = soc_ppd_llp_sa_auth_mac_info_get(soc_sand_dev_id, &mac_address_key, &mac_auth_info, &mac_auth_enable);
    SOC_SAND_IF_ERR_RETURN(soc_sand_rv);

    if (!mac_auth_enable) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Key not found")));
    }

/* convert PPD auth info to BCM type */
    rv = _bcm_petra_l2_auth_from_sand_auth_info(unit, addr, &mac_auth_info);
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_l2_auth_traverse(
                           int unit, 
                           bcm_l2_auth_traverse_cb trav_fn, 
                           void *user_data)
{
    int rv = BCM_E_NONE;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    SOC_PPC_LLP_SA_AUTH_MATCH_RULE rule;
    SOC_SAND_TABLE_BLOCK_RANGE block_range;
    bcm_l2_auth_addr_t addr;
    uint32 nof_entries=0;
    int indx;

    BCMDNX_INIT_FUNC_DEFS;

    /* check parameters */
    BCM_DPP_UNIT_CHECK(unit);



    soc_sand_dev_id = (unit);

    /* traverse entry by entry */
    /* for traversing, ignore port and vid to match every entry */
    SOC_PPC_LLP_SA_AUTH_MATCH_RULE_clear(&rule);
    rule.port.sys_id = SOC_PPD_IGNORE_VAL;
    rule.vid = SOC_PPD_IGNORE_VAL;
    rule.rule_type = SOC_PPC_LLP_SA_MATCH_RULE_TYPE_ALL;

    soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&block_range);
    block_range.entries_to_act = _BCM_PETRA_L2_TRVRS_ITER_BLK_SIZE;
    block_range.entries_to_scan = SOC_SAND_TBL_ITER_SCAN_ALL;

    for (indx = 0; indx < _BCM_PETRA_L2_TRVRS_ITER_BLK_SIZE; indx++) {
        soc_sand_SAND_PP_MAC_ADDRESS_clear(&_bcm_l2_auth_traverse_mac_addrs[unit][indx]);
        SOC_PPC_LLP_SA_AUTH_MAC_INFO_clear(&_bcm_l2_auth_traverse_mac_infos[unit][indx]);
    }

    while (!SOC_SAND_TBL_ITER_IS_END(&(block_range.iter))) {
        soc_sand_rv = soc_ppd_llp_sa_auth_get_block(soc_sand_dev_id, &rule, &block_range, 
                                                    _bcm_l2_auth_traverse_mac_addrs[unit], _bcm_l2_auth_traverse_mac_infos[unit], &nof_entries);
        SOC_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (nof_entries == 0) {
            break;
        }

        for (indx = 0; indx < nof_entries; indx++) {
            /* act on entry */
            if (trav_fn != NULL) {
                sal_memset(&addr, 0, sizeof(bcm_l2_auth_addr_t));
                rv = _bcm_petra_mac_from_sand_mac(addr.sa_mac, &_bcm_l2_auth_traverse_mac_addrs[unit][indx]);
                BCM_IF_ERROR_RETURN(rv);

                /* convert PPD auth info to BCM type */
                rv = _bcm_petra_l2_auth_from_sand_auth_info(unit, &addr, &_bcm_l2_auth_traverse_mac_infos[unit][indx]);
                BCMDNX_IF_ERR_EXIT(rv);

                /* call  trav_fn */
                rv = trav_fn(unit, &addr, user_data);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }
    }
exit:
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
    BCMDNX_FUNC_RETURN;
}


int _bcm_dpp_mact_interrupts_mask(int unit, uint8 mask_interrupts) {
    bcm_switch_event_control_t event_control;
    int rv;

    BCMDNX_INIT_FUNC_DEFS;

    event_control.event_id = ARAD_INT_IHP_MACTEVENTREADY;
    event_control.index = 0;
    event_control.action = bcmSwitchEventMask; 
    rv = bcm_petra_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, event_control, mask_interrupts);
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_dpp_l2_event_fifo_interrupt_handler(int unit, uint32 *buf) {
    int rv = BCM_E_NONE;
    uint32 soc_sand_dev_id;
    uint32 soc_sand_rv;
    uint32 i;
    bcm_l2_addr_t 
        l2addr;
    SOC_PPC_FRWRD_MACT_EVENT_BUFFER   
        event_buf;
    SOC_PPC_FRWRD_MACT_EVENT_INFO   
        mact_event;

    BCMDNX_INIT_FUNC_DEFS;
    /* check parameters*/
    BCM_DPP_UNIT_CHECK(unit);
    _DPP_L2_INIT_CHECK(unit);

    SOC_PPC_FRWRD_MACT_EVENT_BUFFER_clear(&event_buf);
    SOC_PPC_FRWRD_MACT_EVENT_INFO_clear(&mact_event);

    event_buf.buff_len = SOC_PPC_FRWRD_MACT_EVENT_BUFF_MAX_SIZE;

    for (i = 0; i < event_buf.buff_len; i++) {
        event_buf.buff[i] = buf[i];
    }
    
    soc_sand_dev_id = (unit);
    soc_sand_rv = soc_ppd_frwrd_mact_event_parse(soc_sand_dev_id, 
                                                 &event_buf,
                                                 &mact_event
                                                 );
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* Map PPD key and value to BCM */
    rv = _bcm_petra_l2_from_petra_entry(unit,&l2addr, FALSE, &(mact_event.key), &(mact_event.value));
    BCMDNX_IF_ERR_EXIT(rv);
    switch (mact_event.type) {
        case SOC_PPC_FRWRD_MACT_EVENT_TYPE_LEARN:
            _BCM_PETRA_L2_CB_RUN(unit, &l2addr, BCM_L2_CALLBACK_LEARN_EVENT);
            break;
        case SOC_PPC_FRWRD_MACT_EVENT_TYPE_TRANSPLANT:
            _BCM_PETRA_L2_CB_RUN(unit, &l2addr, BCM_L2_CALLBACK_MOVE_EVENT);
            break;
        case SOC_PPC_FRWRD_MACT_EVENT_TYPE_AGED_OUT:
            _BCM_PETRA_L2_CB_RUN(unit, &l2addr, BCM_L2_CALLBACK_AGE_EVENT);
            break;
        default:
            break;
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_l2_gport_forward_info_get(int unit,int gport_id, bcm_l2_gport_forward_info_t *frwrd_info){
    return _bcm_dpp_gport_to_port_encap(unit,gport_id,&frwrd_info->phy_gport,&frwrd_info->encap_id);
}



/* verify parameter of bcm_petra_l2_egress_create
   For replace mode, get the l2_egress object and check it according to flags */
int 
_bcm_petra_l2_egress_create_verify(int unit, bcm_l2_egress_t *egr) {

    int rv; 
    int global_outlif = 0, local_outlif = 0;  /* used to get allocated outlif/eedb index */
    BCMDNX_INIT_FUNC_DEFS;


    if (SOC_IS_JERICHO_PLUS(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_l2_egress_create not supported ")));    
    }

    /* arad+ require a multiset to manage prge data entry.
       The prge data entry multiset is up only if roo is enabled */
    if (SOC_IS_ARADPLUS_A0(unit) && !SOC_IS_ROO_ENABLE(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG(" l2_egress_create require soc property roo to be enabled \n"))); 
    }

    /* check unsupported flags */
    if (egr->flags & _BCM_L2_EGRESS_UNSUPPORTED_FLAGS) {
        LOG_ERROR(BSL_LS_BCM_L2,
                  (BSL_META_U(unit,
                              "flags %x is not supported\n"), egr->flags));
        BCM_RETURN_VAL_EXIT(BCM_E_PARAM);
    }

    /*check vlan_qos_map_id*/
    if (BCM_QOS_MAP_PROFILE_GET(egr->vlan_qos_map_id) > L2_EGRESS_PCP_DEI_PROFILE_MAX) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG(" l2_egress_create vlan_qos_map_id is too big\n")));
    }

    /* check flag WITH_ID */ 
    if (egr->flags & BCM_L2_EGRESS_WITH_ID) {

        /* check encap id is an interface of type encap */
        if (!BCM_L3_ITF_TYPE_IS_LIF(egr->encap_id)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("bcm_l2_egress_create is supported only with valid encap_id and flag BCM_L2_EGRESS_WITH_ID \n"))); 
        }

        /* check the entry wasn't allocated, otherwise, we were expecting REPLACE flag */ 
        global_outlif = BCM_L3_ITF_VAL_GET(egr->encap_id); 
        rv = (SOC_IS_JERICHO(unit)) ? bcm_dpp_am_global_lif_is_alloced(unit, BCM_DPP_AM_FLAG_ALLOC_EGRESS, global_outlif) : bcm_dpp_am_l3_eep_is_alloced(unit, global_outlif);

        if ((rv == BCM_E_EXISTS) && !(egr->flags & BCM_L2_EGRESS_REPLACE)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("REPLACE and WITH_ID flags are mandatory for an existing id \n")));
        }

        if ((rv != BCM_E_EXISTS) && (egr->flags & BCM_L2_EGRESS_REPLACE)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG(" existing id is mandatory for REPLACE and WITH_ID flags \n")));
        }
    }

    /* Check */

    /* For arad+: check ethertype is ip: we support only IPoEth */
    if (SOC_IS_ARADPLUS_A0(unit) && (egr->ethertype != SOC_SAND_PP_ETHERNET_ETHERTYPE_IPV4) 
                                 && (egr->ethertype != SOC_SAND_PP_ETHERNET_ETHERTYPE_TRILL)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG(
           "bcm_l2_egress_create is only supported for IPoETH and TRILLoETH. Ethertype must be 0x%x or 0x%x \n"), 
               SOC_SAND_PP_ETHERNET_ETHERTYPE_IPV4, SOC_SAND_PP_ETHERNET_ETHERTYPE_TRILL)); 
    }

    /* check fields supported from jericho but not supported in arad+ */
    if (SOC_IS_ARADPLUS_A0(unit)) {
        if (egr->inner_tpid != 0) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("inner tpid not supported in arad+ \n"))); 
        }
        if (egr->inner_vlan != 0) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("inner vlan not supported in arad+ \n"))); 
        }
        if (egr->vlan_qos_map_id != 0) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("PCP-DEI QOS mapping not supported in arad+ \n"))); 
        }
    }

    /* for replace mode:
     * 1. check replace mode is valid
     * 2. get the l2_egress_t object
     * 3. check l2_egress object according to REPLACE flags */

    if (egr->flags & BCM_L2_EGRESS_REPLACE) {
        _bcm_lif_type_e usage;
        bcm_l2_egress_t egr_ori; 
        bcm_l2_egress_t_init(&egr_ori); 

        /* 1. check replace mode is valid: WITH_ID is mandatory  */
        if (!(egr->flags & BCM_L2_EGRESS_WITH_ID)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_L2_EGRESS_REPLACE is supported only with valid encap_id when replace egress object\n")));
        }

        /* Check that egr->encap_id is an Overlay LL OutLIF (in SW DB) */
        if (SOC_IS_JERICHO(unit))  
        {
            global_outlif = BCM_L3_ITF_VAL_GET(egr->encap_id);
            rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, global_outlif, &local_outlif);
            BCMDNX_IF_ERR_EXIT(rv);
        }
        else
        {
            local_outlif = global_outlif = BCM_L3_ITF_VAL_GET(egr->encap_id);
        }

        BCMDNX_IF_ERR_EXIT( _bcm_dpp_local_lif_sw_resources_lif_usage_get(unit, -1, local_outlif, NULL, &usage) );
        if (usage != _bcmDppLifTypeOverlayLinkLayer) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Can't replace OutLIF to Overlay LL, because it's a different type (%d)"),usage));
        }

        /* 2. get the l2_egress_t object */
        rv = bcm_petra_l2_egress_get(unit, egr->encap_id, &egr_ori); 
        BCMDNX_IF_ERR_EXIT(rv);

        /* 3. check object according to REPLACE flags */
        /* check dest mac */
        if (!(egr->flags & BCM_L2_EGRESS_DEST_MAC_REPLACE) 
            && (sal_memcmp(egr->dest_mac, egr_ori.dest_mac, sizeof(bcm_mac_t)))) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("BCM_L2_EGRESS_REPLACE cannot change dest_mac because BCM_L2_EGRESS_DEST_MAC_REPLACE is not set\n")));
        }

        /* check src mac */
        if (!(egr->flags & BCM_L2_EGRESS_SRC_MAC_REPLACE) 
            && (sal_memcmp(egr->src_mac, egr_ori.src_mac, sizeof(bcm_mac_t)))) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("BCM_L2_EGRESS_REPLACE cannot change dest_mac because BCM_L2_EGRESS_SRC_MAC_REPLACE is not set\n")));
        }

        /* check outer vlan */
        if (SOC_IS_JERICHO(unit)) {
            if (!(egr->flags & BCM_L2_EGRESS_OUTER_VLAN_REPLACE) 
                && (VLAN_CTRL_ID(egr->outer_vlan) != VLAN_CTRL_ID(egr_ori.outer_vlan))) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("BCM_L2_EGRESS_REPLACE cannot change outer vlan because BCM_L2_EGRESS_OUTER_VLAN_REPLACE is not set\n")));
            }
        } 
        /* eedb 1/4 entry is not supported. Therefore outer_vlan lsbs are not supported. Check only 8 MSBS of outer_vlan */
        else {
            if (!(egr->flags & BCM_L2_EGRESS_OUTER_VLAN_REPLACE) 
                && ((VLAN_CTRL_ID(egr->outer_vlan) & 0xFF0) != (VLAN_CTRL_ID(egr_ori.outer_vlan) & 0xFF0))) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("BCM_L2_EGRESS_REPLACE cannot change outer vlan because BCM_L2_EGRESS_OUTER_VLAN_REPLACE is not set\n")));
            }
        }
        /* check outer pcp dei */
        if (!(egr->flags & BCM_L2_EGRESS_OUTER_PRIO_REPLACE)) {
            uint8 pcp_dei; 
            uint8 pcp_dei_ori; 
            L2_EGRESS_CREATE_PCP_DEI_FROM_VLAN(pcp_dei, egr->outer_vlan); 
            L2_EGRESS_CREATE_PCP_DEI_FROM_VLAN(pcp_dei_ori, egr_ori.outer_vlan); 
            if (pcp_dei != pcp_dei_ori) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("BCM_L2_EGRESS_REPLACE cannot change outer prio because BCM_L2_EGRESS_OUTER_PRIO_REPLACE is not set\n")));
            }
        }

        /* check ethertype */
        if (!(egr->flags & BCM_L2_EGRESS_ETHERTYPE_REPLACE) 
            && (egr->ethertype != egr_ori.ethertype)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("BCM_L2_EGRESS_REPLACE cannot change ethertype because BCM_L2_EGRESS_ETHERTYPE_REPLACE is not set\n")));
        }

        /* check replace fields supported from Jericho */ 
        if (SOC_IS_JERICHO(unit)) {
            /* check inner vlan */
            if (!(egr->flags & BCM_L2_EGRESS_INNER_VLAN_REPLACE) 
                && (VLAN_CTRL_ID(egr->inner_vlan) != VLAN_CTRL_ID(egr_ori.inner_vlan))) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("BCM_L2_EGRESS_REPLACE cannot change inner vlan because BCM_L2_EGRESS_INNER_VLAN_REPLACE is not set\n")));
            }
            /* check inner pcp dei */
            if (!(egr->flags & BCM_L2_EGRESS_INNER_PRIO_REPLACE)) {
                uint8 pcp_dei; 
                uint8 pcp_dei_ori; 
                L2_EGRESS_CREATE_PCP_DEI_FROM_VLAN(pcp_dei, egr->inner_vlan); 
                L2_EGRESS_CREATE_PCP_DEI_FROM_VLAN(pcp_dei_ori, egr_ori.inner_vlan); 
                if (pcp_dei != pcp_dei_ori) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("BCM_L2_EGRESS_REPLACE cannot change inner prio because BCM_L2_EGRESS_INNER_PRIO_REPLACE is not set\n")));
                }
            }
            /* check vlan qos map */
            if (!(egr->flags & BCM_L2_EGRESS_VLAN_QOS_MAP_REPLACE))  {
                uint8 pcp_dei_profile; 
                uint8 pcp_dei_profile_ori; 
                L2_EGRESS_PCP_DEI_PROFILE_SET(pcp_dei_profile, egr->vlan_qos_map_id); 
                L2_EGRESS_PCP_DEI_PROFILE_SET(pcp_dei_profile_ori, egr_ori.vlan_qos_map_id); 
                if (pcp_dei_profile != pcp_dei_profile_ori) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("BCM_L2_EGRESS_VLAN_QOS_MAP_REPLACE cannot change vlan qos map because BCM_L2_EGRESS_VLAN_QOS_MAP_REPLACE is not set\n")));
                }
            }
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/* 
 * Helper function for bcm_l2_egress_create: get the eth_type_index pointed by eedb roo ll
 */
STATIC int 
_bcm_petra_l2_egress_eedb_roo_ll_eth_header_info_get(int unit, int outlif, int *eth_type_index_ndx,int * num_of_tags) {
    int rv;    
    uint32 next_eep[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX];
    SOC_PPC_EG_ENCAP_ENTRY_INFO encap_entry_info[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX];
    uint32 nof_entries;

    BCMDNX_INIT_FUNC_DEFS; 

    /* get the current index for CfgEtherTypeIndex from eedb entry */
    rv = soc_ppd_eg_encap_entry_get(
       unit, SOC_PPC_EG_ENCAP_EEP_TYPE_ROO_LL, outlif, 0, encap_entry_info, next_eep, &nof_entries);
    BCM_SAND_IF_ERR_EXIT(rv);
    *eth_type_index_ndx = encap_entry_info[0].entry_val.overlay_arp_encap_info.eth_type_index; 
    *num_of_tags = encap_entry_info[0].entry_val.overlay_arp_encap_info.nof_tags;
exit:
    BCMDNX_FUNC_RETURN;
}
 

/*
  * A new allocation algorithm for eth_type_index table under adv extended mpls label mode
  * 2 rounds searching process for the availble entry:
  * Round 1: partial match only searching;
  * Example 1 :
  *   If a ROO LL only adds one vlan tag, and another LL add 2 vlan tags and have occupied a index entry, 
  *  if the ethertype and outer-tpid match, they could share the same index entry without update HW.
  *   etc...
  * Example 2:
  *   If a ROO LL only add one vlan tag, and another LL add no vlan tag and have occupied a index entry.
  * If the ether type match , they could share this index entry after updating the HW
  * etc
  * Round 2: legacy full match searching;
*/
int _bcm_dpp_roo_ll_with_extend_mpls_label_adv_mode_eth_type_index_alloc(int unit,int local_out_lif, SOC_PPC_EG_ENCAP_ETHER_TYPE_INDEX_INFO *new_eth_index_entry ,int *ether_type_index,int * update_hw)
{
    int rv = BCM_E_NOT_FOUND;
    int alloc_flags =0;
    SOC_PPC_EG_ENCAP_ETHER_TYPE_INDEX_INFO          eth_type_index_entry ;
    int is_new_alloc = 0,is_last = 0,new_eth_type_index = -1;
    uint16   index = 0, partial_match_found=FALSE,found = FALSE, new_field_required=FALSE;
    _l2_ether_type_index_entry_ref_t entry_ref_cnt;

    BCMDNX_INIT_FUNC_DEFS;

    /*Check whether there is a exist full-match*/
    rv = bcm_dpp_am_template_eedb_roo_ll_format_eth_type_index_index_get(unit,new_eth_index_entry,ether_type_index);
    if (rv == BCM_E_NONE) {
        found = TRUE;
        new_eth_type_index = *ether_type_index;
        rv = bcm_dpp_am_template_eedb_roo_ll_format_eth_type_index_alloc(unit,0,new_eth_index_entry,update_hw,&new_eth_type_index);
        if (rv != BCM_E_NONE) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("Faied to alloc existing ether index entry\n")));
        }

        goto found_process;
    }

    /*Lookup a partial match*/
    for (index = 0;index < _BCM_L2_NUM_OF_ETH_TYPE_INDEX_ENTRY;index++)  {

        rv = L2_ACCESS.ether_type_index_table_ref_cnt.get(unit,index,&entry_ref_cnt);
        BCMDNX_IF_ERR_EXIT(rv);

        /*If not used, bypass it*/
        if ((entry_ref_cnt.eth_type_ref+entry_ref_cnt.outer_tpid_ref+entry_ref_cnt.inner_tpid_ref)==0){
            continue;
        }

        new_field_required = FALSE;
        sal_memset(&eth_type_index_entry,0,sizeof(SOC_PPC_EG_ENCAP_ETHER_TYPE_INDEX_INFO));

        rv = bcm_dpp_am_template_eedb_roo_ll_format_eth_type_index_get(
           unit, index,&eth_type_index_entry);
        BCMDNX_IF_ERR_EXIT(rv);

        if (_BCM_L2_IS_VALID_FIELD(new_eth_index_entry->ether_type)) {
            if (!_BCM_L2_IS_VALID_FIELD(eth_type_index_entry.ether_type)) {
                    new_field_required = TRUE;
             } else if (new_eth_index_entry->ether_type!=eth_type_index_entry.ether_type){
                    continue;
            }
        }

        if (_BCM_L2_IS_VALID_FIELD(new_eth_index_entry->tpid_0)) {
            if (!_BCM_L2_IS_VALID_FIELD(eth_type_index_entry.tpid_0)) {
                    new_field_required = TRUE;
             } else if (new_eth_index_entry->tpid_0!=eth_type_index_entry.tpid_0){
                    continue;
            }
        }

        if (_BCM_L2_IS_VALID_FIELD(new_eth_index_entry->tpid_1)) {
            if (!_BCM_L2_IS_VALID_FIELD(eth_type_index_entry.tpid_1)) {
                    new_field_required = TRUE;
             } else if (new_eth_index_entry->tpid_1!=eth_type_index_entry.tpid_1){
                    continue;
            }
        }

        partial_match_found = TRUE;
        break;

    }


    if (partial_match_found) {
        new_eth_type_index = index;
        found = TRUE;
        rv = bcm_dpp_am_template_eedb_roo_ll_format_eth_type_index_alloc(unit,0,&eth_type_index_entry,&is_new_alloc,&new_eth_type_index);
        BCMDNX_IF_ERR_EXIT(rv);

        if (new_field_required) {
            alloc_flags = BCM_DPP_AM_FLAG_ALLOC_WITH_ID;
            eth_type_index_entry.ether_type = _BCM_L2_IS_VALID_FIELD(new_eth_index_entry->ether_type)?new_eth_index_entry->ether_type:eth_type_index_entry.ether_type;
            eth_type_index_entry.tpid_0= _BCM_L2_IS_VALID_FIELD(new_eth_index_entry->tpid_0)?new_eth_index_entry->tpid_0:eth_type_index_entry.tpid_0;
            eth_type_index_entry.tpid_1= _BCM_L2_IS_VALID_FIELD(new_eth_index_entry->tpid_1)?new_eth_index_entry->tpid_1:eth_type_index_entry.tpid_1;

            rv = bcm_dpp_am_template_eedb_roo_ll_format_eth_type_index_exchange(unit, alloc_flags,index, &eth_type_index_entry, &new_eth_type_index,&is_last,&is_new_alloc);
            BCMDNX_IF_ERR_EXIT(rv);
            if (new_eth_type_index != index) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("Faied to share a in-use ether-type-index to extended roo\n")));
            }
        }
    } else {/*Try to allocate a new empty entry*/
        rv = bcm_dpp_am_template_eedb_roo_ll_format_eth_type_index_alloc(unit,0,new_eth_index_entry,update_hw,&new_eth_type_index);
        if (rv != BCM_E_NONE) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("Faied to alloc new empty ether index entry\n")));
        }
        sal_memset(&entry_ref_cnt,0,sizeof(_l2_ether_type_index_entry_ref_t));
        sal_memcpy(&eth_type_index_entry,new_eth_index_entry,sizeof(SOC_PPC_EG_ENCAP_ETHER_TYPE_INDEX_INFO));
        found = TRUE;
        new_field_required = TRUE;

    }

found_process:
    if (found) {
        *update_hw = FALSE;/*Update the HW here if necessary , so no need update HW outside of this routing*/
        rv = L2_ACCESS.ether_type_index_table_ref_cnt.get(unit,new_eth_type_index,&entry_ref_cnt);
        BCMDNX_IF_ERR_EXIT(rv);

        if (_BCM_L2_IS_VALID_FIELD(new_eth_index_entry->ether_type))entry_ref_cnt.eth_type_ref++;
        if (_BCM_L2_IS_VALID_FIELD(new_eth_index_entry->tpid_0))entry_ref_cnt.outer_tpid_ref++;
        if (_BCM_L2_IS_VALID_FIELD(new_eth_index_entry->tpid_1))entry_ref_cnt.inner_tpid_ref++;

        rv = L2_ACCESS.ether_type_index_table_ref_cnt.set(unit,new_eth_type_index,&entry_ref_cnt);
        BCMDNX_IF_ERR_EXIT(rv);

        if (new_field_required) {
            rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_eg_encap_ether_type_index_set,
                                      (unit, new_eth_type_index, &eth_type_index_entry)));
            BCMDNX_IF_ERR_EXIT(rv);
        }


    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("No availble resource for ether type index\n")));
    }

exit:

    *ether_type_index = new_eth_type_index;

    BCMDNX_FUNC_RETURN;

}

int _bcm_dpp_roo_ll_with_extend_mpls_label_adv_mode_eth_type_index_free(int unit,int local_out_lif,int ether_type_index,int *is_last,int num_of_tags)
{
    int rv = 0;
    SOC_PPC_EG_ENCAP_ETHER_TYPE_INDEX_INFO          eth_type_index_entry ;
    uint8 update_hw=FALSE;
    _l2_ether_type_index_entry_ref_t entry_ref_cnt,old_entry_ref_cnt;
    int new_index = 0;
    int is_new_alloc;
    int  alloc_flags = BCM_DPP_AM_FLAG_ALLOC_WITH_ID;
    uint32 ref_count = 0;
    _bcm_dpp_gport_sw_resources gport_sw_resources;


    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_local_lif_to_sw_resources(unit, -1, local_out_lif, _BCM_DPP_GPORT_SW_RESOURCES_EGRESS, &gport_sw_resources);
    BCMDNX_IF_ERR_EXIT(rv);


    rv = L2_ACCESS.ether_type_index_table_ref_cnt.get(unit,ether_type_index,&entry_ref_cnt);
    BCMDNX_IF_ERR_EXIT(rv);

    sal_memset(&eth_type_index_entry,0,sizeof(SOC_PPC_EG_ENCAP_ETHER_TYPE_INDEX_INFO));

    rv = bcm_dpp_am_template_eedb_roo_ll_format_eth_type_index_get(
       unit, ether_type_index,&eth_type_index_entry);
    BCMDNX_IF_ERR_EXIT(rv);

    sal_memcpy(&old_entry_ref_cnt,&entry_ref_cnt,sizeof(_l2_ether_type_index_entry_ref_t));

    if (gport_sw_resources.out_lif_sw_resources.flags & _BCM_DPP_OUTLIF_MATCH_INFO_MPLS_EXTENDED_LABEL) {
        entry_ref_cnt.inner_tpid_ref--;
        if (gport_sw_resources.out_lif_sw_resources.flags & _BCM_DPP_OUTLIF_MATCH_INFO_MPLS_EXTENDED_LABEL_TAGGED)entry_ref_cnt.outer_tpid_ref--;
    } else {
        entry_ref_cnt.eth_type_ref--;
        if (num_of_tags > 0)entry_ref_cnt.outer_tpid_ref--;
        if (num_of_tags > 1)entry_ref_cnt.inner_tpid_ref--;

    }

    if ((old_entry_ref_cnt.eth_type_ref !=0) && (entry_ref_cnt.eth_type_ref == 0)) {
        update_hw = TRUE;
        eth_type_index_entry.ether_type = _BCM_L2_INVALID_ETH_TYPE;
    }
  
    if ((old_entry_ref_cnt.outer_tpid_ref !=0) && (entry_ref_cnt.outer_tpid_ref == 0))
    {
        update_hw = TRUE;
        eth_type_index_entry.tpid_0 = _BCM_L2_INVALID_TPID;
    }

    if ((old_entry_ref_cnt.inner_tpid_ref !=0) && (entry_ref_cnt.inner_tpid_ref == 0)){
        update_hw = TRUE;
        eth_type_index_entry.tpid_1 = _BCM_L2_INVALID_TPID;
    }

    rv = bcm_dpp_am_template_eedb_roo_ll_format_eth_type_index_ref_count_get(
       unit, ether_type_index, &ref_count); 
    BCMDNX_IF_ERR_EXIT(rv);

    if ((ref_count >1) && update_hw) {
        new_index = ether_type_index;

        rv = bcm_dpp_am_template_eedb_roo_ll_format_eth_type_index_exchange(unit, alloc_flags,ether_type_index, &eth_type_index_entry, &new_index,is_last,&is_new_alloc);
        BCMDNX_IF_ERR_EXIT(rv);
        if (ether_type_index != new_index) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("Faied to update the field value when try to release it\n")));
        }

    }

    rv = bcm_dpp_am_template_eedb_roo_ll_format_eth_type_index_free(
       unit, ether_type_index, is_last); 
    BCMDNX_IF_ERR_EXIT(rv);

    rv = L2_ACCESS.ether_type_index_table_ref_cnt.set(unit,ether_type_index,&entry_ref_cnt);
    BCMDNX_IF_ERR_EXIT(rv);


exit:
    BCMDNX_FUNC_RETURN;

}


int 
bcm_petra_l2_egress_create(int unit, bcm_l2_egress_t *egr)
{
    int rv = BCM_E_NONE;
    int global_outlif = 0, local_ll_outlif = 0, local_tunnel_outlif = 0;  /* used to get allocated outlif/eedb index (ll and tunnel) */
    SOC_PPC_EG_ENCAP_OVERLAY_ARP_ENCAP_INFO ll_encap_info;  
    int tunnel_eep_index;
    uint8 is_tunnel;
    uint32 alloc_flags = 0;
    int update; /* true if replace mode */
    uint16 mpls_extended_label_ether_type_index_table_is_set = 0;
    uint16 advanced_extension_mpls_label_mode;

    BCMDNX_INIT_FUNC_DEFS;

    advanced_extension_mpls_label_mode = (soc_property_get(unit, spn_ROO_EXTENSION_LABEL_ENCAPSULATION, 0) && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "advanced_extension_label_mode", 0))?1:0;


    /* check parameters*/
    BCM_DPP_UNIT_CHECK(unit);
    rv = _bcm_petra_l2_egress_create_verify(unit, egr);
    BCMDNX_IF_ERR_EXIT(rv);

    /* allocate overlay arp eedb entry: 
     * 1. allocate outlif: allocate the eedb entry (SW)
     * 2. convert overlay arp infos, from bcm to soc
     * 3. build and insert overlay arp eedb entry (HW)
     * 4. link overlay tunnel eedb entry to overlay arp eedb entry
     * 
     * For replace functionality:
     * 1. don't allocate outlif. It's already allocated (and checked by _bcm_petra_l2_egress_create_verify)
     * 2. for extra tables: get the current entry, update it, update overlay arp infos
     * 
     * fields details for bcm_l2_egress_t: 
     * encap_id:   overlay arp eedb index
     * outer-vlan: VID[12] PCP-DEI[4]
     * inner-vlan: inner-VID[12] inner-PCP-DEI[4]
     * l3_intf:    overlay tunnel l3_intf   
     */



    /* replace mode*/
    update = (egr->flags & BCM_L2_EGRESS_REPLACE); 

    /* For mpls extended label configuration, there is only one possible profile that can be configured.
       This profile has to be configured once.
       Usage of inner vlan tag (tpid + pcp + vid) is prohibited for mpls extended label configuration*/
    if (egr->mpls_extended_label_value) {
        if (!soc_property_get(unit, spn_ROO_EXTENSION_LABEL_ENCAPSULATION, 0) || !SOC_IS_JERICHO(unit)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, \
            (_BSL_BCM_MSG("soc property ROO_EXTENSION_LABEL_ENCAPSULATION must be on and device must be jericho and above in case mpls_extended_label_value is given")));
        }

        if (egr->inner_tpid!=0 || egr->inner_vlan!=0) {
            LOG_WARN(BSL_LS_BCM_L2,(BSL_META_U(unit, 
                                    "When configuring a mpls extended label, the maximum number of vlan tags is 1: inner_tpid and inner_vlan will be ignored\n")));
        }

        BCMDNX_IF_ERR_EXIT(_bcm_petra_l2_mpls_extended_label_ether_type_index_table_is_set_get(unit, &mpls_extended_label_ether_type_index_table_is_set));

        if (mpls_extended_label_ether_type_index_table_is_set) {
            if (egr->outer_tpid!=0 || egr->ethertype!=0) {
                LOG_WARN(BSL_LS_BCM_L2,(BSL_META_U(unit, 
                                        "When configuring a mpls extended label,outer tpid and ethertype are configured only once. These values won't be applied\n")));
            }
        } 
    }

    /* 1. allocate outlif: allocate the eedb entry */
    /* with_id: outlif is supplied. */
    if (egr->flags & BCM_L2_EGRESS_WITH_ID) {
        global_outlif = BCM_L3_ITF_VAL_GET(egr->encap_id); 

        /* before jer, global lif = local lif. */
        if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
            local_ll_outlif = global_outlif; 
        } else {
            if (update) { /* Get local lif only in case outlif exists */
                rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, global_outlif, &local_ll_outlif);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }

        alloc_flags = BCM_DPP_AM_FLAG_ALLOC_WITH_ID;
    }

    /* allocate the eedb entry. No allocation if replace */
    if (!update) {

        if (SOC_IS_JERICHO(unit)) {
            bcm_dpp_am_local_out_lif_info_t local_out_lif_info;

            sal_memset(&local_out_lif_info, 0, sizeof(local_out_lif_info));

            /* Set the local lif info */
            local_out_lif_info.app_alloc_info.pool_id = dpp_am_res_eep_global;
            local_out_lif_info.app_alloc_info.application_type = bcm_dpp_am_egress_encap_app_linker_layer;

            /* wide entry is always allocated */
            local_out_lif_info.local_lif_flags |= BCM_DPP_AM_OUT_LIF_FLAG_WIDE;
            local_out_lif_info.counting_profile_id = BCM_DPP_AM_COUNTING_PROFILE_NONE;

            rv = _bcm_dpp_gport_alloc_global_and_local_lif(unit, alloc_flags, &global_outlif, NULL, &local_out_lif_info);
            BCM_IF_ERROR_RETURN(rv);

            /* Retrieve the allocated local lif */
            local_ll_outlif = local_out_lif_info.base_lif_id;

        } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
            rv = bcm_dpp_am_l3_eep_alloc(unit,_BCM_DPP_AM_L3_EEP_TYPE_ROO_LINKER_LAYER,alloc_flags ,&local_ll_outlif);
            BCM_IF_ERROR_RETURN(rv);
            global_outlif = local_ll_outlif;
        }
    }

    /* update allocated encap id (overylay arp eedb index) */
    BCM_L3_ITF_SET(egr->encap_id,BCM_L3_ITF_TYPE_LIF, global_outlif); 

    rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, global_outlif, &local_ll_outlif);
    if (rv != BCM_E_NONE) {
         BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("eep not allocated")));
    }              

    /* 2. convert overlay arp infos, from bcm to soc */

    SOC_PPC_EG_ENCAP_OVERLAY_ARP_ENCAP_INFO_clear(&ll_encap_info);

    /* dest mac */
    rv = _bcm_petra_mac_to_sand_mac(
       egr->dest_mac,
       &ll_encap_info.dest_mac);
    BCMDNX_IF_ERR_EXIT(rv);
    /* src mac */
    rv = _bcm_petra_mac_to_sand_mac(
       egr->src_mac,
       &ll_encap_info.src_mac);
    BCMDNX_IF_ERR_EXIT(rv);

    /* In case egr->mpls_extended_label_value is non zero, outer vlan is stored in the inner vlan part in the entry
       The mpls extended label is stored in the outer vlan part in the entry */
    if (egr->mpls_extended_label_value) {
        if (advanced_extension_mpls_label_mode) {
            ll_encap_info.out_vid = VLAN_CTRL_ID(egr->outer_vlan);
            L2_EGRESS_CREATE_PCP_DEI_FROM_VLAN(ll_encap_info.pcp_dei, egr->outer_vlan);
        } else {
            ll_encap_info.inner_vid = VLAN_CTRL_ID(egr->outer_vlan);
            L2_EGRESS_CREATE_PCP_DEI_FROM_VLAN(ll_encap_info.inner_pcp_dei, egr->outer_vlan); 
        }
    } else {
        /* outer-vid and PCP-DEI are both encode in outer-vlan field: encoding: {PCP DEI [15:12] VID[11:0]} */
        ll_encap_info.out_vid = VLAN_CTRL_ID(egr->outer_vlan);
        /* PCP DEI
        * extract outer PCP DEI from outer-vlan */
        L2_EGRESS_CREATE_PCP_DEI_FROM_VLAN(ll_encap_info.pcp_dei, egr->outer_vlan);
    }
    /* In arad+ ethertype and outer tpid are saved in eedb entry (prge data entry)
       In Jericho, they are saved in an additional table (eth type index table) */
    if (SOC_IS_ARADPLUS_A0(unit)) {
        /* ethertype */
        ll_encap_info.ether_type = egr->ethertype;
        /* outer tpid */
        ll_encap_info.outer_tpid = egr->outer_tpid;
    }

    /* from jericho:
       jericho supports inner vlan tag and PCP-DEI QOS mapping */
    if (SOC_IS_JERICHO(unit)) {
        SOC_PPC_EG_ENCAP_ETHER_TYPE_INDEX_INFO          eth_type_index_entry ;

        int template_is_new = FALSE; 
        int old_eth_type_index_ndx = 0,old_num_of_tags = 0; 
        int new_eth_type_index_ndx = 0; 
        int template_is_last = 0;

        soc_sand_os_memset(&eth_type_index_entry, 0x0, sizeof(SOC_PPC_EG_ENCAP_ETHER_TYPE_INDEX_INFO)); 

        /* For configuration of mpls extended label, 12 lsbs of the label are inserted to the outer vid field,
           bits 12-15 of the label are inserted into the outer pcp dei field, bits 16-19 are inserted into
           the outer tpid field in the extended entry. These bits are the ether type index table's index, so we'll
           see in the entry: outer tpid == ether type index table's index == mpls_extended_label_value[16:19] (see further on) */
        if (egr->mpls_extended_label_value) {
            if (advanced_extension_mpls_label_mode) {
                L2_EGRESS_CREATE_OUTER_VID_FROM_MPLS_EXTENDED_LABEL(ll_encap_info.inner_vid, egr->mpls_extended_label_value); 
                L2_EGRESS_CREATE_OUTER_PCP_DEI_FROM_MPLS_EXTENDED_LABEL(ll_encap_info.inner_pcp_dei, egr->mpls_extended_label_value);
            }else {
                L2_EGRESS_CREATE_OUTER_VID_FROM_MPLS_EXTENDED_LABEL(ll_encap_info.out_vid, egr->mpls_extended_label_value); 
                L2_EGRESS_CREATE_OUTER_PCP_DEI_FROM_MPLS_EXTENDED_LABEL(ll_encap_info.pcp_dei, egr->mpls_extended_label_value);
                L2_EGRESS_CREATE_ETHER_TYPE_INDEX_NDX_FROM_MPLS_EXTENDED_LABEL(ll_encap_info.eth_type_index,egr->mpls_extended_label_value);
            }
        } else {
            ll_encap_info.inner_vid = VLAN_CTRL_ID(egr->inner_vlan);
            L2_EGRESS_CREATE_PCP_DEI_FROM_VLAN(ll_encap_info.inner_pcp_dei, egr->inner_vlan); 
        }

        /*PCP DEI profile */
        L2_EGRESS_PCP_DEI_PROFILE_SET(ll_encap_info.pcp_dei_profile, egr->vlan_qos_map_id); 

        /* number of tags (indicate if EVE is needed) */
        ll_encap_info.nof_tags = 0;
        /* If a mpls extended label is given, inner vlan is used (It contains the outer vlan)*/
        if (egr->inner_tpid != 0 || egr->mpls_extended_label_value) ll_encap_info.nof_tags++; 
        if (egr->outer_tpid != 0) ll_encap_info.nof_tags++;
        if (egr->mpls_extended_label_value&&advanced_extension_mpls_label_mode)ll_encap_info.nof_tags=2;

        /* allocate entry for CfgEtherTypeIndex
           (additional table for eedb of type roo link layer format) */
        eth_type_index_entry.ether_type = egr->ethertype; 

        if (egr->mpls_extended_label_value) {
            /* with mpls extended label configured, inner tag holds the outer tag information */
            if (advanced_extension_mpls_label_mode) {
                eth_type_index_entry.tpid_0 = egr->outer_tpid?egr->outer_tpid:_BCM_L2_INVALID_TPID;
                L2_EGRESS_CREATE_INNER_TPID_FROM_MPLS_EXTENDED_LABEL( eth_type_index_entry.tpid_1,egr->mpls_extended_label_value,egr->outer_tpid);
                eth_type_index_entry.ether_type = _BCM_L2_INVALID_ETH_TYPE;
           }else {
                eth_type_index_entry.tpid_1 = egr->outer_tpid;
            }

        } else {
            eth_type_index_entry.tpid_0 = advanced_extension_mpls_label_mode?(egr->outer_tpid?egr->outer_tpid:_BCM_L2_INVALID_TPID):egr->outer_tpid; 
            eth_type_index_entry.tpid_1 = advanced_extension_mpls_label_mode?(egr->inner_tpid?egr->inner_tpid:_BCM_L2_INVALID_TPID):egr->inner_tpid;
            eth_type_index_entry.ether_type = advanced_extension_mpls_label_mode?(egr->ethertype?egr->ethertype:_BCM_L2_INVALID_ETH_TYPE):egr->ethertype;
        }
        
        if (!egr->mpls_extended_label_value || advanced_extension_mpls_label_mode){

            /* (sw) allocate entry for CfgEtherTypeIndex */

            /* replace mode: get the current index for CfgEtherTypeIndex from eedb entry
               Note: outlif != 0 since we've tested that replace and with_id flags are both required
               Update will not be done in case a mpls extended label is given (table is configured only once) */
            if (update) {

                /* get the current index for CfgEtherTypeIndex from eedb entry */
                rv = _bcm_petra_l2_egress_eedb_roo_ll_eth_header_info_get(unit, local_ll_outlif, &old_eth_type_index_ndx,&old_num_of_tags); 
                BCMDNX_IF_ERR_EXIT(rv);

                /* clear sw allocation for current eth_type_index */
                if (advanced_extension_mpls_label_mode) {
                    rv = _bcm_dpp_roo_ll_with_extend_mpls_label_adv_mode_eth_type_index_free(unit, local_ll_outlif,old_eth_type_index_ndx, &template_is_last,old_num_of_tags); 
                    BCMDNX_IF_ERR_EXIT(rv);

                } else {
                    rv = bcm_dpp_am_template_eedb_roo_ll_format_eth_type_index_free(unit, old_eth_type_index_ndx, &template_is_last); 
                    BCMDNX_IF_ERR_EXIT(rv);
                }

            } 

            if (advanced_extension_mpls_label_mode) {
                /* sw allocation for new eth_type_index */
                rv = _bcm_dpp_roo_ll_with_extend_mpls_label_adv_mode_eth_type_index_alloc(
                   unit, local_ll_outlif, &eth_type_index_entry, &new_eth_type_index_ndx,&template_is_new); 
                BCMDNX_IF_ERR_EXIT(rv);

            } else {
                /* sw allocation for new eth_type_index */
                rv = bcm_dpp_am_template_eedb_roo_ll_format_eth_type_index_alloc(
                   unit, 0, &eth_type_index_entry, &template_is_new, &new_eth_type_index_ndx); 
                BCMDNX_IF_ERR_EXIT(rv);
            }
            ll_encap_info.eth_type_index = new_eth_type_index_ndx; 

            /* in case we have removed last entry and add a different new entry, hw remove of the eth_type_index entry    */
            if (update && template_is_last && (old_eth_type_index_ndx != new_eth_type_index_ndx)) {
                rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_eg_encap_ether_type_index_clear,
                          (unit, old_eth_type_index_ndx)));
                BCMDNX_IF_ERR_EXIT(rv);
            }

            if (template_is_new) {
                /* hw allocation of eth_type_index entry */

                rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_eg_encap_ether_type_index_set,
                                          (unit, ll_encap_info.eth_type_index, &eth_type_index_entry)));
                BCMDNX_IF_ERR_EXIT(rv);
            }
        } 

        if(egr->mpls_extended_label_value && !advanced_extension_mpls_label_mode){
            if (!mpls_extended_label_ether_type_index_table_is_set) {
                   /* In this case, fill the whole table */
                   rv = _bcm_petra_l2_mpls_extended_label_fill_ether_type_index_table(unit,eth_type_index_entry);
                   BCMDNX_IF_ERR_EXIT(rv);
        
                   BCMDNX_IF_ERR_EXIT(_bcm_petra_l2_mpls_extended_label_ether_type_index_table_is_set_set(unit,1));
              }
        
      }

    }

    /* 3. build and insert overlay arp eedb entry */

    /* 3. insert */

    /* build eedb entry, and insert the entry in allocated place */
    rv = soc_ppd_eg_encap_overlay_arp_data_entry_add(unit, local_ll_outlif, &ll_encap_info);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Update SW DB */
    rv = _bcm_dpp_out_lif_overlay_ll_match_add(unit, egr, local_ll_outlif);
    BCMDNX_IF_ERR_EXIT(rv);

    /* 4. link overlay tunnel eedb entry to overlay arp eedb entry */

    if (egr->l3_intf > 0) {
        if (BCM_L3_ITF_TYPE_IS_LIF(egr->l3_intf)) {
            /* get eedb index from router/tunnel intf */
            rv = _bcm_l3_intf_eep_get(unit, egr->l3_intf, &is_tunnel, &tunnel_eep_index);
            BCMDNX_IF_ERR_EXIT(rv);

            rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, tunnel_eep_index, &local_tunnel_outlif);
            BCMDNX_IF_ERR_EXIT(rv);

            /* update eedb tunnel entry */
            rv = _bcm_tunnel_intf_ll_eep_set(unit,egr->l3_intf,local_tunnel_outlif, local_ll_outlif);
            BCMDNX_IF_ERR_EXIT(rv);
        } 
        /* tunnel intf is invalid */
        else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Intf given is not valid 0x%x"), egr->l3_intf));
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_l2_egress_get(
    int unit, 
    bcm_if_t encap_id, 
    bcm_l2_egress_t *egr)
{
    SOC_PPC_EG_ENCAP_ENTRY_INFO encap_entry_info[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX];
    uint32 next_eep[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX];
    uint32 nof_entries; 
    SOC_PPC_EG_ENCAP_OVERLAY_ARP_ENCAP_INFO ll_encap_info;

    int rv = BCM_E_NONE; 
    _bcm_lif_type_e usage;
    int global_outlif = 0, local_outlif = 0;  /* used to get allocated outlif/eedb index */
    uint16 advanced_extension_mpls_label_mode;

    BCMDNX_INIT_FUNC_DEFS; 

    advanced_extension_mpls_label_mode = (soc_property_get(unit, spn_ROO_EXTENSION_LABEL_ENCAPSULATION, 0) && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "advanced_extension_label_mode", 0))?1:0;


    if (SOC_IS_JERICHO_PLUS(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_l2_egress_get not supported ")));    
    }

    /* check parameters*/
    BCM_DPP_UNIT_CHECK(unit); 
    BCMDNX_NULL_CHECK(egr); 
    /* check encap id is an interface of type encap */
    if (!BCM_L3_ITF_TYPE_IS_LIF(encap_id)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("bcm_petra_l2_egress_get is supported only with encap_id of type encap \n"))); 
    }

    global_outlif = BCM_L3_ITF_VAL_GET(encap_id); 

    if (SOC_IS_JERICHO(unit))  
    {
        rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, global_outlif, &local_outlif);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else
    {
        local_outlif = global_outlif;
    }

    /* Check in SW DB that the LIF is LL */
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_local_lif_sw_resources_lif_usage_get(unit, -1, local_outlif, NULL, &usage));
    if (usage != _bcmDppLifTypeOverlayLinkLayer) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("OutLIF is not L2")));
    }

    /* get overlay arp eedb entry:
     * 1. check outlif is allocated 
     * 2. get eedb entry
     * 3. convert overlay arp infos, from soc to bcm 
     */

    /* 1. check outlif (encap_id) is allocated  */
    rv = (SOC_IS_JERICHO(unit)) ? bcm_dpp_am_global_lif_is_alloced(unit, BCM_DPP_AM_FLAG_ALLOC_EGRESS, global_outlif) : bcm_dpp_am_l3_eep_is_alloced(unit, global_outlif);
    if (rv != BCM_E_EXISTS) {
       BCMDNX_IF_ERR_EXIT(rv);
    }

    egr->encap_id = encap_id; 

    /* 2. Parse eedb entry */

    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        /*    2.1 get eedb entry as a data info entry
         *        For arad+, parse eedb of type data entry. Only parse data entry field. */
        rv = soc_ppd_eg_encap_entry_get(
           unit, SOC_PPC_EG_ENCAP_EEP_TYPE_DATA, local_outlif, 0, encap_entry_info, next_eep, &nof_entries);
        BCM_SAND_IF_ERR_EXIT(rv);

        /* 2.2 parse data entry field from encap info to fill all fields in encap info */
        rv = soc_ppd_eg_encap_entry_data_info_to_overlay_arp_encap_info(
           unit, &(encap_entry_info[0]));
        BCM_SAND_IF_ERR_EXIT(rv);
    } 
    /* from jericho */
    else {
        /*  2.1 From jericho, parse eedb of type roo LL. Parse all fields and fill encap info */
        rv = soc_ppd_eg_encap_entry_get(
           unit, SOC_PPC_EG_ENCAP_EEP_TYPE_ROO_LL, local_outlif, 0, encap_entry_info, next_eep, &nof_entries);
        BCM_SAND_IF_ERR_EXIT(rv);
    }
    ll_encap_info = encap_entry_info[0].entry_val.overlay_arp_encap_info; 


    /* 3. convert overlay arp infos, from soc to bcm  */
    /* dest mac */
    rv = _bcm_petra_mac_from_sand_mac(egr->dest_mac, &ll_encap_info.dest_mac);
    BCMDNX_IF_ERR_EXIT(rv);

    /* src mac */
    rv = _bcm_petra_mac_from_sand_mac(egr->src_mac, &ll_encap_info.src_mac);
    BCMDNX_IF_ERR_EXIT(rv);

    /* outer-vid and PCP-DEI
       Both are encode in outer-vlan field: encoding: {PCP DEI [15:12] VID[11:0]} */
    L2_EGRESS_GET_VLAN_FROM_VID_PCP_DEI(egr->outer_vlan, 
                                        ll_encap_info.out_vid, 
                                        ll_encap_info.pcp_dei);


    /* In arad+ ethertype and outer tpid are saved in eedb entry (prge data entry)
       In Jericho, they are saved in an additional table (eth type index table) */
    if (SOC_IS_ARADPLUS_A0(unit)) {
        /* ethertype */
        egr->ethertype = ll_encap_info.ether_type;

        /* outer tpid */
        egr->outer_tpid = ll_encap_info.outer_tpid;
    }

    /* from jericho */
    if (SOC_IS_JERICHO(unit)) {

        _bcm_dpp_gport_sw_resources gport_sw_resources;

        SOC_PPC_EG_ENCAP_ETHER_TYPE_INDEX_INFO eth_type_index_entry;
        soc_sand_os_memset(
           &eth_type_index_entry, 0x0, sizeof(SOC_PPC_EG_ENCAP_ETHER_TYPE_INDEX_INFO)); 

        BCMDNX_IF_ERR_EXIT(_bcm_dpp_local_lif_to_sw_resources(unit, -1, local_outlif, _BCM_DPP_GPORT_SW_RESOURCES_EGRESS, &gport_sw_resources));

        if (gport_sw_resources.out_lif_sw_resources.flags & _BCM_DPP_OUTLIF_MATCH_INFO_MPLS_EXTENDED_LABEL) {
            if (!soc_property_get(unit, spn_ROO_EXTENSION_LABEL_ENCAPSULATION, 0)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, \
                                    (_BSL_BCM_MSG("soc property ROO_EXTENSION_LABEL_ENCAPSULATION must be on for a ROO LL entry with mpls extended label")));
            }

           rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_eg_encap_ether_type_index_get,
                                          (unit, ll_encap_info.eth_type_index, &eth_type_index_entry)));
           BCMDNX_IF_ERR_EXIT(rv);
           /* Assemble mpls extended label from inner vid, inner pcp dei, inner tpid */

           if (advanced_extension_mpls_label_mode) {
               L2_EGRESS_GET_MPLS_EXTENDED_LABEL_FROM_OUTER_VID(egr->mpls_extended_label_value, ll_encap_info.inner_vid);
               L2_EGRESS_GET_MPLS_EXTENDED_LABEL_FROM_OUTER_PCP_DEI(egr->mpls_extended_label_value, ll_encap_info.inner_pcp_dei);
               L2_EGRESS_GET_MPLS_EXTENDED_LABEL_FROM_OUTER_TPID(egr->mpls_extended_label_value, eth_type_index_entry.tpid_1);
               egr->outer_tpid = eth_type_index_entry.tpid_0; 
               L2_EGRESS_GET_VLAN_FROM_VID_PCP_DEI(egr->outer_vlan, 
                                                   ll_encap_info.out_vid, 
                                                   ll_encap_info.pcp_dei);

           } else {
               L2_EGRESS_GET_MPLS_EXTENDED_LABEL_FROM_OUTER_VID(egr->mpls_extended_label_value, ll_encap_info.out_vid);
               L2_EGRESS_GET_MPLS_EXTENDED_LABEL_FROM_OUTER_PCP_DEI(egr->mpls_extended_label_value, ll_encap_info.pcp_dei);
               L2_EGRESS_GET_MPLS_EXTENDED_LABEL_FROM_OUTER_TPID(egr->mpls_extended_label_value, eth_type_index_entry.tpid_0);
               egr->outer_tpid = eth_type_index_entry.tpid_1; 
               L2_EGRESS_GET_VLAN_FROM_VID_PCP_DEI(egr->outer_vlan, 
                                                   ll_encap_info.inner_vid, 
                                                   ll_encap_info.inner_pcp_dei);

           }
        } else {
            /* inner vid and inner PCP DEI (see outer vid) */
            L2_EGRESS_GET_VLAN_FROM_VID_PCP_DEI(egr->inner_vlan, 
                                                ll_encap_info.inner_vid, 
                                                ll_encap_info.inner_pcp_dei);
            /* get additional info drom table CfgEtherTypeIndex.
            Get the entry from SW */
            rv = bcm_dpp_am_template_eedb_roo_ll_format_eth_type_index_get(
               unit,ll_encap_info.eth_type_index, &eth_type_index_entry); 
            BCMDNX_IF_ERR_EXIT(rv);
            /* convert from template mngr entry to bcm  */
            egr->inner_tpid = eth_type_index_entry.tpid_1; 
            egr->outer_tpid = eth_type_index_entry.tpid_0;
        }

        if (advanced_extension_mpls_label_mode) {
            egr->ethertype = _BCM_L2_IS_VALID_FIELD(eth_type_index_entry.ether_type)?eth_type_index_entry.ether_type:0; 

        } else {
            egr->ethertype = eth_type_index_entry.ether_type; 
        }

        /*PCP DEI profile */
        egr->vlan_qos_map_id = ll_encap_info.pcp_dei_profile;

        /* nof tags */
        if (egr->inner_tpid != 0) ll_encap_info.nof_tags++;
        if (egr->outer_tpid != 0) ll_encap_info.nof_tags++;

    }

    exit:
    BCMDNX_FUNC_RETURN; 
}

int 
bcm_petra_l2_egress_destroy(
    int unit, 
    bcm_if_t encap_id)
{
    int rv = BCM_E_NONE; 
    _bcm_lif_type_e usage;
    int global_outlif = 0, local_outlif = 0;    /* used to get allocated outlif/eedb index */
    uint16 advanced_extension_mpls_label_mode;

    BCMDNX_INIT_FUNC_DEFS; 

    advanced_extension_mpls_label_mode = (soc_property_get(unit, spn_ROO_EXTENSION_LABEL_ENCAPSULATION, 0) && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "advanced_extension_label_mode", 0))?1:0;
    
    /* check parameters*/
    BCM_DPP_UNIT_CHECK(unit); 

    if (SOC_IS_JERICHO_PLUS(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("bcm_petra_l2_egress_destroy not supported ")));
    }

    global_outlif = BCM_L3_ITF_VAL_GET(encap_id); 

    if (SOC_IS_JERICHO(unit))  
    {
        rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, global_outlif, &local_outlif);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else
    {
        local_outlif = global_outlif;
    }

    /* Check in SW DB that the LIF is LL */
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_local_lif_sw_resources_lif_usage_get(unit, -1, local_outlif, NULL, &usage));
    if (usage != _bcmDppLifTypeOverlayLinkLayer) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("OutLIF is not L2")));
    }

    /* check outlif is allocated */
    rv = (SOC_IS_JERICHO(unit)) ? bcm_dpp_am_global_lif_is_alloced(unit, BCM_DPP_AM_FLAG_ALLOC_EGRESS, global_outlif) : bcm_dpp_am_l3_eep_is_alloced(unit, global_outlif);
    if (rv != BCM_E_EXISTS) {
       BCMDNX_IF_ERR_EXIT(rv);
    }

    /* free used outlif (SW) */
    rv = (SOC_IS_JERICHO(unit)) ? _bcm_dpp_gport_delete_global_and_local_lif(unit, global_outlif, _BCM_GPORT_ENCAP_ID_LIF_INVALID, local_outlif) : bcm_dpp_am_l3_eep_dealloc(unit, local_outlif);
    BCMDNX_IF_ERR_EXIT(rv);

    /* free used eedb entry */
    if (SOC_IS_ARADPLUS_A0(unit)) {
        rv = soc_ppd_eg_encap_entry_remove(unit, SOC_PPC_EG_ENCAP_EEP_TYPE_DATA, local_outlif);
        BCM_SAND_IF_ERR_EXIT(rv);
    } else {
        SOC_PPC_EG_ENCAP_ENTRY_INFO encap_entry_info[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX];
        uint32 next_eep[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX];
        uint32 nof_entries; 
        int eth_type_index; 
        int is_last; 

        /* get current entry to access eth-type-index */
        rv = soc_ppd_eg_encap_entry_get(
           unit, SOC_PPC_EG_ENCAP_EEP_TYPE_ROO_LL, local_outlif, 0, encap_entry_info, next_eep, &nof_entries);
        BCM_SAND_IF_ERR_EXIT(rv);
        eth_type_index  = encap_entry_info[0].entry_val.overlay_arp_encap_info.eth_type_index; 

        /* remove eedb entry in HW*/
        rv = soc_ppd_eg_encap_entry_remove(unit, SOC_PPC_EG_ENCAP_EEP_TYPE_ROO_LL, local_outlif);
        BCM_SAND_IF_ERR_EXIT(rv);

        /* free template mgr entry (sw db). */
        if (advanced_extension_mpls_label_mode) {
            rv = _bcm_dpp_roo_ll_with_extend_mpls_label_adv_mode_eth_type_index_free(
               unit, local_outlif,eth_type_index, &is_last,encap_entry_info[0].entry_val.overlay_arp_encap_info.nof_tags); 
        } else {
            rv = bcm_dpp_am_template_eedb_roo_ll_format_eth_type_index_free(
               unit, eth_type_index, &is_last); 
        }
        BCM_SAND_IF_ERR_EXIT(rv);

        /* remove additional table: CfgEtherTypeIndex table */
        /* remove from HW 
           (if this entry was the last one pointing on CfgEtherTypeIndex table) */
        if (is_last) {
            rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_eg_encap_ether_type_index_clear,(unit, eth_type_index)));
            BCMDNX_IF_ERR_EXIT(rv);
       }
    }

    /* Remove entry from SW DB */
    rv = _bcm_dpp_local_lif_sw_resources_delete(unit, -1, local_outlif, _BCM_DPP_GPORT_SW_RESOURCES_EGRESS);
    BCMDNX_IF_ERR_EXIT(rv); 

    exit:
    BCMDNX_FUNC_RETURN; 
 
 
}

int bcm_petra_l2_egress_find(int unit,bcm_l2_egress_t *egr, bcm_if_t *encap_id) {

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_l2_egress_find not supported ")));
exit:
    BCMDNX_FUNC_RETURN;
}
int bcm_petra_l2_egress_traverse(int unit,bcm_l2_egress_traverse_cb trav_fn, void *user_data) {

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_l2_egress_traverse not supported ")));
exit:
    BCMDNX_FUNC_RETURN;
}




