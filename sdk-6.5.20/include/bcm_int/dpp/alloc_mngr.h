/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        alloc_mngr.h
 * Purpose:     Resource allocation manager for SOC_SAND chips.
 */

#ifndef  INCLUDE_ALLOC_MNGR_H
#define  INCLUDE_ALLOC_MNGR_H

#include <soc/dpp/PPD/ppd_api_general.h>
#include <soc/dpp/TMC/tmc_api_end2end_scheduler.h>
#include <soc/dpp/TMC/tmc_api_tdm.h>
#include <soc/dpp/TMC/tmc_api_cnm.h>
#include <soc/dpp/PPD/ppd_api_eg_encap.h>
#include <soc/dpp/PPD/ppd_api_oam.h>
#include <soc/dpp/PPD/ppd_api_vsi.h>
#include <soc/dpp/PPD/ppd_api_ptp.h>
#include <soc/dpp/PPC/ppc_api_mymac.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/QAX/QAX_PP/qax_pp_eg_encap_access.h>
#include <bcm_int/dpp/alloc_mngr_shr.h>
#include <bcm_int/dpp/alloc_mngr_lif.h>
#include <bcm_int/dpp/cosq.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/port.h>
#include <bcm_int/dpp/l2.h>
#include <bcm_int/dpp/l3.h>
#include <bcm_int/dpp/vlan.h>
#include <bcm_int/dpp/rx.h>
#include <bcm/rx.h>
#include <bcm/l2.h>
#include <shared/shr_template.h>
#include <shared/swstate/sw_state_resmgr.h>

#define _BCM_LAST_USER_DEFINE (SOC_PPC_TRAP_CODE_USER_DEFINED_LAST)

#define BCM_DPP_AM_TEMPLATE_FLAG_CHECK      (1)

#define BCM_DPP_AM_METER_GROUP_ANY      (2)
#define BCM_DPP_AM_METER_CORE_ANY		(SOC_DPP_CONFIG(unit)->meter.nof_meter_cores)



/* LLVP profiles. */
#define _DPP_AM_TEMPLATE_TPID_PROFILE_LOW_ID (0)
#define _DPP_AM_TEMPLATE_TPID_PROFILE_COUNT(unit) (SOC_DPP_DEFS_GET(unit, llvp_profiles_count))
#define _DPP_AM_TEMPLATE_TPID_PROFILE_MAX_ENTITIES(unit) (_BCM_PETRA_NOF_LOCAL_PORTS(unit))
#define _DPP_AM_TEMPLATE_TPID_PROFILE_SIZE   (sizeof(uint32))
/* TPID profile Encoding : AC-Key-Map-Type[15:14] , PORT-DTAG-MODE[13:12] , Tag-Type[11:8] , Accept-mode[7:0] */
#define _DPP_AM_TEMPLATE_PORT_ACC_MODE_SHIFT   (0)
#define _DPP_AM_TEMPLATE_PORT_ACC_MODE_MASK    (0xFF)

#define _DPP_AM_TEMPLATE_PORT_TAG_TYPE_SHIFT   (8)
#define _DPP_AM_TEMPLATE_PORT_TAG_TYPE_MASK    (0xF)

#define _DPP_AM_TEMPLATE_PORT_DTAG_MODE_SHIFT  (12)
#define _DPP_AM_TEMPLATE_PORT_DTAG_MODE_MASK   (0x3)

#define _DPP_AM_TEMPLATE_PORT_AC_KEY_MAP_TYPE_SHIFT (14)
#define _DPP_AM_TEMPLATE_PORT_AC_KEY_MAP_TYPE_MASK  (0x3)

/* TTL Scoping */


/* Egress action profile - number of entities pointing to it without counting the Field entries */
#define BCM_DPP_AM_TRAP_EGRESS_MAX_ENTITIES_NO_FIELD (SOC_PPC_NOF_TRAP_EG_TYPES_ARAD + 3) 

/* Egress thresh */
#define _DPP_AM_TEMPLATE_COSQ_EGR_THRESH_COUNT(unit) (16)

/* CRPS compensation profiles size */
#define _DPP_CRPS_COMPENSATION_PROFILES_SIZE SOC_IS_QAX(unit) ? (8):(16)
#define _DPP_STAT_INTERFACE_COMPENSATION_PROFILES_SIZE SOC_IS_QAX(unit) ? (8):(16)


/* Scheduler adjust size (compensation) final delta mapping  */
#define _DPP_AM_TEMPLATE_SCHEDULER_ADJUST_SIZE_FINAL_DELTA_PROFILE_LOW_ID       (0)
#define _DPP_AM_TEMPLATE_SCHEDULER_ADJUST_SIZE_FINAL_DELTA_PROFILE_COUNT        (32) /* support up to 32 different compensation values */
#define _DPP_AM_TEMPLATE_SCHEDULER_ADJUST_SIZE_FINAL_DELTA_MAX_ENTITIES         (256)
#define _DPP_AM_TEMPLATE_SCHEDULER_ADJUST_SIZE_FINAL_DELTA_CLS_SIZE             (sizeof(int))

/* IP tunnel encapsulation mode - Jericho B0, QAX and above */
#define _DPP_AM_TEMPLATE_IP_TUNNEL_ENCAPSULATION_MODE_DUMMY_TEMPLATE (4)    /* encapsulation mode dummy profile*/

typedef int (*dpp_am_cosq_flow_region_set_f)(int, int, int, int, int, int);
typedef int (*dpp_am_cosq_queue_region_set_f)(int, int, int, int, int);

typedef struct dpp_am_cosq_resource_info_s {
    dpp_am_cosq_flow_region_set_f      dpp_am_cosq_flow_region_set;
    dpp_am_cosq_queue_region_set_f     dpp_am_cosq_queue_region_set;
} dpp_am_cosq_resource_info_t;


typedef struct {
    uint32  cir;
    uint32  eir;
    uint32  max_cir;
    uint32  max_eir;    
    uint32  cbs;
    uint32  ebs;
    uint32  flags;
    uint32  mode;
    uint32  entropy_id;
    uint32  ir_rev_exp; /* relevant only for MEF mode (cascade) */
    uint32  mark_black;
} bcm_dpp_am_meter_entry_t;


typedef struct _dpp_res_pool_cosq_s {
    int dpp_res_pool_cosq_connector_cont;
    int dpp_res_pool_cosq_connector_non_cont_region_type_1;
    int dpp_res_pool_cosq_connector_non_cont_region_type_2;
    int dpp_res_pool_cosq_se_cl_fq_region_type_1;
    int dpp_res_pool_cosq_se_cl_fq_region_type_2;
    int dpp_res_pool_cosq_se_cl_hr;
    int dpp_res_pool_cosq_fq_connector_region_type_2_sync;
    int dpp_res_pool_cosq_hr_connector_region_type_2_sync;
    int dpp_res_pool_cosq_e2e_end;
    int dpp_res_pool_cosq_queue_unicast_cont;
    int dpp_res_pool_cosq_queue_unicast_non_cont;
    int dpp_res_pool_cosq_queue_multicast_cont;
    int dpp_res_pool_cosq_queue_multicast_non_cont;
    int dpp_res_pool_cosq_queue_isq_cont;
    int dpp_res_pool_cosq_queue_isq_non_cont;
    int dpp_res_pool_cosq_queue_dynamic_start;
    int dpp_res_pool_cosq_queue_dynamic_end;
} _dpp_res_pool_cosq_t;

#define _BCM_DPP_AM_LIF_NOF_BANKS (16) /* Number of banks 64K/4K */

typedef struct bcm_dpp_am_cosq_quad_allocation_s {
    int flow1;
    int flow2;
    int flow3;
    int flow4;
}bcm_dpp_am_cosq_quad_allocation_t;

typedef struct _dpp_res_type_cosq_s {
    int dpp_res_type_cosq_connector_cont;
    int dpp_res_type_cosq_connector_non_cont_region_type_1;
    int dpp_res_type_cosq_connector_non_cont_region_type_2;
    int dpp_res_type_cosq_se_cl_fq_region_type_1;
    int dpp_res_type_cosq_se_cl_fq_region_type_2;
    int dpp_res_type_cosq_se_cl_hr;
    int dpp_res_type_cosq_fq_connector_region_type_2_sync;
    int dpp_res_type_cosq_hr_connector_region_type_2_sync;
    int dpp_res_type_cosq_e2e_end;
    int dpp_res_type_cosq_queue_unicast_cont;
    int dpp_res_type_cosq_queue_unicast_non_cont;
    int dpp_res_type_cosq_queue_multicast_cont;
    int dpp_res_type_cosq_queue_multicast_non_cont;
    int dpp_res_type_cosq_queue_isq_cont;
    int dpp_res_type_cosq_queue_isq_non_cont;
    int dpp_res_type_cosq_queue_dynamic_start;
    int dpp_res_type_cosq_queue_dynamic_end;
} _dpp_res_type_cosq_t;

/* allocation mngr structs */
/* Egress encapsulation start definiations */
typedef struct bcm_dpp_am_egress_encap_s {
  SHR_BITDCL             *egress_encap_banks;  /* Bitmap of existing Egress Encapsulation banks */
  uint8                   egress_encap_count;   /* Number of existing Egress Encapsulation banks */
  uint8                   init; /* Indicate egress encap was initialized */
} bcm_dpp_am_egress_encap_t;

/* Ingress LIF start definitions */
typedef struct bcm_dpp_am_ingress_lif_s {
  SHR_BITDCL             *ingress_lif_banks;  /* Bitmap of existing Ingress LIF banks */
  uint8                   ingress_lif_count;   /* Number of existing Ingress LIF banks */
  uint8                   init; /* Indicate ingress lif was initialized */
} bcm_dpp_am_ingress_lif_t;

typedef struct bcm_dpp_am_sync_lif_s {
  SHR_BITDCL           *sync_lif_banks;  /* Bitmap of sync LIF banks */
  uint8                 init; /* Indicate sync lif was initialized */
} bcm_dpp_am_sync_lif_t;

typedef struct {
    uint8 fec_bank_groups[SOC_DPP_DEFS_MAX(NOF_FEC_BANKS)];
    PARSER_HINT_ARR int *resource_to_pool_map;
    PARSER_HINT_ARR int *template_to_pool_map;
    bcm_dpp_am_egress_encap_t egress_encap;
    bcm_dpp_am_ingress_lif_t  ingress_lif;
    bcm_dpp_am_sync_lif_t     sync_lif;
    int egr_thresh_map[SOC_DPP_DEFS_MAX(NOF_LOGICAL_PORTS)];
    int egr_interface_unicast_thresh_map[SOC_DPP_IMP_DEFS_MAX(NOF_CORE_INTERFACES)*SOC_DPP_DEFS_MAX(NOF_CORES)];
    int egr_interface_multicast_thresh_map[SOC_DPP_IMP_DEFS_MAX(NOF_CORE_INTERFACES)*SOC_DPP_DEFS_MAX(NOF_CORES)];
#ifdef BCM_88470_A0
    SHR_BITDCL fec_is_cascaded[_SHR_BITDCLSIZE(SOC_DPP_DEFS_MAX(NOF_FECS))];
#endif /*BCM_88470_A0*/
} bcm_dpp_alloc_mngr_info_t;

int
bcm_dpp_am_attach(
    int unit);

int
bcm_dpp_am_clear(
    int unit);

int
bcm_dpp_am_detach(
    int unit);

int
bcm_dpp_am_vlan_vsi_alloc(
    int unit,
    bcm_vlan_t vid,
    SOC_PPC_VSI_ID *vsi,
    SOC_PPC_LIF_ID *lif);

int
bcm_dpp_am_vlan_vsi_dealloc(
    int unit,
    bcm_vlan_t vid,
    SOC_PPC_VSI_ID vsi,
    SOC_PPC_LIF_ID lif);


 /*
  * L2 VPN
  */
int
bcm_dpp_am_l2_vpn_vsi_alloc(
        int unit,
        uint32 flags,
        bcm_vlan_t *vsi);
int
bcm_dpp_am_l2_vpn_vsi_dealloc(
        int unit,
        bcm_vlan_t vsi);
int
  bcm_dpp_am_l2_vpn_vsi_is_alloced(
      int unit,
      bcm_vlan_t vsi);

#define _BCM_DPP_AM_L2_VPN_AC_TYPE_DEFAULT           (0)
#define _BCM_DPP_AM_L2_VPN_AC_TYPE_PON_3_TAGS_DATA   (1)
#define _BCM_DPP_AM_L2_VPN_AC_TYPE_PON_3_TAGS_OUT_AC (2)
#ifdef BCM_88660_A0
#define _BCM_DPP_AM_L2_VPN_AC_TYPE_BIG_OUT_AC        (3)
#endif

int
bcm_dpp_am_l2_vpn_ac_alloc(
        int unit,
        uint32 types,
        uint32 flags,
        SOC_PPC_LIF_ID *lif);

int
bcm_dpp_am_l2_vpn_ac_dealloc(
        int unit,
        uint32 types,
        uint32 flags,
        SOC_PPC_LIF_ID lif);

int
bcm_dpp_trill_lif_alloc(
        int unit,
        uint32 flags,
        SOC_PPC_LIF_ID *lif);
int
bcm_dpp_trill_lif_is_alloced(
        int unit,
        SOC_PPC_LIF_ID lif);
int
bcm_dpp_trill_lif_dealloc(
        int unit,
        uint32 flags,
        SOC_PPC_LIF_ID lif);


#define _BCM_DPP_AM_L2_AC_TYPE_VLAN_VSI              (0)
#define _BCM_DPP_AM_L2_AC_TYPE_VLAN_EDITING          (1)
#define _BCM_DPP_AM_L2_AC_TYPE_ISID                  (2)
#define _BCM_DPP_AM_L2_AC_TYPE_INITIAL_VLAN          (3)
#define _BCM_DPP_AM_L2_AC_TYPE_DOUBLE                (4)
int
bcm_dpp_am_l2_ac_alloc(
        int unit,
        uint32 types,
        uint32 flags,
        SOC_PPC_LIF_ID *lif);

int
bcm_dpp_am_l2_ac_dealloc(
        int unit,
        uint32 lif_type,
        SOC_PPC_LIF_ID lif);

