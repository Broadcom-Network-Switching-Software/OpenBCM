/* 
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $Copyright: (c) 2013 Broadcom Corporation All Rights Reserved.$
 * $
*/

#ifndef _BCM_INT_DPP_VLAN_H
#define _BCM_INT_DPP_VLAN_H

#include <sal/types.h>
#include <bcm/types.h>
#include <bcm/vlan.h>
#include <soc/types.h>
#include <soc/dpp/SAND/Utils/sand_multi_set.h>
#include <soc/dpp/PPD/ppd_api_lif.h>
#include <soc/dpp/PPD/ppd_api_lif_ing_vlan_edit.h>
#include <soc/dpp/PPD/ppd_api_eg_vlan_edit.h>
#include <soc/dpp/PPD/ppd_api_llp_parse.h>
#include <soc/dpp/dpp_wb_engine.h>
#include <shared/swstate/sw_state.h>

#define VLAN_ACCESS         sw_state_access[unit].dpp.bcm.vlan

/*
 * Define:
 *    VLAN_CHK_ID
 * Purpose:
 *    Causes a routine to return BCM_E_BADID if the specified
 *    VLAN ID is out of range.
 */

#define BCM_DPP_VLAN_CHK_ID(unit, vid) do { \
    if (vid > BCM_VLAN_MAX) BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid VID"))); \
    } while (0);

/*
 * Define:
 *    ING_TUNNEL_CHK_ID
 * Purpose:
 *    Causes a routine to return BCM_E_BADID if the specified
 *    TUNNEL ID is out of range.
 */

#define BCM_DPP_ING_TUNNEL_CHK_ID(unit, tunnel) do { \
    if (tunnel > 2047) BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid ingress Tunnel ID"))); \
    } while (0);

/*
 * Define:
 *    EGR_TUNNEL_CHK_ID
 * Purpose:
 *    Causes a routine to return BCM_E_BADID if the specified
 *    TUNNEL ID is out of range.
 */

#define BCM_DPP_EGR_TUNNEL_CHK_ID(unit, tunnel) do { \
    if (tunnel > 4096) BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid egress Tunnel ID"))); \
    } while (0);

 /*
  * Define:
 *    BCM_DPP_GPON_TUNNEL_CHK_ID
 * Purpose:
 *    Causes a routine to return BCM_E_BADID if the specified
 *    TUNNEL ID is out of range.
 */

#define BCM_DPP_GPON_TUNNEL_CHK_ID(unit, tunnel) do { \
    if (tunnel > 0x0fff) BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid ingress Tunnel ID"))); \
    } while (0);

/*
 * Define:
 *    VLAN_PORT_CLASS_VALID
 * Purpose:
 *    Causes a routine to return BCM_E_PARAM if the specified
 *    PORT CLASS is out of range.
 */

#define BCM_DPP_PORT_CLASS_VALID(unit, port_class) do { \
    if (port_class > SOC_DPP_DEFS_GET(unit, nof_vlan_domains)) BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("Invalid port class"))); \
    } while (0);

/*
 * Define:
 *    VLAN_CHK_PRIO
 * Purpose:
 *    Causes a routine to return BCM_E_PARAM if the specified
 *    priority (802.1p CoS) is out of range.
 */

#define BCM_DPP_VLAN_CHK_PRIO(unit, prio) do { \
    if ((prio < BCM_PRIO_MIN || prio > BCM_PRIO_MAX)) BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid priority"))); \
    } while (0);

#define BCM_DPP_VLAN_CHK_ACTION(unit, action) do { \
    if (action < bcmVlanActionNone || action > bcmVlanActionCopy) BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid action"))); \
    } while (0);


/* 
  * In case of vlan port match is BCM_VLAN_PORT_MATCH_PORT, 
  * if vsi is set to -1, the VSI  assignment is set to SOC_PPC_VSI_EQ_IN_VID.
  */
#define _BCM_DPP_VSI_USE_AS_ASSIGNMENT_MODE(_vsi) (_vsi == BCM_VLAN_ALL)


/* Default forward profile for VSIs. Part of mapping to vsi default default forward
 * info.
 * default forward profile 0 maps unknown UC/MC/BC to mc group 0 + vsid
 * default forward profile 1 maps unknown UC to mc group 0 + vsid, unknown MC to 4K + vsid,
 * and unkonwn BC to 8K + vsid.
 */

#define DPP_VSI_DEFAULT_DROP_PROFILE (0)
#define DPP_VSI_DEFAULT_FRWRD_PROFILE (3)
#define DPP_VSI_DEFAULT_FRWRD_PROFILE_UC_0_MC_4K_UC_8K (2)
#define DPP_BVID_DEFAULT_FRWRD_PROFILE (1) /* for mac in mac, point to trap flooding, base = 12K*/

