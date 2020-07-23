/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    l3.c
 * Purpose: Manages L3 interface table, forwarding table, routing table
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_L3

#include <shared/bsl.h>
#include <soc/defs.h>
#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/failover.h>
#include <bcm_int/dpp/sw_db.h>

#ifdef INCLUDE_L3
 
#include <soc/drv.h>
#include <soc/dpp/mbcm.h>
#include <soc/enet.h>
#include <shared/gport.h>
#include <bcm/types.h>
#include <bcm/module.h>
#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm/l2.h>
#include <bcm/l3.h>
#include <bcm/ipmc.h>
#include <bcm/tunnel.h>
#include <bcm/stack.h>
#include <bcm/cosq.h>
#include <bcm/mpls.h>
#include <bcm/trunk.h>
#include <bcm/pkt.h>
#include <bcm/debug.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/petra_dispatch.h>

#include <bcm_int/dpp/l3.h>
#include <bcm_int/dpp/l2.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/alloc_mngr_local_lif.h>
#include <bcm_int/dpp/alloc_mngr_glif.h>
#include <bcm_int/dpp/mpls.h>
#include <bcm_int/dpp/qos.h>
#include <bcm_int/dpp/tunnel.h>
#include <bcm_int/dpp/trill.h>
#include <bcm_int/dpp/switch.h>
#include <bcm_int/dpp/pon.h>

#include <soc/dpp/mbcm_pp.h>
#include <soc/dpp/PPD/ppd_api_mymac.h>
#include <soc/dpp/PPD/ppd_api_port.h>
#include <soc/dpp/PPD/ppd_api_rif.h>
#include <soc/dpp/PPD/ppd_api_lif.h>
#include <soc/dpp/PPD/ppd_api_vsi.h>
#include <soc/dpp/PPD/ppd_api_eg_encap.h>
#include <soc/dpp/PPD/ppd_api_frwrd_ipv4.h>
#include <soc/dpp/PPD/ppd_api_frwrd_ipv6.h>
#include <soc/dpp/PPD/ppd_api_l3_src_bind.h>
#include <soc/dpp/PPD/ppd_api_lif_table.h>
#include <soc/dpp/PPD/ppd_api_slb.h>
#include <soc/dpp/PPC/ppc_api_mymac.h>
#include <soc/dpp/PPC/ppc_api_general.h>

#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>
#include <soc/dpp/trunk_sw_db.h>

#include <soc/dcmn/dcmn_wb.h>
#include <shared/swstate/access/sw_state_access.h>
#include <shared/swstate/sw_state_sync_db.h>


/***************************************************************/
/***************************************************************/

/*
 * Local defines
 */

#define DPP_L3_UNIT_INIT_CHECK \
    {                                                                                         \
        uint8 _is_allocated;                                                                  \
        BCMDNX_IF_ERR_EXIT(L3_ACCESS.is_allocated(unit, &_is_allocated));                     \
        if(!_is_allocated) {                                                                  \
            BCMDNX_ERR_EXIT_MSG(BCM_E_INIT, (_BSL_BCM_MSG("unit %d not initialised"), unit)); \
        }                                                                                     \
    }

#define DPP_L3_LOCK_TAKE \
    if (sal_mutex_take(sw_state_sync_db[(unit)].dpp.l3_lock, sal_mutex_FOREVER)) { \
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("unable to obtain l3 lock on unit %d"), unit)); \
    }
/* release lock if unit is legal */
#define DPP_L3_LOCK_RELEASE \
    if ((unit) >= 0 && ((unit) < (BCM_MAX_NUM_UNITS))) {                                                       \
        uint8 _is_allocated = FALSE;                                                                           \
        L3_ACCESS.is_allocated(unit, &_is_allocated);                                                          \
        if(_is_allocated && sal_mutex_give(sw_state_sync_db[(unit)].dpp.l3_lock)) {                            \
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("unable to release l3 lock on unit %d"), unit)); \
        }                                                                                                      \
    }


#define BCM_DPP_MTU_VALID(_mtu) (((_mtu) >= 0) && (_mtu <= ARAD_PP_MTU_MAX))

/* check that the number of native vlan expected is valid */
#define BCM_DPP_NATIVE_ROUTING_VLAN_TAGS_VALID(_native_routing_vlan_tags) (_native_routing_vlan_tags <= SOC_PPC_VLAN_TAGS_MAX)


#define DPP_COMMON_VLAN_DOMAIN 0

/* Get Block modes */
#define DPP_L3_TRAVERSE_CB                 0
#define DPP_L3_TRAVERSE_DELETE_ALL         1
#define DPP_L3_TRAVERSE_DELETE_INTERFACE   2 

#define DPP_L3_MIN_ENTRIES_IN_ECMP           (1)
#define DPP_L3_MAX_ENTRIES_IN_ECMP           (SOC_DPP_CONFIG(unit)->l3.ecmp_max_paths)

#define DPP_FRWRD_IP_GET_BLOCK_NOF_ENTRIES   (130)
#define DPP_FRWRD_IP_ENTRIES_TO_SCAN         SOC_SAND_TBL_ITER_SCAN_ALL
#define _BCM_DPP_MYMAC_VSI_LSB_MAX0  ((1<<8)-1)
#define _BCM_DPP_MYMAC_VSI_LSB_MAX1  ((1<<4)-1)

#define _BCM_DPP_VRRP_MAC_DISABLE        (0x0)
#define _BCM_DPP_VRRP_MAC_ENABLE         (0x1)
#define _BCM_DPP_VRRP_MAC_IPV4_ENABLE    (0x1) /* default - same as enable */
#define _BCM_DPP_VRRP_MAC_IPV6_ENABLE    (0x2)
#define _BCM_DPP_VRRP_MAC_IS_IPV6        (0x2) /* same as ipv6_enable */



#define L3_ROUTE_SUPPORTED_FLAGS (BCM_L3_REPLACE      | \
                                  BCM_L3_WITH_ID      | \
                                  BCM_L3_RPF          | \
                                  BCM_L3_IPMC         | \
                                  BCM_L3_TGID         | \
                                  BCM_L3_IP6          | \
                                  BCM_L3_HOST_LOCAL   | \
                                  BCM_L3_SRC_DISCARD  | \
                                  BCM_L3_ENCAP_SPACE_OPTIMIZED)

#define L3_EGRESS_SUPPORTED_FLAGS (BCM_L3_ROUTE_LABEL              | \
                                  BCM_L3_TRILL_ONLY                | \
                                  BCM_L3_ENCAP_SPACE_OPTIMIZED     | \
                                  BCM_L3_HIT                       | \
                                  BCM_L3_RPF                       | \
                                  BCM_L3_IPMC                      | \
								  BCM_L3_EGRESS_ONLY               | \
                                  BCM_L3_CASCADED                  | \
                                  BCM_L3_NATIVE_ENCAP)

#define L3_EGRESS_GET_SUPPORTED_FLAGS (BCM_L3_HIT_CLEAR)

#define L3_IS_MAC_EQUAL(m1,m2)                             \
                                 ((((m1)[0] == (m2)[0]) && \
                                   ((m1)[1] == (m2)[1]) && \
                                   ((m1)[2] == (m2)[2]) && \
                                   ((m1)[3] == (m2)[3]) && \
                                   ((m1)[4] == (m2)[4]) && \
                                   ((m1)[5] == (m2)[5])) ? 1:0)




#define _BCM_PPD_L3_IS_PROTECTED_IF(if_id)  ((if_id)->failover_id !=0 )

#define _BCM_PPD_L3_IS_PROTECTING_IF(if_id)  ((_BCM_PPD_L3_IS_PROTECTED_IF(if_id)) && ((if_id)->failover_if_id == BCM_GPORT_TYPE_NONE))

/* given FEC returns working FEC */
#define _BCM_L3_FEC_TO_WORK_FEC(fec) ((((fec)%2) == 1)?(fec-1):(fec))

#define _BCM_L3_FEC_IS_EVEN_FEC(fec_ndx) (((fec_ndx)%2) == 0)
#define _BCM_L3_FEC_IS_ODD_FEC(fec_ndx) (((fec_ndx)%2) == 1)

#define _BCM_L3_EGRESS_ECMP_RESILIENT_MATCH_FLAGS   (BCM_L3_ECMP_RESILIENT_MATCH_ECMP | BCM_L3_ECMP_RESILIENT_MATCH_HASH_KEY | BCM_L3_ECMP_RESILIENT_MATCH_INTF)
#define _BCM_L3_EGRESS_ECMP_RESILIENT_ACTION_FLAGS  (BCM_L3_ECMP_RESILIENT_REPLACE | BCM_L3_ECMP_RESILIENT_COUNT | BCM_L3_ECMP_RESILIENT_DELETE)

/* is the given gport working gport in protection, assuming gport has protection*/

/* Verify entry type is tunnel */
#define _BCM_PPD_EG_ENCAP_IS_TUNNEL_TYPE(unit,entry_type) \
    (entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_MPLS_ENCAP || entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_IPV4_ENCAP \
    || entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_IPV6_ENCAP \
    || entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_VSI || entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_SWAP_CMND \
    || entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_POP_CMND || entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_PWE)

#define _BCM_PETRA_FILL_LABEL_ENTRY_WITH_L3_EGR_CONTENTS(label_entry, egr) \
    do { \
        label_entry.label = egr->mpls_label; \
        if ((label_entry.action = egr->mpls_action) == BCM_MPLS_EGRESS_ACTION_PUSH){ \
            label_entry.flags = egr->mpls_flags; \
            label_entry.qos_map_id = egr->qos_map_id; \
            label_entry.exp = egr->mpls_exp; \
            label_entry.ttl = egr->mpls_ttl; \
            label_entry.pkt_pri = egr->mpls_pkt_pri; \
            label_entry.pkt_cfi = egr->mpls_pkt_cfi; \
        } \
    } while (0)

#define _BCM_PETRA_FILL_L3_EGR_WITH_LABEL_ENTRY_CONTENTS(egr, label_entry) \
    do { \
        egr->mpls_label = label_entry.label; \
        if ((egr->mpls_action = label_entry.action) == BCM_MPLS_EGRESS_ACTION_PUSH){ \
            egr->mpls_flags   = label_entry.flags; \
            egr->qos_map_id   = label_entry.qos_map_id; \
            egr->mpls_exp     = label_entry.exp; \
            egr->mpls_ttl     = label_entry.ttl; \
            egr->mpls_pkt_pri = label_entry.pkt_pri; \
            egr->mpls_pkt_cfi = label_entry.pkt_cfi; \
        } \
    } while (0)

#define _BCM_L3_SOURCE_BIND_IP6_COMPRESSION(rate_id) ((rate_id) & 0x80000000)
#define _BCM_L3_SOURCE_BIND_IP6_COMPRESSION_SAV(unit, port) ((_BCM_PPD_IS_PON_PP_PORT(port, unit)) && (IS_PON_PORT(unit, _BCM_PPD_GPORT_PON_TO_PHY_PORT(unit, port))))
#define _BCM_L3_SOURCE_BIND_IP6_COMPRESSION_TT_RESULT_GET(rate_id) (((rate_id) >> 15) & 0xFFFF)
#define _BCM_L3_SOURCE_BIND_IP6_COMPRESSION_RESULT_GET(rate_id) ((rate_id) & 0x7FFF)
#define _BCM_L3_SOURCE_BIND_IP6_COMPRESSION_RESULT_SET(rate_id, tt_result, result) ((rate_id) =(0x80000000 | ((tt_result) << 15) | (result)))
#define _BCM_L3_IP6_COMPRESSION_TCAM_GET(flags)  ((flags) & 0x80000000)
#define _BCM_L3_IP6_COMPRESSION_LEM_GET(flags)  ((flags) & 0x40000000)
#define _BCM_L3_IP6_COMPRESSION_TCAM_SET(flags)  ((flags) | 0x80000000)
#define _BCM_L3_IP6_COMPRESSION_LEM_SET(flags)  ((flags) | 0x40000000)

#define _BCM_L3_INGRESS_URPF_IS_STRICT  0x80000000
#define _BCM_L3_INGRESS_URPF_IS_LOOSE   0x40000000


#define _BCM_ERROR_IF_OUT_OF_RANGE(val, max_limit)  \
    if (val > max_limit) {                          \
        BCMDNX_IF_ERR_EXIT(BCM_E_PARAM);;                         \
    }                                             



#ifdef BCM_88660_A0
static bcm_mac_t vrrp_ipv4_mask = {0x00, 0x00, 0x5e, 0x00, 0x01, 0x00};
static bcm_mac_t vrrp_ipv6_mask = {0x00, 0x00, 0x5e, 0x00, 0x02, 0x00};
static bcm_mac_t vrrp_mac_mask  = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
#endif /*BCM_88660_A0*/

/* ecmp info type */
typedef struct ecmp_info_s {
    uint8 is_successive;      /* are the fecs associated with the ecmp in successive memory locations */
    uint8 exists_protected;   /* is one of the fecs associated with the ecmp protected */

    /* The following info is only used for ecmp_* APIs. */
    SOC_PPC_FEC_ID first_fec;
    SOC_PPC_FEC_ID last_fec;
    int curr_size; /* The current ECMP size. */
    int max_size; /* The max ECMP size. */
    /* exists_protected is equivalent to is_protected for ecmp_* APIs */
    /* is_successive is implied for ecmp_* APIs */

#ifdef BCM_88660_A0
    uint8 is_stateful;
#endif /* BCM_88660_A0 */

} ecmp_info_t;

typedef enum ecmp_simple_fec_mgmt_err_e {
  ECMP_SIMPLE_FEC_MGMT_ERR_NO_ERROR,
  ECMP_SIMPLE_FEC_MGMT_ERR_NOT_SUCCESSIVE,
  ECMP_SIMPLE_FEC_MGMT_ERR_NOT_FEC,
  ECMP_SIMPLE_FEC_MGMT_ERR_MIXED_PROTECTION
} ecmp_simple_fec_mgmt_err_t;

/*
 * Actions for the _bcm_ppd_rif_vsid_map_set API
 */
typedef enum rif_vsid_map_set_action_e {
  RIF_VSID_MAP_SET_ACTION_CREATE,
  RIF_VSID_MAP_SET_ACTION_UPDATE,
  RIF_VSID_MAP_SET_ACTION_DELETE
} rif_vsid_map_set_action_t;

/* Forward declarations */
int
  _bcm_ppd_eg_encap_entry_get(
    int unit,
    int eep_index,
    bcm_l3_egress_t *egr
  );

/***************************************************************/
/***************************************************************/

/*********** SW DBs translation functions *******************/
int _bcm_dpp_out_lif_ll_match_add(int unit, int lif)
{
    _bcm_dpp_gport_sw_resources gport_sw_resources;
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_local_lif_to_sw_resources(unit, -1, lif, _BCM_DPP_GPORT_SW_RESOURCES_EGRESS, &gport_sw_resources);
    BCMDNX_IF_ERR_EXIT(rv);

    gport_sw_resources.out_lif_sw_resources.lif_type = _bcmDppLifTypeLinkLayer;

    rv = _bcm_dpp_local_lif_sw_resources_set(unit, -1, lif, _BCM_DPP_GPORT_SW_RESOURCES_EGRESS, &gport_sw_resources);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * L3 Module Helper functions
 */
void
  __dpp_l3_vrf_route_t_init(_dpp_l3_vrf_route_t *data)
{
    if (NULL != data) {
        sal_memset(data, 0, sizeof(_dpp_l3_vrf_route_t));
    }
}

int
  _bcm_l3_sw_init(
    int unit
  )
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_FUNC_RETURN;
}

void
_bcm_l3_alloc_ac(
    int unit,
    int flags,
    int *ac_index
  )
{
    int status = BCM_E_NONE;
 
    status = bcm_dpp_am_l3_lif_alloc(unit,
                                     -1,
                                     flags,
                                     ac_index);

    if (status != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_L3,
                  (BSL_META_U(unit,
                              "allocation failed: %x\n"), status));
    }

    LOG_VERBOSE(BSL_LS_BCM_L3,
                (BSL_META_U(unit,
                            "alloc ac: %x\n"), *ac_index));
}

void
  _bcm_l3_intf_to_ac(
    int unit,
    bcm_if_t intf,
    int *ac_index
  )
{
    *ac_index = (int)intf;
}

void
  _bcm_l3_ac_to_intf(
    int unit,
    int ac_index,
    bcm_if_t *intf
  )
{
    *intf = (bcm_if_t)ac_index;
}

/* allocate EEPs
 *  - allocate LL EEP (in given place or any place)
 *      - if with-id, and given EEP is LL in given place
 *      - if with-id, and given EEP is MPLS LL in any place
 *      - if not wit-id in any given place.
 *  - allocate MPLS EEP
 *      - if with-id and given EEP is MPLS
 *      - if need for mpls tunnel.
 */
int
_bcm_l3_alloc_eep(
    int unit,
    bcm_l3_egress_t *egr, 
    int *local_lif_index,
    int *global_lif_index,
    int *ll_local_lif_index,
    int *ll_global_lif_index
  )
{
    int rv = BCM_E_NONE;
    uint32 alloc_flags;
    uint8 counting_profile_is_allocated;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_L3_UNIT_INIT_CHECK;

    alloc_flags = (egr->encap_id == 0) ? 0 : BCM_DPP_AM_FLAG_ALLOC_WITH_ID;

    /* need tunnel EEP */
    if (egr->flags & BCM_L3_ROUTE_LABEL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("ARAD does not support MPLS tunnel allocation in l3_egress_create. Use bcm_mpls_tunnel_initiator_* APIs."))); 
    }

    if (SOC_IS_JERICHO(unit)) {
        bcm_dpp_am_local_out_lif_info_t local_out_lif_info;

        sal_memset(&local_out_lif_info, 0, sizeof(local_out_lif_info));

        /* Set the local lif info */
        local_out_lif_info.app_alloc_info.pool_id = dpp_am_res_eep_global;
        /* allocate a native link layer */
        if (egr->flags & BCM_L3_NATIVE_ENCAP) {
            local_out_lif_info.app_alloc_info.application_type = bcm_dpp_am_egress_encap_app_native_arp;
        } 
        /* allocate a link layer */
        else {
            local_out_lif_info.app_alloc_info.application_type = 0;
        }

        /* get the counting_profile_id if range allocated*/
        BCMDNX_IF_ERR_EXIT(bcm_dpp_counter_lif_range_is_allocated(unit, &counting_profile_is_allocated));
        if (counting_profile_is_allocated) 
        {
            if ((egr->counting_profile != BCM_STAT_LIF_COUNTING_PROFILE_NONE) && (egr->counting_profile >= SOC_TMC_CNT_LIF_COUNTING_NOF_PROFILES(unit))) 
            {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("OutLIF-counting-profile %d is out of range."), egr->counting_profile));
            }
            BCMDNX_IF_ERR_EXIT(bcm_dpp_counter_lif_local_profile_get(unit, egr->counting_profile, &local_out_lif_info.counting_profile_id));
        } 
        else 
        {
            local_out_lif_info.counting_profile_id = BCM_STAT_LIF_COUNTING_PROFILE_NONE;
        }

        if (DPP_IS_FLAG_SET(egr->flags2, BCM_L3_FLAGS2_EGRESS_WIDE)) {
            local_out_lif_info.local_lif_flags |= BCM_DPP_AM_OUT_LIF_FLAG_WIDE;
        }     

        /* Allocate the global and local lif */
        rv = _bcm_dpp_gport_alloc_global_and_local_lif(unit, alloc_flags, ll_global_lif_index, NULL, &local_out_lif_info);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Retrieve the allocated local lif */
        *ll_local_lif_index = local_out_lif_info.base_lif_id;

    } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        rv = bcm_dpp_am_l3_eep_alloc(unit, _BCM_DPP_AM_L3_EEP_TYPE_DEFAULT, alloc_flags,ll_global_lif_index);
        BCMDNX_IF_ERR_EXIT(rv);
        *ll_local_lif_index = *ll_global_lif_index;
    }

    *local_lif_index = *ll_local_lif_index;
    *global_lif_index = *ll_global_lif_index;

    BCM_EXIT;

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_l3_dealloc_eep(
    int unit,
    int eep_index    
  )
{
    int status = BCM_E_NONE;
    
    BCMDNX_INIT_FUNC_DEFS;
    status = bcm_dpp_am_l3_eep_dealloc(unit,                                  
                                       eep_index);
    
    LOG_VERBOSE(BSL_LS_BCM_L3,
                (BSL_META_U(unit,
                            "dealloc eep: %x\n"), eep_index));
    BCMDNX_IF_ERR_EXIT(status);
exit:
    BCMDNX_FUNC_RETURN;
}

#ifdef BCM_ARAD_SUPPORT

int
_bcm_l3_alloc_ecmp(
    int unit,
    int flags,
    int *ecmp_index    
  )
{
    int status = BCM_E_NONE;
    bcm_ecmp_t ecmp_id = (bcm_ecmp_t) *ecmp_index;
    bcm_gport_t ecmp_gport;

    BCMDNX_INIT_FUNC_DEFS;
    status = bcm_dpp_am_ecmp_alloc(unit,
                                   flags, 
                                   &ecmp_id);

    *ecmp_index = (int)ecmp_id;
    LOG_VERBOSE(BSL_LS_BCM_L3,
                (BSL_META_U(unit,
                            "alloc ecmp: %x\n"), *ecmp_index));
    BCMDNX_IF_ERR_EXIT(status);  

    BCM_GPORT_FORWARD_PORT_SET(ecmp_gport, *ecmp_index);
    
    status = bcm_dpp_gport_mgmt_sw_resources_fec_to_gport_set(unit, *ecmp_index, ecmp_gport);
    BCMDNX_IF_ERR_EXIT(status);  
      
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_l3_dealloc_ecmp(
    int unit,
    int ecmp_index    
  )
{
    int status = BCM_E_NONE;
    bcm_ecmp_t ecmp_id = (bcm_ecmp_t) ecmp_index;

    BCMDNX_INIT_FUNC_DEFS;
    status = bcm_dpp_am_ecmp_dealloc(unit, ecmp_id);
    LOG_VERBOSE(BSL_LS_BCM_L3,
                (BSL_META_U(unit,
                            "dealloc ecmp: %x\n"), ecmp_index));
    BCMDNX_IF_ERR_EXIT(status); 
        
    status = bcm_dpp_gport_mgmt_sw_resources_fec_to_gport_set(unit, ecmp_index, 0);
    BCMDNX_IF_ERR_EXIT(status);     
       
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_l3_is_alloced_ecmp(
    int unit,
    int ecmp_index    
  )
{
    int status = BCM_E_NONE;
    bcm_ecmp_t ecmp_id = (bcm_ecmp_t) ecmp_index;

    BCMDNX_INIT_FUNC_DEFS;

    status = bcm_dpp_am_ecmp_is_alloced(unit, ecmp_id);
    
    if (status != BCM_E_NONE) {
      BCM_ERR_EXIT_NO_MSG(status);    
    }
exit:
    BCMDNX_FUNC_RETURN;
}

#else

/* Dummy implementation to allow compilation on pb. */
STATIC int
  _bcm_l3_alloc_ecmp(
    int unit,
    int flags,
    int *ecmp_index    
  ) 
{

  BCMDNX_INIT_FUNC_DEFS;

  BCMDNX_VERIFY(FALSE);

exit:
  BCMDNX_FUNC_RETURN;
}

STATIC int
  _bcm_l3_dealloc_ecmp(
    int unit,
    int ecmp_index    
)
{
  BCMDNX_INIT_FUNC_DEFS;

  BCMDNX_VERIFY(FALSE);

exit:
  BCMDNX_FUNC_RETURN;
}

STATIC int
  _bcm_l3_is_alloced_ecmp(
    int unit,
    int ecmp_index    
  )
{
  BCMDNX_INIT_FUNC_DEFS;

  BCMDNX_VERIFY(FALSE);

exit:
  BCMDNX_FUNC_RETURN;
}

#endif

int
_bcm_l3_alloc_fec(
    int unit,
    int flags,
    int usage,/*
      0: Standard FEC, 
      1: l3 ecmp (SHOULD NOT BE USED - used only by bcm_l3_egress_multipath_*, which is obsolete)
      2: Group-B FEC (Hierarchical FEC) */
    int size,/*non-zero for protection or ECMP*/
    int *fec_index    
  )
{
    int status = BCM_E_NONE;
    SOC_PPC_FEC_ID fec_id = (SOC_PPC_FEC_ID) *fec_index;

    BCMDNX_INIT_FUNC_DEFS;
    status = bcm_dpp_am_fec_alloc(unit,
                                  flags, 
                                  usage,
                                  size,
                                  &fec_id);

    *fec_index = (int)fec_id;
    LOG_VERBOSE(BSL_LS_BCM_L3,
                (BSL_META_U(unit,
                            "alloc fec: %x\n"), *fec_index));
    BCMDNX_IF_ERR_EXIT(status);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_l3_dealloc_fec(
    int unit,
    int flags,
    int size,/*non-zero for protection or ECMP*/
    int fec_index    
  )
{
    int status = BCM_E_NONE;
    SOC_PPC_FEC_ID fec_id = (SOC_PPC_FEC_ID) fec_index;

    BCMDNX_INIT_FUNC_DEFS;
    status = bcm_dpp_am_fec_dealloc(unit,
                                  flags, 
                                  size,
                                  fec_id);
    
    LOG_VERBOSE(BSL_LS_BCM_L3,
                (BSL_META_U(unit,
                            "dealloc fec: %x\n"), fec_index));
    BCMDNX_IF_ERR_EXIT(status);
exit:
    BCMDNX_FUNC_RETURN;
}

int
  _bcm_l3_intf_to_fec(
    int unit,
    bcm_if_t intf,
    SOC_PPC_FEC_ID *fec_index
  )
{
    BCMDNX_INIT_FUNC_DEFS;
    *fec_index = (SOC_PPC_FEC_ID)BCM_L3_ITF_VAL_GET(intf);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
  _bcm_l3_encap_id_to_eep(
    int unit,
    int encap_id,
    int *eep
  )
{
    BCMDNX_INIT_FUNC_DEFS;
    *eep = BCM_L3_ITF_VAL_GET(encap_id);
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

void
  _bcm_l3_fec_to_intf(
    int unit,
    SOC_PPC_FEC_ID fec_index,
    bcm_if_t *intf
  )
{
    BCM_L3_ITF_SET(*intf, BCM_L3_ITF_TYPE_FEC, fec_index); 
}

int
  _bcm_l3_sand_ipv6_addr_to_bcm_ipv6_addr(
    int unit,
    SOC_SAND_PP_IPV6_ADDRESS *soc_sand_ipv6_addr,
    bcm_ip6_t *ipv6_addr
  )
{
    uint32 i;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(soc_sand_ipv6_addr);
    BCMDNX_NULL_CHECK(ipv6_addr);

    /*Encode from uint32 to uint8*/
    for (i=0; i<WORDS2BYTES(SOC_SAND_PP_IPV6_ADDRESS_NOF_UINT32S); i++) {
        (*ipv6_addr)[i] = (uint8)((soc_sand_ipv6_addr->address[SOC_SAND_PP_IPV6_ADDRESS_NOF_UINT32S-1-i/4] >> BYTES2BITS(3 - (i % 4))) & 0xFF);
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
  _bcm_l3_bcm_ipv6_addr_to_sand_ipv6_addr(
    int unit,
    bcm_ip6_t ipv6_addr,
    SOC_SAND_PP_IPV6_ADDRESS *soc_sand_ipv6_addr
  )
{
    uint32 i;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(soc_sand_ipv6_addr);
    BCMDNX_NULL_CHECK(ipv6_addr);

    soc_sand_SAND_PP_IPV6_ADDRESS_clear(soc_sand_ipv6_addr);

    /*Encode from uint8 to uint32*/
    for (i=0; i<SOC_SAND_PP_IPV6_ADDRESS_NOF_UINT32S; i++) {
        soc_sand_ipv6_addr->address[SOC_SAND_PP_IPV6_ADDRESS_NOF_UINT32S-1-i] = ipv6_addr[WORDS2BYTES(i)]   << 24   |
                                                                                ipv6_addr[WORDS2BYTES(i)+1] << 16   |
                                                                                ipv6_addr[WORDS2BYTES(i)+2] << 8  |
                                                                                ipv6_addr[WORDS2BYTES(i)+3] << 0;
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/***************************************************************/
/***************************************************************/

/*
 * RIF Helper functions
 */

/* Function _ppd_xxxx */
int
  _bcm_ppd_mymac_msb_set(
    int unit,
    bcm_mac_t mac_address
  )
{
  uint32
    ret;
  uint8
    is_set;
  
  SOC_SAND_PP_MAC_ADDRESS
    soc_ppd_mac_org, soc_ppd_mac;

  BCMDNX_INIT_FUNC_DEFS;

  /* common utility: always returns sucess */
  ret = _bcm_petra_mac_to_sand_mac(mac_address, &soc_ppd_mac);

  /* ignore 12b' LSB */
  soc_ppd_mac.address[0] &= (uint8)(~(_BCM_DPP_MYMAC_VSI_LSB_MAX0));
  soc_ppd_mac.address[1] &= (uint8)(~(_BCM_DPP_MYMAC_VSI_LSB_MAX1));

  BCMDNX_IF_ERR_EXIT(_bcm_petra_l2_mymac_msb_is_set_get(unit, &is_set));
  if (is_set) { /* MyMac msb has been set already */

      ret = soc_ppd_mymac_msb_get(
            unit,
            &soc_ppd_mac_org
        );
      if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
      { 
          LOG_ERROR(BSL_LS_BCM_L3,
                    (BSL_META_U(unit,
                                "soc_sand error %x\n"), ret));
          BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("_bcm_petra_l2_mymac_msb_is_set_get failed")));
      }

      /* ignore 12b' LSB */
      soc_ppd_mac_org.address[0] &= (uint8)(~(_BCM_DPP_MYMAC_VSI_LSB_MAX0));
      soc_ppd_mac_org.address[1] &= (uint8)(~(_BCM_DPP_MYMAC_VSI_LSB_MAX1));

      if (!(L3_IS_MAC_EQUAL(soc_ppd_mac_org.address, soc_ppd_mac.address))) {
          /* a change in MyMac is needed */
          LOG_ERROR(BSL_LS_BCM_L3,
                    (BSL_META_U(unit,
                                "Given MyMac MSB does not match existing MyMac MSB, to change it call bcm_petra_l2_station_add\n")));
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Given MyMac MSB must match existing MyMac MSB")));
      }
  }
  else { /* MyMac msb hasn't been set yet */

      /* Set the router MyMac msb */
      ret = soc_ppd_mymac_msb_set(
            unit,
            &soc_ppd_mac
        );
      if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) { 
          LOG_ERROR(BSL_LS_BCM_L3,
                    (BSL_META_U(unit,
                                "soc_sand error %x\n"), ret));
          BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_mymac_msb_set failed")));
      }

      /* mark MyMac msb as set */
      BCMDNX_IF_ERR_EXIT(_bcm_petra_l2_mymac_msb_is_set_set(unit, 1));
  }

  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}


/* Function _ppd_xxxx */
int
  _bcm_ppd_mymac_set(
    int unit,
    bcm_vlan_t vsi,
    bcm_mac_t mac_address
  )
{
  uint32
    ret;
  
  SOC_SAND_PP_MAC_ADDRESS
    soc_ppd_mac1;

  BCMDNX_INIT_FUNC_DEFS;

  /* verify the router MyMac msb.*/
  ret = _bcm_ppd_mymac_msb_set(unit, mac_address);
  BCMDNX_IF_ERR_EXIT(ret);

  /* common utility: always returns sucess */
  ret = _bcm_petra_mac_to_sand_mac(mac_address, &soc_ppd_mac1);

  /*
   * Set the router MyMac lsb. Packets with vid equals to the RIF's vid
   * and DA equals to {MyMac-msb, MyMac-lsb} are forwarded to the routing
   * engine for further processing.
   */

  soc_ppd_mac1.address[5] = 0;
  soc_ppd_mac1.address[4] = 0;
  soc_ppd_mac1.address[3] = 0;
  soc_ppd_mac1.address[2] = 0;
  soc_ppd_mac1.address[0] &= _BCM_DPP_MYMAC_VSI_LSB_MAX0;
  soc_ppd_mac1.address[1] &= _BCM_DPP_MYMAC_VSI_LSB_MAX1;

  ret = soc_ppd_mymac_vsi_lsb_set(
            unit,
            vsi,
            &soc_ppd_mac1
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  { 
    LOG_ERROR(BSL_LS_BCM_L3,
              (BSL_META_U(unit,
                          "soc_sand error %x\n"), ret));
    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_mymac_vsi_lsb_set failed")));
  }

  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}

/* Function _ppd_xxxx */
int
  _bcm_ppd_mymac_get(
    int unit,
    bcm_vlan_t vsi,
    bcm_mac_t mac_address
  )
{
  uint32
    ret;
  
  SOC_SAND_PP_MAC_ADDRESS
    soc_ppd_mac_org, soc_ppd_mac_org1, soc_ppd_mac_org2;
  int i;

  BCMDNX_INIT_FUNC_DEFS;

  ret = soc_ppd_mymac_msb_get(
            unit,
            &soc_ppd_mac_org1
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  { 
    LOG_ERROR(BSL_LS_BCM_L3,
              (BSL_META_U(unit,
                          "soc_sand error %x\n"), ret));
    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_mymac_msb_get failed")));
  }

  ret = soc_ppd_mymac_vsi_lsb_get(
            unit,
            vsi,
            &soc_ppd_mac_org2
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  { 
    LOG_ERROR(BSL_LS_BCM_L3,
              (BSL_META_U(unit,
                          "soc_sand error %x\n"), ret));
    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_mymac_vsi_lsb_get failed")));
  }

  /* Arad uses 12 lsb of mymac for VSI */
  soc_ppd_mac_org.address[0] = soc_ppd_mac_org2.address[0];
  soc_ppd_mac_org.address[1] = (0xf0 & soc_ppd_mac_org1.address[1]) | (0x0f & soc_ppd_mac_org2.address[1]);
  for (i=2; i<SOC_SAND_PP_MAC_ADDRESS_NOF_U8; ++i)
  {
     soc_ppd_mac_org.address[i] = soc_ppd_mac_org1.address[i];
  }

  /* common utility: always returns sucess */
  BCMDNX_IF_ERR_EXIT(_bcm_petra_mac_from_sand_mac(mac_address, &soc_ppd_mac_org));

  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}

/* Function _ppd_xxxx */
int
  _bcm_ppd_vsi_info_get(
    int unit,
    bcm_vlan_t vsi,
    SOC_PPC_VSI_INFO *vsi_info
  )
{
  uint32
    ret;
  
  
  BCMDNX_INIT_FUNC_DEFS;

  SOC_PPC_VSI_INFO_clear(vsi_info);

  ret = soc_ppd_vsi_info_get(
            unit,
            vsi,
            vsi_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  { 
    LOG_ERROR(BSL_LS_BCM_L3,
              (BSL_META_U(unit,
                          "soc_sand error %x\n"), ret));
    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_vsi_info_get failed")));
  }

  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}

/* Function _ppd_xxxx */
int
_bcm_ppd_vsi_info_set(
    int unit,
    bcm_vlan_t vsi,
    int enable,
    bcm_l3_intf_t *intf)
{
    uint32 ret;
    SOC_PPC_VSI_INFO vsi_info;
    bcm_dpp_vsi_egress_profile_t vsi_egress_profile_data,
                                 vsi_egress_profile_data_old;
    bcm_dpp_vsi_ingress_profile_t vsi_ingress_profile_data;
    int old_profile, is_last, ingress_profile, egress_profile, is_allocated;
#ifdef BCM_ARAD_SUPPORT
    SOC_PPC_EG_ENCAP_ENTRY_INFO encap_entry_info[1];
    uint32 nof_entries, next_eep[1];
#endif

    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);

    SOC_PPC_VSI_INFO_clear(&vsi_info);
    sal_memset(&vsi_egress_profile_data,0x0, sizeof(vsi_egress_profile_data));
    sal_memset(&vsi_egress_profile_data_old,0x0, sizeof(vsi_egress_profile_data_old));
    sal_memset(&vsi_ingress_profile_data,0x0, sizeof(vsi_ingress_profile_data));

    ret = soc_ppd_vsi_info_get(unit, vsi, &vsi_info);
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) {
        LOG_ERROR(BSL_LS_BCM_L3, (BSL_META_U(unit,
                                  "soc_sand error %x\n"), ret));
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                                (_BSL_BCM_MSG("soc_ppd_vsi_info_get failed")));
    }

    vsi_info.enable_my_mac = enable;
    vsi_info.fid_profile_id = SOC_PPC_VSI_FID_IS_VSID;


    /* Usually the driver configures the MTU values according to a profile that it allocates
     * automatically without the user's knowlendge. When custom_feature_mtu_advanced_mode is
     * used, then it's the user's responsibility to configure the MTU values per profile.
     */
    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE,
                                    "mtu_advanced_mode", 0)) {
        uint32 res;
        uint32 mtu_profile = 0, mtu_forwarding_profile = 0;
        uint32 profile_id, mtu_fwd_value = 0, mtu_llr_value = 0;

        if(intf->l3a_mtu_forwarding != 0 && intf->l3a_mtu != 0)
        {
            /* Look for valid MTU profiles 1/2/3 */
            for (profile_id = 1; profile_id <= 3; profile_id++) {
                mtu_fwd_value = 0;
                mtu_llr_value = 0;
                if (intf->l3a_mtu_forwarding != 0) {
                    res = arad_pp_vsi_egress_mtu_get_unsafe(unit, TRUE, profile_id,
                                                            &mtu_fwd_value);
                    if (soc_sand_get_error_code_from_error_word(res) !=
                        SOC_SAND_OK) { 
                        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG(
                                    "arad_pp_vsi_egress_mtu_get_unsafe failed")));
                    }
                }
                if (intf->l3a_mtu != 0) {
                    res = arad_pp_vsi_egress_mtu_get_unsafe(unit, FALSE, profile_id,
                                                            &mtu_llr_value);
                    if (soc_sand_get_error_code_from_error_word(res) !=
                        SOC_SAND_OK) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG(
                                    "arad_pp_vsi_egress_mtu_get_unsafe failed")));
                    }
                }
                /* Get the first mtu fwd profile.*/
                if ((mtu_forwarding_profile == 0) &&
                    (intf->l3a_mtu_forwarding != 0)) {
                    mtu_forwarding_profile =
                        (intf->l3a_mtu_forwarding == mtu_fwd_value)? profile_id : 0;
                }

                /* Get the first mtu link layer profile.*/
                if ((mtu_profile == 0) && (intf->l3a_mtu != 0)) {
                    mtu_profile = (intf->l3a_mtu == mtu_llr_value)? profile_id : 0;
                }

                /* Get the availabe mtu profile.*/
                if ((intf->l3a_mtu_forwarding == mtu_fwd_value) &&
                    (intf->l3a_mtu == mtu_llr_value)) {
                    mtu_forwarding_profile = profile_id;
                    mtu_profile = profile_id;
                    break;
                }
            }
        }

        /* If the Forwarding Layer MTU value is 0, then is it presumed to
         * use the port's configured MTU (if enabled) or none at all
         * (if disabled)
         */
        if ((intf->l3a_mtu_forwarding != 0) && (mtu_forwarding_profile == 0)) {
            LOG_ERROR(BSL_LS_BCM_L3, (BSL_META_U(unit,
                    "MTU profile does not exist for l3a_mtu_forwarding = %d\n"),
                    intf->l3a_mtu_forwarding));
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                    (_BSL_BCM_MSG("soc_ppd_vsi_info_set failed")));
        }

        /* If the Link Layer MTU value is 0, then is it presumed to use the
         * port's configured MTU (if enabled) or none at all (if disabled)
         */
        if ((intf->l3a_mtu != 0) && (mtu_profile == 0)) {
            LOG_ERROR(BSL_LS_BCM_L3, (BSL_META_U(unit,
                    "MTU profile does not exist for l3a_mtu=%d\n"),
                    intf->l3a_mtu));
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                    (_BSL_BCM_MSG("soc_ppd_vsi_info_set failed")));
        }

        /* MTU profiles for Forwarding layer and Link Layer must be the same */
        if ((intf->l3a_mtu_forwarding != 0) &&
            (intf->l3a_mtu != 0) &&
            (mtu_forwarding_profile != mtu_profile)) {
            LOG_ERROR(BSL_LS_BCM_L3, (BSL_META_U(unit,
                        "MTU profile of Forwarding Layer (%d) does not "
                        "match Link Layer (%d)\n"),
                        mtu_forwarding_profile,
                        mtu_profile));
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                        (_BSL_BCM_MSG("soc_ppd_vsi_info_set failed")));
        }

        /* Limitation does not exist in Jericho and above */
        if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
            if ((vsi >= 1) && (vsi <= 3) &&
                (((intf->l3a_mtu != 0) && (vsi != mtu_profile)) ||
                 ((intf->l3a_mtu_forwarding != 0) &&
                 (vsi != mtu_forwarding_profile)))) {
                LOG_ERROR(BSL_LS_BCM_L3, (BSL_META_U(unit,
                        "MTU profile (%d) is invalid. For VSI %d "
                        "MTU profile must be also be equal to VSI\n"),
                        mtu_profile, vsi));
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                        (_BSL_BCM_MSG("soc_ppd_vsi_info_set failed")));
            }
        }

        /* The VSI profile is set according to Forwarding/Link Layer MTU profile */
        vsi_info.profile_egress =
            (intf->l3a_mtu != 0) ? mtu_profile : mtu_forwarding_profile;
    } else {
        ret = _bcm_dpp_am_template_vsi_egress_profile_data_get(unit, vsi,
                                                        &vsi_egress_profile_data);
        BCMDNX_IF_ERR_EXIT(ret);
        if ((vsi_egress_profile_data.mtu != intf->l3a_mtu) || !enable) {
             /* getting vsi profile using alloc mngr */
            ret = _bcm_dpp_am_template_vsi_ingress_profile_data_get(unit, vsi,
                                                            &vsi_ingress_profile_data);
            BCMDNX_IF_ERR_EXIT(ret);
            ret = _bcm_dpp_am_template_vsi_ingress_profile_mapping_get(unit, vsi,
                                                            &ingress_profile);
            BCMDNX_IF_ERR_EXIT(ret);

            vsi_egress_profile_data.mtu = enable ? intf->l3a_mtu : 0;
            ret = _bcm_dpp_am_template_vsi_egress_profile_exchange(unit, 0/*flags*/, vsi, &vsi_egress_profile_data, &old_profile, 
                                                         &is_last, &egress_profile, &is_allocated);
            BCMDNX_IF_ERR_EXIT(ret);
            if (is_allocated) {
                ret = _bcm_dpp_l2_vsi_profile_update(unit, egress_profile, _BCM_L2_MC_RESERVED_EGRESS_PROFILE_TYPE_VSI,
                                                     ingress_profile, vsi_egress_profile_data, vsi_ingress_profile_data);
                BCMDNX_IF_ERR_EXIT(ret);
            }
            if (is_last && (old_profile != egress_profile)) {
                vsi_egress_profile_data_old.eve_da_lsbs_bitmap = vsi_egress_profile_data.eve_da_lsbs_bitmap;
                vsi_egress_profile_data_old.mtu = 0;
                ret = _bcm_dpp_l2_vsi_profile_update(unit, old_profile, _BCM_L2_MC_RESERVED_EGRESS_PROFILE_TYPE_VSI,
                                                     ingress_profile, vsi_egress_profile_data_old, vsi_ingress_profile_data);
                BCMDNX_IF_ERR_EXIT(ret);
            }
            vsi_info.profile_egress = egress_profile;
         }
    }

  ret = soc_ppd_vsi_info_set(
            unit,
            vsi,
            &vsi_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  { 
    LOG_ERROR(BSL_LS_BCM_L3,
              (BSL_META_U(unit,
                          "soc_sand error %x\n"), ret));
    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_vsi_info_set failed")));
  }

  /* egress RIF info */

  if(!(intf->l3a_flags & BCM_L3_TRILL_ONLY) && !((soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mim_spb_enable", 0)) && _BCM_DPP_VLAN_IS_BVID(unit, vsi))) {
      SOC_PPC_EG_ENCAP_ENTRY_INFO_clear(encap_entry_info);

      ret = soc_ppd_eg_encap_entry_get(unit, SOC_PPC_EG_ENCAP_EEP_TYPE_VSI, vsi, 0, encap_entry_info, next_eep, &nof_entries);
      if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
      {
        LOG_ERROR(BSL_LS_BCM_L3,
                  (BSL_META_U(unit,
                              "soc_sand error %x\n"), ret));
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_eg_encap_entry_get failed")));
      }

      encap_entry_info[0].entry_val.vsi_info.out_vsi = intf->l3a_vid;
      if (enable)
      {
          /* Create a L3 interface without BCM_L3_TRILL_ONLY */
          encap_entry_info[0].entry_val.vsi_info.remark_profile = intf->dscp_qos.qos_map_id;
          ret = soc_ppd_eg_encap_vsi_entry_add(
                    unit,
                    vsi,
                    &(encap_entry_info[0].entry_val.vsi_info),
                    0, /* eep is not valid as taken from egress object*/
                    0
                );
          if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
          { 
            LOG_ERROR(BSL_LS_BCM_L3,
                      (BSL_META_U(unit,
                                  "soc_sand error %x\n"), ret));
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_eg_encap_vsi_entry_add failed to create regular L3 interface")));
          }
      }
      else if (!enable && encap_entry_info[0].entry_val.vsi_info.outrif_profile_index && nof_entries)
      {
          /* Delete a L3 interface without BCM_L3_TRILL_ONLY */
          encap_entry_info[0].entry_val.vsi_info.remark_profile = 0;
          encap_entry_info[0].entry_val.vsi_info.oam_lif_set    = 0;
          encap_entry_info[0].entry_val.vsi_info.drop           = 0;
          encap_entry_info[0].entry_val.vsi_info.outlif_profile = 0xFF;
          ret = soc_ppd_eg_encap_vsi_entry_add(
                    unit,
                    vsi,
                    &(encap_entry_info[0].entry_val.vsi_info),
                    0, /* eep is not valid as taken from egress object*/
                    0
                );
          if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
          { 
            LOG_ERROR(BSL_LS_BCM_L3,
                      (BSL_META_U(unit,
                                  "soc_sand error %x\n"), ret));
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_eg_encap_vsi_entry_add failed to delete regular L3 interface")));
          }

      }

  }

  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}

/* Function _ppd_xxxx */
int
  _bcm_ppd_rif_vsid_map_get(
    int unit,
    bcm_l3_intf_t *intf
  )
{
  int 
    rv;
  uint32
    ret;
  
  SOC_PPC_RIF_INFO
    rif_info;
  bcm_vlan_t vsi;
  SOC_SAND_PP_IP_TTL 
    ttl_val;

  BCMDNX_INIT_FUNC_DEFS;

  SOC_PPC_RIF_INFO_clear(&rif_info);
  vsi = intf->l3a_vid;

  /* get and update rif attributes */
  ret = soc_ppd_rif_vsid_map_get(
            unit,
            vsi,
            &rif_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  { 
    LOG_ERROR(BSL_LS_BCM_L3,
              (BSL_META_U(unit,
                          "soc_sand error %x\n"), ret));
    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_rif_vsid_map_get failed")));
  }

  if (rif_info.uc_rpf_enable)
    intf->l3a_flags |= BCM_L3_RPF;
  
  intf->l3a_vrf = rif_info.vrf_id;

  if (rif_info.ttl_scope_index) {
      /* get TTL threshold */
      rv = _bcm_dpp_am_template_ttl_scope_index_ttl_mapping_get(unit, rif_info.ttl_scope_index, &ttl_val);
      BCMDNX_IF_ERR_EXIT(rv);

      intf->l3a_ttl = ttl_val;
  }

  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}

/* Function _ppd_xxxx */
int
  _bcm_ppd_rif_vsid_map_set(
    int                       unit,
    bcm_l3_intf_t             *intf,
    rif_vsid_map_set_action_t action
  )
{
  uint32
    ret;
  int is_last_rout_enabler = 0; 
  SOC_PPC_RIF_INFO
    rif_info;
  bcm_vlan_t vsi;
  SOC_SAND_PP_IP_TTL 
    ttl;
  int 
    rv,
    ttl_scope_index, 
    is_last,
    new_ttl_scope_index,
    is_allocated;
  uint32 
    ttl_ndx;

  BCMDNX_INIT_FUNC_DEFS;

  SOC_PPC_RIF_INFO_clear(&rif_info);
  vsi = intf->l3a_vid;

  /* get and update rif attributes */
  ret = soc_ppd_rif_vsid_map_get(
        unit,
        vsi,
        &rif_info
    );

  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  { 
      LOG_ERROR(BSL_LS_BCM_L3,
                (BSL_META_U(unit,
                            "soc_sand error %x\n"), ret));
      BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_rif_vsid_map_get failed")));
  }

  if ((action == RIF_VSID_MAP_SET_ACTION_CREATE) || (action == RIF_VSID_MAP_SET_ACTION_UPDATE)) {

      if (SOC_IS_JERICHO(unit) && (action == RIF_VSID_MAP_SET_ACTION_UPDATE)) {
          rv = _bcm_dpp_am_template_l3_rif_mac_termination_combination_profile_get(unit, rif_info.routing_enablers_bm, &(rif_info.routing_enablers_bm_id));
          BCMDNX_IF_ERR_EXIT(rv);
          rv = _bcm_dpp_am_template_l3_rif_mac_termination_combination_free(unit, rif_info.routing_enablers_bm_id, &is_last_rout_enabler);
          BCMDNX_IF_ERR_EXIT(rv);
      }

      if (action == RIF_VSID_MAP_SET_ACTION_CREATE) {
          rif_info.routing_enablers_bm |= SOC_PPC_RIF_ROUTE_ENABLE_TYPE_IP_UC;
          rif_info.routing_enablers_bm |= SOC_PPC_RIF_ROUTE_ENABLE_TYPE_IP_MC;
          rif_info.routing_enablers_bm |= SOC_PPC_RIF_ROUTE_ENABLE_TYPE_MPLS;
      }
      rif_info.uc_rpf_enable = (intf->l3a_flags & BCM_L3_RPF) ? 1 : 0;
      rif_info.vrf_id = intf->l3a_vrf;
      if (SOC_IS_JERICHO(unit)) {
          /*When new RIF is created, firstly check enablers_bm is existing in TM(template manage) or not. If no, 
                   check if enablers_bm+URPF is existing in TM(template manage) or not. If yes, using it directly. Otherwise, alloc a new template*/
          uint32 routing_enablers_bm_tmp;
          rv = _bcm_dpp_am_template_l3_rif_mac_termination_combination_profile_get(unit, rif_info.routing_enablers_bm, &(rif_info.routing_enablers_bm_id));
          if (rv != BCM_E_NONE) {
              routing_enablers_bm_tmp = rif_info.routing_enablers_bm | _BCM_L3_INGRESS_URPF_IS_LOOSE;
              rv = _bcm_dpp_am_template_l3_rif_mac_termination_combination_profile_get(unit, routing_enablers_bm_tmp, &(rif_info.routing_enablers_bm_id));
              if (rv == BCM_E_NONE) {/*loose exists*/
                  rif_info.routing_enablers_bm = routing_enablers_bm_tmp; 
              }else {
                  routing_enablers_bm_tmp = rif_info.routing_enablers_bm | _BCM_L3_INGRESS_URPF_IS_STRICT;
                  rv = _bcm_dpp_am_template_l3_rif_mac_termination_combination_profile_get(unit, routing_enablers_bm_tmp, &(rif_info.routing_enablers_bm_id));
                  if (rv == BCM_E_NONE) {/*strict exists*/
                      rif_info.routing_enablers_bm = routing_enablers_bm_tmp;
                  } 
              }  
          }
          
          rv = _bcm_dpp_am_template_l3_rif_mac_termination_combination_alloc(unit, 0, &rif_info.routing_enablers_bm, &rif_info.routing_enablers_bm_id, &rif_info.is_configure_enabler_needed);
          BCMDNX_IF_ERR_EXIT(rv);
      }
  } else {
      if (SOC_IS_JERICHO(unit)) {
          SOC_PPC_RIF_INFO rif_info_to_delete;
          SOC_PPC_RIF_INFO_clear(&rif_info_to_delete);
      
          ret = soc_ppd_rif_vsid_map_get(unit,vsi,&rif_info_to_delete);
          if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) {
              LOG_ERROR(BSL_LS_BCM_L3,
                        (BSL_META_U(unit,
                                    "soc_sand error %x\n"), ret));
              BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("failed to get rif info")));
          }
          rv = _bcm_dpp_am_template_l3_rif_mac_termination_combination_profile_get(unit, rif_info_to_delete.routing_enablers_bm, &(rif_info_to_delete.routing_enablers_bm_id));
          BCMDNX_IF_ERR_EXIT(rv);
          rv = _bcm_dpp_am_template_l3_rif_mac_termination_combination_free(unit, rif_info_to_delete.routing_enablers_bm_id, &is_last_rout_enabler);
          BCMDNX_IF_ERR_EXIT(rv);
          /*
           * Clear all the RIF-Profile information to set the profile of the deleted IN-RIF to the default value of 0
           */
          rif_info.uc_rpf_enable               = 0;
          rif_info.enable_default_routing      = 0;
          rif_info.custom_rif_bit              = 0;
          rif_info.intf_class                  = 0;
          rif_info.routing_enablers_bm_id      = 0;
          rif_info.is_configure_enabler_needed = 0;
      } else {
          /*
           * Clear all the RIF-Profile information to set the profile of the deleted IN-RIF to the default value as 0
           */
          rif_info.routing_enablers_bm         = 0;
          rif_info.enable_default_routing      = 0;
          rif_info.uc_rpf_enable               = 0;
          rif_info.vrf_id                      = 0;
      }

      intf->l3a_ttl = 0;
  }

  /* get the TTL threshold pointed by this TTL scope index */
  ttl_scope_index = rif_info.ttl_scope_index;
    
  /* search for a TTL scope for the TTL threshold */
  ttl = intf->l3a_ttl;
  rv = _bcm_dpp_am_template_ttl_scope_exchange(unit, ttl_scope_index, &ttl, &is_last, &new_ttl_scope_index, &is_allocated);
  BCMDNX_IF_ERR_EXIT(rv);

  if (is_allocated) { /* new TTL threshold */

      /* write the new threshold */
      ttl_ndx = new_ttl_scope_index;
      ret = soc_ppd_rif_ttl_scope_set(unit, ttl_ndx, ttl);
      if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) { 
          LOG_ERROR(BSL_LS_BCM_L3,
                    (BSL_META_U(unit,
                                "soc_sand error %x\n"), ret));
          BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("failed to set TTL scope")));
      }
  }

  if (is_last && (new_ttl_scope_index != ttl_scope_index)) { /* last TTL threshold */

      /* Delete the old threshold */
      ttl_ndx = ttl_scope_index;
      ret = soc_ppd_rif_ttl_scope_set(unit, ttl_ndx, ttl);
      if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) { 
          LOG_ERROR(BSL_LS_BCM_L3,
                    (BSL_META_U(unit,
                                "soc_sand error %x\n"), ret));
          BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("failed to set TTL scope")));
      }
  }
  /* update rif_info with new index */
  rif_info.ttl_scope_index = new_ttl_scope_index;

  ret = soc_ppd_rif_vsid_map_set(
            unit,
            vsi,
            &rif_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  { 
    LOG_ERROR(BSL_LS_BCM_L3,
              (BSL_META_U(unit,
                          "soc_sand error %x\n"), ret));
    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_rif_vsid_map_set failed")));
  }
  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}



/* Function _ppd_xxxx */
int
  _bcm_ppd_mpls_rif_vsid_map_set(
    int unit,
    bcm_vlan_t vsi
  )
{
  uint32
    ret;
  
  SOC_PPC_RIF_INFO
    rif_info;

  BCMDNX_INIT_FUNC_DEFS;

  SOC_PPC_RIF_INFO_clear(&rif_info);

  /* get and update rif attributes */
  ret = soc_ppd_rif_vsid_map_get(
            unit,
            vsi,
            &rif_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  { 
    LOG_ERROR(BSL_LS_BCM_L3,
              (BSL_META_U(unit,
                          "soc_sand error %x\n"), ret));
    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_rif_vsid_map_get failed")));
  }

  rif_info.routing_enablers_bm |= SOC_PPC_RIF_ROUTE_ENABLE_TYPE_MPLS;

  ret = soc_ppd_rif_vsid_map_set(
            unit,
            vsi,
            &rif_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  { 
    LOG_ERROR(BSL_LS_BCM_L3,
              (BSL_META_U(unit,
                          "soc_sand error %x\n"), ret));
    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_rif_vsid_map_set failed")));
  }
  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}

/* handling intf --> eep mapping,
   used to identify if interface definded over tunnel and what is the EEP
   REMARKS:
    - used array, if intf-id can be any value, use hash
    - if no tunneling then eep is set to zero (as not used) can be 'improved' to set ll-eep if needed
 */
int _bcm_tunnel_intf_eep_get(
    int unit,
    bcm_if_t intf,
    int *eep)
{
    BCMDNX_INIT_FUNC_DEFS;

    if (intf - _BCM_PETRA_L3_ITF_ID_BASE >= _BCM_PETRA_L3_NOF_ITFS) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Invalid Interface ID")));  /* interface id out of range */
    }
    if (intf < _BCM_PETRA_L3_ITF_ID_BASE) {
        *eep = 0;
    } else  {
	    BCMDNX_IF_ERR_EXIT(L3_ACCESS.dpp_l3_state.bcm_tunnel_intf_to_eep.get(unit, intf - _BCM_PETRA_L3_ITF_ID_BASE, eep));
    }
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_tunnel_intf_eep_set(
    int unit,
    bcm_if_t intf,
    int eep)
{
    BCMDNX_INIT_FUNC_DEFS;
    if (intf - _BCM_PETRA_L3_ITF_ID_BASE >= _BCM_PETRA_L3_NOF_ITFS) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("interface id out of range")));
    }

	BCMDNX_IF_ERR_EXIT(L3_ACCESS.dpp_l3_state.bcm_tunnel_intf_to_eep.set(unit, intf - _BCM_PETRA_L3_ITF_ID_BASE, eep));

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;

}

/* check if the given interface is actually MPLS or IPV4 tunnel*/
/* intf can be Out-RIF / FEC / Tunnel */
int _bcm_tunnel_intf_is_tunnel(
    int unit,
    bcm_if_t intf,
    int *tunneled,
    int *eep,
    int *vsi,
    int *ll_eep)
{
    bcm_l3_intf_t l3_if_t;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    int itf_val;
    int rv = BCM_E_NONE;
    SOC_PPC_EG_ENCAP_ENTRY_INFO *encap_entry_info = NULL;
    uint32 next_eep[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX], nof_entries;
    SOC_PPC_EG_ENCAP_ENTRY_TYPE entry_type;
    int indx;
    int local_lif=0, global_lif=0;
    _bcm_lif_type_e lif_usage = _bcmDppLifTypeAny;

    BCMDNX_INIT_FUNC_DEFS;

    soc_sand_dev_id = (unit);

    itf_val = BCM_L3_ITF_VAL_GET(intf);

    /* rif */
    if(BCM_L3_ITF_TYPE_IS_RIF(intf)) {
        rv = _bcm_tunnel_intf_eep_get(unit, itf_val, eep);
        BCMDNX_IF_ERR_EXIT(rv);
        bcm_l3_intf_t_init(&l3_if_t);
        l3_if_t.l3a_intf_id = itf_val;
        rv = bcm_petra_l3_intf_get(unit, &l3_if_t);
        BCMDNX_IF_ERR_EXIT(rv);
        *vsi = l3_if_t.l3a_vid;
    }
    else if(BCM_L3_ITF_TYPE_IS_LIF(intf)) { /* OutLIF */
        *eep = itf_val;
        *vsi = 0;
    }
    else { /* FEC */
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("No support for interface = FEC.")));
    }

    if (SOC_IS_JERICHO(unit))  {
        global_lif = *eep;
        rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, global_lif, &local_lif);
        BCMDNX_IF_ERR_EXIT(rv);

        if (SOC_DPP_CONFIG(unit)->pp.l2_src_encap_enable == 1) {
            rv = _bcm_dpp_local_lif_sw_resources_lif_usage_get(unit, -1, local_lif, NULL, &lif_usage);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    } else {
        local_lif = global_lif = *eep;
    }

    soc_sand_rv = soc_ppd_eg_encap_entry_type_get(soc_sand_dev_id,
                                                  local_lif,
                                                  &entry_type);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if (lif_usage == _bcmDppLifTypeL2SrcEncap) {
        *tunneled = 1;
        BCM_EXIT;
    }

    if (_BCM_PPD_EG_ENCAP_IS_TUNNEL_TYPE(unit, entry_type)) { /* Entry is PWE, MPLS types,IP tunnel, or VSI */
        *tunneled = 1;
        *ll_eep = 0;
    } else if (entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_DATA) { /* DATA */
        BCMDNX_ALLOC(encap_entry_info, sizeof(SOC_PPC_EG_ENCAP_ENTRY_INFO) * SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX, "_bcm_tunnel_intf_is_tunnel.encap_entry_info");
        if (encap_entry_info == NULL) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
        }
        soc_sand_rv =
           soc_ppd_eg_encap_entry_get(soc_sand_dev_id,
                                      SOC_PPC_EG_ENCAP_EEP_TYPE_TUNNEL_EEP, local_lif, 2,
                                      encap_entry_info, next_eep, &nof_entries);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        *tunneled = 0;
        *ll_eep = 0;
    } else if (entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_LL_ENCAP) { /* Link layer */
        *tunneled = 0;
        *ll_eep = *eep;
        *eep = 0;
    } else { /* NULL / Data / Not allocated */
        *tunneled = 0;
        *ll_eep = 0;
    }

    /* get ll_eep and VSI for tunneled EEP */
    if(*tunneled) {
        if (encap_entry_info == NULL) {
            BCMDNX_ALLOC(encap_entry_info, sizeof(SOC_PPC_EG_ENCAP_ENTRY_INFO)*SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX, "_bcm_tunnel_intf_is_tunnel.encap_entry_info");
            if (encap_entry_info == NULL) {        
                BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
            }
            soc_sand_rv =
                soc_ppd_eg_encap_entry_get(soc_sand_dev_id,
                                       SOC_PPC_EG_ENCAP_EEP_TYPE_TUNNEL_EEP, local_lif, 2,
                                       encap_entry_info, next_eep, &nof_entries);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }

        for (indx = 0; indx < nof_entries; ++indx) {
            if (encap_entry_info[indx].entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_MPLS_ENCAP) {
                if(encap_entry_info[indx].entry_val.mpls_encap_info.nof_tunnels > 0) {
                    *vsi = encap_entry_info[indx].entry_val.mpls_encap_info.out_vsi;
                    *ll_eep = next_eep[indx];
                }
            }
            else if (encap_entry_info[indx].entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_IPV4_ENCAP) {
                *vsi = encap_entry_info[indx].entry_val.ipv4_encap_info.out_vsi;
                *ll_eep = next_eep[indx];
            }
            
            else if (encap_entry_info[indx].entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_POP_CMND) {
                *vsi = encap_entry_info[indx].entry_val.pop_info.out_vsi;
                *ll_eep = next_eep[indx];
            }
            
            else if (encap_entry_info[indx].entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_PWE) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("not expected pwe type. shoud be mpls encap")));
            }
            else if (encap_entry_info[indx].entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_SWAP_CMND) {
                *vsi = encap_entry_info[indx].entry_val.swap_info.out_vsi;
                *ll_eep = next_eep[indx];
            }
            else if (encap_entry_info[indx].entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_VSI) {
                *vsi = encap_entry_info[indx].entry_val.vsi_info.out_vsi;
                *ll_eep = next_eep[indx];
            }
        }
    }

exit:
    BCM_FREE(encap_entry_info);
    BCMDNX_FUNC_RETURN;
}


/* get eedb index from router/tunnel intf */
int _bcm_l3_intf_eep_get(
    int unit, 
    bcm_if_t intf, 
    uint8 *tunneled, 
    int *eep) {

    bcm_l3_intf_t l3_intf;
    
    BCMDNX_INIT_FUNC_DEFS;

    bcm_l3_intf_t_init(&l3_intf);

    if (intf > 0) {
        if (BCM_L3_ITF_TYPE_IS_RIF(intf)) {

            l3_intf.l3a_intf_id = intf;

            bcm_petra_l3_intf_get(unit, &l3_intf) ;
            /* Don't check return value because l3 interface can be allocated on remote device */

            *eep = l3_intf.l3a_tunnel_idx;
            *tunneled = FALSE;
            
        } else if (BCM_L3_ITF_TYPE_IS_LIF(intf)) {
            *eep = BCM_L3_ITF_VAL_GET(intf);
            *tunneled = TRUE;
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Intf given is not valid 0x%x"), intf));
        }
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/* Function _ppd_xxxx */
int
  _bcm_ppd_l3_intf_is_tunneled(
    int unit,
    bcm_l3_intf_t *intf,
    int *is_tunnel,
    int *tunnel_idx
  )
{
  int rv;
  int eep;
  uint32 next_eep, nof_entries;
  int itf_val;
  uint32 soc_sand_rv;
  unsigned int soc_sand_dev_id = 0;
  SOC_PPC_EG_ENCAP_ENTRY_INFO *encap_entry_info = NULL;
  int global_lif, local_lif;

  BCMDNX_INIT_FUNC_DEFS;

  soc_sand_dev_id = (unit);

  *is_tunnel = 0;
  *tunnel_idx = 0;

  itf_val = BCM_L3_ITF_VAL_GET(intf->l3a_intf_id);

  rv = _bcm_tunnel_intf_eep_get(unit, itf_val, &eep);
  BCMDNX_IF_ERR_EXIT(rv);

  /* eep zero is NULL, i.e. no encapsulation*/
  if(eep == 0) {
      BCM_EXIT;
  }

  if (SOC_IS_JERICHO(unit)) 
  {
      global_lif = eep;
      rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, global_lif, &local_lif);
      BCMDNX_IF_ERR_EXIT(rv);
  }
  else
  {
      local_lif = global_lif = eep;
  }

  /* ARAD */
  BCMDNX_ALLOC(encap_entry_info, sizeof(SOC_PPC_EG_ENCAP_ENTRY_INFO) * SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX, "_bcm_ppd_l3_intf_is_tunneled.encap_entry_info");
  if (encap_entry_info == NULL) {
      BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
  }
  soc_sand_rv =
     soc_ppd_eg_encap_entry_get(soc_sand_dev_id,
                                SOC_PPC_EG_ENCAP_EEP_TYPE_TUNNEL_EEP,
                                local_lif, 1,
                                encap_entry_info, &next_eep,
                                &nof_entries);
  BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

  if (_BCM_PPD_EG_ENCAP_IS_TUNNEL_TYPE(unit, encap_entry_info[0].entry_type)) {
      *is_tunnel = 1;
      *tunnel_idx = eep;
  }

exit:
  BCM_FREE(encap_entry_info);
  BCMDNX_FUNC_RETURN;
}

/* Function _ppd_xxxx */
int
  _bcm_ppd_l2_lif_ac_remove(
    int unit,
    int ac_index,
    bcm_l3_intf_t *intf
  )
{
  uint32
    ret;
  
  SOC_PPC_L2_LIF_AC_KEY
    ac_key;
  SOC_PPC_LIF_ID
    lif_index;

  BCMDNX_INIT_FUNC_DEFS;

  SOC_PPC_L2_LIF_AC_KEY_clear(&ac_key);
  ac_key.vlan_domain = DPP_COMMON_VLAN_DOMAIN;
  ac_key.outer_vid = intf->l3a_vid;
  ac_key.inner_vid = SOC_PPC_LIF_IGNORE_INNER_VID;

  ret = soc_ppd_l2_lif_ac_remove(
            unit,
            &ac_key,
            &lif_index
          );

  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  { 
    LOG_ERROR(BSL_LS_BCM_L3,
              (BSL_META_U(unit,
                          "soc_sand error %x\n"), ret));
    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_l2_lif_ac_remove failed")));
  }
  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}


/***************************************************************/
/***************************************************************/

/* 
 * FEC Helper functions
 */
  

/* 
 * Retreive from FEC information application specific information: 
 * Decision is done according to FEC type 
 * Fill fields: 
 * intf, 
 * encap_id, 
 * flags 
 */
int
_bcm_ppd_fec_entry_to_application_info(int unit,
                                       SOC_PPC_FRWRD_FEC_ENTRY_INFO *fec_entry,
                                       bcm_l3_egress_t *egr)
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    egr->intf = 0;
    egr->encap_id = 0;

    switch(fec_entry->type) {
        case SOC_PPC_FEC_TYPE_IP_UC: /* FEC is used for HOST */
            if (!BCM_VLAN_VALID(fec_entry->eep)) { /* EEP must be RIF index */
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                    (_BSL_BCM_MSG("Internal error: in case fec type is IP_UC "
                                  "eep should represent a valid out_rif ")));
            }
            BCM_L3_ITF_SET(egr->intf,BCM_L3_ITF_TYPE_RIF,fec_entry->eep);
            break;
        case SOC_PPC_FEC_TYPE_DROP:
        case SOC_PPC_FEC_TYPE_SIMPLE_DEST:
        case SOC_PPC_NOF_FEC_TYPES_ARAD:
            /* Nothing to do */
            if (fec_entry->eep != 0 || fec_entry->app_info.out_rif != 0) {/* must be null */
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                    (_BSL_BCM_MSG("Internal error: in case fec type is Simple "
                                  "eep and out_rif must be 0")));
            }

            break;
        case SOC_PPC_FEC_TYPE_IP_MC: /* FEC points to multicast destination */
            BCM_L3_ITF_SET(egr->intf,BCM_L3_ITF_TYPE_RIF,fec_entry->rpf_info.expected_in_rif);
            break;
        case SOC_PPC_FEC_TYPE_TRILL_MC:
            if (fec_entry->app_info.out_rif != 0) { /* must be null */
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                    (_BSL_BCM_MSG("Internal error: in case fec type is "
                                  "trill_mc out_rif must be 0")));
            }

            egr->flags |= BCM_L3_TRILL_ONLY;
            BCM_L3_ITF_SET(egr->encap_id,BCM_L3_ITF_TYPE_LIF,fec_entry->eep);
            break;
        case SOC_PPC_FEC_TYPE_MPLS_LSR:
        case SOC_PPC_FEC_TYPE_MPLS_LSR_EEI_OUTLIF:
        case SOC_PPC_FEC_TYPE_TRILL_UC:
            if (fec_entry->app_info.out_rif != 0) { /* must be null */
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                    (_BSL_BCM_MSG("Internal error: in case fec type is "
                                  "MPLS_LSR out_rif must be 0")));
            }
            BCM_L3_ITF_SET(egr->encap_id,BCM_L3_ITF_TYPE_LIF,fec_entry->eep);
            if (fec_entry->type == SOC_PPC_FEC_TYPE_MPLS_LSR_EEI_OUTLIF) {
                egr->flags |= BCM_L3_ENCAP_SPACE_OPTIMIZED;
            }
            if (fec_entry->type == SOC_PPC_FEC_TYPE_TRILL_UC) {
                egr->flags |= BCM_L3_TRILL_ONLY;
            }
            break;
        case SOC_PPC_FEC_TYPE_TUNNELING: /* Tunnel can be VSI-RIF or Tunnel-RIF. Decide according to range */
        case SOC_PPC_FEC_TYPE_TUNNELING_EEI_OUTLIF:
            if (fec_entry->app_info.out_rif != 0) { /* must be null */
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                    (_BSL_BCM_MSG("Internal error: in case fec type is "
                                  "Tunneling out_rif must be 0")));
            }

            if (BCM_VLAN_VALID(fec_entry->eep)) {
                BCM_L3_ITF_SET(egr->intf,BCM_L3_ITF_TYPE_RIF,fec_entry->eep);
            } else {
                SOC_PPC_EG_ENCAP_ENTRY_INFO encap_entry_info[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX];
                uint32 next_eep[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX];
                uint32 depth = SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX - 1;
                uint32 nof_entries;
                int tunnel_local_lif;
                int ll_global_lif;

                BCM_L3_ITF_SET(egr->intf, BCM_L3_ITF_TYPE_LIF, fec_entry->eep);
                /** Convert tunnel global lif to tunnel local lif */
                rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, fec_entry->eep, &tunnel_local_lif);
                if (rv == BCM_E_NONE)
                {
                    /** Fetch LL EEP from encapuslation stack */
                    arad_pp_eg_encap_entry_get_unsafe(unit, SOC_PPC_EG_ENCAP_EEP_TYPE_TUNNEL_EEP, tunnel_local_lif,
                                                            depth, encap_entry_info, next_eep, &nof_entries);
                    /** Given LL is the last encapuslation entry, and LL EEP is next_eep of the penultimate entry */
                    if (nof_entries >= 2)
                    {
                        /** Convert ll local lif to ll global lif */
                        rv = _bcm_dpp_global_lif_mapping_local_to_global_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, next_eep[nof_entries-2], &ll_global_lif);
                        BCMDNX_IF_ERR_EXIT(rv);
                        /** encap_id points to LL EEP */
                        BCM_L3_ITF_SET(egr->encap_id, BCM_L3_ITF_TYPE_LIF, ll_global_lif);
                    }
                }
            }
            if (fec_entry->type == SOC_PPC_FEC_TYPE_TUNNELING_EEI_OUTLIF) {
                egr->flags |= BCM_L3_ENCAP_SPACE_OPTIMIZED;
            }
            break;
        case SOC_PPC_FEC_TYPE_TUNNELING_EEI_MPLS_COMMAND:
            {
                int identifier = SOC_PPD_MPLS_IDENTIFIER_FROM_EEI_COMMAND_ENCODING(fec_entry->eep);
                bcm_mpls_egress_label_t label_entry;
                uint8 has_cw;

                bcm_mpls_egress_label_t_init(&label_entry);
                if (identifier < SOC_PPC_EEI_IDENTIFIER_SWAP_VAL) {
                    BCMDNX_IF_ERR_EXIT(
                        _bcm_mpls_tunnel_push_profile_info_get(unit, identifier,
                                                        &has_cw, &label_entry));
                }
                label_entry.label = SOC_PPD_MPLS_LABEL_FROM_EEI_COMMAND_ENCODING(fec_entry->eep);

                _BCM_PETRA_FILL_L3_EGR_WITH_LABEL_ENTRY_CONTENTS(egr, label_entry);
                break;
            }
        case SOC_PPC_FEC_TYPE_ROUTING:
            if (!((fec_entry->app_info.out_rif >= BCM_VLAN_DEFAULT) &&
                  (fec_entry->app_info.out_rif < SOC_DPP_CONFIG(unit)->l3.nof_rifs))) {
                  /* must be RIF index */
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                    (_BSL_BCM_MSG("Internal error: in case fec type is Routing "
                                  "rif should represent a valid out_rif. rif value is %d"),
                     fec_entry->rpf_info.expected_in_rif));
            }
            BCM_L3_ITF_SET(egr->intf,BCM_L3_ITF_TYPE_RIF,fec_entry->app_info.out_rif);
            BCM_L3_ITF_SET(egr->encap_id,BCM_L3_ITF_TYPE_LIF,fec_entry->eep);
            break;
        case SOC_PPC_FEC_TYPE_FORWARD:
            BCM_L3_ITF_SET(egr->encap_id,BCM_L3_ITF_TYPE_LIF,fec_entry->eep);
            break;
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                (_BSL_BCM_MSG("Internal error: unsupported fec_entry type %d"),
                 fec_entry->type));
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

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
   ) 
{
    SOC_PPC_FRWRD_FEC_ENTRY_INFO protect_fec;
    SOC_PPC_FRWRD_FEC_PROTECT_INFO protect_info;
    int ret = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    SOC_PPC_FRWRD_FEC_PROTECT_INFO_clear(&protect_info);

    /*If we get an odd FEC then we first do a get for the even FEC for protection info. 
      If no protection is configured, we do another get for the odd fec data.*/
    ret = soc_ppd_frwrd_fec_entry_get(
       unit,
       _BCM_L3_FEC_TO_WORK_FEC(fec_ndx),
       protect_type,
       working_fec,
       &protect_fec,
       &protect_info
       );
    BCM_SAND_IF_ERR_EXIT(ret);

    /*No protection*/
    if (*protect_type == SOC_PPC_FRWRD_FEC_PROTECT_TYPE_NONE) {
        *failover_id = 0;

        /*Odd entry requested, get it*/
        if (_BCM_L3_FEC_IS_ODD_FEC(fec_ndx)) {
            SOC_PPC_FRWRD_FEC_ENTRY_INFO_clear(working_fec);
            SOC_PPC_FRWRD_FEC_ENTRY_INFO_clear(&protect_fec);
            SOC_PPC_FRWRD_FEC_PROTECT_INFO_clear(&protect_info);

            ret = soc_ppd_frwrd_fec_entry_get(
               unit,
               fec_ndx,
               protect_type,
               working_fec,
               &protect_fec,
               &protect_info
               );
            BCM_SAND_IF_ERR_EXIT(ret);
        }
    }
    /*Protection configured*/
    else {
        if (*protect_type == SOC_PPC_FRWRD_FEC_PROTECT_TYPE_FACILITY) {
            *failover_id = protect_info.oam_instance_id;
        } else {
            DPP_FAILOVER_TYPE_SET(*failover_id, protect_info.oam_instance_id, DPP_FAILOVER_TYPE_FEC);
        }

        if (_BCM_L3_FEC_IS_ODD_FEC(fec_ndx)) {
            *working_fec = protect_fec; /*Odd fec was requested, make it the working_fec*/
        }
    }


    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


/* Function _bcm_ppd_frwrd_fec_entry_get */
int
  _bcm_ppd_frwrd_fec_entry_get(
    int unit,
    int fec_index,
    bcm_l3_egress_t *egr
  )
{
  uint32
    ret;
  
  SOC_PPC_FEC_ID
    fec_ndx = fec_index;
  SOC_PPC_FRWRD_FEC_PROTECT_TYPE
    protect_type;
  SOC_PPC_FRWRD_FEC_ENTRY_INFO
    working_fec;
  SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO
    accessed_info;
  int rv = BCM_E_NONE;
  int is_local_conf = 0;
  _bcm_dpp_gport_info_t gport_info;
  _bcm_lif_type_e usage;
  int local_lif;
  bcm_failover_t failover_id;

  BCMDNX_INIT_FUNC_DEFS;
  
  SOC_PPC_FRWRD_FEC_ENTRY_INFO_clear(&working_fec);

  ret = _bcm_ppd_frwrd_fec_entry_from_hw(unit, fec_ndx, &protect_type, &working_fec, &failover_id);
  BCM_SAND_IF_ERR_EXIT(ret);

  egr->failover_id = failover_id;

  /*Protection configured*/
  if (protect_type != SOC_PPC_FRWRD_FEC_PROTECT_TYPE_NONE) {
	  if (_BCM_L3_FEC_IS_EVEN_FEC(fec_ndx)) {
		  _bcm_l3_fec_to_intf(unit, fec_ndx + 1, &egr->failover_if_id);
	  }
  }

  rv = _bcm_dpp_gport_from_sand_pp_dest(unit, &egr->port, &working_fec.dest); 
  BCMDNX_IF_ERR_EXIT(rv);

  /* get accessed status, clear if requested */
  ret = soc_ppd_frwrd_fec_entry_accessed_info_get(
          unit,
          fec_ndx,
          (egr->flags & BCM_L3_HIT_CLEAR) ? TRUE : FALSE,
          &accessed_info
        );
  BCM_SAND_IF_ERR_EXIT(ret);

  if (accessed_info.accessed) 
  {
      egr->flags |= BCM_L3_HIT;
  }  

  /* multicast RPF information */
  if ((working_fec.rpf_info.rpf_mode & SOC_PPC_FRWRD_FEC_RPF_MODE_MC_EXPLICIT)
      || (working_fec.rpf_info.rpf_mode & SOC_PPC_FRWRD_FEC_RPF_MODE_MC_USE_SIP_WITH_ECMP)) {
      
      egr->flags |= BCM_L3_RPF;

      if (working_fec.rpf_info.rpf_mode & SOC_PPC_FRWRD_FEC_RPF_MODE_MC_USE_SIP_WITH_ECMP) {
          egr->flags |= BCM_L3_IPMC;
      }      
  }

  /* make sure any other fec created with egr, gets the same urpf mode */
  if(egr->flags2 & BCM_L3_FLAGS2_INGRESS_URPF_MODE){
	  if (working_fec.rpf_info.rpf_mode & SOC_PPC_FRWRD_FEC_RPF_MODE_UC_STRICT){
		  egr->urpf_mode = bcmL3IngressUrpfStrict;
	  } else {
		  egr->urpf_mode = bcmL3IngressUrpfLoose;
	  }
  }

  /* Convert FEC to intf, encap_id */
  rv = _bcm_ppd_fec_entry_to_application_info(unit, &working_fec, egr);
  BCMDNX_IF_ERR_EXIT(rv);

  /* Call LL information only in case Local configuration (unicast port is local) */
  if(BCM_GPORT_IS_MCAST(egr->port) || BCM_GPORT_IS_BLACK_HOLE(egr->port)) {
     is_local_conf = FALSE;
  } else {
    rv = _bcm_dpp_gport_to_phy_port(unit, egr->port, 0, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rv);

    is_local_conf = _BCM_DPP_GPORT_INFO_IS_LOCAL_PORT(gport_info);
  }

  if (is_local_conf)
  {
      if (BCM_L3_ITF_TYPE_IS_LIF(egr->encap_id)) {

          rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, BCM_L3_ITF_VAL_GET(egr->encap_id), &local_lif);
          BCMDNX_IF_ERR_EXIT(rv);
 
          BCMDNX_IF_ERR_EXIT( _bcm_dpp_local_lif_sw_resources_lif_usage_get(unit, -1, local_lif, NULL, &usage));
          if (usage == _bcmDppLifTypeLinkLayer) { /* EEDB entry is LL, fill egr with it's data */
              rv = _bcm_ppd_eg_encap_entry_get(unit, BCM_L3_ITF_VAL_GET(egr->encap_id), egr); 
              BCMDNX_IF_ERR_EXIT(rv);
          }
      }      
  }

  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}

/* Function _ppd_xxxx */
int
  _bcm_ppd_frwrd_fec_entry_add(
    int unit,
    int fec_index,
    int eep_index,
    int out_rif,
    uint8 is_tunnel, /* If TRUE, FEC should be of type Tunnel */
    uint8 is_fec_eei_outlif, /* If TRUE, FEC points to eep_index using format C with outlif encoding, else using format B*/
    uint8 is_tunnel_eei_mpls_command, /* If TRUE, FEC should be of type Tunnel and points to eep_index using format C with mpls command encoding*/
    uint32 eei_val,
    bcm_l3_egress_t *egr
  )
{

  int switch_rpf;
#ifdef BCM_ARAD_SUPPORT
  int mc;
#endif
  SOC_PPC_FEC_ID
    fec_ndx;
  SOC_PPC_FRWRD_FEC_PROTECT_TYPE
    protect_type;
  SOC_PPC_FRWRD_FEC_ENTRY_INFO
    working_fec;
  SOC_PPC_FRWRD_FEC_PROTECT_INFO
    protect_info;
  SOC_PPC_FRWRD_FEC_ENTRY_ACCESSED_INFO
    accessed_info;
  uint8
    success = SOC_SAND_SUCCESS;
  uint32
    soc_sand_rv;


  /*
  bcm_module_t    _modid;
  bcm_port_t      _mod_port;
  */
  int rv;

  BCMDNX_INIT_FUNC_DEFS;

  fec_ndx = fec_index;
  protect_type = SOC_PPC_FRWRD_FEC_PROTECT_TYPE_NONE;

  SOC_PPC_FRWRD_FEC_ENTRY_INFO_clear(&working_fec);
  SOC_PPC_FRWRD_FEC_PROTECT_INFO_clear(&protect_info);

  /* unicast RPF */
  rv = _bcm_sw_db_switch_urpf_mode_get(unit, &switch_rpf);
  BCMDNX_IF_ERR_EXIT(rv);

  switch(switch_rpf) {
      case bcmL3IngressUrpfLoose:
          working_fec.rpf_info.rpf_mode = SOC_PPC_FRWRD_FEC_RPF_MODE_UC_LOOSE;
      break;
      case bcmL3IngressUrpfStrict:
          working_fec.rpf_info.rpf_mode = SOC_PPC_FRWRD_FEC_RPF_MODE_UC_STRICT;
      break;
      case bcmL3IngressUrpfDisable:
      default:
          working_fec.rpf_info.rpf_mode = SOC_PPC_FRWRD_FEC_RPF_MODE_NONE;
      break;
  }

  /* per FEC uc RPF mode configuration*/
  if(egr->flags2 & BCM_L3_FLAGS2_INGRESS_URPF_MODE) {
	  working_fec.rpf_info.rpf_mode = (egr->urpf_mode == bcmL3IngressUrpfStrict) ? SOC_PPC_FRWRD_FEC_RPF_MODE_UC_STRICT :
	  SOC_PPC_FRWRD_FEC_RPF_MODE_UC_LOOSE;
  }

  /* multicast RPF */
  if(egr->flags & BCM_L3_RPF) {
      if(egr->flags & BCM_L3_IPMC) {
          working_fec.rpf_info.rpf_mode |= SOC_PPC_FRWRD_FEC_RPF_MODE_MC_EXPLICIT;
          working_fec.rpf_info.expected_in_rif = out_rif;
      } else {
          working_fec.rpf_info.rpf_mode |= SOC_PPC_FRWRD_FEC_RPF_MODE_MC_USE_SIP_WITH_ECMP;
      }
  }


  if (egr->flags & BCM_L3_TRILL_ONLY) {
      mc = BCM_GPORT_MCAST_GET(egr->port);
      if (mc == -1) {
          working_fec.type = SOC_PPC_FEC_TYPE_TRILL_UC;
          working_fec.eep = eep_index;
      } else {
          working_fec.type = SOC_PPC_FEC_TYPE_TRILL_MC;
          working_fec.eep = eep_index;
      }
  } else if (is_tunnel_eei_mpls_command) {
      working_fec.type = SOC_PPC_FEC_TYPE_TUNNELING_EEI_MPLS_COMMAND;
      working_fec.eep = eei_val;
  } else if ((egr->flags & BCM_L3_IPMC)) {
      working_fec.type = SOC_PPC_FEC_TYPE_IP_MC;
      working_fec.eep = 0;
      working_fec.rpf_info.expected_in_rif = out_rif;
  } else if (out_rif == 0 && eep_index == 0) {
      working_fec.type = SOC_PPC_FEC_TYPE_SIMPLE_DEST;
  } else if (eep_index == 0) { /* eep is not present then FEC points to outRIF*/
      if (is_fec_eei_outlif) { /* Is tunnel eei means FEC should point to EEP using FEC EEI FORMAT C */
          working_fec.type = SOC_PPC_FEC_TYPE_TUNNELING_EEI_OUTLIF;
      } else { /* Else FEC should point to EEP using FEC Outlif FORMAT B */
          working_fec.type = SOC_PPC_FEC_TYPE_TUNNELING;
      }
      working_fec.eep = out_rif;
  } else if (is_tunnel) { /* out-rif Is tunnel means FEC should point to EEP */
      if (is_fec_eei_outlif) { /* Is tunnel eei means FEC should point to EEP using FEC EEI FORMAT C */
          working_fec.type = SOC_PPC_FEC_TYPE_TUNNELING_EEI_OUTLIF;
      }
      else { /* Else FEC should point to EEP using FEC Outlif FORMAT B */
          working_fec.type = SOC_PPC_FEC_TYPE_TUNNELING;
      }
      working_fec.eep = eep_index;
  } else if (BCM_GPORT_IS_MCAST(egr->port)) { /* out-rif Is tunnel means FEC should points to EEP */
      working_fec.type = SOC_PPC_FEC_TYPE_IP_MC;
      working_fec.eep = 0;
      working_fec.rpf_info.expected_in_rif = out_rif;
  } else if (out_rif == 0) { /* In case intf is NULL and encap_id != 0 then we are in type of MPLS_LSR */
      if (is_fec_eei_outlif) { /* Is tunnel eei means FEC should point to EEP using FEC EEI FORMAT C */
          working_fec.type = SOC_PPC_FEC_TYPE_MPLS_LSR_EEI_OUTLIF;
      }
      else { /* Else FEC should point to EEP using FEC Outlif FORMAT B */
          if (SOC_DPP_CONFIG(unit)->pp.pon_dscp_remarking) {
              working_fec.type = SOC_PPC_FEC_TYPE_FORWARD;
          } else {
              working_fec.type = SOC_PPC_FEC_TYPE_MPLS_LSR;
          }
      }
      working_fec.eep = eep_index;
      working_fec.app_info.out_rif = 0;
  } else {
      working_fec.type = SOC_PPC_FEC_TYPE_ROUTING; /* normal routing with outlif (EEP) + outRIF*/
      working_fec.eep = eep_index;
      working_fec.app_info.out_rif = out_rif;
  }

  rv = _bcm_dpp_gport_to_sand_pp_dest(unit, egr->port, &working_fec.dest);
  BCMDNX_IF_ERR_EXIT(rv);

  if (_BCM_PPD_L3_IS_PROTECTED_IF(egr))
  {
    rv = _bcm_dpp_l2_gport_fill_fec(unit, egr, BCM_DPP_GPORT_TYPE_EGRESS_IF,fec_ndx, &working_fec);
    BCMDNX_IF_ERR_EXIT(rv);
  }
  else
  {    
    soc_sand_rv = soc_ppd_frwrd_fec_entry_add(
            unit,
            fec_ndx,
            protect_type,
            &working_fec,
            NULL,
            &protect_info,
            &success
          );
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    if(!success){
        BCMDNX_ERR_EXIT_MSG(BCM_E_FULL, (_BSL_BCM_MSG("fail adding FEC entry")));
    }
  }

  /*
   * Clear the hit indication of the new FEC.
   * In case the BCM_L3_FLAGS2_SKIP_HIT_CLEAR flag is on don't clear to enhance time performance.
   */
  if ((egr->flags2 & BCM_L3_FLAGS2_SKIP_HIT_CLEAR) == 0) {
      rv = soc_ppd_frwrd_fec_entry_accessed_info_get(
                unit,
                fec_ndx,
                1,    /* Clear on read */
                &accessed_info
                );
      BCM_SAND_IF_ERR_EXIT(rv);
  }

exit:
  BCMDNX_FUNC_RETURN;
}

/* Function _ppd_xxxx */
int
  _bcm_ppd_frwrd_fec_remove(
    int unit,
    int fec_index
  )
{
  uint32
    ret;
  
  SOC_PPC_FEC_ID
    fec_ndx;

  BCMDNX_INIT_FUNC_DEFS;

  fec_ndx = fec_index;

  ret = soc_ppd_frwrd_fec_remove(
            unit,
            fec_ndx
          );

  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  { 
    LOG_ERROR(BSL_LS_BCM_L3,
              (BSL_META_U(unit,
                          "soc_sand error %x\n"), ret));
    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_fec_remove failed")));
  }
  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}


/***************************************************************/
/***************************************************************/

/*
 * L3 Egress Helper functions
 */

/* Function _ppd_xxxx */
int
  _bcm_ppd_eg_encap_entry_add(
    int unit,
    int fec_ndx,
    int eep_index, /* eep can be LL or Tunnel */
    int ll_eep_indx,
    int *fec_eep_index, /* EEP FEC to point to */
    bcm_l3_egress_t *egr
  )
{ 
  uint32
    ret;
  
  SOC_PPC_EG_ENCAP_LL_INFO
    ll_encap_info;
  int rv;


  BCMDNX_INIT_FUNC_DEFS;

  
  SOC_PPC_EG_ENCAP_LL_INFO_clear(&ll_encap_info);

  ll_encap_info.out_vid_valid = FALSE;
  ll_encap_info.pcp_dei_valid = FALSE;

  if (egr->flags & BCM_L3_TRILL_ONLY  ) {
      ll_encap_info.out_vid_valid = TRUE;
      ll_encap_info.out_vid = egr->vlan;
      ll_encap_info.out_ac_valid = TRUE;  
      rv = _bcm_petra_trill_get_trill_out_ac(unit, &ll_encap_info.out_ac_lsb);
      BCMDNX_IF_ERR_EXIT(rv);

  }

  /* egress vlan is valid, then the LL-entry will include DA, VID.
     if vlan valid this means that the VSI/VID of the built LL encapsulation is taken
     from ARP entry.
     if not valid, then the VSI/VID is taken from higher level: tunnel entry, or for IPuc routing
     it's taken seperately from FEC.
     for routing and tunneling: this should be reset.
     for LSR, IPv6 tunneling: should be set.
   */
  if(egr->vlan != 0) {
      ll_encap_info.out_vid_valid = TRUE;
      ll_encap_info.out_vid = egr->vlan;
  }

  ret = _bcm_petra_mac_to_sand_mac(
      egr->mac_addr,
      &ll_encap_info.dest_mac);

  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  { 
    LOG_ERROR(BSL_LS_BCM_L3,
              (BSL_META_U(unit,
                          "soc_sand error %x\n"), ret));
    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("_bcm_petra_mac_to_sand_mac failed")));
  }

  /* 
   * QOS infomration - remark profile
   */
  ll_encap_info.ll_remark_profile = BCM_QOS_MAP_PROFILE_GET(egr->qos_map_id);

  /*
   * QUF6/JP6: add new native-LL indication. (QUX, Jericho+ and QAX B0)
   */
  if (SOC_IS_JERICHO_PLUS(unit) && !SOC_IS_QAX_A0(unit)) {
      ll_encap_info.native_ll = ((egr->flags & BCM_L3_NATIVE_ENCAP) ? 1 : 0);
  }

  /* write LL entry */
  ret = soc_ppd_eg_encap_ll_entry_add(
          unit,
          ll_eep_indx,
          &ll_encap_info
        );

  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  { 
    LOG_ERROR(BSL_LS_BCM_L3,
              (BSL_META_U(unit,
                          "soc_sand error %x\n"), ret));
    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_eg_encap_ll_entry_add failed")));
  }

  /* Add to SW DB */
  rv = _bcm_dpp_out_lif_ll_match_add(unit, ll_eep_indx);
  BCMDNX_IF_ERR_EXIT(rv);

  /* The fec should get the global lif, not the local lif */
  rv = _bcm_dpp_global_lif_mapping_local_to_global_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, ll_eep_indx, fec_eep_index);
  BCMDNX_IF_ERR_EXIT(rv);

  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}

/* Function _ppd_xxxx */
int
  _bcm_ppd_eg_encap_entry_remove(
    int unit,
    int eep_index
  )
{
  uint32
    ret;
  _bcm_lif_type_e usage;

  BCMDNX_INIT_FUNC_DEFS;

  /* Verify LIF is LL */
  BCMDNX_IF_ERR_EXIT( _bcm_dpp_local_lif_sw_resources_lif_usage_get(unit, -1, eep_index, NULL, &usage) );
  if (usage != _bcmDppLifTypeLinkLayer) {
      BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("OutLIF is not LL")));
  }

  ret = soc_ppd_eg_encap_entry_remove(
            unit,
            SOC_PPC_EG_ENCAP_EEP_TYPE_LL,
            eep_index
          );

  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  { 
    LOG_ERROR(BSL_LS_BCM_L3,
              (BSL_META_U(unit,
                          "soc_sand error %x\n"), ret));
    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_eg_encap_entry_remove failed")));
  }

  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}

/* Function _ppd_xxxx */
int
  _bcm_ppd_eg_encap_ipv4_entry_add(
    int unit,
    int ll_eep_index,
    int eep_index,
    bcm_l3_egress_t *egr
  ) 
{   
  uint32
    ret;
  
  uint32
    ll_eep;
  uint32
    tunnel_eep_ndx;
  SOC_PPC_EG_ENCAP_IPV4_ENCAP_INFO
    ipv4_encap_info;

  BCMDNX_INIT_FUNC_DEFS;

  ll_eep = ll_eep_index;
  tunnel_eep_ndx = eep_index;

  SOC_PPC_EG_ENCAP_IPV4_ENCAP_INFO_clear(&ipv4_encap_info);

  ipv4_encap_info.out_vsi = egr->vlan;

  ipv4_encap_info.dest.enable_gre = FALSE;
  ipv4_encap_info.dest.dest = 0;

  ret = soc_ppd_eg_encap_ipv4_entry_add(
          unit,
          tunnel_eep_ndx,
          &ipv4_encap_info,
          ll_eep
        );

  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  { 
    LOG_ERROR(BSL_LS_BCM_L3,
              (BSL_META_U(unit,
                          "soc_sand error %x\n"), ret));
    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_eg_encap_ipv4_entry_add failed")));
  }
  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}

/* GET LL infomration */
int
  _bcm_ppd_eg_encap_entry_get(
    int unit,
    int eep_index,
    bcm_l3_egress_t *egr
  )
{
  uint32
      ret;
  int 
      rv;
  SOC_PPC_EG_ENCAP_ENTRY_INFO 
      encap_entry_info[1];
  uint32 
      next_eep[1];
  uint32 
      nof_entries;
  _bcm_lif_type_e 
      usage;
  int global_lif, local_lif;

  BCMDNX_INIT_FUNC_DEFS;

  rv = BCM_E_NONE;
  
  if (SOC_IS_JERICHO(unit))  
  {
      global_lif = eep_index;
      rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, global_lif, &local_lif);
      BCMDNX_IF_ERR_EXIT(rv);
  }
  else
  {
      local_lif = global_lif = eep_index;
  }

  /* Verify LIF is LL */
  BCMDNX_IF_ERR_EXIT( _bcm_dpp_local_lif_sw_resources_lif_usage_get(unit, -1, local_lif, NULL, &usage));
  if (usage != _bcmDppLifTypeLinkLayer) {
      BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("OutLIF is not LL")));
  }

  BCM_L3_ITF_SET(egr->encap_id, BCM_L3_ITF_TYPE_LIF, eep_index);
  
  ret = soc_ppd_eg_encap_entry_get(unit,
                                   SOC_PPC_EG_ENCAP_EEP_TYPE_LL,
                                   local_lif, 0,
                                   encap_entry_info, next_eep,
                                   &nof_entries);
  BCM_SAND_IF_ERR_EXIT(ret);

  /* Fill egress info with LL entry information */
  rv = _bcm_petra_mac_from_sand_mac(egr->mac_addr, &(*encap_entry_info).entry_val.ll_info.dest_mac);
  BCMDNX_IF_ERR_EXIT(rv);

  if ((*encap_entry_info).entry_val.ll_info.ll_remark_profile != 0) {
      BCM_INT_QOS_MAP_REMARK_SET(egr->qos_map_id,(*encap_entry_info).entry_val.ll_info.ll_remark_profile);
  }   

  if ((*encap_entry_info).entry_val.ll_info.out_vid_valid) {
      egr->vlan = (*encap_entry_info).entry_val.ll_info.out_vid;
  }

  if ((SOC_IS_JERICHO_PLUS(unit) && !SOC_IS_QAX_A0(unit)) 
      && (*encap_entry_info).entry_val.ll_info.native_ll) {
      egr->flags |= BCM_L3_NATIVE_ENCAP; 
  }

  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}
/***************************************************************/
/***************************************************************/
 
/*
 * IPv4 Helper functions
 */

/* Function _ppd_xxxx */
int
  _bcm_ppd_frwrd_ipv4_glbl_info_get(
    int unit,
    bcm_l3_info_t *l3info
  )
{
  BCMDNX_INIT_FUNC_DEFS;
    
  l3info->l3info_max_vrf = 0;          /* Maximum number of virtual routers allowed. */
  BCMDNX_IF_ERR_EXIT(L3_ACCESS.used_vrf.get(unit, &(l3info->l3info_used_vrf))); /* Number of virtual routers created. */
  l3info->l3info_max_intf = 0;         /* Maximum L3 interfaces the chip supports. */
  l3info->l3info_max_intf_group = 0;   /* Maximum L3 interface groups the chip supports. */
  l3info->l3info_max_host = 0;         /* L3 host table size(unit is IPv4 unicast). */
  l3info->l3info_max_route = 0;        /* L3 route table size (unit is IPv4 route). */
  l3info->l3info_max_ecmp = 0;         /* Maximum ECMP paths allowed. */
  BCMDNX_IF_ERR_EXIT(L3_ACCESS.used_intf.get(unit, &(l3info->l3info_used_intf))); /* L3 interfaces used. */
  BCMDNX_IF_ERR_EXIT(L3_ACCESS.used_host.get(unit, &(l3info->l3info_used_host))); /* L3 host entries used. */
  BCMDNX_IF_ERR_EXIT(L3_ACCESS.used_route.get(unit, &(l3info->l3info_used_route))); /* L3 route entries used. */
  l3info->l3info_max_lpm_block = 0;    /* Maximum LPM blocks. */
  l3info->l3info_used_lpm_block = 0;   /* LPM blocks used. */
  l3info->l3info_max_l3 = 0;           /* Superseded by l3info_max_host. */
  l3info->l3info_max_defip = 0;        /* Superseded by l3info_max_route. */
  l3info->l3info_used_l3 = 0;          /* Superseded by l3info_occupied_host. */
  l3info->l3info_used_defip = 0;       /* Superseded by l3info_occupied_route. */

  BCM_EXIT;

exit:
  BCMDNX_FUNC_RETURN;
}


/* Function _ppd_xxxx */
int
  _bcm_ppd_frwrd_ipv4_nof_vrfs_get(
    int unit,
    uint32 *nof_vrfs
  )
{
 
  BCMDNX_INIT_FUNC_DEFS;
  
  *nof_vrfs = SOC_DPP_CONFIG(unit)->l3.max_nof_vrfs;
  
  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}

/* Function _ppd_xxxx */ 
int
  _bcm_ppd_frwrd_ipv4_frwrd_ip_routes_cache_mode_enable_set(
    int unit
  )
{
  uint32
    ret;
  
  SOC_PPC_VRF_ID
    vrf_ndx;
  uint32
    route_types;

  BCMDNX_INIT_FUNC_DEFS;

  vrf_ndx = DPP_DEFAULT_VRF;

  ret = soc_ppd_frwrd_ip_routes_cache_mode_enable_get(
          unit,
          vrf_ndx,
          &route_types
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  { 
    LOG_ERROR(BSL_LS_BCM_L3,
              (BSL_META_U(unit,
                          "soc_sand error %x\n"), ret));
    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ip_routes_cache_mode_enable_get failed")));
  }

  route_types = SOC_SAND_PP_IP_TYPE_ALL;


  ret = soc_ppd_frwrd_ip_routes_cache_mode_enable_set(
          unit,
          vrf_ndx,
          route_types
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  { 
    LOG_ERROR(BSL_LS_BCM_L3,
              (BSL_META_U(unit,
                          "soc_sand error %x\n"), ret));
    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ip_routes_cache_mode_enable_set failed")));
  }
  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}


/* Function _ppd_xxxx */
int
  _bcm_ppd_frwrd_ipv4_frwrd_ip_routes_cache_mode_enable_get(
    int unit
  )
{
  uint32
    ret;
  
  SOC_PPC_VRF_ID
    vrf_ndx;
  uint32
    route_types;

  BCMDNX_INIT_FUNC_DEFS;

  vrf_ndx = DPP_DEFAULT_VRF;

  ret = soc_ppd_frwrd_ip_routes_cache_mode_enable_get(
          unit,
          vrf_ndx,
          &route_types
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  { 
    LOG_ERROR(BSL_LS_BCM_L3,
              (BSL_META_U(unit,
                          "soc_sand error %x\n"), ret));
    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ip_routes_cache_mode_enable_get failed")));
  }
  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}


/* Function _ppd_xxxx */
int
  _bcm_ppd_frwrd_ipv4_frwrd_ip_routes_cache_commit(
    int unit
  )
{
  uint32
    ret;
  
  uint32
    route_types = 0;
  SOC_PPC_VRF_ID
    vrf_ndx;
  SOC_SAND_SUCCESS_FAILURE
    success;

  BCMDNX_INIT_FUNC_DEFS;

  vrf_ndx = DPP_DEFAULT_VRF;
  route_types = SOC_SAND_PP_IP_TYPE_ALL;

  ret = soc_ppd_frwrd_ip_routes_cache_commit(
          unit,
          route_types,
          vrf_ndx,
          &success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  { 
    LOG_ERROR(BSL_LS_BCM_L3,
              (BSL_META_U(unit,
                          "soc_sand error %x\n"), ret));
    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ip_routes_cache_commit failed")));
  }
  SOC_SAND_IF_FAIL_EXIT(success);
  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}

/* set default destination of UC/MC router */ 
int
  _bcm_ppd_frwrd_ipv4_default_dest_set(
    int unit,
    bcm_l3_route_t *info
  )
{  
  uint32
    ret;
  
  SOC_PPC_FEC_ID
    fec_id = 0xffffffff;
  SOC_PPC_FRWRD_IPV4_VRF_INFO
      vrf_info;
  SOC_PPC_FRWRD_DECISION_INFO 
      frwrd_decision;
  SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION
      *router_dflt_dest_ptr;
  int rv = BCM_E_NONE;

  BCMDNX_INIT_FUNC_DEFS;


  _bcm_l3_intf_to_fec(unit, info->l3a_intf, &fec_id);

  SOC_PPC_FRWRD_IPV4_VRF_INFO_clear(&vrf_info);

  ret = soc_ppd_frwrd_ipv4_vrf_info_get(
            unit,
            info->l3a_vrf,
            &vrf_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  { 
    LOG_ERROR(BSL_LS_BCM_L3,
              (BSL_META_U(unit,
                          "soc_sand error %x\n"), ret));
    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv4_vrf_info_get failed")));
  }

 /* set into MC or UC default depends on the flag */
  if(info->l3a_flags & BCM_L3_IPMC) {
      router_dflt_dest_ptr = &vrf_info.router_info.mc_default_action;
  }
  else{
      router_dflt_dest_ptr = &vrf_info.router_info.uc_default_action;
  }

  /* if port set then set default destination to forwarding decision according to port */
  if(BCM_GPORT_IS_SET(info->l3a_port_tgid)){

      /* only trap is allowed */
      if(!BCM_GPORT_IS_TRAP(info->l3a_port_tgid)) {
          BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Router default destination can be egress-object or trap")));
      }
      /* map gport to frwrd decision */
      rv = _bcm_dpp_gport_to_fwd_decision(unit,info->l3a_port_tgid,&frwrd_decision);
      BCMDNX_IF_ERR_EXIT(rv);

      router_dflt_dest_ptr->type = SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_TYPE_ACTION_PROFILE;
      sal_memcpy(
          &(router_dflt_dest_ptr->value.action_profile),
          &(frwrd_decision.additional_info.trap_info.action_profile),
          sizeof(SOC_PPC_ACTION_PROFILE)
      );
  }
  else{
      /* otherwise default destination is FEC */
      router_dflt_dest_ptr->type = SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_TYPE_FEC;
      router_dflt_dest_ptr->value.route_val = fec_id;
  }

  /* call soc level to config the HW */
  ret = soc_ppd_frwrd_ipv4_vrf_info_set(
            unit,
            info->l3a_vrf,
            &vrf_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  { 
    LOG_ERROR(BSL_LS_BCM_L3,
              (BSL_META_U(unit,
                          "soc_sand error %x\n"), ret));
    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv4_vrf_info_set failed")));
  }


  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}



/* get default destination of UC/MC router */ 
int
  _bcm_ppd_frwrd_ipv4_default_dest_get(
    int unit,
    bcm_l3_route_t *info
  )
{  
  uint32
    ret;
  
  SOC_PPC_FRWRD_IPV4_VRF_INFO
      vrf_info;
  SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION
      *router_dflt_dest_ptr;

  BCMDNX_INIT_FUNC_DEFS;


  ret = soc_ppd_frwrd_ipv4_vrf_info_get(
            unit,
            info->l3a_vrf,
            &vrf_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  { 
    LOG_ERROR(BSL_LS_BCM_L3,
              (BSL_META_U(unit,
                          "soc_sand error %x\n"), ret));
    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv4_vrf_info_get failed")));
  }

 /* get into MC or UC default depends on the flag */
  if(info->l3a_flags & BCM_L3_IPMC) {
      router_dflt_dest_ptr = &vrf_info.router_info.mc_default_action;
  }
  else{
      router_dflt_dest_ptr = &vrf_info.router_info.uc_default_action;
  }

  /* if port set then set default destination to forwarding decision according to port */
  if(router_dflt_dest_ptr->type == SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_TYPE_ACTION_PROFILE) {
      BCM_GPORT_TRAP_SET(info->l3a_port_tgid,
                         router_dflt_dest_ptr->value.action_profile.trap_code,
                         router_dflt_dest_ptr->value.action_profile.frwrd_action_strength,
                         router_dflt_dest_ptr->value.action_profile.snoop_action_strength);
  }
  else{
      /* otherwise default destination is FEC */
      _bcm_l3_fec_to_intf(unit, router_dflt_dest_ptr->value.route_val, &(info->l3a_intf));
  }

  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}

/* given bcm uc route_t info calculates ppd route info */
STATIC int _bcm_ppd_ipv4_uc_route_info_to_ppd_get(
    int                                 unit,
    bcm_l3_route_t                      *info,
    SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY     *route_key,
    SOC_PPC_FEC_ID                      *fec_id
)
{
  BCMDNX_INIT_FUNC_DEFS;

  SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY_clear(route_key);

  route_key->subnet.prefix_len = bcm_ip_mask_length(info->l3a_ip_mask);
  route_key->subnet.ip_address = info->l3a_subnet;
  if(info->l3a_flags2 &  BCM_L3_FLAGS2_SCALE_ROUTE){
	  if (SOC_IS_JERICHO(unit)) {
		  route_key->route_scale = 1;
	  }else{
		  BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_L3_FLAGS2_SCALE_ROUTE supported from Jericho and above")));
	  }
  }

  _bcm_l3_intf_to_fec(unit, info->l3a_intf, fec_id);

  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}

/* given bcm uc route_t info calculates ppd route info */
STATIC int _bcm_ppd_ipv4_vrf_uc_route_info_to_ppd_get(
    int                                 unit,
    bcm_l3_route_t                      *info,
    _dpp_l3_vrf_route_t                 *vrf_route
)
{
  BCMDNX_INIT_FUNC_DEFS;

  __dpp_l3_vrf_route_t_init(vrf_route);

  /* Route key */
  vrf_route->is_ipv6        = FALSE;
  vrf_route->vrf_id         = info->l3a_vrf;
  vrf_route->vrf_subnet     = info->l3a_subnet;
  vrf_route->vrf_ip_mask    = info->l3a_ip_mask;
  vrf_route->l3a_flags      = info->l3a_flags;
  vrf_route->l3a_flags2     = info->l3a_flags2;

  vrf_route->vrf_intf = info->l3a_intf;

  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}

/* Function _ppd_xxxx */ 
int
  _bcm_ppd_frwrd_ipv4_uc_route_add(
    int unit,
    bcm_l3_route_t *info
  )
{  
  uint32
    ret;
  
  SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY
    route_key;
  SOC_PPC_FEC_ID
    fec_id = 0xffffffff;
  SOC_SAND_SUCCESS_FAILURE
    success;
  int
    rv;

  BCMDNX_INIT_FUNC_DEFS;

  rv = _bcm_ppd_ipv4_uc_route_info_to_ppd_get(unit, info, &route_key, &fec_id);
  BCMDNX_IF_ERR_EXIT(rv);

  ret = soc_ppd_frwrd_ipv4_uc_route_add(
          unit,
          &route_key,
          fec_id,
          &success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  { 
    if(success == SOC_SAND_FAILURE_OUT_OF_RESOURCES){
        LOG_ERROR(BSL_LS_BCM_L3,
                  (BSL_META_U(unit,
                              "soc_sand error %x\n"), ret));
        BCMDNX_ERR_EXIT_MSG(BCM_E_FULL, (_BSL_BCM_MSG("Table full")));
    }
    else{
        LOG_ERROR(BSL_LS_BCM_L3,
                  (BSL_META_U(unit,
                              "soc_sand error %x\n"), ret));
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv4_uc_route_add failed")));
    }

  }
  SOC_SAND_IF_FAIL_EXIT(success);

  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}

/* Function _ppd_xxxx */
int
  _bcm_ppd_frwrd_ipv4_uc_route_get(
    int unit,
    bcm_l3_route_t *info
  )
{
  uint32
    ret;
  
  SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY
    route_key;
  uint8
    exact_match;
  SOC_PPC_FEC_ID
    fec_id;
  SOC_PPC_FRWRD_IP_ROUTE_STATUS
    route_status;
  SOC_PPC_FRWRD_IP_ROUTE_LOCATION
    location;
  uint8
    found;

  BCMDNX_INIT_FUNC_DEFS;

  /* if not default destination get from LPM db */
  SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY_clear(&route_key);

  route_key.subnet.prefix_len = bcm_ip_mask_length(info->l3a_ip_mask);
  route_key.subnet.ip_address = info->l3a_subnet;
  if(info->l3a_flags2 &  BCM_L3_FLAGS2_SCALE_ROUTE){
      if (SOC_IS_JERICHO(unit)) {
          route_key.route_scale = 1;
      }else{
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_L3_FLAGS2_SCALE_ROUTE supported from Jericho and above")));
      }
  }

  exact_match = TRUE;

  ret = soc_ppd_frwrd_ipv4_uc_route_get(
          unit,
          &route_key,
          exact_match,
          &fec_id,
          &route_status,
          &location,
          &found
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  { 
    LOG_ERROR(BSL_LS_BCM_L3,
              (BSL_META_U(unit,
                          "soc_sand error %x\n"), ret));
    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv4_uc_route_get failed")));
  }

  if (found == FALSE) {
    /* Entry not found */
    BCMDNX_ERR_EXIT_VERB_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("%s: Entry not found\n"), FUNCTION_NAME()));
  }

  /* Entry found */
  _bcm_l3_fec_to_intf(unit, fec_id, &(info->l3a_intf));

  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}

/* Function _ppd_xxxx */
int
  _bcm_ppd_frwrd_ipv4_uc_route_remove(
    int unit,
    bcm_l3_route_t *info
  )
{
  uint32
    ret;
  
  SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY
    route_key;
  SOC_SAND_SUCCESS_FAILURE
    success;


  BCMDNX_INIT_FUNC_DEFS;
  SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY_clear(&route_key);

  route_key.subnet.prefix_len = bcm_ip_mask_length(info->l3a_ip_mask);
  route_key.subnet.ip_address = info->l3a_subnet;
  if(info->l3a_flags2 &  BCM_L3_FLAGS2_SCALE_ROUTE){
      if (SOC_IS_JERICHO(unit)) {
          route_key.route_scale = 1;
      }else{
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_L3_FLAGS2_SCALE_ROUTE supported from Jericho and above")));
      }
  }

  ret = soc_ppd_frwrd_ipv4_uc_route_remove(
          unit,
          &route_key,
          &success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  { 
    LOG_ERROR(BSL_LS_BCM_L3,
              (BSL_META_U(unit,
                          "soc_sand error %x\n"), ret));
    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv4_uc_route_remove failed")));
  }
  SOC_SAND_IF_FAIL_EXIT(success);

exit:
  BCMDNX_FUNC_RETURN;
}

/* Function _ppd_xxxx */
int
  _bcm_ppd_frwrd_ipv4_routing_table_clear(
    int unit
  )
{
  
  int
    soc_sand_rv;

  BCMDNX_INIT_FUNC_DEFS;

  soc_sand_rv = soc_ppd_frwrd_ipv4_vrf_all_routing_tables_clear(
            unit,
            SOC_PPC_FRWRD_IP_LPM_ONLY
          );
  BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
  BCMDNX_FUNC_RETURN;
}

STATIC int _bcm_ppd_ipv4_host_key_to_ppd_get(
    int                                 unit,
    bcm_l3_host_t                       *info,
    SOC_PPC_FRWRD_IPV4_HOST_KEY         *host_key
)
{
  BCMDNX_INIT_FUNC_DEFS;
  SOC_PPC_FRWRD_IPV4_HOST_KEY_clear(host_key);

  host_key->ip_address = info->l3a_ip_addr;
  host_key->vrf_ndx = info->l3a_vrf;

  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}

/* given bcm host_t info calculates ppd host info */
STATIC int _bcm_ppd_ipv4_host_info_to_ppd_get(
    int                                 unit,
    bcm_l3_host_t                       *info,
    SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO  *routing_info
)
{
  uint32
    ret;
  uint32 push_profile;
  int label;
  SOC_PPC_FEC_ID fec_id=0;
  int rif_entry_id=0;
  int rv = BCM_E_NONE;
  _bcm_dpp_gport_hw_resources gport_hw_resources;

  /* 0:FEC, 1:port+eep, 2:FEC+eep 3:tunnel-itf + port --> tunnel, 4 (format III in arad+      : FEC + ARP + out-RIF  
                                                                     format III B in jericho)
   * 5:FEC+outlif
   */
  int frwrd_type = 0;
  int eep_val;

  BCMDNX_INIT_FUNC_DEFS;
  SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO_clear(routing_info);

  /* check mode: assign internal frwrd type */

  /* for roo (routing over overlay): FEC + ARP + out-RIF.
   * In arad+: support limited to vxlan overlay,
   * In jericho: all types of overlay are supported. 
   * check FEC exists: 
   *    1. check l3a_port_tgid is an overlay gport (jericho) or vxlan gport (arad+) or frwrd gport (arad+ and jericho)
   *    2. Check that l3a_port_tgid gport is a FEC
   * check outRIF exists: check l3a_intf is an outRIF.
   * check ARP pointer exists: check encap_id is an ARP pointer
   */

  if ((((_BCM_GPORT_IS_OVERLAY(info->l3a_port_tgid)) && (SOC_IS_JERICHO(unit))) /* check FEC: check is overlay gport */
        || ((SOC_IS_ARADPLUS(unit)) &&  (BCM_GPORT_IS_VXLAN_PORT(info->l3a_port_tgid))) 
        || ((SOC_IS_ARADPLUS(unit)) &&  (BCM_GPORT_IS_FORWARD_PORT(info->l3a_port_tgid))))
     && (BCM_L3_ITF_TYPE_IS_RIF(info->l3a_intf))  /* check outRIF */
     && (info->encap_id != 0))/* check ARP */ { 
      if ((SOC_IS_ARADPLUS(unit)) &&  (BCM_GPORT_IS_FORWARD_PORT(info->l3a_port_tgid))) {
          frwrd_type = 4;
      } else {
          /* check FEC: check overlay gport is a FEC) */
          rv = _bcm_dpp_gport_to_hw_resources(unit, info->l3a_port_tgid, _BCM_DPP_GPORT_HW_RESOURCES_FEC | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK, 
                                             &gport_hw_resources);
          if (rv == BCM_E_PARAM) {
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("l3a_port_tgid should be a gport for an overlay, so must have a fec")));
          }
          BCMDNX_IF_ERR_EXIT(rv);

          frwrd_type = 4;
      }  

  }
  /* no FEC is used and encap-id is valid: EEP (MAC + vlan) + port  */
  else if(!BCM_L3_ITF_TYPE_IS_FEC(info->l3a_intf) && info->encap_id != 0) {
          frwrd_type = 1;
  }
  /* for tunnel: tunnel-itf + port */
  else if(BCM_L3_ITF_TYPE_IS_LIF(info->l3a_intf)) {
      frwrd_type = 3;
  }
  
  else if(info->encap_id != 0){
      if (BCM_FORWARD_ENCAP_ID_IS_OUTLIF(info->encap_id)) {
          /* FEC + OutLIF */
          frwrd_type = 5;
      }
      else {
          /* EEP is valid this host points to: FEC + EEP */
          frwrd_type = 2;
      }
  }
  else{/* simply host points to FEC */
      frwrd_type = 0;
  }

  /* verify parameters according to mode */
  if(frwrd_type == 1 || frwrd_type == 3) {/* l3-interface or tunnel interface */
      /* port has to be valid forwarding is port */
      if(info->l3a_port_tgid == BCM_GPORT_TYPE_NONE) {
          LOG_ERROR(BSL_LS_BCM_L3,
                    (BSL_META_U(unit,
                                "host points directly to outRIF, port must be valid \n")));
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("host points directly to outRIF, port must be valid")));
       }
  }
      
  if (!SOC_DPP_CONFIG(unit)->pp.next_hop_mac_extension_enable &&
      !BCM_MAC_IS_ZERO(info->l3a_nexthop_mac)) {
      BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("l3a_nexthop_mac must be zero in case ARP extension is not enabled")));
  }
  
  /* verify parameters according to mode */
  if(frwrd_type == 1 || frwrd_type == 4) {/* l3-interface */
      /* encap has to be valid */
      if(info->encap_id == 0) {
          LOG_ERROR(BSL_LS_BCM_L3,
                    (BSL_META_U(unit,
                                "host points directly to outRIF, encap_id must be valid \n")));
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("host points directly to outRIF, encap_id must be valid")));
       }

      /* for frwrd_type == 4, encap must be even.
         lsb is truncated when inserted in LEM table. */
      if ((frwrd_type == 4) && ((BCM_L3_ITF_VAL_GET(info->encap_id) & 0x1) != 0)) {
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("encap_id must be even")));
      }
      /* check encap id is not out of range.
         Take into account, the truncated lsb
         For Jericho only, check against 2MSBs.
         If the device is in arad system mode, then 2 MSBs are set as EEI type lsbs (and are not outlif MSBs)
         */
      if (frwrd_type == 4) {
          /* For jericho in arad system mode or arad+, ARP msbs are fixed to match EEI type 
             So only check that ARP MSBs are 0*/
          if ((SOC_IS_ARADPLUS_A0(unit)) 
              || ((SOC_IS_JERICHO(unit)) && (soc_property_get(unit, spn_SYSTEM_IS_ARAD_IN_SYSTEM, 0)))) {
              if ((BCM_L3_ITF_VAL_GET(info->encap_id) >> (1 + SOC_DPP_DEFS_GET(unit, lem_3b_payload_format_eei_nof_bits))) != 0) {
                  BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("encap_id is out of range")));
              }
          }
          /* for jericho, not in arad sytem mode, ARP msbs are global configuration
             Check that the ARP msbs are as expected */
          else if (SOC_IS_JERICHO(unit)) {
              if ((BCM_L3_ITF_VAL_GET(info->encap_id) >> (1 + SOC_DPP_DEFS_GET(unit, lem_3b_payload_format_eei_nof_bits))) != SOC_ROO_HOST_ARP_MSBS(unit)) {
                  /* Case of out of range, MSBs are 0 */
                  if (SOC_ROO_HOST_ARP_MSBS(unit) == 0) {
                      BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("encap_id is out of range")));
                  } else {
                      BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("encap_id MSBs must be configured as 0x%x"), SOC_ROO_HOST_ARP_MSBS(unit)));
                  }
              }
          }
      }

  }
  
  /* set LEM destination: FEC or port */
  /* intf can be FEC or directly l3-intf */
  if (BCM_L3_ITF_TYPE_IS_FEC(info->l3a_intf)) {
      _bcm_l3_intf_to_fec(unit, info->l3a_intf, &fec_id);
      SOC_PPD_FRWRD_DECISION_FEC_SET(unit, &routing_info->frwrd_decision, fec_id, ret);
      BCM_SAND_IF_ERR_EXIT(ret);
  }
  else if(BCM_L3_ITF_TYPE_IS_RIF(info->l3a_intf) || BCM_L3_ITF_TYPE_IS_LIF(info->l3a_intf)) {/* l3-interface*/
      /* for roo, and if l3a_port_tgid is forward gport. Extract the fec from the gport */
      if (BCM_GPORT_IS_FORWARD_PORT(info->l3a_port_tgid)) {
              routing_info->frwrd_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_FEC;
          routing_info->frwrd_decision.dest_id = BCM_GPORT_FORWARD_PORT_GET(info->l3a_port_tgid);
      } else {
          /* port has to be valid then use it and point to oultif */
          rv = _bcm_dpp_gport_to_fwd_decision(unit,info->l3a_port_tgid,&routing_info->frwrd_decision);
          BCMDNX_IF_ERR_EXIT(rv);

          /* we reach here with fwd type = dest+outlif */
          if ((frwrd_type == 1) && (routing_info->frwrd_decision.type == SOC_PPC_FRWRD_DECISION_TYPE_FEC)) {
              /* destination is FEC, change fwd type to be FEC+EEP */
              frwrd_type = 2;
          }
      }
  }
  if((frwrd_type == 2) && (!BCM_FORWARD_ENCAP_ID_IS_EEI(info->encap_id)) && (info->l3a_flags & BCM_L3_ENCAP_SPACE_OPTIMIZED)){
      BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("encap_id must be encoded as EEI when using flag BCM_L3_ENCAP_SPACE_OPTIMIZED")));
  }
  if((frwrd_type == 2) && (BCM_FORWARD_ENCAP_ID_IS_EEI(info->encap_id)) && (info->l3a_flags & BCM_L3_ENCAP_SPACE_OPTIMIZED)){
      if (BCM_FORWARD_ENCAP_ID_EEI_USAGE_GET(info->encap_id) == BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT) {
          label = BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_VC_GET(info->encap_id);
          push_profile = BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_PUSH_PROFILE_GET(info->encap_id);
          routing_info->frwrd_decision.additional_info.eei.type = SOC_PPC_EEI_TYPE_MPLS;
          routing_info->frwrd_decision.additional_info.eei.val.mpls_command.command = SOC_PPC_MPLS_COMMAND_TYPE_PUSH;
          routing_info->frwrd_decision.additional_info.eei.val.mpls_command.label = label;
          routing_info->frwrd_decision.additional_info.eei.val.mpls_command.push_profile = push_profile;
      }
  }
  else if((frwrd_type == 2) && BCM_FORWARD_ENCAP_ID_IS_EEI(info->encap_id) &&
      (BCM_FORWARD_ENCAP_ID_EEI_USAGE_GET(info->encap_id) == BCM_FORWARD_ENCAP_ID_EEI_USAGE_DEST_VTEP_PTR)){
      routing_info->frwrd_decision.additional_info.eei.type = SOC_PPC_EEI_TYPE_RAW;
      routing_info->frwrd_decision.additional_info.eei.val.raw = BCM_FORWARD_ENCAP_ID_VAL_GET(info->encap_id);
  }
  
  /* setting host frwrd decision */
  /* forward decision = port + out-RIF (outlif) */
  else if (frwrd_type == 3) {
      rif_entry_id = BCM_L3_ITF_VAL_GET(info->l3a_intf);
  
      routing_info->frwrd_decision.additional_info.outlif.type = SOC_PPC_OUTLIF_ENCODE_TYPE_RAW;
      routing_info->frwrd_decision.additional_info.outlif.val = rif_entry_id;
  }
  else if(frwrd_type == 1) {
      routing_info->frwrd_decision.additional_info.outlif.type = SOC_PPC_OUTLIF_ENCODE_TYPE_RAW;
      routing_info->frwrd_decision.additional_info.outlif.val = BCM_L3_ITF_VAL_GET(info->encap_id);
  }
  /* FEC + MAC --> forward decision = FEC + ARP (EEI)*/
  else if(frwrd_type == 2) {
      routing_info->frwrd_decision.additional_info.eei.type = SOC_PPC_EEI_TYPE_OUTLIF;

      _bcm_l3_encap_id_to_eep(unit, info->encap_id, &eep_val);
      routing_info->frwrd_decision.additional_info.eei.val.outlif = eep_val;

      /* unused bits of the EEI should point at the MAC LSBs */
      routing_info->mac_lsb = info->l3a_nexthop_mac[5];
  }
  /* format III. 
  * forward decision = FEC + ARP (EEI) + HI + native-vsi/out-RIF
  * configure eei and native-vsi/out-RIF in frwrd decision object (soc layer object) */
  else if (frwrd_type == 4) {
     /* configure native-vsi/out-RIF */
     rif_entry_id = BCM_L3_ITF_VAL_GET(info->l3a_intf);
     routing_info->native_vsi = rif_entry_id; 
     
     /* configure frwrd_decision.eei entry (arp pointer) */
     routing_info->frwrd_decision.additional_info.eei.type = SOC_PPC_EEI_TYPE_OUTLIF;
     _bcm_l3_encap_id_to_eep(unit, info->encap_id, &eep_val);
     routing_info->frwrd_decision.additional_info.eei.val.outlif = eep_val;

     /* HI: mac lsbs */
     routing_info->mac_lsb = info->l3a_nexthop_mac[5];   
  }
  else if((frwrd_type == 5) && BCM_FORWARD_ENCAP_ID_IS_OUTLIF(info->encap_id)) {
      routing_info->frwrd_decision.additional_info.outlif.type = SOC_PPC_OUTLIF_ENCODE_TYPE_RAW;
      routing_info->frwrd_decision.additional_info.outlif.val = BCM_FORWARD_ENCAP_ID_VAL_GET(info->encap_id);
  }

  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}

/* Function _ppd_xxxx */
int
  _bcm_ppd_frwrd_ipv4_host_add(
    int unit,
    bcm_l3_host_t *info
  )
{
  uint32
    ret; 
  SOC_PPC_FRWRD_IPV4_HOST_KEY
    host_key;
  SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO
    routing_info;
  SOC_SAND_SUCCESS_FAILURE
    success;
  int rv = BCM_E_NONE;

  BCMDNX_INIT_FUNC_DEFS;

#if defined(INCLUDE_KBP)
      if(ARAD_KBP_ENABLE_ANY_IPV4UC_PROGRAM && ARAD_KBP_24BIT_FWD && !(info->l3a_flags & BCM_L3_HOST_LOCAL)) {
          BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_l3_host_add is not supported for 24bit FWD; Use bcm_l3_route_add instead")));
      }
#endif

  rv = _bcm_ppd_ipv4_host_info_to_ppd_get(unit, info, &routing_info);
  BCMDNX_IF_ERR_EXIT(rv);

  rv = _bcm_ppd_ipv4_host_key_to_ppd_get(unit, info, &host_key);
  BCMDNX_IF_ERR_EXIT(rv);

  if(info->l3a_flags2 &  BCM_L3_FLAGS2_RAW_ENTRY) {
      routing_info.frwrd_decision.flags |= _BCM_L3_FLAGS2_RAW_ENTRY;
  }

  if(info->l3a_flags &  BCM_L3_HOST_LOCAL){
      if (SOC_IS_JERICHO(unit)) {
          host_key.is_local_host = 1;
      }else{
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_L3_HOST_LOCAL supported from Jericho and above")));
      }
  }

  if(info->l3a_flags2 &  BCM_L3_FLAGS2_SCALE_ROUTE){
      if (SOC_IS_JERICHO(unit)) {
          host_key.route_scale = 1;
      }else{
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_L3_FLAGS2_SCALE_ROUTE supported from Jericho and above")));
      }
  }

#if defined(INCLUDE_KBP)
  if(ARAD_KBP_ENABLE_ANY_IPV4UC_PROGRAM && (host_key.route_scale == 1) && (host_key.is_local_host == 1)){
      BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_L3_FLAGS2_SCALE_ROUTE and BCM_L3_HOST_LOCAL cannot be assign together in KBP mode")));
  }

  if(ARAD_KBP_ENABLE_IPV4_UC_PUBLIC && (host_key.vrf_ndx == 0) && (host_key.is_local_host == 1)){
      BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("bcm_l3_host_add is not supported to add public entry to the LEM when public KBP enabled")));
  }
#endif

#ifdef _BCM_DPP_KBP_TIME_MEASUREMENTS
  soc_sand_ll_timer_set("ARAD_KBP_TIMERS_SOC", ARAD_KBP_TIMERS_SOC);
#endif
  ret = soc_ppd_frwrd_ipv4_host_add(
          unit,
          &host_key,
          &routing_info,
          &success
        );
#ifdef _BCM_DPP_KBP_TIME_MEASUREMENTS
  soc_sand_ll_timer_stop(ARAD_KBP_TIMERS_SOC);
#endif
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  { 
    LOG_ERROR(BSL_LS_BCM_L3,
              (BSL_META_U(unit,
                          "soc_sand error %x\n"), ret));
    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv4_host_add failed")));
  }
  SOC_SAND_IF_FAIL_EXIT(success);

  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}

/* given ppd host info calculate bcm host_t info */
STATIC int _bcm_ppd_host_info_from_ppd_get(
   int unit,
   SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO  *routing_info,
   SOC_PPC_FRWRD_IP_ROUTE_STATUS       *route_status,
   bcm_l3_host_t                       *info
)
{
    int rv;

    /* for roo application, fec is saved in info->l3a_port_tgid as gport */
    uint32 is_roo_application;
    int label;
    uint32 push_profile;
    int eei;

    BCMDNX_INIT_FUNC_DEFS;

    is_roo_application = routing_info->native_vsi;


    /* destination */
    /* FEC */
    if (routing_info->frwrd_decision.type == SOC_PPC_FRWRD_DECISION_TYPE_FEC && !is_roo_application) {
        _bcm_l3_fec_to_intf(unit, routing_info->frwrd_decision.dest_id, &(info->l3a_intf)); 
    }
    /* FEC. For roo application in arad+, always a vxlan gport */
    else if (routing_info->frwrd_decision.type == SOC_PPC_FRWRD_DECISION_TYPE_FEC && is_roo_application) {
        /* We do not provide the opposite direction of mapping FEC-ID to VXLAN-Gport.
           The downside of it is that Add and get is assymetric but the positive side is that
           we don't add more SW-state or complicated SW implementation.
           It is expected from the user to save mapping between FEC-ID and its gport.
           For bcm_l3_host_find, we'll return a forward gport
           For bcm_l3_host_add, the user provide a vxlan gport */
        if (SOC_IS_ARADPLUS(unit)) {
            BCM_GPORT_FORWARD_PORT_SET(info->l3a_port_tgid, routing_info->frwrd_decision.dest_id);
        }
    }
    else{ /* destination is FEC then use port to get destination*/
       /* port has to be valid then use it and point to oultif */

       /* Just use {type, dest_id} in  frwrd_decision to get destination */
       SOC_PPC_FRWRD_DECISION_INFO frwrd_decision_local;
       SOC_PPC_FRWRD_DECISION_INFO_clear(&frwrd_decision_local);
       frwrd_decision_local.type= routing_info->frwrd_decision.type;
       frwrd_decision_local.dest_id = routing_info->frwrd_decision.dest_id;       
       
       rv = _bcm_dpp_gport_and_encap_from_fwd_decision(unit,&(info->l3a_port_tgid),NULL,&frwrd_decision_local, -1/*encap_usage*/,0/*force_destination*/);
       BCMDNX_IF_ERR_EXIT(rv);
    }
    /* FEC + EEI */
    if (routing_info->frwrd_decision.additional_info.eei.type == SOC_PPC_EEI_TYPE_MPLS) {
        /* frwrd_type = 2 */
        info->l3a_flags |= BCM_L3_ENCAP_SPACE_OPTIMIZED;
        label = routing_info->frwrd_decision.additional_info.eei.val.mpls_command.label;
        push_profile = routing_info->frwrd_decision.additional_info.eei.val.mpls_command.push_profile;
        BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_SET(eei, label, push_profile);
        BCM_FORWARD_ENCAP_ID_VAL_SET(info->encap_id, BCM_FORWARD_ENCAP_ID_TYPE_EEI, BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT, eei);
    }

    /* encap-id */
    if ( routing_info->frwrd_decision.additional_info.eei.type == SOC_PPC_EEI_TYPE_OUTLIF) {
        /* frwrd_type = 2 or 4 */
        BCM_L3_ITF_SET(info->encap_id, BCM_L3_ITF_TYPE_LIF, routing_info->frwrd_decision.additional_info.eei.val.outlif); 
    }

    if (routing_info->frwrd_decision.additional_info.outlif.type == SOC_PPC_OUTLIF_ENCODE_TYPE_RAW) {
        if (routing_info->frwrd_decision.additional_info.outlif.val < SOC_DPP_CONFIG(unit)->l3.nof_rifs) {
            /* frwrd_type = 3 */
            BCM_L3_ITF_SET(info->l3a_intf, BCM_L3_ITF_TYPE_LIF, routing_info->frwrd_decision.additional_info.outlif.val);
        }
        else {
            /* frwrd_type = 1 */
            BCM_L3_ITF_SET(info->encap_id, BCM_L3_ITF_TYPE_LIF, routing_info->frwrd_decision.additional_info.outlif.val); 
        }      
    }

    /* flags */
    /* hit indication */
    if (*route_status & SOC_PPC_FRWRD_IP_ROUTE_STATUS_ACCESSED) {
        info->l3a_flags |= BCM_L3_HIT;
    }

    /* MAC address */
    info->l3a_nexthop_mac[5] = routing_info->mac_lsb;

    /* native-vsi */
    if (routing_info->native_vsi != 0) {
        BCM_L3_ITF_SET(info->l3a_intf, BCM_L3_ITF_TYPE_RIF, routing_info->native_vsi); 
    }


exit:
  BCMDNX_FUNC_RETURN;
}

/* Function _ppd_xxxx */
int
  _bcm_ppd_frwrd_ipv6_host_add(
    int unit,
    bcm_l3_host_t *info
  )
{
  uint32
    ret; 
  /* The routing_info is identical in IPV6 and IPV4  */
  SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO
    routing_info;
  SOC_PPC_FRWRD_IPV6_UC_ROUTE_KEY
    route_key;
  SOC_SAND_SUCCESS_FAILURE
    success;
  int rv = BCM_E_NONE;
  uint32 vrf_ndx;

  BCMDNX_INIT_FUNC_DEFS;

#if defined(INCLUDE_KBP)
  if(ARAD_KBP_ENABLE_ANY_IPV6UC_PROGRAM && ARAD_KBP_24BIT_FWD) {
      BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_l3_host_add is not supported for 24bit FWD; Use bcm_l3_route_add instead")));
  }
#endif

  SOC_PPC_FRWRD_IPV6_UC_ROUTE_KEY_clear(&route_key);

  rv = _bcm_ppd_ipv4_host_info_to_ppd_get(unit, info, &routing_info);
  BCMDNX_IF_ERR_EXIT(rv);

  /* Route key */
  SOC_PPC_FRWRD_IPV6_UC_ROUTE_KEY_clear(&route_key);
  route_key.subnet.prefix_len = _SHR_L3_IP6_MAX_NETLEN;
  rv = _bcm_l3_bcm_ipv6_addr_to_sand_ipv6_addr(unit, info->l3a_ip6_addr, &(route_key.subnet.ipv6_address));
  BCMDNX_IF_ERR_EXIT(rv);
  if (info->l3a_flags2 &  BCM_L3_FLAGS2_SCALE_ROUTE) {
      route_key.route_scale = 1;
  }
  vrf_ndx = info->l3a_vrf;

  ret = soc_ppd_frwrd_ipv6_host_add(
          unit,
          vrf_ndx,
          &route_key,
          &routing_info,
          &success
        );

  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  { 
    LOG_ERROR(BSL_LS_BCM_L3,
              (BSL_META_U(unit,
                          "soc_sand error %x\n"), ret));
    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("_bcm_ppd_frwrd_ipv6_host_add failed")));
  }
  SOC_SAND_IF_FAIL_EXIT(success);

  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}

/* Function _ppd_xxxx */
int
  _bcm_ppd_frwrd_ipv6_host_get(
    int unit,
    bcm_l3_host_t *info
  )
{
  uint32
    ret; 
  /* The routing_info is identical in IPV6 and IPV4  */
  SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO
    routing_info;
  SOC_PPC_FRWRD_IPV6_UC_ROUTE_KEY
    route_key;
  SOC_PPC_FRWRD_IP_ROUTE_STATUS
    route_status;
  uint8
    found;
  int rv = BCM_E_NONE;
  uint32 vrf_ndx;

  BCMDNX_INIT_FUNC_DEFS;

#if defined(INCLUDE_KBP)
  if(ARAD_KBP_ENABLE_ANY_IPV6UC_PROGRAM && ARAD_KBP_24BIT_FWD) {
      BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_l3_host_find is not supported for 24bit FWD in KBP; Use bcm_l3_route_get instead")));
  }
#endif

  SOC_PPC_FRWRD_IPV6_UC_ROUTE_KEY_clear(&route_key);
  SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO_clear(&routing_info);

  /* Route key */
  route_key.subnet.prefix_len = _SHR_L3_IP6_MAX_NETLEN;
  rv = _bcm_l3_bcm_ipv6_addr_to_sand_ipv6_addr(unit, info->l3a_ip6_addr, &(route_key.subnet.ipv6_address));
  BCMDNX_IF_ERR_EXIT(rv);
  if (info->l3a_flags2 & BCM_L3_FLAGS2_SCALE_ROUTE) {
      route_key.route_scale = 1;
  }
  vrf_ndx = info->l3a_vrf;

  ret = soc_ppd_frwrd_ipv6_host_get(
          unit,
          vrf_ndx,
          &route_key,
          &route_status,
          &routing_info,
          &found
        );

  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  { 
    LOG_ERROR(BSL_LS_BCM_L3,
              (BSL_META_U(unit,
                          "soc_sand error %x\n"), ret));
    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("_bcm_ppd_frwrd_ipv6_host_add failed")));
  }

  if (found == FALSE) {
    /* Entry not found */
    BCMDNX_ERR_EXIT_VERB_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("%s: Entry not found\n"), FUNCTION_NAME()));
  }

  rv = _bcm_ppd_host_info_from_ppd_get(unit,&routing_info,&route_status,info);
  BCMDNX_IF_ERR_EXIT(rv);

  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}

/* Function _ppd_xxxx */
int
  _bcm_ppd_frwrd_ipv6_host_delete(
    int unit,
    bcm_l3_host_t *info
  )
{
  uint32
    ret;
  SOC_PPC_FRWRD_IPV6_UC_ROUTE_KEY
    route_key;
  SOC_SAND_SUCCESS_FAILURE
    success;
  int rv = BCM_E_NONE;
  uint32 vrf_ndx;

  BCMDNX_INIT_FUNC_DEFS;

  SOC_PPC_FRWRD_IPV6_UC_ROUTE_KEY_clear(&route_key);

  /* Route key */
  route_key.subnet.prefix_len = _SHR_L3_IP6_MAX_NETLEN;
  rv = _bcm_l3_bcm_ipv6_addr_to_sand_ipv6_addr(unit, info->l3a_ip6_addr, &(route_key.subnet.ipv6_address));
  BCMDNX_IF_ERR_EXIT(rv);
  if (info->l3a_flags2 &  BCM_L3_FLAGS2_SCALE_ROUTE) {
      route_key.route_scale = 1;
  }
  vrf_ndx = info->l3a_vrf;

  ret = soc_ppd_frwrd_ipv6_host_delete(
          unit,
          vrf_ndx,
          &route_key,
          &success
        );

  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  { 
    LOG_ERROR(BSL_LS_BCM_L3,
              (BSL_META_U(unit,
                          "soc_sand error %x\n"), ret));
    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("_bcm_ppd_frwrd_ipv6_host_delete failed")));
  }
  SOC_SAND_IF_FAIL_EXIT(success);

  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}

/* IPV6 VRF UC route functions */ 
/* Function _ppd_xxxx */
int
  _bcm_ppd_frwrd_ipv6_vrf_uc_route_add(
    int unit,
    bcm_l3_route_t *info,
    SOC_PPC_FRWRD_DECISION_INFO            *route_info
  )
{ 
  uint32               rv;
  _dpp_l3_vrf_route_t  vrf_route;
    
  BCMDNX_INIT_FUNC_DEFS;
  __dpp_l3_vrf_route_t_init(&vrf_route);

  /* Route key */
  vrf_route.is_ipv6        = TRUE;
  vrf_route.vrf_id         = info->l3a_vrf;
  sal_memcpy(&(vrf_route.vrf_ip6_net),&(info->l3a_ip6_net),sizeof(bcm_ip6_t));
  sal_memcpy(&(vrf_route.vrf_ip6_mask),&(info->l3a_ip6_mask),sizeof(bcm_ip6_t));
  vrf_route.l3a_flags      = info->l3a_flags;
  vrf_route.l3a_flags2     = info->l3a_flags2;

  /* Common function for UC and MC route add */
  rv = _bcm_ppd_frwrd_ipv6_vrf_route_add(unit, &vrf_route, route_info);
  BCMDNX_IF_ERR_EXIT(rv);

exit:
  BCMDNX_FUNC_RETURN;
}

int
  _bcm_ppd_frwrd_ipv6_vrf_uc_route_get(
    int unit,
    bcm_l3_route_t *info,
    bcm_if_t *encap_id
  )
{
    uint32                           rv;
    uint8                            get_flags=0, found;
    SOC_PPC_VRF_ID                   vrf_ndx;
    SOC_PPC_FRWRD_IPV6_VPN_ROUTE_KEY route_key;
    SOC_PPC_FRWRD_IP_ROUTE_STATUS    route_status = 0;
    SOC_PPC_FRWRD_IP_ROUTE_LOCATION  location;
    SOC_PPC_FRWRD_DECISION_INFO      route_info;

   BCMDNX_INIT_FUNC_DEFS;
   SOC_PPC_FRWRD_IPV6_VPN_ROUTE_KEY_clear(&route_key);
   SOC_PPC_FRWRD_DECISION_INFO_clear(&route_info);

   /* Route key */
   route_key.subnet.prefix_len = bcm_ip6_mask_length(info->l3a_ip6_mask);
   rv = _bcm_l3_bcm_ipv6_addr_to_sand_ipv6_addr(unit, info->l3a_ip6_net, &(route_key.subnet.ipv6_address));
   BCMDNX_IF_ERR_EXIT(rv);
   if (info->l3a_flags2 &  BCM_L3_FLAGS2_SCALE_ROUTE) {
       route_key.route_scale = 1;
   }
   if (info->l3a_flags2 &  BCM_L3_FLAGS2_RAW_ENTRY) {
       route_info.flags |= _BCM_L3_FLAGS2_RAW_ENTRY;
   }

   vrf_ndx   = info->l3a_vrf;
   get_flags = SOC_PPC_FRWRD_IP_EXACT_MATCH;

   if (info->l3a_flags & BCM_L3_HIT_CLEAR) {
       get_flags |= SOC_PPC_FRWRD_IP_CLEAR_ON_GET;
   }

   rv = soc_ppd_frwrd_ipv6_vrf_route_get(unit,
                                          vrf_ndx,
                                          &route_key,
                                          get_flags,
                                          &route_info,
                                          &route_status,
                                          &location,
                                          &found);

   if (soc_sand_get_error_code_from_error_word(rv) != SOC_SAND_OK)
   {
     LOG_ERROR(BSL_LS_BCM_L3, (BSL_META_U(unit, "soc_sand error %x\n"), rv));
     BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv6_vrf_route_get failed")));
   }

   if (found == FALSE) {
     /* Entry not found */
     BCMDNX_ERR_EXIT_VERB_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("%s: Entry not found\n"), FUNCTION_NAME()));
   }

   /* Entry found */
   if (route_status & SOC_PPC_FRWRD_IP_ROUTE_STATUS_ACCESSED) {
       info->l3a_flags |= BCM_L3_HIT;
   }

   if (info->l3a_flags2 & BCM_L3_FLAGS2_RAW_ENTRY) {
       /* in case of raw data entry no decoding in needed*/
       info->l3a_intf = route_info.dest_id;
   } else {
       _bcm_l3_fec_to_intf(unit, route_info.dest_id, &(info->l3a_intf));
   }

   *encap_id = 0; 
   if (route_info.additional_info.eei.type == SOC_PPC_EEI_TYPE_OUTLIF) {
       /* Retrieve the encap-id for Host table */
       BCM_L3_ITF_SET(*encap_id, BCM_L3_ITF_TYPE_LIF, route_info.additional_info.eei.val.outlif);
   }

exit:
  BCMDNX_FUNC_RETURN;
}

/* Function _ppd_xxxx */
int
  _bcm_ppd_frwrd_ipv6_vrf_uc_route_remove(
    int unit,
    bcm_l3_route_t *info
  )
{ 
  uint32               rv;
  _dpp_l3_vrf_route_t  vrf_route;
    
  BCMDNX_INIT_FUNC_DEFS;
  __dpp_l3_vrf_route_t_init(&vrf_route);

  /* Route key */
  vrf_route.is_ipv6        = TRUE;
  vrf_route.vrf_id         = info->l3a_vrf;
  sal_memcpy(&(vrf_route.vrf_ip6_net),&(info->l3a_ip6_net),sizeof(bcm_ip6_t));
  sal_memcpy(&(vrf_route.vrf_ip6_mask),&(info->l3a_ip6_mask),sizeof(bcm_ip6_t));
  vrf_route.l3a_flags      = info->l3a_flags;
  vrf_route.l3a_flags2     = info->l3a_flags2;

  /* Common function for UC and MC route remove */
  rv = _bcm_ppd_frwrd_ipv6_vrf_route_remove(unit,&vrf_route);

  BCMDNX_IF_ERR_EXIT(rv);    
exit:
  BCMDNX_FUNC_RETURN;
}

int
  _bcm_ppd_frwrd_ipv6_uc_route_add(
    int unit,
    bcm_l3_route_t *info,
    SOC_PPC_FRWRD_DECISION_INFO            *route_info
  )
{  
  uint32
    ret;
  
  SOC_PPC_FRWRD_IPV6_UC_ROUTE_KEY
    route_key;
  SOC_SAND_SUCCESS_FAILURE
    success;

  BCMDNX_INIT_FUNC_DEFS;
  SOC_PPC_FRWRD_IPV6_UC_ROUTE_KEY_clear(&route_key);

  route_key.subnet.prefix_len = bcm_ip6_mask_length(info->l3a_ip6_mask);
  ret = _bcm_l3_bcm_ipv6_addr_to_sand_ipv6_addr(unit, info->l3a_ip6_net, &(route_key.subnet.ipv6_address));
  BCMDNX_IF_ERR_EXIT(ret);

  ret = soc_ppd_frwrd_ipv6_uc_route_add(
          unit,
          &route_key,
          route_info,
          &success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
      if(success == SOC_SAND_FAILURE_OUT_OF_RESOURCES){
          LOG_ERROR(BSL_LS_BCM_L3,
                    (BSL_META_U(unit,
                                "soc_sand error %x\n"), ret));
          BCMDNX_ERR_EXIT_MSG(BCM_E_FULL, (_BSL_BCM_MSG("Table full")));
      }
      else{
          LOG_ERROR(BSL_LS_BCM_L3,
                    (BSL_META_U(unit,
                                "soc_sand error %x\n"), ret));
          BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv6_uc_route_add failed")));
      }
  }
  SOC_SAND_IF_FAIL_EXIT(success);

  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}

/* Function _ppd_xxxx */
int
  _bcm_ppd_frwrd_ipv6_uc_route_get(
    int unit,
    bcm_l3_route_t *info,
    bcm_if_t *encap_id
  )
{
  uint32
    ret;
  
  SOC_PPC_FRWRD_IPV6_UC_ROUTE_KEY
    route_key;
  uint8
    get_flags=0;
  SOC_PPC_FRWRD_IP_ROUTE_STATUS
    route_status;
  SOC_PPC_FRWRD_IP_ROUTE_LOCATION
    location;
  uint8
    found;
  SOC_PPC_FRWRD_DECISION_INFO
    route_info;

  BCMDNX_INIT_FUNC_DEFS;
  SOC_PPC_FRWRD_IPV6_UC_ROUTE_KEY_clear(&route_key);

  route_key.subnet.prefix_len = bcm_ip6_mask_length(info->l3a_ip6_mask);
  ret = _bcm_l3_bcm_ipv6_addr_to_sand_ipv6_addr(unit, info->l3a_ip6_net, &(route_key.subnet.ipv6_address));
  BCMDNX_IF_ERR_EXIT(ret);
  
  get_flags = SOC_PPC_FRWRD_IP_EXACT_MATCH;

  if (info->l3a_flags & BCM_L3_HIT_CLEAR) {
      get_flags |= SOC_PPC_FRWRD_IP_CLEAR_ON_GET;
  }

  ret = soc_ppd_frwrd_ipv6_uc_route_get(
          unit,
          &route_key,
          get_flags,
          &route_info,
          &route_status,
          &location,
          &found
        );

  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  { 
    LOG_ERROR(BSL_LS_BCM_L3,
              (BSL_META_U(unit,
                          "soc_sand error %x\n"), ret));
    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv6_uc_route_get failed")));
  }

  if (found == FALSE) {
    /* Entry not found */
    BCMDNX_ERR_EXIT_VERB_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("%s: Entry not found\n"), FUNCTION_NAME()));
  }

  /* Entry found */
  if (route_status & SOC_PPC_FRWRD_IP_ROUTE_STATUS_ACCESSED) {
      info->l3a_flags |= BCM_L3_HIT;
  }

  _bcm_l3_fec_to_intf(unit, route_info.dest_id, &(info->l3a_intf));

  *encap_id = 0;
  if (route_info.additional_info.eei.type == SOC_PPC_EEI_TYPE_OUTLIF) {
      /* Retrieve the encap-id for Host table */
      BCM_L3_ITF_SET(*encap_id, BCM_L3_ITF_TYPE_LIF, route_info.additional_info.eei.val.outlif); 
  }

  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}

/* Function _ppd_xxxx */
int
  _bcm_ppd_frwrd_ipv6_uc_route_remove(
    int unit,
    bcm_l3_route_t *info
  )
{
  uint32
    ret;
  
  SOC_PPC_FRWRD_IPV6_UC_ROUTE_KEY
    route_key;
  SOC_SAND_SUCCESS_FAILURE
    success;


  BCMDNX_INIT_FUNC_DEFS;
  SOC_PPC_FRWRD_IPV6_UC_ROUTE_KEY_clear(&route_key);

  route_key.subnet.prefix_len = bcm_ip6_mask_length(info->l3a_ip6_mask);
  ret = _bcm_l3_bcm_ipv6_addr_to_sand_ipv6_addr(unit, info->l3a_ip6_net, &(route_key.subnet.ipv6_address));
  BCMDNX_IF_ERR_EXIT(ret);

  ret = soc_ppd_frwrd_ipv6_uc_route_remove(
          unit,
          &route_key,
          &success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  { 
    LOG_ERROR(BSL_LS_BCM_L3,
              (BSL_META_U(unit,
                          "soc_sand error %x\n"), ret));
    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv6_uc_route_remove failed")));
  }
  SOC_SAND_IF_FAIL_EXIT(success);

  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}

int
  _bcm_ppd_frwrd_ipv6_host_to_route(
    int unit,
    bcm_l3_host_t *info,
    bcm_l3_route_t *r_info
  )
{
  uint32 i;

  BCMDNX_INIT_FUNC_DEFS;

  bcm_l3_route_t_init(r_info);

  r_info->l3a_flags = info->l3a_flags;
  r_info->l3a_flags2 = info->l3a_flags2;
  r_info->l3a_vrf = info->l3a_vrf;
  sal_memcpy(&(r_info->l3a_ip6_net), &(info->l3a_ip6_addr), sizeof(bcm_ip6_t));
  for (i=0; i<16; i++) {
      r_info->l3a_ip6_mask[i] = 0xff;
  }
  r_info->l3a_intf = info->l3a_intf;

  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}

/* given bcm IPv6 route_t info calculates ppd route info */
STATIC int _bcm_ppd_ipv6_route_info_to_ppd_get(
    int                                 unit,
    bcm_l3_route_t                      *info,
    SOC_PPC_FRWRD_DECISION_INFO         *route_info,
    SOC_PPC_FEC_ID                      *fec_id,
    int                                 *eep_val,
    uint8                               is_host,
    bcm_if_t                            encap_id
)
{
  uint32
    soc_sand_rv;
  int
    force_tcam;

  BCMDNX_INIT_FUNC_DEFS;

  SOC_PPC_FRWRD_DECISION_INFO_clear(route_info);
  force_tcam = 0;

  /*in case of raw data payload mo enceding is needed*/
  if (info->l3a_flags2 & BCM_L3_FLAGS2_RAW_ENTRY) {
      route_info->dest_id = info->l3a_intf;
      route_info->flags |= _BCM_L3_FLAGS2_RAW_ENTRY;
  } else {
      /* Different payload processing if host or not */
      /* destination must be FEC */
      _bcm_l3_intf_to_fec(unit, info->l3a_intf, fec_id);
      SOC_PPD_FRWRD_DECISION_FEC_SET(unit, route_info, *fec_id, soc_sand_rv);
      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
  }
  
  if (is_host) {

      /* Check whether IPv6 should be entered in TCAM */
      if (SOC_IS_JERICHO(unit)) {
          force_tcam = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "l3_ipv6_uc_use_tcam", 0);
      }

      /* EEP is valid this host points to: FEC + EEP */
      if ((SOC_IS_JERICHO(unit) && BCM_L3_ITF_TYPE_IS_FEC(info->l3a_intf) && force_tcam == 0 && encap_id == 0)
          || (SOC_IS_JERICHO(unit) && BCM_L3_ITF_TYPE_IS_FEC(info->l3a_intf) && force_tcam == 1 && encap_id != 0)
          || (SOC_IS_ARADPLUS_AND_BELOW(unit) && BCM_L3_ITF_TYPE_IS_FEC(info->l3a_intf) && encap_id != 0)) {

          /* setting host frwrd decision */
          /* FEC + MAC --> forward decision = FEC + ARP (EEI) */
          route_info->additional_info.eei.type = SOC_PPC_EEI_TYPE_OUTLIF;

          _bcm_l3_encap_id_to_eep(unit, encap_id, eep_val);
          route_info->additional_info.eei.val.outlif = *eep_val;
      }
      else {
          if (SOC_IS_JERICHO(unit) && encap_id != 0) {
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("IPV6 host must get FEC and info->encap_id == 0")));
          }
          else {
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("IPV6 host must get FEC and info->encap_id != 0")));
          }
      }
  }

  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}

int
  _bcm_ppd_frwrd_ipv6_route_add(
    int unit,
    bcm_l3_route_t *r_info,
    uint8 is_host,
    bcm_if_t encap_id
  )
{
  uint32
    ret;
  SOC_PPC_FEC_ID
    fec_id=0;
  SOC_PPC_FRWRD_DECISION_INFO            
    route_info;
  int eep_val, rv;

  BCMDNX_INIT_FUNC_DEFS;

  rv = _bcm_ppd_ipv6_route_info_to_ppd_get(unit, r_info, &route_info, &fec_id, &eep_val, is_host, encap_id);
  BCMDNX_IF_ERR_EXIT(rv);

  if ((r_info->l3a_vrf == DPP_DEFAULT_VRF) && SOC_IS_ARADPLUS_AND_BELOW(unit)) {
      ret = _bcm_ppd_frwrd_ipv6_uc_route_add(unit, r_info, &route_info);
  } else {
      ret = _bcm_ppd_frwrd_ipv6_vrf_uc_route_add(unit, r_info, &route_info);
  }
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  { 
    LOG_ERROR(BSL_LS_BCM_L3,
              (BSL_META_U(unit,
                          "soc_sand error %x\n"), ret));
    BCMDNX_ERR_EXIT_MSG(ret, (_BSL_BCM_MSG("_bcm_ppd_frwrd_ipv6_vrf_uc_route_add failed")));
  }

  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}

int
  _bcm_ppd_frwrd_ipv6_route_get(
    int unit,
    bcm_l3_route_t *r_info,
    bcm_if_t *encap_id
  )
{
  int status;

  BCMDNX_INIT_FUNC_DEFS;

  if ((r_info->l3a_vrf == DPP_DEFAULT_VRF) && SOC_IS_ARADPLUS_AND_BELOW(unit)) {
      status = _bcm_ppd_frwrd_ipv6_uc_route_get(unit, r_info, encap_id);
  } else {
      status = _bcm_ppd_frwrd_ipv6_vrf_uc_route_get(unit, r_info, encap_id);
  }

  if (status != BCM_E_NONE && status != BCM_E_NOT_FOUND) {
      BCMDNX_IF_ERR_EXIT(status);
  }
  else if (status != BCM_E_NONE)
  {
      BCM_RETURN_VAL_EXIT(status);
  }
  
  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}

int
  _bcm_ppd_frwrd_ipv6_route_delete(
    int unit,
    bcm_l3_route_t *r_info
  )
{
  int status;
  bcm_if_t encap_id;

  BCMDNX_INIT_FUNC_DEFS;

  status = _bcm_ppd_frwrd_ipv6_route_get(unit, r_info, &encap_id);
  BCMDNX_IF_ERR_EXIT(status);    

  if ((r_info->l3a_vrf == DPP_DEFAULT_VRF) && SOC_IS_ARADPLUS_AND_BELOW(unit)) {
      status = _bcm_ppd_frwrd_ipv6_uc_route_remove(unit, r_info);
      BCMDNX_IF_ERR_EXIT(status);    
  } else {
      status = _bcm_ppd_frwrd_ipv6_vrf_uc_route_remove(unit, r_info);
      BCMDNX_IF_ERR_EXIT(status);    
  }

  BCM_EXIT;

exit:
  BCMDNX_FUNC_RETURN;
}

/* Function _ppd_xxxx */
int
  _bcm_ppd_frwrd_route_print_info(
    int unit,
    bcm_l3_route_t *info
  )
{
  int rv = BCM_E_NONE;

  BCMDNX_INIT_FUNC_DEFS;

  /* Print IPv6 Details */
  if (info->l3a_flags & BCM_L3_IP6){
      rv = _bcm_ppd_frwrd_ipv6_route_print_info(unit, info);
      BCMDNX_IF_ERR_EXIT(rv);
  }
  /* Print IPv4 Details */
  else {
    if((info->l3a_vrf == DPP_DEFAULT_VRF) && !((info->l3a_ip_mask == 0) && (SOC_IS_ARADPLUS_AND_BELOW(unit)))) {
      rv = _bcm_ppd_frwrd_ipv4_uc_route_print_info(unit, info);
      BCMDNX_IF_ERR_EXIT(rv);
    }
    else if (!((info->l3a_ip_mask == 0) && (SOC_IS_ARADPLUS_AND_BELOW(unit)))) {
      rv = _bcm_ppd_frwrd_ipv4_vrf_uc_route_print_info(unit, info);
      BCMDNX_IF_ERR_EXIT(rv);
    }
  }

  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}


/* Function _ppd_xxxx */
int
  _bcm_ppd_frwrd_ipv6_route_print_info(
    int unit,
    bcm_l3_route_t *info
  )
{

  bcm_if_t 
    encap_id = 0;
  uint8
    is_host = FALSE;
  SOC_PPC_FEC_ID
    fec_id = 0;
  SOC_PPC_FRWRD_DECISION_INFO            
    route_info;
  int eep_val, rv;

  BCMDNX_INIT_FUNC_DEFS;

  rv =  _bcm_ppd_ipv6_route_info_to_ppd_get(unit, info, &route_info, &fec_id, &eep_val, is_host, encap_id);
  BCMDNX_IF_ERR_EXIT(rv);

  cli_out("fec_id: %u\n\r",fec_id);

  if (info->l3a_flags & BCM_L3_HIT) {
    cli_out("Accessed: YES\n");
  }
  else {
    cli_out("Accessed: NO\n");
  }

  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}

/* Function _ppd_xxxx */
int
  _bcm_ppd_frwrd_ipv4_uc_route_print_info(
    int unit,
    bcm_l3_route_t *info
  )
{
  SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY
    route_key;
  SOC_PPC_FEC_ID
    fec_id;
  int
    rv;

  BCMDNX_INIT_FUNC_DEFS;

  rv = _bcm_ppd_ipv4_uc_route_info_to_ppd_get(unit, info, &route_key, &fec_id);
  BCMDNX_IF_ERR_EXIT(rv);

  cli_out("fec_id: %u\n\r",fec_id);

  if (info->l3a_flags & BCM_L3_HIT) {
    cli_out("Accessed: YES\n");
  }
  else {
    cli_out("Accessed: NO\n");
  }

  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}

/* Function _ppd_xxxx */
int
  _bcm_ppd_frwrd_ipv4_vrf_uc_route_print_info(
    int unit,
    bcm_l3_route_t *info
  )
{
  _dpp_l3_vrf_route_t
    route_key;
  SOC_PPC_FEC_ID
    fec_id;
  int
    rv;

  BCMDNX_INIT_FUNC_DEFS;

  rv = _bcm_ppd_ipv4_vrf_uc_route_info_to_ppd_get(unit, info, &route_key);
  BCMDNX_IF_ERR_EXIT(rv);

  _bcm_l3_intf_to_fec(unit, info->l3a_intf, &fec_id);

  cli_out("fec_id: %u\n\r",fec_id);

  if (info->l3a_flags & BCM_L3_HIT) {
    cli_out("Accessed: YES\n");
  }
  else {
    cli_out("Accessed: NO\n");
  }

  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}

/* Function _ppd_xxxx */
int
  _bcm_ppd_frwrd_host_print_info(
    int unit,
    bcm_l3_host_t *info
  )
{
  int rv = BCM_E_NONE;
  bcm_l3_route_t r_info;

  BCMDNX_INIT_FUNC_DEFS;

  /* Print IPv6 Details */
  if (info->l3a_flags & BCM_L3_IP6){
    rv = _bcm_ppd_frwrd_ipv6_host_to_route(unit, info, &r_info);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_ppd_frwrd_ipv6_route_print_info(unit, &r_info);
    BCMDNX_IF_ERR_EXIT(rv);
  }
  /* Print IPv4 Details */
  else {
    rv = _bcm_ppd_frwrd_ipv4_host_print_info(unit, info);
    BCMDNX_IF_ERR_EXIT(rv);
  }

  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}

/* Function _ppd_xxxx */
int
  _bcm_ppd_frwrd_ipv4_host_print_info(
    int unit,
    bcm_l3_host_t *info
  )
{
  SOC_PPC_FRWRD_IPV4_HOST_KEY
    host_key;
  SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO
    routing_info;
  int rv = BCM_E_NONE;

  BCMDNX_INIT_FUNC_DEFS;

  rv = _bcm_ppd_ipv4_host_info_to_ppd_get(unit, info, &routing_info);
  BCMDNX_IF_ERR_EXIT(rv);

  rv = _bcm_ppd_ipv4_host_key_to_ppd_get(unit, info, &host_key);
  BCMDNX_IF_ERR_EXIT(rv);

  SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO_print(&routing_info);

  if (info->l3a_flags & BCM_L3_HIT) {
    cli_out("Accessed: YES\n");
  }
  else {
    cli_out("Accessed: NO\n");
  }

  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}

/* Function _ppd_xxxx */
int
  _bcm_ppd_frwrd_ipv4_host_get(
    int unit,
    bcm_l3_host_t *info
  )
{
  uint32
    ret;
  
  SOC_PPC_FRWRD_IPV4_HOST_KEY
    host_key;
  SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO
    routing_info;
  SOC_PPC_FRWRD_IP_ROUTE_STATUS
    route_status;
  SOC_PPC_FRWRD_IP_ROUTE_LOCATION
    location;
  uint32 host_flags=0;
  uint8
    found;
  int rv = BCM_E_NONE;


  BCMDNX_INIT_FUNC_DEFS;

#if defined(INCLUDE_KBP)
      if(ARAD_KBP_ENABLE_ANY_IPV4UC_PROGRAM && ARAD_KBP_24BIT_FWD && !(info->l3a_flags & BCM_L3_HOST_LOCAL)) {
          BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_l3_host_find is not supported for 24bit FWD; Use bcm_l3_route_get instead")));
      }

      if(ARAD_KBP_ENABLE_IPV4_UC_PUBLIC && (info->l3a_vrf == 0) && (info->l3a_flags & BCM_L3_HOST_LOCAL)){
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("bcm_l3_host_find is not supported for search public entry from the LEM when public KBP enabled")));
      }
#endif

  SOC_PPC_FRWRD_IPV4_HOST_KEY_clear(&host_key);
  SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO_clear(&routing_info);

  host_key.ip_address = info->l3a_ip_addr;
  host_key.vrf_ndx = info->l3a_vrf;

  if(info->l3a_flags2 &  BCM_L3_FLAGS2_RAW_ENTRY) {
      routing_info.frwrd_decision.flags |= _BCM_L3_FLAGS2_RAW_ENTRY;
  }

  if (info->l3a_flags & BCM_L3_HIT_CLEAR) {
      host_flags  = SOC_PPC_FRWRD_IP_HOST_CLEAR_ON_GET;
  }

  if(info->l3a_flags &  BCM_L3_HOST_LOCAL){
      host_key.is_local_host = 1;
  }

  if(info->l3a_flags2 &  BCM_L3_FLAGS2_SCALE_ROUTE){
      if (SOC_IS_JERICHO(unit)) {
          host_key.route_scale = 1;
      }else{
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_L3_FLAGS2_SCALE_ROUTE supported from Jericho and above")));
      }
  }

  ret = soc_ppd_frwrd_ipv4_host_get(
          unit,
          &host_key,
          host_flags,
          &routing_info,
          &route_status,
          &location,
          &found
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  { 
    LOG_ERROR(BSL_LS_BCM_L3,
              (BSL_META_U(unit,
                          "soc_sand error %x\n"), ret));
    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv4_host_get failed")));
  }

  if (found == FALSE) {
    /* Entry not found */
    BCMDNX_ERR_EXIT_VERB_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("%s: Entry not found\n"), FUNCTION_NAME()));
  }

  rv = _bcm_ppd_host_info_from_ppd_get(unit,&routing_info,&route_status,info);
  BCMDNX_IF_ERR_EXIT(rv);

exit:
  BCMDNX_FUNC_RETURN;
}

/* Function _ppd_xxxx */
int
  _bcm_ppd_frwrd_ipv4_host_remove(
    int unit,
    bcm_l3_host_t *info
  )
{
  uint32
    ret;
  
  SOC_PPC_FRWRD_IPV4_HOST_KEY
    host_key;

  BCMDNX_INIT_FUNC_DEFS;
  SOC_PPC_FRWRD_IPV4_HOST_KEY_clear(&host_key);

  host_key.ip_address = info->l3a_ip_addr;
  host_key.vrf_ndx = info->l3a_vrf;

  if(info->l3a_flags &  BCM_L3_HOST_LOCAL){
      host_key.is_local_host = 1;
  }

  if(info->l3a_flags2 &  BCM_L3_FLAGS2_SCALE_ROUTE){
      if (SOC_IS_JERICHO(unit)) {
          host_key.route_scale = 1;
      }else{
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_L3_FLAGS2_SCALE_ROUTE supported from Jericho and above")));
      }
  }

  ret = soc_ppd_frwrd_ipv4_host_remove(
          unit,
          &host_key
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  { 
    LOG_ERROR(BSL_LS_BCM_L3,
              (BSL_META_U(unit,
                          "soc_sand error %x\n"), ret));
    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv4_host_remove failed")));
  }

  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}



/* Common functions to handle ipv4 vrf route (UC and MC) */
/* Function _ppd_xxxx */
int
  _bcm_ppd_frwrd_ipv4_vrf_route_add(
    int unit,
    _dpp_l3_vrf_route_t *info
  )
{
  uint32
    soc_sand_rv;
  
  SOC_PPC_VRF_ID
    vrf_ndx;
  SOC_PPC_FEC_ID
    fec_id;
  SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY
    route_key;
  SOC_SAND_SUCCESS_FAILURE
    success = SOC_SAND_SUCCESS;

  BCMDNX_INIT_FUNC_DEFS;
  SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY_clear(&route_key);

  if(info->l3a_flags2 &  BCM_L3_FLAGS2_SCALE_ROUTE){
      if (SOC_IS_JERICHO(unit)) {
          route_key.route_scale = 1;
      }else{
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_L3_FLAGS2_SCALE_ROUTE supported from Jericho and above")));
      }
  }

  /* Route key */
  route_key.subnet.ip_address = info->vrf_subnet;
  route_key.subnet.prefix_len = bcm_ip_mask_length(info->vrf_ip_mask);

  vrf_ndx   = info->vrf_id;
  _bcm_l3_intf_to_fec(unit, info->vrf_intf, &fec_id);

  /* in case the payload is raw data */
  if (info->l3a_flags2 &  BCM_L3_FLAGS2_RAW_ENTRY) {
      soc_sand_rv = soc_ppd_frwrd_ipv4_vrf_route_add(unit, 
                                         vrf_ndx,
                                         &route_key,
                                         info->vrf_intf,
                                         _BCM_L3_FLAGS2_RAW_ENTRY,
                                         &success);
  } else {
      soc_sand_rv = soc_ppd_frwrd_ipv4_vrf_route_add(unit, 
                                         vrf_ndx,
                                         &route_key,
                                         fec_id,
                                         0, /*flags*/
                                         &success);
  }

  if (success == SOC_SAND_FAILURE_OUT_OF_RESOURCES)
  {
      /* Out of resources */
      LOG_ERROR(BSL_LS_BCM_L3,
                (BSL_META_U(unit,
                            "Table is out of resources\n")));
      BCM_RETURN_VAL_EXIT(BCM_E_FULL);
  }

  BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
  SOC_SAND_IF_FAIL_EXIT(success);

  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}
int
  _bcm_ppd_frwrd_ipv4_vrf_route_get(
    int unit,
    _dpp_l3_vrf_route_t *info
  )
{
   uint32
    soc_sand_rv;
  
  SOC_PPC_VRF_ID
    vrf_ndx;
  SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY
    route_key;
  uint8
    get_flags,
    found;
  SOC_PPC_FEC_ID
    fec_id;
  SOC_PPC_FRWRD_IP_ROUTE_STATUS
    route_status;
  SOC_PPC_FRWRD_IP_ROUTE_LOCATION
    location;
  uint32
    flags = 0;

  BCMDNX_INIT_FUNC_DEFS;
  SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY_clear(&route_key);

  if(info->l3a_flags2 &  BCM_L3_FLAGS2_SCALE_ROUTE){
      if (SOC_IS_JERICHO(unit)) {
          route_key.route_scale = 1;
      }else{
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_L3_FLAGS2_SCALE_ROUTE supported from Jericho and above")));
      }
  }
  if(info->l3a_flags2 &  BCM_L3_FLAGS2_RAW_ENTRY) {
      flags |= _BCM_L3_FLAGS2_RAW_ENTRY;
  }

  /* Route key */
  route_key.subnet.ip_address = info->vrf_subnet;
  route_key.subnet.prefix_len = bcm_ip_mask_length(info->vrf_ip_mask);
 
  vrf_ndx   = info->vrf_id;

  get_flags = TRUE;

  if (info->l3a_flags & BCM_L3_HIT_CLEAR) {
      get_flags |= SOC_PPC_FRWRD_IP_CLEAR_ON_GET;
  }

  soc_sand_rv = soc_ppd_frwrd_ipv4_vrf_route_get(unit,
                                         vrf_ndx,
                                         &route_key,
                                         get_flags,
                                         flags,
                                         &fec_id,
                                         &route_status,
                                         &location,
                                         &found);
  BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

  if (found == FALSE) {
    /* Entry not found */
    BCM_RETURN_VAL_EXIT(BCM_E_NOT_FOUND);
  } 

  /* Entry found */
  if (info->l3a_flags2 & BCM_L3_FLAGS2_RAW_ENTRY) {
      /* in case of raw data entry no decoding in needed*/
      info->vrf_intf = fec_id;
  } else {
      _bcm_l3_fec_to_intf(unit, fec_id, &(info->vrf_intf));
  }

  if (route_status & SOC_PPC_FRWRD_IP_ROUTE_STATUS_ACCESSED) {
      info->l3a_flags |= BCM_L3_HIT;
  }

  BCM_EXIT;        
exit:
  BCMDNX_FUNC_RETURN;
}

int
  _bcm_ppd_frwrd_ipv4_vrf_route_remove(
    int unit,
    _dpp_l3_vrf_route_t *info
  )
{
  uint32
    soc_sand_rv;
  
  SOC_PPC_VRF_ID
    vrf_ndx;
  SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY
    route_key;
  SOC_SAND_SUCCESS_FAILURE
    success;

  BCMDNX_INIT_FUNC_DEFS;
  SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY_clear(&route_key);

  if(info->l3a_flags2 &  BCM_L3_FLAGS2_SCALE_ROUTE){
      if (SOC_IS_JERICHO(unit)) {
          route_key.route_scale = 1;
      }else{
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_L3_FLAGS2_SCALE_ROUTE supported from Jericho and above")));
      }
  }

  /* Route key */
  route_key.subnet.ip_address = info->vrf_subnet;
  route_key.subnet.prefix_len = bcm_ip_mask_length(info->vrf_ip_mask);
 
  vrf_ndx   = info->vrf_id;

  soc_sand_rv = soc_ppd_frwrd_ipv4_vrf_route_remove(unit,vrf_ndx,&route_key,&success);
  BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

  if (success != SOC_SAND_SUCCESS) {
        /* Internal error */
        LOG_ERROR(BSL_LS_BCM_L3,
                  (BSL_META_U(unit,
                              "remove failed\n")));
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv4_vrf_route_remove failed")));
  }

  BCM_EXIT;         
exit:
  BCMDNX_FUNC_RETURN;
}


/* IPV4 VRF UC route functions */ 
/* Function _ppd_xxxx */
int
  _bcm_ppd_frwrd_ipv4_vrf_uc_route_add(
    int unit,
    bcm_l3_route_t *info
  )
{ 
  bcm_error_t
    rv;
  _dpp_l3_vrf_route_t
    *vrf_route = NULL;
    
  BCMDNX_INIT_FUNC_DEFS;

  BCMDNX_ALLOC(vrf_route, sizeof(_dpp_l3_vrf_route_t), "vrf_route");
  if (vrf_route == NULL) {        
      BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
  }

  rv = _bcm_ppd_ipv4_vrf_uc_route_info_to_ppd_get(unit, info, vrf_route);

  /* Common function for UC and MC route add */
  rv = _bcm_ppd_frwrd_ipv4_vrf_route_add(unit, vrf_route);
  BCMDNX_IF_ERR_EXIT(rv);

exit:
  BCM_FREE(vrf_route);
  BCMDNX_FUNC_RETURN;
}

int
  _bcm_ppd_frwrd_ipv4_vrf_uc_route_get(
    int unit,
    bcm_l3_route_t *info
  )
{
  bcm_error_t
    rv;
  _dpp_l3_vrf_route_t
    vrf_route;
    
  BCMDNX_INIT_FUNC_DEFS;
  __dpp_l3_vrf_route_t_init(&vrf_route);

  /* Route key */
  rv = _bcm_ppd_ipv4_vrf_uc_route_info_to_ppd_get(unit, info, &vrf_route);

  /* Common function for UC and MC route remove */
  rv = _bcm_ppd_frwrd_ipv4_vrf_route_get(unit, &vrf_route);

  if ((rv != BCM_E_NONE) && (rv != BCM_E_NOT_FOUND)) {
    BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("get vrf route failed")));
  }
  else if (rv == BCM_E_NOT_FOUND) {
      BCM_RETURN_VAL_EXIT(rv);
  }

  /* Entry found */
  info->l3a_flags = vrf_route.l3a_flags;
  info->l3a_intf = vrf_route.vrf_intf;

  
  /* Get information for other fields in l3 info from FEC */

exit:
  BCMDNX_FUNC_RETURN;
}


/* Function _ppd_xxxx */
int
  _bcm_ppd_frwrd_ipv4_vrf_uc_route_remove(
    int unit,
    bcm_l3_route_t *info
  )
{ 
  bcm_error_t
    rv;
  _dpp_l3_vrf_route_t
    vrf_route;
    
  BCMDNX_INIT_FUNC_DEFS;
  __dpp_l3_vrf_route_t_init(&vrf_route);

  /* Route key */
  rv = _bcm_ppd_ipv4_vrf_uc_route_info_to_ppd_get(unit, info, &vrf_route);

  /* Common function for UC and MC route remove */
  rv = _bcm_ppd_frwrd_ipv4_vrf_route_remove(unit,&vrf_route);
  BCMDNX_IF_ERR_EXIT(rv);

exit:
  BCMDNX_FUNC_RETURN;
}


/***************************************************************/
/***************************************************************/
 
/*
 * IPv6 Helper functions
 */

int
  _bcm_ppd_frwrd_ipv6_routing_table_clear(
    int unit
  )
{
  uint32 ret;

  BCMDNX_INIT_FUNC_DEFS;

  ret = soc_ppd_frwrd_ipv6_uc_routing_table_clear(
          unit
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  { 
    LOG_ERROR(BSL_LS_BCM_L3,
              (BSL_META_U(unit,
                          "soc_sand error %x\n"), ret));
    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv6_uc_routing_table_clear failed")));
  }

  ret = soc_ppd_frwrd_ipv6_vrf_all_routing_tables_clear(unit);
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    {
      LOG_ERROR(BSL_LS_BCM_L3,
                (BSL_META_U(unit,
                            "soc_sand error %x\n"), ret));
      BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv6_vrf_all_routing_tables_clear failed")));
    }

  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}

int
  _bcm_ppd_frwrd_ipv6_route_traverse(
    int unit,
    uint32 flags,
    bcm_l3_route_traverse_cb cb,
    void *user_data
  )
{
    SOC_DPP_DBAL_SW_TABLE_IDS table_ids[] = {
        SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE,
        SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE_SCALE_LONG_KAPS,
        SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE_SCALE_SHORT_KAPS,
        SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_FWD_KBP,
        SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_KBP_DEFAULT_ROUTE_KAPS
    };
    uint32 tables_num = sizeof(table_ids)/sizeof(SOC_DPP_DBAL_SW_TABLE_IDS);
    uint32
      ret;
    int i, qual_idx, is_table_initiated, idx=0;
    SOC_PPC_FP_QUAL_TYPE qual_type;
    SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    uint32 kaps_payload = 0;
    SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO kbp_payload;
    void *payload = NULL;
    uint8 found;
    bcm_l3_route_t entry;
    SOC_SAND_PP_IPV6_ADDRESS ipv6_addr, ipv6_mask;
    SOC_DPP_DBAL_PHYSICAL_DB_TYPES physical_db_type;
    uint32 soc_flags = 0;
    uint8 hit_bit = 0;

    BCMDNX_INIT_FUNC_DEFS;

    if (flags & BCM_L3_HIT_CLEAR) {
        if (!SOC_IS_JERICHO_PLUS(unit)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("BCM_L3_HIT_CLEAR only available in Jericho plus and above.")));
        }

        soc_flags = SOC_PPC_FRWRD_IP_CLEAR_ON_GET;
    }

    for (i=0; i<tables_num; i++) {
        ret = soc_ppd_frwrd_ipv4_uc_route_iterator_table_is_initiated(unit, table_ids[i], &is_table_initiated);
        if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
        {
          LOG_ERROR(BSL_LS_BCM_L3,
                    (BSL_META_U(unit,
                                "soc_sand error %x\n"), ret));
          BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv4_uc_route_iterator_table_is_initiated failed")));
        }

        if (is_table_initiated) {
            ret = soc_ppd_frwrd_ipv4_uc_route_iterator_init(unit, table_ids[i], &physical_db_type);
            if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
            {
              LOG_ERROR(BSL_LS_BCM_L3,
                        (BSL_META_U(unit,
                                    "soc_sand error %x\n"), ret));
              BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv4_uc_route_iterator_init failed")));
            }

            /* Per Physical DB payload */
            if (physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS) {
                payload = &kaps_payload;
            } else if (physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP) {
                payload = &kbp_payload;
            }

            ret = soc_ppd_frwrd_ipv4_uc_route_iterator_get_next(unit, table_ids[i], soc_flags, qual_vals, payload, NULL/*priority*/, &hit_bit, &found);
            if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
            {
              LOG_ERROR(BSL_LS_BCM_L3,
                        (BSL_META_U(unit,
                                    "soc_sand error %x\n"), ret));
              BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv4_uc_route_iterator_get_next failed")));
            }

            while (found==TRUE) {
                bcm_l3_route_t_init(&entry);

                sal_memset(&ipv6_addr,0x0,sizeof(SOC_SAND_PP_IPV6_ADDRESS));
                sal_memset(&ipv6_mask,0x0,sizeof(SOC_SAND_PP_IPV6_ADDRESS));
                /* From qual_Vals to entry */
                for (qual_idx = 0; qual_idx < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; qual_idx++) {
                    qual_type = qual_vals[qual_idx].type;
                    if ((qual_type == SOC_PPC_NOF_FP_QUAL_TYPES) || (qual_type == BCM_FIELD_ENTRY_INVALID)) {
                        break;
                    }
                    if (qual_type == SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_LOW) {
                        ipv6_addr.address[0] = qual_vals[qual_idx].val.arr[0];
                        ipv6_addr.address[1] = qual_vals[qual_idx].val.arr[1];
                        ipv6_mask.address[0] = qual_vals[qual_idx].is_valid.arr[0];
                        ipv6_mask.address[1] = qual_vals[qual_idx].is_valid.arr[1];

                    }
                    if (qual_type == SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH) {
                        ipv6_addr.address[2] = qual_vals[qual_idx].val.arr[0];
                        ipv6_addr.address[3] = qual_vals[qual_idx].val.arr[1];
                        ipv6_mask.address[2] = qual_vals[qual_idx].is_valid.arr[0];
                        ipv6_mask.address[3] = qual_vals[qual_idx].is_valid.arr[1];
                    }
                    if (qual_type == SOC_PPC_FP_QUAL_IRPP_VRF) {
                        entry.l3a_vrf = qual_vals[qual_idx].val.arr[0];
                    }
                }
                ret = _bcm_l3_sand_ipv6_addr_to_bcm_ipv6_addr(unit, &ipv6_addr, &(entry.l3a_ip6_net));
                BCMDNX_IF_ERR_EXIT(ret);
                ret = _bcm_l3_sand_ipv6_addr_to_bcm_ipv6_addr(unit, &ipv6_mask, &(entry.l3a_ip6_mask));
                BCMDNX_IF_ERR_EXIT(ret);

                if ((table_ids[i] == SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE) ||
                    (table_ids[i] == SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE_SCALE_LONG_KAPS) || 
                    (table_ids[i] == SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE_SCALE_SHORT_KAPS) ||
                    (table_ids[i] == SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_FWD_KBP) ||
                    (table_ids[i] == SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_KBP_DEFAULT_ROUTE_KAPS)) {
                    entry.l3a_flags |= BCM_L3_IP6;
                }

                if ((table_ids[i] == SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE_SCALE_LONG_KAPS) || (table_ids[i] == SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE_SCALE_SHORT_KAPS)) {
                    entry.l3a_flags2 |= BCM_L3_FLAGS2_SCALE_ROUTE;
                }

                if (hit_bit & 1) {
                    entry.l3a_flags |= BCM_L3_HIT;
                }

                /* Per Physical DB payload */
                if (physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS) {
                    if (BCM_L3_ITF_TYPE_IS_FEC(kaps_payload)) {
                        entry.l3a_intf = kaps_payload;
                    } else {
                        entry.l3a_intf = 0;
                    }
                } else if (physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP) {
					_bcm_l3_fec_to_intf(unit, kbp_payload.fec_id, &(entry.l3a_intf));
                }

                (*cb)(unit, idx, &entry, user_data);
                idx++;

                ret = soc_ppd_frwrd_ipv4_uc_route_iterator_get_next(unit, table_ids[i], soc_flags, qual_vals, payload, NULL/*priority*/, &hit_bit, &found);
                if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
                {
                  LOG_ERROR(BSL_LS_BCM_L3,
                            (BSL_META_U(unit,
                                        "soc_sand error %x\n"), ret));
                  BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv4_uc_route_iterator_get_next failed")));
                }
            }

            ret = soc_ppd_frwrd_ipv4_uc_route_iterator_deinit(unit, table_ids[i]);
            if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
            {
              LOG_ERROR(BSL_LS_BCM_L3,
                        (BSL_META_U(unit,
                                    "soc_sand error %x\n"), ret));
              BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv4_uc_route_iterator_deinit failed")));
            }
        }
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
  _bcm_ppd_frwrd_ipv6_route_get_block(
    int unit,
    uint32 mode,
    bcm_if_t interface,
    bcm_l3_route_traverse_cb cb,
    void *user_data
  )
{  
    uint32
      ret;
    
    SOC_PPC_VRF_ID
      vrf_ndx = BCM_L3_VRF_DEFAULT;
    SOC_PPC_IP_ROUTING_TABLE_RANGE
      block_range;
    SOC_PPC_FRWRD_IPV6_UC_ROUTE_KEY *route_keys = NULL;
    SOC_PPC_FRWRD_IPV6_VPN_ROUTE_KEY *vpn_route_keys = NULL;
    SOC_PPC_FRWRD_DECISION_INFO
      *route_infos = NULL;
    SOC_PPC_FRWRD_IP_ROUTE_STATUS
      routes_status[DPP_FRWRD_IP_GET_BLOCK_NOF_ENTRIES];
    SOC_PPC_FRWRD_IP_ROUTE_LOCATION
      routes_location[DPP_FRWRD_IP_GET_BLOCK_NOF_ENTRIES];
    uint32          nof_entries;
    bcm_l3_route_t  entry;
    bcm_ip6_t       ip6_mask;
    int idx, rv = 0;

    BCMDNX_INIT_FUNC_DEFS;

    SOC_PPC_IP_ROUTING_TABLE_RANGE_clear(&block_range);

    BCMDNX_ALLOC(route_keys, 
	      sizeof(SOC_PPC_FRWRD_IPV6_UC_ROUTE_KEY) * DPP_FRWRD_IP_GET_BLOCK_NOF_ENTRIES, 
	      "UC Route Key Temp Storage");
    if (route_keys) {
        sal_memset(route_keys, 0, sizeof(SOC_PPC_FRWRD_IPV6_UC_ROUTE_KEY) * DPP_FRWRD_IP_GET_BLOCK_NOF_ENTRIES);
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("UC Route Keys Storage Unavailable")));
    }

    BCMDNX_ALLOC(vpn_route_keys, 
	      sizeof(SOC_PPC_FRWRD_IPV6_VPN_ROUTE_KEY) * DPP_FRWRD_IP_GET_BLOCK_NOF_ENTRIES, 
	      "VPN Route Key Temp Storage");
    if (vpn_route_keys) {
        sal_memset(vpn_route_keys, 0, sizeof(SOC_PPC_FRWRD_IPV6_VPN_ROUTE_KEY) * DPP_FRWRD_IP_GET_BLOCK_NOF_ENTRIES);
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("VPN Route Keys Storage Unavailable")));
    }

    block_range.entries_to_act = DPP_FRWRD_IP_GET_BLOCK_NOF_ENTRIES;
    block_range.entries_to_scan = DPP_FRWRD_IP_ENTRIES_TO_SCAN;
    block_range.start.type = SOC_PPC_IP_ROUTING_TABLE_ITER_TYPE_PREFIX_IP_ORDERED;
    /* SOC_PPC_IP_ROUTING_TABLE_ITER_BEGIN(block_range.start.payload); */
    soc_sand_u64_clear(&block_range.start.payload);

    BCMDNX_ALLOC(route_infos, sizeof(SOC_PPC_FRWRD_DECISION_INFO) * DPP_FRWRD_IP_GET_BLOCK_NOF_ENTRIES, "_bcm_ppd_frwrd_ipv6_route_get_block.route_infos");
    if (route_infos == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
    }
    for (vrf_ndx=BCM_L3_VRF_DEFAULT; vrf_ndx<SOC_DPP_CONFIG(unit)->l3.max_nof_vrfs; vrf_ndx++)
    {
      soc_sand_u64_clear(&block_range.start.payload);
      while(!SOC_PPC_IP_ROUTING_TABLE_ITER_IS_END(&block_range.start.payload))
      {
        nof_entries = 0;
        if (vrf_ndx == BCM_L3_VRF_DEFAULT)
        {
          ret = soc_ppd_frwrd_ipv6_uc_route_get_block(unit,
                                                  &block_range,
                                                  route_keys,
                                                  route_infos,
                                                  routes_status,
                                                  routes_location,
                                                  &nof_entries);
        } else {
          ret = soc_ppd_frwrd_ipv6_vrf_route_get_block(unit,
                                                   vrf_ndx,
                                                   &block_range,
                                                   vpn_route_keys,
                                                   route_infos,
                                                   routes_status,
                                                   routes_location,
                                                   &nof_entries);
        }

        if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
        {
          LOG_ERROR(BSL_LS_BCM_L3,
                    (BSL_META_U(unit,
                                "soc_sand error %x\n"), ret));
          BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv6_vrf_route_get_block failed")));
        }

        if(nof_entries == 0)
        {
            break;
        }

        for(idx = 0; idx < nof_entries; idx++)
        {
          bcm_l3_route_t_init(&entry);

          if (vrf_ndx == BCM_L3_VRF_DEFAULT)
          {
              entry.l3a_vrf     = BCM_L3_VRF_DEFAULT;
          rv = _bcm_l3_sand_ipv6_addr_to_bcm_ipv6_addr(unit,
                               &(route_keys[idx].subnet.ipv6_address),
                               &(entry.l3a_ip6_net));
          BCMDNX_IF_ERR_EXIT(rv);
          rv = bcm_ip6_mask_create(ip6_mask, route_keys[idx].subnet.prefix_len);
          if (rv == BCM_E_PARAM){   /* prefix_len 0 is a legitimate value, but bcm_ip6_mask_create reutrns BCM_E_PARAM when getting it as argument. */
              rv = BCM_E_NONE;
          }
          BCMDNX_IF_ERR_EXIT(rv);
          sal_memcpy(&(entry.l3a_ip6_mask),&(ip6_mask),sizeof(bcm_ip6_t));
          } else {
              entry.l3a_vrf     = vrf_ndx;
          rv = _bcm_l3_sand_ipv6_addr_to_bcm_ipv6_addr(unit,
                               &(vpn_route_keys[idx].subnet.ipv6_address),
                               &(entry.l3a_ip6_net));
          BCMDNX_IF_ERR_EXIT(rv);
          rv = bcm_ip6_mask_create(ip6_mask, vpn_route_keys[idx].subnet.prefix_len);
          BCMDNX_IF_ERR_EXIT(rv);
          sal_memcpy(&(entry.l3a_ip6_mask),&(ip6_mask),sizeof(bcm_ip6_t));
          }
          _bcm_l3_fec_to_intf(unit, route_infos[idx].dest_id, &(entry.l3a_intf));


          switch(mode)
          {
            case DPP_L3_TRAVERSE_CB:
            {
              /* Invoke user callback. */
              entry.l3a_flags |= BCM_L3_IP6; /*Necessary flag for count callback*/
              (*cb)(unit, idx, &entry, user_data);
              break;
            }

            case DPP_L3_TRAVERSE_DELETE_ALL:
            {
              rv = bcm_petra_l3_route_delete(unit, &entry);
              BCMDNX_IF_ERR_EXIT(rv);
              break;
            }

            case DPP_L3_TRAVERSE_DELETE_INTERFACE:
            {
              LOG_ERROR(BSL_LS_BCM_L3,
                  (BSL_META_U(unit,
                              "DPP_L3_TRAVERSE_DELETE_INTERFACE not implemented\n")));
              BCM_RETURN_VAL_EXIT(BCM_E_UNAVAIL);
              break;
            }

            default:
            {
              LOG_ERROR(BSL_LS_BCM_L3,
                        (BSL_META_U(unit,
                                    "illegal traverse mode %x\n"), mode));
              BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Invalid mode")));
            }
          }
        }
      }
    }

    BCM_EXIT;
exit:
    BCM_FREE(route_keys);
    BCM_FREE(vpn_route_keys);
    BCM_FREE(route_infos);
    BCMDNX_FUNC_RETURN;
}


/* Common functions to handle ipv6 vrf route (UC and MC) */
int
  _bcm_ppd_frwrd_ipv6_vrf_route_add(
    int unit,
    _dpp_l3_vrf_route_t *info,
    SOC_PPC_FRWRD_DECISION_INFO *route_info
  )
{
  bcm_error_t
    rv;
  uint32
    soc_sand_rv;
  
  SOC_PPC_VRF_ID
    vrf_ndx;
  SOC_PPC_FRWRD_IPV6_VPN_ROUTE_KEY
    route_key;
  SOC_SAND_SUCCESS_FAILURE
    success;

  BCMDNX_INIT_FUNC_DEFS;
  SOC_PPC_FRWRD_IPV6_VPN_ROUTE_KEY_clear(&route_key);

  /* Route key */
  route_key.subnet.prefix_len = bcm_ip6_mask_length(info->vrf_ip6_mask);
  rv = _bcm_l3_bcm_ipv6_addr_to_sand_ipv6_addr(unit, info->vrf_ip6_net, &(route_key.subnet.ipv6_address));
  BCMDNX_IF_ERR_EXIT(rv);
  if (info->l3a_flags2 &  BCM_L3_FLAGS2_SCALE_ROUTE) {
      route_key.route_scale = 1;
  }

  vrf_ndx   = info->vrf_id;

  soc_sand_rv = soc_ppd_frwrd_ipv6_vrf_route_add(unit,
                                         vrf_ndx,
                                         &route_key,
                                         route_info,
                                         &success);

  if (success == SOC_SAND_FAILURE_OUT_OF_RESOURCES)
  {
      /*Out of resources*/
      LOG_ERROR(BSL_LS_BCM_L3,
                (BSL_META_U(unit,
                            "Table is out of resources\n")));
      BCM_RETURN_VAL_EXIT(BCM_E_FULL);
  }
  BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

  BCM_EXIT; 
exit:
  BCMDNX_FUNC_RETURN;
}

int
  _bcm_ppd_frwrd_ipv6_vrf_route_remove(
    int unit,
    _dpp_l3_vrf_route_t *info
  )
{
    bcm_error_t
    rv;
  uint32
    soc_sand_rv;
  
  SOC_PPC_VRF_ID
    vrf_ndx;
  SOC_PPC_FRWRD_IPV6_VPN_ROUTE_KEY
    route_key;
  SOC_SAND_SUCCESS_FAILURE
    success;

  BCMDNX_INIT_FUNC_DEFS;
  SOC_PPC_FRWRD_IPV6_VPN_ROUTE_KEY_clear(&route_key);

  /* Route key */
  route_key.subnet.prefix_len = bcm_ip6_mask_length(info->vrf_ip6_mask);
  rv = _bcm_l3_bcm_ipv6_addr_to_sand_ipv6_addr(unit, info->vrf_ip6_net, &(route_key.subnet.ipv6_address));
  BCMDNX_IF_ERR_EXIT(rv);
  if (info->l3a_flags2 &  BCM_L3_FLAGS2_SCALE_ROUTE) {
      route_key.route_scale = 1;
  }

  vrf_ndx   = info->vrf_id;

  soc_sand_rv = soc_ppd_frwrd_ipv6_vrf_route_remove(unit,vrf_ndx,&route_key,&success);
  BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

  if (success != SOC_SAND_SUCCESS) {
        /* Internal error */
        LOG_ERROR(BSL_LS_BCM_L3,
                  (BSL_META_U(unit,
                              "remove failed\n")));
        BCM_RETURN_VAL_EXIT(BCM_E_INTERNAL);
  }

exit:
  BCMDNX_FUNC_RETURN;
}


/***************************************************************/
/***************************************************************/

/*
 *
 *    Aux function used by bcm_petra_l3_egress_traverse
 */

int
  _bcm_ppd_frwrd_fec_get_block(
    int unit,
    uint32 mode,
    bcm_if_t interface,
    bcm_l3_egress_traverse_cb cb,
    void *user_data
  )
{
    uint32
      ret;
    
    SOC_PPC_FRWRD_FEC_MATCH_RULE
      rule;
    SOC_SAND_TABLE_BLOCK_RANGE
      block_range;
    SOC_PPC_FEC_ID
      fec_ids[DPP_FRWRD_IP_GET_BLOCK_NOF_ENTRIES];
    uint32
      nof_entries;
    bcm_l3_egress_t
      entry;
    int idx;

    BCMDNX_INIT_FUNC_DEFS;

    SOC_PPC_FRWRD_FEC_MATCH_RULE_clear(&rule);
    soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&block_range);

    block_range.entries_to_act = DPP_FRWRD_IP_GET_BLOCK_NOF_ENTRIES;
    block_range.entries_to_scan = DPP_FRWRD_IP_ENTRIES_TO_SCAN;
    SOC_SAND_TBL_ITER_SET_BEGIN(&block_range.iter);

    rule.type = SOC_PPC_FRWRD_FEC_MATCH_RULE_TYPE_ALL;
    rule.value = 0;

    while(!SOC_SAND_TBL_ITER_IS_END(&(block_range.iter)))
    {
        ret = soc_ppd_frwrd_fec_get_block(unit,
                                      &rule,
                                      &block_range,
                                      fec_ids,
                                      &nof_entries);

        if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
        {
          LOG_ERROR(BSL_LS_BCM_L3,
                    (BSL_META_U(unit,
                                "soc_sand error %x\n"), ret));
          BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_fec_get_block failed")));
        }

        LOG_VERBOSE(BSL_LS_BCM_L3,
                    (BSL_META_U(unit,
                                "nof_entries %u, entries_to_scan %u entries to act %u\n"), nof_entries, block_range.entries_to_scan, block_range.entries_to_act));

        if(nof_entries == 0)
        {
            break;
        }

        for(idx = 0; idx < nof_entries; idx++)
        {
          switch(mode)
          {
            case DPP_L3_TRAVERSE_CB:
            {
                /* Get fec entry*/
                bcm_l3_egress_t_init(&entry);
                ret = _bcm_ppd_frwrd_fec_entry_get(unit, fec_ids[idx], &entry);
                if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
                {
                    LOG_ERROR(BSL_LS_BCM_L3,(BSL_META_U(unit,"soc_sand error %x\n"), ret));
                    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("_bcm_ppd_frwrd_fec_entry_get failed")));
                }

                /* Invoke user callback. */
                (*cb)(unit, fec_ids[idx], &entry, user_data);
                break;
            }

            case DPP_L3_TRAVERSE_DELETE_ALL:
            case DPP_L3_TRAVERSE_DELETE_INTERFACE:
            default:
            {
              LOG_ERROR(BSL_LS_BCM_L3,
                        (BSL_META_U(unit,
                                    "illegal traverse mode %x\n"), mode));
              BCM_RETURN_VAL_EXIT(BCM_E_INTERNAL);
            }
          }
        }
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Function _ppd_xxxx */
int
  _bcm_ppd_frwrd_ipv4_route_get_block(
    int unit,
    uint32 mode,
    bcm_if_t interface,
    bcm_l3_route_traverse_cb cb,
    void *user_data
  )
{  
    uint32
      ret;
    
    SOC_PPC_VRF_ID
      vrf_ndx = BCM_L3_VRF_DEFAULT;
    SOC_PPC_IP_ROUTING_TABLE_RANGE
      block_range;
    SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY
      *route_keys = NULL;
    SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY
      vpn_route_keys[DPP_FRWRD_IP_GET_BLOCK_NOF_ENTRIES];
    SOC_PPC_FEC_ID
      fec_ids[DPP_FRWRD_IP_GET_BLOCK_NOF_ENTRIES];
    SOC_PPC_FRWRD_IP_ROUTE_STATUS
      routes_status[DPP_FRWRD_IP_GET_BLOCK_NOF_ENTRIES];
    SOC_PPC_FRWRD_IP_ROUTE_LOCATION
      routes_location[DPP_FRWRD_IP_GET_BLOCK_NOF_ENTRIES];
    uint32
      nof_entries, vrf_max;
    bcm_l3_route_t
      entry;
    int idx, status;

    BCMDNX_INIT_FUNC_DEFS;

    SOC_PPC_IP_ROUTING_TABLE_RANGE_clear(&block_range);

    block_range.entries_to_act = DPP_FRWRD_IP_GET_BLOCK_NOF_ENTRIES;
    block_range.entries_to_scan = DPP_FRWRD_IP_ENTRIES_TO_SCAN;
    block_range.start.type = SOC_PPC_IP_ROUTING_TABLE_ITER_TYPE_PREFIX_IP_ORDERED;
    /* SOC_PPC_IP_ROUTING_TABLE_ITER_BEGIN(block_range.start.payload); */
    soc_sand_u64_clear(&block_range.start.payload);

    status = _bcm_ppd_frwrd_ipv4_nof_vrfs_get(unit, &vrf_max);

    if (status != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_L3,
                  (BSL_META_U(unit,
                              "nof_vrfs_get failed: %x\n"), status));
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("_bcm_ppd_frwrd_ipv4_nof_vrfs_getv failed")));
    }

    BCMDNX_ALLOC(route_keys, sizeof(SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY)*DPP_FRWRD_IP_GET_BLOCK_NOF_ENTRIES, "_bcm_ppd_frwrd_ipv4_route_get_block.route_keys");
    if (route_keys == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
    }
    for (vrf_ndx=BCM_L3_VRF_DEFAULT; vrf_ndx<vrf_max; vrf_ndx++)
    {
      soc_sand_u64_clear(&block_range.start.payload);
      while(!SOC_PPC_IP_ROUTING_TABLE_ITER_IS_END(&block_range.start.payload))
      {
        nof_entries = 0;
        if (vrf_ndx == BCM_L3_VRF_DEFAULT)
        {
          ret = soc_ppd_frwrd_ipv4_uc_route_get_block(unit,
                                                  &block_range,
                                                  route_keys,
                                                  fec_ids,
                                                  routes_status,
                                                  routes_location,
                                                  &nof_entries);
        } else {
          ret = soc_ppd_frwrd_ipv4_vrf_route_get_block(unit,
                                                   vrf_ndx,
                                                   &block_range,
                                                   vpn_route_keys,
                                                   fec_ids,
                                                   routes_status,
                                                   routes_location,
                                                   &nof_entries);
        }

        if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
        {
          LOG_ERROR(BSL_LS_BCM_L3,
                    (BSL_META_U(unit,
                                "soc_sand error %x\n"), ret));
          BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv4_vrf_route_get_block failed")));
        }

        if(nof_entries == 0)
        {
            break;
        }

        for(idx = 0; idx < nof_entries; idx++)
        {
          bcm_l3_route_t_init(&entry);

          if (vrf_ndx == BCM_L3_VRF_DEFAULT)
          {
              entry.l3a_vrf     = BCM_L3_VRF_DEFAULT;
              entry.l3a_subnet  = route_keys[idx].subnet.ip_address;
              entry.l3a_ip_mask = bcm_ip_mask_create(route_keys[idx].subnet.prefix_len);
          } else {
              entry.l3a_vrf     = vrf_ndx;
              entry.l3a_subnet  = vpn_route_keys[idx].subnet.ip_address;
              entry.l3a_ip_mask = bcm_ip_mask_create(vpn_route_keys[idx].subnet.prefix_len);
          }
          _bcm_l3_fec_to_intf(unit, fec_ids[idx], &(entry.l3a_intf));

          switch(mode)
          {
            case DPP_L3_TRAVERSE_CB:
            {
              /* Invoke user callback. */
              (*cb)(unit, idx, &entry, user_data);
              break;
            }

            case DPP_L3_TRAVERSE_DELETE_ALL:
            {
              status = bcm_petra_l3_route_delete(unit, &entry);
              break;
            }

            case DPP_L3_TRAVERSE_DELETE_INTERFACE:
            {
              LOG_ERROR(BSL_LS_BCM_L3,
                  (BSL_META_U(unit,
                              "DPP_L3_TRAVERSE_DELETE_INTERFACE not implemented\n")));
              BCM_RETURN_VAL_EXIT(BCM_E_UNAVAIL);
              break;
            }

            default:
            {
              LOG_ERROR(BSL_LS_BCM_L3,
                        (BSL_META_U(unit,
                                    "illegal traverse mode %x\n"), mode));
              BCM_RETURN_VAL_EXIT(BCM_E_INTERNAL);
            }
          }
        }
      }
    }

    BCM_EXIT;
exit:
    BCM_FREE(route_keys);
    BCMDNX_FUNC_RETURN;
}

int
  _bcm_ppd_frwrd_ipv4_route_traverse(
    int unit,
    uint32 flags,
    bcm_l3_route_traverse_cb cb,
    void *user_data
  )
{  
    SOC_DPP_DBAL_SW_TABLE_IDS table_ids[] = {
        SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KAPS,
        SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_ROUTE_SCALE_LONG_KAPS,
        SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_ROUTE_SCALE_SHORT_KAPS,
		SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_FWD_KBP,
		SOC_DPP_DBAL_SW_TABLE_ID_IPV4_DC,
        SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KBP_DEFAULT_ROUTE_KAPS
    };
    uint32 tables_num = sizeof(table_ids)/sizeof(SOC_DPP_DBAL_SW_TABLE_IDS);
    uint32
      ret;
    int i, qual_idx, is_table_initiated, idx=0;
    SOC_PPC_FP_QUAL_TYPE qual_type;
    SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    uint32 kaps_payload = 0;
    SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO kbp_payload;
    void *payload = NULL;
    uint8 found;
    uint8 hit_bit = 0;
    uint32 soc_flags = 0;
    bcm_l3_route_t entry;
    SOC_DPP_DBAL_PHYSICAL_DB_TYPES physical_db_type;

    BCMDNX_INIT_FUNC_DEFS;

    if (flags & BCM_L3_HIT_CLEAR) {
        if (!SOC_IS_JERICHO_PLUS(unit)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("BCM_L3_HIT_CLEAR only available in Jericho plus and above.")));
        }

        soc_flags = SOC_PPC_FRWRD_IP_CLEAR_ON_GET;
    }

    for (i=0; i<tables_num; i++) {
        ret = soc_ppd_frwrd_ipv4_uc_route_iterator_table_is_initiated(unit, table_ids[i], &is_table_initiated);
        if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
        {
          LOG_ERROR(BSL_LS_BCM_L3,
                    (BSL_META_U(unit,
                                "soc_sand error %x\n"), ret));
          BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv4_uc_route_iterator_table_is_initiated failed")));
        }

        if (is_table_initiated) {
            ret = soc_ppd_frwrd_ipv4_uc_route_iterator_init(unit, table_ids[i], &physical_db_type);
            if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
            {
              LOG_ERROR(BSL_LS_BCM_L3,
                        (BSL_META_U(unit,
                                    "soc_sand error %x\n"), ret));
              BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv4_uc_route_iterator_init failed")));
            }

            /* Per Physical DB payload */
            if (physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS) {
                payload = &kaps_payload;
            } else if (physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP) {
                payload = &kbp_payload;
            }

            ret = soc_ppd_frwrd_ipv4_uc_route_iterator_get_next(unit, table_ids[i], soc_flags, qual_vals, payload, NULL/*priority*/, &hit_bit, &found);
            if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
            {
              LOG_ERROR(BSL_LS_BCM_L3,
                        (BSL_META_U(unit,
                                    "soc_sand error %x\n"), ret));
              BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv4_uc_route_iterator_get_next failed")));
            }

            while (found==TRUE) {
                bcm_l3_route_t_init(&entry);
                /* From qual_Vals to entry */
                for (qual_idx = 0; qual_idx < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; qual_idx++) {
                    qual_type = qual_vals[qual_idx].type;
                    if ((qual_type == SOC_PPC_NOF_FP_QUAL_TYPES) || (qual_type == BCM_FIELD_ENTRY_INVALID)) {
                        break;
                    }
                    if (qual_type == SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP) {
                        entry.l3a_subnet = qual_vals[qual_idx].val.arr[0];
                        entry.l3a_ip_mask = qual_vals[qual_idx].is_valid.arr[0];
                    }
                    if (qual_type == SOC_PPC_FP_QUAL_IRPP_VRF) {
                        entry.l3a_vrf = qual_vals[qual_idx].val.arr[0];
                    }
                }

                if (hit_bit & 1) {
                    entry.l3a_flags |= BCM_L3_HIT;
                }

                if ((table_ids[i] == SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_ROUTE_SCALE_SHORT_KAPS) || (table_ids[i] == SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_ROUTE_SCALE_LONG_KAPS) || (table_ids[i] == SOC_DPP_DBAL_SW_TABLE_ID_IPV4_DC)) {
                    entry.l3a_flags2 |= BCM_L3_FLAGS2_SCALE_ROUTE;
                }

                /* Per Physical DB payload */
                if (physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS) {
                    if (BCM_L3_ITF_TYPE_IS_FEC(kaps_payload)) {
                        entry.l3a_intf = kaps_payload;
                    } else {
                        entry.l3a_intf = 0;
                    }
				} else if (physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP) {
					_bcm_l3_fec_to_intf(unit, kbp_payload.fec_id, &(entry.l3a_intf));
                }

                (*cb)(unit, idx, &entry, user_data);
                idx++;

                ret = soc_ppd_frwrd_ipv4_uc_route_iterator_get_next(unit, table_ids[i], soc_flags, qual_vals, payload, NULL/*priority*/, &hit_bit, &found);
                if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
                {
                  LOG_ERROR(BSL_LS_BCM_L3,
                            (BSL_META_U(unit,
                                        "soc_sand error %x\n"), ret));
                  BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv4_uc_route_iterator_get_next failed")));
                }
            }

            ret = soc_ppd_frwrd_ipv4_uc_route_iterator_deinit(unit, table_ids[i]);
            if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
            {
              LOG_ERROR(BSL_LS_BCM_L3,
                        (BSL_META_U(unit,
                                    "soc_sand error %x\n"), ret));
              BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv4_uc_route_iterator_deinit failed")));
            }
        }
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
  _bcm_ppd_frwrd_ipv4_host_traverse(
    int unit,
    uint32 flags,
    bcm_l3_host_traverse_cb cb,
    void *user_data
  )
{  
    SOC_DPP_DBAL_SW_TABLE_IDS table_ids[] = {
        SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_LEM,
        SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_ROUTE_SCALE_LEM,
		SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_FWD_KBP,
		SOC_DPP_DBAL_SW_TABLE_ID_IPV4_DC
    };
    uint32 tables_num = sizeof(table_ids)/sizeof(SOC_DPP_DBAL_SW_TABLE_IDS);
    uint32
      ret;
    int i, qual_idx, is_table_initiated, idx=0;
    SOC_PPC_FP_QUAL_TYPE qual_type;
    SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO payload;
    SOC_PPC_FRWRD_IP_ROUTE_STATUS payload_status = 0;
    uint8 found;
    bcm_l3_host_t entry;
    uint32 ppd_flags = 0;
    uint8 hit_bit = 0;
    SOC_DPP_DBAL_PHYSICAL_DB_TYPES physical_db_type;

    BCMDNX_INIT_FUNC_DEFS;

    for (i=0; i<tables_num; i++) {
        ret = soc_ppd_frwrd_ipv4_uc_route_iterator_table_is_initiated(unit, table_ids[i], &is_table_initiated);
        if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
        {
          LOG_ERROR(BSL_LS_BCM_L3,
                    (BSL_META_U(unit,
                                "soc_sand error %x\n"), ret));
          BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv4_uc_route_iterator_table_is_initiated failed")));
        }

        if (is_table_initiated) {
            ppd_flags = 0;
            if ((table_ids[i] == SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_LEM) || (table_ids[i] == SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_ROUTE_SCALE_LEM)) {
                if (flags & BCM_L3_HIT) {
                    ppd_flags |= SOC_PPC_FRWRD_IP_HOST_GET_ACCESSED_ONLY;
                }
                if (flags & BCM_L3_HIT_CLEAR) {
                    ppd_flags |= SOC_PPC_FRWRD_IP_HOST_CLEAR_ON_GET;
                }
            } else if ((table_ids[i] == SOC_DPP_DBAL_SW_TABLE_ID_IPV4_DC) || (table_ids[i] == SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_FWD_KBP)) {
				ppd_flags |= SOC_PPC_FRWRD_IP_HOST_KBP_HOST_INDICATION;
            }

            ret = soc_ppd_frwrd_ipv4_uc_route_iterator_init(unit, table_ids[i], &physical_db_type);
            if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
            {
              LOG_ERROR(BSL_LS_BCM_L3,
                        (BSL_META_U(unit,
                                    "soc_sand error %x\n"), ret));
              BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv4_uc_route_iterator_init failed")));
            }

            ret = soc_ppd_frwrd_ipv4_uc_route_iterator_get_next(unit, table_ids[i], ppd_flags, qual_vals, &payload, NULL/*priority*/, &hit_bit, &found);
            if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
            {
              LOG_ERROR(BSL_LS_BCM_L3,
                        (BSL_META_U(unit,
                                    "soc_sand error %x\n"), ret));
              BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv4_uc_route_iterator_get_next failed")));
            }

            while (found==TRUE) {
                bcm_l3_host_t_init(&entry);
                /* From qual_Vals to entry */
                for (qual_idx = 0; qual_idx < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; qual_idx++) {
                    qual_type = qual_vals[qual_idx].type;
                    if ((qual_type == SOC_PPC_NOF_FP_QUAL_TYPES) || (qual_type == BCM_FIELD_ENTRY_INVALID)) {
                        break;
                    }
                    if (qual_type == SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP) {
                        entry.l3a_ip_addr = qual_vals[qual_idx].val.arr[0];
                    }
                    if (qual_type == SOC_PPC_FP_QUAL_IRPP_VRF) {
                        entry.l3a_vrf = qual_vals[qual_idx].val.arr[0];
                    }
                }

                if ((table_ids[i] == SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_ROUTE_SCALE_LEM) || (table_ids[i] == SOC_DPP_DBAL_SW_TABLE_ID_IPV4_DC)) {
                    entry.l3a_flags2 |= BCM_L3_FLAGS2_SCALE_ROUTE;
                }
#if defined(INCLUDE_KBP)
                if ((table_ids[i] == SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_LEM) && (ARAD_KBP_ENABLE_IPV4_UC)) {
                    entry.l3a_flags |= BCM_L3_HOST_LOCAL;
                }
#endif
                payload_status = SOC_PPC_FRWRD_IP_ROUTE_STATUS_COMMITED;
                if (hit_bit & 1) {
                    payload_status |= SOC_PPC_FRWRD_IP_ROUTE_STATUS_ACCESSED;
                }

                ret = _bcm_ppd_host_info_from_ppd_get(unit, &payload, &payload_status, &entry);
                if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
                {
                  LOG_ERROR(BSL_LS_BCM_L3,
                            (BSL_META_U(unit,
                                        "soc_sand error %x\n"), ret));
                  BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("_bcm_ppd_host_info_from_ppd_get failed")));
                }

                (*cb)(unit, idx, &entry, user_data);
                idx++;

                ret = soc_ppd_frwrd_ipv4_uc_route_iterator_get_next(unit, table_ids[i], ppd_flags, qual_vals, &payload, NULL/*priority*/, &hit_bit, &found);
                if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
                {
                  LOG_ERROR(BSL_LS_BCM_L3,
                            (BSL_META_U(unit,
                                        "soc_sand error %x\n"), ret));
                  BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv4_uc_route_iterator_get_next failed")));
                }
            }

            ret = soc_ppd_frwrd_ipv4_uc_route_iterator_deinit(unit, table_ids[i]);
            if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
            {
              LOG_ERROR(BSL_LS_BCM_L3,
                        (BSL_META_U(unit,
                                    "soc_sand error %x\n"), ret));
              BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv4_uc_route_iterator_deinit failed")));
            }
        }
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
  _bcm_ppd_frwrd_ipv6_host_traverse(
    int unit,
    uint32 flags,
    bcm_l3_host_traverse_cb cb,
    void *user_data
  )
{
    SOC_DPP_DBAL_SW_TABLE_IDS table_ids[] = {
        SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE_SCALE_LONG_LEM,
        SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE_SCALE_SHORT_LEM,
        SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_FWD_KBP
    };
    uint32 tables_num = sizeof(table_ids)/sizeof(SOC_DPP_DBAL_SW_TABLE_IDS);
    uint32
      ret;
    int i, qual_idx, is_table_initiated, idx=0;
    SOC_PPC_FP_QUAL_TYPE qual_type;
    SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO payload;
    SOC_PPC_FRWRD_IP_ROUTE_STATUS payload_status = 0;
    uint8 found;
    bcm_l3_host_t entry;
    uint32 ppd_flags = 0;
    uint8 hit_bit = 0;
    SOC_DPP_DBAL_PHYSICAL_DB_TYPES physical_db_type;
    SOC_SAND_PP_IPV6_ADDRESS ipv6_addr;

    BCMDNX_INIT_FUNC_DEFS;

    for (i=0; i<tables_num; i++) {
        ret = soc_ppd_frwrd_ipv4_uc_route_iterator_table_is_initiated(unit, table_ids[i], &is_table_initiated);
        if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
        {
          LOG_ERROR(BSL_LS_BCM_L3,
                    (BSL_META_U(unit,
                                "soc_sand error %x\n"), ret));
          BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv4_uc_route_iterator_table_is_initiated failed")));
        }

        if (is_table_initiated) {
            ppd_flags = 0;
            if ((table_ids[i] == SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE_SCALE_SHORT_LEM) || (table_ids[i] == SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE_SCALE_LONG_LEM)) {
                if (flags & BCM_L3_HIT) {
                    ppd_flags |= SOC_PPC_FRWRD_IP_HOST_GET_ACCESSED_ONLY;
                }
                if (flags & BCM_L3_HIT_CLEAR) {
                    ppd_flags |= SOC_PPC_FRWRD_IP_HOST_CLEAR_ON_GET;
                }
            } else if (table_ids[i] == SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_FWD_KBP) {
				ppd_flags |= SOC_PPC_FRWRD_IP_HOST_KBP_HOST_INDICATION;
            }

            ret = soc_ppd_frwrd_ipv4_uc_route_iterator_init(unit, table_ids[i], &physical_db_type);
            if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
            {
              LOG_ERROR(BSL_LS_BCM_L3,
                        (BSL_META_U(unit,
                                    "soc_sand error %x\n"), ret));
              BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv4_uc_route_iterator_init failed")));
            }

            ret = soc_ppd_frwrd_ipv4_uc_route_iterator_get_next(unit, table_ids[i], ppd_flags, qual_vals, &payload, NULL/*priority*/, &hit_bit, &found);
            if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
            {
              LOG_ERROR(BSL_LS_BCM_L3,
                        (BSL_META_U(unit,
                                    "soc_sand error %x\n"), ret));
              BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv4_uc_route_iterator_get_next failed")));
            }

            while (found==TRUE) {
                bcm_l3_host_t_init(&entry);
                sal_memset(&ipv6_addr,0x0,sizeof(SOC_SAND_PP_IPV6_ADDRESS));
                /* From qual_Vals to entry */
                for (qual_idx = 0; qual_idx < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; qual_idx++) {
                    qual_type = qual_vals[qual_idx].type;
                    if ((qual_type == SOC_PPC_NOF_FP_QUAL_TYPES) || (qual_type == BCM_FIELD_ENTRY_INVALID)) {
                        break;
                    }
                    if (qual_type == SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP) {
                        entry.l3a_ip_addr = qual_vals[qual_idx].val.arr[0];
                    }
                    if (qual_type == SOC_PPC_FP_QUAL_IRPP_VRF) {
                        entry.l3a_vrf = qual_vals[qual_idx].val.arr[0];
                    }
                    if (qual_type == SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_LOW) {
                        ipv6_addr.address[0] = qual_vals[qual_idx].val.arr[0];
                        ipv6_addr.address[1] = qual_vals[qual_idx].val.arr[1];

                    }
                    if (qual_type == SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH) {
                        ipv6_addr.address[2] = qual_vals[qual_idx].val.arr[0];
                        ipv6_addr.address[3] = qual_vals[qual_idx].val.arr[1];
                    }
                }

                ret = _bcm_l3_sand_ipv6_addr_to_bcm_ipv6_addr(unit, &ipv6_addr, &(entry.l3a_ip6_addr));
                BCMDNX_IF_ERR_EXIT(ret);

                if ((table_ids[i] == SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE_SCALE_SHORT_LEM)) {
                    entry.l3a_flags2 |= BCM_L3_FLAGS2_SCALE_ROUTE;
                }

                if ((table_ids[i] == SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE_SCALE_SHORT_LEM) ||
                    (table_ids[i] == SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_ROUTE_SCALE_LONG_LEM) ||
                    (table_ids[i] == SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_FWD_KBP)) {
                    entry.l3a_flags |= BCM_L3_IP6;
                }

                payload_status = SOC_PPC_FRWRD_IP_ROUTE_STATUS_COMMITED;
                if (hit_bit & 1) {
                    payload_status |= SOC_PPC_FRWRD_IP_ROUTE_STATUS_ACCESSED;
                }

                ret = _bcm_ppd_host_info_from_ppd_get(unit, &payload, &payload_status, &entry);
                if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
                {
                  LOG_ERROR(BSL_LS_BCM_L3,
                            (BSL_META_U(unit,
                                        "soc_sand error %x\n"), ret));
                  BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("_bcm_ppd_host_info_from_ppd_get failed")));
                }

                (*cb)(unit, idx, &entry, user_data);
                idx++;

                ret = soc_ppd_frwrd_ipv4_uc_route_iterator_get_next(unit, table_ids[i], ppd_flags, qual_vals, &payload, NULL/*priority*/, &hit_bit, &found);
                if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
                {
                  LOG_ERROR(BSL_LS_BCM_L3,
                            (BSL_META_U(unit,
                                        "soc_sand error %x\n"), ret));
                  BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv4_uc_route_iterator_get_next failed")));
                }
            }

            ret = soc_ppd_frwrd_ipv4_uc_route_iterator_deinit(unit, table_ids[i]);
            if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
            {
              LOG_ERROR(BSL_LS_BCM_L3,
                        (BSL_META_U(unit,
                                    "soc_sand error %x\n"), ret));
              BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv4_uc_route_iterator_deinit failed")));
            }
        }
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Function _ppd_xxxx */
int
  _bcm_ppd_frwrd_ipv4_host_get_block(
    int unit,
    uint32 mode,
    uint32 flags,
    bcm_if_t interface,
    bcm_l3_host_traverse_cb cb,
    void *user_data
  )
{
    uint32
        ret,
        nof_entries = 0,
        host_flags=SOC_PPC_FRWRD_IP_HOST_GET_ACCESSS_STATUS;
    
    SOC_SAND_TABLE_BLOCK_RANGE
        block_range;
    SOC_PPC_FRWRD_IPV4_HOST_KEY
        *host_keys = NULL;
    SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO
        *routes_info = NULL;
    SOC_PPC_FRWRD_IP_ROUTE_STATUS
        routes_status[DPP_FRWRD_IP_GET_BLOCK_NOF_ENTRIES];
    bcm_l3_host_t
        entry;
    int idx, status;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&block_range);


    /* Block range */
    block_range.entries_to_act = DPP_FRWRD_IP_GET_BLOCK_NOF_ENTRIES;
    block_range.entries_to_scan = DPP_FRWRD_IP_ENTRIES_TO_SCAN;
    SOC_SAND_TBL_ITER_SET_BEGIN(&(block_range.iter));

    if (flags & BCM_L3_HIT) {
        host_flags |= SOC_PPC_FRWRD_IP_HOST_GET_ACCESSED_ONLY;
    }

    if (flags & BCM_L3_HIT_CLEAR) {
        host_flags |= SOC_PPC_FRWRD_IP_HOST_CLEAR_ON_GET;
    }


    BCMDNX_ALLOC(routes_info, sizeof(SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO)*DPP_FRWRD_IP_GET_BLOCK_NOF_ENTRIES, "_bcm_ppd_frwrd_ipv4_host_get_block.routes_info");
    if (routes_info == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
    }


    BCMDNX_ALLOC(host_keys, sizeof(SOC_PPC_FRWRD_IPV4_HOST_KEY)*DPP_FRWRD_IP_GET_BLOCK_NOF_ENTRIES, "_bcm_ppd_frwrd_ipv4_host_get_block.host_keys");
    if (host_keys == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
    }

    /*initialize routes_info and host_keys with 0*/
    sal_memset(routes_info, 0, sizeof(SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO)*DPP_FRWRD_IP_GET_BLOCK_NOF_ENTRIES);
    sal_memset(host_keys, 0, sizeof(SOC_PPC_FRWRD_IPV4_HOST_KEY)*DPP_FRWRD_IP_GET_BLOCK_NOF_ENTRIES);

    while(!SOC_SAND_TBL_ITER_IS_END(&(block_range.iter)))
    {
        ret = soc_ppd_frwrd_ipv4_host_get_block(unit,
                                            &block_range,
                                            host_flags,
                                            host_keys,
                                            routes_info,
                                            routes_status,
                                            &nof_entries);
        if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
        {
          LOG_ERROR(BSL_LS_BCM_L3,
                    (BSL_META_U(unit,
                                "soc_sand error %x\n"), ret));
          BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv4_host_get_block failed")));
        }

        if(nof_entries == 0)
        {
            break;
        }

        for(idx = 0; idx < nof_entries; idx++)
        {
          bcm_l3_host_t_init(&entry);

          entry.l3a_vrf       = host_keys[idx].vrf_ndx;
          entry.l3a_ip_addr   = host_keys[idx].ip_address;

          switch(mode)
          {
            case DPP_L3_TRAVERSE_CB:
            {
              /* get rest of information */
                status = _bcm_ppd_host_info_from_ppd_get(unit,&routes_info[idx],&routes_status[idx],&entry);
                BCMDNX_IF_ERR_EXIT(status);
              /* Invoke user callback. */
              (*cb)(unit, idx, &entry, user_data);
              break;
            }

            case DPP_L3_TRAVERSE_DELETE_ALL:
            {
              status = bcm_petra_l3_host_delete(unit, &entry);
              BCMDNX_IF_ERR_EXIT(status);
              break;
            }

            case DPP_L3_TRAVERSE_DELETE_INTERFACE:
            {
              LOG_ERROR(BSL_LS_BCM_L3,
                  (BSL_META_U(unit,
                              "DPP_L3_TRAVERSE_DELETE_INTERFACE not implemented\n")));
              BCM_RETURN_VAL_EXIT(BCM_E_UNAVAIL);
              break;
            }

            default:
            {
              LOG_ERROR(BSL_LS_BCM_L3,
                        (BSL_META_U(unit,
                                    "illegal traverse mode %x\n"), mode));
              BCM_RETURN_VAL_EXIT(BCM_E_INTERNAL);
            }
          }
        }
    }

    BCM_EXIT;
exit:
    BCM_FREE(routes_info);
    BCM_FREE(host_keys);
    BCMDNX_FUNC_RETURN;
}


/***************************************************************/
/***************************************************************/

/*
 * ECMP Helper Functions
 *
 * 16 ECMP sizes are supported. 
 * The sizes are between 1 and 288.
 */


/***************************************************************/
/***************************************************************/

/*
 * VRRP Helper Functions
 *
 * Virtual Router Redundancy Protocol
 * - VRRP support mythologies:
 *   VSI / 256 VSIs
 */

/* Function _ppd_xxxx */
int
  _bcm_ppd_mymac_vrrp_info_get(
    int unit
  )
{
  
  SOC_PPC_MYMAC_VRRP_INFO
    vrrp_info;

  BCMDNX_INIT_FUNC_DEFS;

  SOC_PPC_MYMAC_VRRP_INFO_clear(&vrrp_info);

  BCM_SAND_IF_ERR_EXIT(soc_ppd_mymac_vrrp_info_get(unit, &vrrp_info));

  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}

/* Function _ppd_xxxx */
int
  _bcm_ppd_mymac_vrrp_info_set(
      int unit, 
      uint32 flags,
      int mode
  )
{
  
  SOC_PPC_MYMAC_VRRP_INFO
    vrrp_info;

  BCMDNX_INIT_FUNC_DEFS;

  SOC_PPC_MYMAC_VRRP_INFO_clear(&vrrp_info);

  vrrp_info.enable = TRUE;
  vrrp_info.vrid_my_mac_msb.address[5] = 0;
  vrrp_info.vrid_my_mac_msb.address[4] = 0;
  vrrp_info.vrid_my_mac_msb.address[3] = 0x5e;
  vrrp_info.vrid_my_mac_msb.address[2] = 0;
  vrrp_info.vrid_my_mac_msb.address[0] = 0;
  
  vrrp_info.mode = mode;
  vrrp_info.ipv6_enable = (flags & BCM_L3_IP6) ? TRUE : FALSE;
  vrrp_info.vrid_my_mac_msb.address[1] = 1;
  if (vrrp_info.ipv6_enable) {
      vrrp_info.ipv6_vrid_my_mac_msb.address[5] = 0;
      vrrp_info.ipv6_vrid_my_mac_msb.address[4] = 0;
      vrrp_info.ipv6_vrid_my_mac_msb.address[3] = 0x5e;
      vrrp_info.ipv6_vrid_my_mac_msb.address[2] = 0;
      vrrp_info.ipv6_vrid_my_mac_msb.address[1] = 2;
      vrrp_info.ipv6_vrid_my_mac_msb.address[0] = 0;
  }

  BCM_SAND_IF_ERR_EXIT(soc_ppd_mymac_vrrp_info_set(unit,&vrrp_info));

  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}

/* Function _ppd_xxxx */
int
  _bcm_ppd_mymac_vrrp_mac_get(
    int unit,
    bcm_vlan_t vlan,
    uint32 vrid,
    uint8 *enable
  )
{
  
  uint32
    vrrp_id_ndx;
  SOC_SAND_PP_MAC_ADDRESS
    vrrp_mac_lsb_key;

  BCMDNX_INIT_FUNC_DEFS;

  vrrp_id_ndx = vlan;

  vrrp_mac_lsb_key.address[0] = (uint8)vrid;

  BCM_SAND_IF_ERR_EXIT(soc_ppd_mymac_vrrp_mac_get(
             unit,
             vrrp_id_ndx,
             &vrrp_mac_lsb_key,
             enable
             ));

  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}


/* Function _ppd_xxxx */
int
  _bcm_ppd_mymac_vrrp_mac_set(
    int unit,
    bcm_vlan_t vlan, 
    uint32 vrid,
    uint8 enable
  )
{
  
  uint32
    vrrp_id_ndx;
  SOC_SAND_PP_MAC_ADDRESS
    vrrp_mac_lsb_key;

  BCMDNX_INIT_FUNC_DEFS;

  vrrp_id_ndx = vlan;

  vrrp_mac_lsb_key.address[0] = (uint8)vrid;
  
  BCM_SAND_IF_ERR_EXIT(soc_ppd_mymac_vrrp_mac_set(
             unit,
             vrrp_id_ndx,
             &vrrp_mac_lsb_key,
             enable
             ));

  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}


/***************************************************************/
/***************************************************************/

/*
 * Begin BCM Functions
 */

/*
 * Function:
 *      bcm_petra_l3_init
 * Purpose:
 *      Initialize the BCM L3 subsystem.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_init(
    int unit)
{
    int status;
    int vrrp_mode = 0;
    uint32 vrrp_flags = 0;
    soc_dpp_config_l3_t *dpp_l3;
    bcm_dpp_vsi_egress_profile_t vsi_egress_profile_init_data = {0};
    bcm_dpp_vsi_ingress_profile_t vsi_ingress_profile_init_data;
    uint32 rv;
    uint16 ttl = 0;
    bcm_tunnel_terminator_t tunnel_term_ip_lif_dummy;
    bcm_tunnel_terminator_t port_tunnel_term_ip_lif_dummy;
    bcm_tunnel_terminator_t port_tunnel_term_gre_ip_lif_dummy;
    int current_multiple_mymac_mode;
    uint8 is_allocated;
    QAX_PP_EG_ENCAP_ACCESS_OUT_RIF_PROFILE_ENTRY_FORMAT outrif_init_profile;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    sw_state_sync_db[(unit)].dpp.l3_lock = sal_mutex_create("_dpp_l3_unit_lock");
    if (!sw_state_sync_db[(unit)].dpp.l3_lock) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("Faqiled to create mutex")));
    }

    if (SOC_WARM_BOOT(unit))
    {
        BCM_EXIT;
    }

    dpp_l3 = &(SOC_DPP_CONFIG(unit))->l3;

    /* get max_vid and ipv6_distinct form soc properties */

    switch(dpp_l3->vrrp_max_vid) {
      case 4096:
            vrrp_mode = SOC_PPC_MYMAC_VRRP_MODE_ALL_VSI_BASED;
        break;
      case 2*1024:
            vrrp_mode = SOC_PPC_MYMAC_VRRP_MODE_2K_VSI_BASED;
        break;
      case 1024:
            vrrp_mode = SOC_PPC_MYMAC_VRRP_MODE_1K_VSI_BASED;
        break;
      case 512:
            vrrp_mode = SOC_PPC_MYMAC_VRRP_MODE_512_VSI_BASED;
        break;
      case 256:
            vrrp_mode = SOC_PPC_MYMAC_VRRP_MODE_256_VSI_BASED;
        break;
    default:
       BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Soc property L3_VRRP_MAX_VID is out of range")));
    }

    /* Either ipv6 distinction is on, or we're using multiple mymac and then the distinction is
       by multiple mymac mode */
    if (dpp_l3->vrrp_ipv6_distinct 
        || (dpp_l3->multiple_mymac_enabled && dpp_l3->multiple_mymac_mode == 1)) {
        vrrp_flags |= BCM_L3_IP6;
    }
    if (((vrrp_mode == SOC_PPC_MYMAC_VRRP_MODE_ALL_VSI_BASED) || (vrrp_mode == SOC_PPC_MYMAC_VRRP_MODE_2K_VSI_BASED))
        && dpp_l3->vrrp_ipv6_distinct && SOC_IS_ARAD_B1_AND_BELOW(unit)
        ) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("If L3_VRRP_MAX_VID = 4K or 2K, L3_VRRP_IPV6_DISTINCT cannot be 1")));
    }

    /* create new unit state information */
    BCMDNX_IF_ERR_EXIT(L3_ACCESS.is_allocated(unit, &is_allocated));
    if(!is_allocated) {
        BCMDNX_IF_ERR_EXIT(L3_ACCESS.alloc(unit));
    }
    BCMDNX_IF_ERR_EXIT(L3_ACCESS.dpp_l3_state.is_allocated(unit, &is_allocated));
    if(!is_allocated) {
        BCMDNX_IF_ERR_EXIT(L3_ACCESS.dpp_l3_state.alloc(unit));
    }

    BCMDNX_IF_ERR_EXIT(L3_ACCESS.dpp_l3_state.urpf_mode.set(unit, bcmL3IngressUrpfStrict));
#ifdef BCM_88660_A0
    /* In Arad+ the uRPF mode can be resolved per RIF by using the inlif profile to determine the uRPF mode. */
    /* The final uRPF mode is the stricter mode of the FEC uRPF mode and the inlif profile uRPF mode. */
    /* To cancel the effect of the FEC uRPF mode which is now redundant, we set it to loose. */
    if (SOC_IS_ARADPLUS(unit) && soc_property_get(unit, spn_BCM886XX_L3_INGRESS_URPF_ENABLE, 0)) {
        BCMDNX_IF_ERR_EXIT(L3_ACCESS.dpp_l3_state.urpf_mode.set(unit, bcmL3IngressUrpfLoose));
    }
#endif /* BCM_88660_A0 */


    status = _bcm_l3_sw_init(unit);

    /* mapping l3 interface to EEP */
    BCMDNX_IF_ERR_EXIT(L3_ACCESS.dpp_l3_state.bcm_tunnel_intf_to_eep.is_allocated(unit, &is_allocated));
    if(!is_allocated) {
        BCMDNX_IF_ERR_EXIT(L3_ACCESS.dpp_l3_state.bcm_tunnel_intf_to_eep.alloc(unit, _BCM_PETRA_L3_NOF_ITFS));
    }

    /* initialize tunnel resources */
    if (status == BCM_E_NONE) {
        status = _bcm_ip_tunnel_sw_init(unit);
    }

    /* Create IP-LIF-Dummy */
    if (SOC_DPP_CONFIG(unit)->pp.ip_lif_dummy_id) {
        bcm_tunnel_terminator_t_init(&tunnel_term_ip_lif_dummy);
        tunnel_term_ip_lif_dummy.type = bcmTunnelTypeIpAnyIn4;/* this is IPv4 termination */
        BCM_GPORT_TUNNEL_ID_SET(tunnel_term_ip_lif_dummy.tunnel_id, SOC_DPP_CONFIG(unit)->pp.ip_lif_dummy_id);
        tunnel_term_ip_lif_dummy.flags |= (BCM_TUNNEL_TERM_TUNNEL_WITH_ID);
            
        rv = bcm_petra_tunnel_terminator_create(unit, &tunnel_term_ip_lif_dummy);
        if (rv != BCM_E_NONE) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Failed to create IP-LIF-Dummy")));
        }
    } else if (SOC_DPP_CONFIG(unit)->pp.ip_lif_dummy_id_vxlan) {
        bcm_tunnel_terminator_t_init(&tunnel_term_ip_lif_dummy);
        tunnel_term_ip_lif_dummy.type = bcmTunnelTypeIpAnyIn4;
        BCM_GPORT_TUNNEL_ID_SET(tunnel_term_ip_lif_dummy.tunnel_id, SOC_DPP_CONFIG(unit)->pp.ip_lif_dummy_id_vxlan);
        tunnel_term_ip_lif_dummy.flags |= (BCM_TUNNEL_TERM_TUNNEL_WITH_ID);
            
        rv = bcm_petra_tunnel_terminator_create(unit, &tunnel_term_ip_lif_dummy);
        if (rv != BCM_E_NONE) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Failed to create VXLAN-IP-LIF-Dummy")));
        }
    } else if (SOC_DPP_CONFIG(unit)->pp.custom_feature_ip_port_termination_lif_id) {
        bcm_tunnel_terminator_t_init(&port_tunnel_term_ip_lif_dummy);
        port_tunnel_term_ip_lif_dummy.type = bcmTunnelTypeIpAnyIn4;
        BCM_GPORT_TUNNEL_ID_SET(port_tunnel_term_ip_lif_dummy.tunnel_id, SOC_DPP_CONFIG(unit)->pp.custom_feature_ip_port_termination_lif_id);
        port_tunnel_term_ip_lif_dummy.flags |= (BCM_TUNNEL_TERM_TUNNEL_WITH_ID);

        rv = bcm_petra_tunnel_terminator_create(unit, &port_tunnel_term_ip_lif_dummy);
        if (rv != BCM_E_NONE) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Failed to create IP-LIF-Dummy")));
        }
    }
    
    /*L3 GRE*/
    if (SOC_DPP_CONFIG(unit)->pp.ip_lif_dummy_id_gre) {
        bcm_tunnel_terminator_t_init(&port_tunnel_term_gre_ip_lif_dummy);
        port_tunnel_term_gre_ip_lif_dummy.type = bcmTunnelTypeGre4In4;
        BCM_GPORT_TUNNEL_ID_SET(port_tunnel_term_gre_ip_lif_dummy.tunnel_id, SOC_DPP_CONFIG(unit)->pp.ip_lif_dummy_id_gre);
        port_tunnel_term_gre_ip_lif_dummy.flags |= (BCM_TUNNEL_TERM_TUNNEL_WITH_ID);
            
        rv = bcm_petra_tunnel_terminator_create(unit, &port_tunnel_term_gre_ip_lif_dummy);
        if (rv != BCM_E_NONE) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Failed to create GRE-IP-LIF-Dummy")));
        }
    } else if (SOC_DPP_CONFIG(unit)->pp.custom_feature_ip_l3_gre_port_termination_lif_id) {
        bcm_tunnel_terminator_t_init(&port_tunnel_term_gre_ip_lif_dummy);
        port_tunnel_term_gre_ip_lif_dummy.type = bcmTunnelTypeGre4In4;
        BCM_GPORT_TUNNEL_ID_SET(port_tunnel_term_gre_ip_lif_dummy.tunnel_id, SOC_DPP_CONFIG(unit)->pp.custom_feature_ip_l3_gre_port_termination_lif_id);
        port_tunnel_term_gre_ip_lif_dummy.flags |= (BCM_TUNNEL_TERM_TUNNEL_WITH_ID);

        rv = bcm_petra_tunnel_terminator_create(unit, &port_tunnel_term_gre_ip_lif_dummy);
        if (rv != BCM_E_NONE) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Failed to create GRE-IP-LIF-Dummy")));
        }
    }
    /* Initialize ttl scope template */
    rv = _bcm_dpp_am_template_ttl_scope_init(unit, 0, &ttl);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Initialize out_rif_profile template to invalid profile value*/
    if(SOC_IS_JERICHO_PLUS(unit) && ((SOC_DPP_CONFIG(unit))->l3.nof_rifs > 0)){
        outrif_init_profile.outlif_profile = 0xFF;
        outrif_init_profile.oam_lif_set = 0;
        outrif_init_profile.drop = 0;
        outrif_init_profile.remark_profile = 0 ;
        rv = _bcm_dpp_am_template_out_rif_profile_init(unit,0,&outrif_init_profile);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    if (!SOC_WARM_BOOT(unit)) {
        /* Initialize device vrrp modes */
        status = _bcm_ppd_mymac_vrrp_info_set(unit, vrrp_flags, vrrp_mode);
        BCMDNX_IF_ERR_EXIT(status);

        if (SOC_IS_JERICHO(unit)) {
            /* Device protocol group need to be initialized via protocol group APIs.
               However, the APIs won't work if the protocol group mode is disabled.
               So we save the current mode, activate the portocol group mode,
               and then restore it to what it was. */

            current_multiple_mymac_mode = _BCM_PETRA_L3_MULTIPLE_MYMAC_MODE(unit);

            _BCM_PETRA_L3_MULTIPLE_MYMAC_MODE(unit) = _BCM_PETRA_L3_MULTIPLE_MYMAC_MODE_PROTOCOL_GROUP_ACTIVE;

            /* Initialize device's protocol groups. */
            status = bcm_dpp_switch_l3_protocol_group_init(unit);

            _BCM_PETRA_L3_MULTIPLE_MYMAC_MODE(unit) = current_multiple_mymac_mode; /* Restore global value */

            BCMDNX_IF_ERR_EXIT(status);
        }
    }
    
    if (status == BCM_E_NONE) {
        LOG_VERBOSE(BSL_LS_BCM_L3,
                    (BSL_META_U(unit,
                                "L3 completed init successfully\n")));
    } else {
        BCMDNX_ERR_EXIT_MSG(status, (_BSL_BCM_MSG("L3 init failed")));
    }

     /*init vsi profile template*/
     vsi_egress_profile_init_data.mtu = 0;
     COMPILER_64_ZERO(vsi_egress_profile_init_data.eve_da_lsbs_bitmap);
     rv = _bcm_dpp_am_template_vsi_egress_profile_init(unit, &vsi_egress_profile_init_data);
     BCMDNX_IF_ERR_EXIT(rv);
     COMPILER_64_ZERO(vsi_ingress_profile_init_data.ive_da_lsbs_bitmap);
     COMPILER_64_ZERO(vsi_ingress_profile_init_data.drop_da_lsbs_bitmap);
     COMPILER_64_ZERO(vsi_ingress_profile_init_data.peer_da_lsbs_bitmap);
     rv = _bcm_dpp_am_template_vsi_ingress_profile_init(unit, &vsi_ingress_profile_init_data);
     BCMDNX_IF_ERR_EXIT(rv);

    if (SOC_IS_JERICHO(unit)) {
        /* creating RIF profile 0  this profile is saved for VSIs that doesn't have RIF (for IPV4 comp)
           this is done only in the SW, the HW profile 0 is already configured by init with all "0" but now profile 0 is reserved and the SW will not allocate it */
        SOC_PPC_RIF_INFO rif_info;
        SOC_PPC_RIF_INFO_clear(&rif_info);

        rif_info.routing_enablers_bm = 0; /*DISABLE ALL*/
        rv = _bcm_dpp_am_template_l3_rif_mac_termination_combination_alloc(unit, 0, &rif_info.routing_enablers_bm, &rif_info.routing_enablers_bm_id, &rif_info.is_configure_enabler_needed);
        BCMDNX_IF_ERR_EXIT(rv);        
    }

    BCMDNX_IF_ERR_EXIT(L3_ACCESS.dpp_l3_state.init.set(unit, TRUE));


    BCMDNX_IF_ERR_EXIT(status);
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_cleanup
 * Purpose:
 *      De-initialize the BCM L3 subsystem.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_cleanup(
    int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    int init;
    uint8 is_allocated;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    
    BCMDNX_IF_ERR_EXIT(L3_ACCESS.is_allocated(unit, &is_allocated));
    if(!is_allocated) {
        BCM_EXIT;
    }
    BCMDNX_IF_ERR_EXIT(L3_ACCESS.dpp_l3_state.init.get(unit, &init));
    if (init == TRUE) {

        /* Free IP-LIF-Dummy resource */
        if (SOC_DPP_CONFIG(unit)->pp.ip_lif_dummy_id &&
            bcm_dpp_am_l3_lif_is_allocated(unit, _BCM_DPP_AM_L3_LIF_IP_TERM, 0, SOC_DPP_CONFIG(unit)->pp.ip_lif_dummy_id) == BCM_E_EXISTS) {
            rv = bcm_dpp_am_l3_lif_dealloc(unit, SOC_DPP_CONFIG(unit)->pp.ip_lif_dummy_id);
            BCMDNX_IF_ERR_EXIT(rv);

            rv = _bcm_dpp_local_lif_sw_resources_delete(unit, SOC_DPP_CONFIG(unit)->pp.ip_lif_dummy_id, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS);
            BCMDNX_IF_ERR_EXIT(rv);
        } else if (SOC_DPP_CONFIG(unit)->pp.ip_lif_dummy_id_vxlan &&
            bcm_dpp_am_l3_lif_is_allocated(unit, _BCM_DPP_AM_L3_LIF_IP_TERM, 0, SOC_DPP_CONFIG(unit)->pp.ip_lif_dummy_id_vxlan) == BCM_E_EXISTS) {
            rv = bcm_dpp_am_l3_lif_dealloc(unit, SOC_DPP_CONFIG(unit)->pp.ip_lif_dummy_id_vxlan);
            BCMDNX_IF_ERR_EXIT(rv);

            rv = _bcm_dpp_local_lif_sw_resources_delete(unit, SOC_DPP_CONFIG(unit)->pp.ip_lif_dummy_id_vxlan, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS);
            BCMDNX_IF_ERR_EXIT(rv);
        } else if (SOC_DPP_CONFIG(unit)->pp.custom_feature_ip_port_termination_lif_id &&
                   bcm_dpp_am_l3_lif_is_allocated(unit, _BCM_DPP_AM_L3_LIF_IP_TERM, 0, SOC_DPP_CONFIG(unit)->pp.custom_feature_ip_port_termination_lif_id) == BCM_E_EXISTS) {
            rv = bcm_dpp_am_l3_lif_dealloc(unit, SOC_DPP_CONFIG(unit)->pp.custom_feature_ip_port_termination_lif_id);
            BCMDNX_IF_ERR_EXIT(rv);

            rv = _bcm_dpp_local_lif_sw_resources_delete(unit, SOC_DPP_CONFIG(unit)->pp.custom_feature_ip_port_termination_lif_id, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /* Free GRE-LIF-Dummy resource */
        if (SOC_DPP_CONFIG(unit)->pp.ip_lif_dummy_id_gre &&
            bcm_dpp_am_l3_lif_is_allocated(unit, _BCM_DPP_AM_L3_LIF_IP_TERM, 0, SOC_DPP_CONFIG(unit)->pp.ip_lif_dummy_id_gre) == BCM_E_EXISTS) {
            rv = bcm_dpp_am_l3_lif_dealloc(unit, SOC_DPP_CONFIG(unit)->pp.ip_lif_dummy_id_gre);
            BCMDNX_IF_ERR_EXIT(rv);

            rv = _bcm_dpp_local_lif_sw_resources_delete(unit, SOC_DPP_CONFIG(unit)->pp.ip_lif_dummy_id_gre, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS);
            BCMDNX_IF_ERR_EXIT(rv);
        } 
        
    	rv = _bcm_ip_tunnel_sw_cleanup(unit);
    	if (rv != BCM_E_NONE) {
    	    LOG_VERBOSE(BSL_LS_BCM_L3,
                            (BSL_META_U(unit,
                                        "L3 IP TUNNEL cleanup failed\n")));
    	}

    	if (sw_state_sync_db[(unit)].dpp.l3_lock != NULL) {
    	    sal_mutex_destroy(sw_state_sync_db[(unit)].dpp.l3_lock);
    	}
	
    }

    LOG_VERBOSE(BSL_LS_BCM_L3,
                (BSL_META_U(unit,
                            "L3 completed cleanup successfully\n")));

    BCM_EXIT; 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_enable_set
 * Purpose:
 *      Enable/disable L3 function without clearing any L3 tables.
 * Parameters:
 *      unit - (IN) Unit number.
 *      enable - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_enable_set(
    int unit, 
    int enable)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_L3_UNIT_INIT_CHECK;

    DPP_L3_LOCK_TAKE;

    DPP_L3_LOCK_RELEASE;

    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_l3_enable_set is not available"))); 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_info
 * Purpose:
 *      Get the status of hardware tables.
 * Parameters:
 *      unit - (IN) Unit number.
 *      l3info - (OUT) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_info(
    int unit, 
    bcm_l3_info_t *l3info)
{
    int status = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_L3_UNIT_INIT_CHECK;

    DPP_L3_LOCK_TAKE;

    status = _bcm_ppd_frwrd_ipv4_glbl_info_get(unit, l3info);
    BCMDNX_IF_ERR_EXIT(status);

exit:
    DPP_L3_LOCK_RELEASE;
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_untagged_update
 * Purpose:
 *      bcm_petra_l3_untagged_update
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 *      Not Supported
 */
int 
bcm_petra_l3_untagged_update(
    int unit)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_l3_untagged_update is not available"))); 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_intf_create
 * Purpose:
 *      Create a new L3 interface.
 * Parameters:
 *      unit - (IN) Unit number.
 *      intf - (IN/OUT) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_intf_create(
    int unit, 
    bcm_l3_intf_t *intf)
{
    int status = BCM_E_NONE;
    SOC_PPC_VSI_ID vsi;
    SOC_PPC_VSI_INFO vsi_info;
    bcm_l3_intf_t intf_ori;
    int used_intf;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_L3_UNIT_INIT_CHECK;

    /* Input parameters check. */
    BCMDNX_NULL_CHECK(intf);

    vsi = intf->l3a_vid;   

    if (intf->l3a_flags & BCM_L3_WITH_ID) {
      if(intf->l3a_intf_id != intf->l3a_vid){
        LOG_ERROR(BSL_LS_BCM_L3,
                  (BSL_META_U(unit,
                              "BCM_L3_WITH_ID not implemented\n")));
        BCM_RETURN_VAL_EXIT(BCM_E_UNAVAIL);
      }
    } else {
      intf->l3a_intf_id = intf->l3a_vid;
    }

    if (intf->l3a_flags & BCM_L3_REPLACE) {
        if (!(intf->l3a_flags & BCM_L3_WITH_ID)) {
            LOG_ERROR(BSL_LS_BCM_L3,
                      (BSL_META_U(unit,
                                  "BCM_L3_REPLACE is supported only with BCM_L3_WITH_ID\n")));
            BCM_RETURN_VAL_EXIT(BCM_E_PARAM);
        }
    }

    if ((!intf->l3a_vid) || 
        (intf->l3a_vid && (intf->l3a_vid >= SOC_DPP_CONFIG(unit)->l3.nof_rifs) && (SOC_DPP_CONFIG(unit)->l3.nof_rifs >= BCM_VLAN_DEFAULT))) {
        if (!(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mim_spb_enable", 0) && _BCM_DPP_VLAN_IS_BVID(unit, intf->l3a_vid))) {
           LOG_ERROR(BSL_LS_BCM_L3,
                     (BSL_META_U(unit,
                              "invalid vid(0x%x)\n"), intf->l3a_vid));
           BCM_RETURN_VAL_EXIT(BCM_E_PARAM);
        }
    }

    if (!DPP_VRF_VALID(unit, intf->l3a_vrf)) {
        LOG_ERROR(BSL_LS_BCM_L3,
                  (BSL_META_U(unit,
                              "invalid vrf(0x%x)\n"), intf->l3a_vrf));
        BCM_RETURN_VAL_EXIT(BCM_E_PARAM);
    }

    if (BCM_MAC_IS_ZERO(intf->l3a_mac_addr) || BCM_MAC_IS_MCAST(intf->l3a_mac_addr)) {
        LOG_ERROR(BSL_LS_BCM_L3,
                  (BSL_META_U(unit,
                              "invalid mac address(%p)\n"), intf->l3a_mac_addr));
        BCM_RETURN_VAL_EXIT(BCM_E_PARAM);
    }

    if (!BCM_TTL_VALID(intf->l3a_ttl)) {
        LOG_ERROR(BSL_LS_BCM_L3,
                  (BSL_META_U(unit,
                              "invalid TTL(0x%x)\n"), intf->l3a_ttl));
        BCM_RETURN_VAL_EXIT(BCM_E_PARAM);
    }

    if (!BCM_DPP_MTU_VALID(intf->l3a_mtu)) {
        LOG_ERROR(BSL_LS_BCM_L3,
                  (BSL_META_U(unit,
                              "invalid MTU(0x%x)\n"), intf->l3a_mtu));
        BCM_RETURN_VAL_EXIT(BCM_E_PARAM);
    }

    if (!BCM_DPP_MTU_VALID(intf->l3a_mtu_forwarding)) {
        LOG_ERROR(BSL_LS_BCM_L3,
                  (BSL_META_U(unit,
                              "invalid MTU(0x%x)\n"), intf->l3a_mtu_forwarding));
        BCM_RETURN_VAL_EXIT(BCM_E_PARAM);
    }

    /* the field native_routing_vlan_tags is used from QAX */
    if (SOC_IS_JERICHO_AND_BELOW(unit) && SOC_IS_ROO_ENABLE(unit) && !BCM_DPP_NATIVE_ROUTING_VLAN_TAGS_VALID(intf->native_routing_vlan_tags)) {
        LOG_ERROR(BSL_LS_BCM_L3,
                  (BSL_META_U(unit,
                              "invalid number of vlan tags expected(0x%x)\n"), intf->native_routing_vlan_tags));
        BCM_RETURN_VAL_EXIT(BCM_E_PARAM);
    }

    /* For QAX/Jericho+, number of vlan tag expected is no longer used.
       we'll only check that the caller didn't set a value other than the initiazation value */
    if (SOC_IS_JERICHO_PLUS(unit) && intf->native_routing_vlan_tags != 0) {
        LOG_ERROR(BSL_LS_BCM_L3,
                  (BSL_META_U(unit,
                              "number of vlan tags expected is deprecated in QAX/Jericho+. \n")));
        BCM_RETURN_VAL_EXIT(BCM_E_PARAM);
    }


    /* interface class should be 0 */
    if (!intf->l3a_group == 0) {
        LOG_ERROR(BSL_LS_BCM_L3,
                  (BSL_META_U(unit,
                              "invalid interface class(0x%x)\n"), intf->l3a_group));
        BCM_RETURN_VAL_EXIT(BCM_E_PARAM);
    }

    /*
     * soc_ppd_mymac_msb_set
     * soc_ppd_vsi_info_set
     * soc_ppd_rif_vsid_map_set
     * soc_ppd_mymac_set
     * soc_ppd_rif_native_routing_vlan_tags_set
     */

    /* need to write data only if creating a new one or replacing an old one */
    if (!(intf->l3a_flags & BCM_L3_REPLACE)) {
        status = _bcm_ppd_vsi_info_get(unit,vsi,&vsi_info);
        BCMDNX_IF_ERR_EXIT(status);
        if (vsi_info.enable_my_mac) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("interface exists for vsi")));
        }
    }
    else {
        bcm_l3_intf_t_init(&intf_ori);
        intf_ori.l3a_intf_id = intf->l3a_intf_id;

        /* Check if L3 intf already exists. */
        status = bcm_petra_l3_intf_get(unit, &intf_ori);
        BCMDNX_IF_ERR_EXIT(status);
    }
    DPP_L3_LOCK_TAKE;

    if (status == BCM_E_NONE)
    {
        status = _bcm_ppd_vsi_info_set(unit, vsi, 0x1,intf);
        if (status == BCM_E_NONE)
        {
            status = _bcm_ppd_rif_vsid_map_set(unit, intf, (intf->l3a_flags & BCM_L3_REPLACE) ? RIF_VSID_MAP_SET_ACTION_UPDATE : RIF_VSID_MAP_SET_ACTION_CREATE);
            if (status == BCM_E_NONE)
            {
                status = _bcm_ppd_mymac_set(unit, vsi, intf->l3a_mac_addr);
                if (status == BCM_E_NONE) 
                {
                    /* set number of VLAN tags expected when interface is used for native routing */
                    if (SOC_IS_JERICHO_AND_BELOW(unit) &&  SOC_IS_ROO_ENABLE(unit) && 
                       !((soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mim_spb_enable", 0)) && _BCM_DPP_VLAN_IS_BVID(unit, intf->l3a_vid))) {
                        status = soc_ppd_rif_native_routing_vlan_tags_set(unit, intf->l3a_intf_id, intf->native_routing_vlan_tags);
                    }
                }
            }
        }
    }

    intf->l3a_intf_id = vsi;

    DPP_L3_LOCK_RELEASE;
    /*if (!(intf->l3a_flags & BCM_L3_WITH_ID))*/

    if (status == BCM_E_NONE) {
        BCMDNX_IF_ERR_EXIT(L3_ACCESS.used_intf.get(unit, &used_intf));
        ++used_intf;
        BCMDNX_IF_ERR_EXIT(L3_ACCESS.used_intf.set(unit, used_intf));
    }else {
        /*Clean up if creating failed.*/
        BCMDNX_IF_ERR_REPORT(bcm_petra_l3_intf_delete(unit, intf));
    }

    BCMDNX_IF_ERR_EXIT(status);
exit:
    BCMDNX_FUNC_RETURN;
}



/*
 * In case set, do not return error in case intf is not found
 */
#define _BCM_PETRA_L3_INTF_IGNORE_NOT_FOUND (0x1)
/*
 * Internal delete intf
 * Flags more indications to API
 * _BCM_PETRA_L3_INTF_XXX 
 */
STATIC int 
_bcm_petra_l3_intf_delete(
                         int unit,
                         uint32 flags,
                         bcm_l3_intf_t *intf,
                         uint8 *is_found)
{
    int status = BCM_E_NONE;
    SOC_PPC_VSI_ID vsi;
    SOC_PPC_VSI_INFO vsi_info;
    uint8 is_found_tmp = 1;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_L3_UNIT_INIT_CHECK;

    DPP_L3_LOCK_TAKE;

    BCMDNX_NULL_CHECK(intf);

    vsi = intf->l3a_intf_id;

    if (vsi <= 0 || vsi > SOC_DPP_CONFIG(unit)->l3.nof_rifs-1) {
        if (!(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mim_spb_enable", 0)) || !_BCM_DPP_VLAN_IS_BVID(unit, vsi)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid interface")));
        }
    }

    status = _bcm_ppd_vsi_info_get(unit,vsi,&vsi_info);
    BCMDNX_IF_ERR_EXIT(status);

    if (!vsi_info.enable_my_mac) {
        is_found_tmp = 0;
        if (!(flags & _BCM_PETRA_L3_INTF_IGNORE_NOT_FOUND)) {
            BCMDNX_ERR_EXIT_VERB_MSG(BCM_E_NOT_FOUND,
                                     (_BSL_BCM_MSG("Intf not found")));
        }
    } else {
        is_found_tmp = 1;
    }

    if (is_found_tmp) {
        intf->l3a_vid = intf->l3a_intf_id;
        status = _bcm_ppd_vsi_info_set(unit, vsi, 0x0,intf);
        if (status == BCM_E_NONE) {
            status = _bcm_ppd_rif_vsid_map_set(unit, intf, RIF_VSID_MAP_SET_ACTION_DELETE);
            if (status == BCM_E_NONE) {
                if (SOC_IS_JERICHO_AND_BELOW(unit) && SOC_IS_ROO_ENABLE(unit) && 
                   !((soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mim_spb_enable", 0)) && _BCM_DPP_VLAN_IS_BVID(unit, vsi))) {
                    status = soc_ppd_rif_native_routing_vlan_tags_set(unit,
                                                          intf->l3a_intf_id, 0);
                    BCMDNX_IF_ERR_EXIT(status);
                }
            }
        }
    }

    if (is_found) {
        *is_found = is_found_tmp;
    }

    BCMDNX_IF_ERR_EXIT(status);
exit:
    DPP_L3_LOCK_RELEASE;
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      bcm_petra_l3_intf_delete
 * Purpose:
 *      Delete an L3 interface.
 * Parameters:
 *      unit - (IN) Unit number.
 *      intf - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcm_petra_l3_intf_delete(
                         int unit, 
                         bcm_l3_intf_t *intf)
{
  return _bcm_petra_l3_intf_delete(unit, 0x0, intf, NULL);
}


/*
 * Function:
 *      bcm_petra_l3_intf_delete_all
 * Purpose:
 *      Delete all L3 interfaces.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_intf_delete_all(
    int unit)
{
    int status = BCM_E_NONE;
    bcm_l3_intf_t intf;
    int i;
    uint8 is_found;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_L3_UNIT_INIT_CHECK;

    DPP_L3_LOCK_TAKE;

    bcm_l3_intf_t_init(&intf);
    
    for (i=1; i < SOC_DPP_CONFIG(unit)->l3.nof_rifs; ++i) {
      intf.l3a_intf_id = i;
      /* do not return error in case of not found */
      status = _bcm_petra_l3_intf_delete(unit,_BCM_PETRA_L3_INTF_IGNORE_NOT_FOUND,&intf,&is_found);
      BCMDNX_IF_ERR_EXIT(status);      
    }

exit:
    DPP_L3_LOCK_RELEASE;
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_intf_get
 * Purpose:
 *      Given the L3 interface number, return the interface
 *      information.
 * Parameters:
 *      unit - (IN) Unit number.
 *      intf - (IN/OUT) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_intf_get(
    int unit, 
    bcm_l3_intf_t *intf)
{
    int status = BCM_E_NONE;
    SOC_PPC_VSI_INFO vsi_info;
    SOC_PPC_VSI_ID vsi;
    int is_tunneled = 0;
    int old_profile;
    bcm_dpp_vsi_egress_profile_t vsi_egress_profile_data;
    SOC_PPC_EG_ENCAP_ENTRY_INFO encap_entry_info[1];
    uint32 next_eep[1];
    uint32 nof_entries;
    uint32 l3a_mtu = 0, l3a_mtu_forwarding = 0;
    uint32 ret;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_L3_UNIT_INIT_CHECK;

    DPP_L3_LOCK_TAKE;

    if (intf->l3a_intf_id == 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("l3a_intf_id = 0 is not supported!\n")));
    }

    vsi = intf->l3a_intf_id;

    if (vsi > SOC_DPP_CONFIG(unit)->l3.nof_rifs-1) {
        if (!(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mim_spb_enable", 0)) || !_BCM_DPP_VLAN_IS_BVID(unit, vsi)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VSI-RIF %d not matched to valid range"),vsi));
        }			
    }

    /* check if l3-itf is tunnel, get tunnel id */
    if (!(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mim_spb_enable", 0)) && _BCM_DPP_VLAN_IS_BVID(unit, vsi)) {
       status =  _bcm_ppd_l3_intf_is_tunneled(unit,intf,&is_tunneled,&intf->l3a_tunnel_idx);
    }
    if (status == BCM_E_NONE) {

        if(!is_tunneled) {
            intf->l3a_tunnel_idx = 0;
        }
		
        intf->l3a_vid = intf->l3a_intf_id;
        status = _bcm_ppd_vsi_info_get(unit,intf->l3a_vid,&vsi_info);
        if (status == BCM_E_NONE) {
            if(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mtu_advanced_mode", 0)) {

                ret = _bcm_dpp_am_template_vsi_egress_profile_mapping_get(unit, vsi, &old_profile);
                BCMDNX_IF_ERR_EXIT(ret);

                if (old_profile != 0) {
                    /* MTU profiles for Forwarding layer and Link Layer are the same */
                    ret = soc_ppd_vsi_egress_mtu_get(unit, FALSE, old_profile, &l3a_mtu);
                    BCM_SAND_IF_ERR_EXIT(ret);

                    ret = soc_ppd_vsi_egress_mtu_get(unit, TRUE, old_profile, &l3a_mtu_forwarding);
                    BCM_SAND_IF_ERR_EXIT(ret);
                }

                intf->l3a_mtu            = l3a_mtu;
                intf->l3a_mtu_forwarding = l3a_mtu_forwarding;
            } else {
                /* getting MTU using template mngr */
                status = _bcm_dpp_am_template_vsi_egress_profile_data_get(unit, vsi, &vsi_egress_profile_data);
                if (status == BCM_E_NONE) {
                    intf->l3a_mtu = vsi_egress_profile_data.mtu;
                }
            }

            /* skip egress RIF when spb is enabled and BVID-Vlan as it is not configured in l3 intf create */
            if (!(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mim_spb_enable", 0)) || !_BCM_DPP_VLAN_IS_BVID(unit, intf->l3a_vid)) {
                if (vsi_info.enable_my_mac) {

                    ret = soc_ppd_eg_encap_entry_get(unit,
                                                     SOC_PPC_EG_ENCAP_EEP_TYPE_VSI,
                                                     vsi, 0,
                                                     encap_entry_info, next_eep,
                                                     &nof_entries);
                    BCM_SAND_IF_ERR_EXIT(ret);

                    if ((*encap_entry_info).entry_val.vsi_info.remark_profile != 0) {
                        BCM_INT_QOS_MAP_REMARK_SET(intf->dscp_qos.qos_map_id,(*encap_entry_info).entry_val.vsi_info.remark_profile);
                    }
                }

                if (!vsi_info.enable_my_mac) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("enable_my_mac not set for vsi")));
                }
                status = _bcm_ppd_rif_vsid_map_get(unit,intf);
            }
			
            if (status == BCM_E_NONE) {
                status = _bcm_ppd_mymac_get(unit, intf->l3a_vid, intf->l3a_mac_addr);
            }

            if (status == BCM_E_NONE) {
                if (SOC_IS_JERICHO_AND_BELOW(unit) &&  SOC_IS_ROO_ENABLE(unit) && 
                   !((soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mim_spb_enable", 0)) && _BCM_DPP_VLAN_IS_BVID(unit, intf->l3a_vid))) {
                    status = soc_ppd_rif_native_routing_vlan_tags_get(unit, intf->l3a_intf_id, &(intf->native_routing_vlan_tags));
                }
            }
        }
    }

    BCMDNX_IF_ERR_EXIT(status);
exit:
    DPP_L3_LOCK_RELEASE;
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_intf_find
 * Purpose:
 *      Search for L3 interface by MAC address and VLAN.
 * Parameters:
 *      unit - (IN) Unit number.
 *      intf - (IN/OUT) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_intf_find(
    int unit, 
    bcm_l3_intf_t *intf)
{
    int status = BCM_E_NONE;
    bcm_mac_t addr;
    int i;
    SOC_PPC_VSI_INFO vsi_info;
    SOC_PPC_VSI_ID vsi;
    uint32 l3a_mtu = 0, l3a_mtu_forwarding = 0;
    int old_profile;
    bcm_dpp_vsi_egress_profile_t vsi_egress_profile_data;
    uint32 ret;

    /*
    int ac_index=intf->l3a_vid; 
    */ 

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_L3_UNIT_INIT_CHECK;

    BCMDNX_NULL_CHECK(intf);

    /*
    status = _bcm_ppd_l2_lif_ac_get(unit, ac_index, intf); 
    */ 
    if (BCM_MAC_IS_ZERO(intf->l3a_mac_addr) || BCM_MAC_IS_MCAST(intf->l3a_mac_addr)) {
        LOG_ERROR(BSL_LS_BCM_L3,
                  (BSL_META_U(unit,
                              "invalid mac address(%p)\n"), intf->l3a_mac_addr));
        BCM_RETURN_VAL_EXIT(BCM_E_PARAM);
    }
    vsi = intf->l3a_vid;
    if (vsi <= 0 || vsi > SOC_DPP_CONFIG(unit)->l3.nof_rifs-1) {
	    if (!(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mim_spb_enable", 0)) || !_BCM_DPP_VLAN_IS_BVID(unit, intf->l3a_vid)) {
           BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid interface")));
        }
    }

    intf->l3a_intf_id = intf->l3a_vid;
    sal_memcpy(addr, intf->l3a_mac_addr, SOC_SAND_PP_MAC_ADDRESS_NOF_U8);

    DPP_L3_LOCK_TAKE;

    status = _bcm_ppd_vsi_info_get(unit,intf->l3a_vid,&vsi_info);
    if (status == BCM_E_NONE) {
      if(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mtu_advanced_mode", 0)) {
            ret = _bcm_dpp_am_template_vsi_egress_profile_mapping_get(unit, vsi, &old_profile);
            BCMDNX_IF_ERR_EXIT(ret);

            if (old_profile != 0) {
                /* MTU profiles for Forwarding layer and Link Layer are the same */
                ret = soc_ppd_vsi_egress_mtu_get(unit, FALSE, old_profile, &l3a_mtu);
                BCM_SAND_IF_ERR_EXIT(ret);

                ret = soc_ppd_vsi_egress_mtu_get(unit, TRUE, old_profile, &l3a_mtu_forwarding);
                BCM_SAND_IF_ERR_EXIT(ret);
            }

            intf->l3a_mtu            = l3a_mtu;
            intf->l3a_mtu_forwarding = l3a_mtu_forwarding;
        } else {
            /* getting MTU using template mngr */
            status = _bcm_dpp_am_template_vsi_egress_profile_data_get(unit, vsi, &vsi_egress_profile_data);
            if (status == BCM_E_NONE) {
                intf->l3a_mtu = vsi_egress_profile_data.mtu;
            }
        }
            
      if (!vsi_info.enable_my_mac) {
        status = BCM_E_NOT_FOUND;
      }
      if (status == BCM_E_NONE) {
          status = _bcm_ppd_rif_vsid_map_get(unit,intf);
        
          /* Check that the if MyMac equals the specified MAC */
          if (status == BCM_E_NONE) {
            status = _bcm_ppd_mymac_get(unit, intf->l3a_vid, intf->l3a_mac_addr);
            if (status == BCM_E_NONE) {
                for (i=0; i < SOC_SAND_PP_MAC_ADDRESS_NOF_U8; ++i) {
                    if (addr[i] != intf->l3a_mac_addr[i]) {
                        status = BCM_E_NOT_FOUND;
                        break;
                    }
                }
            }
          }
      }
    }

    DPP_L3_LOCK_RELEASE;

    BCMDNX_IF_ERR_EXIT(status);
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_intf_find_vlan
 * Purpose:
 *      Search for L3 interface by VLAN only.
 * Parameters:
 *      unit - (IN) Unit number.
 *      intf - (IN/OUT) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_intf_find_vlan(
    int unit, 
    bcm_l3_intf_t *intf)
{
    int status = BCM_E_NONE;
    SOC_PPC_VSI_ID vsi;

    /*
    int ac_index=0; 
    */ 

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_L3_UNIT_INIT_CHECK;

    vsi = intf->l3a_vid;
    if (vsi <= 0 || vsi > SOC_DPP_CONFIG(unit)->l3.nof_rifs-1) {
      BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid interface")));
    }

    DPP_L3_LOCK_TAKE;

    /*
    status = _bcm_ppd_l2_lif_ac_get(unit, ac_index, intf); 
    */
    
    intf->l3a_intf_id=intf->l3a_vid;

    status = bcm_petra_l3_intf_get(unit,intf);

    DPP_L3_LOCK_RELEASE;

    BCMDNX_IF_ERR_EXIT(status);
exit:
    BCMDNX_FUNC_RETURN;
}


/* 
 *  given l3-interface that include tunnels, 
 *   set the LL EEP of this interface tunnels.
 *  L3-interface can be RIF or ENCAP
 */
int _bcm_tunnel_intf_ll_eep_set(
    int unit,
    bcm_if_t intf,
    int tunnel_eep,
    int ll_eep)
{
    int eep, is_tunneled;
    SOC_PPC_EG_ENCAP_ENTRY_INFO *encap_entry_info = NULL;
    SOC_PPC_EG_ENCAP_MPLS_ENCAP_INFO mpls_encap_info;
    SOC_PPC_EG_ENCAP_IPV4_ENCAP_INFO ipv4_encap_info;
    SOC_PPC_EG_ENCAP_SWAP_INFO mpls_swap_info;
    SOC_PPC_EG_ENCAP_POP_INFO mpls_pop_info;
    SOC_PPC_EG_ENCAP_DATA_INFO mpls_data_info;
    uint32 next_eep[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX], nof_entries;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    int rv = BCM_E_NONE;
    int update_eep=0;
    int vsi, tmp_ll_eep;
    int global_lif, local_lif;
    _bcm_lif_type_e lif_usage = _bcmDppLifTypeAny;
    SOC_PPC_EG_ENCAP_ENTRY_TYPE last_entry_type = SOC_PPC_NOF_EG_ENCAP_ENTRY_TYPES_ARAD;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    soc_sand_dev_id = (unit);

    /* check if there is tunnel associated with intf at all */
    rv = _bcm_tunnel_intf_is_tunnel(unit, intf, &is_tunneled, &eep, &vsi, &tmp_ll_eep);
    BCMDNX_IF_ERR_EXIT(rv);

    /* this interface has no label */
    if (is_tunneled == 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Interface has no tunnel")));
    }

    BCMDNX_ALLOC(encap_entry_info, sizeof(SOC_PPC_EG_ENCAP_ENTRY_INFO)*SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX, "_bcm_tunnel_intf_ll_eep_set.encap_entry_info");
    if (encap_entry_info == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
    }

    if (SOC_IS_JERICHO(unit))  
    {
        global_lif = eep;
        rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, global_lif, &local_lif);
        BCMDNX_IF_ERR_EXIT(rv);

        if (SOC_DPP_CONFIG(unit)->pp.l2_src_encap_enable == 1) {
            rv = _bcm_dpp_local_lif_sw_resources_lif_usage_get(unit, -1, local_lif, NULL, &lif_usage);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }
    else
    {
        local_lif = global_lif = eep;
    }

    if (lif_usage == _bcmDppLifTypeL2SrcEncap) {
        BCM_EXIT;
    }

    soc_sand_rv =
        soc_ppd_eg_encap_entry_get(soc_sand_dev_id,
                               SOC_PPC_EG_ENCAP_EEP_TYPE_TUNNEL_EEP, local_lif, 2,
                               encap_entry_info, next_eep, &nof_entries);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    SOC_PPC_EG_ENCAP_MPLS_ENCAP_INFO_clear(&mpls_encap_info);
    SOC_PPC_EG_ENCAP_IPV4_ENCAP_INFO_clear(&ipv4_encap_info);
    SOC_PPC_EG_ENCAP_SWAP_INFO_clear(&mpls_swap_info);
    SOC_PPC_EG_ENCAP_POP_INFO_clear(&mpls_pop_info);
    SOC_PPC_EG_ENCAP_DATA_INFO_clear(&mpls_data_info);

    /* get last tunnel in this labels */
    if ((encap_entry_info[1].entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_MPLS_ENCAP)
        && (encap_entry_info[1].entry_val.mpls_encap_info.nof_tunnels > 0)) {
        /* memcpy, cause override, magic-num, so here one by one copy */
        mpls_encap_info.nof_tunnels = encap_entry_info[1].entry_val.mpls_encap_info.nof_tunnels;
        mpls_encap_info.orientation = encap_entry_info[1].entry_val.mpls_encap_info.orientation;
        mpls_encap_info.oam_lif_set = encap_entry_info[1].entry_val.mpls_encap_info.oam_lif_set;
        mpls_encap_info.drop = encap_entry_info[1].entry_val.mpls_encap_info.drop;
        mpls_encap_info.out_vsi = vsi;
        mpls_encap_info.tunnels[0].push_profile = encap_entry_info[1].entry_val.mpls_encap_info.tunnels[0].push_profile;
        mpls_encap_info.tunnels[0].tunnel_label = encap_entry_info[1].entry_val.mpls_encap_info.tunnels[0].tunnel_label;
        mpls_encap_info.tunnels[1].push_profile = encap_entry_info[1].entry_val.mpls_encap_info.tunnels[1].push_profile;
        mpls_encap_info.tunnels[1].tunnel_label = encap_entry_info[1].entry_val.mpls_encap_info.tunnels[1].tunnel_label;
        update_eep = next_eep[0];
        last_entry_type = SOC_PPC_EG_ENCAP_ENTRY_TYPE_MPLS_ENCAP;
        if (SOC_IS_JERICHO(unit)) {
            mpls_encap_info.outlif_profile = encap_entry_info[1].entry_val.mpls_encap_info.outlif_profile;
        }
    } else
        if ((encap_entry_info[0].entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_MPLS_ENCAP)
            && (encap_entry_info[0].entry_val.mpls_encap_info.nof_tunnels > 0)) {
        /* memcpy, cause override, magic-num, so here one by one copy */
        mpls_encap_info.nof_tunnels = encap_entry_info[0].entry_val.mpls_encap_info.nof_tunnels;
        mpls_encap_info.orientation = encap_entry_info[0].entry_val.mpls_encap_info.orientation;
        mpls_encap_info.oam_lif_set = encap_entry_info[0].entry_val.mpls_encap_info.oam_lif_set;
        mpls_encap_info.drop = encap_entry_info[0].entry_val.mpls_encap_info.drop;
        mpls_encap_info.out_vsi = vsi;
        mpls_encap_info.tunnels[0].push_profile = encap_entry_info[0].entry_val.mpls_encap_info.tunnels[0].push_profile;
        mpls_encap_info.tunnels[0].tunnel_label = encap_entry_info[0].entry_val.mpls_encap_info.tunnels[0].tunnel_label;
        mpls_encap_info.tunnels[1].push_profile = encap_entry_info[0].entry_val.mpls_encap_info.tunnels[1].push_profile;
        mpls_encap_info.tunnels[1].tunnel_label = encap_entry_info[0].entry_val.mpls_encap_info.tunnels[1].tunnel_label;
        update_eep = tunnel_eep;
        last_entry_type = SOC_PPC_EG_ENCAP_ENTRY_TYPE_MPLS_ENCAP;
        if (SOC_IS_JERICHO(unit)) {
            mpls_encap_info.outlif_profile = encap_entry_info[0].entry_val.mpls_encap_info.outlif_profile;
        }
    } else 
       if (encap_entry_info[0].entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_IPV4_ENCAP) {
        /* memcpy, cause override, magic-num, so here one by one copy */
         ipv4_encap_info.dest.dest =
           encap_entry_info[0].entry_val.ipv4_encap_info.dest.dest;
         ipv4_encap_info.dest.src_index =
           encap_entry_info[0].entry_val.ipv4_encap_info.dest.src_index;
         ipv4_encap_info.dest.ttl_index =
           encap_entry_info[0].entry_val.ipv4_encap_info.dest.ttl_index;
         ipv4_encap_info.dest.tos_index =
           encap_entry_info[0].entry_val.ipv4_encap_info.dest.tos_index;
         ipv4_encap_info.dest.enable_gre =
           encap_entry_info[0].entry_val.ipv4_encap_info.dest.enable_gre;
         ipv4_encap_info.dest.encapsulation_mode =
           encap_entry_info[0].entry_val.ipv4_encap_info.dest.encapsulation_mode;
         /* encapsulation mode index is for jericho B0, QAX and above. */
         ipv4_encap_info.dest.encapsulation_mode_index =
            encap_entry_info[0].entry_val.ipv4_encap_info.dest.encapsulation_mode_index;
         ipv4_encap_info.oam_lif_set = encap_entry_info[0].entry_val.ipv4_encap_info.oam_lif_set;
         ipv4_encap_info.drop = encap_entry_info[0].entry_val.ipv4_encap_info.drop;
         ipv4_encap_info.out_vsi = vsi;
         update_eep = tunnel_eep;
         last_entry_type = SOC_PPC_EG_ENCAP_ENTRY_TYPE_IPV4_ENCAP;
        if (SOC_IS_JERICHO(unit)) {
            ipv4_encap_info.outlif_profile = encap_entry_info[0].entry_val.ipv4_encap_info.outlif_profile;
        }
       } else if (encap_entry_info[0].entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_IPV6_ENCAP) {
            if ((SOC_DPP_CONFIG(unit)->pp.ipv6_tunnel_encap_mode == SOC_DPP_IP6_TUNNEL_ENCAP_MODE_NORMAL) && 
                (encap_entry_info[1].entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_IPV4_ENCAP)) {
                /* in case of IPv6 tunnel connect me to LL*/
                /* already done on egress create */
                soc_sand_rv = soc_ppd_eg_encap_lif_field_set(
                          soc_sand_dev_id,
                          next_eep[0],
                          SOC_PPC_EG_ENCAP_ENTRY_UPDATE_NEXT_LIF,
                          ll_eep
                      );
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                /* done as pointer was updated */
            } else {
                /* in case of IPv6 tunnel connect me to LL*/
                /* already done on egress create */
                soc_sand_rv = soc_ppd_eg_encap_lif_field_set(
                          soc_sand_dev_id,
                          local_lif,
                          SOC_PPC_EG_ENCAP_ENTRY_UPDATE_NEXT_LIF,
                          ll_eep
                      );
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                /* done as pointer was updated */
            }
            BCM_EXIT;
    } else if (encap_entry_info[0].entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_PWE) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("3 labels are not supported")));
    } else if (encap_entry_info[0].entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_SWAP_CMND) {
        mpls_swap_info.swap_label = 
          encap_entry_info[0].entry_val.swap_info.swap_label;
        mpls_swap_info.out_vsi = vsi; 
        update_eep = tunnel_eep;
        last_entry_type = SOC_PPC_EG_ENCAP_ENTRY_TYPE_SWAP_CMND;
        mpls_swap_info.remark_profile = encap_entry_info[0].entry_val.swap_info.remark_profile;
        if (SOC_IS_JERICHO(unit)) {
            mpls_swap_info.outlif_profile = encap_entry_info[0].entry_val.swap_info.outlif_profile;
        }
    } else if (encap_entry_info[0].entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_POP_CMND) {
        mpls_pop_info.out_vsi = vsi;
        mpls_pop_info.oam_lif_set = 
          encap_entry_info[0].entry_val.pop_info.oam_lif_set;
        mpls_pop_info.drop = 
          encap_entry_info[0].entry_val.pop_info.drop;         
        mpls_pop_info.ethernet_info.has_cw = 
          encap_entry_info[0].entry_val.pop_info.ethernet_info.has_cw;
        mpls_pop_info.ethernet_info.tpid_profile = 
          encap_entry_info[0].entry_val.pop_info.ethernet_info.tpid_profile;
        mpls_pop_info.model = 
          encap_entry_info[0].entry_val.pop_info.model;
        mpls_pop_info.pop_next_header = 
          encap_entry_info[0].entry_val.pop_info.pop_next_header;
        mpls_pop_info.pop_type = 
          encap_entry_info[0].entry_val.pop_info.pop_type;        
        update_eep = tunnel_eep;
        last_entry_type = SOC_PPC_EG_ENCAP_ENTRY_TYPE_POP_CMND;
        if (SOC_IS_JERICHO(unit)) {
            mpls_pop_info.outlif_profile = encap_entry_info[0].entry_val.pop_info.outlif_profile;
        }
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("no tunnel in give interface")));
    }

    if (last_entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_IPV4_ENCAP) {
      soc_sand_rv =
         soc_ppd_eg_encap_ipv4_entry_add(soc_sand_dev_id, update_eep,
                                       &ipv4_encap_info, ll_eep);
    } else if (last_entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_MPLS_ENCAP) {
      soc_sand_rv =
          soc_ppd_eg_encap_mpls_entry_add(soc_sand_dev_id, update_eep,
                                    &mpls_encap_info, ll_eep);
    } else if (last_entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_POP_CMND) {
      soc_sand_rv =
          soc_ppd_eg_encap_pop_command_entry_add(soc_sand_dev_id, update_eep,
                                    &mpls_pop_info, ll_eep);
    } else if (last_entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_SWAP_CMND) {
      soc_sand_rv =
          soc_ppd_eg_encap_swap_command_entry_add(soc_sand_dev_id, update_eep,
                                    &mpls_swap_info, ll_eep);
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("no tunnel in give interface")));
    }
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:    
    BCM_FREE(encap_entry_info);
    BCMDNX_FUNC_RETURN;
}

int 
_bcm_petra_l3_egress_create_verify(
    int unit, 
    uint32 flags, 
    bcm_l3_egress_t *egr, 
    bcm_if_t *if_id)
{
    int status = BCM_E_NONE;
    uint8 is_ingress = 1;
    uint8 is_egress = 1;
    int ll_eep_index = 0;
    uint8 encap_with_id = 0;
    int update = 0;
    bcm_l3_egress_t egr_ori;    
    bcm_if_t l3_if_val = 0, if_get = 0;
    int ll_eep_index_ori = 0;
    int rv;
    int max_nof_lags = 0;
    bcm_gport_t gport = 0;
    SOC_PPC_FRWRD_FEC_ENTRY_INFO
    fec_info;
    
    BCMDNX_INIT_FUNC_DEFS;

    /* Verify parameters */
    if (BCM_L3_ITF_TYPE_IS_RIF(egr->intf)) {    
        if (egr->intf < 0 || egr->intf > SOC_DPP_CONFIG(unit)->l3.nof_rifs-1) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG_STR("l3 egress interface has to be between 0x%x and 0x%x\n"), 0, SOC_DPP_CONFIG(unit)->l3.nof_rifs-1));
        }
    }

    /*check vlan range*/
    if (egr->vlan >= SOC_DPP_DEFS_GET(unit, nof_out_vsi)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("The vlan ID is out of range\n")));
    }

    if (egr->port == -1) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("The port is invalid\n")));
    }
    else if ((flags & BCM_L3_WITH_ID) && BCM_L3_ITF_TYPE_IS_FEC(*if_id)) {
        BCM_L3_ITF_FEC_TO_GPORT_FORWARD_GROUP(gport, *if_id);
        if (egr->port == gport) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, invalid configuration, FEC GPORT 0x%x can't points to same FEC GPORT 0x%x\n"), unit, egr->port, gport));
        }
    }
    else {
      rv = _bcm_dpp_gport_to_sand_pp_dest(unit, egr->port, &fec_info.dest);
      BCMDNX_IF_ERR_EXIT(rv);

      switch(fec_info.dest.dest_type)
      {
        case SOC_SAND_PP_DEST_SINGLE_PORT:
          _BCM_ERROR_IF_OUT_OF_RANGE(fec_info.dest.dest_val,
            (ARAD_IS_VOQ_MAPPING_INDIRECT(unit)?SOC_TMC_MAX_SYSTEM_PHYSICAL_PORT_ID:SOC_TMC_MAX_SYSTEM_PHYSICAL_PORT_ID_ARAD));
        break;
        case SOC_SAND_PP_DEST_MULTICAST:
          _BCM_ERROR_IF_OUT_OF_RANGE(fec_info.dest.dest_val,ARAD_MAX_MC_ID(unit));
        break;
        case SOC_SAND_PP_DEST_LAG:
          BCMDNX_IF_ERR_EXIT(soc_dpp_trunk_sw_db_max_nof_trunks_get(unit, &max_nof_lags));
          _BCM_ERROR_IF_OUT_OF_RANGE(fec_info.dest.dest_val, max_nof_lags - 1);
        break;
        case SOC_SAND_PP_DEST_EXPLICIT_FLOW:
          _BCM_ERROR_IF_OUT_OF_RANGE(fec_info.dest.dest_val,SOC_DPP_NOF_FLOWS_ARAD-1);
        break;
        case SOC_SAND_PP_DEST_FEC:
            if(SOC_IS_ARADPLUS_AND_BELOW(unit))
            {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Hierarchical FEC isn't supported on this device, FEC can't point to a FEC.\n")));
            }
            _BCM_ERROR_IF_OUT_OF_RANGE(fec_info.dest.dest_val,SOC_DPP_DEFS_GET(unit,nof_fecs)-1);
            break;
        case SOC_SAND_PP_DEST_TYPE_ROUTER:
            _BCM_ERROR_IF_OUT_OF_RANGE(fec_info.dest.dest_val,SOC_DPP_DEFS_GET(unit,nof_fecs)-1);
        break; 
        default:
        break;
      }

      
    }

    /* Validate failover_id type and range */
    if (egr->flags & ~BCM_L3_EGRESS_ONLY && _BCM_PPD_IS_VALID_FAILOVER_ID(egr->failover_id)) {
        if (_bcm_dpp_failover_is_valid_id(unit, egr->failover_id, DPP_FAILOVER_TYPE_FEC) ) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("The failover ID is out of range\n")));
        }
    }

    if (egr->flags & ~L3_EGRESS_SUPPORTED_FLAGS) {
         BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("The L3 egress flags provided are not supported\n")));
    }

    if ((egr->flags & BCM_L3_NATIVE_ENCAP) && (flags & BCM_L3_INGRESS_ONLY)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_L3_NATIVE_ENCAP flag not supported with BCM_L3_INGRESS_ONLY flag\n")));
    }

    if ((!SOC_IS_JERICHO_PLUS(unit)) && (egr->flags & BCM_L3_NATIVE_ENCAP)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_L3_NATIVE_ENCAP flag supported on Jericho+ and above\n")));
    }

    if (SOC_IS_ARADPLUS_AND_BELOW(unit) && (egr->flags & BCM_L3_CASCADED)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Cascaded FECs don't supported in this device, the BCM_L3_CASCADED flag should be removed.\n")));
    }

    encap_with_id = (egr->encap_id != 0) ? TRUE:FALSE;

    /* In case VIRTUAL unset we do ingress settings */
    is_ingress = (!(flags & BCM_L3_VIRTUAL_OBJECT)) ? TRUE:FALSE;
    /* In case INGRESS ONLY unset we do egress settings */
    is_egress = (!(flags & BCM_L3_INGRESS_ONLY)) ? TRUE:FALSE;

    if (!is_ingress && !is_egress) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("The L3 egress can't be both Ingress only and Egress only\n")));
    }

    /* In case of egress_only and replace and skip mac dst and skip vlan, no need to do replace check */
    if ((flags & BCM_L3_VIRTUAL_OBJECT) && 
        (flags & BCM_L3_REPLACE) &&
        (flags & BCM_L3_KEEP_DSTMAC) &&
        (flags & BCM_L3_KEEP_VLAN)) {
        BCM_EXIT;
    }

    /* Retreive Link Layer ID in case encap with id is set */
    if (encap_with_id) {
        _bcm_l3_encap_id_to_eep(unit, egr->encap_id, &ll_eep_index);
    }

    if (egr->flags & BCM_L3_ENCAP_SPACE_OPTIMIZED) {
        if (SOC_IS_ARADPLUS_A0(unit) && (soc_property_get(unit, spn_BCM886XX_ROO_ENABLE, 0) || soc_property_get(unit, spn_BCM886XX_ERSPAN_TUNNEL_ENABLE, 0))) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_L3_ENCAP_SPACE_OPTIMIZED mode is not supported when routing over overlay or ERSPAN is enabled\n")));
        }
    }

    /* Read the state of the flags */
    update = (flags & BCM_L3_REPLACE) ? TRUE : FALSE;

    if (update) {
        /* 1. replace check. BCM_L3_REPLACE is supported only with id */
        if (!is_ingress) {
            if (!encap_with_id) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_L3_EGRESS_REPLACE is supported only with valid encap_id when replce egress object\n")));
            }
            
            l3_if_val = BCM_L3_ITF_VAL_GET(egr->encap_id);
            BCM_L3_ITF_SET(if_get, BCM_L3_ITF_TYPE_LIF, l3_if_val);
        }
        else {
            if (!(flags & BCM_L3_WITH_ID)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_L3_EGRESS_REPLACE is supported only with valid FEC ID when replace ingress object\n")));
            }

            l3_if_val = BCM_L3_ITF_VAL_GET(*if_id);
            BCM_L3_ITF_SET(if_get, BCM_L3_ITF_TYPE_FEC, l3_if_val);
        }

        /* 2. replace check. check if object is already created */
        bcm_l3_egress_t_init(&egr_ori);       
        status = bcm_petra_l3_egress_get(unit, if_get, &egr_ori);
        BCMDNX_IF_ERR_EXIT(status);

        if (is_egress) {
             /* check if egress is already created */
            if (egr_ori.encap_id == 0) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_L3_EGRESS_REPLACE is supported only if old eep exists\n")));
            }

            if ((egr_ori.flags & BCM_L3_NATIVE_ENCAP) != (egr->flags & BCM_L3_NATIVE_ENCAP)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_L3_EGRESS_REPLACE don't support for ARP type change\n")));
            }
        }
       
        /* 3. replace check. check if replace Link Layer ID is possible */
        /* Retreive old Link Layer ID */
        _bcm_l3_encap_id_to_eep(unit, egr_ori.encap_id, &ll_eep_index_ori);        
        if (ll_eep_index != ll_eep_index_ori) {
            if (!(flags & BCM_L3_INGRESS_ONLY)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("BCM_L3_EGRESS_REPLACE can change encap_id only in case BCM_L3_INGRESS_ONLY\n")));
            } 
        }

        if (is_egress) {
            /* 4. replace check. check fields{mac, vlan, qos map id} stored in egress object */
            /* 4.1 check if replace mac is possible */
            if (sal_memcmp(egr->mac_addr, egr_ori.mac_addr, sizeof(sal_mac_addr_t))) {     
                if (flags & BCM_L3_KEEP_DSTMAC) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("BCM_L3_EGRESS_REPLACE cannot change mac because BCM_L3_KEEP_DSTMAC is set\n")));
                }
            }
            
            /* 4.2 check if replace vlan is possible */
            if (egr->vlan != egr_ori.vlan) {
                if (flags & BCM_L3_KEEP_VLAN) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("BCM_L3_EGRESS_REPLACE cannot change vlan because BCM_L3_KEEP_VLAN is set\n")));
                }
            }
        }

        if (is_ingress) {
            /* 5. check if replace failover id and failover if id is possible */
            if (((egr->failover_id == 0) && (egr_ori.failover_id != 0)) ||
                ((egr->failover_id != 0) && (egr_ori.failover_id == 0))) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("BCM_L3_EGRESS_REPLACE cannot change failover_id from !=0 to ==0 or from ==0 to !=0\n")));
            }                

            if (egr->failover_if_id != egr_ori.failover_if_id) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("BCM_L3_EGRESS_REPLACE cannot change failover_if_id\n")));
            }
        }
    }

exit:    
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      bcm_petra_l3_egress_create
 * Purpose:
 *      Create an Egress forwarding object.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN) <UNDEF>
 *      egr - (IN) Egress forwarding destination.
 *      if_id - (IN/OUT) L3 interface id pointing to Egress object.
 *                      This is an IN argument if either BCM_L3_REPLACE
 *                      or BCM_L3_WITH_ID are given in flags.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_egress_create(
    int unit, 
    uint32 flags, 
    bcm_l3_egress_t *egr, 
    bcm_if_t *if_id)
{
    int status = BCM_E_NONE;
    uint8 is_ingress = 1;
    uint8 is_egress = 1;
    int fec_index = 0,protect_fec_index;
    int ll_local_lif = 0, ll_global_lif = 0;
    int tunnel_local_lif = 0, tunnel_global_lif = 0;
    int fec_eep_pointer = 0;
    bcm_l3_intf_t l3_intf;
    int rif_entry_id=0;
    int out_rif = 0;    
    int intf_tunnel_local_lif = 0, intf_tunnel_global_lif = 0;
    SOC_PPC_FEC_ID fec_id = 0,protect_fec_id = 0;
    uint8 is_fec_eei_outlif = FALSE;
    uint8 is_tunnel = FALSE;
    uint8 is_tunnel_eei_mpls_command = FALSE;
    uint32 ret, nof_entries, next_eep[1];
    _bcm_dpp_am_fec_alloc_usage fec_usage;
    bcm_gport_t gport;
    SOC_PPC_FRWRD_FEC_PROTECT_TYPE
       protect_type_dummy;
    SOC_PPC_FRWRD_FEC_ENTRY_INFO
       working_fec;
    bcm_failover_t failover_id_dummy;
    SOC_PPC_EG_ENCAP_ENTRY_INFO
      encap_entry_info[1];
    uint32 fec_flags = 0;
    uint8 encap_with_id = 0;
    uint32 push_profile = 0;
    int is_last_dummy;
    uint8 update_fec_sw_db = 1;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_L3_UNIT_INIT_CHECK;

    DPP_L3_LOCK_TAKE;

    BCMDNX_NULL_CHECK(egr);
    BCMDNX_NULL_CHECK(if_id);

    /* Verify parameters */
    status = _bcm_petra_l3_egress_create_verify(unit, flags, egr, if_id);
    BCMDNX_IF_ERR_EXIT(status);

    encap_with_id = (egr->encap_id != 0) ? TRUE:FALSE;

    /* In case VIRTUAL unset we do ingress settings */
    is_ingress = (!(flags & BCM_L3_VIRTUAL_OBJECT)) ? TRUE:FALSE;
    /* In case INGRESS ONLY unset we do egress settings */
    is_egress = (!(flags & BCM_L3_INGRESS_ONLY)) ? TRUE:FALSE;
    /* In Jericho, the CASCADED flag indicates that the FEC is group B (pointed FEC). this flag is supported for Jericho and above devices. */
    fec_usage = (egr->flags & BCM_L3_CASCADED) ? _BCM_DPP_AM_FEC_ALLOC_USAGE_CASCADED : _BCM_DPP_AM_FEC_ALLOC_USAGE_STANDARD;
            
    bcm_l3_intf_t_init(&l3_intf);

    if (egr->mpls_label != BCM_MPLS_LABEL_INVALID) {
        is_tunnel_eei_mpls_command = TRUE;
    }
    else if (egr->intf > 0) {
        if (BCM_L3_ITF_TYPE_IS_RIF(egr->intf)) {
            l3_intf.l3a_intf_id = egr->intf;

            bcm_petra_l3_intf_get(unit, &l3_intf) ;
            /* Don't check return value because l3 interface can be allocated on remote device */

            intf_tunnel_global_lif = l3_intf.l3a_tunnel_idx;
            
        } else if (BCM_L3_ITF_TYPE_IS_LIF(egr->intf)) {
            intf_tunnel_global_lif = BCM_L3_ITF_VAL_GET(egr->intf);
			/* This flag is used to signal that the FEC output is EEI (Format C). 
			 * Otherwise it is OutLif (Format B).
			 */
            is_tunnel = TRUE; 
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Intf given is not valid 0x%x"),egr->intf));
        }
    }

    if (egr->flags & BCM_L3_ENCAP_SPACE_OPTIMIZED) {
        is_fec_eei_outlif = TRUE;
    }

    /* Retreive Link Layer ID in case encap with id is set */
    if (encap_with_id) {
        _bcm_l3_encap_id_to_eep(unit, egr->encap_id, &ll_global_lif);
        _BCM_ERROR_IF_OUT_OF_RANGE(ll_global_lif, SOC_DPP_CONFIG(unit)->pp.nof_global_out_lifs-1);
    }

    /* Allocate a FEC entry - ingress object */
    /* if virtual object flag set then don't allocate FEC table */
    if (is_ingress) {
        if (flags & BCM_L3_WITH_ID) {
            _bcm_l3_intf_to_fec(unit, *if_id, &fec_id);
            fec_index = (int)fec_id;
            BCM_L3_ITF_SET((*if_id), BCM_L3_ITF_TYPE_FEC, fec_id);

            fec_flags |= BCM_DPP_AM_FLAG_ALLOC_WITH_ID;
            if ((flags & BCM_L3_REPLACE)) {
                /* check it's already alloced otherwise error */
                status = bcm_dpp_am_fec_is_alloced(unit, 0 /*flags*/, 1 /*size*/, fec_index);
                if (status != BCM_E_EXISTS) {
                    BCMDNX_IF_ERR_EXIT(status);
                }

				/* In Jericho, check that the CASCADED/not-CASCADED matches.
				   No such limitation in QAX.*/
                if (SOC_IS_JERICHO(unit) && !SOC_IS_QAX(unit)) {
                    _bcm_dpp_am_fec_alloc_usage old_fec_usage;
                    status = bcm_dpp_am_fec_get_usage(unit, fec_index, &old_fec_usage);
                    BCMDNX_IF_ERR_EXIT(status);

                    if (fec_usage != old_fec_usage) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("a CASCADED FEC may not be replaced with a regular FEC (and vice versa)")));
                    }
                }

                /* Check if old push profile has to be removed */
                ret = _bcm_ppd_frwrd_fec_entry_from_hw(unit, fec_index, &protect_type_dummy, &working_fec, &failover_id_dummy);
                BCM_SAND_IF_ERR_EXIT(ret);

                if (working_fec.type == SOC_PPC_FEC_TYPE_TUNNELING_EEI_MPLS_COMMAND) {
                    /* Free push profile */
                    push_profile = SOC_PPD_MPLS_IDENTIFIER_FROM_EEI_COMMAND_ENCODING(working_fec.eep);
                    if (push_profile < SOC_PPC_EEI_IDENTIFIER_SWAP_VAL) {
                        status = _bcm_dpp_am_template_mpls_push_profile_free(unit,
                                                                         push_profile,
                                                                         &is_last_dummy);
                        BCMDNX_IF_ERR_EXIT(status);
                    }
                }
            } else { /* alloc new FEC*/
                /* no protection alloc one */
                if (!_BCM_PPD_L3_IS_PROTECTED_IF(egr)) {
                    status = _bcm_l3_alloc_fec(unit, fec_flags, fec_usage, 1, &fec_index);
                    if (status != BCM_E_NONE) {
                        BCMDNX_IF_ERR_EXIT(status);
                    }
                }
                /* protection, if protecting alloc two*/
                else if (_BCM_PPD_L3_IS_PROTECTING_IF(egr)) {
                    if (fec_index %2 != 1 ) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("secondary (failover) if_id has to be odd")));
                    }

                    protect_fec_index = fec_index - 1;
                    /* allocate 2 FECs, start on even FEC */
                    status = _bcm_l3_alloc_fec(unit, fec_flags, fec_usage, 2, &protect_fec_index);
                    if (status != BCM_E_NONE) {
                        BCMDNX_IF_ERR_EXIT(status);
                    }
                }
                else {
                    /* protection: working check given id, equal to protection +1 */
                    _bcm_l3_intf_to_fec(unit, egr->failover_if_id, &protect_fec_id);
                    if (fec_index != protect_fec_id - 1 ) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("primary if_id has to be equal to failover_if_id+1")));
                    }
                }

            }
        } else {
            /* no protection alloc one */
            if (!_BCM_PPD_L3_IS_PROTECTED_IF(egr)) {
                status = _bcm_l3_alloc_fec(unit, 0, fec_usage, 1, &fec_index);
                if (status != BCM_E_NONE) {
                    BCMDNX_IF_ERR_EXIT(status);     
                }
            }
            /* protection, if protecting alloc two*/
            else if (_BCM_PPD_L3_IS_PROTECTING_IF(egr)) {
                status = _bcm_l3_alloc_fec(unit, 0, fec_usage, 2, &fec_index);
                if (status != BCM_E_NONE) {
                    BCMDNX_IF_ERR_EXIT(status);
                }
            }
            else {
                /* protection: working */
                fec_index = BCM_L3_ITF_VAL_GET(egr->failover_if_id);
                fec_index = _BCM_L3_FEC_TO_WORK_FEC(fec_index);
            }
            BCM_L3_ITF_SET(*if_id, BCM_L3_ITF_TYPE_FEC, fec_index); 
        }     
        fec_eep_pointer = ll_global_lif;

        if ((flags & BCM_L3_WITH_ID) && (flags & BCM_L3_REPLACE)) {
            /* if we are doing an update */
            status = bcm_dpp_gport_mgmt_sw_resources_fec_to_gport_get(unit, fec_index, &gport);
            BCMDNX_IF_ERR_EXIT(status);

            /* And the FEC originally pointed to a VLAN port */
            if (BCM_GPORT_IS_VLAN_PORT(gport)) {
                int local_ll_outlif = 0;
                SOC_PPC_EG_ENCAP_ENTRY_TYPE ll_entry_type; 

                status = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, egr->encap_id, &local_ll_outlif);
                BCMDNX_IF_ERR_EXIT(status);

                /* Verify whether ll entry is of type linker layer */
                status = soc_ppd_eg_encap_entry_type_get(unit, local_ll_outlif, &ll_entry_type);
                SOC_SAND_IF_ERR_RETURN(status);

                if (ll_entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_DATA) {
                    /* Special handling when PON DSCP remark feature is enabled, do not update FEC->GPORT SW DB
                    * when change FEC from pointing to VLAN port to pointing to Data entry->VLAN port
                    */
                    update_fec_sw_db = 0;
                }
            }
        }

        if (update_fec_sw_db) {
            /* Update FEC->GPORT SW DB */
            BCM_L3_ITF_FEC_TO_GPORT_FORWARD_GROUP(gport, *if_id);
            status = bcm_dpp_gport_mgmt_sw_resources_fec_to_gport_set(unit, fec_index, gport);
            BCMDNX_IF_ERR_EXIT(status);
        }
    }

    /* 
     * Allocate a corresponding EEP entry
     * Populate MAC DA in EEP
     */

    /* 
     * Egress, eep allocation: 
     * Run allocation in case Ingress only is unset 
     *  - if encap_id != 0, then encap-id with ID. 
     *  - otherwise allocate new
     *  when writing to ingress, so if sync EEP allocate also at ingress
     *   - then encap-id must be != 0
     */
    if (is_egress) {    
        /* if replace then reuse exist encap -id if not zero */
        if ((encap_with_id) && (flags & BCM_L3_REPLACE)) { /* second call */
            _bcm_lif_type_e lif_type;
            tunnel_global_lif = ll_global_lif;

            status = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, tunnel_global_lif, &tunnel_local_lif);
            if (status != BCM_E_NONE) {
                 BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("eep not allocated")));
            }

            /* Confirm that the lif is of the correct type (link layer). */
            status = _bcm_dpp_local_lif_sw_resources_lif_usage_get(unit, _BCM_GPORT_ENCAP_ID_LIF_INVALID, tunnel_local_lif, NULL, &lif_type);
            BCMDNX_IF_ERR_EXIT(status);

            if (lif_type != _bcmDppLifTypeLinkLayer) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Out lif is not of type link layer.")));
            }


            /* In ARAD and above, the tunnel and ll lifs are the same */
            ll_local_lif = tunnel_local_lif;
            ll_global_lif = tunnel_global_lif;
                         
            fec_eep_pointer = tunnel_global_lif;         
        } else {/* (encap_with_id) && (flags & BCM_L3_REPLACE) */
            ret = _bcm_l3_alloc_eep(unit, egr, &tunnel_local_lif, &tunnel_global_lif, &ll_local_lif, &ll_global_lif); 
            if (ret != BCM_E_NONE) {
                /*clear up if creating eep failed */
                bcm_petra_l3_egress_destroy(unit, *if_id);
                BCMDNX_IF_ERR_EXIT(ret);
            }
            fec_eep_pointer = tunnel_global_lif;
            BCM_L3_ITF_SET(egr->encap_id,BCM_L3_ITF_TYPE_LIF,ll_global_lif); 
        }    
   
       /*
        *  IP Routing only requires a LL EEP
        *  IP Tunneling requires an additional Tunnel EEP
        */

        /* If encap_id is given with BCM_L3_KEEP_DSTMAC | BCM_L3_KEEP_VLAN | BCM_L3_REPLACE flags,
           we assume Link-layer is not changed and we don't update it.
           Useful in case the only need is to update multiple Tunnel-intfs such as MPLS-Tunnel or IP-Tunnel to same Link-Layer */
        if ((!encap_with_id) || ((flags & (BCM_L3_KEEP_DSTMAC | BCM_L3_KEEP_VLAN | BCM_L3_REPLACE)) != (BCM_L3_KEEP_DSTMAC | BCM_L3_KEEP_VLAN | BCM_L3_REPLACE))) {
            status = _bcm_ppd_eg_encap_entry_add(unit, fec_index, tunnel_local_lif, ll_local_lif, &fec_eep_pointer, egr);
            if (status != BCM_E_NONE) {
                BCMDNX_IF_ERR_EXIT(status);
            }


        }

        if((egr->intf > 0) && BCM_L3_ITF_TYPE_IS_RIF(egr->intf)) { /* update outrif to point to LL, needed only in ARAD */
            SOC_PPC_EG_ENCAP_ENTRY_INFO_clear(encap_entry_info);

            rif_entry_id = BCM_L3_ITF_VAL_GET(egr->intf);
            ret = soc_ppd_eg_encap_entry_get(unit,SOC_PPC_EG_ENCAP_EEP_TYPE_VSI,rif_entry_id,0,encap_entry_info,next_eep,&nof_entries);
            if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
            {
              LOG_ERROR(BSL_LS_BCM_L3,
                        (BSL_META_U(unit,
                                    "soc_sand error %x\n"), ret));
              BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_eg_encap_entry_get failed")));
            }

            ret = soc_ppd_eg_encap_vsi_entry_add(
                      unit,
                      rif_entry_id,
                      &((*encap_entry_info).entry_val.vsi_info),
                      0, /* Out-RIF no-way points to LL*/
                      ll_local_lif
                  );
            if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
            { 
              LOG_ERROR(BSL_LS_BCM_L3,
                        (BSL_META_U(unit,
                                    "soc_sand error %x\n"), ret));
              BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_eg_encap_vsi_entry_add failed")));
            }
        }

        /* tunnel already exist, update make it point to LL-EEP */
        if (intf_tunnel_global_lif != 0) {
            status = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, intf_tunnel_global_lif, &intf_tunnel_local_lif);
            if (status != BCM_E_NONE) {
                BCMDNX_IF_ERR_EXIT(status);
            }

            status = _bcm_tunnel_intf_ll_eep_set(unit, egr->intf, intf_tunnel_local_lif, ll_local_lif);
            if (status != BCM_E_NONE) {
                BCMDNX_IF_ERR_EXIT(status);
            }
            fec_eep_pointer = intf_tunnel_global_lif;
        }

    } else {
        if(intf_tunnel_global_lif != 0) {
          fec_eep_pointer = intf_tunnel_global_lif;          
        }
    }

    out_rif = l3_intf.l3a_vid;

    /* if virtual object flag set then don't update FEC table */
    if(is_ingress) {
        uint32 eei_val = 0;
        if (is_tunnel_eei_mpls_command) {
            uint32 identifier;
            uint8 is_first;
            bcm_mpls_egress_label_t label_entry;
            bcm_mpls_egress_label_t_init(&label_entry);
            _BCM_PETRA_FILL_LABEL_ENTRY_WITH_L3_EGR_CONTENTS(label_entry, egr);

            if (label_entry.action == BCM_MPLS_EGRESS_ACTION_PUSH){
                status = _bcm_mpls_tunnel_push_profile_alloc(unit, &identifier, FALSE, &label_entry, FALSE, FALSE, FALSE, TRUE, &is_first,0);
                if (status != BCM_E_NONE) {
                    BCMDNX_IF_ERR_EXIT(status);
                }
            } else /* BCM_MPLS_EGRESS_ACTION_SWAP */ {
                identifier = SOC_PPC_EEI_IDENTIFIER_SWAP_VAL;
            }
            eei_val = SOC_PPC_EEI_ENCODING_MPLS_COMMAND(identifier, egr->mpls_label);
        }
        status = _bcm_ppd_frwrd_fec_entry_add(unit, 
                                              fec_index,
                                              fec_eep_pointer,
                                              out_rif,
                                              is_tunnel,
                                              is_fec_eei_outlif,
                                              is_tunnel_eei_mpls_command,
                                              eei_val,
                                              egr);
        if (status != BCM_E_NONE) {
            BCMDNX_IF_ERR_EXIT(status);
        }
    }

exit:
    DPP_L3_LOCK_RELEASE;
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_egress_destroy
 * Purpose:
 *      Destroy an Egress forwarding object.
 * Parameters:
 *      unit - (IN) Unit number.
 *      intf - (IN) L3 interface id pointing to Egress object.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcm_petra_l3_egress_destroy(
   int unit,
   bcm_if_t intf) {

    int rv = BCM_E_NONE;
    int fec_index, global_lif, local_lif;
    SOC_PPC_FEC_ID fec_id;
    SOC_PPC_FRWRD_FEC_PROTECT_TYPE
       protect_type_dummy;
    SOC_PPC_FRWRD_FEC_ENTRY_INFO
       working_fec;
    bcm_failover_t failover_id_dummy;
    uint32 push_profile = 0;
    int is_last_dummy;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_L3_UNIT_INIT_CHECK;

    DPP_L3_LOCK_TAKE;

    if (BCM_L3_ITF_TYPE_IS_LIF(intf)) { /* LL */

        _bcm_l3_encap_id_to_eep(unit, intf, &global_lif);

        rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, global_lif, &local_lif);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = _bcm_ppd_eg_encap_entry_remove(unit, local_lif);
        BCMDNX_IF_ERR_EXIT(rv);

        /* free ENCAP entry usage */
        if (SOC_IS_JERICHO(unit)) {
            rv = _bcm_dpp_gport_delete_global_and_local_lif(unit, global_lif, _BCM_GPORT_ENCAP_ID_LIF_INVALID, local_lif);
            BCMDNX_IF_ERR_EXIT(rv);

        } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
            rv = _bcm_l3_dealloc_eep(unit, global_lif);
            BCMDNX_IF_ERR_EXIT(rv);
        }

    } else { /* FEC */

        _bcm_l3_intf_to_fec(unit, intf, &fec_id);
        fec_index = (int)fec_id;

        rv = _bcm_ppd_frwrd_fec_entry_from_hw(unit, fec_index, &protect_type_dummy, &working_fec, &failover_id_dummy);
        BCM_SAND_IF_ERR_EXIT(rv);

        if (working_fec.type == SOC_PPC_FEC_TYPE_TUNNELING_EEI_MPLS_COMMAND) {
            /* Free push profile */
            push_profile = SOC_PPD_MPLS_IDENTIFIER_FROM_EEI_COMMAND_ENCODING(working_fec.eep);
            if (push_profile < SOC_PPC_EEI_IDENTIFIER_SWAP_VAL) {
                rv = _bcm_dpp_am_template_mpls_push_profile_free(unit,
                                                                 push_profile,
                                                                 &is_last_dummy);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }

        rv = _bcm_ppd_frwrd_fec_remove(unit, fec_index);
        BCMDNX_IF_ERR_EXIT(rv);

        /* free FEC entry usage */
        rv = _bcm_l3_dealloc_fec(unit, 0, 1, fec_index);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    DPP_L3_LOCK_RELEASE;
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_egress_get
 * Purpose:
 *      Get an Egress forwarding object.
 * Parameters:
 *      unit - (IN) Unit number.
 *      intf - (IN) <UNDEF>
 *  	egr -  (IN/OUT) This is an IN argument in order to pass
 *                      the BCM_L3_HIT_CLEAR flag.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_egress_get(
    int unit, 
    bcm_if_t intf, 
    bcm_l3_egress_t *egr)
{
    SOC_PPC_FEC_ID fec_id;
    int fec_index, global_lif, local_lif;
    int status = BCM_E_NONE;
	uint32 get_flags, get_urpf_mode;
    uint8 is_wide_entry, ext_type_dummy;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_L3_UNIT_INIT_CHECK;

    DPP_L3_LOCK_TAKE;

	BCMDNX_NULL_CHECK(egr);

	/* Check input flags */
	get_flags = egr->flags;
	get_urpf_mode = egr->flags2;

	/* Reset the output parameter */
	bcm_l3_egress_t_init(egr);
	egr->flags = (get_flags & L3_EGRESS_GET_SUPPORTED_FLAGS);
	egr->flags2 = (get_urpf_mode & BCM_L3_FLAGS2_INGRESS_URPF_MODE);

    if (BCM_L3_ITF_TYPE_IS_LIF(intf)) { /* LL */

        _bcm_l3_encap_id_to_eep(unit, intf, &global_lif);

        status = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, global_lif, &local_lif);
        if (status != BCM_E_NONE) {
            BCMDNX_IF_ERR_EXIT(status);
        }

        status = _bcm_ppd_eg_encap_entry_get(unit, global_lif, egr);
        if (status != BCM_E_NONE) {
            BCMDNX_IF_ERR_EXIT(status);
        }

        if (SOC_IS_JERICHO(unit)) {

            /* Verifying whether this tunnel is a wide entry. Verification is done with local out lif id */
            status = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_lif_is_wide_entry, (unit, local_lif, FALSE, &is_wide_entry, &ext_type_dummy));
            BCMDNX_IF_ERR_EXIT(status);

            egr->flags2 |= is_wide_entry ? BCM_L3_FLAGS2_EGRESS_WIDE : 0; 
        }

    } else { /* FEC */
        _bcm_l3_intf_to_fec(unit, intf, &fec_id);
        fec_index = (int)fec_id; 

        status = bcm_dpp_am_fec_is_alloced(unit, 0 /*flags*/, 1 /*size*/, fec_index);
        if (status == BCM_E_EXISTS) {
            status = _bcm_ppd_frwrd_fec_entry_get(unit, fec_index, egr);
            /* In JERICHO possibly set the CASCADED flag. */
            if (SOC_IS_JERICHO(unit)) {
                _bcm_dpp_am_fec_alloc_usage usage;
                status = bcm_dpp_am_fec_get_usage(unit, fec_id, &usage);
                BCMDNX_IF_ERR_EXIT(status);

                if (usage == _BCM_DPP_AM_FEC_ALLOC_USAGE_CASCADED) {
                    egr->flags |= BCM_L3_CASCADED;
                }
            }
        }
    }

	/*Remove input flags*/
	egr->flags = (egr->flags & ~L3_EGRESS_GET_SUPPORTED_FLAGS);

    BCMDNX_IF_ERR_EXIT(status);
exit:
    DPP_L3_LOCK_RELEASE;
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_egress_find
 * Purpose:
 *      Find an interface pointing to an Egress forwarding object.
 * Parameters:
 *      unit - (IN) Unit number.
 *      egr - (IN) <UNDEF>
 *      intf - (OUT) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_egress_find(
    int unit, 
    bcm_l3_egress_t *egr, 
    bcm_if_t *intf)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_L3_UNIT_INIT_CHECK;

    DPP_L3_LOCK_TAKE;
     
    DPP_L3_LOCK_RELEASE;

    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_l3_egress_find is not available"))); 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_egress_traverse
 * Purpose:
 *      Traverse through the egress object table and run callback at
 *      each valid entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      trav_fn - (IN) <UNDEF>
 *      user_data - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_egress_traverse(
    int unit, 
    bcm_l3_egress_traverse_cb trav_fn, 
    void *user_data)
{
    int status;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_L3_UNIT_INIT_CHECK;

    DPP_L3_LOCK_TAKE;

    status = _bcm_ppd_frwrd_fec_get_block(unit, 0, 0, trav_fn, user_data);

    DPP_L3_LOCK_RELEASE;

    BCMDNX_IF_ERR_EXIT(status);
exit:
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Iterate all allocated FEC's and change their unicast RPF mode
*/
int
  _bcm_ppd_frwrd_fec_urpf_mode_traverse_set(
    int unit, 
    uint32 uc_rpf_mode
  )
{
  int rv, status, fec_index, fec_first, fec_last;
  BCMDNX_INIT_FUNC_DEFS;

  rv = status = BCM_E_NONE;

  if (bcmL3IngressUrpfDisable != uc_rpf_mode && bcmL3IngressUrpfLoose != uc_rpf_mode && bcmL3IngressUrpfStrict != uc_rpf_mode) {
	   BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Illegal Unicast RPF mode."))); 
  }

   
  fec_first = SOC_DPP_DEFS_GET(unit,nof_ecmp);
  fec_last = SOC_DPP_DEFS_GET(unit,nof_fecs);
  for (fec_index = fec_first; fec_index < fec_last; fec_index++) {
	  status = bcm_dpp_am_fec_is_alloced(unit, 0, 1, fec_index);
      if (status != BCM_E_EXISTS) {
		  continue;
	  }
	  rv = (MBCM_PP_DRIVER_CALL(unit, mbcm_pp_frwrd_fec_entry_urpf_mode_set,(unit, fec_index, uc_rpf_mode)));
	  BCMDNX_IF_ERR_EXIT(rv);
  }
  BCM_EXIT;

exit:
  BCMDNX_FUNC_RETURN;
}

/*
 * Traverse all the HW memory(IHP_IN_RIF_CONFIG_TABLEm) and change their unicast RPF mode
*/
int
  _bcm_ppd_rif_global_urpf_mode_traverse_set(
    int unit, 
    uint32 uc_rpf_mode
  )
{
  int rv, i, rif_id_max;
  BCMDNX_INIT_FUNC_DEFS;

  rv = BCM_E_NONE;

  if (bcmL3IngressUrpfDisable != uc_rpf_mode && bcmL3IngressUrpfLoose != uc_rpf_mode && bcmL3IngressUrpfStrict != uc_rpf_mode) {
	   BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Illegal Unicast RPF mode."))); 
  }

  rif_id_max = SOC_DPP_CONFIG(unit)->l3.nof_rifs - 1;
  for (i = 0; i <= rif_id_max; i++) {
	  rv = (MBCM_PP_DRIVER_CALL(unit, mbcm_pp_rif_global_urpf_mode_set,(unit, i,uc_rpf_mode)));
	  BCMDNX_IF_ERR_EXIT(rv);
  }
  BCM_EXIT;

exit:
  BCMDNX_FUNC_RETURN;
}


int
bcm_dpp_l3_intf_arr_to_fec_info_arr(
  int unit,  
  bcm_if_t *intf_array,
  int intf_count,
  SOC_PPC_FRWRD_FEC_ENTRY_INFO *fec_array,
  SOC_PPC_FRWRD_FEC_PROTECT_TYPE *protect_type,
  SOC_PPC_FRWRD_FEC_ENTRY_INFO *protect_fec,
  SOC_PPC_FRWRD_FEC_PROTECT_INFO *protect_info,
  ecmp_info_t *ecmp_info
  )
{
  int i;
  int rv = BCM_E_NONE;
  SOC_PPC_FEC_ID fec;

  uint32 ret;
  BCMDNX_INIT_FUNC_DEFS;

  ecmp_info->exists_protected = 0;
  ecmp_info->is_successive = 1;

  for (i = 0; i<intf_count; i++)
  {
    rv = _bcm_l3_intf_to_fec(unit, intf_array[i], &fec);
    BCMDNX_IF_ERR_EXIT(rv);

    ret = soc_ppd_frwrd_fec_entry_get(
      unit,
      fec,
      &protect_type[i],
      &fec_array[i],
      &protect_fec[i],
      &protect_info[i]
    );
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
    { 
      LOG_ERROR(BSL_LS_BCM_L3,
                (BSL_META_U(unit,
                            "soc_sand error %x\n"), ret));
      BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_fec_entry_get failed")));
    }

    if (protect_type[i]) 
    {
      ecmp_info->exists_protected = 1;
    }
    
  }
  for (i=1; i<intf_count; i++) 
  {
    if (intf_array[i] != intf_array[i-1] + 1 + ecmp_info->exists_protected) 
    {
      ecmp_info->is_successive = 0;
    }
  }
  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}
  
int 
bcm_petra_l3_ecmp_base_fec_get(
    int unit, 
    bcm_if_t ecmp_index, 
    SOC_PPC_FEC_ID *base_fec)
{
#ifdef BCM_ARAD_SUPPORT

    uint32 soc_sand_rv;
    SOC_PPC_FRWRD_FEC_ECMP_INFO ecmp_info;
#endif
    BCMDNX_INIT_FUNC_DEFS;

    *base_fec = 0;

#ifdef BCM_ARAD_SUPPORT
    soc_sand_rv = soc_ppd_frwrd_fec_ecmp_info_get(unit,ecmp_index,&ecmp_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    *base_fec = ecmp_info.base_fec_id;
#endif

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;

}

/* reuse, means the old ECMP is prefix of new ECMP, relevant only when old and new ECMP is not successive*/
int 
bcm_petra_l3_multipath_reuse_check(
    int unit,
    uint32 flags,
    int max_paths,
    int intf_count,
    bcm_if_t *intf_array,
    bcm_if_t mpintf,
    uint8    *reuse,
    bcm_if_t *old_intf_array,
    int *identicals
)
{  
    int index;
    int rv = BCM_E_NONE;
    int old_ecmp_max_size,old_ecmp_curr_size;
    int old_count;
    uint8 old_successive;
    
    BCMDNX_INIT_FUNC_DEFS;

    *identicals = 0;

    /* get old size and max-size */
    rv = bcm_sw_db_l3_get_ecmp_sizes(unit, mpintf, &old_ecmp_max_size, &old_ecmp_curr_size);;
    BCMDNX_IF_ERR_EXIT(rv);

    rv = bcm_sw_db_l3_get_ecmp_is_successive(unit,mpintf,&old_successive);
    BCMDNX_IF_ERR_EXIT(rv);

    /* not same max size re-allocated */
    if(old_ecmp_max_size != max_paths || old_successive) {
        *reuse = 0;
        BCM_EXIT;
    }

    rv = bcm_petra_l3_egress_multipath_get(unit,mpintf,old_ecmp_curr_size,old_intf_array,&old_count);
    BCMDNX_IF_ERR_EXIT(rv);

    for(index = 0; index < intf_count && index < old_ecmp_curr_size; ++index) {
        if(old_intf_array[index] != intf_array[index]) {
            *reuse = 0;
            BCM_EXIT;
        }
        ++(*identicals);
    }

    *reuse = 1;

exit:
  BCMDNX_FUNC_RETURN;
}

/**
 * Block the bcm_l3_egress_multipath_* () since it's deprecated.
 * Always returns BCM_E_CONFIG. 
 */
int bcm_petra_l3_ecmp_check_api_is_multipath(int unit)
{
	BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("Error - The bcm_l3_egress_multipath_* APIs are deprecated.\nUse the bcm_l3_egress_ecmp_* APIs instead.\n")));

exit:
  BCMDNX_FUNC_RETURN;
}

/**
 * Function:
 *      bcm_petra_l3_egress_multipath_max_create
 * Purpose:
 *      Create a Multipath Egress forwarding object with specified
 *      path-width.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN) <UNDEF>
 *      max_paths - (IN) <UNDEF>
 *      intf_count - (IN) <UNDEF>
 *      intf_array - (IN) <UNDEF>
 *      mpintf - (IN/OUT) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_egress_multipath_max_create(
    int unit,
    uint32 flags,
    int max_paths,
    int intf_count,
    bcm_if_t *intf_array,
    bcm_if_t *mpintf)
{  
  uint8 success = SOC_SAND_SUCCESS;
  uint32 ret = 0;
  SOC_PPC_FRWRD_FEC_ENTRY_INFO *fec_array = NULL; 
  SOC_PPC_FRWRD_FEC_PROTECT_TYPE *protect_type_array = NULL; 
  SOC_PPC_FRWRD_FEC_PROTECT_INFO *protect_info_array = NULL; 
  SOC_PPC_FRWRD_FEC_ENTRY_INFO *protect_array = NULL; 

  uint32 shr_flags = 0;
  int rv = 0;
  int intf_indx = 0;
  int ecmp_index = 0;
  ecmp_info_t ecmp_copy_info;
  SOC_PPC_FEC_ID
      fec_id=0;

#ifdef BCM_ARAD_SUPPORT
  int i;
  SOC_PPC_FRWRD_FEC_ECMP_INFO ecmp_info;
  SOC_PPC_FEC_ID fec_ndx;
  int fec_start_ndx;
  bcm_if_t *old_intf_array=NULL;
  int identicals;
  int old_hanlde = 0; /* 0: none, 1:resue 2: remove*/
  SOC_PPC_FEC_ID
      start_fec_id;
  uint8 old_successive;
  int index;
  int old_ecmp_max_size,old_ecmp_curr_size=0;
  SOC_PPC_FEC_ID old_start_fec_id = 0;
  uint8 old_exist=0,old_protected,reuse=0;
  uint32 soc_sand_rv;
#endif

  BCMDNX_INIT_FUNC_DEFS;
  BCM_DPP_UNIT_CHECK(unit);
  DPP_L3_UNIT_INIT_CHECK;

  /* get soc_sand device id from the bcm unit id */

  DPP_L3_LOCK_TAKE;    

  rv = bcm_petra_l3_ecmp_check_api_is_multipath(unit);
  BCMDNX_IF_ERR_EXIT(rv);

  LOG_VERBOSE(BSL_LS_BCM_L3,
              (BSL_META_U(unit,
                          "flags:%x, max_paths:%d, intf_count:%d, mpintf:%d\n"), flags, max_paths, intf_count,*mpintf));
  
  /*check that intf_count is smaller than max count*/
  if (intf_count > max_paths)
  {
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("intf_count is larger than max_paths")));
  }    

  /*check that max_paths is between 1-16 (soc_petra HW restriction)*/
  if (max_paths > DPP_L3_MAX_ENTRIES_IN_ECMP || max_paths < DPP_L3_MIN_ENTRIES_IN_ECMP)
  {
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("not supported ECMP size")));
  }

  /* check flags */
  if (flags & BCM_L3_REPLACE) {
    if (!(flags & BCM_L3_WITH_ID)) {
      LOG_ERROR(BSL_LS_BCM_L3,
                (BSL_META_U(unit,
                            "BCM_L3_REPLACE is supported only with BCM_L3_WITH_ID\n")));
      BCM_RETURN_VAL_EXIT(BCM_E_PARAM);
    }
  } 

  BCMDNX_ALLOC(fec_array, DPP_L3_MAX_ENTRIES_IN_ECMP * sizeof(SOC_PPC_FRWRD_FEC_ENTRY_INFO), "fec array");
  if (fec_array == NULL) {
    BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
  }
  BCMDNX_ALLOC(protect_array, DPP_L3_MAX_ENTRIES_IN_ECMP * sizeof(SOC_PPC_FRWRD_FEC_ENTRY_INFO), "protect array");
  if (protect_array == NULL) {  
    BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
  }
  BCMDNX_ALLOC(protect_type_array, DPP_L3_MAX_ENTRIES_IN_ECMP * sizeof(SOC_PPC_FRWRD_FEC_PROTECT_TYPE), "protect type array");
  if (protect_type_array == NULL) { 
    BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
  }
  BCMDNX_ALLOC(protect_info_array, DPP_L3_MAX_ENTRIES_IN_ECMP * sizeof(SOC_PPC_FRWRD_FEC_PROTECT_INFO), "protect info array");
  if (protect_info_array == NULL) { 
    BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
  }

  /* handle allocation flags */
  if (flags & BCM_L3_WITH_ID) 
  {
    shr_flags |= BCM_DPP_AM_FLAG_ALLOC_WITH_ID;
    _bcm_l3_intf_to_fec(unit, *mpintf, &fec_id);
    ecmp_index = fec_id;

    LOG_VERBOSE(BSL_LS_BCM_L3,
                (BSL_META_U(unit,
                            "with-id, mpintf:%x i.e. ECMP = %d \n"), *mpintf, ecmp_index));
  } 
  if (flags & BCM_L3_REPLACE) 
  {
    /* in this case just overwrite, ignoring the allocation manager*/ 
  }
  else
  {
    /* Allocate a FEC entries */
    rv = _bcm_l3_alloc_ecmp(unit, shr_flags, &ecmp_index);
    if (rv != BCM_E_NONE)
    {
      BCMDNX_IF_ERR_EXIT(rv);
    }      

    LOG_VERBOSE(BSL_LS_BCM_L3,
                (BSL_META_U(unit,
                            "allocated ECMP:%d \n"), ecmp_index));
  }      

  _bcm_l3_fec_to_intf(unit, ecmp_index, mpintf);

  /* check if this replace exist ECMP, then get exist ECMP info */
  if(flags & BCM_L3_REPLACE) {
      rv = bcm_sw_db_l3_get_ecmp_is_successive(unit,*mpintf,&old_successive);
      BCMDNX_IF_ERR_EXIT(rv);
      rv = bcm_sw_db_l3_get_ecmp_is_protected(unit,*mpintf,&old_protected);
      BCMDNX_IF_ERR_EXIT(rv);
      rv = bcm_sw_db_l3_get_ecmp_sizes(unit, *mpintf, &old_ecmp_max_size, &old_ecmp_curr_size);;
      BCMDNX_IF_ERR_EXIT(rv);
      old_exist = (old_ecmp_max_size != 0);

      soc_sand_rv = soc_ppd_frwrd_fec_ecmp_info_get(unit,ecmp_index,&ecmp_info);
      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
      old_start_fec_id = ecmp_info.base_fec_id;

      LOG_VERBOSE(BSL_LS_BCM_L3,
                  (BSL_META_U(unit,
                              "replace exist ECMP: max-size:%d cur-size:%d start-fec:%d successive:%d\n"), old_ecmp_max_size, old_ecmp_curr_size, old_start_fec_id, old_successive));
  }


  /* convert intf array to fec array to be used in SOC_PPD_frwrd_fec_ecmp_add */
  fec_start_ndx = intf_array[0];
  _bcm_l3_intf_to_fec(unit,fec_start_ndx,&start_fec_id); 

  rv = bcm_dpp_l3_intf_arr_to_fec_info_arr(unit, intf_array,intf_count,fec_array,protect_type_array,protect_array,protect_info_array,&ecmp_copy_info);
  if (rv != BCM_E_NONE)
  {
    BCMDNX_IF_ERR_EXIT(rv);
  }

  LOG_VERBOSE(BSL_LS_BCM_L3,
              (BSL_META_U(unit,
                          "new ECMP with is %s successive\n"), (ecmp_copy_info.is_successive)?"":"not"));
  /* allocate FEC entries if needed (copy) */
   /* Allocate a FEC entry */
  if (!ecmp_copy_info.is_successive) {
      shr_flags = 0;

      if(old_ecmp_curr_size > 0) {
          BCMDNX_ALLOC(old_intf_array, old_ecmp_curr_size * sizeof(bcm_if_t), "old_fec array");
          if (old_intf_array == NULL) {        
              BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
          }

          rv = bcm_petra_l3_multipath_reuse_check(unit,flags,max_paths,intf_count,intf_array,*mpintf,&reuse,old_intf_array,&identicals);
          BCMDNX_IF_ERR_EXIT(rv);
      }

      if(reuse) {
          old_hanlde = 1;
          start_fec_id = old_start_fec_id;
          LOG_VERBOSE(BSL_LS_BCM_L3,
                      (BSL_META_U(unit,
                                  "reuse exist ECMP block\n")));
      }
      /* if(old_exist && (flags & BCM_L3_REPLACE) && !old_successive && old_ecmp_max_size == max_paths) {*/
      else{

          rv = _bcm_l3_alloc_fec(unit, shr_flags, 1, max_paths, (int*)&start_fec_id);
          BCMDNX_IF_ERR_EXIT(rv);

          LOG_VERBOSE(BSL_LS_BCM_L3,
                      (BSL_META_U(unit,
                                  "allocate place for FECs start at :%d \n"), start_fec_id));

          /* if replace but cannot reuse old free it */
          if(old_exist && (flags & BCM_L3_REPLACE) && old_successive){
              old_hanlde = 2; /* remove */
              LOG_VERBOSE(BSL_LS_BCM_L3,
                          (BSL_META_U(unit,
                                      "dealloc old FECs start at :%d \n"), old_start_fec_id));
              /* rv = _bcm_l3_dealloc_fec(unit, 0, 1, old_ecmp_max_size, old_start_fec_id);
              BCMDNX_IF_ERR_EXIT(rv);*/
          }

     }
  }
  else{
      /* if replace where old is not successive and new successive remove old */
      if(old_exist && (flags & BCM_L3_REPLACE) && !old_successive){
          old_hanlde = 2; /* remove*/
      }
      /* first FEC in array */
      rv = _bcm_l3_intf_to_fec(unit, intf_array[0], &start_fec_id);
      BCMDNX_IF_ERR_EXIT(rv);
  }

  /* loop over fecs and update the corresponding fecs on the sw_db*/
  for (intf_indx = 0; intf_indx < intf_count; intf_indx++)
  {
    bcm_sw_db_l3_add_fec_mapping_to_ecmp(unit, flags, intf_array[intf_indx], *mpintf, start_fec_id + intf_indx);
  }

  /*update curr size and max size for the ecmp group*/
  rv = bcm_sw_db_l3_set_ecmp_sizes(unit,*mpintf,max_paths,intf_count);
  BCMDNX_IF_ERR_EXIT(rv);

  /* save if it successive*/
  bcm_sw_db_l3_set_ecmp_is_successive(unit,*mpintf,ecmp_copy_info.is_successive);
  BCMDNX_IF_ERR_EXIT(rv);

  bcm_sw_db_l3_set_ecmp_is_protected(unit,*mpintf,ecmp_copy_info.exists_protected);
  BCMDNX_IF_ERR_EXIT(rv);

  if (!ecmp_copy_info.is_successive) {
      fec_ndx = start_fec_id;
      for (i=0; i<intf_count; i++) {

          /* if identical to old skip*/
          if(old_hanlde == 1 && identicals) {
              fec_ndx += 1 + ecmp_copy_info.exists_protected;
          }

        ret = soc_ppd_frwrd_fec_entry_add(
            unit,
            fec_ndx,
            protect_type_array[i],
            &fec_array[i],
            &protect_array[i],
            &protect_info_array[i],
            &success
            );
        if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
        { 
            LOG_ERROR(BSL_LS_BCM_L3,
                      (BSL_META_U(unit,
                                  "soc_sand error %x\n"), ret));
            rv = BCM_E_INTERNAL; 
            BCMDNX_IF_ERR_EXIT(rv);
        }
        fec_ndx += 1 + ecmp_copy_info.exists_protected;
      }   
}

      SOC_PPC_FRWRD_FEC_ECMP_INFO_clear(&ecmp_info);
  _bcm_l3_intf_to_fec(unit, start_fec_id, &ecmp_info.base_fec_id);
  ecmp_info.is_protected = ecmp_copy_info.exists_protected;
  ecmp_info.size = intf_count;
  ecmp_info.rpf_fec_index = 0;
  /* if ECMP is empty, make it point to FEC zero */
  if(ecmp_info.size == 0) {
      ecmp_info.base_fec_id = 0;
      ecmp_info.size = 1;/* size zero is not supported*/
  }
  soc_ppd_frwrd_fec_ecmp_info_set(unit,ecmp_index,&ecmp_info);

  if(old_hanlde == 1 ) {/* reuse only of copied*/
      /* remove all unused */
      for(i = identicals; i < old_ecmp_curr_size; ++i) {
            rv = bcm_sw_db_l3_fec_remove_fec(unit, old_start_fec_id+i);    
            BCMDNX_IF_ERR_EXIT(rv);

            /* dealloc fec indx */
            rv = _bcm_l3_dealloc_fec(unit, 0, 1 + old_protected, old_start_fec_id + i);
            BCMDNX_IF_ERR_EXIT(rv);

            rv = soc_ppd_frwrd_fec_remove(
                    unit,
                    old_start_fec_id + i
                   );
            BCMDNX_IF_ERR_EXIT(rv);
      }
  }

  /* old hanlde */
  if(old_hanlde == 2){ /* remove */

      LOG_VERBOSE(BSL_LS_BCM_L3,
                  (BSL_META_U(unit,
                              "dealloc old FECs start at :%d to %d \n"), old_start_fec_id, old_start_fec_id+old_ecmp_curr_size));

      for (index = 0; index < old_ecmp_curr_size; index++)
      {
        bcm_sw_db_l3_fec_remove_fec(unit, old_start_fec_id+index);    
      } 
      /*rv = _bcm_l3_dealloc_fec(unit, 0, 1, old_ecmp_max_size, old_start_fec_id);
      BCMDNX_IF_ERR_EXIT(rv);*/

      /* remove old FECs if they were copied and not reused */
      if(!old_successive) {
          for (index = 0; index < old_ecmp_curr_size; index++ ) {
            ret = soc_ppd_frwrd_fec_remove(
            unit,
            old_start_fec_id + index
            );
            if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
            { 
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_sand error %x"), ret));
            }

            /* dealloc fec indx */
            rv = _bcm_l3_dealloc_fec(unit, 0, 1 + old_protected, old_start_fec_id + index);
            BCMDNX_IF_ERR_EXIT(rv);

          }
      }

  }


  exit:
  if (protect_array)
     BCM_FREE(protect_array);
  if (protect_info_array)
     BCM_FREE(protect_info_array);
  if (protect_type_array) 
     BCM_FREE(protect_type_array);
  if (fec_array) 
     BCM_FREE(fec_array);
#ifdef BCM_ARAD_SUPPORT
  if (old_intf_array) 
     BCM_FREE(old_intf_array);
#endif /* BCM_ARAD_SUPPORT */


  /* DPP_L3_LOCK_RELEASE could return BCM_E_INTERNAL, so resources need
   * to be freed before calling it */

  DPP_L3_LOCK_RELEASE;
  BCMDNX_FUNC_RETURN; 
}

/*
 * Function:
 *      bcm_petra_l3_egress_multipath_create
 * Purpose:
 *      Create a Multipath Egress forwarding object.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN) <UNDEF>
 *      intf_count - (IN) <UNDEF>
 *      intf_array - (IN) <UNDEF>
 *      mpintf - (IN/OUT) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_egress_multipath_create(
    int unit, 
    uint32 flags, 
    int intf_count, 
    bcm_if_t *intf_array, 
    bcm_if_t *mpintf)
{
    int rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_L3_UNIT_INIT_CHECK;
    
    rv = bcm_petra_l3_ecmp_check_api_is_multipath(unit);
    BCMDNX_IF_ERR_EXIT(rv);    

    /* the max size available for soc_petra b is 16, thus call
       bcm_petra_l3_egress_multipath_max_create with max_path=16 */
    BCMDNX_IF_ERR_EXIT(bcm_petra_l3_egress_multipath_max_create(unit,flags,intf_count,intf_count,intf_array,mpintf));
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_egress_multipath_destroy
 * Purpose:
 *      Destroy an Egress Multipath forwarding object.
 * Parameters:
 *      unit - (IN) Unit number.
 *      mpintf - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_egress_multipath_destroy(
    int unit, 
    bcm_if_t mpintf)
{
  int rv;
  
  BCMDNX_INIT_FUNC_DEFS;
  BCM_DPP_UNIT_CHECK(unit);
  DPP_L3_UNIT_INIT_CHECK;

  DPP_L3_LOCK_TAKE;

  rv = bcm_petra_l3_ecmp_check_api_is_multipath(unit);
  BCMDNX_IF_ERR_EXIT(rv);
  
exit:
  DPP_L3_LOCK_RELEASE;
  BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_egress_multipath_get
 * Purpose:
 *      Get an Egress Multipath forwarding object.
 * Parameters:
 *      unit - (IN) Unit number.
 *      mpintf - (IN) <UNDEF>
 *      intf_size - (IN) <UNDEF>
 *      intf_array - (OUT) <UNDEF>
 *      intf_count - (OUT) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_egress_multipath_get(
    int unit, 
    bcm_if_t mpintf, 
    int intf_size, 
    bcm_if_t *intf_array, 
    int *intf_count)
{

  int index;
  int ecmp_max_size;
  int ecmp_curr_size;
  fec_copy_info_t fec_to_ecmps;
  SOC_PPC_FRWRD_FEC_ECMP_INFO ecmp_info;
  SOC_PPC_FEC_ID fec_index;
  int rv = BCM_E_NONE;

  BCMDNX_INIT_FUNC_DEFS;
  BCM_DPP_UNIT_CHECK(unit);
  DPP_L3_UNIT_INIT_CHECK;


  DPP_L3_LOCK_TAKE;

  rv = bcm_petra_l3_ecmp_check_api_is_multipath(unit);
  BCMDNX_IF_ERR_EXIT(rv);

  rv = bcm_sw_db_l3_get_ecmp_sizes(unit, mpintf, &ecmp_max_size, &ecmp_curr_size);;
  BCMDNX_IF_ERR_EXIT(rv);

  /* map interface ID for FEC index */
  _bcm_l3_intf_to_fec(unit, mpintf, &fec_index);

  soc_ppd_frwrd_fec_ecmp_info_get(unit,fec_index,&ecmp_info);
  *intf_count = ecmp_curr_size;
  /* only count */
  if (intf_size == 0)
  {     
    BCM_EXIT;
  }  
  if (ecmp_curr_size == 0) { /* no members return*/
    BCM_EXIT;
  }

  for (index = 0; index < intf_size; index++) {
      /* map copy FEC to real-FEC and ECMP */
      bcm_sw_db_l3_copy_fec_to_ecmps_get(unit, ecmp_info.base_fec_id + index, &fec_to_ecmps);
      intf_array[index] = fec_to_ecmps.real_fec; /* copy fec with encoding */

      /*intf_array[index] = ecmp_info.base_fec_id + index * (1 + ecmp_info.is_protected);*/
  }

  BCM_EXIT; 
exit:
  DPP_L3_LOCK_RELEASE;
  BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_egress_multipath_add
 * Purpose:
 *      Add an Egress forwarding object to an Egress Multipath
 *      forwarding object.
 * Parameters:
 *      unit - (IN) Unit number.
 *      mpintf - (IN) <UNDEF>
 *      intf - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_egress_multipath_add(
    int unit, 
    bcm_if_t mpintf, 
    bcm_if_t intf)
{
  int rv = 0;
  SOC_PPC_FRWRD_FEC_ENTRY_INFO *fec_array = NULL; 
  SOC_PPC_FRWRD_FEC_PROTECT_TYPE *protect_type_array = NULL; 
  SOC_PPC_FRWRD_FEC_PROTECT_INFO *protect_info_array = NULL; 
  SOC_PPC_FRWRD_FEC_ENTRY_INFO *protect_array = NULL; 
  uint8 success = SOC_SAND_SUCCESS;
  uint32 ret = 0;    
  int ecmp_index = 0;    
  SOC_PPC_FEC_ID fec_id = 0;
  bcm_if_t *intf_array = NULL;
  int index = 0;
  int ecmp_max_size = 0;
  int ecmp_curr_size = 0;
  ecmp_info_t ecmp_copy_info;
#ifdef BCM_ARAD_SUPPORT
  SOC_PPC_FRWRD_FEC_ECMP_INFO ecmp_info;
  SOC_PPC_FEC_ID fec_ndx;
#endif /* BCM_ARAD_SUPPORT */

  BCMDNX_INIT_FUNC_DEFS;
  BCM_DPP_UNIT_CHECK(unit);
  DPP_L3_UNIT_INIT_CHECK;

  DPP_L3_LOCK_TAKE;

  rv = bcm_petra_l3_ecmp_check_api_is_multipath(unit);
  BCMDNX_IF_ERR_EXIT(rv);

  /* get the current size of the ecmp group */
  bcm_sw_db_l3_get_ecmp_sizes(unit, mpintf, &ecmp_max_size, &ecmp_curr_size);

  /* check that the ecmp size is not growing above its limit */
  if (ecmp_curr_size == ecmp_max_size)
  {
    BCMDNX_ERR_EXIT_MSG(BCM_E_FULL, (_BSL_BCM_MSG("exceed ECMP max size")));
  }

  BCMDNX_ALLOC(fec_array, DPP_L3_MAX_ENTRIES_IN_ECMP * sizeof(SOC_PPC_FRWRD_FEC_ENTRY_INFO), "fec array");
  if (fec_array == NULL) {        
    BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
  }
  BCMDNX_ALLOC(intf_array, DPP_L3_MAX_ENTRIES_IN_ECMP * sizeof(bcm_if_t), "intf array");
  if (intf_array == NULL) {        
    BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
  }
  BCMDNX_ALLOC(protect_array, DPP_L3_MAX_ENTRIES_IN_ECMP * sizeof(SOC_PPC_FRWRD_FEC_ENTRY_INFO), "protect array");
  if (protect_array == NULL) {        
    BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
  }
  BCMDNX_ALLOC(protect_type_array, DPP_L3_MAX_ENTRIES_IN_ECMP * sizeof(SOC_PPC_FRWRD_FEC_PROTECT_TYPE), "protect type array");
  if (protect_type_array == NULL) {        
    BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
  }
  BCMDNX_ALLOC(protect_info_array, DPP_L3_MAX_ENTRIES_IN_ECMP * sizeof(SOC_PPC_FRWRD_FEC_PROTECT_INFO), "protect info array");
  if (protect_info_array == NULL) {        
    BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
  }

  /* create an intf_array of the ecmp */
  for(index = 0; index < ecmp_curr_size; index++)
  {
    intf_array[index] = mpintf + index;
  }
  /* add the new intf to the array */
  intf_array[index] = intf;


  /* save the old ecmp into a fec array */
  rv = bcm_dpp_l3_intf_arr_to_fec_info_arr(unit,intf_array,ecmp_curr_size+1,fec_array,protect_type_array,protect_array,protect_info_array,&ecmp_copy_info);
  if (rv != BCM_E_NONE)
  {
    BCMDNX_IF_ERR_EXIT(rv);
  }

  _bcm_l3_intf_to_fec(unit, mpintf, &fec_id);
  ecmp_index = (int)fec_id;

  /*update curr size and max size for the ecmp group*/
  rv = bcm_sw_db_l3_set_ecmp_sizes(unit,mpintf,ecmp_max_size,ecmp_curr_size /* +1? */);  
  if (rv != BCM_E_NONE)
  {
    BCMDNX_IF_ERR_EXIT(rv);
  }  

    SOC_PPC_FRWRD_FEC_ECMP_INFO_clear(&ecmp_info);
    soc_ppd_frwrd_fec_ecmp_info_get(unit,ecmp_index,&ecmp_info);   

    /* if adding protected fec to unprotected ecmp */
    if (!ecmp_info.is_protected && !protect_type_array[index]==SOC_PPC_FRWRD_FEC_PROTECT_TYPE_NONE) {
        bcm_petra_l3_egress_multipath_create(unit, BCM_L3_WITH_ID | BCM_L3_REPLACE, ecmp_curr_size + 1, intf_array, &ecmp_index);
    } 
    else
    {
        /* LCOV_EXCL_START */
        /* calculate next fec index (add 2 for protected ecmp, 1 for non-protected) */
        fec_ndx = ecmp_info.base_fec_id + (ecmp_info.is_protected ? ecmp_info.size*2 : ecmp_info.size);
        /* update the fec to ecmp mapping in sw_db */
        bcm_sw_db_l3_add_fec_mapping_to_ecmp(unit, 0, intf, mpintf, fec_ndx);

        ret = soc_ppd_frwrd_fec_entry_add(
            unit,
            fec_ndx,
            protect_type_array[index],
            &fec_array[index],
            &protect_array[index],
            &protect_info_array[index],
            &success
            );
        if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
        { 
            LOG_ERROR(BSL_LS_BCM_L3,
                      (BSL_META_U(unit,
                                  "soc_sand error %x\n"), ret));
            rv = BCM_E_INTERNAL; 
            BCMDNX_IF_ERR_EXIT(rv);
        }   
        ecmp_info.size++; 
        soc_ppd_frwrd_fec_ecmp_info_set(unit,ecmp_index,&ecmp_info);
        /* LCOV_EXCL_START */
    }

  exit:
  if (intf_array)
     BCM_FREE(intf_array);
  if (protect_array)
     BCM_FREE(protect_array);
  if (protect_info_array)
     BCM_FREE(protect_info_array);
  if (protect_type_array)
     BCM_FREE(protect_type_array);
  if (fec_array)
     BCM_FREE(fec_array);

  /* DPP_L3_LOCK_RELEASE could return with BCM_E_INTERNAL, 
   * not freeing resources, so it needed to be moved */
  DPP_L3_LOCK_RELEASE;
  BCMDNX_FUNC_RETURN;
}



/*
 * Function:
 *      bcm_petra_l3_egress_multipath_delete
 * Purpose:
 *      Delete an Egress forwarding object from an Egress Multipath
 *      forwarding object.
 * Parameters:
 *      unit - (IN) Unit number.
 *      mpintf - (IN) <UNDEF>
 *      intf - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_egress_multipath_delete(
    int unit, 
    bcm_if_t mpintf, 
    bcm_if_t intf)
{
  int rv;
  SOC_PPC_FRWRD_FEC_ENTRY_INFO *fec_array = NULL;    
  int ecmp_index;    
  SOC_PPC_FEC_ID fec_id,base_fec_id;
  bcm_if_t *intf_array = NULL;

  int index;
  int ecmp_max_size;
  int ecmp_curr_size;
  fec_copy_info_t fec_to_ecmps;
  int found_flag = 0;
  
  BCMDNX_INIT_FUNC_DEFS;
  BCM_DPP_UNIT_CHECK(unit);
  DPP_L3_UNIT_INIT_CHECK;

  DPP_L3_LOCK_TAKE;

  rv = bcm_petra_l3_ecmp_check_api_is_multipath(unit);
  BCMDNX_IF_ERR_EXIT(rv);

  /* get the current size of the ecmp group */
  bcm_sw_db_l3_get_ecmp_sizes(unit, mpintf, &ecmp_max_size, &ecmp_curr_size);

  /* check that the ecmp size is not decreasing below 1 */
  if (ecmp_curr_size < 1)
  {
    BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("ECMP already empty")));
  }

  BCMDNX_ALLOC(intf_array, DPP_L3_MAX_ENTRIES_IN_ECMP * sizeof(bcm_if_t), "intf array");
  if (intf_array == NULL) {        
    BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
  }

  _bcm_l3_intf_to_fec(unit, mpintf, &fec_id);
  ecmp_index = fec_id;

  rv = bcm_petra_l3_ecmp_base_fec_get(unit,ecmp_index,&base_fec_id);
  BCMDNX_IF_ERR_EXIT(rv);

  /* create an intf_array of the ecmp */
  for(index = 0; index < ecmp_curr_size; index++)
  {
    bcm_sw_db_l3_copy_fec_to_ecmps_get(unit, base_fec_id + index, &fec_to_ecmps);
    if (fec_to_ecmps.ecmp == -1)
    {
      BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("ecmp not found")));
    }
    if (fec_to_ecmps.ecmp != mpintf)
    {
      BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("ecmp does not match mulipath interface")));
    }
    
    if (found_flag == 0 && fec_to_ecmps.real_fec == intf)
    {
      /* found_flag++; */
      /* if multiple copies of a fec are in the same ecmp, remove only one */
      found_flag = 1;
    }
    else
    {
      /* intf_array includes real-FEC for all coppies in the ECMP */
        _bcm_l3_fec_to_intf(unit,fec_to_ecmps.real_fec,&intf_array[index-found_flag]);
    }

  }

  /* check that the intf was actually part of the multipath */
  if (found_flag == 0)
  {
    BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("interface is not part of the multipath")));
  }  

  /* call update whith new ECMP membership */
  bcm_petra_l3_egress_multipath_max_create(unit, BCM_L3_WITH_ID | BCM_L3_REPLACE, ecmp_max_size, ecmp_curr_size - found_flag, intf_array, &mpintf);

exit:
 BCM_FREE(intf_array);
 BCM_FREE(fec_array);
  DPP_L3_LOCK_RELEASE;
  BCMDNX_FUNC_RETURN;
}

int 
_bcm_l3_multipath_find_increase_counter(
  int ecmp,
  int arr_size,
  int *ecmp_ids,
  int *ecmp_counters
)
{
  int index;

  BCMDNX_INIT_FUNC_DEFS_NO_UNIT;
  for (index=0; index < arr_size; index++)
  {
    if (ecmp_ids[index] == -1)
    {
      ecmp_ids[index] = ecmp;
      ecmp_counters[index]++;
      break;
    } 
    else if (ecmp_ids[index] == ecmp)
    {
      ecmp_counters[index]++;
      break;
    }    
  }

  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      bcm_petra_l3_egress_multipath_find
 * Purpose:
 *      Find an interface pinting to an Egress Multipath forwarding
 *      object.
 * Parameters:
 *      unit - (IN) Unit number.
 *      intf_count - (IN) <UNDEF>
 *      intf_array - (IN) <UNDEF>
 *      mpintf - (OUT) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_egress_multipath_find(
    int unit, 
    int intf_count, 
    bcm_if_t *intf_array, 
    bcm_if_t *mpintf)
{ 
  int index = 0, index2 = 0;  
  fec_to_ecmps_t fec_to_ecmps;
  int max_ecmps_per_fec = 0;
  int *ecmp_ids = NULL;
  int *ecmp_counters = NULL;
  int found = 0;
  int rv = BCM_E_NONE;

  BCMDNX_INIT_FUNC_DEFS;
  BCM_DPP_UNIT_CHECK(unit);
  DPP_L3_UNIT_INIT_CHECK;  

  DPP_L3_LOCK_TAKE;

  rv = bcm_petra_l3_ecmp_check_api_is_multipath(unit);
  BCMDNX_IF_ERR_EXIT(rv);

  /* check that intf_count is less or equal to 16, the max allowed for soc_petra*/
  if (intf_count >= 16)
  {
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("The maximal intf_count for petra is 16")));
  }

  /* get the max number of ecmps attached to one of the fecs in intf_array */
  for(index = 0; index < intf_count; index++)
  {
    bcm_sw_db_l3_fec_to_ecmps_get(unit, intf_array[index], &fec_to_ecmps);
    if (fec_to_ecmps.nof_ecmps < 1)
    {
      BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("ecmp not found")));
    }  
    else if (fec_to_ecmps.nof_ecmps > max_ecmps_per_fec)
    {
      max_ecmps_per_fec = fec_to_ecmps.nof_ecmps;
    }       
  }

  /* alloc the counters for the member check */
  BCMDNX_ALLOC(ecmp_ids, sizeof(int) * max_ecmps_per_fec, "ecmp_ids");
  if (ecmp_ids == NULL) { 
    BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
  }
  BCMDNX_ALLOC(ecmp_counters, sizeof(int) * max_ecmps_per_fec, "ecmp_counters");
  if (ecmp_counters == NULL) {
    BCM_FREE(ecmp_ids);
    BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
  }

  /* initialize counters and ids */
  for (index = 0; index<max_ecmps_per_fec; index++)
  {
    ecmp_ids[index] = -1;
    ecmp_counters[index] = 0;
  }

  /* go over all ecmps and count */
  for(index = 0; index < intf_count; index++)
  {
    bcm_sw_db_l3_fec_to_ecmps_get(unit, intf_array[index], &fec_to_ecmps);
    for (index2 = 0; index2 < fec_to_ecmps.nof_ecmps; index2++)
    {
      _bcm_l3_multipath_find_increase_counter(fec_to_ecmps.ecmps[index2],max_ecmps_per_fec,ecmp_ids,ecmp_counters);
    }        
  } 

  /* go over all counter, if one of the values is equal to the number of intfs return this ecmp */
  for(index = 0; index < max_ecmps_per_fec; index++)
  {
    if (ecmp_counters[index]==intf_count)
    {
      found++;
      *mpintf = ecmp_ids[index];
      break;
    }            
  }

  if (found == 0)
  {
    BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("multipath not found")));
  }

exit:
    DPP_L3_LOCK_RELEASE;
    BCM_FREE(ecmp_ids);
    BCM_FREE(ecmp_counters);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_egress_multipath_traverse
 * Purpose:
 *      Traverse through the multipath egress object table and run
 *      callback at each valid entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      trav_fn - (IN) <UNDEF>
 *      user_data - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_egress_multipath_traverse(
    int unit, 
    bcm_l3_egress_multipath_traverse_cb trav_fn, 
    void *user_data)
{
  int rv = 0;

  BCMDNX_INIT_FUNC_DEFS;
  BCM_DPP_UNIT_CHECK(unit);
  DPP_L3_UNIT_INIT_CHECK;

  DPP_L3_LOCK_TAKE;

  rv = bcm_petra_l3_ecmp_check_api_is_multipath(unit);
  BCMDNX_IF_ERR_EXIT(rv);

exit:
  DPP_L3_LOCK_RELEASE;
  BCMDNX_FUNC_RETURN;
}

/*
 * Check that a bcm_if_t represents a valid ECMP FEC index.
*/
STATIC int
  _bcm_petra_l3_egress_ecmp_fec_ndx_check(
    int unit,
    bcm_if_t ecmp_intf
  )
{
  SOC_PPC_FEC_ID fec_ndx = BCM_L3_ITF_VAL_GET(ecmp_intf);

  BCMDNX_INIT_FUNC_DEFS;

  if ((fec_ndx < 1) || (fec_ndx > SOC_DPP_CONFIG(unit)->l3.fec_ecmp_reserved)) {
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG_STR("Illegal ECMP FEC id 0x%08x.\n"), fec_ndx));
  }

  BCM_EXIT;

exit:
  BCMDNX_FUNC_RETURN;
}

/* 
 * The following are wrappers around alloc/dealloc/is_alloced_ecmp 
 * that allow using bcm_l3_egress_ecmp_t instead of a 
 * SOC_PPC_FEC_ID.
 */
STATIC int
  _bcm_petra_l3_egress_ecmp_alloc_ecmp(
    int unit,
    int flags,
    bcm_l3_egress_ecmp_t *ecmp
  )
{
  int rv;
  int fec_ndx;

  BCMDNX_INIT_FUNC_DEFS;

  BCMDNX_VERIFY(ecmp != NULL);
  if (flags & BCM_DPP_AM_FLAG_ALLOC_WITH_ID) {
    rv = _bcm_petra_l3_egress_ecmp_fec_ndx_check(unit, ecmp->ecmp_intf);
    BCMDNX_IF_ERR_EXIT(rv);
  }
  
  fec_ndx = BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf);

  rv = _bcm_l3_alloc_ecmp(unit, flags, &fec_ndx);
  BCMDNX_IF_ERR_EXIT(rv);
  _bcm_l3_fec_to_intf(unit, fec_ndx, &ecmp->ecmp_intf);
  BCMDNX_VERIFY(_bcm_petra_l3_egress_ecmp_fec_ndx_check(unit,ecmp->ecmp_intf) == BCM_E_NONE);

exit:
  BCMDNX_FUNC_RETURN;
}

STATIC int
  _bcm_petra_l3_egress_ecmp_dealloc_ecmp(
    int unit,
    bcm_l3_egress_ecmp_t *ecmp
  )
{
  int rv;
  SOC_PPC_FEC_ID fec_ndx;

  BCMDNX_INIT_FUNC_DEFS;

  BCMDNX_VERIFY(ecmp != NULL);
  rv = _bcm_petra_l3_egress_ecmp_fec_ndx_check(unit, ecmp->ecmp_intf);
  BCMDNX_IF_ERR_EXIT(rv);
  
  fec_ndx = BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf);

  rv = _bcm_l3_dealloc_ecmp(unit, fec_ndx);
  BCMDNX_IF_ERR_EXIT(rv);

exit:
  BCMDNX_FUNC_RETURN;
}

/* (On success) If it is alloced then *is_alloced != 0. */
STATIC int
  _bcm_petra_l3_egress_ecmp_is_alloced_ecmp(
    int unit,
    bcm_l3_egress_ecmp_t *ecmp,
    uint8 *is_alloced
  )
{
  int rv;
  SOC_PPC_FEC_ID fec_ndx;

  BCMDNX_INIT_FUNC_DEFS;

  BCMDNX_VERIFY(ecmp != NULL);
  rv = _bcm_petra_l3_egress_ecmp_fec_ndx_check(unit, ecmp->ecmp_intf);
  BCMDNX_IF_ERR_EXIT(rv);
  
  fec_ndx = BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf);

  rv = _bcm_l3_is_alloced_ecmp(unit, fec_ndx);
  if (rv == BCM_E_NOT_FOUND) {
    *is_alloced = 0;
  } else if (rv == BCM_E_EXISTS) {
    *is_alloced = 1;
  } else {
    BCMDNX_IF_ERR_EXIT(rv);
  }

exit:
  BCMDNX_FUNC_RETURN;
}

/* 
 * (The ECMP is assumed to be allocated). 
 * This function gets the info from HW and the SW DB. 
 * It does not expect the ECMP to be allocated. 
 *  
 * Get some info on the ECMP: 
 * 1. Is intf protection_type == SOC_PPC_FRWRD_FEC_PROTECT_TYPE_NONE ? If yes ecmp_info.exists_protected == 0, o/w ecmp_info.exists_protected == 1. 
 *    intf should be an egress intf that encodes a FEC.
 * 2. Get the current ECMP size. 
 * 3. Get the first and last FEC. 
 */
STATIC int
  _bcm_petra_l3_egress_ecmp_get_ecmp_info(
    int unit,
    bcm_l3_egress_ecmp_t *ecmp,
    ecmp_info_t *ecmp_info
  )
{
  uint32                          soc_sand_rv;
  SOC_PPC_FRWRD_FEC_ECMP_INFO     ecmp_hw_info;
  SOC_PPC_FEC_ID                  ecmp_fec_ndx;
  int                             rv = BCM_E_NONE;

  BCMDNX_INIT_FUNC_DEFS;

  BCMDNX_VERIFY(ecmp != NULL);
  BCMDNX_VERIFY(ecmp_info != NULL);
  BCMDNX_VERIFY(_bcm_petra_l3_egress_ecmp_fec_ndx_check(unit, ecmp->ecmp_intf) == BCM_E_NONE);

  ecmp_fec_ndx = BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf);
  
  /* Get current and max size. */
  rv = bcm_sw_db_l3_get_ecmp_sizes(unit, ecmp_fec_ndx, &ecmp_info->max_size, &ecmp_info->curr_size);
  BCMDNX_IF_ERR_EXIT(rv);

  /* Get other stats. */
  soc_sand_rv = soc_ppd_frwrd_fec_ecmp_info_get(unit,ecmp_fec_ndx,&ecmp_hw_info);
  BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

  ecmp_info->is_successive = 1;
  ecmp_info->exists_protected = ecmp_hw_info.is_protected;
  ecmp_info->first_fec = ecmp_hw_info.base_fec_id;
  ecmp_info->last_fec = ecmp_info->first_fec + ecmp_info->curr_size * (1 + (ecmp_info->exists_protected ? 1 : 0)) - 1;
  
#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS(unit)) {
    ecmp_info->is_stateful = ecmp_hw_info.is_stateful;
  }
#endif /* BCM_88660_A0 */

exit:
  BCMDNX_FUNC_RETURN;
}

/* 
 * (The ECMP is assumed to be allocated). 
 * This function sets atomically (rollback if there is a problem) 
 * the ecmp_info to HW and SW DB. 
 *  
 * The following fields are set if needed (if they differ from current HW /SW DB values) : 
 * 1. ecmp_info->curr_size: The ECMP size.
 * 2. ecmp_info->max_size: The ECMP max size.
 * 3. ecmp_info->first_fec: The ECMP base FEC id.
 * 4. ecmp_info->exists_protected: The ECMP protected bit. 
 * [Arad+] 5. ecmp_info->is_stateful: Is the ECMP stateful.  
 *  
 * The rest of the fields are ignored. 
 */
STATIC int
  _bcm_petra_l3_egress_ecmp_set_ecmp_info(
    int unit,
    bcm_l3_egress_ecmp_t *ecmp,
    ecmp_info_t *ecmp_info
  )
{
  int rv = BCM_E_NONE;
  ecmp_info_t old_info;
  SOC_PPC_FEC_ID ecmp_fec_ndx = 0;
  uint8 need_rollback_on_err = 0;
  uint32 soc_sand_rv;

  BCMDNX_INIT_FUNC_DEFS;

  BCMDNX_VERIFY(ecmp != NULL);
  BCMDNX_VERIFY(ecmp_info != NULL);
  BCMDNX_VERIFY(_bcm_petra_l3_egress_ecmp_fec_ndx_check(unit, ecmp->ecmp_intf) == BCM_E_NONE);

  ecmp_fec_ndx = BCM_L3_ITF_VAL_GET(ecmp->ecmp_intf);

  rv = _bcm_petra_l3_egress_ecmp_get_ecmp_info(unit, ecmp, &old_info);
  BCMDNX_IF_ERR_EXIT(rv);
  
  if ((old_info.curr_size != ecmp_info->curr_size) ||
      (old_info.max_size != ecmp_info->max_size)) {
    /* Update curr size and max size for the ecmp group*/
    rv = bcm_sw_db_l3_set_ecmp_sizes(unit, ecmp_fec_ndx, ecmp_info->max_size, ecmp_info->curr_size);
    BCMDNX_IF_ERR_EXIT(rv);
  }
  
  need_rollback_on_err = 1;
  
  if ((old_info.first_fec != ecmp_info->first_fec) ||
      (old_info.exists_protected != ecmp_info->exists_protected) ||
      (old_info.curr_size != ecmp_info->curr_size)

#ifdef BCM_88660_A0
      || (SOC_IS_ARADPLUS(unit) && (old_info.is_stateful != ecmp_info->is_stateful))
#endif /* BCM_88660_A0 */
      ) {
    SOC_PPC_FRWRD_FEC_ECMP_INFO     ecmp_hw_info;

    SOC_PPC_FRWRD_FEC_ECMP_INFO_clear(&ecmp_hw_info);

    ecmp_hw_info.is_protected = ecmp_info->exists_protected;
    ecmp_hw_info.rpf_fec_index = 0;
    ecmp_hw_info.size = ecmp_info->curr_size;
    ecmp_hw_info.base_fec_id = ecmp_info->first_fec;

#ifdef BCM_88660_A0
    if (SOC_IS_ARADPLUS(unit)) {
      ecmp_hw_info.is_stateful = ecmp_info->is_stateful;
    }
#endif /* BCM_88660_A0 */

    soc_sand_rv = soc_ppd_frwrd_fec_ecmp_info_set(unit, ecmp_fec_ndx, &ecmp_hw_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
  }

exit:
  if ((_rv != BCM_E_NONE) && (need_rollback_on_err)) {
    rv = bcm_sw_db_l3_set_ecmp_sizes(unit, ecmp_fec_ndx, old_info.curr_size, old_info.max_size);
    BCMDNX_IF_ERR_EXIT(rv);
  }

  BCMDNX_FUNC_RETURN;
}

/*
 * This is done by reading H/W.
 */
STATIC int
  _bcm_petra_l3_is_fec_protected(int unit, bcm_if_t intf, uint8 *set_for_yes)
{

  BCMDNX_INIT_FUNC_DEFS;

  BCMDNX_VERIFY(BCM_L3_ITF_TYPE_IS_FEC(intf));

  BCMDNX_IF_ERR_EXIT(MBCM_PP_DRIVER_CALL(unit,mbcm_pp_frwrd_fec_is_protected_get,(unit,BCM_L3_ITF_VAL_GET(intf),set_for_yes)));
  
exit:
  BCMDNX_FUNC_RETURN;
}

/*
 * Do the following checks: 
 * 1. All intfs are actually FECs. 
 * 2. The FECs are successive. 
 * 3. Either all FECs are protected or none are. 
 *  
 * If print_errs is non-zero then errors are printed. 
 * In case of internal errors, an error is returned. 
 * Otherwise, the problem is returned in err. 
 */
STATIC int 
  _bcm_petra_l3_egress_ecmp_simple_fec_mgmt_check(
    int                           unit,
    int                           intf_count,
    bcm_if_t *                    intf_array,
    ecmp_simple_fec_mgmt_err_t *  err,
    uint8                         print_errs
  )
{
  int               rv = BCM_E_NONE;
  uint32            intf_idx;
  uint8             first_protected = FALSE;
  uint8             curr_protected;

  BCMDNX_INIT_FUNC_DEFS;

  *err = ECMP_SIMPLE_FEC_MGMT_ERR_NO_ERROR;

  if (intf_count == 0) {
    return BCM_E_NONE;
  }

  for (intf_idx = 0; intf_idx < intf_count; intf_idx++) {
    
    if (!BCM_L3_ITF_TYPE_IS_FEC(intf_array[intf_idx])) {
      if (print_errs) {
        LOG_ERROR(BSL_LS_BCM_L3,
                  (BSL_META_U(unit,
                              "Error - Interface 0x%X (intf_array[%d]) is not a FEC.\n"), intf_array[intf_idx], intf_idx));
      }
      *err = ECMP_SIMPLE_FEC_MGMT_ERR_NOT_FEC;
      BCM_EXIT;
    }

    rv = _bcm_petra_l3_is_fec_protected(unit, intf_array[intf_idx], &curr_protected);
    BCMDNX_IF_ERR_EXIT(rv);

    if (intf_idx == 0) {
      first_protected = curr_protected;
    } else if (curr_protected != first_protected) {
      if (print_errs) {
        LOG_ERROR(BSL_LS_BCM_L3,
                  (BSL_META_U(unit,
                              "WARNING - The FEC at index %d is %sprotected, but the FEC at index %d is %sprotected.\n"), 
                              0, first_protected ? "" : "not ", intf_idx, curr_protected ? "" : "not "));  
      }
      *err = ECMP_SIMPLE_FEC_MGMT_ERR_MIXED_PROTECTION;
      BCM_EXIT;
    }
    
    /* Check that the FECs are successive. */
    if (intf_idx > 0) {
      SOC_PPC_FEC_ID prev_fec_ndx = BCM_L3_ITF_VAL_GET(intf_array[intf_idx-1]);
      SOC_PPC_FEC_ID curr_fec_ndx = BCM_L3_ITF_VAL_GET(intf_array[intf_idx]);

      if (curr_fec_ndx != (prev_fec_ndx + 1 + (first_protected ? 1 : 0))) {
        if (print_errs) {
          LOG_ERROR(BSL_LS_BCM_L3,
                    (BSL_META_U(unit,
                                "Error - The FECs (0x%X and 0x%X) at indexes %d and %d are not successive.\n"), 
                                prev_fec_ndx, curr_fec_ndx, intf_idx - 1, intf_idx));
        }
        *err = ECMP_SIMPLE_FEC_MGMT_ERR_NOT_SUCCESSIVE;
        BCM_EXIT;
      }

    }
  }

exit:
  BCMDNX_FUNC_RETURN;
}

STATIC int
  _bcm_petra_l3_egress_ecmp_create_internal(
    int unit,
    bcm_l3_egress_ecmp_t *ecmp,
    int intf_count,
    bcm_if_t *intf_array
  )
{
  /* Valid values for flags/modes. */
  const uint32                    possible_ecmp_group_flags = 0;
  const uint32                    possible_flags = BCM_L3_WITH_ID | BCM_L3_REPLACE;

  int                             rv = BCM_E_NONE;
  uint8                           print_errs = 1;
  ecmp_simple_fec_mgmt_err_t      err = ECMP_SIMPLE_FEC_MGMT_ERR_NO_ERROR;
  bcm_l3_egress_ecmp_t            ecmp_obj_copy;
  ecmp_info_t                     ecmp_info;
  uint8                           new_ecmp_allocated = 0;
  uint32                          shr_flags = 0;
  uint32                          mode_idx;
  
  BCMDNX_INIT_FUNC_DEFS;

  BCM_DPP_UNIT_CHECK(unit);
  DPP_L3_UNIT_INIT_CHECK;

  BCMDNX_NULL_CHECK(ecmp);
  
  ecmp_obj_copy = *ecmp; 

  /* Check that the flags are valid. */
  if (ecmp_obj_copy.flags & ~(possible_flags)) {
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG_STR("Error - invalid flags.")));
  }

  /* Check that the ecmp_group_flags are valid. */
  if (ecmp_obj_copy.ecmp_group_flags & ~(possible_ecmp_group_flags)) {
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG_STR("Error - invalid ecmp_group_flags.")));
  }

  /* Check that if we have the REPLACE flag we also have the WITH_ID flag. */
  if ((ecmp_obj_copy.flags & BCM_L3_REPLACE) && (!(ecmp_obj_copy.flags & BCM_L3_WITH_ID))) {
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG_STR("Error - BCM_L3_REPLACE is only valid when used with BCM_L3_WITH_ID.")));
  }

  /* Check that intf_count non-negative and that intf_array is not null if intf_count is non-zero. */
  if (intf_count < 0) {
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG_STR("Error - intf_count must be positive.")));
  } else if (intf_count > 0) {
    BCMDNX_NULL_CHECK(intf_array);
  }

  /* Check that max_paths is not lower than intf_count. */
  if (ecmp_obj_copy.max_paths < intf_count) {
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG_STR("Error - intf_array (%d intfs) is too big for the ECMP group (max %d intfs).\n"), intf_count, ecmp_obj_copy.max_paths));
  }

  /* Check that max_paths is valid. */
  if ((ecmp_obj_copy.max_paths < 1) || (ecmp_obj_copy.max_paths > SOC_DPP_CONFIG(unit)->l3.ecmp_max_paths)) {
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG_STR("Error - max_paths must be an integer between 1 and %d."), SOC_DPP_CONFIG(unit)->l3.ecmp_max_paths));
  }

  /* Check that dynamic_mode is valid. */
  {
    uint32 dynamic_modes[] = {0, BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT};
    uint32 dynamic_modes_allowed[] = {TRUE, FALSE};
    uint32 dynamic_modes_nof = 2;

#ifdef BCM_88660_A0
    /* Resilient hashing - may be used either for stateful load balancing or for advanced hashing. */
    if (SOC_IS_ARADPLUS(unit)) {
      dynamic_modes_allowed[1] = TRUE;
    }
#endif /* BCM_88660_A0 */

    for (mode_idx = 0; mode_idx < dynamic_modes_nof; mode_idx++) {
      if ((ecmp_obj_copy.dynamic_mode == dynamic_modes[mode_idx]) && dynamic_modes_allowed[mode_idx]) {
        break;
      }
    }

    if (mode_idx == dynamic_modes_nof) {
      BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG_STR("Error - Illegal dynamic mode %d for this device."), ecmp_obj_copy.dynamic_mode));
    }
  }


  /* Check that all other dynamic variables are zero. */
  if (
      (ecmp_obj_copy.dynamic_age != 0) || 
      (ecmp_obj_copy.dynamic_expected_load_exponent != 0) || 
      (ecmp_obj_copy.dynamic_load_exponent != 0) ||
      (ecmp_obj_copy.dynamic_size != 0)
     ) {
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, 
                     (_BSL_BCM_MSG_STR("Error - The ecmp members dynamic_age, dynamic_expected_load_exponent, dynamic_load_exponent and dynamic_size must all be zero.")));
  }
   
  if (ecmp_obj_copy.flags & BCM_L3_WITH_ID) {

    /* Get the FEC index of the ECMP and set allocation flags. */
    shr_flags |= BCM_DPP_AM_FLAG_ALLOC_WITH_ID;

    /* Note 1: We assume that if this is an illegal FEC id, then the alloc will error out. */
    /* Note 2: The user can either input a FEC ID or an egress intf encoding a FEC ID. */
    rv = _bcm_petra_l3_egress_ecmp_fec_ndx_check(unit, ecmp_obj_copy.ecmp_intf);
    BCMDNX_IF_ERR_EXIT(rv);
  }

  /* If this is not replace then we must allocate the ECMP. */
  if (!(ecmp->flags & BCM_L3_REPLACE))
  {
    rv = _bcm_petra_l3_egress_ecmp_alloc_ecmp(unit, shr_flags, &ecmp_obj_copy);
    BCMDNX_IF_ERR_EXIT(rv);

    new_ecmp_allocated = TRUE;
  } else {
    uint8 is_allocated;
    
    /* Make sure the ECMP is allocated. */
    rv = _bcm_petra_l3_egress_ecmp_is_alloced_ecmp(unit, &ecmp_obj_copy, &is_allocated);
    BCMDNX_IF_ERR_EXIT(rv);
    
    if (!is_allocated) {
      BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG_STR("ECMP %d is not allocated.\n"), ecmp_obj_copy.ecmp_intf));
    }
  
  }

  /* In bcm_l3_egress_ecmp_* APIs we are very strict about the input. */
  /* After this we know that: */
  /* 1. The intf array contains FECs. */
  /* 2. The FEC array is successive. */
  /* 3. Either all FECs are protected, or all FECs are not protected. */
  rv = _bcm_petra_l3_egress_ecmp_simple_fec_mgmt_check(unit, intf_count, intf_array, &err, print_errs);
  BCMDNX_IF_ERR_EXIT(rv);

  if (err != ECMP_SIMPLE_FEC_MGMT_ERR_NO_ERROR) {
    BCM_ERR_EXIT_NO_MSG(BCM_E_PARAM);
  }

  ecmp_info.max_size = ecmp->max_paths;

#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS(unit) && (ecmp_obj_copy.dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT)) {
    ecmp_info.is_stateful = TRUE;
  } else {
    ecmp_info.is_stateful = FALSE;
  }
#endif /* BCM_88660_A0 */

  if (intf_count == 0) {
    /* If intf_count == 0 then use size = 1 and base_fec_id = 0. */
    ecmp_info.first_fec = 0;
    ecmp_info.curr_size = 1;
    ecmp_info.exists_protected = 0;
    
  } else {

    /* Is the group protected ? Determined by a single FEC (we use 0). */
    rv = _bcm_petra_l3_is_fec_protected(unit, intf_array[0], &ecmp_info.exists_protected);
    BCMDNX_IF_ERR_EXIT(rv);

    ecmp_info.curr_size = intf_count;
    ecmp_info.first_fec = BCM_L3_ITF_VAL_GET(intf_array[0]);
  }

  rv = _bcm_petra_l3_egress_ecmp_set_ecmp_info(unit, &ecmp_obj_copy, &ecmp_info);
  BCMDNX_IF_ERR_EXIT(rv);

  /* On success copy back the id. */
  ecmp->ecmp_intf = ecmp_obj_copy.ecmp_intf;

exit:
  
  /* If there was an error and we allocated a new ECMP we need to free it. */
  if ((_rv != BCM_E_NONE) && (new_ecmp_allocated)) {
    rv = _bcm_petra_l3_egress_ecmp_dealloc_ecmp(unit, &ecmp_obj_copy);
    if (rv != BCM_E_NONE) {
      LOG_WARN(BSL_LS_BCM_L3,
               (BSL_META_U(unit,
                           "WARNING: Could not deallocate ECMP %d.\n"), ecmp_obj_copy.ecmp_intf));
    }
  }
  
  BCMDNX_FUNC_RETURN;
}

/**
 * Purpose:
 *   Create an Egress ECMP forwarding object.
 * Parameters:
 *   unit                     [IN]    - Unit number.
 *   ecmp                     [INOUT] - The new ECMP object.
 *     ecmp.flags               [IN]  - Specify flags for the ecmp creation.
 *                                      Valid flags are: BCM_L3_WITH_ID, BCM_L3_REPLACE.
 *     ecmp.max_paths           [IN]  - Max num of members in the ECMP group.
 *     ecmp.ecmp_group_flags    [IN]  - None of these flags are valid (Must be zero).
 *     ecmp.dynamic_mode        [IN]  - May be 0 or BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT.
 *                                      If it is BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT then the
 *                                      ECMP group hash is stateful.
 *     ecmp.dynamic_*           [IN]  - Must be zero.
 *   intf_count               [IN]    - Size of intf_array.
 *   intf_array               [IN]    - Egress objects to be used for the ECMP group.
 * Returns: 
 *   BCM_E_PARAM 
 *   BCM_E_*
 * Notes:
 *   This function only works for Arad and above.
 */
int
  bcm_petra_l3_egress_ecmp_create(
    int unit,
    bcm_l3_egress_ecmp_t *ecmp,
    int intf_count,
    bcm_if_t *intf_array
  )
{
  int rv;

  BCMDNX_INIT_FUNC_DEFS;

  BCM_DPP_UNIT_CHECK(unit);
  DPP_L3_UNIT_INIT_CHECK;

  DPP_L3_LOCK_TAKE;

  rv = _bcm_petra_l3_egress_ecmp_create_internal(unit, ecmp, intf_count, intf_array);
  BCMDNX_IF_ERR_EXIT(rv);
  
exit:
  DPP_L3_LOCK_RELEASE;
  BCMDNX_FUNC_RETURN;
}

/**
 * Purpose:
 *   Get an ECMP object.
 * Parameters:
 *   unit                     [IN]    - Unit number.
 *   ecmp                     [INOUT] - The ECMP object to get.
 *     ecmp.ecmp_intf           [IN]  - Get this ECMP object.
 *     ecmp.max_paths           [OUT] - Max paths allowed for this ECMP object.
 *     ecmp.flags               [OUT] - Zero.
 *     ecmp.ecmp_group_flags    [OUT] - Zero.
 *     ecmp.dynamic_mode        [OUT] - If 0 then the ECMP is not stateful
 *                                      If BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT then the ECMP is stateful.
 *     ecmp.dynamic_*           [OUT] - Zero.
 *   intf_size                [IN]    - Size of the intf_array array.
 *   intf_array               [OUT]   - Filled with the egress objects belonging to this ECMP group.
 *   intf_count               [OUT]   - The amount of egress objects filled.
 *                                      If intf_size == 0 then this will be the current number of
 *                                      egress objects in the ECMP group.
 * Returns: 
 *   BCM_E_PARAM 
 *   BCM_E_*
 * Notes:
 *   This function only works for Arad and above.
 */
int
  _bcm_petra_l3_egress_ecmp_get_internal(
    int unit,
    bcm_l3_egress_ecmp_t *ecmp,
    int intf_size,
    bcm_if_t *intf_array,
    int *intf_count
  )
{
  int rv = BCM_E_NONE;
  int intf_array_idx;
  int max_idx;
  uint8 is_alloced;
  ecmp_info_t ecmp_info;

  BCMDNX_INIT_FUNC_DEFS;

  BCM_DPP_UNIT_CHECK(unit);
  DPP_L3_UNIT_INIT_CHECK;

  BCMDNX_NULL_CHECK(ecmp);
  BCMDNX_NULL_CHECK(intf_count);

  if (intf_size < 0) {
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG_STR("intf_size must not be negative.\n")));
  } else if (intf_size > 0) {
    BCMDNX_NULL_CHECK(intf_array);
  }

  rv = _bcm_petra_l3_egress_ecmp_is_alloced_ecmp(unit, ecmp, &is_alloced);
  BCMDNX_IF_ERR_EXIT(rv);

  if (!is_alloced) {
    BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG_STR("ECMP %d is not allocated.\n"), ecmp->ecmp_intf));
  }

  rv = _bcm_petra_l3_egress_ecmp_get_ecmp_info(unit, ecmp, &ecmp_info);
  BCMDNX_IF_ERR_EXIT(rv);
 
  ecmp->max_paths = ecmp_info.max_size;
  ecmp->flags = 0;
  ecmp->ecmp_group_flags = 0;
 
#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS(unit)) {
    ecmp->dynamic_mode = ecmp_info.is_stateful ? BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT : 0;
  }
#endif /* BCM_88660_A0 */

  /* Fill the intf array. */
  max_idx = SOC_SAND_MIN(ecmp_info.curr_size, intf_size);

  /* The ECMP data is sufficient to build the intf array. */
  for (intf_array_idx = 0; intf_array_idx < max_idx; intf_array_idx++) {
    SOC_PPC_FEC_ID curr_fec_ndx = ecmp_info.first_fec + intf_array_idx * (1 + (ecmp_info.exists_protected ? 1 : 0));
    _bcm_l3_fec_to_intf(unit, curr_fec_ndx, intf_array + intf_array_idx);
  }

  /* If intf_size == 0 then return the ecmp size. */
  *intf_count = (intf_size == 0) ? ecmp_info.curr_size : max_idx;

exit:
  
  BCMDNX_FUNC_RETURN;
}

/**
 * Purpose:
 *   Destroy an Egress ECMP forwarding object.
 * Parameters:
 *   unit               [IN] - Unit number.
 *   ecmp               [IN] - The ECMP object to destroy.
 *     ecmp.ecmp_intf     [IN] - Destroyed.
 *     ecmp.*             [IN] - Ignored.
 * Returns: 
 *   BCM_E_PARAM
 *   BCM_E_*
 * Notes:
 *   This function only works for Arad and above.
 */
int
  bcm_petra_l3_egress_ecmp_destroy(
    int unit,
    bcm_l3_egress_ecmp_t *ecmp
  )
{
  int rv = BCM_E_NONE;
  const uint32 possible_flags = 0;
  const uint32 possible_ecmp_group_flags = 0;
  ecmp_info_t ecmp_info;
  uint8 is_alloced;

  BCMDNX_INIT_FUNC_DEFS;

  BCM_DPP_UNIT_CHECK(unit);
  DPP_L3_UNIT_INIT_CHECK;
  DPP_L3_LOCK_TAKE;

  BCMDNX_NULL_CHECK(ecmp);

  if (ecmp->flags & ~(possible_flags)) {
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG_STR("invalid flags.")));
  }

  if (ecmp->ecmp_group_flags & ~(possible_ecmp_group_flags)) {
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG_STR("invalid ecmp_group_flags.")));
  }

  /* Is this ECMP allocated ? */
  rv = _bcm_petra_l3_egress_ecmp_is_alloced_ecmp(unit, ecmp, &is_alloced);
  BCMDNX_IF_ERR_EXIT(rv);

  if (!is_alloced) {
    BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG_STR("ECMP %d is not allocated.\n"), ecmp->ecmp_intf));
  }

  /* Clear H/W and SW DB. */
  ecmp_info.curr_size = 1;
  ecmp_info.max_size = 0;
  ecmp_info.exists_protected = 0;
  ecmp_info.first_fec = 0;

#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS(unit)) {
    ecmp_info.is_stateful = 0;
  }
#endif /* BCM_88660_A0 */

  rv = _bcm_petra_l3_egress_ecmp_set_ecmp_info(unit, ecmp, &ecmp_info);
  BCMDNX_IF_ERR_EXIT(rv);

  /* Next deallocate the ECMP. */
  rv = _bcm_petra_l3_egress_ecmp_dealloc_ecmp(unit, ecmp);
  BCMDNX_IF_ERR_EXIT(rv);

exit:
  DPP_L3_LOCK_RELEASE;
  BCMDNX_FUNC_RETURN;
}

int
  bcm_petra_l3_egress_ecmp_get(
    int unit,
    bcm_l3_egress_ecmp_t *ecmp,
    int intf_size,
    bcm_if_t *intf_array,
    int *intf_count
  )
{
  int rv = BCM_E_NONE;

  BCMDNX_INIT_FUNC_DEFS;

  BCM_DPP_UNIT_CHECK(unit);
  DPP_L3_UNIT_INIT_CHECK;
  DPP_L3_LOCK_TAKE;

  rv = _bcm_petra_l3_egress_ecmp_get_internal(unit, ecmp, intf_size, intf_array, intf_count);
  BCMDNX_IF_ERR_EXIT(rv);

exit:
  DPP_L3_LOCK_RELEASE;
  BCMDNX_FUNC_RETURN;
}

/**
 * Purpose:
 *   Add an egress forwarding object to an Egress ECMP forwarding object.
 * Parameters:
 *   unit                     [IN] - Unit number.
 *   ecmp                     [IN] - The ECMP group to add the egress object to.
 *     ecmp.ecmp_intf           [IN] - ECMP group identifier.
 *     ecmp.max_paths           [IN] - Updated according to the policy in create for REPLACE.
 *     ecmp.flags               [IN] - Must be zero.
 *     ecmp.ecmp_group_flags    [IN] - Must be zero.
 *     ecmp.dynamic_*           [IN] - Updated according to the policy in create for REPLACE.
 *   intf                     [IN] - The egress object to add.
 * Returns:
 *   BCM_E_*
 * Notes:
 *   This function only works for Arad and above.
 *   The interface must be valid with the array - i.e. if it were used in create
 *   it would have passed the tests.
 */
int
  bcm_petra_l3_egress_ecmp_add(
    int unit,
    bcm_l3_egress_ecmp_t *ecmp,
    bcm_if_t intf
  )
{
  int rv = BCM_E_NONE;
  uint32 possible_ecmp_flags = 0;
  uint32 possible_ecmp_group_flags = 0;
  int curr_intf_count = 0;
  int intf_count_get = 0;
  bcm_if_t *intf_array = NULL;
  bcm_if_t *first_intf = NULL;
  bcm_if_t *last_intf = NULL;
  ecmp_simple_fec_mgmt_err_t err;
  uint8 dont_print_errs = FALSE;
  uint8 print_errs = TRUE;
  bcm_l3_egress_ecmp_t replace_ecmp = *ecmp;
  bcm_l3_egress_ecmp_t ecmp_copy = *ecmp;

  BCMDNX_INIT_FUNC_DEFS;

  BCM_DPP_UNIT_CHECK(unit);
  DPP_L3_UNIT_INIT_CHECK;
  DPP_L3_LOCK_TAKE;

  BCMDNX_NULL_CHECK(ecmp);

  if (ecmp_copy.flags & ~(possible_ecmp_flags)) {
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG_STR("Invalid ecmp flags.")));
  }

  if (ecmp_copy.ecmp_group_flags & ~(possible_ecmp_group_flags)) {
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG_STR("Invalid ecmp group flags.")));
  }
  
  /* The plan is to replace the FEC array with a new one. */
  replace_ecmp.flags |= BCM_L3_REPLACE | BCM_L3_WITH_ID;

  /* First get the current ecmp size (and check that the ECMP exists). */
  rv = _bcm_petra_l3_egress_ecmp_get_internal(unit, &ecmp_copy, 0, NULL, &curr_intf_count);
  BCMDNX_IF_ERR_EXIT(rv);

  if (curr_intf_count == 0) {
    /* We simply replace the old intf array with a new one (with the new intf) and do create with replace. */
    rv = _bcm_petra_l3_egress_ecmp_create_internal(unit, &replace_ecmp, 1, &intf);
    BCMDNX_IF_ERR_EXIT(rv);
    BCM_EXIT;
  }

  /* Allocate an array for the intfs. */
  /* We allocate two extra cells in case we need to insert in the back or front. */
  BCMDNX_ALLOC(intf_array, sizeof(*intf_array) * (curr_intf_count + 2), "intf_array");
  if (intf_array == NULL) {        
      BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG_STR("failed to allocate memory")));
  }

  first_intf = intf_array + 1;
  last_intf = first_intf + curr_intf_count - 1;

  /* Get the current intf_array. */
  rv = _bcm_petra_l3_egress_ecmp_get_internal(unit, &ecmp_copy, curr_intf_count, first_intf, &intf_count_get);
  BCMDNX_IF_ERR_EXIT(rv);

  BCMDNX_VERIFY(intf_count_get == curr_intf_count);

  /* Now we check if the new intf can fit in to the FEC array - after the last FEC or before the first FEC.*/
  /* We assume that to check if a new FEC fits, it suffices to check it */
  /* with an adjacent FEC (both protection and successiveness are satisfied by such a check). */
  /* This is in contrast to checking the full FEC array with a new FEC which is unnecessary. */

  /* This check is done by first considering the array with the new FEC at the last place, */
  /* and afterwards considering the array with the new FEC at the first place. */

  /* Put the new FEC in both extra positions - before the first and after the last. */
  *(first_intf - 1) = *(last_intf+1) = intf;

  /* First check with the new FEC at the tail. */
  rv = _bcm_petra_l3_egress_ecmp_simple_fec_mgmt_check(unit, 2, last_intf, &err, dont_print_errs);
  BCMDNX_IF_ERR_EXIT(rv);

  if ((err != ECMP_SIMPLE_FEC_MGMT_ERR_NO_ERROR) && (err != ECMP_SIMPLE_FEC_MGMT_ERR_MIXED_PROTECTION)) {
    /* If that fails, check with the new FEC at the head. */
    rv = _bcm_petra_l3_egress_ecmp_simple_fec_mgmt_check(unit, 2, first_intf - 1, &err, dont_print_errs);
    BCMDNX_IF_ERR_EXIT(rv);

    if ((err != ECMP_SIMPLE_FEC_MGMT_ERR_NO_ERROR) && (err != ECMP_SIMPLE_FEC_MGMT_ERR_MIXED_PROTECTION)) {
      /* Both insertions are invalid - print errors. */
      LOG_ERROR(BSL_LS_BCM_L3,
                (BSL_META_U(unit,
                            "Could not insert the new intf at the end or at the start of the current group.\n")));
      LOG_ERROR(BSL_LS_BCM_L3,
                (BSL_META_U(unit,
                            "Could not insert the new intf at the end of the group: \n")));
      rv = _bcm_petra_l3_egress_ecmp_simple_fec_mgmt_check(unit, 2, last_intf, &err, print_errs);
      BCMDNX_VERIFY(rv == BCM_E_NONE);
      LOG_ERROR(BSL_LS_BCM_L3,
                (BSL_META_U(unit,
                            "Could not insert the new intf at the start of the group: \n")));
      rv = _bcm_petra_l3_egress_ecmp_simple_fec_mgmt_check(unit, 2, first_intf - 1, &err, print_errs);
      BCMDNX_VERIFY(rv == BCM_E_NONE);
      BCM_ERR_EXIT_NO_MSG(BCM_E_PARAM);
    } else {
      /* else - Add the array with the new intf at the beginning of the new array. */
      first_intf--;
    }
  } /* else - everything is ok, just add the array as it is with the new FEC at the end.*/

  /* Finally do create with replace. */
  rv = _bcm_petra_l3_egress_ecmp_create_internal(unit, &replace_ecmp, curr_intf_count+1, first_intf);
  BCMDNX_IF_ERR_EXIT(rv);

exit:
  if (intf_array) {
    BCM_FREE(intf_array);
  }

  DPP_L3_LOCK_RELEASE;
  BCMDNX_FUNC_RETURN;
}

/**
 * Purpose:
 *   Delete an Egress forwarding object from an Egress ECMP forwarding
 *   object.
 * Parameters:
 *   unit                     [IN] - Unit number.
 *   ecmp                     [IN] - The ECMP group to delete the egress object from.
 *     ecmp.ecmp_intf           [IN] - ECMP group identifier.
 *     ecmp.max_paths           [IN] - Updated according to the policy in create for REPLACE.
 *     ecmp.flags               [IN] - Must be zero.
 *     ecmp.ecmp_group_flags    [IN] - Must be zero.
 *     ecmp.dynamic_*           [IN] - Updated according to the policy in create for REPLACE.
 *   intf                     [IN] - The egress object to delete.
 * Returns: 
 *   BCM_E_PARAM 
 *   BCM_E_*
 * Notes:
 *   1) This function only works for Arad and above.
 *   2) Only the last or first FECs may be deleted.
 */
int
  bcm_petra_l3_egress_ecmp_delete(
    int unit,
    bcm_l3_egress_ecmp_t *ecmp,
    bcm_if_t intf
  )
{
  int rv = BCM_E_NONE;
  int curr_intf_count = 0;
  int intf_count_get = 0;
  uint32 possible_ecmp_flags = 0;
  uint32 possible_ecmp_group_flags = 0;
  bcm_if_t *intf_array = NULL;
  bcm_if_t *new_intf_array = NULL;
  bcm_l3_egress_ecmp_t replace_ecmp = *ecmp;
  bcm_l3_egress_ecmp_t ecmp_copy = *ecmp;

  BCMDNX_INIT_FUNC_DEFS;

  BCM_DPP_UNIT_CHECK(unit);
  DPP_L3_UNIT_INIT_CHECK;
  DPP_L3_LOCK_TAKE;

  BCMDNX_NULL_CHECK(ecmp);

  if (ecmp_copy.flags & ~(possible_ecmp_flags)) {
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG_STR("Invalid ECMP flags.\n")));
  }

  if (ecmp_copy.ecmp_group_flags & ~(possible_ecmp_group_flags)) {
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG_STR("Invalid ECMP group flags.\n")));
  }

  /* The plan is to replace the FEC array with a new one. */
  replace_ecmp.flags |= BCM_L3_REPLACE | BCM_L3_WITH_ID;

  /* First get the current ecmp size (and check that the ECMP exists). */
  rv = _bcm_petra_l3_egress_ecmp_get_internal(unit, &ecmp_copy, 0, NULL, &curr_intf_count);
  BCMDNX_IF_ERR_EXIT(rv);

  if (curr_intf_count == 0) {
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG_STR("The ECMP group is empty.\n")));
  }

  /* Allocate an array for the intfs. */
  /* We allocate two extra cells in case we need to insert in the back or front. */
  BCMDNX_ALLOC(intf_array, sizeof(*intf_array) * curr_intf_count, "intf_array");
  if (intf_array == NULL) {        
      BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG_STR("failed to allocate memory.\n")));
  }

  /* Get the current intf_array. */
  rv = _bcm_petra_l3_egress_ecmp_get_internal(unit, &ecmp_copy, curr_intf_count, intf_array, &intf_count_get);
  BCMDNX_IF_ERR_EXIT(rv);
  BCMDNX_VERIFY(intf_count_get == curr_intf_count);

  new_intf_array = intf_array;
  
  /* Correct the array. */
  if (intf_array[0] == intf) {
    new_intf_array++;
  } else if (intf_array[curr_intf_count - 1] == intf) {
    /* Do nothing. */
  } else {
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG_STR("Error - Can only remove the last FEC or the first FEC.\n")));
  }

  /* Finally do create with replace. */
  rv = _bcm_petra_l3_egress_ecmp_create_internal(unit, &replace_ecmp, curr_intf_count - 1, new_intf_array);
  BCMDNX_IF_ERR_EXIT(rv);

exit:
  if (intf_array) {
    BCM_FREE(intf_array);
  }

  DPP_L3_LOCK_RELEASE;
  BCMDNX_FUNC_RETURN;
}

/**
 * Purpose:
 *   Traverse through the ecmp egress object table and run callback at each
 *   valid entry.
 * Parameters:
 *   unit [IN] - Unit number.
 *   trav_fun [IN] - The callback.
 *   user_data [IN] - Opaque user data to be passed back to the callback.
 * Returns: 
 *   BCM_E_PARAM 
 *   BCM_E_*
 * Notes:
 *   This function only works for Arad and above.
 *   If the callback returns an error rv (!= BCM_E_NONE), then traverse stops.
 *   IMPORTANT: We only traverse the ECMPs, and we do NOT pass FEC arrays to the callback (intf_array == NULL, intf_count == 0 always).
 *   To get those use bcm_l3_egress_ecmp_get.
 */
int _bcm_petra_l3_egress_ecmp_traverse_locked(
    int unit,
    bcm_l3_egress_ecmp_traverse_cb trav_fn,
    void *user_data)
{
  int rv = BCM_E_NONE;
  SOC_PPC_FEC_ID fec_idx;

  BCMDNX_INIT_FUNC_DEFS;

  BCM_DPP_UNIT_CHECK(unit);
  DPP_L3_UNIT_INIT_CHECK;
  DPP_L3_LOCK_TAKE;

  BCMDNX_NULL_CHECK(trav_fn);

  /* Remark: We assume that the FECs are traversed in sequential order and that the first FECs are ECMPs. */
  for (fec_idx = 1; fec_idx < SOC_DPP_CONFIG(unit)->l3.fec_ecmp_reserved; fec_idx++) {
    int curr_ecmp_size;
    bcm_l3_egress_ecmp_t ecmp;
    uint8 is_alloced;

    bcm_l3_egress_ecmp_t_init(&ecmp);
    _bcm_l3_fec_to_intf(unit, fec_idx, &ecmp.ecmp_intf);

    /* Is this ECMP allocated ? */
    rv = _bcm_petra_l3_egress_ecmp_is_alloced_ecmp(unit, &ecmp, &is_alloced);
    BCMDNX_IF_ERR_EXIT(rv);

    if (!is_alloced) {
      continue;
    }

    rv = _bcm_petra_l3_egress_ecmp_get_internal(unit, &ecmp, 0, NULL, &curr_ecmp_size);
    BCMDNX_IF_ERR_EXIT(rv);
    
    rv = (*trav_fn)(unit, &ecmp, 0, NULL, user_data);
    BCMDNX_IF_ERR_EXIT(rv);
    
  }
  
exit:
  DPP_L3_LOCK_RELEASE;
  BCMDNX_FUNC_RETURN;
}

typedef struct _bcm_petra_l3_egress_ecmp_find_traverse_callback_data_s {
  SOC_PPC_FEC_ID lowest_fec_idx;
  SOC_PPC_FEC_ID highest_fec_idx;
  bcm_l3_egress_ecmp_t ecmp;
  uint8 success;
} _bcm_petra_l3_egress_ecmp_find_traverse_callback_data_t ;

int
  _bcm_petra_l3_egress_ecmp_find_traverse_callback(
    int unit, 
    bcm_l3_egress_ecmp_t *ecmp, 
    int intf_count, 
    bcm_if_t *intf_array, 
    void *user_data)
{
  int rv = BCM_E_NONE;
  ecmp_info_t ecmp_info;
  _bcm_petra_l3_egress_ecmp_find_traverse_callback_data_t *find_data = (_bcm_petra_l3_egress_ecmp_find_traverse_callback_data_t*)user_data;

  BCMDNX_INIT_FUNC_DEFS;

  BCMDNX_VERIFY(user_data != NULL);

  rv = _bcm_petra_l3_egress_ecmp_get_ecmp_info(unit, ecmp, &ecmp_info);
  BCMDNX_IF_ERR_EXIT(rv);
  
  /* Size is 0. */
  if (ecmp_info.curr_size < 1) {
    BCM_EXIT; /* continue traverse */
  }

  /* Check if all intfs are in the range. */
  if ((ecmp_info.first_fec <= find_data->lowest_fec_idx) && (ecmp_info.last_fec >= find_data->highest_fec_idx)) {
    find_data->success = TRUE;
    find_data->ecmp = *ecmp;
    /* Stop traverse - use some positive error (it will be ignored anyways). */
    BCM_ERR_EXIT_NO_MSG(1);
  }

  BCM_EXIT; /* continue traverse */

exit:
  BCMDNX_FUNC_RETURN;
}

/**
 * Purpose:
 *   Find (a single) ECMP object that has all intf_count interfaces
 *   in it.
 * Parameters:
 *   unit [IN] - Unit number.
 *   intf_count [IN] - Count of intfs in intf_array.
 *   intf_array [IN] - The interfaces to look for.
 *   ecmp [OUT] - Same as get for an egress object containing all intfs in intf_array (on success).
 *                For details see bcm_l3_egress_ecmp_get.
 * Returns: 
 *   BCM_E_NONE - If found. 
 *   BCM_E_NOT_FOUND - If not found.
 *   BCM_E_* - Other errors. 
 * Notes:
 *   This function only works for Arad and above.
 *   This function will always find a particular occurance of an ECMP pointing
 *   to all requested FECs (i.e. if there are multiple ECMPs that fulfill the criteria,
 *   there is no way to traverse them). To find other matches, use
 *   bcm_l3_egress_ecmp_traverse instead.
 */
int
  bcm_petra_l3_egress_ecmp_find(
    int unit,
    int intf_count,
    bcm_if_t *intf_array,
    bcm_l3_egress_ecmp_t *ecmp
  )
{
  int rv = BCM_E_NONE;
  _bcm_petra_l3_egress_ecmp_find_traverse_callback_data_t find_data;
  SOC_PPC_FEC_ID highest_ndx;
  SOC_PPC_FEC_ID lowest_ndx;
  int intf_idx;

  BCMDNX_INIT_FUNC_DEFS;

  BCM_DPP_UNIT_CHECK(unit);
  DPP_L3_UNIT_INIT_CHECK;

  BCMDNX_NULL_CHECK(ecmp);

  if (intf_count <= 0) {
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG_STR("At least one intf must be specified.\n")));
  }

  BCMDNX_NULL_CHECK(intf_array);

  rv = _bcm_l3_intf_to_fec(unit, intf_array[0], &highest_ndx);
  BCMDNX_IF_ERR_EXIT(rv);
  rv = _bcm_l3_intf_to_fec(unit, intf_array[0], &lowest_ndx);
  BCMDNX_IF_ERR_EXIT(rv);

  /* Calculate the start and end of the minimal range that contains all intfs. */
  for (intf_idx = 1; intf_idx < intf_count; intf_idx++) {
    SOC_PPC_FEC_ID curr_fec_ndx;
    rv = _bcm_l3_intf_to_fec(unit, intf_array[intf_idx], &curr_fec_ndx);
    BCMDNX_IF_ERR_EXIT(rv);

    lowest_ndx = SOC_SAND_MIN(lowest_ndx, curr_fec_ndx);
    highest_ndx = SOC_SAND_MAX(highest_ndx, curr_fec_ndx);
  }

  find_data.highest_fec_idx = highest_ndx;
  find_data.lowest_fec_idx = lowest_ndx;
  find_data.success = FALSE;
  rv = _bcm_petra_l3_egress_ecmp_traverse_locked(unit, _bcm_petra_l3_egress_ecmp_find_traverse_callback, &find_data);
  
  /* If we hit something, then we don't care about errors. */  
  if (find_data.success) {
    *ecmp = find_data.ecmp;
    /* Just in case... */
    BCMDNX_VERIFY(rv == 1); 
  } else if (rv != BCM_E_NONE) {
    BCMDNX_IF_ERR_EXIT(rv);
  } else {
    /* No such ECMP exists. */
    BCM_ERR_EXIT_NO_MSG(BCM_E_NOT_FOUND);
  }
  
exit:
  BCMDNX_FUNC_RETURN;

}

int bcm_petra_l3_egress_ecmp_traverse(
    int unit,
    bcm_l3_egress_ecmp_traverse_cb trav_fn,
    void *user_data)
{
  int rv = BCM_E_NONE;

  BCMDNX_INIT_FUNC_DEFS;

  BCM_DPP_UNIT_CHECK(unit);
  DPP_L3_UNIT_INIT_CHECK;

  rv = _bcm_petra_l3_egress_ecmp_traverse_locked(unit, trav_fn, user_data);
  BCMDNX_IF_ERR_EXIT(rv);

exit:
  BCMDNX_FUNC_RETURN;
}

#ifdef BCM_88660_A0

/** 
 * PURPOSE 
 *   Traverse all SLB entries and execute the callback (trav_fn) 
 *   on each entry that matchs the rule specified in match_entry.
 *  
 * PARAMETERS 
 *   unit [IN]
 *   flags [IN]                                 - Specify options. Flag combinations are allowed.
 *     BCM_L3_ECMP_RESILIENT_MATCH_ECMP [FLAG]    - Match the ECMP group (specified in match_entry).
 *                                                  The LSB of the ECMP is ignored (due to a
 *                                                  hardware limitation).
 *     BCM_L3_ECMP_RESILIENT_MATCH_INTF [FLAG]    - Match the interface (specified in match_entry).
 *   match_entry [IN]                           - Specify the rule to use when matching entries.
 *     ecmp [IN]                                  - The ECMP group to match (in case MATCH_ECMP is specified).
 *     intf [IN]                                  - The interface to match (in case MATCH_INTF is specified).
 *   trav_fn [IN]                               - A callback to be called for each matching entry.
 *                                                If the callback returns a non-zero result, then
 *                                                traverse stops, and BCM_E_FAIL is returned.
 *   user_data [IN]                             - Opaque user data to be passed back to the callback.
 *  
 * RETURNS 
 *   BCM_E_PARAM     - Error in parameters.
 *   BCM_E_INTERNAL  - Internal error or error in parameters.
 *   BCM_E_FAIL      - The traverse callback returned a non-zero result. 
 *  
 * NOTES 
 *   This function is only available for Arad+. 
 *   If no match flag is specified then all entries are matched. 
 */
int bcm_petra_l3_egress_ecmp_resilient_traverse(
    int unit, 
    uint32 flags, 
    bcm_l3_egress_ecmp_resilient_entry_t *match_entry, 
    bcm_l3_egress_ecmp_resilient_traverse_cb trav_fn, 
    void *user_data)
{
  int rv = BCM_E_NONE;
  uint32 soc_sand_rv;

  uint32 nof_match_rules = 0;
  const uint32 possible_flags = _BCM_L3_EGRESS_ECMP_RESILIENT_MATCH_FLAGS;
  uint32 nof_scanned_entries;

  SOC_PPC_SLB_TRAVERSE_MATCH_RULE_ALL_ECMP all_ecmp_match_rule;
  SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_ECMP group_match_rule;
  SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_MEMBER_ECMP member_match_rule;
  const SOC_PPC_SLB_TRAVERSE_MATCH_RULE * match_rules[2];

  SOC_SAND_TABLE_BLOCK_RANGE block_range;

  SOC_PPC_SLB_ENTRY_KEY   *slb_keys = NULL;
  SOC_PPC_SLB_ENTRY_VALUE *slb_values = NULL;
  bcm_l3_egress_ecmp_resilient_entry_t *user_entries = NULL;

  BCMDNX_INIT_FUNC_DEFS;

  BCM_DPP_UNIT_CHECK(unit);
  DPP_L3_UNIT_INIT_CHECK;

  /* Only Arad+ and above is supported. */
  if (!SOC_IS_ARADPLUS(unit)) {
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG_STR("bcm_l3_egress_ecmp_resilient_replace is unsupported for this device.\n")));
  }

  /* Check flags */
  if ((flags & ~possible_flags) != 0) {
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG_STR("Invalid flags specified (0x%x).\n"), flags & ~possible_flags));
  }

  /* We currently do not support matching the key. */
  if ((flags & BCM_L3_ECMP_RESILIENT_MATCH_HASH_KEY) != 0) {
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG_STR("BCM_L3_ECMP_RESILIENT_MATCH_HASH_KEY (0x%x) is unsupported.\n"), BCM_L3_ECMP_RESILIENT_MATCH_HASH_KEY));
  }

  /* If we have some match flag, then check match_entry. */
  if (flags & _BCM_L3_EGRESS_ECMP_RESILIENT_MATCH_FLAGS) {
    BCMDNX_NULL_CHECK(match_entry);
  } 

  /* Check the callback is not NULL. */
  BCMDNX_NULL_CHECK(trav_fn);

  BCMDNX_ALLOC(slb_keys, sizeof(slb_keys[0]) * SOC_PPC_SLB_MAX_ENTRIES_FOR_GET_BLOCK, "slb_keys");
  BCMDNX_ALLOC(slb_values, sizeof(slb_values[0]) * SOC_PPC_SLB_MAX_ENTRIES_FOR_GET_BLOCK, "slb_values");
  BCMDNX_ALLOC(user_entries, sizeof(user_entries[0]) * SOC_PPC_SLB_MAX_ENTRIES_FOR_GET_BLOCK, "user_entries");

  if ((slb_keys == NULL) || (slb_values == NULL) || (user_entries == NULL)) {
    BCM_FREE(slb_keys);
    BCM_FREE(slb_values);
    BCM_FREE(user_entries);
    BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG_STR("Could not allocate enough memory for this operation.\n")));
  }

  SOC_PPC_SLB_CLEAR(&all_ecmp_match_rule, SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_MATCH_RULE_ALL_ECMP);
  SOC_PPC_SLB_CLEAR(&group_match_rule, SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_MATCH_RULE_LB_GROUP_ECMP);
  SOC_PPC_SLB_CLEAR(&member_match_rule, SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_MATCH_RULE_LB_GROUP_MEMBER_ECMP);

  /* If there are no match flags then match all. */
  if ((flags & _BCM_L3_EGRESS_ECMP_RESILIENT_MATCH_FLAGS) == 0) {
    match_rules[0] = SOC_PPC_SLB_DOWNCAST(&all_ecmp_match_rule, SOC_PPC_SLB_TRAVERSE_MATCH_RULE);
    nof_match_rules = 1;
  }

  if (flags & BCM_L3_ECMP_RESILIENT_MATCH_ECMP) {
    rv = _bcm_l3_intf_to_fec(unit, match_entry->ecmp.ecmp_intf, &group_match_rule.ecmp_ndx);
    BCMDNX_IF_ERR_EXIT(rv);

    match_rules[nof_match_rules] = SOC_PPC_SLB_DOWNCAST(&group_match_rule, SOC_PPC_SLB_TRAVERSE_MATCH_RULE);
    nof_match_rules++;
  } 

  if (flags & BCM_L3_ECMP_RESILIENT_MATCH_INTF) {
    rv = _bcm_l3_intf_to_fec(unit, match_entry->intf, &member_match_rule.ecmp_member_ndx);
    BCMDNX_IF_ERR_EXIT(rv);

    match_rules[nof_match_rules] = SOC_PPC_SLB_DOWNCAST(&member_match_rule, SOC_PPC_SLB_TRAVERSE_MATCH_RULE);
    nof_match_rules++;
  } 

  /* Prepare the block range. */
  soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&block_range);

  SOC_SAND_TBL_ITER_SET_BEGIN(&block_range.iter);
  
  block_range.entries_to_act = SOC_PPC_SLB_MAX_ENTRIES_FOR_GET_BLOCK;
  block_range.entries_to_scan = SOC_SAND_TBL_ITER_SCAN_ALL;

  /* Loop over all entries. */
  while (!SOC_SAND_TBL_ITER_IS_END(&block_range.iter)) {
    unsigned int entry_idx;

    DPP_L3_LOCK_TAKE; /* { */

      soc_sand_rv = soc_ppd_slb_get_block(
        unit,
        match_rules,
        nof_match_rules,
        &block_range,
        slb_keys,
        slb_values,
        &nof_scanned_entries
      );

      DPP_L3_LOCK_RELEASE

      /* Fill the user entries. */
      /* We keep the lock to run get. */
      for (entry_idx = 0; entry_idx < nof_scanned_entries; entry_idx++) {
        int intf_count;

        BCMDNX_VERIFY(slb_keys[entry_idx].is_fec);
        BCMDNX_VERIFY(slb_values[entry_idx].ecmp_valid);

        bcm_l3_egress_ecmp_t_init(&user_entries[entry_idx].ecmp);
        
        /* The ECMP FEC index. */
        _bcm_l3_fec_to_intf(unit, slb_keys[entry_idx].lb_group.ecmp_ndx, &user_entries[entry_idx].ecmp.ecmp_intf);

        /* Get the ECMP object. */
        DPP_L3_LOCK_TAKE;
        rv = _bcm_petra_l3_egress_ecmp_get_internal(unit, &user_entries[entry_idx].ecmp, 0, NULL, &intf_count);
        DPP_L3_LOCK_RELEASE;
        BCMDNX_IF_ERR_EXIT(rv);
        
        /* The FEC member. */
        _bcm_l3_fec_to_intf(unit, slb_values[entry_idx].ecmp_member_ndx, &user_entries[entry_idx].intf);

        user_entries[entry_idx].hash_key = slb_keys[entry_idx].flow_label_id;
      }
    /* } */

    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    for (entry_idx = 0; entry_idx < nof_scanned_entries; entry_idx++) {
      rv = trav_fn(unit, user_entries + entry_idx, user_data);
      if (rv != 0) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_FAIL);
      }
    }
  }

exit:
  BCMDNX_FUNC_RETURN;
}

/** 
 * PURPOSE 
 *   Perform an action on matching SLB ECMP entries.
 *  
 * PARAMETERS 
 *   unit [IN]
 *   flags [IN]                                 - Specify options. Flag combinations are allowed.
 *                                                Any combination of match flags (MATCH_XXX) is allowed.
 *                                                However exactly one action (REPLACE/DELETE/COUNT) flag
 *                                                must be specified.
 *  
 *     BCM_L3_ECMP_RESILIENT_MATCH_ECMP [FLAG]    - Match the ECMP group (specified in match_entry).
 *                                                  The LSB of the ECMP is ignored (due to a
 *                                                  hardware limitation).
 *     BCM_L3_ECMP_RESILIENT_MATCH_INTF [FLAG]    - Match the interface (specified in match_entry).
 *     BCM_L3_ECMP_RESILIENT_REPLACE [FLAG]       - Replace matching entries with the data specified
 *                                                  in replace_entry.
 *     BCM_L3_ECMP_RESILIENT_DELETE [FLAG]        - Delete matching entries.
 *     BCM_L3_ECMP_RESILIENT_COUNT [FLAG]         - Count matching entries (no action).
 *   match_entry [IN]                           - Specify the rule to use when matching entries.
 *     ecmp [IN]                                  - The ECMP group to match (in case MATCH_ECMP is specified).
 *     intf [IN]                                  - The interface to match (in case MATCH_INTF is specified).
 *   num_entries [OUT]                          - If non-NULL then the no. of entries that were matched is
 *                                                returned.
 *   replace_entry [IN]                         - In case the action is REPLACE, then this will determine
 *                                                the values that matching entries will be replaced with.
 *     member [IN]                                  - The ECMP member will be replaced to this.
 * RETURNS 
 *   BCM_E_PARAM     - Error in parameters.
 *   BCM_E_INTERNAL  - Internal error or error in parameters.
 *  
 * NOTES 
 *   This function is only available for Arad+. 
 *   If no match flag is specified then all entries are matched. 
 */
int bcm_petra_l3_egress_ecmp_resilient_replace(
    int unit, 
    uint32 flags, 
    bcm_l3_egress_ecmp_resilient_entry_t *match_entry, 
    int *num_entries, 
    bcm_l3_egress_ecmp_resilient_entry_t *replace_entry)
{
  int rv = BCM_E_NONE;
  uint32 soc_sand_rv;

  uint32 nof_match_rules = 0;
  uint32 num_entries_internal;
  const uint32 possible_flags = _BCM_L3_EGRESS_ECMP_RESILIENT_MATCH_FLAGS | _BCM_L3_EGRESS_ECMP_RESILIENT_ACTION_FLAGS;
  
  SOC_PPC_SLB_TRAVERSE_MATCH_RULE_ALL_ECMP all_ecmp_match_rule;
  SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_ECMP group_match_rule;
  SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_MEMBER_ECMP member_match_rule;
  const SOC_PPC_SLB_TRAVERSE_MATCH_RULE *match_rules[2];
  SOC_PPC_SLB_TRAVERSE_ACTION *action = 0;
  SOC_PPC_SLB_TRAVERSE_ACTION_COUNT count_action;
  SOC_PPC_SLB_TRAVERSE_ACTION_REMOVE remove_action;
  SOC_PPC_SLB_TRAVERSE_ACTION_UPDATE update_action;
  SOC_PPC_SLB_TRAVERSE_UPDATE_VALUE_ECMP_MEMBER update_value_ecmp_member;

  BCMDNX_INIT_FUNC_DEFS;

  BCM_DPP_UNIT_CHECK(unit);
  DPP_L3_UNIT_INIT_CHECK;

  /* This is only supported for Arad+ and above. */
  if (!SOC_IS_ARADPLUS(unit)) {
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG_STR("bcm_l3_egress_ecmp_resilient_replace is unsupported for this device.\n")));
  }

  /* Check possible flags. */
  if ((flags & ~possible_flags) != 0) {
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG_STR("Invalid flags specified (0x%x).\n"), flags & ~possible_flags));
  }

  /* If a match flag is present then check match_entry. */
  if ((flags & _BCM_L3_EGRESS_ECMP_RESILIENT_MATCH_FLAGS) != 0) {
    BCMDNX_NULL_CHECK(match_entry);
  }

  /* An action must be specified. */
  if ((flags & _BCM_L3_EGRESS_ECMP_RESILIENT_ACTION_FLAGS) == 0) {
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG_STR("No action specified.\n")));
  }

  /* We currently do not support matching the key. */
  if ((flags & BCM_L3_ECMP_RESILIENT_MATCH_HASH_KEY) != 0) {
    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG_STR("BCM_L3_ECMP_RESILIENT_MATCH_HASH_KEY (0x%x) is unsupported.\n"), BCM_L3_ECMP_RESILIENT_MATCH_HASH_KEY));
  }

  /* If the action is REPLACE then check replace_entry. */
  if (flags & BCM_L3_ECMP_RESILIENT_REPLACE) {
    BCMDNX_NULL_CHECK(replace_entry);
  }


  SOC_PPC_SLB_CLEAR(&all_ecmp_match_rule, SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_MATCH_RULE_ALL_ECMP);
  SOC_PPC_SLB_CLEAR(&group_match_rule, SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_MATCH_RULE_LB_GROUP_ECMP);
  SOC_PPC_SLB_CLEAR(&member_match_rule, SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_MATCH_RULE_LB_GROUP_MEMBER_ECMP);
  SOC_PPC_SLB_CLEAR(&count_action, SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_ACTION_COUNT);
  SOC_PPC_SLB_CLEAR(&remove_action, SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_ACTION_REMOVE);
  SOC_PPC_SLB_CLEAR(&update_action, SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_ACTION_UPDATE);
  SOC_PPC_SLB_CLEAR(&update_value_ecmp_member, SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_UPDATE_VALUE_ECMP_MEMBER);

  /* If no match specified then we match all ECMP entries. */
  if ((flags & _BCM_L3_EGRESS_ECMP_RESILIENT_MATCH_FLAGS) == 0) {
    match_rules[0] = SOC_PPC_SLB_DOWNCAST(&all_ecmp_match_rule, SOC_PPC_SLB_TRAVERSE_MATCH_RULE);
    nof_match_rules = 1;
  }

  /* Match ECMP group. */
  if (flags & BCM_L3_ECMP_RESILIENT_MATCH_ECMP) {
    rv = _bcm_l3_intf_to_fec(unit, match_entry->ecmp.ecmp_intf, &group_match_rule.ecmp_ndx);
    BCMDNX_IF_ERR_EXIT(rv);

    match_rules[nof_match_rules] = SOC_PPC_SLB_DOWNCAST(&group_match_rule, SOC_PPC_SLB_TRAVERSE_MATCH_RULE);
    nof_match_rules++;
  } 

  /* Match ECMP member. */
  if (flags & BCM_L3_ECMP_RESILIENT_MATCH_INTF) {
    rv = _bcm_l3_intf_to_fec(unit, match_entry->intf, &member_match_rule.ecmp_member_ndx);
    BCMDNX_IF_ERR_EXIT(rv);

    match_rules[nof_match_rules] = SOC_PPC_SLB_DOWNCAST(&member_match_rule, SOC_PPC_SLB_TRAVERSE_MATCH_RULE);
    nof_match_rules++;
  } 

  /* COUNT action. */
  if (flags & BCM_L3_ECMP_RESILIENT_COUNT) {
    action = SOC_PPC_SLB_DOWNCAST(&count_action, SOC_PPC_SLB_TRAVERSE_ACTION);

  /* DELETE action. */
  } else if (flags & BCM_L3_ECMP_RESILIENT_DELETE) {
    action = SOC_PPC_SLB_DOWNCAST(&remove_action, SOC_PPC_SLB_TRAVERSE_ACTION);

  /* REPLACE action. */
  } else if (flags & BCM_L3_ECMP_RESILIENT_REPLACE) {
    action = SOC_PPC_SLB_DOWNCAST(&update_action, SOC_PPC_SLB_TRAVERSE_ACTION);
    update_action.traverse_update_value = SOC_PPC_SLB_DOWNCAST(&update_value_ecmp_member, SOC_PPC_SLB_TRAVERSE_UPDATE_VALUE);
    update_value_ecmp_member.new_ecmp_member_ndx = replace_entry->intf;
  } else {
    BCMDNX_VERIFY(FALSE);
    action = NULL;
  }
  
  DPP_L3_LOCK_TAKE; /* { */

    soc_sand_rv = soc_ppd_slb_traverse(
      unit,
      match_rules,
      nof_match_rules,
      action,
      &num_entries_internal
    );

  /* } */ DPP_L3_LOCK_RELEASE;

  BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

  if (num_entries) {
    *num_entries = num_entries_internal;
  }

exit:
  BCMDNX_FUNC_RETURN;
}
#else
int bcm_petra_l3_egress_ecmp_resilient_traverse(
    int unit, 
    uint32 flags, 
    bcm_l3_egress_ecmp_resilient_entry_t *match_entry, 
    bcm_l3_egress_ecmp_resilient_traverse_cb trav_fn, 
    void *user_data)
{
  BCMDNX_INIT_FUNC_DEFS;

  BCM_DPP_UNIT_CHECK(unit);
  DPP_L3_UNIT_INIT_CHECK;

  BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG_STR("bcm_l3_egress_ecmp_resilient_traverse is unsupported for this device.\n")));

exit:
  BCMDNX_FUNC_RETURN;
}

int bcm_petra_l3_egress_ecmp_resilient_replace(
    int unit, 
    uint32 flags, 
    bcm_l3_egress_ecmp_resilient_entry_t *match_entry, 
    int *num_entries, 
    bcm_l3_egress_ecmp_resilient_entry_t *replace_entry)
{
  BCMDNX_INIT_FUNC_DEFS;

  BCM_DPP_UNIT_CHECK(unit);
  DPP_L3_UNIT_INIT_CHECK;

  BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG_STR("bcm_l3_egress_ecmp_resilient_traverse is unsupported for this device.\n")));
exit:
  BCMDNX_FUNC_RETURN;
}
#endif /* BCM_88660_A0 */

/*
 * Function:
 *      bcm_petra_l3_ingress_create
 * Purpose:
 *      Create an L3 Ingress Interface.
 * Parameters:
 *      unit - (IN) Unit number.
 *      ing_intf - (IN) <UNDEF>
 *      intf_id - (IN/OUT) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_ingress_create(
    int unit, 
    bcm_l3_ingress_t *ing_intf, 
    bcm_if_t *intf_id)
{
    unsigned int soc_sand_dev_id = 0, ret=0;
    SOC_PPC_RIF_INFO rif_info;
    uint32 if_id = SOC_PPC_RIF_NULL;
    uint8 routing_enablers_bm_id_tmp = 0, routing_enablers_bm_id_tmp_old = 0, with_id = 0;
    uint32 routing_enablers_bm_tmp = 0;
    int is_last_rout_enabler = 0; 
    int urpf_mode;
    bcm_l3_intf_t intf_ori;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_L3_UNIT_INIT_CHECK;

    DPP_L3_LOCK_TAKE;

    BCMDNX_NULL_CHECK(ing_intf);
    BCMDNX_NULL_CHECK(intf_id);

    /* invalid flag */
    if((ing_intf->flags & BCM_L3_INGRESS_WITH_ID) == 0) { 
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("flag BCM_L3_INGRESS_WITH_ID must be set")));
    }
    
    if( SOC_IS_JERICHO(unit) && (SOC_DPP_CONFIG(unit)->pp.ipmc_l3mcastl2_mode == 0) && (ing_intf->flags & BCM_L3_INGRESS_L3_MCAST_L2)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("flag BCM_L3_INGRESS_L3_MCAST_L2 must be set with ipmc_l3mcastl2_mode SOC property")));
    }

    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        /* invalid flags combinations */
    if (((ing_intf->flags & BCM_L3_INGRESS_ROUTE_DISABLE_IP4_UCAST) && !(ing_intf->flags & BCM_L3_INGRESS_ROUTE_DISABLE_IP6_UCAST)) ||
        (!(ing_intf->flags & BCM_L3_INGRESS_ROUTE_DISABLE_IP4_UCAST) && (ing_intf->flags & BCM_L3_INGRESS_ROUTE_DISABLE_IP6_UCAST)) ||
        ((ing_intf->flags & BCM_L3_INGRESS_ROUTE_DISABLE_IP4_MCAST) && !(ing_intf->flags & BCM_L3_INGRESS_ROUTE_DISABLE_IP6_MCAST)) ||
        (!(ing_intf->flags & BCM_L3_INGRESS_ROUTE_DISABLE_IP4_MCAST) && (ing_intf->flags & BCM_L3_INGRESS_ROUTE_DISABLE_IP6_MCAST))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("IP4 and IPV6 UCAST/MCAST: one cannot be disabled while the other is enabled")));
        }
    }

    if (SOC_IS_JERICHO(unit)) {
        if(ing_intf->intf_class > SOC_DPP_CONFIG(unit)->l3.intf_class_max_value){
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("intf_class value exceeded limit")));
        }
        rif_info.intf_class = ing_intf->intf_class;
    }

    if_id = *intf_id;
    
    if ((if_id >= SOC_DPP_CONFIG(unit)->l3.nof_rifs) && (if_id != SOC_PPC_RIF_NULL)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid interface")));
    }

    if (!DPP_VRF_VALID(unit, ing_intf->vrf)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid vrf")));
    }

    bcm_l3_intf_t_init(&intf_ori);
    intf_ori.l3a_intf_id = if_id;

    /*L3 intf must already exists. */
    BCMDNX_IF_ERR_EXIT(bcm_petra_l3_intf_get(unit, &intf_ori));

    soc_sand_dev_id = (unit);
    SOC_PPC_RIF_INFO_clear(&rif_info);


    if (if_id != SOC_PPC_RIF_NULL) {
        SOC_PPC_RIF_INFO_clear(&rif_info);

        BCM_SAND_IF_ERR_EXIT(soc_ppd_rif_info_get(soc_sand_dev_id, if_id, &rif_info));

        rif_info.cos_profile = BCM_QOS_MAP_PROFILE_GET(ing_intf->qos_map_id);
#ifdef BCM_88660_A0
        if (SOC_IS_ARADPLUS(unit) && soc_property_get(unit, spn_BCM886XX_L3_INGRESS_URPF_ENABLE, 0)) {
            rif_info.uc_rpf_mode = ing_intf->urpf_mode;
            switch (rif_info.uc_rpf_mode) {
                case bcmL3IngressUrpfLoose:
                    rif_info.uc_rpf_mode = SOC_PPC_FRWRD_FEC_RPF_MODE_UC_LOOSE;
                    rif_info.uc_rpf_enable = TRUE;
                    break;
                case bcmL3IngressUrpfStrict:
                    rif_info.uc_rpf_mode = SOC_PPC_FRWRD_FEC_RPF_MODE_UC_STRICT;
                    rif_info.uc_rpf_enable = TRUE;
                    break;
                case bcmL3IngressUrpfDisable:
                    rif_info.uc_rpf_enable = FALSE;
                    break;
                default:
                    LOG_ERROR(BSL_LS_BCM_L3,
                              (BSL_META_U(unit,
                                          "rif_info urpf_mode %d invalid %x\n"),ing_intf->urpf_mode, ret));
                    BCM_RETURN_VAL_EXIT(BCM_E_PARAM);
                    break;
            }
        } else {
#endif /* BCM_88660_A0 */
        switch (ing_intf->urpf_mode) {
            case bcmL3IngressUrpfLoose:
                BCMDNX_IF_ERR_EXIT(L3_ACCESS.dpp_l3_state.urpf_mode.get(unit, &urpf_mode));
                if (urpf_mode == bcmL3IngressUrpfLoose) {
                    rif_info.uc_rpf_enable = TRUE;
                } else {
                    LOG_WARN(BSL_LS_BCM_L3,
                             (BSL_META_U(unit,
                                         "rif_info urpf_mode %d mismatch, urpf_mode not enabled\n"),ing_intf->urpf_mode));
                }
                break;
            case bcmL3IngressUrpfStrict:
                BCMDNX_IF_ERR_EXIT(L3_ACCESS.dpp_l3_state.urpf_mode.get(unit, &urpf_mode));
                if (urpf_mode == bcmL3IngressUrpfStrict) {
                    rif_info.uc_rpf_enable = TRUE;
                } else {
                    LOG_WARN(BSL_LS_BCM_L3,
                             (BSL_META_U(unit,
                                         "rif_info urpf_mode %d mismatch, urpf_mode not enabled\n"),ing_intf->urpf_mode));
                }
                break;
            case bcmL3IngressUrpfDisable:
                rif_info.uc_rpf_enable = FALSE;
                break;
            default:
                LOG_ERROR(BSL_LS_BCM_L3,
                          (BSL_META_U(unit,
                                      "rif_info urpf_mode %d invalid %x\n"),ing_intf->urpf_mode, ret));
                BCM_RETURN_VAL_EXIT(BCM_E_PARAM);
            }
#ifdef BCM_88660_A0
        }
#endif /* BCM_88660_A0 */

        /* WARNING ABOUT RPF WITH SLB. */
        /* If RPF is used with SLB, then only LPM hits will be used (since the LEM search is used for SLB). */
        /* Therefore RPF will NOT be done for SIPs that are in the LEM. */
        if (SOC_IS_JERICHO(unit)) {
            routing_enablers_bm_id_tmp = rif_info.routing_enablers_bm_id;
            routing_enablers_bm_id_tmp_old = rif_info.routing_enablers_bm_id;
            routing_enablers_bm_tmp = rif_info.routing_enablers_bm;
        }
        rif_info.routing_enablers_bm = SOC_PPC_RIF_ROUTE_ENABLE_TYPE_ALL;
        if (ing_intf->flags & BCM_L3_INGRESS_ROUTE_DISABLE_IP4_UCAST) {
            rif_info.routing_enablers_bm &= ~SOC_PPC_RIF_ROUTE_ENABLE_TYPE_IPV4_UC;
        }
        if (ing_intf->flags & BCM_L3_INGRESS_ROUTE_DISABLE_IP6_UCAST) {
            rif_info.routing_enablers_bm &= ~SOC_PPC_RIF_ROUTE_ENABLE_TYPE_IPV6_UC;
        }

        if (ing_intf->flags & BCM_L3_INGRESS_ROUTE_DISABLE_IP6_MCAST) {
            rif_info.routing_enablers_bm &= ~SOC_PPC_RIF_ROUTE_ENABLE_TYPE_IPV6_MC;
        }
        if (ing_intf->flags & BCM_L3_INGRESS_ROUTE_DISABLE_IP4_MCAST) {
            rif_info.routing_enablers_bm &= ~SOC_PPC_RIF_ROUTE_ENABLE_TYPE_IPV4_MC;
        }

        if (ing_intf->flags & BCM_L3_INGRESS_ROUTE_DISABLE_MPLS) {
            rif_info.routing_enablers_bm &= ~SOC_PPC_RIF_ROUTE_ENABLE_TYPE_MPLS;
        }

        rif_info.enable_default_routing = (ing_intf->flags & BCM_L3_INGRESS_GLOBAL_ROUTE) != 0;

#if defined(INCLUDE_KBP)
            if(ing_intf->flags & BCM_L3_INGRESS_EXT_IPV4_DOUBLE_CAPACITY) {
                if (SOC_IS_JERICHO(unit)) {
                    if(ARAD_KBP_ENABLE_IPV4_DC){
                        rif_info.custom_rif_bit = 1;
                    }else{
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("flag BCM_L3_INGRESS_EXT_IPV4_DOUBLE_CAPACITY must be set with double capacity enabled")));
                    }
            }else{
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("flag BCM_L3_INGRESS_EXT_IPV4_DOUBLE_CAPACITY supported only for Jericho and above ")));
            }
        }
#else
        if( (ing_intf->flags & BCM_L3_INGRESS_EXT_IPV4_DOUBLE_CAPACITY )) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("flag BCM_L3_INGRESS_EXT_IPV4_DOUBLE_CAPACITY must be set with external tcam double capacity enabled")));
        }
#endif
        if (SOC_IS_JERICHO(unit)) {
            int old_is_last = 0;
            if(ing_intf->flags & BCM_L3_INGRESS_L3_MCAST_L2) {
                rif_info.custom_rif_bit = 1;
            }

            /*If current enablers is configurated without urpf, check if the template (enablers | urpf_type) exists or not. 
                       If yes, use it as current template. Otherwise, alloc a new template.*/
            if (rif_info.uc_rpf_enable == FALSE) {
                if (routing_enablers_bm_tmp != rif_info.routing_enablers_bm) {
                    if ((routing_enablers_bm_tmp == (rif_info.routing_enablers_bm | _BCM_L3_INGRESS_URPF_IS_LOOSE))
                        || (routing_enablers_bm_tmp == (rif_info.routing_enablers_bm | _BCM_L3_INGRESS_URPF_IS_STRICT))) {
                        rif_info.routing_enablers_bm = routing_enablers_bm_tmp;
                    }else {
                        routing_enablers_bm_tmp = rif_info.routing_enablers_bm | _BCM_L3_INGRESS_URPF_IS_LOOSE;
                        ret = _bcm_dpp_am_template_l3_rif_mac_termination_combination_profile_get(unit, routing_enablers_bm_tmp, &routing_enablers_bm_id_tmp);
                        if (ret != BCM_E_NONE) {
                            routing_enablers_bm_tmp = rif_info.routing_enablers_bm | _BCM_L3_INGRESS_URPF_IS_STRICT;
                            ret = _bcm_dpp_am_template_l3_rif_mac_termination_combination_profile_get(unit, routing_enablers_bm_tmp, &routing_enablers_bm_id_tmp);
                            if (ret == BCM_E_NONE) {/*strict exists*/
                                rif_info.routing_enablers_bm = routing_enablers_bm_tmp;
                                routing_enablers_bm_id_tmp = routing_enablers_bm_id_tmp_old;
                            }
                        }else {/*loose exists*/
                            rif_info.routing_enablers_bm = routing_enablers_bm_tmp;
                            routing_enablers_bm_id_tmp = routing_enablers_bm_id_tmp_old;
                        }
                    }
                }
            }else {
                uint32 urpf_type=0;
                if (ing_intf->urpf_mode == bcmL3IngressUrpfLoose) {
                    urpf_type = _BCM_L3_INGRESS_URPF_IS_LOOSE;
                }else if (ing_intf->urpf_mode == bcmL3IngressUrpfStrict) {
                    urpf_type = _BCM_L3_INGRESS_URPF_IS_STRICT;
                }

                /*If current enablers is configurated with urpf and can't find the template (enablers | urpf_type) in TM, check if the template (enablers) exists or not. 
                          If yes, replace it with (enablers_bm | urpf_type). Otherwise, alloc a new template.*/
                routing_enablers_bm_tmp = rif_info.routing_enablers_bm | urpf_type;
                ret = _bcm_dpp_am_template_l3_rif_mac_termination_combination_profile_get(unit, routing_enablers_bm_tmp, &routing_enablers_bm_id_tmp);
                if (ret != BCM_E_NONE) {
                    /*if urpf_type doesn't exist, check if enablers_bm exists or not*/
                    ret = _bcm_dpp_am_template_l3_rif_mac_termination_combination_profile_get(unit, rif_info.routing_enablers_bm, &routing_enablers_bm_id_tmp);
                    if (ret == BCM_E_NONE) {
                        /*the old template is got by 'soc_ppd_rif_info_get'. It might be 'SOC_PPC_RIF_ROUTE_ENABLE_TYPE_ALL' that created by L3 interface. So here
                                      need to decrease reference count for old template and add reference count for current template*/
                        if (routing_enablers_bm_id_tmp_old != routing_enablers_bm_id_tmp) {
                            ret = _bcm_dpp_am_template_l3_rif_mac_termination_combination_free(unit, routing_enablers_bm_id_tmp_old, &is_last_rout_enabler);
                            BCMDNX_IF_ERR_EXIT(ret);
                            ret = _bcm_dpp_am_template_l3_rif_mac_termination_combination_alloc(unit, 0, &rif_info.routing_enablers_bm, &rif_info.routing_enablers_bm_id, &rif_info.is_configure_enabler_needed);
                            BCMDNX_IF_ERR_EXIT(ret);
                        }

                        /*enablers exists, replace it with (enablers_bm | urpf_type)*/
                        with_id = 1;
                    }else {
                        /*enablers doesn't exist, alloc (enablers_bm | urpf_type)*/
                        routing_enablers_bm_id_tmp = rif_info.routing_enablers_bm_id;
                    }
                }else {
                    /* Decrease reference count for old template and add reference count for current template when profile changed */
                    if (routing_enablers_bm_id_tmp_old != routing_enablers_bm_id_tmp) {
                        ret = _bcm_dpp_am_template_l3_rif_mac_termination_combination_free(unit, routing_enablers_bm_id_tmp_old, &is_last_rout_enabler);
                        BCMDNX_IF_ERR_EXIT(ret);
                        ret = _bcm_dpp_am_template_l3_rif_mac_termination_combination_alloc(unit, 0, &routing_enablers_bm_tmp, &rif_info.routing_enablers_bm_id, &rif_info.is_configure_enabler_needed);
                        BCMDNX_IF_ERR_EXIT(ret);
                    }
                    /* If template (enablers_bm | urpf_type) exists, use the profile */
                    with_id = 1;
                }

                rif_info.routing_enablers_bm |= urpf_type;
            }

            /* assign profile to the current input flags if it is a new profile need to configure the HW */
            ret = _bcm_dpp_am_template_l3_rif_mac_termination_combination_exchange(unit, routing_enablers_bm_id_tmp, &old_is_last, &rif_info.routing_enablers_bm, &routing_enablers_bm_id_tmp, &rif_info.is_configure_enabler_needed, with_id);
            BCMDNX_IF_ERR_EXIT(ret);
            if (old_is_last && (!with_id)) { /* Need to write all 0 in the old allocated HW */
                int is_configure_enabler_needed_tmp = rif_info.is_configure_enabler_needed;
                routing_enablers_bm_tmp = rif_info.routing_enablers_bm;

                rif_info.routing_enablers_bm = 0;
                rif_info.is_configure_enabler_needed = 1;

                ret = soc_ppd_rif_info_set(soc_sand_dev_id, if_id, &rif_info);
                if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
                {
                    LOG_ERROR(BSL_LS_BCM_L3,
                              (BSL_META_U(unit,
                                          "rif_info_set soc_sand error %x\n"), ret));
                    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_sand_get_error_code_from_error_word failed")));
                }

                rif_info.is_configure_enabler_needed = is_configure_enabler_needed_tmp;
                rif_info.routing_enablers_bm = routing_enablers_bm_tmp;
            }
            rif_info.routing_enablers_bm_id = routing_enablers_bm_id_tmp;

            rif_info.intf_class = ing_intf->intf_class;
        }

        rif_info.vrf_id = ing_intf->vrf;

        /* HW rif configuration */
        ret = soc_ppd_rif_info_set(soc_sand_dev_id, if_id, &rif_info);
        if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
        { 
            LOG_ERROR(BSL_LS_BCM_L3,
                      (BSL_META_U(unit,
                                  "rif_info_set soc_sand error %x\n"), ret));
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_sand_get_error_code_from_error_word failed")));
        }
    }

    BCM_EXIT;
exit:
    DPP_L3_LOCK_RELEASE;
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_ingress_destroy
 * Purpose:
 *      Destroy an L3 Ingress Interface.
 * Parameters:
 *      unit - (IN) Unit number.
 *      intf_id - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_ingress_destroy(
    int unit, 
    bcm_if_t intf_id)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_L3_UNIT_INIT_CHECK;

    DPP_L3_LOCK_TAKE;

    DPP_L3_LOCK_RELEASE;

    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_l3_ingress_destroy is not available, intf delete should be used."))); 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_ingress_get
 * Purpose:
 *      Get L3 Ingress Interface configuration.
 * Parameters:
 *      unit - (IN) Unit number.
 *      intf - (IN) <UNDEF>
 *      ing_intf - (OUT) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_ingress_get(
    int unit, 
    bcm_if_t intf, 
    bcm_l3_ingress_t *ing_intf)
{
    unsigned int soc_sand_dev_id;
    SOC_PPC_RIF_INFO rif_info;
    uint32 if_id = intf;
    int urpf_mode;
    bcm_l3_intf_t intf_ori;
    int status = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_L3_UNIT_INIT_CHECK;

    DPP_L3_LOCK_TAKE;
    
    /* Input parameters check. */
    BCMDNX_NULL_CHECK(ing_intf);

    if ((if_id >= SOC_DPP_CONFIG(unit)->l3.nof_rifs) && (if_id != SOC_PPC_RIF_NULL)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid interface")));
    }

    bcm_l3_intf_t_init(&intf_ori);
    intf_ori.l3a_intf_id = if_id;

    /*Check L3 intf is existing or not.*/
    status = bcm_petra_l3_intf_get(unit, &intf_ori);
    if (status != BCM_E_NONE) {
        LOG_WARN(BSL_LS_BCM_L3,(BSL_META_U(unit, "L3 interface[%d] isn't existing!\n"), if_id));
    }

    soc_sand_dev_id = (unit);
    SOC_PPC_RIF_INFO_clear(&rif_info);  
    bcm_l3_ingress_t_init(ing_intf);
    
    if (intf != SOC_PPC_RIF_NULL) {
        BCM_SAND_IF_ERR_EXIT(soc_ppd_rif_info_get(soc_sand_dev_id, intf, &rif_info));
    }
    ing_intf->qos_map_id = rif_info.cos_profile;
    ing_intf->intf_class = rif_info.intf_class;
    ing_intf->intf_class_route_disable = rif_info.routing_enablers_bm_rif_profile;
    
    if (rif_info.uc_rpf_enable == FALSE) {
        ing_intf->urpf_mode = bcmL3IngressUrpfDisable;
    } else {
        BCMDNX_IF_ERR_EXIT(L3_ACCESS.dpp_l3_state.urpf_mode.get(unit, &urpf_mode));
        if (urpf_mode == SOC_PPC_FRWRD_FEC_RPF_MODE_UC_LOOSE) {
            ing_intf->urpf_mode = bcmL3IngressUrpfLoose;
        } else {
            ing_intf->urpf_mode = bcmL3IngressUrpfStrict;
        }
    }

#ifdef BCM_88660_A0
    if (SOC_IS_ARADPLUS(unit) && soc_property_get(unit, spn_BCM886XX_L3_INGRESS_URPF_ENABLE, 0) && rif_info.uc_rpf_enable != FALSE) {
        ing_intf->urpf_mode = rif_info.uc_rpf_mode;
    }
#endif /* BCM_88660_A0 */
    ing_intf->flags = 0;    
    if (!(rif_info.routing_enablers_bm & SOC_PPC_RIF_ROUTE_ENABLE_TYPE_IPV4_UC)) {
        ing_intf->flags |= (BCM_L3_INGRESS_ROUTE_DISABLE_IP4_UCAST);
    }
    if (!(rif_info.routing_enablers_bm & SOC_PPC_RIF_ROUTE_ENABLE_TYPE_IPV6_UC)) {
        ing_intf->flags |= (BCM_L3_INGRESS_ROUTE_DISABLE_IP6_UCAST);
    }

    if (!(rif_info.routing_enablers_bm & SOC_PPC_RIF_ROUTE_ENABLE_TYPE_IPV4_MC)) {
        ing_intf->flags |= (BCM_L3_INGRESS_ROUTE_DISABLE_IP4_MCAST);
    }    
    if (!(rif_info.routing_enablers_bm & SOC_PPC_RIF_ROUTE_ENABLE_TYPE_IPV6_MC)) {
        ing_intf->flags |= (BCM_L3_INGRESS_ROUTE_DISABLE_IP6_MCAST);
    }

    if (!(rif_info.routing_enablers_bm & SOC_PPC_RIF_ROUTE_ENABLE_TYPE_MPLS)) {
        ing_intf->flags |= BCM_L3_INGRESS_ROUTE_DISABLE_MPLS;
    }
    if (rif_info.enable_default_routing) {
        ing_intf->flags |= BCM_L3_INGRESS_GLOBAL_ROUTE;
    }
    if (SOC_IS_JERICHO(unit)) {
        if (rif_info.custom_rif_bit) {
            if ((SOC_DPP_CONFIG(unit)->pp.ipmc_l3mcastl2_mode == 1)) {
                ing_intf->flags |= BCM_L3_INGRESS_L3_MCAST_L2;
            }else{
                ing_intf->flags |= BCM_L3_INGRESS_EXT_IPV4_DOUBLE_CAPACITY;
            }
        }
    }

    ing_intf->vrf = rif_info.vrf_id;

    BCM_EXIT;
exit:
    DPP_L3_LOCK_RELEASE;
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_ingress_find
 * Purpose:
 *      Find an identical ingress L3 interface.
 * Parameters:
 *      unit - (IN) Unit number.
 *      ing_intf - (IN) <UNDEF>
 *      intf_id - (OUT) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_ingress_find(
    int unit, 
    bcm_l3_ingress_t *ing_intf, 
    bcm_if_t *intf_id)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_L3_UNIT_INIT_CHECK;

    DPP_L3_LOCK_TAKE;

    DPP_L3_LOCK_RELEASE;

    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_l3_ingress_find is not available"))); 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_ingress_traverse
 * Purpose:
 *      Traverse through the L3 ingress interfaces and invoke a user
 *      provide callback for each valid entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      trav_fn - (IN) <UNDEF>
 *      user_data - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_ingress_traverse(
    int unit, 
    bcm_l3_ingress_traverse_cb trav_fn, 
    void *user_data)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_L3_UNIT_INIT_CHECK;

    DPP_L3_LOCK_TAKE;

    DPP_L3_LOCK_RELEASE;

    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_l3_ingress_traverse is not available"))); 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_host_find
 * Purpose:
 *      Look up an L3 host table entry based on IP address.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN/OUT) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_host_find(
    int unit, 
    bcm_l3_host_t *info)
{
    int status = BCM_E_NONE;
    bcm_l3_route_t r_info;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_L3_UNIT_INIT_CHECK;

    DPP_L3_LOCK_TAKE;

    BCMDNX_NULL_CHECK(info);

    if (info->l3a_flags & BCM_L3_IP6) {
        if ((SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length_ipv6_long)) {
            if (SOC_IS_JERICHO(unit)) {
                status = _bcm_ppd_frwrd_ipv6_host_get(unit, info);
                if (status != BCM_E_NONE) {
                    BCMDNX_IF_ERR_EXIT(status);
                }
            } else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_L3_FLAGS2_SCALE_ROUTE supported from Jericho and above.")));
            }
        } else {
            status = _bcm_ppd_frwrd_ipv6_host_to_route(unit, info, &r_info);
            BCMDNX_IF_ERR_EXIT(status);    

            status = _bcm_ppd_frwrd_ipv6_route_get(unit, &r_info, &info->encap_id);
            BCMDNX_IF_ERR_EXIT(status);    

            info->l3a_flags = r_info.l3a_flags;
            info->l3a_intf = r_info.l3a_intf;
        }
    }
    else {
        status = _bcm_ppd_frwrd_ipv4_host_get(unit, info);
        BCMDNX_IF_ERR_EXIT(status);    
    }

exit:
    DPP_L3_LOCK_RELEASE;
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_host_add
 * Purpose:
 *      Add an entry into the L3 switching table.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_host_add(
    int unit, 
    bcm_l3_host_t *info)
{
    int status = BCM_E_NONE;
    bcm_l3_route_t r_info;
    int update = 0;
    bcm_l3_host_t info_ori;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_L3_UNIT_INIT_CHECK;
    DPP_L3_LOCK_TAKE;

    BCMDNX_NULL_CHECK(info);

    if (info->l3a_flags & ~L3_ROUTE_SUPPORTED_FLAGS) {
        LOG_ERROR(BSL_LS_BCM_L3,
                  (BSL_META_U(unit,
                              "flags %x is not supported\n"), info->l3a_flags));
        BCM_RETURN_VAL_EXIT(BCM_E_PARAM);
    }

    /* Read the state of the flags */
    update = (info->l3a_flags & BCM_L3_REPLACE) ? TRUE : FALSE;

    if (update) {
        /* Copy provided structure to local so it can be modified. */
        sal_memcpy(&info_ori, info, sizeof(bcm_l3_host_t));

        /* Check if host entry already exists. */
        status = bcm_petra_l3_host_find(unit, &info_ori);
        if (status != BCM_E_NONE) {
            BCMDNX_IF_ERR_EXIT(status);
        }
    }

    if (info->l3a_flags & BCM_L3_IP6) {
        if ((SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length_ipv6_long)) {
            if (SOC_IS_JERICHO(unit)) {
                status = _bcm_ppd_frwrd_ipv6_host_add(unit, info);
                if (status != BCM_E_NONE) {
                    BCMDNX_IF_ERR_EXIT(status);
                }
            } else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_L3_FLAGS2_SCALE_ROUTE supported from Jericho and above.")));
            }
        } else {
            status = _bcm_ppd_frwrd_ipv6_host_to_route(unit, info, &r_info);
            if (status != BCM_E_NONE) {
                BCMDNX_IF_ERR_EXIT(status);
            }  

            status = _bcm_ppd_frwrd_ipv6_route_add(unit, &r_info, TRUE /* host */, info->encap_id);
            if (status != BCM_E_NONE) {
                BCMDNX_IF_ERR_EXIT(status);
            }   
        }
    }
    else {
        status = _bcm_ppd_frwrd_ipv4_host_add(unit, info);
        if (status != BCM_E_NONE) {
            BCMDNX_IF_ERR_EXIT(status);
        }   
    }

    BCMDNX_IF_ERR_EXIT(status);    
exit:
    DPP_L3_LOCK_RELEASE;
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_host_delete
 * Purpose:
 *      Delete an entry from the L3 host table.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_host_delete(
    int unit, 
    bcm_l3_host_t *info)
{
    int status = BCM_E_NONE;
    bcm_l3_route_t r_info;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_L3_UNIT_INIT_CHECK;

    DPP_L3_LOCK_TAKE;

    BCMDNX_NULL_CHECK(info);

    if (info->l3a_flags & BCM_L3_IP6) {
#if defined(INCLUDE_KBP)
        if(ARAD_KBP_ENABLE_ANY_IPV6UC_PROGRAM && ARAD_KBP_24BIT_FWD) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_l3_host_delete is not supported for 24bit FWD; Use bcm_l3_route_delete instead")));
        }
#endif
        if (SOC_DPP_CONFIG(unit)->pp.enhanced_fib_scale_prefix_length_ipv6_long) {
            if (SOC_IS_JERICHO(unit)) {
                /* Check if there is a valid entry before removing it */
                status = _bcm_ppd_frwrd_ipv6_host_get(unit, info);

                if (status == BCM_E_NONE) {
                    status = _bcm_ppd_frwrd_ipv6_host_delete(unit, info);
                }
            } else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_L3_FLAGS2_SCALE_ROUTE supported from Jericho and above.")));
            }
        } else {
            status = _bcm_ppd_frwrd_ipv6_host_to_route(unit, info, &r_info);
            BCMDNX_IF_ERR_EXIT(status);    

            status = _bcm_ppd_frwrd_ipv6_route_delete(unit, &r_info);
            BCMDNX_IF_ERR_EXIT(status);    
        }
    }
    else {
#if defined(INCLUDE_KBP)
        if(ARAD_KBP_ENABLE_ANY_IPV4UC_PROGRAM && ARAD_KBP_24BIT_FWD && !(info->l3a_flags & BCM_L3_HOST_LOCAL)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_l3_host_delete is not supported for 24bit FWD; Use bcm_l3_route_delete instead")));
        }

        if(ARAD_KBP_ENABLE_IPV4_UC_PUBLIC && (info->l3a_vrf == 0) && (info->l3a_flags & BCM_L3_HOST_LOCAL)){
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("bcm_l3_host_delete is not supported to delete public entry from the LEM when public KBP enabled")));
        }
#endif
        /* Check if there is a valid entry before removing it */
        status = _bcm_ppd_frwrd_ipv4_host_get(unit, info);

        if (status == BCM_E_NONE) {
          status = _bcm_ppd_frwrd_ipv4_host_remove(unit, info);
        }
    }

    BCMDNX_IF_ERR_EXIT(status);
exit:
    DPP_L3_LOCK_RELEASE;
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_host_delete_by_network
 * Purpose:
 *      Delete L3 entries based on IP prefix (network).
 * Parameters:
 *      unit - (IN) Unit number.
 *      ip_addr - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_host_delete_by_network(
    int unit, 
    bcm_l3_route_t *ip_addr)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_l3_host_delete_by_network is not available")));
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_host_delete_by_interface
 * Purpose:
 *      Deletes L3 entries that match or do not match a specified L3
 *      interface number.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_host_delete_by_interface(
    int unit, 
    bcm_l3_host_t *info)
{
    int status = BCM_E_NONE;
    uint32 mode;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_L3_UNIT_INIT_CHECK;

    BCMDNX_NULL_CHECK(info);

    if (info->l3a_flags & BCM_L3_IP6) {
        LOG_VERBOSE(BSL_LS_BCM_L3,
                    (BSL_META_U(unit,
                                "IPv6: flags %x\n"), info->l3a_flags));
        BCM_RETURN_VAL_EXIT(BCM_E_UNAVAIL);
    }

    DPP_L3_LOCK_TAKE;

    mode = DPP_L3_TRAVERSE_DELETE_INTERFACE;

    status = _bcm_ppd_frwrd_ipv4_host_get_block(unit, mode, info->l3a_flags, info->l3a_intf, 0, 0);

    DPP_L3_LOCK_RELEASE;

    BCMDNX_IF_ERR_EXIT(status);    
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_host_delete_all
 * Purpose:
 *      Deletes all L3 host table entries.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_host_delete_all(
    int unit, 
    bcm_l3_host_t *info)
{
    uint32 soc_sand_rv;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_L3_UNIT_INIT_CHECK;

    DPP_L3_LOCK_TAKE;

    BCMDNX_NULL_CHECK(info);

    if (info->l3a_flags & BCM_L3_IP6) {
        LOG_VERBOSE(BSL_LS_BCM_L3,
                    (BSL_META_U(unit,
                                "IPv6: flags %x\n"), info->l3a_flags));
        BCM_RETURN_VAL_EXIT(BCM_E_UNAVAIL);
    }

#ifdef PLISIM
    /* Host get block not supported in PCID */
    soc_sand_rv = soc_ppd_frwrd_ipv4_vrf_all_routing_tables_clear(
       unit,
       SOC_PPC_FRWRD_IP_HOST_ONLY
       );
#else
    /* Clear entries in LEM and Shadow */
    soc_sand_rv = _bcm_ppd_frwrd_ipv4_host_get_block(unit, DPP_L3_TRAVERSE_DELETE_ALL, 0, 0, NULL, NULL);
#endif
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv); 

exit:
    DPP_L3_LOCK_RELEASE;
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_host_conflict_get
 * Purpose:
 *      Return list of conflicts in the L3 table.
 * Parameters:
 *      unit - (IN) Unit number.
 *      ipkey - (IN) <UNDEF>
 *      cf_array - (OUT) <UNDEF>
 *      cf_max - (IN) <UNDEF>
 *      cf_count - (OUT) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 *      Not Supported
 */
int 
bcm_petra_l3_host_conflict_get(
    int unit, 
    bcm_l3_key_t *ipkey, 
    bcm_l3_key_t *cf_array, 
    int cf_max, 
    int *cf_count)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_l3_host_conflict_get is not available"))); 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_host_age
 * Purpose:
 *      Run L3 table aging.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN) <UNDEF>
 *      age_cb - (IN) <UNDEF>
 *      user_data - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 *      Not Supported - phase 1
 * Notes:
 */
int 
bcm_petra_l3_host_age(
    int unit, 
    uint32 flags, 
    bcm_l3_host_traverse_cb age_cb, 
    void *user_data)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_l3_host_age is not available"))); 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_host_traverse
 * Purpose:
 *      Traverse through the L3 table and run callback at each valid
 *      L3 entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN) <UNDEF>
 *      start - (IN) <UNDEF>
 *      end - (IN) <UNDEF>
 *      cb - (IN) <UNDEF>
 *      user_data - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_host_traverse(
    int unit, 
    uint32 flags, 
    uint32 start, 
    uint32 end, 
    bcm_l3_host_traverse_cb cb, 
    void *user_data)
{
    int status = BCM_E_NONE;
    uint32 mode;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_L3_UNIT_INIT_CHECK;

    DPP_L3_LOCK_TAKE;

    mode = DPP_L3_TRAVERSE_CB;

#if defined(INCLUDE_KBP)
    if ((flags & BCM_L3_IP6)) {
        status = _bcm_ppd_frwrd_ipv6_host_traverse(unit, flags, cb, user_data);
    } else if (SOC_IS_JERICHO(unit) && !(ARAD_KBP_ENABLE_IPV4_MPLS_EXTENDED)) {
        status = _bcm_ppd_frwrd_ipv4_host_traverse(unit, flags, cb, user_data);
    } else 
#endif /*defined(INCLUDE_KBP)*/
    {
        status = _bcm_ppd_frwrd_ipv4_host_get_block(unit, mode, flags, 0, cb, user_data);
    }

    DPP_L3_LOCK_RELEASE;

    BCMDNX_IF_ERR_EXIT(status);    
exit:
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_host_invalidate_entry
 * Purpose:
 *      bcm_petra_l3_host_invalidate_entry
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 *      Not Supported
 */
int 
bcm_petra_l3_host_invalidate_entry(
    int unit, 
    bcm_ip_t info)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_l3_host_invalidate_entry is not available"))); 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_host_validate_entry
 * Purpose:
 *      bcm_petra_l3_host_validate_entry
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 *      Not Supported
 */
int 
bcm_petra_l3_host_validate_entry(
    int unit, 
    bcm_ip_t info)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_l3_host_validate_entry is not available"))); 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_route_add
 * Purpose:
 *      Add an IP route to the L3 route table.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_route_add(
    int unit, 
    bcm_l3_route_t *info)

{
    int status = BCM_E_NONE;
    int update = 0;
    bcm_l3_route_t info_ori;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_L3_UNIT_INIT_CHECK;
    DPP_L3_LOCK_TAKE;

    BCMDNX_NULL_CHECK(info);

    if (info->l3a_flags & ~L3_ROUTE_SUPPORTED_FLAGS) {
        LOG_ERROR(BSL_LS_BCM_L3,
                  (BSL_META_U(unit,
                              "flags %x is not supported\n"), info->l3a_flags));
        BCM_RETURN_VAL_EXIT(BCM_E_PARAM);
    }

#if defined(INCLUDE_KBP)
    if( (SOC_DPP_CONFIG(unit)->arad->init.elk.fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_LSR_IP_SHARED]) || (SOC_DPP_CONFIG(unit)->arad->init.elk.fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_EXTENDED_IPV6]) ) {
         if(!(info->l3a_flags & BCM_L3_RPF))
         {
             BCM_RETURN_VAL_EXIT(BCM_E_UNAVAIL);
         }
    }
#endif 
    /* Read the state of the flags */
    update = (info->l3a_flags & BCM_L3_REPLACE) ? TRUE : FALSE;

    if (update) {
        /* Copy provided structure to local so it can be modified. */
        sal_memcpy(&info_ori, info, sizeof(bcm_l3_route_t));

        /* Check if route entry already exists. */
        status = bcm_petra_l3_route_get(unit, &info_ori);
        BCMDNX_IF_ERR_EXIT(status);
    }

    if (info->l3a_flags & BCM_L3_IP6) {
        status = _bcm_ppd_frwrd_ipv6_route_add(unit, info, FALSE /* route, not host */, 0 /* encap_id not relevant*/);
        BCMDNX_IF_ERR_EXIT(status);    
    } else {
        /* this is default route setting */
        if((info->l3a_ip_mask == 0) && (SOC_IS_ARADPLUS_AND_BELOW(unit))) {
            /* In Jericho, no special treatment to the default route */
            status = _bcm_ppd_frwrd_ipv4_default_dest_set(unit,info);
        }
        else if ((info->l3a_vrf == DPP_DEFAULT_VRF) && (SOC_IS_ARADPLUS_AND_BELOW(unit))) {
            status = _bcm_ppd_frwrd_ipv4_uc_route_add(unit, info);
        } else {
            status = _bcm_ppd_frwrd_ipv4_vrf_uc_route_add(unit, info);
        }
    }

    BCMDNX_IF_ERR_EXIT(status);    
exit:
    DPP_L3_LOCK_RELEASE;
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_route_delete
 * Purpose:
 *      Delete an IP route from the DEFIP table.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_route_delete(
    int unit, 
    bcm_l3_route_t *info)
{
    int status = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_L3_UNIT_INIT_CHECK;

    DPP_L3_LOCK_TAKE;

    BCMDNX_NULL_CHECK(info);

    /* if working with ip_mpls extended, need to delete the entry only if the RPF flag is enbaled */
#if defined(INCLUDE_KBP)
    if(SOC_DPP_CONFIG(unit)->arad->init.elk.fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_LSR_IP_SHARED]) {
       if(!(info->l3a_flags & BCM_L3_RPF))
       {
           BCM_RETURN_VAL_EXIT(BCM_E_UNAVAIL);
       }
    }
#endif 

    /* Check if there is a valid entry before removing it */

    if (info->l3a_flags & BCM_L3_IP6) {
        status = _bcm_ppd_frwrd_ipv6_route_delete(unit, info);
        BCMDNX_IF_ERR_EXIT(status);    
    } else {
        /* check this is not default route, this restriction does not apply for Jericho */
        if((SOC_IS_ARADPLUS_AND_BELOW(unit)) && (info->l3a_ip_mask == 0)) {
            LOG_ERROR(BSL_LS_BCM_L3,
                      (BSL_META_U(unit,
                                  "cannot remove default destination info->l3a_ip_mask \n")));
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("cannot remove default destination")));
        }
    if ((info->l3a_vrf == DPP_DEFAULT_VRF) && (SOC_IS_ARADPLUS_AND_BELOW(unit))) {
        status = _bcm_ppd_frwrd_ipv4_uc_route_get(unit, info);
        if (status == BCM_E_NONE) {
            status = _bcm_ppd_frwrd_ipv4_uc_route_remove(unit, info);
        }
    } else {
        status = _bcm_ppd_frwrd_ipv4_vrf_uc_route_get(unit, info);
        if (status == BCM_E_NONE) {
            status = _bcm_ppd_frwrd_ipv4_vrf_uc_route_remove(unit, info);
            }
        }
    }

    BCMDNX_IF_ERR_EXIT(status);    
exit:
    DPP_L3_LOCK_RELEASE;
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_route_delete_by_interface
 * Purpose:
 *      Delete routes based on matching or non-matching L3 interface
 *      number.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_route_delete_by_interface(
    int unit, 
    bcm_l3_route_t *info)
{
    int status = BCM_E_NONE;
    uint32 mode;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_L3_UNIT_INIT_CHECK;

    DPP_L3_LOCK_TAKE;

    BCMDNX_NULL_CHECK(info);

    mode = DPP_L3_TRAVERSE_DELETE_INTERFACE;

    if (info->l3a_flags & BCM_L3_IP6) {
        status = _bcm_ppd_frwrd_ipv6_route_get_block(unit, mode, info->l3a_intf, 0, 0);
    } else {
        status = _bcm_ppd_frwrd_ipv4_route_get_block(unit, mode, info->l3a_intf, 0, 0);
    }

    DPP_L3_LOCK_RELEASE;

    BCMDNX_IF_ERR_EXIT(status);    
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_route_delete_all
 * Purpose:
 *      Delete all routes.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_route_delete_all(
    int unit, 
    bcm_l3_route_t *info)
{
    int status = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_L3_UNIT_INIT_CHECK;

    DPP_L3_LOCK_TAKE;

    if (info == NULL) {
        /* clear both V4 & V6 */
        status = _bcm_ppd_frwrd_ipv4_routing_table_clear(unit);
        if (BCM_SUCCESS(status)) {
            status = _bcm_ppd_frwrd_ipv6_routing_table_clear(unit);
        }
    } else if (info->l3a_flags & BCM_L3_IP6) {
      status = _bcm_ppd_frwrd_ipv6_routing_table_clear(unit);
    } else {
      status = _bcm_ppd_frwrd_ipv4_routing_table_clear(unit);
    }

    DPP_L3_LOCK_RELEASE;

    BCMDNX_IF_ERR_EXIT(status);    
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_route_get
 * Purpose:
 *      Look up a route given the network and netmask.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN/OUT) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_route_get(
    int unit, 
    bcm_l3_route_t *info)
{
    int status = BCM_E_NONE;
    bcm_if_t encap_id; /* Not needed, only for host */

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_L3_UNIT_INIT_CHECK;

    DPP_L3_LOCK_TAKE;

    BCMDNX_NULL_CHECK(info);

    if (info->l3a_flags & BCM_L3_IP6) {
        status = _bcm_ppd_frwrd_ipv6_route_get(unit, info, &encap_id);
    } else {
        if((info->l3a_ip_mask == 0) && (SOC_IS_ARADPLUS_AND_BELOW(unit))) {/* this is default route setting */
            status = _bcm_ppd_frwrd_ipv4_default_dest_get(unit,info);
        }
        else if ((info->l3a_vrf == DPP_DEFAULT_VRF)  && (SOC_IS_ARADPLUS_AND_BELOW(unit))) {
            status = _bcm_ppd_frwrd_ipv4_uc_route_get(unit, info);
        } else {
            status = _bcm_ppd_frwrd_ipv4_vrf_uc_route_get(unit, info);
        }
    }
    BCM_RETURN_VAL_EXIT(status);
    
exit:
    DPP_L3_LOCK_RELEASE;
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_route_multipath_get
 * Purpose:
 *      Given a network, return all the paths for this route.
 * Parameters:
 *      unit - (IN) Unit number.
 *      the_route - (IN) <UNDEF>
 *      path_array - (OUT) <UNDEF>
 *      max_path - (IN) <UNDEF>
 *      path_count - (OUT) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_route_multipath_get(
    int unit, 
    bcm_l3_route_t *info, 
    bcm_l3_route_t *path_array, 
    int max_path, 
    int *path_count)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_L3_UNIT_INIT_CHECK;

    BCMDNX_NULL_CHECK(path_count);
    BCMDNX_NULL_CHECK(info);
    BCMDNX_NULL_CHECK(path_array);
    if (max_path <= 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("max_path must be positive")));
    }

    if (info->l3a_flags & BCM_L3_IP6) {
        LOG_VERBOSE(BSL_LS_BCM_L3,
                    (BSL_META_U(unit,
                                "IPv6: flags %x\n"), info->l3a_flags));
        BCM_RETURN_VAL_EXIT(BCM_E_UNAVAIL);
    }

    DPP_L3_LOCK_TAKE;

    DPP_L3_LOCK_RELEASE;

    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_l3_route_multipath_get is not available"))); 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_route_age
 * Purpose:
 *      Age the route table.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN) <UNDEF>
 *      age_out - (IN) <UNDEF>
 *      user_data - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 *      Not Supported - phase 1
 */
int 
bcm_petra_l3_route_age(
    int unit, 
    uint32 flags, 
    bcm_l3_route_traverse_cb age_out, 
    void *user_data)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_l3_route_age is not available"))); 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_route_traverse
 * Purpose:
 *      Traverse through the routing table and run callback at each
 *      route.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN) <UNDEF>
 *      start - (IN) <UNDEF>
 *      end - (IN) <UNDEF>
 *      trav_fn - (IN) <UNDEF>
 *      user_data - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_route_traverse(
    int unit, 
    uint32 flags, 
    uint32 start, 
    uint32 end, 
    bcm_l3_route_traverse_cb trav_fn, 
    void *user_data)
{
    int status = BCM_E_NONE;
    uint32 mode;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_L3_UNIT_INIT_CHECK;

    DPP_L3_LOCK_TAKE;

    mode = DPP_L3_TRAVERSE_CB;

    if (flags & BCM_L3_IP6) {
#if defined(INCLUDE_KBP)
        if (SOC_IS_JERICHO(unit) && !(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "l3_ipv6_uc_use_tcam", 0))) {
            status = _bcm_ppd_frwrd_ipv6_route_traverse(unit, flags, trav_fn, user_data);
        } else 
#endif /*defined(INCLUDE_KBP)*/
        {
            status = _bcm_ppd_frwrd_ipv6_route_get_block(unit, mode, 0, trav_fn, user_data);
        }
    } else {
#if defined(INCLUDE_KBP)
		if (SOC_IS_JERICHO(unit) && !(ARAD_KBP_ENABLE_IPV4_MPLS_EXTENDED)) {
            status = _bcm_ppd_frwrd_ipv4_route_traverse(unit, flags, trav_fn, user_data);
        } else 
#endif /*defined(INCLUDE_KBP)*/
        {
            status = _bcm_ppd_frwrd_ipv4_route_get_block(unit, mode, 0, trav_fn, user_data);
        }
    }

    DPP_L3_LOCK_RELEASE;

    BCMDNX_IF_ERR_EXIT(status);    
exit:
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      bcm_petra_l3_route_max_ecmp_set
 * Purpose:
 *      Set the maximum ECMP paths allowed for a route.
 * Parameters:
 *      unit - (IN) Unit number.
 *      max - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_route_max_ecmp_set(
    int unit, 
    int max)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_L3_UNIT_INIT_CHECK;

    DPP_L3_LOCK_TAKE;

    DPP_L3_LOCK_RELEASE;

    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_l3_route_max_ecmp_set is not available"))); 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_route_max_ecmp_get
 * Purpose:
 *      Get the maximum ECMP paths allowed for a route.
 * Parameters:
 *      unit - (IN) Unit number.
 *      max - (OUT) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_route_max_ecmp_get(
    int unit, 
    int *max)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_L3_UNIT_INIT_CHECK;

    BCMDNX_NULL_CHECK(max);

    DPP_L3_LOCK_TAKE;

    DPP_L3_LOCK_RELEASE;

    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_l3_route_max_ecmp_get is not available"))); 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_conflict_get
 * Purpose:
 *      Return list of conflicts in the L3 table.
 * Parameters:
 *      unit - (IN) Unit number.
 *      ipkey - (IN) <UNDEF>
 *      cf_array - (OUT) <UNDEF>
 *      cf_max - (IN) <UNDEF>
 *      cf_count - (OUT) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 *      Not Supported
 */
int 
bcm_petra_l3_conflict_get(
    int unit, 
    bcm_l3_key_t *ipkey, 
    bcm_l3_key_t *cf_array, 
    int cf_max, 
    int *cf_count)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_l3_conflict_get is not available"))); 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_invalidate_entry
 * Purpose:
 *      Invalidate L3 entry without clearing so it can be re-validated
 *      later.
 * Parameters:
 *      unit - (IN) Unit number.
 *      l3_addr - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 *      Not Supported
 */
int 
bcm_petra_l3_invalidate_entry(
    int unit, 
    bcm_ip_t l3_addr)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_l3_invalidate_entry is not available"))); 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_validate_entry
 * Purpose:
 *      Invalidate L3 entry without clearing so it can be re-validated
 *      later.
 * Parameters:
 *      unit - (IN) Unit number.
 *      l3_addr - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 *      Not Supported
 */
int 
bcm_petra_l3_validate_entry(
    int unit, 
    bcm_ip_t l3_addr)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_l3_validate_entry is not available"))); 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_age
 * Purpose:
 *      Run L3 table aging.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN) <UNDEF>
 *      age_out - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 *      Not Supported
 */
int 
bcm_petra_l3_age(
    int unit, 
    uint32 flags, 
    bcm_l3_age_cb age_out)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_l3_age is not available"))); 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_status
 * Purpose:
 *      Return L3 status information.
 * Parameters:
 *      unit - (IN) Unit number.
 *      free_l3intf - (OUT) <UNDEF>
 *      free_l3 - (OUT) <UNDEF>
 *      free_defip - (OUT) <UNDEF>
 *      free_lpm_blk - (OUT) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 *      Not Supported
 */
int 
bcm_petra_l3_status(
    int unit, 
    int *free_l3intf, 
    int *free_l3, 
    int *free_defip, 
    int *free_lpm_blk)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_l3_status is not available"))); 
exit:
    BCMDNX_FUNC_RETURN;
}


#ifdef DPP_L3_STAT_SUPPORT
/*
 * Function:
 *      bcm_petra_l3_egress_stat_get
 * Purpose:
 *      Extract L3 egress statistics from the chip.
 * Parameters:
 *      unit - (IN) Unit number.
 *      egr - (IN) Pointer to an egress L3 object.
 *      stat - (IN) Type of the counter to retrieve.
 *      val - (OUT) Pointer to a counter value.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_egress_stat_get(
    int unit, 
    bcm_l3_egress_t *egr, 
    bcm_l3_stat_t stat, 
    uint64 *val)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_l3_egress_stat_get is not available"))); 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_egress_stat_get32
 * Purpose:
 *      Extract L3 egress statistics from the chip.
 * Parameters:
 *      unit - (IN) Unit number.
 *      egr - (IN) Pointer to an egress L3 object.
 *      stat - (IN) Type of the counter to retrieve.
 *      val - (OUT) Pointer to a counter value.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_egress_stat_get32(
    int unit, 
    bcm_l3_egress_t *egr, 
    bcm_l3_stat_t stat, 
    uint32 *val)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_l3_egress_stat_get32 is not available"))); 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_egress_stat_set
 * Purpose:
 *      Set the specified L3 statistic to the indicated value for the
 *      specified L3 egress.
 * Parameters:
 *      unit - (IN) Unit number.
 *      egr - (IN) Pointer to an egress L3 object.
 *      stat - (IN) Type of the counter to set.
 *      val - (IN) New counter value.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_egress_stat_set(
    int unit, 
    bcm_l3_egress_t *egr, 
    bcm_l3_stat_t stat, 
    uint64 val)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_l3_egress_stat_set is not available"))); 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_egress_stat_set32
 * Purpose:
 *      Set the specified L3 statistic to the indicated value for the
 *      specified L3 egress.
 * Parameters:
 *      unit - (IN) Unit number.
 *      egr - (IN) Pointer to an egress L3 object.
 *      stat - (IN) Type of the counter to set.
 *      val - (IN) New counter value.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_egress_stat_set32(
    int unit, 
    bcm_l3_egress_t *egr, 
    bcm_l3_stat_t stat, 
    uint32 val)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_l3_egress_stat_set32 is not available"))); 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_egress_stat_enable_set
 * Purpose:
 *      Enable/disable collection of statistics on the indicated L3
 *      egress.
 * Parameters:
 *      unit - (IN) Unit number.
 *      egr - (IN) Pointer to an egress L3 object.
 *      enable - (IN) Non-zero to enable counter collection, zero to disable.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_egress_stat_enable_set(
    int unit, 
    bcm_l3_egress_t *egr, 
    int enable)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_l3_egress_stat_enable_set is not available"))); 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_host_stat_get
 * Purpose:
 *      Extract L3 host statistics from the chip.
 * Parameters:
 *      unit - (IN) Unit number.
 *      host - (IN) Pointer to an L3 host entry.
 *      stat - (IN) Type of the counter to retrieve.
 *      val - (OUT) Pointer to a counter value.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_host_stat_get(
    int unit, 
    bcm_l3_host_t *host, 
    bcm_l3_stat_t stat, 
    uint64 *val)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_l3_host_stat_get is not available"))); 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_host_stat_get32
 * Purpose:
 *      Extract L3 host statistics from the chip.
 * Parameters:
 *      unit - (IN) Unit number.
 *      host - (IN) Pointer to an L3 host entry.
 *      stat - (IN) Type of the counter to retrieve.
 *      val - (OUT) Pointer to a counter value.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_host_stat_get32(
    int unit, 
    bcm_l3_host_t *host, 
    bcm_l3_stat_t stat, 
    uint32 *val)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_l3_host_stat_get32 is npot available"))); 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_host_stat_set
 * Purpose:
 *      Set the specified L3 statistic to the indicated value for the
 *      specified L3 host.
 * Parameters:
 *      unit - (IN) Unit number.
 *      host - (IN) Pointer to an L3 host entry.
 *      stat - (IN) Type of the counter to set.
 *      val - (IN) New counter value.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_host_stat_set(
    int unit, 
    bcm_l3_host_t *host, 
    bcm_l3_stat_t stat, 
    uint64 val)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_l3_host_stat_set is not available"))); 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_host_stat_set32
 * Purpose:
 *      Set the specified L3 statistic to the indicated value for the
 *      specified L3 host.
 * Parameters:
 *      unit - (IN) Unit number.
 *      host - (IN) Pointer to an L3 host entry.
 *      stat - (IN) Type of the counter to set.
 *      val - (IN) New counter value.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_host_stat_set32(
    int unit, 
    bcm_l3_host_t *host, 
    bcm_l3_stat_t stat, 
    uint32 val)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_l3_host_stat_set32 is not available"))); 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_host_stat_enable_set
 * Purpose:
 *      Enable/disable collection of statistics for the indicated L3
 *      host.
 * Parameters:
 *      unit - (IN) Unit number.
 *      host - (IN) Pointer to an L3 host entry.
 *      enable - (IN) Non-zero to enable counter collection, zero to disable.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_host_stat_enable_set(
    int unit, 
    bcm_l3_host_t *host, 
    int enable)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_l3_host_stat_enable_set is not available"))); 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_route_stat_get
 * Purpose:
 *       * Parameters:
 *      unit - (IN) Unit number.
 *      route - (IN) Pointer to an L3 route.
 *      stat - (IN) Type of the counter to retrieve.
 *      val - (OUT) Pointer to a counter value.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_route_stat_get(
    int unit, 
    bcm_l3_route_t *route, 
    bcm_l3_stat_t stat, 
    uint64 *val)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_l3_route_stat_get is not available"))); 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_route_stat_get32
 * Purpose:
 *       * Parameters:
 *      unit - (IN) Unit number.
 *      route - (IN) Pointer to an L3 route.
 *      stat - (IN) Type of the counter to retrieve.
 *      val - (OUT) Pointer to a counter value.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_route_stat_get32(
    int unit, 
    bcm_l3_route_t *route, 
    bcm_l3_stat_t stat, 
    uint32 *val)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_l3_route_stat_get32 is not available"))); 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_route_stat_set
 * Purpose:
 *       * Parameters:
 *      unit - (IN) Unit number.
 *      route - (IN) Pointer to an L3 route.
 *      stat - (IN) Type of the counter to set.
 *      val - (IN) New counter value.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_route_stat_set(
    int unit, 
    bcm_l3_route_t *route, 
    bcm_l3_stat_t stat, 
    uint64 val)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_l3_route_stat_set is not available"))); 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_route_stat_set32
 * Purpose:
 *       * Parameters:
 *      unit - (IN) Unit number.
 *      route - (IN) Pointer to an L3 route.
 *      stat - (IN) Type of the counter to set.
 *      val - (IN) New counter value.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_route_stat_set32(
    int unit, 
    bcm_l3_route_t *route, 
    bcm_l3_stat_t stat, 
    uint32 val)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_l3_route_stat_set32 is not available"))); 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_route_stat_enable_set
 * Purpose:
 *      Enable/disable collection of statistics for the indicated L3
 *      route.
 * Parameters:
 *      unit - (IN) Unit number.
 *      route - (IN) Pointer to an L3 route object.
 *      enable - (IN) Non-zero to enable counter collection, zero to disable.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_route_stat_enable_set(
    int unit, 
    bcm_l3_route_t *route, 
    int enable)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_l3_route_stat_enable_set is not available"))); 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_vrf_stat_enable_set
 * Purpose:
 *      Enable/disable packet and byte counters for the selected VRF.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vrf - (IN) Virtual router instance.
 *      enable - (IN) Non-zero to enable counter collection, zero to disable.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_vrf_stat_enable_set(
    int unit, 
    bcm_vrf_t vrf, 
    int enable)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_l3_vrf_stat_enable_set is not available"))); 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_vrf_stat_get
 * Purpose:
 *      Get 64-bit counter value for specified VRF statistic type.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vrf - (IN) Virtual router instance.
 *      stat - (IN) Type of the counter to retrieve.
 *      val - (OUT) Pointer to a counter value.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_vrf_stat_get(
    int unit, 
    bcm_vrf_t vrf, 
    bcm_l3_vrf_stat_t stat, 
    uint64 *val)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_l3_vrf_stat_get is not available"))); 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_vrf_stat_get32
 * Purpose:
 *      Get lower 32-bit counter value for specified VRF statistic
 *      type.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vrf - (IN) Virtual router instance.
 *      stat - (IN) Type of the counter to retrieve.
 *      val - (OUT) Pointer to a counter value.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_vrf_stat_get32(
    int unit, 
    bcm_vrf_t vrf, 
    bcm_l3_vrf_stat_t stat, 
    uint32 *val)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_l3_vrf_stat_get32 is not available"))); 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_vrf_stat_set
 * Purpose:
 *      Set 64-bit counter value for specified VRF statistic type.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vrf - (IN) Virtual router instance.
 *      stat - (IN) Type of the counter to retrieve.
 *      val - (IN) New counter value.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_vrf_stat_set(
    int unit, 
    bcm_vrf_t vrf, 
    bcm_l3_vrf_stat_t stat, 
    uint64 val)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_l3_vrf_stat_set is not available"))); 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_vrf_stat_set32
 * Purpose:
 *      Set lower 32-bit counter value for specified VRF statistic
 *      type.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vrf - (IN) Virtual router instance.
 *      stat - (IN) Type of the counter to retrieve.
 *      val - (IN) New counter value.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_vrf_stat_set32(
    int unit, 
    bcm_vrf_t vrf, 
    bcm_l3_vrf_stat_t stat, 
    uint32 val)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_l3_vrf_stat_set32 is not available"))); 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_vrf_stat_multi_get
 * Purpose:
 *      Get 64-bit counter value for multiple VRF statistic types.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vrf - (IN) Virtual router instance.
 *      nstat - (IN) Number of elements in stat array
 *      stat_arr - (IN) Collected statistics descriptors array
 *      value_arr - (OUT) Collected counters values
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_vrf_stat_multi_get(
    int unit, 
    bcm_vrf_t vrf, 
    int nstat, 
    bcm_l3_vrf_stat_t *stat_arr, 
    uint64 *value_arr)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_l3_vrf_stat_multi_get is not available"))); 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_vrf_stat_multi_get32
 * Purpose:
 *      Get lower 32-bit counter value for multiple VRF statistic
 *      types.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vrf - (IN) Virtual router instance.
 *      nstat - (IN) Number of elements in stat array
 *      stat_arr - (IN) Collected statistics descriptors array
 *      value_arr - (OUT) Collected counters values
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_vrf_stat_multi_get32(
    int unit, 
    bcm_vrf_t vrf, 
    int nstat, 
    bcm_l3_vrf_stat_t *stat_arr, 
    uint32 *value_arr)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_l3_vrf_stat_multi_get32 is not available"))); 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_vrf_stat_multi_set
 * Purpose:
 *      Set 64-bit counter value for multiple VRF statistic types.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vrf - (IN) Virtual router instance.
 *      nstat - (IN) Number of elements in stat array
 *      stat_arr - (IN) Collected statistics descriptors array
 *      value_arr - (IN) Collected counters values
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_vrf_stat_multi_set(
    int unit, 
    bcm_vrf_t vrf, 
    int nstat, 
    bcm_l3_vrf_stat_t *stat_arr, 
    uint64 *value_arr)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_l3_vrf_stat_multi_set is not available"))); 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_vrf_stat_multi_set32
 * Purpose:
 *      Set lower 32-bit counter value for multiple VRF statistic
 *      types.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vrf - (IN) Virtual router instance.
 *      nstat - (IN) Number of elements in stat array
 *      stat_arr - (IN) Collected statistics descriptors array
 *      value_arr - (IN) Collected counters values
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_vrf_stat_multi_set32(
    int unit, 
    bcm_vrf_t vrf, 
    int nstat, 
    bcm_l3_vrf_stat_t *stat_arr, 
    uint32 *value_arr)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_l3_vrf_stat_multi_set32 is not available"))); 
exit:
    BCMDNX_FUNC_RETURN;
}

#endif /* DPP_L3_STAT_SUPPORT */

/*
 * Function:
 *      bcm_petra_l3_source_bind_enable_set
 * Purpose:
 *      Enable or disable l3 source binding checks on an ingress port.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Packet ingress port.
 *      enable - (IN) Non-zero to enable l3 source binding checks, zero to disable.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_source_bind_enable_set(
    int unit, 
    bcm_port_t port, 
    int enable)
{
    bcm_error_t rv = BCM_E_NONE;
    uint32 soc_sand_rv;    
    SOC_PPC_LIF_ENTRY_INFO *lif_info = NULL;
    int soc_sand_dev_id;
    _bcm_dpp_gport_hw_resources gport_hw_resources;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_L3_UNIT_INIT_CHECK;

    if (SOC_DPP_L3_SOURCE_BIND_DISABLE(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Can't call this api when l3 source binding function is disabled")));
    }

    rv = _bcm_dpp_gport_to_hw_resources(unit, port, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK, 
                                                 &gport_hw_resources);
    BCMDNX_IF_ERR_EXIT(rv);      

    soc_sand_dev_id = (unit);
    BCMDNX_ALLOC(lif_info, sizeof(SOC_PPC_LIF_ENTRY_INFO), "bcm_petra_l3_source_bind_enable_set.lif_info");
    if (lif_info == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
    }
    soc_sand_rv = soc_ppd_lif_table_entry_get(soc_sand_dev_id, gport_hw_resources.local_in_lif, lif_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if (lif_info->type != SOC_PPC_LIF_ENTRY_TYPE_AC) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid lif index.")));
    }

    if (enable == 1){
        /* Enable l3 source bind in AC */
        lif_info->value.ac.cos_profile |= SOC_PPC_LIF_AC_SPOOF_MODE_ENABLE_SET;
    }else {
        /* Disable l3 source bind in AC */
        lif_info->value.ac.cos_profile &= SOC_PPC_LIF_AC_SPOOF_MODE_DISABLE_SET;
    }
        
    /* update existing lif entry */
    soc_sand_rv = soc_ppd_lif_table_entry_update(soc_sand_dev_id, gport_hw_resources.local_in_lif, lif_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    BCM_FREE(lif_info);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_source_bind_enable_get
 * Purpose:
 *      Retrieve whether l3 source binding checks are performed on an
 *      ingress port.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Packet ingress port.
 *      enable - (OUT) Non-zero if l3 source binding checks are enabled, zero if disabled.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_source_bind_enable_get(
    int unit, 
    bcm_port_t port, 
    int *enable)
{
    bcm_error_t rv = BCM_E_NONE;
    uint32 soc_sand_rv;    
    SOC_PPC_LIF_ENTRY_INFO lif_info;
    int soc_sand_dev_id;
    _bcm_dpp_gport_hw_resources gport_hw_resources;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_L3_UNIT_INIT_CHECK;


    if (SOC_DPP_L3_SOURCE_BIND_DISABLE(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Can't call this api when l3 source binding function is disabled")));
    }

    rv = _bcm_dpp_gport_to_hw_resources(unit, port, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK, 
                                                 &gport_hw_resources);
    BCMDNX_IF_ERR_EXIT(rv);      

    soc_sand_dev_id = (unit);
    soc_sand_rv = soc_ppd_lif_table_entry_get(soc_sand_dev_id, gport_hw_resources.local_in_lif, &lif_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if (lif_info.type != SOC_PPC_LIF_ENTRY_TYPE_AC) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid lif index.")));
    }

    if (lif_info.value.ac.cos_profile & SOC_PPC_LIF_AC_SPOOF_MODE_ENABLE_SET) {
        *enable = 1;
    } else {
        *enable = 0;
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
  _bcm_petra_l3_source_bind_check(
    int unit, 
    bcm_l3_source_bind_t *info
  )
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(info);
    
    if (SOC_DPP_L3_SOURCE_BIND_DISABLE(unit) && (SOC_DPP_CONFIG(unit)->pp.compression_ip6_enable == 0)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Can't call this api when l3 source binding function and IPv6 compression are disabled")));
    }

    if (info->flags & BCM_L3_SOURCE_BIND_USE_MASK) {
        if ((info->flags & BCM_L3_SOURCE_BIND_IP6) == 0 && 
            SOC_DPP_CONFIG(unit)->pp.l3_source_bind_subnet_mode == SOC_DPP_L3_SOURCE_BIND_SUBNET_MODE_IPV6) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_ERR_MSG_MODULE_NAME, unit, _BCM_MSG("SOC configuration only support IPv6")));
        } else if ((info->flags & BCM_L3_SOURCE_BIND_IP6) == 1 && 
                   (SOC_DPP_CONFIG(unit)->pp.l3_source_bind_subnet_mode == SOC_DPP_L3_SOURCE_BIND_SUBNET_MODE_IPV4) &&
                   (SOC_DPP_CONFIG(unit)->pp.compression_ip6_enable == 0)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_ERR_MSG_MODULE_NAME, unit, _BCM_MSG("SOC configuration only support IPv4")));
        }
    } else {
        if ((info->flags & BCM_L3_SOURCE_BIND_IP6) == 0 && 
            SOC_DPP_CONFIG(unit)->pp.l3_source_bind_mode == SOC_DPP_L3_SOURCE_BIND_MODE_IPV6) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_ERR_MSG_MODULE_NAME, unit, _BCM_MSG("SOC configuration only support IPv6")));
        } else if ((info->flags & BCM_L3_SOURCE_BIND_IP6) == 1 && 
                   (SOC_DPP_CONFIG(unit)->pp.l3_source_bind_mode == SOC_DPP_L3_SOURCE_BIND_MODE_IPV4) &&
                   (SOC_DPP_CONFIG(unit)->pp.compression_ip6_enable == 0)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_ERR_MSG_MODULE_NAME, unit, _BCM_MSG("SOC configuration only support IPv4")));
        }
    }
    /* In case IPv4 anti-spoofing for subnet or ARP checking , SA must be zero.  */
    if (info->flags & BCM_L3_SOURCE_BIND_USE_MASK &&
         !BCM_MAC_IS_ZERO(info->mac)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_ERR_MSG_MODULE_NAME, unit, _BCM_MSG("SA must be zero when add a configuration for IP subnet")));
    }

    if(!SOC_DPP_PPPOE_IS_ENABLE(unit) && (info->flags & BCM_L3_SOURCE_BIND_PPPoE)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_ERR_MSG_MODULE_NAME, unit, _BCM_MSG("SOC configuration doesn't enable PPPoE")));
    }

    if (SOC_DPP_PPPOE_IS_ENABLE(unit) && 
        (info->flags & BCM_L3_SOURCE_BIND_PPPoE) && 
        (info->session_id > SOC_DPP_PPPOE_SESSION_ID_MAX)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_ERR_MSG_MODULE_NAME, unit, _BCM_MSG("PPPoE session ID must be [0-0xFFFF]")));
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
  _bcm_petra_l3_ip6_compression_to_ppd(
    int unit, 
    int lif_id,
    int global_lif_id,
    bcm_l3_source_bind_t *info,
    SOC_PPC_IPV6_COMPRESSED_ENTRY *ip6_compression_entry
  )
{
    bcm_error_t rv = BCM_E_NONE;
    _bcm_dpp_gport_info_t gport_info;
    uint32 soc_sand_rv;
    unsigned int soc_sand_dev_id;
    SOC_PPC_LIF_ENTRY_INFO lif_entry_info;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(info);
    BCMDNX_NULL_CHECK(ip6_compression_entry);

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, info->port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));
    soc_sand_dev_id = (unit);

    /* LIF_ID is valid, it indicate current entry is SAV and the info->port is AC */
    if (lif_id != _BCM_GPORT_ENCAP_ID_LIF_INVALID) {
        soc_sand_rv = soc_ppd_lif_table_entry_get(soc_sand_dev_id, lif_id, &lif_entry_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        ip6_compression_entry->vsi_id = lif_entry_info.value.ac.vsid;
        ip6_compression_entry->lif_ndx = global_lif_id;
    }

    ip6_compression_entry->ip6_tt_compressed_result = _BCM_L3_SOURCE_BIND_IP6_COMPRESSION_TT_RESULT_GET(info->rate_id);
    ip6_compression_entry->ip6_compressed_result = _BCM_L3_SOURCE_BIND_IP6_COMPRESSION_RESULT_GET(info->rate_id);

    /* SAV only in PON port */
    if (_BCM_L3_SOURCE_BIND_IP6_COMPRESSION_SAV(unit, gport_info.local_port)) {
        ip6_compression_entry->is_spoof = TRUE;
    } else {
        if (ip6_compression_entry->ip6_tt_compressed_result) {
            ip6_compression_entry->flags = SOC_PPC_IP6_COMPRESSION_DIP;
        } 
        else{
            ip6_compression_entry->flags = SOC_PPC_IP6_COMPRESSION_SIP;
        }
        ip6_compression_entry->is_spoof = FALSE;
    }
    if (info->flags & BCM_L3_SOURCE_BIND_USE_MASK) {
        ip6_compression_entry->ip6.prefix_len = bcm_ip6_mask_length(info->ip6_mask);
        if (ip6_compression_entry->ip6.prefix_len > SOC_SAND_PP_IPV6_ADDRESS_NOF_BITS/2) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_ERR_MSG_MODULE_NAME, unit, _BCM_MSG("Unsupported subnet address")));
        }
        /* The mask bits should be IP6[127:64] */
        ip6_compression_entry->ip6.prefix_len = (SOC_SAND_PP_IPV6_ADDRESS_NOF_BITS/2 - ip6_compression_entry->ip6.prefix_len);
    } else {
        ip6_compression_entry->ip6.prefix_len = SOC_SAND_PP_IPV6_ADDRESS_NOF_BITS;
    }

    if (_BCM_L3_IP6_COMPRESSION_TCAM_GET(info->flags)) {
        ip6_compression_entry->flags |= SOC_PPC_IP6_COMPRESSION_TCAM;
    }
    if (_BCM_L3_IP6_COMPRESSION_LEM_GET(info->flags)) {
        ip6_compression_entry->flags |= SOC_PPC_IP6_COMPRESSION_LEM;
    }

    rv = _bcm_l3_bcm_ipv6_addr_to_sand_ipv6_addr(unit, info->ip6, &(ip6_compression_entry->ip6.ipv6_address));
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      bcm_petra_l3_source_bind_add
 * Purpose:
 *      Add or replace an L3 source binding.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) L3 source binding information
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_source_bind_add(
    int unit, 
    bcm_l3_source_bind_t *info)
{
    bcm_error_t rv = BCM_E_NONE;    
    uint32 soc_sand_rv;
    SOC_SAND_SUCCESS_FAILURE success = SOC_SAND_SUCCESS;
    unsigned int soc_sand_dev_id;
    uint32 ret;
    SOC_PPC_LIF_ENTRY_INFO lif_entry_info;
    _bcm_dpp_gport_hw_resources gport_hw_resources;
    uint32 flags = _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS | _BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_L3_UNIT_INIT_CHECK;

    SOC_PPC_LIF_ENTRY_INFO_clear(&lif_entry_info);
    rv = _bcm_petra_l3_source_bind_check(unit, info);
    BCMDNX_IF_ERR_EXIT(rv);
    soc_sand_dev_id = (unit);

    /* The gport is forward group AC */
    if (BCM_GPORT_SUB_TYPE_IS_FORWARD_GROUP(BCM_GPORT_VLAN_PORT_ID_GET(info->port)) && 
	    SOC_DPP_L3_SOURCE_BIND_USE_FEC_LEARN_DATA(unit)) {
        flags |= _BCM_DPP_GPORT_HW_RESOURCES_FEC;
    }
    rv = _bcm_dpp_gport_to_hw_resources(unit, info->port, flags, &gport_hw_resources);
    BCMDNX_IF_ERR_EXIT(rv);    

    if (info->flags & BCM_L3_SOURCE_BIND_PPPoE) {
        SOC_PPC_SRC_BIND_PPPOE_ENTRY pppoe_bind_info;
        SOC_PPC_SRC_BIND_PPPOE_ENTRY_clear(&pppoe_bind_info);
        pppoe_bind_info.pppoe_session_id = info->session_id;
        pppoe_bind_info.lif_ndx = gport_hw_resources.global_in_lif;
        soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_pppoe_anti_spoofing_add, (unit, &pppoe_bind_info, &success));
    }
    else if ((info->flags & BCM_L3_SOURCE_BIND_IP6) == 0) {
        SOC_PPC_SRC_BIND_IPV4_ENTRY ipv4_bind_info;

        SOC_PPC_SRC_BIND_IPV4_ENTRY_clear(&ipv4_bind_info);
        /* Fill SOC_PPC_SRC_BIND_IPV4_ENTRY and call soc_ppd_src_bind_ipv4_add() */
        if (BCM_MAC_IS_ZERO(info->mac)) {
            ipv4_bind_info.smac_valid = 0;
        } else {
            ipv4_bind_info.smac_valid = 1;
            /* MAC from BCM to PPD strucutre*/
            rv = _bcm_petra_mac_to_sand_mac(info->mac,&(ipv4_bind_info.smac));
            BCMDNX_IF_ERR_EXIT(rv);
        }
        /* IPv4 Anti-Spoofing Static DB, the lookup will be VSIxSIP--> expected FEC ID or VSIxSIP--> expected LIF ID or */
        if ((ipv4_bind_info.smac_valid == 0) &&
             BCM_GPORT_SUB_TYPE_IS_FORWARD_GROUP(BCM_GPORT_VLAN_PORT_ID_GET(info->port)) &&
             SOC_DPP_L3_SOURCE_BIND_USE_FEC_LEARN_DATA(unit))
        {
            /* VSIxSIP-> expected FEC ID */
            ipv4_bind_info.lif_ndx = gport_hw_resources.fec_id;
        } else{
            /* VSIxSIP->expected LIF ID */
            ipv4_bind_info.lif_ndx = gport_hw_resources.global_in_lif;
        }
        ipv4_bind_info.sip = info->ip;
        soc_sand_rv = soc_ppd_lif_table_entry_get(soc_sand_dev_id, gport_hw_resources.local_in_lif, &lif_entry_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        ipv4_bind_info.vsi_id = lif_entry_info.value.ac.vsid;

        if (info->flags & BCM_L3_SOURCE_BIND_USE_MASK) {
            ipv4_bind_info.prefix_len = bcm_ip_mask_length(info->ip_mask);
            ipv4_bind_info.is_network = TRUE;
        } else {
            ipv4_bind_info.prefix_len = SOC_SAND_PP_IPV4_ADDRESS_NOF_BITS;
            ipv4_bind_info.is_network = FALSE;
        }
        soc_sand_rv = soc_ppd_src_bind_ipv4_add(soc_sand_dev_id, &ipv4_bind_info, &success);
    }
    else {
        SOC_PPC_SRC_BIND_IPV6_ENTRY ipv6_bind_info;
        SOC_PPC_IPV6_COMPRESSED_ENTRY ip6_compression_entry;

        SOC_PPC_SRC_BIND_IPV6_ENTRY_clear(&ipv6_bind_info);
        sal_memset(&ip6_compression_entry, 0, sizeof(SOC_PPC_IPV6_COMPRESSED_ENTRY));

        if (_BCM_L3_SOURCE_BIND_IP6_COMPRESSION(info->rate_id)) {
            rv = _bcm_petra_l3_ip6_compression_to_ppd(unit, gport_hw_resources.local_in_lif, gport_hw_resources.global_in_lif, info, &ip6_compression_entry);
            BCMDNX_IF_ERR_EXIT(rv);
            soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_ip6_compression_add, (unit, &ip6_compression_entry, &success));
        } else {
            /* Fill SOC_PPC_SRC_BIND_IPV6_ENTRY and call soc_ppd_src_bind_ipv6_add() */
            if (BCM_MAC_IS_ZERO(info->mac)) {
                ipv6_bind_info.smac_valid = 0;
            } else {
                ipv6_bind_info.smac_valid = 1;
                /* MAC from BCM to PPD strucutre*/
                rv = _bcm_petra_mac_to_sand_mac(info->mac,&(ipv6_bind_info.smac));
                BCMDNX_IF_ERR_EXIT(rv);
            }
            if (info->flags & BCM_L3_SOURCE_BIND_USE_MASK) {
                ipv6_bind_info.prefix_len = bcm_ip6_mask_length(info->ip6_mask);
                ipv6_bind_info.is_network = TRUE;
            } else {
                ipv6_bind_info.prefix_len = SOC_SAND_PP_IPV6_ADDRESS_NOF_BITS;
                ipv6_bind_info.is_network = FALSE;
            }
            ipv6_bind_info.lif_ndx = gport_hw_resources.global_in_lif;
            ret = _bcm_l3_bcm_ipv6_addr_to_sand_ipv6_addr(unit, info->ip6, &(ipv6_bind_info.sip6));
            BCMDNX_IF_ERR_EXIT(ret);

            soc_sand_rv = soc_ppd_src_bind_ipv6_add(soc_sand_dev_id, &ipv6_bind_info, &success);
        }
    }

    /* success store error code such SOC_E_FULL, it shoud be checked at first to get expected error code */
    SOC_SAND_IF_FAIL_RETURN(success);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      bcm_petra_l3_source_bind_delete
 * Purpose:
 *      Remove an existing L3 source binding.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) L3 source binding information
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_source_bind_delete(
    int unit, 
    bcm_l3_source_bind_t *info)
{
    bcm_error_t rv = BCM_E_NONE;
    uint32 soc_sand_rv;
    SOC_SAND_SUCCESS_FAILURE success = SOC_SAND_SUCCESS;
    unsigned int soc_sand_dev_id;
    uint32 ret;
    SOC_PPC_LIF_ENTRY_INFO lif_entry_info;
    _bcm_dpp_gport_hw_resources gport_hw_resources;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_L3_UNIT_INIT_CHECK;

    SOC_PPC_LIF_ENTRY_INFO_clear(&lif_entry_info);
    rv = _bcm_petra_l3_source_bind_check(unit, info);
    BCMDNX_IF_ERR_EXIT(rv);
    soc_sand_dev_id = (unit);

    rv = _bcm_dpp_gport_to_hw_resources(unit, info->port, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS | _BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK, 
                                                 &gport_hw_resources);
    BCMDNX_IF_ERR_EXIT(rv);    

    if (info->flags & BCM_L3_SOURCE_BIND_PPPoE) {
        SOC_PPC_SRC_BIND_PPPOE_ENTRY pppoe_bind_info;
        SOC_PPC_SRC_BIND_PPPOE_ENTRY_clear(&pppoe_bind_info);
        pppoe_bind_info.pppoe_session_id = info->session_id;
        pppoe_bind_info.lif_ndx = gport_hw_resources.global_in_lif;
        soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_pppoe_anti_spoofing_delete, (unit, &pppoe_bind_info, &success));
    }
    else if ((info->flags & BCM_L3_SOURCE_BIND_IP6) == 0) {
        SOC_PPC_SRC_BIND_IPV4_ENTRY ipv4_bind_info;

        SOC_PPC_SRC_BIND_IPV4_ENTRY_clear(&ipv4_bind_info);
        /* Fill SOC_PPC_SRC_BIND_IPV4_ENTRY and call soc_ppd_src_bind_ipv4_add() */
        if (BCM_MAC_IS_ZERO(info->mac)) {
            ipv4_bind_info.smac_valid = 0;
        } else {
            ipv4_bind_info.smac_valid = 1;
            /* MAC from BCM to PPD strucutre*/
            rv = _bcm_petra_mac_to_sand_mac(info->mac,&(ipv4_bind_info.smac));
            BCMDNX_IF_ERR_EXIT(rv);
        }
        ipv4_bind_info.lif_ndx = gport_hw_resources.global_in_lif;
        ipv4_bind_info.sip = info->ip;
        soc_sand_rv = soc_ppd_lif_table_entry_get(soc_sand_dev_id, gport_hw_resources.local_in_lif, &lif_entry_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        ipv4_bind_info.vsi_id = lif_entry_info.value.ac.vsid;
        if (info->flags & BCM_L3_SOURCE_BIND_USE_MASK) {
            ipv4_bind_info.prefix_len = bcm_ip_mask_length(info->ip_mask);
            ipv4_bind_info.is_network = TRUE;
        } else {
            ipv4_bind_info.prefix_len = SOC_SAND_PP_IPV4_ADDRESS_NOF_BITS;
            ipv4_bind_info.is_network = FALSE;
        }
        soc_sand_rv = soc_ppd_src_bind_ipv4_remove(soc_sand_dev_id, &ipv4_bind_info, &success);
    }
    else {
        SOC_PPC_SRC_BIND_IPV6_ENTRY ipv6_bind_info;
        SOC_PPC_IPV6_COMPRESSED_ENTRY ip6_compression_entry;

        SOC_PPC_SRC_BIND_IPV6_ENTRY_clear(&ipv6_bind_info);
        sal_memset(&ip6_compression_entry, 0, sizeof(SOC_PPC_IPV6_COMPRESSED_ENTRY));

        if (_BCM_L3_SOURCE_BIND_IP6_COMPRESSION(info->rate_id)) {
            rv = _bcm_petra_l3_ip6_compression_to_ppd(unit, gport_hw_resources.local_in_lif, gport_hw_resources.global_in_lif, info, &ip6_compression_entry);
            BCMDNX_IF_ERR_EXIT(rv);
            soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_ip6_compression_delete, (unit, &ip6_compression_entry, &success));
        } else {
            /* Fill SOC_PPC_SRC_BIND_IPV6_ENTRY and call soc_ppd_src_bind_ipv6_add() */
            if (BCM_MAC_IS_ZERO(info->mac)) {
                ipv6_bind_info.smac_valid = 0;
            } else {
                ipv6_bind_info.smac_valid = 1;
                /* MAC from BCM to PPD strucutre*/
                rv = _bcm_petra_mac_to_sand_mac(info->mac,&(ipv6_bind_info.smac));
                BCMDNX_IF_ERR_EXIT(rv);
            }
            if (info->flags & BCM_L3_SOURCE_BIND_USE_MASK) {
                ipv6_bind_info.prefix_len = bcm_ip6_mask_length(info->ip6_mask);
                ipv6_bind_info.is_network = TRUE;
            } else {
                ipv6_bind_info.prefix_len = SOC_SAND_PP_IPV6_ADDRESS_NOF_BITS;
                ipv6_bind_info.is_network = FALSE;
            }
            ipv6_bind_info.lif_ndx = gport_hw_resources.global_in_lif;
            ret = _bcm_l3_bcm_ipv6_addr_to_sand_ipv6_addr(unit, info->ip6, &(ipv6_bind_info.sip6));
            BCMDNX_IF_ERR_EXIT(ret);

            soc_sand_rv = soc_ppd_src_bind_ipv6_remove(soc_sand_dev_id, &ipv6_bind_info, &success);
        }
    }

    /* success store error code such SOC_E_FULL, it shoud be checked at first to get expected error code */
    SOC_SAND_IF_FAIL_RETURN(success);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_source_bind_delete_all
 * Purpose:
 *      Remove all existing L3 source bindings.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_source_bind_delete_all(
    int unit)
{
    uint32 soc_sand_rv;
    unsigned int soc_sand_dev_id;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_L3_UNIT_INIT_CHECK;


    if (SOC_DPP_L3_SOURCE_BIND_DISABLE(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Can't call this api when l3 source binding function is disabled")));
    }

    soc_sand_dev_id = (unit);
    /* Clean all IPv4 entry */
    if (SOC_DPP_L3_SRC_BIND_IPV4_ENABLE(unit)) {
        soc_sand_rv = soc_ppd_src_bind_ipv4_table_clear(soc_sand_dev_id);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
    /* Clean all IPv6 entry */
    if (SOC_DPP_L3_SRC_BIND_IPV6_ENABLE(unit)) {
        soc_sand_rv = soc_ppd_src_bind_ipv6_table_clear(soc_sand_dev_id);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_source_bind_get
 * Purpose:
 *      Retrieve the details of an existing L3 source binding.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN/OUT) L3 source binding information
 * Returns:
 *      BCM_E_xxx
 * Notes:
 *      Not Supported
 */
int 
bcm_petra_l3_source_bind_get(
    int unit, 
    bcm_l3_source_bind_t *info)
{
    bcm_error_t rv = BCM_E_NONE;
    uint32 soc_sand_rv;
    uint8 found = FALSE;
    unsigned int soc_sand_dev_id;
    uint32 ret;
    SOC_PPC_LIF_ENTRY_INFO lif_entry_info;
    _bcm_dpp_gport_hw_resources gport_hw_resources;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_L3_UNIT_INIT_CHECK;

    SOC_PPC_LIF_ENTRY_INFO_clear(&lif_entry_info);
    rv = _bcm_petra_l3_source_bind_check(unit, info);
    BCMDNX_IF_ERR_EXIT(rv);
    soc_sand_dev_id = (unit);

    rv = _bcm_dpp_gport_to_hw_resources(unit, info->port, _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS | _BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK, 
                                                 &gport_hw_resources);
    BCMDNX_IF_ERR_EXIT(rv);    

    if (info->flags & BCM_L3_SOURCE_BIND_PPPoE) {
        SOC_PPC_SRC_BIND_PPPOE_ENTRY pppoe_bind_info;
        SOC_PPC_SRC_BIND_PPPOE_ENTRY_clear(&pppoe_bind_info);
        pppoe_bind_info.pppoe_session_id = info->session_id;
        pppoe_bind_info.lif_ndx = gport_hw_resources.global_in_lif;
        soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_pppoe_anti_spoofing_get, (unit, &pppoe_bind_info, &found));
    }
    else if ((info->flags & BCM_L3_SOURCE_BIND_IP6) == 0) {
        SOC_PPC_SRC_BIND_IPV4_ENTRY ipv4_bind_info;

        SOC_PPC_SRC_BIND_IPV4_ENTRY_clear(&ipv4_bind_info);
        /* Fill SOC_PPC_SRC_BIND_IPV4_ENTRY and call soc_ppd_src_bind_ipv4_add() */
        if (BCM_MAC_IS_ZERO(info->mac)) {
            ipv4_bind_info.smac_valid = 0;
        } else {
            ipv4_bind_info.smac_valid = 1;
            /* MAC from BCM to PPD strucutre*/
            rv = _bcm_petra_mac_to_sand_mac(info->mac,&(ipv4_bind_info.smac));
            BCMDNX_IF_ERR_EXIT(rv);
        }
        ipv4_bind_info.lif_ndx = gport_hw_resources.global_in_lif;
        ipv4_bind_info.sip = info->ip;
        soc_sand_rv = soc_ppd_lif_table_entry_get(soc_sand_dev_id, gport_hw_resources.local_in_lif, &lif_entry_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        ipv4_bind_info.vsi_id = lif_entry_info.value.ac.vsid;
        if (info->flags & BCM_L3_SOURCE_BIND_USE_MASK) {
            ipv4_bind_info.prefix_len = bcm_ip_mask_length(info->ip_mask);
            ipv4_bind_info.is_network = TRUE;
        } else {
            ipv4_bind_info.prefix_len = SOC_SAND_PP_IPV4_ADDRESS_NOF_BITS;
            ipv4_bind_info.is_network = FALSE;
        }
        soc_sand_rv = soc_ppd_src_bind_ipv4_get(soc_sand_dev_id, &ipv4_bind_info, &found);
    }
    else {
        SOC_PPC_SRC_BIND_IPV6_ENTRY ipv6_bind_info;
        SOC_PPC_IPV6_COMPRESSED_ENTRY ip6_compression_entry;

        SOC_PPC_SRC_BIND_IPV6_ENTRY_clear(&ipv6_bind_info);
        sal_memset(&ip6_compression_entry, 0, sizeof(SOC_PPC_IPV6_COMPRESSED_ENTRY));

        if (_BCM_L3_SOURCE_BIND_IP6_COMPRESSION(info->rate_id)) {
            rv = _bcm_petra_l3_ip6_compression_to_ppd(unit, gport_hw_resources.local_in_lif, gport_hw_resources.global_in_lif, info, &ip6_compression_entry);
            BCMDNX_IF_ERR_EXIT(rv);
            soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_ip6_compression_get, (unit, &ip6_compression_entry, &found));
            _BCM_L3_SOURCE_BIND_IP6_COMPRESSION_RESULT_SET(info->rate_id, ip6_compression_entry.ip6_tt_compressed_result, ip6_compression_entry.ip6_compressed_result);
        } else {
            /* Fill SOC_PPC_SRC_BIND_IPV6_ENTRY and call soc_ppd_src_bind_ipv6_add() */
            if (BCM_MAC_IS_ZERO(info->mac)) {
                ipv6_bind_info.smac_valid = 0;
            } else {
                ipv6_bind_info.smac_valid = 1;
                /* MAC from BCM to PPD strucutre*/
                rv = _bcm_petra_mac_to_sand_mac(info->mac,&(ipv6_bind_info.smac));
                BCMDNX_IF_ERR_EXIT(rv);
            }
            if (info->flags & BCM_L3_SOURCE_BIND_USE_MASK) {
                ipv6_bind_info.prefix_len = bcm_ip6_mask_length(info->ip6_mask);
                ipv6_bind_info.is_network = TRUE;
            } else {
                ipv6_bind_info.prefix_len = SOC_SAND_PP_IPV6_ADDRESS_NOF_BITS;
                ipv6_bind_info.is_network = FALSE;
            }
            ipv6_bind_info.lif_ndx = gport_hw_resources.global_in_lif;
            ret = _bcm_l3_bcm_ipv6_addr_to_sand_ipv6_addr(unit, info->ip6, &(ipv6_bind_info.sip6));
            BCMDNX_IF_ERR_EXIT(ret);

            soc_sand_rv = soc_ppd_src_bind_ipv6_get(soc_sand_dev_id, &ipv6_bind_info, &found);
        }
    }

    if (found) {
        rv = BCM_E_NONE;
    } else {
        rv = BCM_E_NOT_FOUND; 
    }
    BCMDNX_IF_ERR_EXIT(rv);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
  _bcm_ppd_l3_source_ipv4_host_get_block(
    int unit,
    bcm_l3_source_bind_traverse_cb cb,
    void *user_data
  )
{
    uint32
        ret,
        nof_entries = 0;
    SOC_SAND_TABLE_BLOCK_RANGE
        block_range;
    SOC_PPC_SRC_BIND_IPV4_ENTRY 
        *src_bind_ipv4s = NULL;
    SOC_PPC_FRWRD_IP_ROUTE_STATUS
        routes_status[DPP_FRWRD_IP_GET_BLOCK_NOF_ENTRIES];
    bcm_l3_source_bind_t
        entry;
    int idx;

    BCMDNX_INIT_FUNC_DEFS
    soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&block_range);

    /* Block range */
    block_range.entries_to_act = DPP_FRWRD_IP_GET_BLOCK_NOF_ENTRIES;
    block_range.entries_to_scan = DPP_FRWRD_IP_ENTRIES_TO_SCAN;
    SOC_SAND_TBL_ITER_SET_BEGIN(&(block_range.iter));

    BCMDNX_ALLOC(src_bind_ipv4s, sizeof(SOC_PPC_SRC_BIND_IPV4_ENTRY)*DPP_FRWRD_IP_GET_BLOCK_NOF_ENTRIES, "_bcm_ppd_frwrd_ipv4_host_get_block.src_bind_ipv4s");
    if (src_bind_ipv4s == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_ERR_MSG_MODULE_NAME, unit, _BCM_MSG("failed to allocate memory")));
    }

    while(!SOC_SAND_TBL_ITER_IS_END(&(block_range.iter)))
    {
        ret = soc_ppd_src_bind_ipv4_host_get_block(unit,
                                            &block_range,
                                            src_bind_ipv4s,
                                            routes_status,
                                            &nof_entries);
        if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
        {
            LOG_ERROR(BSL_LS_BCM_L3,
                      (BSL_META_U(unit,
                                  "soc_sand error %x\n"), ret));
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_ERR_MSG_MODULE_NAME, unit, _BCM_MSG("_bcm_ppd_l3_source_ipv4_get_block failed")));
        }

        if(nof_entries == 0)
        {
            break;
        }

        for(idx = 0; idx < nof_entries; idx++)
        {
            _bcm_dpp_gport_hw_resources gport_hw_resources;
            bcm_l3_source_bind_t_init(&entry);

            gport_hw_resources.global_out_lif = src_bind_ipv4s[idx].lif_ndx;
            ret = _bcm_dpp_gport_from_hw_resources(unit, &entry.port, _BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS, &gport_hw_resources);
            BCMDNX_IF_ERR_EXIT(ret);

            entry.ip = src_bind_ipv4s[idx].sip;

            /* Invoke user callback. */
            (*cb)(unit, &entry, user_data);
        }
    }

    BCM_EXIT;
exit:
    BCM_FREE(src_bind_ipv4s);
    BCMDNX_FUNC_RETURN;
}

int
  _bcm_ppd_l3_source_ipv4_subnet_get_block(
    int unit,
    bcm_l3_source_bind_traverse_cb cb,
    void *user_data
  )
{  
    uint32
      ret;
    
    SOC_PPC_VRF_ID
      vrf_ndx = BCM_L3_VRF_DEFAULT;
    SOC_PPC_IP_ROUTING_TABLE_RANGE
      block_range;
    SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY
      *route_keys = NULL;
    SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY
      vpn_route_keys[DPP_FRWRD_IP_GET_BLOCK_NOF_ENTRIES];
    SOC_PPC_FEC_ID
      fec_ids[DPP_FRWRD_IP_GET_BLOCK_NOF_ENTRIES];
    SOC_PPC_FRWRD_IP_ROUTE_STATUS
      routes_status[DPP_FRWRD_IP_GET_BLOCK_NOF_ENTRIES];
    SOC_PPC_FRWRD_IP_ROUTE_LOCATION
      routes_location[DPP_FRWRD_IP_GET_BLOCK_NOF_ENTRIES];
    uint32
      nof_entries, vrf_max;
    bcm_l3_source_bind_t
      entry;
    int idx, status;

    BCMDNX_INIT_FUNC_DEFS;

    SOC_PPC_IP_ROUTING_TABLE_RANGE_clear(&block_range);

    block_range.entries_to_act = DPP_FRWRD_IP_GET_BLOCK_NOF_ENTRIES;
    block_range.entries_to_scan = DPP_FRWRD_IP_ENTRIES_TO_SCAN;
    block_range.start.type = SOC_PPC_IP_ROUTING_TABLE_ITER_TYPE_IP_PREFIX_ORDERED;
    soc_sand_u64_clear(&block_range.start.payload);

    status = _bcm_ppd_frwrd_ipv4_nof_vrfs_get(unit, &vrf_max);

    if (status != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_L3,
                  (BSL_META_U(unit,
                              "nof_vrfs_get failed: %x\n"), status));
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_ERR_MSG_MODULE_NAME, unit, _BCM_MSG("_bcm_ppd_frwrd_ipv4_nof_vrfs_getv failed")));
    }

    BCMDNX_ALLOC(route_keys, sizeof(SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY)*DPP_FRWRD_IP_GET_BLOCK_NOF_ENTRIES, "_bcm_ppd_l3_source_ipv4_subnet_get_block.route_keys");
    if (route_keys == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_ERR_MSG_MODULE_NAME, unit, _BCM_MSG("failed to allocate memory")));
    }

    for (vrf_ndx=BCM_L3_VRF_DEFAULT; vrf_ndx<vrf_max; vrf_ndx++)
    {
      soc_sand_u64_clear(&block_range.start.payload);
      while(!SOC_PPC_IP_ROUTING_TABLE_ITER_IS_END(&block_range.start.payload))
      {
        if (vrf_ndx == BCM_L3_VRF_DEFAULT)
        {
          ret = soc_ppd_frwrd_ipv4_uc_route_get_block(unit,
                                                  &block_range,
                                                  route_keys,
                                                  fec_ids,
                                                  routes_status,
                                                  routes_location,
                                                  &nof_entries);
        } else {
          ret = soc_ppd_frwrd_ipv4_vrf_route_get_block(unit,
                                                   vrf_ndx,
                                                   &block_range,
                                                   vpn_route_keys,
                                                   fec_ids,
                                                   routes_status,
                                                   routes_location,
                                                   &nof_entries);
        }

        if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
        {
          LOG_ERROR(BSL_LS_BCM_L3,
                    (BSL_META_U(unit,
                                "soc_sand error %x\n"), ret));
          BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_ERR_MSG_MODULE_NAME, unit, _BCM_MSG("soc_ppd_frwrd_ipv4_vrf_route_get_block failed")));
        }

        if(nof_entries == 0)
        {
            break;
        }

        for(idx = 0; idx < nof_entries; idx++)
        {
          bcm_l3_source_bind_t_init(&entry);
          if (fec_ids[idx])
          {
              _bcm_dpp_gport_hw_resources gport_hw_resources;

              gport_hw_resources.global_out_lif = fec_ids[idx];
              ret = _bcm_dpp_gport_from_hw_resources(unit, &entry.port, _BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS, &gport_hw_resources);
              BCMDNX_IF_ERR_EXIT(ret); 

              if (vrf_ndx == BCM_L3_VRF_DEFAULT)
              {
                  entry.ip = route_keys[idx].subnet.ip_address;
                  entry.ip_mask = bcm_ip_mask_create(route_keys[idx].subnet.prefix_len);
              } else {
                  entry.ip = vpn_route_keys[idx].subnet.ip_address;
                  entry.ip_mask = bcm_ip_mask_create(vpn_route_keys[idx].subnet.prefix_len);
              }

              /* Invoke user callback. */
              (*cb)(unit, &entry, user_data);
          }
        }
      }
    }

    BCM_EXIT;
exit:
    BCM_FREE(route_keys);
    BCMDNX_FUNC_RETURN;
}


int
  _bcm_ppd_l3_source_ipv6_get_block(
    int unit,
    bcm_l3_source_bind_traverse_cb cb,
    void *user_data
  )
{
    bcm_error_t
        rv;
    uint32
        soc_sand_rv,
        nof_entries = 0;
    SOC_PPC_IP_ROUTING_TABLE_RANGE
        block_range_key;
    SOC_PPC_SRC_BIND_IPV6_ENTRY
        *src_bind_ipv6s = NULL;
    SOC_PPC_FRWRD_IP_ROUTE_STATUS
        routes_status[DPP_FRWRD_IP_GET_BLOCK_NOF_ENTRIES];
    bcm_l3_source_bind_t 
        entry;
    int idx;
    bcm_ip6_t       ip6_mask;
    int ret;

    BCMDNX_INIT_FUNC_DEFS;
    SOC_PPC_IP_ROUTING_TABLE_RANGE_clear(&block_range_key);

    unit = (unit);

    /* Block range key */
    block_range_key.start.type = SOC_PPC_IP_ROUTING_TABLE_ITER_TYPE_IP_PREFIX_ORDERED;
    block_range_key.entries_to_act = DPP_FRWRD_IP_GET_BLOCK_NOF_ENTRIES;
    block_range_key.entries_to_scan = DPP_FRWRD_IP_ENTRIES_TO_SCAN;
    BCMDNX_ALLOC(src_bind_ipv6s, sizeof(SOC_PPC_SRC_BIND_IPV6_ENTRY)*DPP_FRWRD_IP_GET_BLOCK_NOF_ENTRIES, "_bcm_ppd_l3_source_ipv6_get_block.src_bind_ipv6s");
    if (src_bind_ipv6s == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_ERR_MSG_MODULE_NAME, unit, _BCM_MSG("failed to allocate memory")));
    }

    while(!SOC_PPC_IP_ROUTING_TABLE_ITER_IS_END(&block_range_key.start.payload))
    {
      /* Call function */
      soc_sand_rv = soc_ppd_src_bind_ipv6_get_block(unit,&block_range_key,src_bind_ipv6s,routes_status,&nof_entries);
      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
      
      for(idx = 0; idx < nof_entries; idx++)
      {
          _bcm_dpp_gport_hw_resources gport_hw_resources;

          bcm_l3_source_bind_t_init(&entry);
          rv = _bcm_l3_sand_ipv6_addr_to_bcm_ipv6_addr(unit,
                               &(src_bind_ipv6s[idx].sip6),
                               &(entry.ip6));
          BCMDNX_IF_ERR_EXIT(rv);

          gport_hw_resources.global_out_lif = src_bind_ipv6s[idx].lif_ndx;
          ret = _bcm_dpp_gport_from_hw_resources(unit, &entry.port, _BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS, &gport_hw_resources);
          BCMDNX_IF_ERR_EXIT(ret); 

          if (src_bind_ipv6s[idx].is_network) {
              rv = bcm_ip6_mask_create(ip6_mask, src_bind_ipv6s[idx].prefix_len);
              if (rv == BCM_E_PARAM){   /* prefix_len 0 is a legitimate value, but bcm_ip6_mask_create reutrns BCM_E_PARAM when getting it as argument. */
                  rv = BCM_E_NONE;
              }
              BCMDNX_IF_ERR_EXIT(rv);
              sal_memcpy(&(entry.ip6_mask),&(ip6_mask),sizeof(bcm_ip6_t));
          }
          /* Invoke user callback. */
          (*cb)(unit, &entry, user_data);
       }
    }

    BCM_EXIT;
exit:
    BCM_FREE(src_bind_ipv6s);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_source_bind_traverse
 * Purpose:
 *      Traverse through the L3 source bindings and run callback at
 *      each defined binding.
 * Parameters:
 *      unit - (IN) Unit number.
 *      cb - (IN) Callback function
 *      user_data - (IN) User data to be passed to callback function
 * Returns:
 *      BCM_E_xxx
 * Notes:
 *      Not Supported
 */
int 
bcm_petra_l3_source_bind_traverse(
    int unit, 
    bcm_l3_source_bind_traverse_cb cb, 
    void *user_data)
{
    uint32 soc_sand_rv;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_L3_UNIT_INIT_CHECK;


    if (SOC_DPP_L3_SOURCE_BIND_DISABLE(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_ERR_MSG_MODULE_NAME, unit, _BCM_MSG("Can't call this api when l3 source binding function is disabled")));
    }

    /* traverse all IPv4 host entry from LEM */
    if (SOC_DPP_L3_SRC_BIND_IPV4_HOST_ENABLE(unit)) {
        soc_sand_rv = _bcm_ppd_l3_source_ipv4_host_get_block(unit, cb, user_data);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

    /* traverse all IPv4 subnet entry from LPM */
    if (SOC_DPP_L3_SRC_BIND_IPV4_SUBNET_ENABLE(unit)) {
        soc_sand_rv = _bcm_ppd_l3_source_ipv4_subnet_get_block(unit, cb, user_data);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

    /* traverse all IPv6 entry from TCAM */
    if (SOC_DPP_L3_SRC_BIND_IPV6_ENABLE(unit)) {
        soc_sand_rv = _bcm_ppd_l3_source_ipv6_get_block(unit, cb, user_data);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

exit:
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
    BCMDNX_FUNC_RETURN;
}



/*
 * Function:
 *      bcm_petra_l3_vrrp_add
 * Purpose:
 *      Add VRID for the given VSI. Adding a VRID using this API means
 *      the physical node has become the master for the virtual router
 * Parameters:
 *      unit - (IN) Unit number.
 *      vlan - (IN) VLAN/VSI
 *      vrid - (IN) VRID - Virtual router ID to be added
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_vrrp_add(
    int unit, 
    bcm_vlan_t vlan, 
    uint32 vrid)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    BCM_RETURN_VAL_EXIT(BCM_E_UNAVAIL);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_vrrp_delete
 * Purpose:
 *      Delete VRID for a particulat VLAN/VSI
 * Parameters:
 *      unit - (IN) Unit number.
 *      vlan - (IN) VLAN/VSI
 *      vrid - (IN) VRID - Virtual router ID to be deleted
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_vrrp_delete(
    int unit, 
    bcm_vlan_t vlan, 
    uint32 vrid)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    BCM_RETURN_VAL_EXIT(BCM_E_UNAVAIL);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_vrrp_delete_all
 * Purpose:
 *      Delete all the VRIDs for a particular VLAN/VSI
 * Parameters:
 *      unit - (IN) Unit number.
 *      vlan - (IN) VLAN/VSI
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_vrrp_delete_all(
    int unit, 
    bcm_vlan_t vlan)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    BCM_RETURN_VAL_EXIT(BCM_E_UNAVAIL);
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_vrrp_get
 * Purpose:
 *      Get all the VRIDs for which the physical node is master for
 *      the virtual routers on the given VLAN/VSI
 * Parameters:
 *      unit - (IN) Unit number.
 *      vlan - (IN) VLAN/VSI
 *      alloc_size - (IN) Number of vrid_array elements
 *      vrid_array - (OUT) Pointer to the array to which the VRIDs will be copied
 *      count - (OUT) Number of VRIDs copied
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_vrrp_get(
    int unit, 
    bcm_vlan_t vlan, 
    int alloc_size, 
    int *vrid_array, 
    int *count)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    BCM_RETURN_VAL_EXIT(BCM_E_UNAVAIL);
exit:
    BCMDNX_FUNC_RETURN;
}

int 
_bcm_l3_vrrp_config_verify(
    int unit, 
    uint32 flags, 
    bcm_vlan_t vlan,
    int* max_vrid,
    int* ipv6_distinct,
    uint8* mode)
{
    int max_vlan;
    soc_dpp_config_l3_t *dpp_l3;

    BCMDNX_INIT_FUNC_DEFS;

    if (_BCM_L3_MULTIPLE_MYMAC_EXCLUSIVE(unit)){
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("VRRP max vid soc property was set to 0. Can't use VRRP APIs.")));
    }

    /* Check vlan */
    if ((vlan == 0 && SOC_IS_ARAD_B1_AND_BELOW(unit) /* ARADPLUS and jericho can tolerate vlan 0 */) 
        || (vlan >= _BCM_PETRA_L3_VRRP_MAX_VSI(unit))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Vlan is not valid")));
    } 

    if (flags == 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("At least one of the flags: IPV4, IPV6 must be set")));
    }

    dpp_l3 = &(SOC_DPP_CONFIG(unit))->l3;

    max_vlan = dpp_l3->vrrp_max_vid;
    if (ipv6_distinct != NULL) {
        *ipv6_distinct = dpp_l3->vrrp_ipv6_distinct;
    }
    *max_vrid = 256;
    *mode = SOC_PPC_MYMAC_VRRP_MODE_ALL_VSI_BASED;

    /* In vanila Arad, if 4096 vlans are used only 8 VRIDs are supported. */
    if (max_vlan == 4096 && SOC_IS_ARAD_B1_AND_BELOW(unit)) {
        *max_vrid = 8;
        (*mode)++; /* We don't care about the actual value, we just want it different than the previous value. */
    }

    if (SOC_IS_ARADPLUS(unit) && max_vlan == 256 && _BCM_PETRA_L3_VRRP_VLAN_IS_ALL_VSI(vlan)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Configuring all VSIs is not supported when vrrp_max_vid == 256")));
    }

    if (SOC_IS_ARADPLUS_AND_BELOW(unit) && vlan >= max_vlan) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Vlan is not supported")));
    }

    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        /* In Jericho, all flag combinations are always legal. */
        if (dpp_l3->vrrp_ipv6_distinct) { /* IPV6 distinct */
            if ((flags & BCM_L3_VRRP_IPV6) && (flags & BCM_L3_VRRP_IPV4)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("flags: IPV4 and IPV6 cannot be both set, because VRRP mode is IPV6 distinct")));
            }
        }
        else { /* VRRP mode is shared between IPV4 and IPV6 */
            if ((!((flags & BCM_L3_VRRP_IPV6) && (flags & BCM_L3_VRRP_IPV4))) ||
                (!((flags & BCM_L3_VRRP_IPV4) && (flags & BCM_L3_VRRP_IPV6)))) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("flags: IPV4 and IPV6 must be both set, because VRRP mode is not IPV6 distinct")));
            }
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_vrrp_config_add
 * Purpose:
 *      Add VRID for the given VSI, for IPV4 or IPV6. Adding a VRID using this
 *      API means the physical node has become the master for the virtual
 *      router
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN) BCM_L3_VRRP_IPV4, BCM_L3_VRRP_IPV6
 *      vlan - (IN) VLAN/VSI
 *      vrid - (IN) VRID - Virtual router ID to be added
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_l3_vrrp_config_add(
    int unit, 
    uint32 flags,
    bcm_vlan_t vlan, 
    uint32 vrid)
{
    int rv = BCM_E_NONE;
    int max_vrid;
    int ipv6_distinct;
    uint8 mode;
#ifdef BCM_88660_A0
    SOC_PPC_VRRP_CAM_INFO cam_info;
#endif
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_L3_UNIT_INIT_CHECK;

    DPP_L3_LOCK_TAKE;

    rv = _bcm_l3_vrrp_config_verify(unit, flags, vlan, &max_vrid, &ipv6_distinct, &mode);
    BCMDNX_IF_ERR_EXIT(rv);

    if (vrid >= max_vrid) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VRID is out of range")));
    }

#ifdef BCM_88660_A0
    if (SOC_IS_ARADPLUS(unit) && mode == SOC_PPC_MYMAC_VRRP_MODE_ALL_VSI_BASED){

        SOC_PPC_VRRP_CAM_INFO_clear(&cam_info);

        cam_info.is_ipv4_entry = ipv6_distinct;
        cam_info.flags = flags;
        rv = _bcm_l3_vrrp_mac_to_global_mac(unit, &cam_info, flags, vrid);
        if (rv != BCM_E_NONE) {
            BCMDNX_IF_ERR_EXIT(rv);    
        }

        rv = _bcm_l3_vrrp_setting_add(unit, &cam_info, vlan);
        if (rv != BCM_E_NONE) {
            BCMDNX_IF_ERR_EXIT(rv);    
        }
    } else
#endif
    /* add this VRID to IPV4 or IPV6, according to flags */
    if (ipv6_distinct && (flags & BCM_L3_VRRP_IPV6)) {
        rv = _bcm_ppd_mymac_vrrp_mac_set(unit, vlan, vrid, _BCM_DPP_VRRP_MAC_ENABLE | _BCM_DPP_VRRP_MAC_IS_IPV6);
    }
    else { /* IPV4 only or shared */
       /* ipv4 is the default */
       rv = _bcm_ppd_mymac_vrrp_mac_set(unit, vlan, vrid, _BCM_DPP_VRRP_MAC_ENABLE);
    }

exit:
    DPP_L3_LOCK_RELEASE;
    BCMDNX_FUNC_RETURN;
}


#ifdef BCM_88660_A0
/*
 * Function:
 *      _bcm_l3_vrrp_mac_to_global_mac
 * Purpose:
 *      Assigns VRRP mac address according to protocol (IPV4/ IPV6) and vrid.
 * Parameters: 
 *      unit            - Device id to be configured 
 *      cam_info        - The struct to be populated
 *       ->is_ipv_entry - When calling the function, it represents whether ipv4 distinctions is active in the device.
 *                          When the function returns, it represents whether the entry is an ipv4 only entry.
 *      flags           - BCM_L3_VRRP_IPV4, BCM_L3_VRRP_IPV6.
 *      vrid            - lsb of the mac address
 * Returns:
 *      void
 */
int 
_bcm_l3_vrrp_mac_to_global_mac(int unit, SOC_PPC_VRRP_CAM_INFO *cam_info, uint32 flags, int vrid){
    int rv;
    uint32 protocol_flags;
    BCMDNX_INIT_FUNC_DEFS;

    /* Configure mac address*/
    if (cam_info->is_ipv4_entry && (flags & BCM_L3_VRRP_IPV4) && !(flags & BCM_L3_VRRP_IPV6)) { /* ipv4 */
        _bcm_petra_mac_to_sand_mac(vrrp_ipv4_mask , &cam_info->mac_addr);
        cam_info->is_ipv4_entry = TRUE;
    } else if (((cam_info->is_ipv4_entry) || SOC_IS_JERICHO(unit)) && (!(flags & BCM_L3_VRRP_IPV4)) && (flags & BCM_L3_VRRP_IPV6)) { /* ipv6 */
        _bcm_petra_mac_to_sand_mac(vrrp_ipv6_mask  , &cam_info->mac_addr);
        cam_info->is_ipv4_entry = FALSE;
    } else {
        _bcm_petra_mac_to_sand_mac(vrrp_ipv4_mask , &cam_info->mac_addr);
        cam_info->is_ipv4_entry = FALSE;
    }
    cam_info->mac_addr.address[0] = vrid;

    /* Configure Jericho specific parameters */
    if (SOC_IS_JERICHO(unit)) {
        cam_info->is_ipv4_entry = FALSE; /* Irrelevant in Jericho */
        /* Configure mac mask */
        _bcm_petra_mac_to_sand_mac(vrrp_mac_mask, &cam_info->mac_mask);

        /* Configure protocol group, and protocol group mask */
        BCM_PETRA_L3_VRRP_TRANSLATE_API_FLAGS_TO_INTERNAL_PROTOCOL_GROUP_FLAGS(flags, protocol_flags);
        rv = bcm_dpp_switch_l3_protocol_group_get_group_by_members(unit, protocol_flags, &(cam_info->protocol_group), 
                                                                   &(cam_info->protocol_group_mask), TRUE);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_l3_vrrp_setting_add
 * Purpose:
 *      Given cam info and vlan (vsi), updates the vrrp cam table, then updates the vrrp mac table to point to the correct
 *      entry in the cam table.
 * Parameters: 
 *      unit            - (IN) Device id to be configured 
 *      cam_info        - (INOUT) Cam entry to be configured. After the call, vrrp_cam_index will be filled with the cam entry used.
 *      vlan            - (IN) VSI to be configured. 
 * Returns:
 *      BCM_E_*
 */
int 
_bcm_l3_vrrp_setting_add(int unit, SOC_PPC_VRRP_CAM_INFO *cam_info, int vlan){
    int is_first,is_last;
    int cam_index;
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    /* Get the entry where the mac is, or where the mac is to be placed. */
    rv = _bcm_dpp_am_template_vrrp_alloc(unit, cam_info, &is_first, &cam_index);
    BCMDNX_IF_ERR_EXIT(rv);

    if (_BCM_PETRA_L3_VRRP_VLAN_IS_ALL_VSI(vlan)) {
        /* Allocate all templates to this cam index. */
        rv = _bcm_dpp_am_template_vrrp_alloc_all(unit, cam_index);
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        /* Check the entry doesn't exist already */
        rv = _bcm_l3_vrrp_setting_exists(unit, cam_index, vlan);
        if (rv != BCM_E_NOT_FOUND) {
            _bcm_dpp_am_template_vrrp_dealloc(unit, cam_index, &is_last);
            BCMDNX_IF_ERR_EXIT(rv);
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("VSI is already configured for this mac address and protocols.")));
        }
    }


    /* Return cam index to the user. */
    cam_info->vrrp_cam_index = cam_index;

    /* If this is a new mac, allocate it in the vrrp cam table */
    if (is_first) {
        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_mymac_vrrp_cam_info_set, (unit, cam_info));
        BCMDNX_IF_ERR_EXIT(rv);

        /* Update protocol group counts */
        if (SOC_IS_JERICHO(unit)) {
            rv = bcm_dpp_switch_l3_protocol_group_change_count(unit, cam_info->protocol_group, cam_info->protocol_group_mask, 1);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

    if (_BCM_PETRA_L3_VRRP_VLAN_IS_ALL_VSI(vlan)) {
        /* Map all */
        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_mymac_vrid_mymac_map_set_to_all_vsi, (unit, cam_index, TRUE));
    } else {
        /* Map the vrrp mac table to the cam index */
        rv = _bcm_ppd_mymac_vrrp_mac_set(unit, vlan, cam_index, _BCM_DPP_VRRP_MAC_ENABLE);
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_l3_vrrp_setting_get
 * Purpose:
 *      Given a cam index (in the cam info), returns the complete cam info for the entry.
 * Parameters: 
 *      unit            - (IN) Device id to be configured 
 *      cam_info        - (INOUT) Fill vrrp_cam_index, the rest of the entry will be filled.
 * Returns:
 *      BCM_E_*
 */
int 
_bcm_l3_vrrp_setting_get(int unit, SOC_PPC_VRRP_CAM_INFO *cam_info){
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_am_template_vrrp_get(unit, cam_info->vrrp_cam_index, cam_info);
    BCM_RETURN_VAL_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_l3_vrrp_setting_exists
 * Purpose:
 *      Given a cam index and VSI (vlan), whether this vlan is pointing at this cam index.
 * Parameters: 
 *      unit            - (IN) Device id to be configured.
 *      profile_index   - (IN) Required profile index.
 *      vlan            - (IN) Required VSI.
 * Returns: 
 *      BCM_E_NOT_FOUND : If the entry is not configured. 
 *      BCM_E_NONE      : If the entry is configured.
 *      BCM_E_*         : Unexepcted.
 */
int
_bcm_l3_vrrp_setting_exists(int unit, int profile_index, int vlan){
    int rv; 
    uint8 enable;
    BCMDNX_INIT_FUNC_DEFS;
    /* Check if the entry in the vrrp mac table points to the cam index 
     In case of all VSIs, we don't care if the entries were allocated using allocate all (and then VSI 0 will be configured) or not. */
    if (!_BCM_PETRA_L3_VRRP_VLAN_IS_ALL_VSI(vlan)) {
        rv = _bcm_ppd_mymac_vrrp_mac_get(unit, vlan, profile_index, &enable);
        BCMDNX_IF_ERR_EXIT(rv);

        if (!enable) {
            BCM_ERR_EXIT_NO_MSG(BCM_E_NOT_FOUND);
        }
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_l3_vrrp_setting_delete
 * Purpose:
 *      Given either a cam index or cam info (either must be set) and vlan, makes the VSI stop pointing a the cam entry. If no VSIs are pointing
 *      at the cam entry, it is deleted.
 * Parameters: 
 *      unit            - (IN) Device id to be configured.
 *      profile_index   - (IN) Profile index to be deleted.
 *      cam_info        - (IN) Profile info to be deleted.
 *      vlan            - (IN) Required VSI.
 * Returns: 
 *      BCM_E_NOT_FOUND : If the entry is not configured. 
 *      BCM_E_NONE      : If the entry is deleted successfully.
 *      BCM_E_*         : Unexepcted.
 */
int 
_bcm_l3_vrrp_setting_delete(int unit, int profile_ndx, SOC_PPC_VRRP_CAM_INFO *cam_info, int vlan){
    int is_last;
    int rv;
    SOC_PPC_VRRP_CAM_INFO backup_cam_info;
    BCMDNX_INIT_FUNC_DEFS;


    if (profile_ndx < 0) {
        /* Get the entry where the mac is, or where the mac is to be placed. */
        BCMDNX_NULL_CHECK(cam_info);
        rv = _bcm_dpp_am_template_vrrp_index_get(unit, cam_info, &profile_ndx);
        BCMDNX_IF_ERR_EXIT(rv);
    } 
    else if (SOC_IS_JERICHO(unit)) { /* If soc is jericho, we need to get the protocol group and protocol group mask so we can decrease
                                        the deleted entries from the protocol group counters. */
        cam_info = &backup_cam_info;
        cam_info->vrrp_cam_index = profile_ndx;
        rv = _bcm_l3_vrrp_setting_get(unit, cam_info);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    rv = _bcm_l3_vrrp_setting_exists(unit, profile_ndx, vlan);
    if (rv == BCM_E_NOT_FOUND) {
        BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("Error: given vlan is not configured to this mac address\n")));
    }
    BCMDNX_IF_ERR_EXIT(rv);


    if (_BCM_PETRA_L3_VRRP_VLAN_IS_ALL_VSI(vlan)) {
        /* Dealloc all*/
        rv = _bcm_dpp_am_template_vrrp_dealloc_all(unit, profile_ndx);
        BCMDNX_IF_ERR_EXIT(rv);
        is_last = TRUE; /*Since we've deallocated all entries, the last entry was obviously also deallocated. */
    } else {
        /* Dealloc one entry. */
        rv = _bcm_dpp_am_template_vrrp_dealloc(unit, profile_ndx, &is_last);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* If this is the last entry using the mac, deallocate it in the vrrp cam table */
    if (is_last) {
        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_mymac_vrrp_cam_info_delete, (unit,profile_ndx));
        BCMDNX_IF_ERR_EXIT(rv);

        /* Update protocol group counts */
        if (SOC_IS_JERICHO(unit)) {
            rv = bcm_dpp_switch_l3_protocol_group_change_count(unit, cam_info->protocol_group, cam_info->protocol_group_mask, -1);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

    /* Delete mapping from the vrrp mac table. */
    if (_BCM_PETRA_L3_VRRP_VLAN_IS_ALL_VSI(vlan)) {
        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_mymac_vrid_mymac_map_set_to_all_vsi, (unit, profile_ndx, FALSE));
    } else {
        rv = _bcm_ppd_mymac_vrrp_mac_set(unit, vlan, profile_ndx, _BCM_DPP_VRRP_MAC_DISABLE);
        BCMDNX_IF_ERR_EXIT(rv);
    }


exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_l3_sand_mac_is_vrrp_mac
 * Purpose:
 *      Given a mac address is soc sand format, returns whether it's a vrrp mac address.
 * Parameters:
 *      address - (IN) mac address to be compared.
 *      is_vrrp - (OUT) If mac matches the vrrp prefix, will be set to TRUE.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
_bcm_l3_sand_mac_is_vrrp_mac(SOC_SAND_PP_MAC_ADDRESS *address, uint8 *is_vrrp){
    int i;
    bcm_mac_t mac;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    BCMDNX_IF_ERR_EXIT(_bcm_petra_mac_from_sand_mac(mac, address));

    *is_vrrp = TRUE;
    for (i = 0 ; i < SOC_SAND_PP_MAC_ADDRESS_NOF_U8 - 1 /* Skip the lsb, it's the vrid */ ; i++) {
        if (mac[i] != vrrp_ipv4_mask[i] && mac[i] != vrrp_ipv6_mask[i]) {
            *is_vrrp = FALSE;
            break;
        }
    }
exit:
    BCMDNX_FUNC_RETURN_NO_UNIT;
}

#endif /*BCM_88660_A0*/


/*
 * Function:
 *      bcm_petra_l3_vrrp_config_get
 * Purpose:
 *      Get all the VRIDs for which the physical node is master for the
 *      virtual routers on the given VLAN/VSI. VRIDs can be for IPV4 or IPV6
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN) BCM_L3_VRRP_IPV4, BCM_L3_VRRP_IPV6
 *      vlan - (IN) VLAN/VSI
 *      alloc_size - (IN) Number of vrid_array elements
 *      vrid_array - (OUT) Pointer to the array to which the VRIDs will be copied
 *      count - (OUT) Number of VRIDs copied
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcm_petra_l3_vrrp_config_get(
    int unit, 
    uint32 flags, 
    bcm_vlan_t vlan, 
    uint32 alloc_size, 
    uint32 *vrid_array, 
    uint32 *count)
{
    int rv = BCM_E_NONE, max_vrid, min_alloc_size;
    uint32 vrid = 0;
    uint8 enable;
    uint8 mode;
#ifdef BCM_88660_A0
    uint8 is_vrrp;
    uint32 protocols,
            existing_flags;
    uint8 write_entry;
    SOC_PPC_VRRP_CAM_INFO cam_info;
#endif  /* BCM_88660_A0 */

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_L3_UNIT_INIT_CHECK;

    DPP_L3_LOCK_TAKE;

    if (vrid_array == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Vrid_array is NULL")));
    }

    if (count == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Count is NULL")));
    } 

    rv = _bcm_l3_vrrp_config_verify(unit, flags, vlan, &max_vrid, NULL, &mode);
    BCMDNX_IF_ERR_EXIT(rv);

#ifdef BCM_88660_A0

    if (SOC_IS_ARADPLUS(unit) && mode == SOC_PPC_MYMAC_VRRP_MODE_ALL_VSI_BASED){
        /* In the get function, max_vrid actually represents the TCAM index.*/
        max_vrid = _BCM_PETRA_L3_VRRP_CAM_MAX_NOF_ENTIRES(unit);
    }
#endif /*BCM_88660_A0*/

    /* In case of Jericho, even though there can be 16 VRIDs, we don't want to return errors 
                        where there weren't any before, so we set it to the number of TCAM entries in A+. */
    min_alloc_size = 8;

    if ((alloc_size < min_alloc_size) || (alloc_size > max_vrid)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid alloc_size")));
    }

    sal_memset(vrid_array, 0, sizeof(int)*alloc_size);
    *count = 0;

    for (vrid = 0; vrid < max_vrid; vrid++) {
        enable = 0;
        rv = _bcm_ppd_mymac_vrrp_mac_get(unit, vlan, vrid, &enable);

        if (rv != BCM_E_NONE) {
            BCMDNX_IF_ERR_EXIT(rv);    
        }

#ifdef BCM_88660_A0
        /* In this mode, the vrid represent the index of the cam table. */
        if (SOC_IS_ARADPLUS(unit) && mode == SOC_PPC_MYMAC_VRRP_MODE_ALL_VSI_BASED){
            /* If the entry is not enabled, no point in going to the cam table. */
            if (enable) {
                /* In this case, the vrid is the index in the cam table. Get the entry.*/
                cam_info.vrrp_cam_index = vrid;
                rv = _bcm_l3_vrrp_setting_get(unit, &cam_info);
                if (rv != BCM_E_NONE) {
                    BCMDNX_IF_ERR_EXIT(rv);    
                }

                /* 
                 *  Decide whether the entry should be returned to the user.
                 */
                  
                /* Check mac address. If the mac MSBs is not vrrp, don't return it. */
                rv = _bcm_l3_sand_mac_is_vrrp_mac(&cam_info.mac_addr, &is_vrrp);
                if (rv != BCM_E_NONE) {
                    BCMDNX_IF_ERR_EXIT(rv);    
                }

                if (!is_vrrp) {
                    continue;
                }
                 
                /* If the entry was configured with the same flags the user gave, it will be returned. */
                write_entry = FALSE;
                if (SOC_IS_JERICHO(unit)) {
                    rv = bcm_dpp_switch_l3_protocol_group_get_members_by_group(unit, cam_info.protocol_group, 
                                                                               cam_info.protocol_group_mask, &protocols);
                    if (rv != BCM_E_NONE) {
                        BCMDNX_IF_ERR_EXIT(rv);    
                    }
                    BCM_PETRA_L3_VRRP_TRANSLATE_PROTOCOL_GROUP_INTERNAL_FLAGS_TO_API(protocols, existing_flags);
                    if (existing_flags == flags) { 
                        write_entry = TRUE;
                    }
                } else { /* ARADPLUS */
                    if (((flags & BCM_L3_VRRP_IPV4) && (cam_info.is_ipv4_entry)) ||
                        ((flags & BCM_L3_VRRP_IPV6) && !(cam_info.is_ipv4_entry))) {
                        write_entry = TRUE;
                    }
                }
                
                if (write_entry) {
                    vrid_array[(*count)++] = cam_info.mac_addr.address[0];
                    if (*count == alloc_size) 
                        break;
                }
            } 
        }
        else
#endif  /* BCM_88660_A0 */        
        if (((flags & BCM_L3_VRRP_IPV4) && (enable & _BCM_DPP_VRRP_MAC_IPV4_ENABLE)) ||
            ((flags & BCM_L3_VRRP_IPV6) && (enable & _BCM_DPP_VRRP_MAC_IPV6_ENABLE)))
        {
            vrid_array[(*count)++] = vrid;
            if (*count == alloc_size) 
                break;
        }
    }

exit:
    DPP_L3_LOCK_RELEASE;
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_vrrp_config_delete
 * Purpose:
 *      Delete VRID for a particulat VLAN/VSI, for IPV4 or IPV6
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN) BCM_L3_VRRP_IPV4, BCM_L3_VRRP_IPV6
 *      vlan - (IN) VLAN/VSI
 *      vrid - (IN) VRID - Virtual router ID to be deleted
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcm_petra_l3_vrrp_config_delete(
    int unit, 
    uint32 flags, 
    bcm_vlan_t vlan, 
    uint32 vrid)
{
    int rv = BCM_E_NONE;
    int max_vrid, ipv6_distinct;
    uint8 mode;
#ifdef BCM_88660_A0
    SOC_PPC_VRRP_CAM_INFO cam_info;
#endif    /*BCM_88660_A0*/
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_L3_UNIT_INIT_CHECK;
    DPP_L3_LOCK_TAKE;

    rv = _bcm_l3_vrrp_config_verify(unit, flags, vlan, &max_vrid, &ipv6_distinct, &mode);
    BCMDNX_IF_ERR_EXIT(rv);

    if (vrid >= max_vrid) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VRID is out of range")));
    }

#ifdef BCM_88660_A0
    if (SOC_IS_ARADPLUS(unit) && mode == SOC_PPC_MYMAC_VRRP_MODE_ALL_VSI_BASED){

        SOC_PPC_VRRP_CAM_INFO_clear(&cam_info);

        cam_info.is_ipv4_entry = ipv6_distinct;
        rv = _bcm_l3_vrrp_mac_to_global_mac(unit, &cam_info, flags, vrid);
        if (rv != BCM_E_NONE) {
            BCMDNX_IF_ERR_EXIT(rv);    
        }
        
        cam_info.flags = flags;
        rv = _bcm_l3_vrrp_setting_delete(unit, -1, &cam_info, vlan);
        if (rv != BCM_E_NONE) {
            BCMDNX_IF_ERR_EXIT(rv);    
        }
    } else
#endif /* BCM_88660_A0 */
    /* add this VRID to IPV4 or IPV6, according to flags */
    if (ipv6_distinct && (flags & BCM_L3_VRRP_IPV6)) { /* IPV6 only */
        rv = _bcm_ppd_mymac_vrrp_mac_set(unit, vlan, vrid, _BCM_DPP_VRRP_MAC_DISABLE | _BCM_DPP_VRRP_MAC_IS_IPV6);
    }
    else { /* IPV4 only or shared */
        /* ipv4 is the default */
        rv = _bcm_ppd_mymac_vrrp_mac_set(unit, vlan, vrid, _BCM_DPP_VRRP_MAC_DISABLE);
    }

exit:
    DPP_L3_LOCK_RELEASE;
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_l3_vrrp_config_delete_all
 * Purpose:
 *      Delete all the VRIDs for a particular VLAN/VSI, for IPV4 or IPV6
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN) BCM_L3_VRRP_IPV4, BCM_L3_VRRP_IPV6
 *      vlan - (IN) VLAN/VSI
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcm_petra_l3_vrrp_config_delete_all(
    int unit, 
    uint32 flags, 
    bcm_vlan_t vlan)
{
    int rv = BCM_E_NONE;
    int vsi, vrid;
    int ipv6_distinct;
    uint8 mode;
    
#ifdef BCM_88660_A0
    uint8 is_vrrp;
    SOC_PPC_VRRP_CAM_INFO cam_info;
#endif  /* BCM_88660_A0 */
    
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_L3_UNIT_INIT_CHECK;

    DPP_L3_LOCK_TAKE;

    rv = _bcm_l3_vrrp_config_verify(unit, flags, vlan, &vsi, &ipv6_distinct, &mode);
    BCMDNX_IF_ERR_EXIT(rv);

#ifdef BCM_88660_A0

    /* In ARADPLUS, the vsi would represent the index in the cam table rather than max vrid. */
    if (SOC_IS_ARADPLUS(unit) && mode == SOC_PPC_MYMAC_VRRP_MODE_ALL_VSI_BASED){
        vsi = _BCM_PETRA_L3_VRRP_CAM_MAX_NOF_ENTIRES(unit);
    }
#endif /* BCM_88660_A0 */

    for (vrid = 0; vrid < vsi; vrid++) {
#ifdef BCM_88660_A0

        if (SOC_IS_ARADPLUS(unit) && mode == SOC_PPC_MYMAC_VRRP_MODE_ALL_VSI_BASED){

            /* Get entry to make sure that it's a vrrp entry and not a multiple mymac entry. */
            cam_info.vrrp_cam_index = vrid;
            rv = _bcm_l3_vrrp_setting_get(unit, &cam_info);

            if (rv == BCM_E_NOT_FOUND) { /* Not found? Doesn't matter. Try next one. */
                rv = BCM_E_NONE;
                continue;
            }

            if (!(flags & cam_info.flags)){ /*delete according to input IPv4/IPv6 flags*/
                rv = BCM_E_NONE;
                continue;
            }

            if (rv != BCM_E_NONE) {
                BCMDNX_IF_ERR_EXIT(rv);    
            }

            /* Check mac address. If the mac MSBs is not vrrp, don't delete it. */
            rv = _bcm_l3_sand_mac_is_vrrp_mac(&cam_info.mac_addr, &is_vrrp);
            if (rv != BCM_E_NONE) {
                BCMDNX_IF_ERR_EXIT(rv);    
            }

            if (!is_vrrp) {
                continue;
            }

            /* The vrid in this case would represent the entry in the cam table. */
            rv = _bcm_l3_vrrp_setting_delete(unit, vrid, NULL, vlan);
            if (rv != BCM_E_NONE && rv != BCM_E_NOT_FOUND) {
                LOG_ERROR(BSL_LS_BCM_L3,
                          (BSL_META_U(unit,
                                      "unit %d flags %d vlan %d vrid %d failed\n"), unit, flags, vlan, vrid));
                break;
            }
        } else {
#endif /* BCM_88660_A0 */
            rv = bcm_petra_l3_vrrp_config_delete(unit, flags, vlan, vrid);
            if (rv != BCM_E_NONE) {
                LOG_ERROR(BSL_LS_BCM_L3,
                          (BSL_META_U(unit,
                                      "unit %d flags %d vlan %d vrid %d failed\n"), unit, flags, vlan, vrid));
                break;
            }
#ifdef BCM_88660_A0
        }
#endif
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    DPP_L3_LOCK_RELEASE;
    BCMDNX_FUNC_RETURN;
}

#endif /* INCLUDE_L3 */ 