int
bcm_dpp_am_l2_ac_is_alloced(
        int unit,
        SOC_PPC_AC_ID out_ac);

int
bcm_dpp_am_out_ac_dealloc(
        int unit,
        uint32 types,
        uint32 flags,
        SOC_PPC_AC_ID out_ac);

int
bcm_dpp_am_out_ac_is_alloced(
        int unit,
        SOC_PPC_AC_ID out_ac);

/* DATA, Mirror , ERSPAN */
int
bcm_dpp_am_eg_data_erspan_alloc(
        int unit,
        uint32 flags,
        uint32 *eg_data_erspan);

int
bcm_dpp_am_eg_data_erspan_dealloc(
        int unit,
        uint32 flags,
        uint32 eg_data_erspan);

int
bcm_dpp_am_eg_data_erspan_is_alloced(
        int unit,
        uint32 eg_data_erspan);

int
bcm_dpp_am_ipv6_tunnel_alloc(
        int unit,
        uint32 flags,
        uint32 *outlif);

int
bcm_dpp_am_ipv6_tunnel_dealloc(
        int unit,
        uint32 flags,
        uint32 ipv6_tunnel);

int
bcm_dpp_am_ipv6_tunnel_is_alloced(
        int unit,
        uint32 ipv6_tunnel);

extern int
bcm_dpp_am_qos_ing_elsp_dealloc(
        int unit,
        uint32 flags,
        int qos_id);

extern int
bcm_dpp_am_qos_ing_elsp_is_alloced(
        int unit,
        int qos_id);

extern int
bcm_dpp_am_qos_ing_lif_cos_dealloc(
        int unit,
        uint32 flags,
        int qos_id);

extern int
bcm_dpp_am_qos_ing_lif_cos_is_alloced(
        int unit,
        int qos_id);

extern int
bcm_dpp_am_qos_ing_pcp_vlan_dealloc(
        int unit,
        uint32 flags,
        int qos_id);

extern int
bcm_dpp_am_qos_ing_pcp_vlan_is_alloced(
        int unit,
        int qos_id);

extern int
bcm_dpp_am_qos_ing_color_dealloc(
        int unit,
        uint32 flags,
        int qos_id);

extern int
bcm_dpp_am_qos_ing_color_is_alloced(
        int unit,
        int qos_id);

extern int
bcm_dpp_am_qos_egr_remark_id_dealloc(
        int unit,
        uint32 flags,
        int qos_id);

extern int
bcm_dpp_am_qos_egr_remark_id_is_alloced(
        int unit,
        int qos_id);

extern int
bcm_dpp_am_qos_egr_mpls_php_id_dealloc(
        int unit,
        uint32 flags,
        int qos_id);

extern int
bcm_dpp_am_qos_egr_mpls_php_id_is_alloced(
        int unit,
        int qos_id);

extern int
bcm_dpp_am_qos_egr_pcp_vlan_dealloc(
        int unit,
        uint32 flags,
        int qos_id);

extern int
bcm_dpp_am_qos_egr_pcp_vlan_is_alloced(
        int unit,
        int qos_id);

extern int
bcm_dpp_am_qos_ing_cos_opcode_dealloc(
        int unit,
        uint32 flags,
        int qos_id);

extern int
bcm_dpp_am_qos_ing_cos_opcode_is_alloced(
        int unit,
        int qos_id);

extern int
bcm_dpp_am_qos_egr_l2_i_tag_dealloc(
        int unit,
        uint32 flags,
        int qos_id);

extern int
bcm_dpp_am_qos_egr_l2_i_tag_is_alloced(
        int unit,
        int qos_id);

#ifdef BCM_88660
extern int
bcm_dpp_am_qos_egr_dscp_exp_marking_dealloc(
        int unit,
        uint32 flags,
        int qos_id);

extern int
bcm_dpp_am_qos_egr_dscp_exp_marking_is_alloced(
        int unit,
        int qos_id);
#endif /* BCM_88660 */

extern int
bcm_dpp_am_meter_dealloc(
        int unit,
		int core,
        uint32 flags,
        int meter_group,
        int nof_meters,
        int meter_id);

extern int
bcm_dpp_am_meter_is_alloced(
        int unit,
		int core,
        int meter_group,
        int nof_meters,
        int meter_id);


#ifdef BCM_ARAD_SUPPORT

int
bcm_dpp_am_ecmp_alloc(
    int unit,
    uint32 flags,
    bcm_ecmp_t *ecmp_id);

int
bcm_dpp_am_ecmp_dealloc(
    int unit,
    bcm_ecmp_t ecmp_id);

int
bcm_dpp_am_ecmp_is_alloced(
    int unit,
    bcm_ecmp_t ecmp_id);

#endif /* BCM_ARAD_SUPPORT */



int
bcm_dpp_am_rp_alloc(
    int unit,
    uint32 flags,
    int *rp_id);

int
bcm_dpp_am_rp_dealloc(
    int unit,
    int rp_id);


int
bcm_dpp_am_rp_is_alloced(
    int unit,
    int rp_id);


int
bcm_dpp_am_failover_dealloc(
    int unit,
    int32 failover_type,
    int32 nof_dealloc_ids,
    bcm_failover_t failover_id);

int
bcm_dpp_am_failover_is_alloced(
    int unit,
    int32 failover_type,
    bcm_failover_t failover_id);

#define _BCM_DPP_AM_L2_PWE_TERM_DEFAULT        (0) /* Default */
#define _BCM_DPP_AM_L2_PWE_TERM_INDEXED_1      (2) /* Indexed 1 */
#define _BCM_DPP_AM_L2_PWE_TERM_INDEXED_2      (3) /* Indexed 2 */
#define _BCM_DPP_AM_L2_PWE_TERM_INDEXED_3      (4) /* Indexed 3 */
#define _BCM_DPP_AM_L2_PWE_TERM_INDEXED_1_2    (5) /* Indexed 1,2 */
#define _BCM_DPP_AM_L2_PWE_TERM_INDEXED_1_3    (6) /* Indexed 1,3 */


int
bcm_dpp_am_l2_vpn_pwe_lif_dealloc(
        int unit,
        SOC_PPC_LIF_ID lif);

int
bcm_dpp_am_l2_vpn_pwe_eep_dealloc(
        int unit,
        uint32 eep);

int
bcm_dpp_am_mc_dealloc(
        int unit,
        SOC_TMC_MULT_ID mc_id,
        uint8 is_egress);

int
bcm_dpp_am_mc_is_alloced(
        int unit,
        SOC_TMC_MULT_ID mc_id,
        uint8 is_egress);

/* 
 * OAM
 */

/* OAM MEP entry type */
#define BCM_DPP_AM_OAM_MEP_DB_ENTRY_TYPE_MEP (1)

int
bcm_dpp_am_oam_ma_id_alloc(
        int unit,
        uint32 flags, /* flags may be SW_STATE_RES_ALLOC_WITH_ID or 0; */
        uint32  *ma_id);

int
bcm_dpp_am_oam_ma_id_dealloc(
        int unit,
        uint32 ma_id);

int
bcm_dpp_am_oam_ma_id_is_alloced(
        int unit,
        uint32 ma_id);



int
bcm_dpp_am_oam_mep_id_short_alloc(
        int unit,
        uint32 flags, /* flags may be SW_STATE_RES_ALLOC_WITH_ID or 0; */
        uint32  *mep_id);

int
bcm_dpp_am_oam_mep_id_short_dealloc(
        int unit,
        uint32 mep_id);

int
bcm_dpp_am_oam_mep_id_short_is_alloced(
        int unit,
        uint32 mep_id);

int
bcm_dpp_am_oam_mep_id_long_alloc_non_48_maid(
        int unit,
        uint32 flags, /* flags may be SW_STATE_RES_ALLOC_WITH_ID or 0; */
        uint32  *mep_id);

int
bcm_dpp_am_oam_mep_id_long_dealloc_non_48_maid(
        int unit,
        uint32 mep_id);

int
bcm_dpp_am_oam_mep_id_long_is_alloced_non_48_maid(
        int unit,
        uint32 mep_id);

int
bcm_dpp_am_oam_mep_id_long_alloc_48_maid(
        int unit,
        uint32 flags, /* flags may be SW_STATE_RES_ALLOC_WITH_ID or 0; */
        uint32  *mep_id);

int
bcm_dpp_am_oam_mep_id_long_dealloc_48_maid(
        int unit,
        uint32 mep_id);

int
bcm_dpp_am_oam_mep_id_long_is_alloced_48_maid(
        int unit,
        uint32 mep_id);

int
bcm_dpp_am_oam_mep_id_long_alloc(
        int unit,
        uint32 flags, /* flags may be SW_STATE_RES_ALLOC_WITH_ID or 0; */
        uint32  *mep_id);

int
bcm_dpp_am_oam_mep_id_long_dealloc(
        int unit,
        uint32 mep_id);

int
bcm_dpp_am_oam_mep_id_long_is_alloced(
        int unit,
        uint32 mep_id);

int
bcm_dpp_am_qax_oam_mep_id_alloc(
        int unit,
        uint32 flags, /* flags may be SW_STATE_RES_ALLOC_WITH_ID or 0; */
        uint8 type,
        uint32  *mep_id);

int
bcm_dpp_am_qax_oam_mep_id_dealloc(
        int unit,
        uint32 mep_id);

int
bcm_dpp_am_qax_oam_mep_id_is_alloced(
        int unit,
        uint32 mep_id);

int
bcm_dpp_am_oam_rmep_id_alloc(
        int unit,
        uint32 flags, /* flags may be SW_STATE_RES_ALLOC_WITH_ID or 0; */
        uint32 *rmep_id);

int
bcm_dpp_am_oam_rmep_id_dealloc(
        int unit,
        uint32 rmep_id);

int
bcm_dpp_am_oam_rmep_id_is_alloced(
        int unit,
        uint32 rmep_id);

int
bcm_dpp_am_oam_mep_id_jumbo_tlv_alloc(
        int unit,
        uint32 flags, /* flags may be SW_STATE_RES_ALLOC_WITH_ID or 0; */
        uint32  *mep_id);

int
bcm_dpp_am_oam_mep_id_jumbo_tlv_dealloc(
        int unit,
        uint32 mep_id);

int
bcm_dpp_am_oam_mep_id_jumbo_tlv_is_alloced(
        int unit,
        uint32 mep_id);


int
bcm_dpp_am_oam_mep_id_no_jumbo_tlv_alloc(
        int unit,
        uint32 flags, /* flags may be SW_STATE_RES_ALLOC_WITH_ID or 0; */
        uint32  *mep_id);

int
bcm_dpp_am_oam_mep_id_no_jumbo_tlv_dealloc(
        int unit,
        uint32 mep_id);

int
bcm_dpp_am_oam_mep_id_no_jumbo_tlv_is_alloced(
        int unit,
        uint32 mep_id);

int
bcm_dpp_am_oam_trap_code_upmep_ftmh_header_alloc(
        int unit,
        uint32 flags, /* flags may be SW_STATE_RES_ALLOC_WITH_ID or 0; */
        uint32 *trap_code);

int
bcm_dpp_am_oam_trap_code_upmep_ftmh_header_dealloc(
        int unit,
        uint32 trap_code);

/*
 *Returns: 
 *      BCM_E_NOT_FOUND if the element is not in use
 *      BCM_E_EXISTS if the element is in use
 *      BCM_E_PARAM if the element is not valid
 *      BCM_E_* as appropriate otherwise
 */
int
bcm_dpp_am_oam_sd_sf_id_alloc(
        int unit,
        uint32 flags, 
        uint32  *sd_sf_id);

int
bcm_dpp_am_oam_sd_sf_id_dealloc(
        int unit,
        uint32 sd_sf_id);

int
bcm_dpp_am_oam_sd_sf_id_is_alloced(
        int unit,
        uint32 sd_sf_id);

int
bcm_dpp_am_oam_y_1711_sd_sf_id_alloc(
        int unit,
        uint32 flags, 
        uint32  *y_1711_sd_sf_id);

int
bcm_dpp_am_oam_y_1711_sd_sf_id_dealloc(
        int unit,
        uint32 y_1711_sd_sf_id);

int
bcm_dpp_am_oam_y_1711_sd_sf_id_is_alloced(
        int unit,
        uint32 y_1711_sd_sf_id);

int _bcm_dpp_am_template_oam_sd_sf_profile_alloc(
    int unit,
    int flags,
    const SOC_PPC_OAM_OAMP_SD_SF_PROFILE_DB *data, 
    int *is_allocated,
    int *profile_ndx);

int _bcm_dpp_am_template_oam_sd_sf_profile_free(
    int unit, 
    int profile_ndx, 
    int *is_last);

int _bcm_dpp_am_template_oam_sd_sf_profile_data_get(
    int unit, 
    int profile_ndx, 
    SOC_PPC_OAM_OAMP_SD_SF_PROFILE_DB* data);


/* OAM - End */

/* 
 * L3
 */
#define _BCM_DPP_AM_L3_LIF_MPLS_TERM                (0) /* MPLS termination Default */
#define _BCM_DPP_AM_L3_LIF_IP_TERM                  (1)
#define _BCM_DPP_AM_L3_LIF_MPLS_TERM_INDEXED_1      (2) /* Indexed 1 */
#define _BCM_DPP_AM_L3_LIF_MPLS_TERM_INDEXED_2      (3) /* Indexed 2 */
#define _BCM_DPP_AM_L3_LIF_MPLS_TERM_INDEXED_3      (4) /* Indexed 3 */
#define _BCM_DPP_AM_L3_LIF_MPLS_TERM_INDEXED_1_2    (5) /* Indexed 1,2 */
#define _BCM_DPP_AM_L3_LIF_MPLS_TERM_INDEXED_1_3    (6) /* Indexed 1,2 */
#define _BCM_DPP_AM_L3_LIF_MPLS_TERM_FRR            (7) /* FRR entry */
#define _BCM_DPP_AM_L3_LIF_MPLS_TERM_EXPLICIT_NULL  (8) /* Explicit NULL (label 0 and 2) */
#define _BCM_DPP_AM_L3_LIF_MPLS_TERM_DUMMY          (9) /* Dummy termination for mLDP*/


int
bcm_dpp_am_l3_lif_type_to_app_type(uint32 type, bcm_dpp_am_ingress_lif_app_t *app_type);