#define DPP_NOF_SHARED_FIDS (7)

/* unknown-da trap-actions for drop/flooding */
#define _BCM_DPP_VLAN_UNKNOWN_DA_DROP_TRAP  (SOC_PPC_TRAP_CODE_UNKNOWN_DA_0)
#define _BCM_DPP_VLAN_UNKNOWN_DA_FLD_TRAP  (SOC_PPC_TRAP_CODE_UNKNOWN_DA_1)
#define _BCM_DPP_BVID_UNKNOWN_DA_FLD_TRAP  (SOC_PPC_TRAP_CODE_UNKNOWN_DA_7)

#define DPP_PRTCL_NOF_ETHER_TYPE_IDS (16)
#define NOS_PRTCL_PORT_PROFILES (8)


#define BCM_DPP_NOF_BRIDGE_VLANS (BCM_VLAN_INVALID)

/* Advanced VLAN edit mode defines */
#define   DPP_NOF_INGRESS_VLAN_EDIT_PROFILES        (8)

#define   DPP_NOF_EGRESS_VLAN_EDIT_PROFILE_BITS(unit)   (SOC_DPP_DEFS_GET(unit, nof_eve_profile_bits))
#define   DPP_NOF_EGRESS_VLAN_EDIT_PROFILES(unit)       (0x1 << (DPP_NOF_EGRESS_VLAN_EDIT_PROFILE_BITS(unit)))
#define   DPP_EGRESS_VLAN_EDIT_PROFILE_MASK(unit)       (DPP_NOF_EGRESS_VLAN_EDIT_PROFILES(unit) - 1)

#define   DPP_NOF_VLAN_TAG_FORMATS                  (16)
#define   DPP_VLAN_TAG_FORMAT_MASK                  (0xF)

/*
 * Define:
 *    BCM_DPP_VLAN_EDIT_PROFILE_VALID
 * Purpose:
 *    Causes a routine to return BCM_E_PARAM if the specified
 *    VLAN edit profile is out of range.
 */

#define BCM_DPP_VLAN_EDIT_PROFILE_VALID(unit, vlan_edit_profile, is_ingress) do {                               \
    if ( (is_ingress && (vlan_edit_profile >= DPP_NOF_INGRESS_VLAN_EDIT_PROFILES)) ||                           \
        (!is_ingress && (vlan_edit_profile >= DPP_NOF_EGRESS_VLAN_EDIT_PROFILES(unit))))                              \
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid VLAN edit profile")));                             \
    } while (0)


/*
 * The entire vlan_info structure is protected by BCM_LOCK.
 */
typedef struct bcm_dpp_vlan_info_s {
    int        init;      /* TRUE if VLAN module has been inited */
    bcm_vlan_t defl;      /* Default VLAN */
    bcm_pbmp_t defl_pbmp; /* Memeber ports of default vlan */
    bcm_pbmp_t defl_ubmp; /* Untagged ports of default vlan */
    int        count;     /* Number of existing VLANs */
    SOC_PPC_LIF_ID local_lif_index_drop; /* Local lif index of the vlan drop lif. */
    SOC_PPC_LIF_ID local_lif_index_simple; /* Local lif index of the vlan simple lif (vsi == vlan). */
    SOC_PPC_LIF_ID local_lif_rch; /* Local lif index of the RCH port default lif (vsi == vlan). */
} bcm_dpp_vlan_info_t;

/* struct used to maintain reference count to used FIDs */
typedef struct {
    int ref_count;
    int fid;
} fid_ref_count_t;

/*
 *  Working state description (per unit)
 */
typedef struct _bcm_dpp_vlan_unit_state_s {
    bcm_dpp_vlan_info_t vlan_info;
    fid_ref_count_t fid_ref_count[DPP_NOF_SHARED_FIDS];
    _bcm_dpp_vlan_translate_action_t edit_ing_action[SOC_PPC_NOF_INGRESS_VLAN_EDIT_ACTION_IDS];
    _bcm_dpp_vlan_translate_action_t edit_eg_action[SOC_PPC_MAX_NOF_EGRESS_VLAN_EDIT_ACTION_IDS];
    uint16 edit_eg_action_mapping[SOC_PPC_MAX_NOF_EGRESS_VLAN_EDIT_ACTION_MAPPINGS];
} _bcm_dpp_vlan_unit_state_t;


uint32
  _sand_multiset_buffer_get_entry(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  uint32                             sec_hanlde,
    SOC_SAND_IN  uint8                              *buffer,
    SOC_SAND_IN  uint32                             offset,
    SOC_SAND_IN  uint32                             len,
    SOC_SAND_OUT uint8                              *data
  );