int
bcm_dpp_am_l3_lif_alloc(
        int unit,
        uint32 types,
        uint32 flags,
        int *lif);

int
bcm_dpp_am_l3_lif_is_allocated(
        int unit,
        uint32 types,
        uint32 flags,
        int lif);



int
bcm_dpp_am_l3_lif_dealloc(
        int unit,
        int lif);
int
bcm_dpp_am_l3_eep_dealloc(
        int unit,
        int eep);

int
bcm_dpp_am_l3_eep_is_alloced(
        int unit,
        int eep);


int
bcm_dpp_am_l3_lif_ip_tnl_dealloc(
        int unit,
        SOC_PPC_LIF_ID lif);



int
bcm_dpp_am_ip_tunnel_eep_is_alloced(
        int unit,
        uint32 flags,
        int eep);


int
bcm_dpp_am_ip_tunnel_eep_dealloc(
        int unit,
        uint32 flags,
        int eep);

int
bcm_dpp_am_mpls_eep_is_alloced(
        int unit,
        int eep);


int
bcm_dpp_am_mpls_eep_dealloc(
        int unit,
        int eep);

int
bcm_dpp_am_ip_tunnel_glbl_src_ip_is_alloced(
        int unit,
        int idx);


int
bcm_dpp_am_ip_tunnel_glbl_src_ip_dealloc(
        int unit,
        int idx);

int
bcm_dpp_am_ip_tunnel_glbl_ttl_is_alloced(
        int unit,
        int idx);


int
bcm_dpp_am_ip_tunnel_glbl_ttl_dealloc(
        int unit,
        int idx);


int
bcm_dpp_am_ip_tunnel_glbl_tos_is_alloced(
        int unit,
        int idx);


int
bcm_dpp_am_ip_tunnel_glbl_tos_dealloc(
        int unit,
        int idx);

/* Get a simple indexing for egress traps */
int
_bcm_dpp_am_eg_trap_index_get(
        int unit,
        SOC_PPC_TRAP_EG_TYPE trap_id, /* from the enum */
        uint32 *trap_index); /* 0...NOF_TRAPS */


int _bcm_dpp_am_template_trap_egress_init(
        int unit,
        int template_init_id,
        SOC_SAND_OUT SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO *init_data);

int _bcm_dpp_am_trap_dealloc(
        int unit,
        int flags,
        int trap_id,
        int elem);

int
_bcm_dpp_am_snoop_dealloc(
        int unit,
        int flags,
        int snoop_cmnd, 
        int elem);


int _bcm_dpp_am_trap_is_alloced(
        int unit,
        int trap_id);

int
_bcm_dpp_am_snoop_is_alloced(
        int unit,
        int snoop_cmnd);


int _bcm_dpp_am_template_trap_egress_data_get(
        int unit,
        SOC_PPC_TRAP_EG_TYPE trap_id, /* Enum trap */
        uint32 egress_trap_entry, /* Egress Trap or Field Entry */                                              
        SOC_SAND_OUT SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO *data);

extern int
_bcm_dpp_am_template_trap_egress_free(int unit,
                                      int template);

int _bcm_dpp_am_template_trap_egress_exchange(
        int unit,
        int flags,
        SOC_PPC_TRAP_EG_TYPE trap_id, /* Enum trap */
        uint32 egress_trap_entry, /* Egress Trap or Field Entry */
        SOC_SAND_IN SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO *data,
        int *old_template,
        int *is_last,
        int *template,
        int *is_allocated);


int
_bcm_dpp_am_template_trap_egress_profile_mapping_get(
        int unit,
        SOC_PPC_TRAP_EG_TYPE trap_id, /* Enum trap */
        uint32 egress_trap_entry, /* Egress Trap or Field Entry */
        SOC_SAND_OUT uint32 *profile);

/*
 * l2 vpn
 */
int
bcm_dpp_am_voq_connector_alloc(
        int unit,
        uint32 flags,
        SOC_TMC_SCH_FLOW_ID *flow_id);

int
bcm_dpp_am_voq_connector_dealloc(
        int unit,
        uint32 flags,
        SOC_TMC_SCH_FLOW_ID flow_id);

int
bcm_dpp_am_queue_alloc(
        int unit,
        uint32 flags,
        uint32 *queue_id);

int
bcm_dpp_am_queue_dealloc(
        int unit,
        uint32 flags,
        uint32 queue_id);

int
bcm_dpp_am_fp_db_alloc(
        int unit,
        uint32 flags,
        uint32 *db_id);

int
bcm_dpp_am_fp_db_dealloc(
        int unit,
        uint32 flags,
        uint32 db_id);

int
bcm_dpp_am_fp_range_checker_alloc(
        int unit,
        uint32 flags,
        uint32 *rc_id);

int
bcm_dpp_am_fp_range_checker_dealloc(
        int unit,
        uint32 flags,
        uint32 rc_id);
int
bcm_dpp_am_vsq_src_port_alloc(
    int unit,
    int core_id,
    uint32 flags,
    int src_pp_port,
    int *vsq_index);
int
bcm_dpp_am_vsq_src_port_free(
    int unit,
    int core_id,
    int vsq_index);
int
bcm_dpp_am_pg_init(
   int unit
   );
int
bcm_dpp_am_pg_alloc(
    int unit,
    int core_id,
    uint32 flags,
    int numq,
    int *pg_base);
int
bcm_dpp_am_pg_check(
    int unit, 
    int core_id, 
    int pg_base);
int
bcm_dpp_am_pg_get(
    int unit,
    int core_id,
    int pg_base,
    int *numq
    );
int
bcm_dpp_am_pg_free(
    int unit,
    int core_id,
    int pg_base);
/* 
 * Template management - Start
 */ 
/* Cosq Egress Thresh - Start */
  /* 
   * Port level drop & fc thresholds type
   * pointed by tm-port,
   * used by bcm_petra_cosq_threshold_set
   */
int _bcm_dpp_am_template_cosq_egr_thresh_init(int unit,
                                              int core,
                                              int template_init_id,
                                              bcm_dpp_cosq_egress_thresh_key_info_t *egr_thresh_data_init);
int _bcm_dpp_am_template_cosq_egr_thresh_data_get(int unit,
                                                  int core,
                                                  int port,
                                                  bcm_dpp_cosq_egress_thresh_key_info_t * data);
int _bcm_dpp_am_template_cosq_egr_thresh_exchange(int unit, 
                                                 int core,
                                                 int port, 
                                                 bcm_dpp_cosq_egress_thresh_key_info_t * data, 
                                                 int *old_template,
                                                 int *is_last, 
                                                 int *template,
                                                 int *is_allocated);
int _bcm_dpp_am_template_cosq_egr_thresh_ref_get(int unit, int template, uint32 *ref_count);

int _bcm_dpp_am_template_cosq_egr_thresh_allocate_group(int unit, uint32 flags, bcm_dpp_cosq_egress_thresh_key_info_t *data, int ref_count, int *is_allocated, int *template);

int _bcm_dpp_am_template_cosq_egr_thresh_tdata_get(int unit, int template, bcm_dpp_cosq_egress_thresh_key_info_t *data);

/* Cosq Egress Thresh - End */

/* Cosq Egress Interface Unicast Thresh - Start */
  /* 
        * Port level drop & fc thresholds type ?
        * pointed by tm-port, ?
        * used by bcm_petra_cosq_threshold_set ?
   */
int _bcm_dpp_am_template_cosq_egr_interface_unicast_thresh_init(int unit,
                                                                int core,
                                              int template_init_id,
                                              bcm_dpp_cosq_egress_interface_unicast_thresh_key_info_t *egr_interface_unicast_thresh_data_init);
int _bcm_dpp_am_template_cosq_egr_interface_unicast_thresh_data_get(int unit,
                                                                    int core,
                                                  int interface_id,
                                                  bcm_dpp_cosq_egress_interface_unicast_thresh_key_info_t * data);
int _bcm_dpp_am_template_cosq_egr_interface_unicast_thresh_exchange(int unit, 
                                                                    int core, 
                                                 int interface_id, 
                                                 bcm_dpp_cosq_egress_interface_unicast_thresh_key_info_t * data, 
                                                 int *old_template,
                                                 int *is_last, 
                                                 int *template,
                                                 int *is_allocated);
/* Cosq Egress Interface Unicast Thresh - End */

/* Cosq Egress Interface Multicast Thresh - Start */
  /* 
        * Port level drop & fc thresholds type ?
        * pointed by tm-port, ?
        * used by bcm_petra_cosq_threshold_set ?
   */
int _bcm_dpp_am_template_cosq_egr_interface_multicast_thresh_init(int unit,
                                                                  int core,
                                              int template_init_id,
                                              bcm_dpp_cosq_egress_interface_multicast_thresh_key_info_t *egr_interface_multicast_thresh_data_init);
int _bcm_dpp_am_template_cosq_egr_interface_multicast_thresh_data_get(int unit,
                                                                      int core,
                                                  int interface_id,
                                                  bcm_dpp_cosq_egress_interface_multicast_thresh_key_info_t * data);
int _bcm_dpp_am_template_cosq_egr_interface_multicast_thresh_exchange(int unit, 
                                                                      int core,
                                                 int interface_id, 
                                                 bcm_dpp_cosq_egress_interface_multicast_thresh_key_info_t * data, 
                                                 int *old_template,
                                                 int *is_last, 
                                                 int *template,
                                                 int *is_allocated);
/* Cosq Egress Interface Multicast Thresh - End */

/* mirror action profile. Data represents CPU-trap-code*/
/* In cases where CPU-trap-code is not used the flag
   SHR_TEMPLATE_MANAGE_IGNORE_DATA should set and a new profile will be allocated (if available).
   In such cases data, is_allocated may be NULL.*/
int _bcm_dpp_am_template_mirror_action_profile_alloc(int unit, int flags, uint32 *data, int *is_allocated, int *profile);
int _bcm_dpp_am_template_mirror_action_profile_free(int unit, int profile, int *is_last);
int _bcm_dpp_am_template_mirror_action_profile_get(int unit, int mirror_profile, uint32 *data);
int _bcm_dpp_am_template_mirror_action_profile_ref_get(int unit, int profile, uint32 *ref_count);

/* User defined trap - Start */
  /*
   * user defined traps, (60 traps) 
   * considering only destination info 
   */

int _bcm_dpp_am_template_user_defined_traps_init(int unit,
                                          int template_init_id,
                                          const bcm_dpp_user_defined_traps_t *data);

int _bcm_dpp_am_template_user_defined_trap_free(int unit, 
                                                int port, 
                                                int* is_last, 
                                                int *user_defined_trap);
int _bcm_dpp_am_template_user_defined_trap_exchange(int unit,
                                                    int port,
                                                    bcm_dpp_user_defined_traps_t * data,
                                                    int *old_template,
                                                    int *is_last,
                                                    int *template,
                                                    int *is_allocated);

/* User defined trap - End */

/* Snoop Command - Start */

/* Set mapping - since get mapping it is SW check */ 
int _bcm_dpp_am_template_snoop_cmd_init(int unit,int template_init_id,const bcm_dpp_snoop_t* data);
                                          
int _bcm_dpp_am_template_snoop_cmd_allocate(int unit,
                                            uint32 flags,
                                            int port,
                                            const bcm_dpp_snoop_t* data,
                                            int *is_allocated,
                                            int *snoop_cmd);
int _bcm_dpp_am_template_snoop_cmd_free(int unit,
                                        int port,
                                        int *is_last,
                                        int *snoop_cmd);
int _bcm_dpp_am_template_snoop_cmd_exchange(int unit,
                                            int port,
                                            bcm_dpp_snoop_t * data,
                                            int *old_snoop_cmd,
                                            int *is_last,
                                            int *new_snoop_cmd,
                                            int *is_allocated);
int _bcm_dpp_am_template_snoop_cmd_data_get(int unit,
                                            int port,
                                            bcm_dpp_snoop_t * data);
/* Snoop Command - End */

/* TPID profile - Start */
int _bcm_dpp_am_template_tpid_profile_init(int unit,
                                           int template_init_id,
                                           _bcm_petra_tpid_profile_t profile_type, int accept_mode, _bcm_petra_dtag_mode_t dtag_mode, _bcm_petra_ac_key_map_type_t ac_key_map_type);

int _bcm_dpp_am_template_tpid_profile_data_get(int unit,
                                               int pp_port,
                                               int core,
                                               _bcm_petra_tpid_profile_t *tpid_profile, int *accept_mode, _bcm_petra_dtag_mode_t* dtag_mode, _bcm_petra_ac_key_map_type_t *ac_key_map_type);

int _bcm_dpp_am_template_tpid_profile_allocate(int unit,
                                               uint32 flags,
                                               int port,
                                               _bcm_petra_tpid_profile_t profile_type, int accept_mode, _bcm_petra_dtag_mode_t dtag_mode, _bcm_petra_ac_key_map_type_t ac_key_map_type,
                                               int *is_allocated,
                                               int *tpid_profile);

int _bcm_dpp_am_template_tpid_profile_free(int unit,
                                           int pp_port,
                                           int core,
                                           int *is_last,
                                           int *tpid_profile);

int _bcm_dpp_am_template_tpid_profile_exchange(int unit,
                                               int pp_port,
                                               int core,
                                               _bcm_petra_tpid_profile_t profile_type, int accept_mode, _bcm_petra_dtag_mode_t dtag_mode, _bcm_petra_ac_key_map_type_t ac_key_map_type, 
                                               int *old_tpid_profile,
                                               int *is_last,
                                               int *new_tpid_profile,
                                               int *is_allocated);

int _bcm_dpp_am_template_tpid_profile_ref_get(int unit, int template, uint32 *ref_count);

int _bcm_dpp_am_template_tpid_profile_tdata_get(int unit, int template, uint32 *data);


/* TPID profile - End */

/* COSQ Egress queue mapping - Start */
int _bcm_dpp_am_template_egress_queue_mapping_init(int unit,
                                                   int template_init_id,
                                                   int core,
                                                   const bcm_dpp_cosq_egress_queue_mapping_info_t* mapping_profile);

int _bcm_dpp_am_template_egress_queue_mapping_data_get(int unit,
                                                       uint32 tm_port,
                                                       int core,
                                                       bcm_dpp_cosq_egress_queue_mapping_info_t * mapping_profile);

int _bcm_dpp_am_template_egress_queue_mapping_exchange(int unit, 
                                                       uint32 tm_port, 
                                                       int core,
                                                       CONST bcm_dpp_cosq_egress_queue_mapping_info_t * mapping_profile, 
                                                       int *old_profile, 
                                                       int *is_last, 
                                                       int *new_profile,
                                                       int *is_allocated);

/* COSQ Egress queue mapping - End */

/* NRDY threshold - Start */
int _bcm_dpp_am_template_nrdy_threshold_alloc(int unit, int core, int flags, uint32 *data, int *is_allocated, int *profile);

int _bcm_dpp_am_template_nrdy_threshold_free(int unit, int core, int profile, int *is_last);

int _bcm_dpp_am_template_nrdy_threshold_exchange(int unit, int core, uint32 tm_port, int flags, uint32 *data,
                                             int *old_profile, int *is_last, int *profile, int *is_allocated);

/* NRDY threshold - End */

/* VLAN Edit Profile mapping Ingress & Egress - Start */
int _bcm_dpp_am_template_vlan_edit_profile_mapping_init(int unit,
                                                        int template_init_id,
                                                        const bcm_dpp_vlan_edit_profile_mapping_info_t *mapping_profile);

int _bcm_dpp_am_template_vlan_edit_profile_mapping_data_get(int unit,
                                                            SOC_PPC_LIF_ID lif_id, 
                                                            bcm_dpp_vlan_edit_profile_mapping_info_t *mapping_profile);

int _bcm_dpp_am_template_vlan_edit_profile_mapping_data_get_by_template(int unit,
                                                                           int template_id, 
                                                                           bcm_dpp_vlan_edit_profile_mapping_info_t *mapping_profile);

int _bcm_dpp_am_template_vlan_edit_profile_mapping_exchange(int unit, 
                                                            SOC_PPC_LIF_ID lif_id, 
                                                            bcm_dpp_vlan_edit_profile_mapping_info_t *mapping_profile, 
                                                            int *old_profile, 
                                                            int *is_last, 
                                                            int *new_profile,
                                                            int *is_allocated);

int _bcm_dpp_am_template_vlan_edit_profile_mapping_dummy_allocate(int unit, 
                                                            uint32 flags, 
                                                            bcm_dpp_vlan_edit_profile_mapping_info_t *mapping_profile, 
                                                            int *template_id, 
                                                            int *is_allocated);

int _bcm_dpp_am_template_vlan_edit_profile_mapping_free(int unit, SOC_PPC_LIF_ID lif_id, int *old_template,int *is_last);

int _bcm_dpp_am_template_vlan_edit_profile_eg_mapping_init(int unit,
                                                           int template_init_id,
                                                           const bcm_dpp_vlan_egress_edit_profile_info_t *mapping_profile);

int _bcm_dpp_am_template_vlan_edit_profile_eg_mapping_data_get(int unit,
                                                               SOC_PPC_AC_ID out_ac, 
                                                               bcm_dpp_vlan_egress_edit_profile_info_t *mapping_profile);

int _bcm_dpp_am_template_vlan_edit_profile_eg_mapping_data_get_by_template(int unit,
                                                                           int template_id, 
                                                                           bcm_dpp_vlan_egress_edit_profile_info_t *mapping_profile);


int _bcm_dpp_am_template_vlan_edit_profile_eg_mapping_exchange(int unit, 
                                                               SOC_PPC_AC_ID out_ac, 
                                                               bcm_dpp_vlan_egress_edit_profile_info_t *mapping_profile, 
                                                               int *old_profile, 
                                                               int *is_last, 
                                                               int *new_profile,
                                                               int *is_allocated);
/* VLAN Edit Profile mapping - End */
/* L2 reserved MC - Start */

int _bcm_dpp_am_template_l2_cache_rm_init(int unit,int template_init_id,_bcm_petra_l2_rsrvd_mc_profile_info_t *init_data);

int _bcm_dpp_am_template_l2_cache_rm_data_get(int unit, int port, _bcm_petra_l2_rsrvd_mc_profile_info_t * data);

int _bcm_dpp_am_template_l2_cache_rm_exchange(int unit, int flag,  int port, _bcm_petra_l2_rsrvd_mc_profile_info_t * data, int *old_template, int *is_last, int *template,int *is_allocated);
int _bcm_dpp_am_template_l2_cache_rm_clear(int unit);

/* L2 reserved MC - End */

/* TPID classify - Start (LLVP PORT profile) */
int _bcm_dpp_am_template_port_tpid_class_init(int unit,int template_init_id,_bcm_dpp_port_tpid_class_info_t *init_data);

int _bcm_dpp_am_template_port_tpid_class_data_get(int unit, int port, _bcm_dpp_port_tpid_class_info_t * data);
int _bcm_dpp_am_template_port_tpid_class_eg_data_get(int unit, int port, _bcm_dpp_port_tpid_class_info_t * data);

int _bcm_dpp_am_template_port_tpid_class_exchange(int unit, int flag,  int port, _bcm_dpp_port_tpid_class_info_t * data, int *old_template, int *is_last, int *template,int *is_allocated);
int _bcm_dpp_am_template_port_tpid_class_clear(int unit);
int _bcm_dpp_am_template_port_tpid_class_eg_exchange(int unit, int flags, int port, _bcm_dpp_port_tpid_class_info_t * data, int *old_template, int *is_last, int *template,int *is_allocated);

/* TPID classify */

/* tpid egress acceptable frame type - Start */

int _bcm_dpp_am_template_port_tpid_class_egress_acceptable_frame_type_init(int unit,int template_init_id, _bcm_dpp_port_tpid_class_acceptable_frame_type_info_t *init_data);

int _bcm_dpp_am_template_port_tpid_class_egress_acceptable_frame_type_data_get(int unit, int port, _bcm_dpp_port_tpid_class_acceptable_frame_type_info_t *data);

int _bcm_dpp_am_template_port_tpid_class_egress_acceptable_frame_type_exchange(int unit, int flags, int port, _bcm_dpp_port_tpid_class_acceptable_frame_type_info_t *data, int *old_template, int *is_last, int *template,int *is_allocated);

int _bcm_dpp_am_template_port_tpid_class_egress_acceptable_frame_type_clear(int unit);

/* tpid egress classify acceptable frame type  - End */

/* Trap- L2 reserved MC - Start */

int _bcm_dpp_am_template_l2_trap_rm_init(int unit,int template_init_id,bcm_dpp_user_defined_traps_t* init_data);

int _bcm_dpp_am_template_l2_trap_rm_data_get(int unit, int port, int mac_lsb, bcm_dpp_user_defined_traps_t* data);

int _bcm_dpp_am_template_l2_trap_rm_exchange(int unit, int flag, int port, int mac_lsb, bcm_dpp_user_defined_traps_t* data, int *old_template, int *is_last, int *template,int *is_allocated, int flag_with_id);

int _bcm_dpp_am_template_l2_trap_rm_port_profile_mapping_get(int unit, int port, int mac_lsb, int *trap_code);

int _bcm_dpp_am_template_l2_trap_rm_clear(int unit);

/* Trap- L2 reserved MC - end */

/* VSI PROFILE egress - Start */
int _bcm_dpp_am_template_vsi_egress_profile_init(int unit, bcm_dpp_vsi_egress_profile_t* init_data);
int _bcm_dpp_am_template_vsi_egress_profile_data_get(int unit, int vsi, bcm_dpp_vsi_egress_profile_t* data);
int _bcm_dpp_am_template_vsi_egress_profile_mapping_get(int unit, int vsi, int *profile);
int _bcm_dpp_am_template_vsi_egress_profile_exchange(int unit, int flags, int vsi, bcm_dpp_vsi_egress_profile_t * data,
                                             int *old_template, int *is_last, int *template, int *is_allocated);

/* VSI PROFILE egress - End */

/* L2CP PROFILE egress - Start */
int _bcm_dpp_am_template_l2cp_egress_profile_init(int unit, bcm_dpp_vsi_egress_profile_t* init_data);
int _bcm_dpp_am_template_l2cp_egress_profile_mapping_get(int unit, int outlif, int *profile);
int _bcm_dpp_am_template_l2cp_egress_profile_data_get(int unit, int outlif, bcm_dpp_l2cp_egress_profile_t * data);
int _bcm_dpp_am_template_l2cp_egress_profile_exchange(int unit, int flags, int outlif, bcm_dpp_l2cp_egress_profile_t * data,
                            int *old_template, int *is_last, int *template, int *is_allocated);
int _bcm_dpp_am_template_l2cp_egress_profile_clear(int unit);



/* L2CP PROFILE egress - End */


/* VSI PROFILE ingress - Start */
int _bcm_dpp_am_template_vsi_ingress_profile_init(int unit, bcm_dpp_vsi_ingress_profile_t* init_data);
int _bcm_dpp_am_template_vsi_ingress_profile_data_get(int unit, int vsi, bcm_dpp_vsi_ingress_profile_t* data);
int _bcm_dpp_am_template_vsi_ingress_profile_mapping_get(int unit, int vsi, int *profile);
int _bcm_dpp_am_template_vsi_ingress_profile_exchange(int unit, int flags, int vsi, bcm_dpp_vsi_ingress_profile_t * data,
                                             int *old_template, int *is_last, int *template, int *is_allocated);

/* VSI PROFILE ingress - End */

/* OAM ICC MAP - Start */
int _bcm_dpp_am_template_oam_icc_map_ndx_alloc(int unit, int flags, SOC_PPC_OAM_ICC_MAP_DATA *data, int *is_allocated, int *icc_ndx);
int _bcm_dpp_am_template_oam_icc_map_ndx_free(int unit, int icc_ndx, int *is_last);
/*int _bcm_dpp_am_template_oam_icc_map_ndx_data_get(int unit, int mep_id, SOC_PPC_OAM_ICC_MAP_DATA* data);
int _bcm_dpp_am_template_oam_icc_map_ndx_get(int unit, int mep_id, int *icc_map_reg_index);
int _bcm_dpp_am_template_oam_icc_map_ndx_exchange(int unit, int flags, int mep_id, SOC_PPC_OAM_ICC_MAP_DATA * data,
                                             int *old_icc_ndx, int *is_last, int *icc_ndx, int *is_allocated);*/
/* OAM ICC MAP - End */


/* OAM SA MAC address (Jericho)*/
int _bcm_dpp_am_template_oam_sa_mac_address_alloc(int unit, int flags, uint8 *data, int *is_allocated, int *profile);
int _bcm_dpp_am_template_oam_sa_mac_address_free(int unit, int profile, int *is_last);
int _bcm_dpp_am_template_oam_sa_mac_address_get(int unit, int profile, uint8* data);


/* OAMP punt event hendling profile - Start */
int _bcm_dpp_am_template_oam_punt_event_hendling_profile_alloc(int unit, int flags, SOC_PPC_OAM_OAMP_PUNT_PROFILE_DATA *data, int *is_allocated, int *profile);
int _bcm_dpp_am_template_oam_punt_event_hendling_profile_free(int unit, int profile, int *is_last);
/* OAMP punt event hendling profile - End */


/* BFD Flags - Start */  
int _bcm_dpp_am_template_bfd_flags_profile_alloc(int unit, int flags, const uint32 *data, int *is_allocated, int *profile_ndx); 
int _bcm_dpp_am_template_bfd_flags_profile_free(int unit, int profile_ndx, int *is_last); 
int _bcm_dpp_am_template_bfd_flags_profile_data_get(int unit, int profile_ndx, uint32 * data);
/* BFD Flags - End */

/* OAM mep profile non accelerated - Start */
int _bcm_dpp_am_template_oam_mep_profile_non_accelerated_alloc(int unit, int flags, SOC_PPC_OAM_LIF_PROFILE_DATA *data, int *is_allocated, int *profile);
int _bcm_dpp_am_template_oam_mep_profile_non_accelerated_free(int unit, int profile, int *is_last);
int _bcm_dpp_am_template_oam_mep_profile_non_accelerated_data_get(int unit, int mep_id, uint8 is_passive, SOC_PPC_OAM_LIF_PROFILE_DATA* data);
int _bcm_dpp_am_template_oam_mep_profile_data_validate(int unit, int mep_id, uint8 is_passive, SOC_PPC_OAM_LIF_PROFILE_DATA* data, int *valid);
int _bcm_dpp_am_template_oam_mep_profile_non_accelerated_data_from_profile_get(int unit, int profile, SOC_PPC_OAM_LIF_PROFILE_DATA* data);
int _bcm_dpp_am_template_oam_mep_profile_non_accelerated_mapping_get(int unit, int mep_id, int is_passive, int *profile);
int _bcm_dpp_am_template_oam_mep_profile_non_accelerated_exchange(int unit, int flags, int mep_id, int is_passive, SOC_PPC_OAM_LIF_PROFILE_DATA * data,
                                             int *old_profile, int *is_last, int *profile, int *is_allocated);
/* OAM mep profile non accelerated - End */

/* OAM mep profile accelerated - Start */
int _bcm_dpp_am_template_oam_mep_profile_accelerated_alloc(int unit, int flags, SOC_PPC_OAM_LIF_PROFILE_DATA *data, int *is_allocated, int *profile);
int _bcm_dpp_am_template_oam_mep_profile_accelerated_free(int unit, int profile, int *is_last);
int _bcm_dpp_am_template_oam_mep_profile_accelerated_data_get(int unit, int mep_id, SOC_PPC_OAM_LIF_PROFILE_DATA* data);
int _bcm_dpp_am_template_oam_mep_profile_accelerated_mapping_get(int unit, int mep_id, int *profile);
int _bcm_dpp_am_template_oam_mep_profile_accelerated_exchange(int unit, int flags, int mep_id, SOC_PPC_OAM_LIF_PROFILE_DATA * data,
                                             int *old_profile, int *is_last, int *profile, int *is_allocated);
/* OAM mep profile accelerated - End */