uint32
  _sand_multiset_buffer_set_entry(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  uint32                             sec_hanlde,
    SOC_SAND_INOUT  uint8                           *buffer,
    SOC_SAND_IN  uint32                             offset,
    SOC_SAND_IN  uint32                             len,
    SOC_SAND_IN  uint8                              *data
  );

/* VLAN SW state */
int _bcm_dpp_in_lif_ac_match_get(int unit, bcm_vlan_port_t *vlan_port, int in_lif);

/*
 * set STP state for port and topology
 */
int
_bcm_ppd_stg_stp_port_set(int unit, bcm_stg_t stg_id, bcm_port_t port, int stp_state);

int
_bcm_ppd_stg_stp_port_get(int unit, bcm_stg_t stg_id, bcm_port_t port, int *stp_state);

int
_bcm_petra_vlan_flooding_per_lif_set(int unit, bcm_port_t port, SOC_PPC_LIF_ID lif_index, int is_port_flooding,  
                                     int is_lif_flooding, bcm_gport_t unknown_unicast_group, 
                                     bcm_gport_t unknown_multicast_group, bcm_gport_t broadcast_group);

int
_bcm_petra_vlan_flooding_per_lif_get(int unit, bcm_port_t port, SOC_PPC_LIF_ID lif_index, int is_port_flooding,
                                     int is_lif_flooding, bcm_gport_t *unknown_unicast_group, 
                                     bcm_gport_t *unknown_multicast_group, bcm_gport_t *broadcast_group);

int 
_bcm_petra_vlan_stg_set(int unit, bcm_vlan_t vid, bcm_stg_t stg);

typedef struct bcm_dpp_vlan_egress_priority_mapping_s {
    /* Priority (PCP & DEI) */
        SOC_SAND_PP_PCP_UP pcp; 
        SOC_SAND_PP_DEI_CFI dei; 
} bcm_dpp_vlan_egress_priority_mapping_t;

typedef struct bcm_dpp_vlan_egress_priority_mapping_table_info_s {
    /* Priority (PCP & DEI) per TC * DP */
    bcm_dpp_vlan_egress_priority_mapping_t    pcp_dei[SOC_SAND_PP_TC_MAX + 1][SOC_SAND_PP_DP_MAX + 1];
} bcm_dpp_vlan_egress_priority_mapping_table_info_t;

typedef struct bcm_dpp_vlan_edit_profile_mapping_info_s {
        /* 
         *  Edit Profile defined by the mapping:
         *  Per VLAN Structure, which IVEC attributes
         */
    SOC_PPC_LIF_ING_VLAN_EDIT_COMMAND_INFO ivec[SOC_SAND_PP_NOF_ETHERNET_FRAME_VLAN_FORMATS];
} bcm_dpp_vlan_edit_profile_mapping_info_t;

typedef struct bcm_dpp_vlan_egress_edit_profile_info_s {
        /* 
         *  Egress Edit Profile 
         *  Per VLAN Structure, which EVEC attributes
         */
    SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO evec[SOC_SAND_PP_NOF_ETHERNET_FRAME_VLAN_FORMATS];
} bcm_dpp_vlan_egress_edit_profile_info_t;

typedef struct bcm_dpp_vlan_flooding_profile_info_s {

  /* 
   *  Unknown Unicast addition in case of flooding
   */
  int unknown_uc_add;
  /* 
   *  Unknown Multicast addition in case of flooding
   */
  int unknown_mc_add;
  /* 
   *  Unknown BC addition in case of flooding
   */
  int bc_add;
} bcm_dpp_vlan_flooding_profile_info_t;

typedef struct bcm_dpp_vlan_port_protocol_profile_info_s {
  /* 
   *  Ethertype
   */
  uint16 ethertype;
  /* 
   *  Other data including: valid_entry(1bit), vlan (12bit), vlan_is_valid (1bit), tc (3bit), tc_is_valid(1bit)
   */
  uint32 vlan_tc_data;

} bcm_dpp_vlan_port_protocol_profile_info_t;

typedef struct bcm_dpp_vlan_port_protocol_entries_s {
   bcm_dpp_vlan_port_protocol_profile_info_t port_protocol_entry[DPP_PRTCL_NOF_ETHER_TYPE_IDS];
} bcm_dpp_vlan_port_protocol_entries_t;

void 
_bcm_dpp_vlan_egress_edit_profile_info_t_init(bcm_dpp_vlan_egress_edit_profile_info_t *info);