/* BFD mep profile non accelerated - Start */
int _bcm_dpp_am_template_bfd_mep_profile_non_accelerated_alloc(int unit, int flags, SOC_PPC_OAM_LIF_PROFILE_DATA *data, int *is_allocated, int *profile);
int _bcm_dpp_am_template_bfd_mep_profile_non_accelerated_free(int unit, int profile, int *is_last);
int _bcm_dpp_am_template_bfd_mep_profile_non_accelerated_data_get(int unit, int mep_id, SOC_PPC_OAM_LIF_PROFILE_DATA* data);
int _bcm_dpp_am_template_bfd_mep_profile_non_accelerated_mapping_get(int unit, int mep_id, int *profile);
int _bcm_dpp_am_template_bfd_mep_profile_non_accelerated_exchange(int unit, int flags, int mep_id, SOC_PPC_OAM_LIF_PROFILE_DATA * data,
                                             int *old_profile, int *is_last, int *profile, int *is_allocated);
/* BFD mep profile non accelerated - End */

/* BFD mep profile accelerated - Start */
int _bcm_dpp_am_template_bfd_mep_profile_accelerated_alloc(int unit, int flags, SOC_PPC_OAM_LIF_PROFILE_DATA *data, int *is_allocated, int *profile);
int _bcm_dpp_am_template_bfd_mep_profile_accelerated_free(int unit, int profile, int *is_last);
int _bcm_dpp_am_template_bfd_mep_profile_accelerated_data_get(int unit, int mep_id, SOC_PPC_OAM_LIF_PROFILE_DATA* data);
int _bcm_dpp_am_template_bfd_mep_profile_accelerated_data_from_profile_get(int unit, int profile, SOC_PPC_OAM_LIF_PROFILE_DATA* data);
int _bcm_dpp_am_template_bfd_mep_profile_accelerated_mapping_get(int unit, int mep_id, int *profile);
int _bcm_dpp_am_template_bfd_mep_profile_accelerated_exchange(int unit, int flags, int mep_id, SOC_PPC_OAM_LIF_PROFILE_DATA * data,
                                             int *old_profile, int *is_last, int *profile, int *is_allocated);
/* BFD mep profile accelerated - End */

/* OAM Priority - Start */
int _bcm_dpp_am_template_oam_tx_priority_alloc(int unit, int flags, const SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES *data, int *is_allocated, int *profile_ndx);
int _bcm_dpp_am_template_oam_tx_priority_free(int unit, int profile_ndx, int *is_last);
int _bcm_dpp_am_template_oam_tx_priority_data_get(int unit, int profile_ndx, SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES* data);
int _bcm_dpp_am_template_oam_tx_priority_exchange(int unit, int flags, const SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES *data, int *is_allocated, int *profile_ndx, int old_profile_ndx, int *is_last);
/* OAM Priority - End */

/* BFD IPV4 DIP - Start */
int _bcm_dpp_am_template_bfd_ip_dip_alloc(int unit, int flags, const SOC_SAND_PP_IPV6_ADDRESS *data, int *is_allocated, int *profile_ndx);
int _bcm_dpp_am_template_bfd_ip_dip_free(int unit, int profile_ndx, int *is_last);
int _bcm_dpp_am_template_bfd_ip_dip_data_get(int unit, int profile_ndx, SOC_SAND_PP_IPV6_ADDRESS* data);
/* BFD IPV4 DIP - End */

/* BFD Push profile - Start */
int _bcm_dpp_am_template_mpls_pwe_push_profile_alloc(int unit, int flags, const SOC_PPC_MPLS_PWE_PROFILE_DATA *data, int *is_allocated, int *mpls_pwe_profile);
int _bcm_dpp_am_template_mpls_pwe_push_profile_data_get(int unit, int mpls_pwe_profile, SOC_PPC_MPLS_PWE_PROFILE_DATA* data);
int _bcm_dpp_am_template_mpls_pwe_push_profile_free(int unit, int mpls_pwe_profile, int *is_last);
/* BFD Push profile - End */

/* BFD Req interval pointer - Start */
int _bcm_dpp_am_template_bfd_req_interval_pointer_alloc(int unit, int flags, const uint32 *data, int *is_allocated, int *rx_rate_pointer);
int _bcm_dpp_am_template_bfd_req_interval_pointer_data_get(int unit, int rx_rate_pointer, uint32* data);
int _bcm_dpp_am_template_bfd_req_interval_pointer_free(int unit, int rx_rate_pointer, int *is_last);
/* BFD Req interval pointer - End */

/* BFD TOS TTL profile - Start */
int _bcm_dpp_am_template_bfd_tos_ttl_profile_alloc(int unit, int flags, const SOC_PPC_BFD_IP_MULTI_HOP_TOS_TTL_DATA *data, int *is_allocated, int *tos_ttl_profile);
int _bcm_dpp_am_template_bfd_tos_ttl_profile_data_get(int unit, int tos_ttl_profile, SOC_PPC_BFD_IP_MULTI_HOP_TOS_TTL_DATA* data);
int _bcm_dpp_am_template_bfd_tos_ttl_profile_free(int unit, int tos_ttl_profile, int *is_last);
/* BFD TOS TTL profile - End */

/* BFD src ip profile - Start */ 
int _bcm_dpp_am_template_bfd_src_ip_profile_alloc(int unit, int flags, const uint32 *data, int *is_allocated, int *src_ip_add_profile);
int _bcm_dpp_am_template_bfd_src_ip_profile_data_get(int unit, int src_ip_add_profile, uint32* data);
int _bcm_dpp_am_template_bfd_src_ip_profile_free(int unit, int src_ip_add_profile, int *is_last);
/* BFD src ip profile - End */

/* oamp_pe_gen_mem MAID48 - Start */
int _bcm_dpp_am_template_oamp_pe_gen_mem_maid48_alloc(int unit, int flags, uint8 *data, int *is_allocated, int *maid_48);
int _bcm_dpp_am_template_oamp_pe_gen_mem_maid48_data_get(int unit, int maid_48, uint8* data);
int _bcm_dpp_am_template_oamp_pe_gen_mem_maid48_free(int unit, int maid_48, int *is_last);
/* oamp_pe_gen_mem MAID48 - End */

/* OAM Local port to system port */
int _bcm_dpp_am_template_oam_local_port_2_sys_port_alloc(int unit, int flags, const uint32* data, int *is_allocated, int *profile_ndx);
int _bcm_dpp_am_template_oam_local_port_2_sys_port_data_get(int unit, int local_port, uint32 *data);
/** Delete by MEP-ID. MEP-DB entry must be active. */
int _bcm_dpp_am_template_oam_local_port_2_sys_port_free(int unit, int profile, int *is_last);

/* OAM OAMP PE gen memory. Used by server.*/
/** MEP_DB.local_port is the profile, 
    but the actual data of MEP_DB.local_port is the src mac address LSB.
    The data returned here is the actual local port.*/
int _bcm_dpp_am_template_oam_oamp_pe_gen_mem_alloc(int unit, int flags, const uint32* data, int *is_allocated, int *profile_ndx);
int _bcm_dpp_am_template_oam_oamp_pe_gen_mem_data_get(int unit, int local_port, uint32 *data); 
/** Delete by MEP-ID. MEP-DB entry must be active. */
int _bcm_dpp_am_template_oam_oamp_pe_gen_mem_free(int unit, int mep_id, int *is_last);



/*OAM - manage LMM  DA NIC tables, ARAD+, start. */
int _bcm_dpp_am_template_oam_lmm_nic_tables_alloc(int unit, int flags, uint32* lsbytes, int *is_allocated, int *profile_ndx);
/**   Profiles are found by endpoint_id. profile_ndx returns
 *    the profile used in the HW. */
int _bcm_dpp_am_template_oam_lmm_nic_tables_free(int unit, int mep_id, int *profile_ndx, int *is_last);
int _bcm_dpp_am_template_oam_lmm_nic_tables_get(int unit, int mep_id, int *profile_ndx, uint32* lsbytes);
/*OAM - manage LMM  DA NIC tables, ARAD+, end */

/*OAM - manage LMM  DA OUI tables, ARAD+, start */
int _bcm_dpp_am_template_oam_lmm_oui_tables_alloc(int unit, int flags, uint32* msbytes, int *is_allocated, int *profile_ndx);
/**   Profiles are found by endpoint_id. profile_ndx returns
 *    the profile used in the HW. */
int _bcm_dpp_am_template_oam_lmm_oui_tables_free(int unit, int mep_id, int *profile_ndx, int *is_last);
int _bcm_dpp_am_template_oam_lmm_oui_tables_get(int unit, int mep_id, int *profile_ndx, uint32* msbytes);
/*OAM - manage LMM  DA OUI tables, ARAD+, end */

/*OAM - manage LMM/DMM mep profile, ARAD+, start */

/* alloc function for each component that uses the mep profile*/

int _bcm_dpp_am_template_oam_eth1731_mep_profile_alloc(int unit, SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY* data,int flags, int *is_allocated, int *profile_ndx);
int _bcm_dpp_am_template_oam_eth_1731_mep_profile_RDI_alloc(int unit,SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY *data  , int *is_allocated, int *profile_ndx);
/* Two modes for the following functions:
   is_first==1 : an entry with only relevant fields matching will be found.
   is_first==0 : data will be updated to reflect changes inserted by other components.
   Either case only relevant  fields (including lmm_da_out_prof) must be properly set in advanced.*/
int _bcm_dpp_am_template_oam_eth_1731_mep_profile_loss_alloc(int unit, int mep_id,SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY *data , int *is_allocated, int *profile_ndx, 
                                                             int *old_profile, int *is_last, int is_first, uint8 is_scan_count_updated);
int _bcm_dpp_am_template_oam_eth_1731_mep_profile_delay_alloc(int unit, int mep_id ,SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY *data, int *is_allocated, int *profile_ndx, 
                                                             int *old_profile, int *is_last , int is_first, uint8 is_scan_count_update);
int _bcm_oam_am_template_oam_eth_1731_mep_profile_loopback_alloc(int unit, int mep_id ,SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY *data, int *is_allocated, int *profile_ndx, 
                                                             int *old_profile, int *is_last, int is_first, uint8 is_scan_count_update);

int _bcm_dpp_am_template_oam_eth1731_mep_profile_free(int unit,int mep_id,  int *profile_ndx, int *is_last);
int _bcm_dpp_am_template_oam_eth1731_mep_profile_get(int unit , int mep_id , int* profile_ndx,SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY* data);
int _bcm_dpp_am_template_oam_eth1731_mep_profile_exchange(int unit,  int old_profile_indx,SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY* data, int *is_allocated,int *is_last, int *profile_ndx);
/*OAM - manage LMM/DMM mep profile, ARAD+, start */


/* Prog Trap - Start */
int _bcm_dpp_am_template_l2_prog_trap_init(int unit,int template_init_id,SOC_PPC_LLP_TRAP_PROG_TRAP_QUALIFIER *init_data);
int _bcm_dpp_am_template_l2_prog_trap_data_get(int unit, int port, int type, SOC_PPC_LLP_TRAP_PROG_TRAP_QUALIFIER *qual, bcm_dpp_user_defined_traps_t *trap_res);
int _bcm_dpp_am_template_l2_prog_trap_alloc(int unit, int flags, SOC_PPC_LLP_TRAP_PROG_TRAP_QUALIFIER *qual, bcm_dpp_user_defined_traps_t *trap_res, int *template,int *is_allocated, bcm_dpp_rx_virtual_traps_t *virtual_traps_p, int is_virtual_traps_p_set);
int _bcm_dpp_am_template_l2_prog_trap_free(int unit, int is_virtual_traps_p_set, int type, int *template, int *is_last);
int _bcm_dpp_am_template_l2_prog_trap_ppc_index_get(int unit, int type, int *trap_index);
int _bcm_dpp_am_template_l2_prog_trap_clear(int unit);

/* Prog Trap - End */

/* L2, VSI/FID --> event-handle-profile --> handle info (BCM_L2_ADDR_DIST_XXX flags) - Start */
int _bcm_dpp_am_template_l2_event_handle_init(int unit,int event_handle, int init_handle_flags);
int _bcm_dpp_am_template_l2_event_handle_fid_profile_mapping_get(int unit, int vsi, int *event_handle_profile);
int _bcm_dpp_am_template_l2_event_handle_data_get(int unit, int vsi, int *handle_flags);
int _bcm_dpp_am_template_l2_event_handle_exchange(int unit, int flags, int vsi, int handle_flags, int *old_template, int *is_last, int *new_profile,int *is_allocated);
int _bcm_dpp_am_template_l2_event_handle_clear(int unit);
/* L2, FID-profile --> even-handle-profile --> handle info (BCM_L2_ADDR_DIST_XXX flags) - End */


/* L2, VSI/FID --> learn-profile --> <even-handle-profile, limit> - Start */
int _bcm_dpp_am_template_l2_learn_profile_init(int unit,int learn_profile_id, int limit, int event_handle_profile, int fid_aging_profile);
int _bcm_dpp_am_template_l2_learn_profile_mapping_get(int unit, int vsi, int *learn_profile);
int _bcm_dpp_am_template_l2_learn_limit_active(int unit, int *is_active);
int _bcm_dpp_am_template_l2_learn_profile_data_get(int unit, int vsi, int *limit, int *event_handle_profile, int *fid_aging_profile);
int _bcm_dpp_am_template_l2_learn_profile_exchange(int unit, int flags, int vsi, int limit,int event_handle_profile, int fid_aging_profile, int *old_template, int *is_last, int *new_profile,int *is_allocated);
int _bcm_dpp_am_template_l2_learn_profile_clear(int unit);
/* L2, VSI/FID --> learn-profile --> <even-handle-profile, limit> - End */

/* L2, VSI/FID --> fid-aging-profile -->  - Start */
/* L2, FID-profile --> fid-aging-profile --> aging info (BCM_L2_ADDR_DIST_XXX flags) - End */

/* Ingress queue rate class - Start */
int _bcm_dpp_am_template_queue_rate_cls_init(int unit,int template_init_id,const bcm_dpp_cosq_ingress_rate_class_info_t* data_rate_cls);
int _bcm_dpp_am_template_queue_rate_cls_data_get(int unit, int core, int queue,bcm_dpp_cosq_ingress_rate_class_info_t * data_rate_cls);
int _bcm_dpp_am_template_queue_rate_cls_exchange(int unit, int core, int queue, bcm_dpp_cosq_ingress_rate_class_info_t * data_rate_cls, int *old_rate_cls, int *is_last, int *new_rate_cls,int *is_allocated);
int _bcm_dpp_am_template_queue_rate_cls_exchange_test(int unit, int core, int queue, bcm_dpp_cosq_ingress_rate_class_info_t * data_rate_cls, int *old_rate_cls, int *is_last, int *new_rate_cls,int *is_allocated);
int _bcm_dpp_am_template_queue_rate_cls_ref_get(int unit, int template, uint32 *ref_count);
int _bcm_dpp_am_template_queue_rate_cls_allocate_group(int unit, uint32 flags, bcm_dpp_cosq_ingress_rate_class_info_t *data_rate_cls, int ref_count, int *is_allocated, int *template);
int _bcm_dpp_am_template_queue_rate_cls_tdata_get(int unit, int template, bcm_dpp_cosq_ingress_rate_class_info_t *data_rate_cls);

/* Ingress queue rate class - End */


/* VSQ PG TC mapping profile - Start */
int _bcm_dpp_am_template_vsq_pg_tc_mapping_profile_init(int unit, int core_id, uint32 pg_tc_bitmap);
int _bcm_dpp_am_template_vsq_pg_tc_mapping_profile_data_get(int unit, int core_id, int pg_tc_profile, uint32 *pg_tc_bitmap);
int _bcm_dpp_am_template_vsq_pg_tc_mapping_exchange(int unit, int core_id, uint32 pg_tc_bitmap, int old_pg_tc_profile, int *is_last, int *new_pg_tc_profile,int *is_allocated);
/* VSQ PG TC mapping profile - End*/

/* VSQ queue rate class - Start */

int _bcm_dpp_am_template_vsq_rate_cls_init(int unit,int template_init_id,const bcm_dpp_cosq_vsq_rate_class_info_t* data_rate_cls);
int _bcm_dpp_am_template_vsq_rate_cls_data_get(int unit, int core_id, int vsq_id, bcm_dpp_cosq_vsq_rate_class_info_t * data_rate_cls);
int _bcm_dpp_am_template_vsq_rate_cls_exchange(int unit, int core_id, int vsq_id, bcm_dpp_cosq_vsq_rate_class_info_t * data_rate_cls, int *old_rate_cls, int *is_last, int *new_rate_cls,int *is_allocated);
int _bcm_dpp_am_template_vsq_rate_cls_ref_get(int unit, int template, SOC_TMC_ITM_VSQ_GROUP vsq_group, uint32 *ref_count);
int _bcm_dpp_am_template_vsq_rate_cls_allocate_group(int unit, uint32 flags, SOC_TMC_ITM_VSQ_GROUP vsq_group, bcm_dpp_cosq_vsq_rate_class_info_t *data_rate_cls, int ref_count, int *is_allocated, int *template);
int _bcm_dpp_am_template_vsq_rate_cls_tdata_get(int unit, int template, SOC_TMC_ITM_VSQ_GROUP vsq_group, bcm_dpp_cosq_vsq_rate_class_info_t *data_rate_cls);

/* VSQ queue rate class - End */

/* System RED drop probability value class - Start */

int _bcm_dpp_am_template_system_red_dp_pr_init(int unit,int template_init_id,const bcm_dpp_cosq_ingress_system_red_dp_pr_info_t* data_sys_red_dp_pr);
int _bcm_dpp_am_template_system_red_dp_pr_data_get(int unit,int dp_pr,bcm_dpp_cosq_ingress_system_red_dp_pr_info_t * data_sys_red_dp_pr);
int _bcm_dpp_am_template_system_red_dp_pr_exchange(int unit, int dp_pr, bcm_dpp_cosq_ingress_system_red_dp_pr_info_t *data_sys_red_dp_pr, int *old_sys_red_dp_pr, int *is_last, int *new_sys_red_dp_pr,int *is_allocated);
int _bcm_dpp_am_template_system_red_dp_pr_ref_get(int unit, int template, uint32 *ref_count);
int _bcm_dpp_am_template_system_red_dp_pr_allocate_group(int unit, uint32 flags, bcm_dpp_cosq_ingress_system_red_dp_pr_info_t *data_sys_red_dp_pr, int ref_count, int *is_allocated, int *template);
int _bcm_dpp_am_template_system_red_dp_pr_tdata_get(int unit, int template, bcm_dpp_cosq_ingress_system_red_dp_pr_info_t *data_sys_red_dp_pr);

/* System RED drop probability value class - End */


/* Ingress queue discount class - Start */

int _bcm_dpp_am_template_queue_discount_cls_init(int unit, int template_init_id,const SOC_TMC_ITM_CR_DISCOUNT_INFO* data_discount_cls);
int _bcm_dpp_am_template_queue_discount_cls_mapping_get(int unit, int core, int queue, int *discount_cls);
int _bcm_dpp_am_template_queue_discount_cls_data_get(int unit, int core, int queue,SOC_TMC_ITM_CR_DISCOUNT_INFO * data_discount_cls);
int _bcm_dpp_am_template_queue_discount_cls_exchange(int unit, int core, int queue, SOC_TMC_ITM_CR_DISCOUNT_INFO * data_discount_cls, int *old_discount_cls, int *is_last, int *new_discount_cls,int *is_allocated);
int _bcm_dpp_am_template_queue_discount_cls_ref_get(int unit, int core, int template, uint32 *ref_count);
int _bcm_dpp_am_template_queue_discount_cls_allocate_group(int unit, int core, uint32 flags, SOC_TMC_ITM_CR_DISCOUNT_INFO *data_discount_cls, int ref_count, int *is_allocated, int *template);
int _bcm_dpp_am_template_queue_discount_cls_tdata_get(int unit, int core, int template, SOC_TMC_ITM_CR_DISCOUNT_INFO *data_discount_cls);

/* Ingress queue discount class - End */

/* Ingress pp port discount class - Start */

int _bcm_dpp_am_template_pp_port_discount_cls_init(int unit, int template_init_id,const SOC_TMC_ITM_CR_DISCOUNT_INFO* data_discount_cls);
int _bcm_dpp_am_template_pp_port_discount_cls_mapping_get(int unit, int core, int port, int *discount_cls);
int _bcm_dpp_am_template_pp_port_discount_cls_data_get(int unit, int core, int port,SOC_TMC_ITM_CR_DISCOUNT_INFO * data_discount_cls);
int _bcm_dpp_am_template_pp_port_discount_cls_exchange(int unit, int core, int port, SOC_TMC_ITM_CR_DISCOUNT_INFO * data_discount_cls, int *old_discount_cls, int *is_last, int *new_discount_cls,int *is_allocated);
int _bcm_dpp_am_template_pp_port_discount_cls_ref_get(int unit, int core, int template, uint32 *ref_count);
int _bcm_dpp_am_template_pp_port_discount_cls_allocate_group(int unit, int core, uint32 flags, SOC_TMC_ITM_CR_DISCOUNT_INFO *data_discount_cls, int ref_count, int *is_allocated, int *template);
int _bcm_dpp_am_template_pp_port_discount_cls_tdata_get(int unit, int core, int template, SOC_TMC_ITM_CR_DISCOUNT_INFO *data_discount_cls);

/* Ingress pp port discount class - End */

/* Egress port discount class - Start */
int _bcm_dpp_am_template_egress_port_discount_cls_free(int unit, int core, uint32 tm_port, int* is_last);
int _bcm_dpp_am_template_egress_port_discount_cls_init(int unit, int core, uint32 tm_port, const SOC_TMC_PORT_EGR_HDR_CR_DISCOUNT_INFO* data_discount_cls);
int _bcm_dpp_am_template_egress_port_discount_cls_data_get(int unit,int core, uint32 tm_port, SOC_TMC_PORT_EGR_HDR_CR_DISCOUNT_INFO * data_discount_cls);
int _bcm_dpp_am_template_egress_port_discount_cls_exchange(int unit, int core, uint32 tm_port, SOC_TMC_PORT_EGR_HDR_CR_DISCOUNT_INFO * data_discount_cls, int *old_discount_cls, int *is_last, int *new_discount_cls,int *is_allocated);
/* int _bcm_dpp_am_template_egress_port_discount_cls_ref_get(int unit, int template, uint32 *ref_count); */
int _bcm_dpp_am_template_egress_port_discount_cls_per_header_type_ref_get(int unit, int core, int template , SOC_TMC_PORT_HEADER_TYPE header_type ,uint32 *ref_count);
/* int _bcm_dpp_am_template_egress_port_discount_cls_allocate_group(int unit, uint32 flags, SOC_TMC_PORT_EGR_HDR_CR_DISCOUNT_INFO *data_discount_cls, int ref_count, int *is_allocated, int *template); */
/* int _bcm_dpp_am_template_egress_port_discount_cls_tdata_get(int unit, int template, SOC_TMC_PORT_EGR_HDR_CR_DISCOUNT_INFO *data_discount_cls); */
int _bcm_dpp_am_template_egress_port_discount_cls_per_header_type_tdata_get(int unit, int core, int template, SOC_TMC_PORT_HEADER_TYPE header_type, SOC_TMC_PORT_EGR_HDR_CR_DISCOUNT_INFO *data_discount_cls);

/* Egress port discount class - End */

/* COSQ Port hr flow control profile - Start */

int _bcm_dpp_am_template_cosq_port_hr_fc_profile_mapping_set(int unit, int core, int port, int profile);
int _bcm_dpp_am_template_cosq_port_hr_fc_init(int unit, int core, int template_init_id, int *fc_data_init);

int _bcm_dpp_am_template_cosq_port_hr_fc_data_get(int unit, int core, int port, int *data);

int _bcm_dpp_am_template_cosq_port_hr_fc_exchange(int unit, int core, int port, int *data, int *old_template, int *is_last, int *template, int *is_allocated);

int _bcm_dpp_am_template_cosq_hr_fc_ref_get(int unit, int core, int template, uint32 *ref_count);

int _bcm_dpp_am_template_cosq_hr_fc_allocate_group(int unit, int core, uint32 flags, int *data, int ref_count, int *is_allocated, int *template);

/* COSQ Port Hr flow control profile - End */


/* COSQ CL scheduler profile - Start */

int _bcm_dpp_am_template_cosq_sched_class_init(int unit);

int _bcm_dpp_am_template_cosq_sched_class_allocate(int unit, int core, int flags, int *data, int *is_allocated, int *class_template);

int _bcm_dpp_am_template_cosq_sched_class_free(int unit, int core, int class_template, int *is_last);

int _bcm_dpp_am_template_cosq_sched_class_data_get(int unit, int core, int class_template, int *data);

int _bcm_dpp_am_template_cosq_sched_class_exchange(int unit, int core, int flags,int *data, int old_class_template, int *is_last, int *class_template, int *is_allocated);

int _bcm_dpp_am_template_cosq_sched_class_ref_get(int unit, int core, int template, uint32 *ref_count);

int _bcm_dpp_am_template_cosq_sched_class_allocate_group(int unit, int core, uint32 flags, int *data, int ref_count, int *is_allocated, int *template);

/* COSQ CL scheduler profile - End */


/* MPLS push profile - Start */
int _bcm_dpp_am_template_mpls_push_profile_exchange(int unit, int push_profile, SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO *push_profile_info,  int *is_last, int *new_push_profile,int *is_allocated, uint8 with_id);
int _bcm_dpp_am_template_mpls_push_profile_free(int unit, int push_profile, int *is_last);
int _bcm_dpp_am_template_mpls_push_profile_is_profile_allocated(int unit, int profile, int *is_allocated);
/* MPLS push profile - End */

/* LIF term profile - Start */
int _bcm_dpp_am_template_lif_term_init(int unit,int template_init_id,const SOC_PPC_MPLS_TERM_PROFILE_INFO* term_profile);
int _bcm_dpp_am_template_lif_term_data_get(int unit, SOC_PPC_LIF_ID lif_id, SOC_PPC_MPLS_TERM_PROFILE_INFO *term_profile_info);
int _bcm_dpp_am_template_lif_term_profile_exchange(int unit, SOC_PPC_LIF_ID lif_id, int is_pwe_lif, SOC_PPC_MPLS_TERM_PROFILE_INFO *term_profile_info,  int *old_template, int *is_last, int *new_term_profile,int *is_allocated);
/* LIF term profile - End */

/* SA drop profile - Start */
int _bcm_dpp_am_template_port_mact_sa_drop_init(int unit,int template_init_id,const _bcm_petra_port_mact_mgmt_action_profile_t* action_profile);
int _bcm_dpp_am_template_port_mact_sa_drop_data_get(int unit,int core_id,int port,_bcm_petra_port_mact_mgmt_action_profile_t* action_profile);
int _bcm_dpp_am_template_port_mact_sa_drop_exchange(int unit, int core_id, int port, _bcm_petra_port_mact_mgmt_action_profile_t* action_profile, int *old_template, int *is_last, int *new_template,int *is_allocated);
/* SA drop profile - End */

/* DA unknown profile - Start */
int _bcm_dpp_am_template_port_mact_da_unknown_init(int unit,int template_init_id,const _bcm_petra_port_mact_mgmt_action_profile_t* action_profile);
int _bcm_dpp_am_template_port_mact_da_unknown_data_get(int unit,int core_id, int port,_bcm_petra_port_mact_mgmt_action_profile_t* action_profile);
int _bcm_dpp_am_template_port_mact_da_unknown_exchange(int unit, int core_id, int port, _bcm_petra_port_mact_mgmt_action_profile_t* action_profile, int *old_template, int *is_last, int *new_template,int *is_allocated);
/* SA drop profile - End */

/* meter profile - Start */
int _bcm_dpp_am_template_meter_init(int unit, int template_init_id, bcm_dpp_am_meter_entry_t *meter_profile_info);
int _bcm_dpp_am_template_meter_data_get(int unit,int core_id, int meter_profile,int group, bcm_dpp_am_meter_entry_t *meter_profile_info);
int _bcm_dpp_am_template_meter_exchange(int unit, int core_id, int meter_profile, int group, bcm_dpp_am_meter_entry_t *meter_profile_info,  int *is_last, int *new_meter_profile, int change_profile, int *is_allocated);
/* meter profile - End */