void 
_bcm_dpp_vlan_edit_profile_mapping_info_t_init(bcm_dpp_vlan_edit_profile_mapping_info_t *info);

int
_bcm_petra_vlan_edit_profile_info_hw_set(int unit,
                     int vlan_edit_profile,
                     bcm_dpp_vlan_edit_profile_mapping_info_t *mapping_info);

extern int
bcm_petra_vlan_detach(int unit);

int _bcm_dpp_vlan_port_create(int unit, bcm_vlan_port_t *vlan_port);

void
_bcm_petra_vlan_edit_command_hw_get(int unit,
                                    int vlan_edit_profile,
                                    SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT vlan_format,
                                    int *ivec_id,
                                    uint8 *is_ivec_to_set,
                                    uint8 *is_vlan_format_valid);


int 
_bcm_petra_vlan_translate_action_add(
    int unit, 
    bcm_gport_t port, 
    bcm_vlan_translate_key_t key_type, 
    bcm_vlan_t outer_vlan, 
    bcm_vlan_t inner_vlan, 
    bcm_vlan_action_set_t *action);

int
_bcm_petra_vlan_translate_match_tpid_value_to_index(int unit, int tpid_value, SOC_PPC_LLP_PARSE_TPID_VALUES* tpid_vals, int* tpid_index);

int
_bcm_petra_vlan_translate_match_tpid_index_to_value(int unit, int tpid_index, SOC_PPC_LLP_PARSE_TPID_VALUES* tpid_vals, uint16* tpid_value);

/* 
 * Advanced VLAN editing: action SW table Get/Set/Init functions
 */

/* Initialize BCM DPP VLAN editing action SW table structure.
   The structure is used in order to store a VLAN translation action in the SW */
void 
_bcm_dpp_vlan_translate_action_t_init(_bcm_dpp_vlan_translate_action_t *action);

/* Set a BCM DPP VLAN editing action structure that is stored in a SW actions table */
int
_bcm_dpp_vlan_edit_action_set(
    int unit,
    int action_id,
    uint32 is_ingress,
    _bcm_dpp_vlan_translate_action_t *action);

/* Get a BCM DPP VLAN editing action structure that is stored in a SW actions table */
int
_bcm_dpp_vlan_edit_action_get(
    int unit,
    int action_id,
    uint32 is_ingress,
    _bcm_dpp_vlan_translate_action_t *action);

/* Get tag format and profile from from action ID that is stored in  SW Egress action mapping table. */
int
_bcm_petra_vlan_edit_eg_command_id_find(
    int unit,
    uint32 action_id,
    int find_from_current,
    SOC_PPC_EG_VLAN_EDIT_COMMAND_KEY *command_key);

/* Given a vlan port and a pointer to lif_ac_keu struct, fills the lif_ac_key struct according to the vlan port. */
int _bcm_dpp_gport_fill_ac_key(
    int                  unit,
    void                 *gport, 
    int                  gport_type, 
    SOC_PPC_L2_LIF_AC_KEY  *in_ac_key);

/*
 * given gport return out_ac_key where this gport is defined
 */
int _bcm_dpp_gport_fill_out_ac_key(
    int                  unit,
    bcm_vlan_port_t      *vlan_port,     
    uint8                *is_cep,
    SOC_PPC_EG_AC_VBP_KEY *vbp_key,
    SOC_PPC_EG_AC_CEP_PORT_KEY *cep_key
 );

/*
 * Convert the VLAN Action to the VLAN Edit Profile Info
 */
int _bcm_petra_vlan_action_to_edit_profile_info_build(
    int unit,
    bcm_vlan_action_set_t *action_set,
    uint32 tpid_profile,
    int is_ingress,
    bcm_dpp_vlan_edit_profile_mapping_info_t *mapping_info,
    bcm_dpp_vlan_egress_edit_profile_info_t *eg_mapping_info,
    uint8 is_extender);

/*
 * Set in the HW the Egress VLAN Edit profile attributes
 */
int _bcm_petra_vlan_edit_profile_info_eg_hw_set(
    int unit,
    int vlan_edit_profile_eg,
    bcm_dpp_vlan_egress_edit_profile_info_t *eg_edit_profile_set);

/*
 *    get vlan port learn fec for mc group
 */
int bcm_petra_vlan_port_get_mc_fec(
    int unit,
    bcm_gport_t gport,
    SOC_PPC_FEC_ID *fec_id);

 int
_bcm_petra_vlan_translation_tpids_to_tpid_profile(
    int unit,
    uint16 *tpids,
    int nof_tpids,
    int inner_tpid_only,
    uint32 *tpid_profile);

#endif    /* !_BCM_INT_DPP_VLAN_H */