/* Fabric TDM direct routing - Start */
int _bcm_dpp_am_template_fabric_tdm_link_ptr_init(int unit,int template_init_id,const SOC_TMC_TDM_DIRECT_ROUTING_INFO* routing_info);
int _bcm_dpp_am_template_fabric_tdm_link_ptr_data_get(int unit,int port,SOC_TMC_TDM_DIRECT_ROUTING_INFO* routing_info);
int _bcm_dpp_am_template_fabric_tdm_link_ptr_exchange(int unit, int port, SOC_TMC_TDM_DIRECT_ROUTING_INFO * routing_info, int *old_link_ptr, int *is_last, int *new_link_ptr,int *is_allocated);
/* Fabric TDM direct routing - End */

/* L2 flooding per LIF - start */
int _bcm_dpp_am_template_l2_flooding_init(int unit,int template_init_id,const bcm_dpp_vlan_flooding_profile_info_t* flooding_info);
int _bcm_dpp_am_template_l2_flooding_data_get(int unit, SOC_PPC_LIF_ID lif_index, bcm_dpp_vlan_flooding_profile_info_t* flooding_info);
int _bcm_dpp_am_template_l2_flooding_exchange(int unit, SOC_PPC_LIF_ID lif_index, bcm_dpp_vlan_flooding_profile_info_t* flooding_info, int *old_template, int *is_last, int *new_template,int *is_allocated);
/* L2 flooding per LIF - end */

/* Port protocol to vlan,tc - start */
int _bcm_dpp_am_template_vlan_port_protocol_init(int unit,int template_init_id,const bcm_dpp_vlan_port_protocol_entries_t* port_protocol_info);
int _bcm_dpp_am_template_vlan_port_protocol_data_get(int unit, int port, bcm_dpp_vlan_port_protocol_entries_t* port_protocol_info);
int _bcm_dpp_am_template_vlan_port_protocol_exchange(int unit, int port, bcm_dpp_vlan_port_protocol_entries_t* port_protocol_info, int *old_template, int *is_last, int *new_template,int *is_allocated);
/* Port protocol to vlan,tc - end */


/* IP tunnel to SIP, TTL, TOS - start */
int _bcm_dpp_am_template_ip_tunnel_init(int unit,int sip_profile ,SOC_PPC_EG_ENCAP_IP_TUNNEL_SRC_IP *sip_val, int ttl_profile, int ttl_val, int tos_profile, int tos_val, int tunnel_template_profile, SOC_PPC_EG_ENCAP_IP_TUNNEL_SIZE_PROTOCOL_TEMPLATE_INFO *tunnel_template_val);
int _bcm_dpp_am_template_ip_tunnel_data_get(int unit, int tunnel_id, SOC_PPC_EG_ENCAP_IP_TUNNEL_SRC_IP *sip_val, int *ttl_val, int *tos_val);
int _bcm_dpp_am_template_ip_tunnel_exchange(int unit, int tunnel_id, int is_new_ip_tunnel, SOC_PPC_EG_ENCAP_IP_TUNNEL_SRC_IP *sip, int ttl, int tos, int *sip_template, int *ttl_template, int *tos_template,int *is_allocated);
int _bcm_dpp_am_template_ip_tunnel_exchange_test(int unit, int tunnel_id, int is_new_ip_tunnel, SOC_PPC_EG_ENCAP_IP_TUNNEL_SRC_IP *sip, int ttl, int tos, int *sip_template, int *ttl_template, int *tos_template,int *is_allocated);

/* l3_rif_mac_termination_combination - Start */
int _bcm_dpp_am_template_l3_rif_mac_termination_combination_init(int unit, uint32 *profile_info, int template);
int _bcm_dpp_am_template_l3_rif_mac_termination_combination_data_get(int unit, int profile_id, uint32 *profile_info);
int _bcm_dpp_am_template_l3_rif_mac_termination_combination_exchange(int unit, int old_profile_id, int *old_is_last, uint32 *new_profile_info, uint8 *new_mapping_profile,int *is_allocated, uint8 with_id);
int _bcm_dpp_am_template_l3_rif_mac_termination_combination_alloc(int unit, int flags, uint32 *profile_info, uint8 *new_profile, int *is_allocated);
int _bcm_dpp_am_template_l3_rif_mac_termination_combination_free(int unit, uint32 profile, int *is_last);
int _bcm_dpp_am_template_l3_rif_mac_termination_combination_profile_get(int unit, uint32 profile_info, uint8* profile_id);
int _bcm_dpp_am_template_l3_rif_mac_termination_combination_ref_get(int unit, int profile, uint32 *ref_count);
/* l3_rif_mac_termination_combination - end */

/* PTP port profile - start */
int _bcm_dpp_am_template_ptp_port_profile_init(int unit, SOC_PPC_PTP_IN_PP_PORT_PROFILE profile_ndx, SOC_PPC_PTP_PORT_INFO *profile_data);
int _bcm_dpp_am_template_ptp_port_profile_deinit(int unit, SOC_PPC_PTP_IN_PP_PORT_PROFILE profile_ndx);
int _bcm_dpp_am_template_ptp_port_profile_exchange(int unit, int port_ndx, SOC_PPC_PTP_PORT_INFO *new_profile_data, SOC_PPC_PTP_IN_PP_PORT_PROFILE *new_profile_ndx);
int _bcm_dpp_am_template_ptp_port_profile_tdata_get(int unit, SOC_PPC_PTP_IN_PP_PORT_PROFILE profile_ndx, SOC_PPC_PTP_PORT_INFO* profile_data);
/* PTP port profile - end */

#ifdef BCM_88660_A0
/* VRRP info */
int _bcm_dpp_am_template_vrrp_alloc(int unit, SOC_PPC_VRRP_CAM_INFO *cam_info, int *is_allocated, int *profile_ndx);
int _bcm_dpp_am_template_vrrp_alloc_all(int unit, int profile_index);
int _bcm_dpp_am_template_vrrp_get(int unit, int profile_ndx, SOC_PPC_VRRP_CAM_INFO *cam_info);
int _bcm_dpp_am_template_vrrp_dealloc(int unit, int profile_ndx, int *is_last);
int _bcm_dpp_am_template_vrrp_dealloc_all(int unit, int profile_index);
int _bcm_dpp_am_template_vrrp_index_get(int unit, SOC_PPC_VRRP_CAM_INFO *cam_info, int *profile_ndx);
#endif /*BCM_88660_A0 */

/* RIF profile info */
/* Only fill the drop, oam_lif_set, outlif_profile entries before calling alloc/index_get */
int _bcm_dpp_am_template_out_rif_profile_init(int unit,int template_id,QAX_PP_EG_ENCAP_ACCESS_OUT_RIF_PROFILE_ENTRY_FORMAT *rif_profile);
int _bcm_dpp_am_template_out_rif_profile_alloc(int unit, QAX_PP_EG_ENCAP_ACCESS_OUT_RIF_PROFILE_ENTRY_FORMAT *rif_profile, int *is_first, int *profile_ndx);
int _bcm_dpp_am_template_out_rif_profile_get(int unit, int profile_ndx, QAX_PP_EG_ENCAP_ACCESS_OUT_RIF_PROFILE_ENTRY_FORMAT *rif_profile);
int _bcm_dpp_am_template_out_rif_profile_dealloc(int unit, int profile_ndx, int *is_last);
int _bcm_dpp_am_template_out_rif_profile_index_get(int unit, QAX_PP_EG_ENCAP_ACCESS_OUT_RIF_PROFILE_ENTRY_FORMAT *rif_profile, int *profile_ndx);
int _bcm_dpp_am_template_out_rif_profile_exchange(int unit, int flags,int outrif_profile_index,QAX_PP_EG_ENCAP_ACCESS_OUT_RIF_PROFILE_ENTRY_FORMAT *new_profile, int *is_last, int *new_profile_index,int *is_first);

/* For roo link layer format: eth_type_index table */
int bcm_dpp_am_template_eedb_roo_ll_format_eth_type_index_alloc(int unit, int flags, SOC_PPC_EG_ENCAP_ETHER_TYPE_INDEX_INFO *data, int *is_allocated, int *eth_type_index_ndx);
int bcm_dpp_am_template_eedb_roo_ll_format_eth_type_index_get(int unit, int eth_type_index_ndx, SOC_PPC_EG_ENCAP_ETHER_TYPE_INDEX_INFO* data);
int bcm_dpp_am_template_eedb_roo_ll_format_eth_type_index_free(int unit, int eth_type_index_ndx, int *is_last); 
int bcm_dpp_am_template_eedb_roo_ll_format_eth_type_index_exchange(int unit, int flags, int old_index_ndx,SOC_PPC_EG_ENCAP_ETHER_TYPE_INDEX_INFO *data,int *new_index_ndx,int * is_last, int *is_allocated);
int bcm_dpp_am_template_eedb_roo_ll_format_eth_type_index_ref_count_get(int unit, int eth_type_index_ndx, uint32 *count);
int bcm_dpp_am_template_eedb_roo_ll_format_eth_type_index_index_get(int unit,SOC_PPC_EG_ENCAP_ETHER_TYPE_INDEX_INFO *data, int *index);


/* For ip tunnel encapsulation mode. From Jericho B0, QAX. */
int bcm_dpp_am_template_ip_tunnel_encapsulation_mode_get(int unit, int encapsulation_mode, SOC_PPC_EG_ENCAP_IP_TUNNEL_SIZE_PROTOCOL_TEMPLATE_INFO* data); 
int bcm_dpp_am_template_ip_tunnel_encapsulation_mode_alloc(int unit, int flags, SOC_PPC_EG_ENCAP_IP_TUNNEL_SIZE_PROTOCOL_TEMPLATE_INFO* data, int* is_allocated, int *encapsulation_mode); 
int bcm_dpp_am_template_ip_tunnel_encapsulation_mode_exchange(int unit, int old_encapsulation_mode, SOC_PPC_EG_ENCAP_IP_TUNNEL_SIZE_PROTOCOL_TEMPLATE_INFO* data, int *old_is_last, int *new_encapsulation_mode,int *is_allocated, uint32 flags);
int bcm_dpp_am_template_ip_tunnel_encapsulation_mode_exchange_test(int unit, int old_encapsulation_mode, SOC_PPC_EG_ENCAP_IP_TUNNEL_SIZE_PROTOCOL_TEMPLATE_INFO* data, int *old_is_last, int *new_encapsulation_mode,int *is_allocated, uint32 flags);
int bcm_dpp_am_template_ip_tunnel_encapsulation_mode_free(int unit, int encapsulation_mode, int *is_last); 

/* credit adjust size scheduler final delta, relevant only for Jericho */
int _bcm_dpp_am_template_scheduler_adjust_size_final_delta_mapping_init(int unit, int template_init_id, int *final_delta);
int _bcm_dpp_am_template_scheduler_adjust_size_final_delta_mapping_data_get(int unit, int core, int delta , int *final_delta);
int _bcm_dpp_am_template_scheduler_adjust_size_final_delta_mapping_exchange(int unit, int core, int delta, int *final_delta, int *old_profile, int *is_last, int *new_profile,int *is_allocated);

/* For mapping to LIF profile an MTU profile. From Jericho B0, QAX, QUX. */
int _bcm_dpp_am_template_lif_mtu_profile_init(int unit, uint32 template_init_id, uint32 *mtu_val);
int _bcm_dpp_am_template_lif_mtu_profile_exchange(int unit, uint32 *mtu_val,
                                        uint32 *old_profile, int *is_last, uint32 *new_profile,int *is_allocated);

/* For seamless BFD non-accelerated endpoints - managing the SRC IP address - JERICHO and above.*/
int _bcm_dpp_am_template_sbfd_non_acc_src_ip_profile_alloc(int unit, int flags, const uint32 *data, int *is_allocated, int *src_ip_add_profile);
int _bcm_dpp_am_template_sbfd_non_acc_src_ip_profile_data_get(int unit, int src_ip_add_profile, uint32* data);
int _bcm_dpp_am_template_sbfd_non_acc_src_ip_profile_free(int unit, int src_ip_add_profile, int *is_last);
int _bcm_dpp_am_template_sbfd_non_acc_src_ip_profile_exchange(int unit, int flags, const uint32 *data, int *is_allocated, int *src_ip_add_profile, int old_src_ip_add_profile, int *is_last);

/* 
 * Template management - End
 */ 


/* Resource manager flags */

#define BCM_DPP_AM_FLAG_ALLOC_WITH_ID               (SW_STATE_RES_ALLOC_WITH_ID) /*0x00000001*/
/* if exist then reuse, can be set only if with id is present */
#define BCM_DPP_AM_FLAG_ALLOC_REPLACE               (SW_STATE_RES_ALLOC_REPLACE) /*0x00000004*/
#define BCM_DPP_AM_FLAG_ALLOC_EG_DECOUPLED_EVEN     (0x00000004)
#define BCM_DPP_AM_FLAG_ALLOC_INGRESS               (0x00000002)
#define BCM_DPP_AM_FLAG_ALLOC_EGRESS                (0x00000004)

#define BCM_DPP_AM_FLAG_ALLOC_RESERVE               (0x00010000)
#define SOC_DPP_OAM_LOCAL_PORT_2_SYSTEM_PORT_RESERVED_VALUE (-1)

/* relevant for IP tunnel allocation if present then allocate tunnel for IPv6 otherwise allocate for IPv4 */
#define BCM_DPP_AM_FLAG_ALLOC_IPV6                  (0x00020000)
#define BCM_DPP_AM_FLAG_ALLOC_EGRESS_ARP_POINTED    (0x00040000)



/* Template manager falgs */
#define BCM_DPP_AM_TEMPLATE_FLAG_ALLOC_WITH_ID      (SHR_TEMPLATE_MANAGE_SET_WITH_ID) /*0x00000001*/



int
bcm_dpp_am_fec_dealloc(
    int unit,
    uint32 flags,
    int size,/*non-zero for protection or ECMP*/
    SOC_PPC_FEC_ID fec_id);

int
bcm_dpp_am_fec_is_alloced(
    int unit,
    uint32 flags,
    int size,/*use 2 for protection*/
    SOC_PPC_FEC_ID fec_id);

int bcm_dpp_am_fec_get_usage(int unit, SOC_PPC_FEC_ID fec_id, _bcm_dpp_am_fec_alloc_usage *usage);

int
bcm_dpp_am_local_inlif_alloc(
    int unit,
    int core_id,/*bank_id*/
    uint32 flags,
    uint8 *tag,
    int *inlif_id);

int
bcm_dpp_am_local_inlif_dealloc(
    int unit,
    int core_id,
    uint32 flags,
    int inlif_id);

int
bcm_dpp_am_local_inlif_is_alloc(
    int unit,
    int core_id,
    uint32 flags,
    int inlif_id);

int
bcm_dpp_am_local_inlif_tag_get(
    int unit,
    int core_id,
    uint32 flags,
    int inlif_id,
    uint8 *tag_id);

int
bcm_dpp_am_local_inlif_range_set(
    int unit, 
    int core_id, 
    uint32 flags, 
    uint8 tag_id, 
    int start, 
    int count);

int
bcm_dpp_am_local_outlif_alloc(
    int unit,
    int core_id,/*bank_id*/
    uint32 flags,
    uint8 *tag,
    int *outif_id);

int
bcm_dpp_am_local_outlif_alloc_align(
    int unit,
    int core_id,/*bank_id*/
    uint32 flags,
    uint8 *tag, 
    int align, 
    int offset, 
    int count,
    int *outlif_id);

int
bcm_dpp_am_local_outlif_dealloc(
    int unit,
    int core_id,
    uint32 flags,
    int count,
    int outlif_id);

int
bcm_dpp_am_local_outlif_is_alloc(
    int unit,
    int core_id,
    uint32 flags,
    int outif_id);

int 
bcm_dpp_am_local_outlif_range_set(
    int unit, 
    int core_id, 
    uint32 flags, 
    uint8 tag_id, 
    int start, 
    int count); 

int
bcm_dpp_am_l3_intf_vsi_alloc(
    int unit,
        uint32 flags,
        bcm_vlan_t vid,
    uint32   *l3_rif);

int
bcm_dpp_am_l3_intf_vsi_dealloc(
    int unit,
        uint32 flags,
    bcm_vlan_t vid,
    uint32 l3_rif);

/*int
bcm_dpp_am_fec_protection_peer_get(*/
int
bcm_dpp_am_vlan_to_vsi(
    int unit,
    bcm_vlan_t vid,
    SOC_PPC_VSI_ID *vsi);

/*
 * Ethernet policer
 */

int
bcm_dpp_am_policer_dealloc(
    int unit,
	int core_id,
    uint32 flags,
    int size,
    int policer_id);

int
bcm_dpp_am_policer_is_alloc(
    int unit,
	int core_id,
    int size,
    int policer_id);

/* TM */
int _bcm_dpp_am_template_ingress_uc_tc_mapping_data_get(int unit,int core_id, int dest_ndx,SOC_TMC_ITM_TC_MAPPING *tc_mapping);
int _bcm_dpp_am_template_ingress_uc_tc_mapping_init(int unit,int template_init_id,const SOC_TMC_ITM_TC_MAPPING* tc_mapping);
int _bcm_dpp_am_template_ingress_uc_tc_mapping_exchange(int unit, int core_id, int dest_ndx, SOC_TMC_ITM_TC_MAPPING *tc_mapping, uint32 *old_mapping_profile, int *is_last, int *new_mapping_profile,int *is_allocated);
int _bcm_dpp_am_template_ingress_flow_tc_mapping_data_get(int unit, int core_id, int dest_ndx,SOC_TMC_ITM_TC_MAPPING *tc_mapping);
int _bcm_dpp_am_template_ingress_flow_tc_mapping_init(int unit, int template_init_id, const SOC_TMC_ITM_TC_MAPPING* tc_mapping);
int _bcm_dpp_am_template_ingress_flow_tc_mapping_exchange(int unit, int core_id, int dest_ndx, SOC_TMC_ITM_TC_MAPPING *tc_mapping, uint32 *old_mapping_profile, int *is_last, int *new_mapping_profile,int *is_allocated);

int _bcm_dpp_am_template_fc_generic_pfc_mapping_init(int unit, int priority, int template_id, SOC_TMC_FC_PFC_GENERIC_BITMAP *generic_bm);
int _bcm_dpp_am_template_fc_generic_pfc_mapping_data_get(int unit, int priority, int generic_bm_id, SOC_TMC_FC_PFC_GENERIC_BITMAP *generic_bm);
int _bcm_dpp_am_template_fc_generic_pfc_mapping_exchange(int unit, int priority, int old_generic_bm_id, SOC_TMC_FC_PFC_GENERIC_BITMAP *new_generic_bm, int *is_last, int *new_mapping_profile,int *is_allocated);

int _bcm_dpp_am_template_ttl_scope_init(int unit, int template_init_id, uint16 *ttl);
int _bcm_dpp_am_template_ttl_scope_exchange(int unit, int ttl_scope_index, uint16 *ttl, int *is_last, int *new_ttl_scope_index, int *is_allocated);
int _bcm_dpp_am_template_ttl_scope_index_ttl_mapping_get(int unit, int ttl_scope_index, uint16 *ttl);

int _bcm_dpp_am_template_pool_cnm_queue_init(int unit, SOC_TMC_CNM_CP_PROFILE_INFO *profile_info, int template);
int _bcm_dpp_am_template_pool_cnm_queue_profile_data_get(int unit, int profile_id, SOC_TMC_CNM_CP_PROFILE_INFO *profile_info);
int _bcm_dpp_am_template_pool_cnm_queue_profile_exchange(int unit,int old_profile_id, SOC_TMC_CNM_CP_PROFILE_INFO *new_profile_info, int *new_mapping_profile,int *is_allocated);

int _bcm_dpp_am_template_crps_pktSize_compensation_init(int unit, int core ,_dpp_am_template_t template_id, int delta, int template);
int _bcm_dpp_am_template_crps_pktSize_compensation_exchange(int unit, int core, _dpp_am_template_t template_id, int port, int data, int old_template, int *is_last, int *template,int *is_allocated);

int _bcm_dpp_am_template_stat_interface_pktSize_compensation_init(int unit, int core ,_dpp_am_template_t template_id, int delta, int template);
int _bcm_dpp_am_template_stat_interface_pktSize_compensation_exchange(int unit, int core, _dpp_am_template_t template_id, int port, int data, int old_template, int *is_last, int *template,int *is_allocated);
/*
 * PON
 */
int
bcm_dpp_am_pon_channel_profile_alloc(
        int unit,
        uint32 flags, /* flags should be SW_STATE_RES_ALLOC_WITH_ID; */
        uint16 *pon_channel_profile);

int
bcm_dpp_am_pon_channel_profile_dealloc(
        int unit,
        uint16 pon_channel_profile);

int
bcm_dpp_am_pon_channel_profile_is_alloced(
        int unit,
        uint16 pon_channel_profile);

/*
 * Ingress and Egress VLAN edit
 */
int
bcm_dpp_am_vlan_edit_action_id_dealloc(
    int unit,
    uint32 flags,
    int action_id);

int
bcm_dpp_am_vlan_edit_action_id_is_alloc(
    int unit,
    uint32 flags,
    int action_id);

int
bcm_dpp_am_vlan_edit_action_id_dealloc_all(
    int unit,
    uint32 flags);

int
bcm_dpp_am_trill_virtual_nick_name_alloc(
    int unit,
    uint32 flags,
    int *virtual_nick_name_index);

int
bcm_dpp_am_trill_virtual_nick_name_dealloc(
    int unit,
    uint32 flags,
    int virtual_nick_name_index);

int
bcm_dpp_am_trill_virtual_nick_name_is_alloc(
    int unit,
    uint32 flags,
    int virtual_nick_name_index);


/* 
 * Global lif functions - Start
 */

/* Egress global lif */
int bcm_dpp_am_global_lif_alloc(int unit, uint32 flags, int *global_lif);
int bcm_dpp_am_global_lif_is_alloced(int unit, uint32 flags, int global_lif);
int bcm_dpp_am_global_lif_dealloc(int unit, uint32 flags, int global_lif);

int _bcm_dpp_lif_id_is_mapped(int unit, int global_lif, uint8 *is_mapped);

int _bcm_dpp_global_lif_id_is_sync(int unit, int global_lif, uint8 *is_sync);



/* 
 * Global lif functions - End
 */




/*
 * Trill eep functions - Start 
 */

#define _BCM_DPP_AM_TRILL_EEP_TYPE_DEFAULT (0) /* Default Trill EEDB entries */
#define _BCM_DPP_AM_TRILL_EEP_TYPE_ROO     (1) /* Trill EEDB entries for ROO  */

int bcm_dpp_am_trill_eep_alloc(int unit, uint32 types, uint32 flags, int *eep); 
int bcm_dpp_am_trill_eep_is_alloc(int unit, int eep); 
int bcm_dpp_am_trill_eep_dealloc(int unit, int eep); 


/*
 * Trill eep functions - End 
 */

/* 
 * map_encap_intpri_color functions - Start
 */ 

int bcm_dpp_am_map_encap_intpri_color_alloc(int unit, uint32 flags, int *qos_id);
int bcm_dpp_am_map_encap_intpri_color_dealloc(int unit, uint32 flags, int qos_id);
int bcm_dpp_am_map_encap_intpri_color_is_alloced(int unit, int qos_id);

/*
 * map_encap_intpri_color functions - End
 */

/*
 * ipsec_sa_id functions - Start
 */

int bcm_dpp_am_ipsec_sa_id_alloc     (int unit, uint32 flags,  uint32  *sa_id);
int bcm_dpp_am_ipsec_sa_id_dealloc   (int unit, uint32 sa_id);
int bcm_dpp_am_ipsec_sa_id_is_alloced(int unit, uint32 sa_id);

/* 
 * ipsec_sa_id functions - Start
 */ 

/*
 * ipsec_tunnel_id functions - Start
 */

int bcm_dpp_am_ipsec_tunnel_id_alloc     (int unit, uint32 flags,  uint32  *tunnel_id);
int bcm_dpp_am_ipsec_tunnel_id_dealloc   (int unit, uint32 tunnel_id);
int bcm_dpp_am_ipsec_tunnel_id_is_alloced(int unit, uint32 tunnel_id);

/*
 * ipsec_tunnel_id functions - Start
 */


extern bcm_dpp_am_pool_info_t *_bcm_dpp_pool_info[BCM_MAX_NUM_UNITS];
extern bcm_dpp_am_template_info_t *_bcm_dpp_template_info[BCM_MAX_NUM_UNITS];
extern bcm_dpp_am_cosq_res_info_t cosq_res_info[BCM_MAX_NUM_UNITS][SOC_DPP_DEFS_MAX(NOF_CORES)];

/*
 *  Following macros provide declaration of various resource calls.
 */
#define _MGR_ALLOC(_type) \
extern int \
bcm_dpp_##_type##_alloc(int unit, uint32 flags, int count, int *elem);
#define _MGR_ALLOC_ALIGN(_type) \
extern int \
bcm_dpp_##_type##_alloc_align(int unit, \
                              uint32 flags, \
                              int align, \
                              int offset, \
                              int count, \
                              int *elem);
#define _MGR_FREE(_type) \
extern int \
bcm_dpp_##_type##_free(int unit, int count, int elem);
#define _MGR_CHECK(_type) \
extern int \
bcm_dpp_##_type##_check(int unit, int count, int elem);
#define _MGR_ALLOC_GROUP(_type) \
extern int \
bcm_dpp_##_type##_alloc_group(int unit, \
                              uint32 grp_flags, \
                              int grp_size, \
                              int *grp_done, \
                              const uint32 *elem_flags, \
                              const int *elem_count, \
                              int *elem);
#define _MGR_ALLOC_ALIGN_GROUP(_type) \
extern int \
bcm_dpp_##_type##_alloc_align_group(int unit, \
                                    uint32 grp_flags, \
                                    int grp_size, \
                                    int *grp_done, \
                                    const uint32 *elem_flags, \
                                    const int *elem_align, \
                                    const int *elem_offset, \
                                    const int *elem_count, \
                                    int *elem);
#define _MGR_FREE_GROUP(_type) \
extern int \
bcm_dpp_##_type##_free_group(int unit, \
                             uint32 grp_flags, \
                             int grp_size, \
                             int *grp_done, \
                             const int *elem_count, \
                             const int *elem);
#define _MGR_CHECK_GROUP(_type) \
extern int \
bcm_dpp_##_type##_check_group(int unit, \
                              uint32 grp_flags, \
                              int grp_size, \
                              int *grp_done, \
                              const int *elem_count, \
                              const int *elem, \
                              int *status);
#define _MGR_ALL_ACTIONS(_type) \
_MGR_ALLOC(_type) \
_MGR_ALLOC_ALIGN(_type) \
_MGR_ALLOC_GROUP(_type) \
_MGR_ALLOC_ALIGN_GROUP(_type) \
_MGR_FREE(_type) \
_MGR_FREE_GROUP(_type) \
_MGR_CHECK(_type) \
_MGR_CHECK_GROUP(_type)
/*
 *  NOTE: must update the below list whenever changing resource types.
 */
_MGR_ALL_ACTIONS(vsi_vlan)
_MGR_ALL_ACTIONS(vsi_mstp)
_MGR_ALL_ACTIONS(vsi_dynamic)
_MGR_ALL_ACTIONS(lif_vlan)
_MGR_ALL_ACTIONS(lif_dynamic)
_MGR_ALL_ACTIONS(vd_dynamic)
_MGR_ALL_ACTIONS(mc_vlan)
_MGR_ALL_ACTIONS(mc_dynamic)
_MGR_ALL_ACTIONS(mirror_action_profile)
_MGR_ALL_ACTIONS(fec_global)
_MGR_ALL_ACTIONS(fec_local)
_MGR_ALL_ACTIONS(eep_global)
_MGR_ALL_ACTIONS(eep_local)
/*
 *  Don't export the declaration macros.
 */
#undef _MGR_ALLOC
#undef _MGR_ALLOC_ALIGN
#undef _MGR_FREE
#undef _MGR_CHECK
#undef _MGR_ALLOC_GROUP
#undef _MGR_ALLOC_ALIGN_GROUP
#undef _MGR_FREE_GROUP
#undef _MGR_CHECK_GROUP
#undef _MGR_ALL_ACTIONS


#endif /* INCLUDE_ALLOC_MNGR_H */
