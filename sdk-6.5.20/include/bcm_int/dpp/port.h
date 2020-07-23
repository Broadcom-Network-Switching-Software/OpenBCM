/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        port.h
 * Purpose:     PORT internal definitions to the BCM library.
 */

#ifndef   _BCM_INT_DPP_PORT_H_
#define   _BCM_INT_DPP_PORT_H_

#include <soc/dpp/PPD/ppd_api_general.h>
#include <soc/dpp/PPD/ppd_api_llp_trap.h>
#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>
#include <soc/dpp/PPC/ppc_api_trap_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_occupation_mgmt.h>

#include <soc/dpp/TMC/tmc_api_action_cmd.h>
#include <bcm/port.h>
#include <soc/types.h>

/*
 * port definition
 */

/* color mapping (DP value)*/
#define _BCM_DPP_COLOR_GREEN   0
#define _BCM_DPP_COLOR_YELLOW  1
#define _BCM_DPP_COLOR_RED     2
#define _BCM_DPP_COLOR_BLACK   3
#define _BCM_DPP_COLOR_PRESERVE  4

/* number of FAP ports */
#define _BCM_PETRA_NOF_LOCAL_PORTS(unit) (SOC_DPP_DEFS_GET(unit, nof_local_ports))
#define _BCM_PETRA_NOF_TM_PORTS(unit)    (SOC_DPP_DEFS_GET(unit, nof_logical_ports))
#define _BCM_PETRA_NOF_MAX_PP_PORTS      (SOC_PPC_MAX_NOF_LOCAL_PORTS_ARAD)
/* number of traps used for learning mode per port*/
#define _BCM_PETRA_PORT_LEARN_NOF_TRAPS 4

#define _BCM_DPP_PORT_TRAP_CODE_SA_DROP_NOF_PROFILES 4
#define _BCM_DPP_PORT_TRAP_CODE_DA_UNKNOWN_NOF_PROFILES 4

#define _BCM_DPP_PORT_DISCARD_MODE_MIM_BIT              (0x40)
#define _BCM_DPP_PORT_DISCARD_MODE_OUTER_PRIO           (0x80)
#define _BCM_DPP_PORT_DISCARD_MODE_MAX                  ((BCM_PORT_DISCARD_COUNT - 1) | _BCM_DPP_PORT_DISCARD_MODE_MIM_BIT | _BCM_DPP_PORT_DISCARD_MODE_OUTER_PRIO)
#define _BCM_DPP_PORT_DISCARD_MODE_IS_MIM(_mode_)       (_mode_ & _BCM_DPP_PORT_DISCARD_MODE_MIM_BIT)       
#define _BCM_DPP_PORT_DISCARD_MODE_MIM_REMOVE(_mode_)   (_mode_ &= ~_BCM_DPP_PORT_DISCARD_MODE_MIM_BIT)       
#define _BCM_DPP_PORT_DISCARD_MODE_MIM_SET(_mode_)      (_mode_ |= _BCM_DPP_PORT_DISCARD_MODE_MIM_BIT)  
#define _BCM_DPP_PORT_DISCARD_MODE_IS_OUTER_PRIO(_mode_) (_mode_ & _BCM_DPP_PORT_DISCARD_MODE_OUTER_PRIO)       
#define _BCM_DPP_PORT_DISCARD_MODE_OUTER_PRIO_REMOVE(_mode_) (_mode_ &= ~_BCM_DPP_PORT_DISCARD_MODE_OUTER_PRIO)       
#define _BCM_DPP_PORT_DISCARD_MODE_OUTER_PRIO_SET(_mode_) (_mode_ |= _BCM_DPP_PORT_DISCARD_MODE_OUTER_PRIO)

#define _BCM_DPP_PORT_PRD_ETH_MAP_INDICATION  _SHR_PORT_PRD_ETH_MAP_INDICATION
#define _BCM_DPP_PORT_PRD_TM_MAP_INDICATION   _SHR_PORT_PRD_TM_MAP_INDICATION
#define _BCM_DPP_PORT_PRD_IP_MAP_INDICATION   _SHR_PORT_PRD_IP_MAP_INDICATION
#define _BCM_DPP_PORT_PRD_MPLS_MAP_INDICATION _SHR_PORT_PRD_MPLS_MAP_INDICATION

#define _BCM_COSQ_INGRESS_PORT_DROP_ETH_MAP_KEY_SET(key)  ( key |= _BCM_DPP_PORT_PRD_ETH_MAP_INDICATION ) 
#define _BCM_COSQ_INGRESS_PORT_DROP_TM_MAP_KEY_SET(key)   ( key |= _BCM_DPP_PORT_PRD_TM_MAP_INDICATION ) 
#define _BCM_COSQ_INGRESS_PORT_DROP_IP_MAP_KEY_SET(key)   ( key |= _BCM_DPP_PORT_PRD_IP_MAP_INDICATION ) 
#define _BCM_COSQ_INGRESS_PORT_DROP_MPLS_MAP_KEY_SET(key) ( key |= _BCM_DPP_PORT_PRD_MPLS_MAP_INDICATION ) 

#define _BCM_COSQ_INGRESS_PORT_DROP_ETH_MAP_KEY_GET(key)  ( key & _BCM_DPP_PORT_PRD_ETH_MAP_INDICATION ) 
#define _BCM_COSQ_INGRESS_PORT_DROP_TM_MAP_KEY_GET(key)   ( key & _BCM_DPP_PORT_PRD_TM_MAP_INDICATION ) 
#define _BCM_COSQ_INGRESS_PORT_DROP_IP_MAP_KEY_GET(key)   ( key & _BCM_DPP_PORT_PRD_IP_MAP_INDICATION ) 
#define _BCM_COSQ_INGRESS_PORT_DROP_MPLS_MAP_KEY_GET(key) ( key & _BCM_DPP_PORT_PRD_MPLS_MAP_INDICATION ) 

#define _BCM_COSQ_INGRESS_PORT_DROP_ETH_MAP_KEY_CLEAR(key)  ( key &= ~_BCM_DPP_PORT_PRD_ETH_MAP_INDICATION ) 
#define _BCM_COSQ_INGRESS_PORT_DROP_TM_MAP_KEY_CLEAR(key)   ( key &= ~_BCM_DPP_PORT_PRD_TM_MAP_INDICATION ) 
#define _BCM_COSQ_INGRESS_PORT_DROP_IP_MAP_KEY_CLEAR(key)   ( key &= ~_BCM_DPP_PORT_PRD_IP_MAP_INDICATION ) 
#define _BCM_COSQ_INGRESS_PORT_DROP_MPLS_MAP_KEY_CLEAR(key) ( key &= ~_BCM_DPP_PORT_PRD_MPLS_MAP_INDICATION ) 

/* user define trap used for drop */
#define _BCM_PETRA_UD_DROP_TRAP (SOC_PPC_TRAP_CODE_USER_DEFINED_DROP_TRAP)
#define _BCM_PETRA_UD_DFLT_TRAP (SOC_PPC_TRAP_CODE_USER_DEFINED_DFLT_TRAP)

typedef struct bcm_dpp_mal_assoc_s {
    uint8 mal;
    uint8 lanes;
    uint16 nif_type;
} bcm_dpp_mal_assoc_t;

typedef struct bcm_dpp_user_defined_traps_s {
    SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO trap;
    uint32 snoop_cmd;
    uint32 gport_trap_id; /*Added for support: bcmRxTrapL2Cache (virtual traps) */
    uint32 ud_unique; /*Added for support: sync user define traps in rx.c with port.c */
} bcm_dpp_user_defined_traps_t;


typedef struct bcm_dpp_snoop_s {
	SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO snoop_action_info; /*snoop profile data */
	uint32 snoop_unique; /*Added for support: sync snoop between diffrent modules */
} bcm_dpp_snoop_t;


/* number of bits per tag-structure
    0:0  PcpDeiProfile  This is the port-based PCP-DEI mapping profile.
    6:1  Ivec  This is the port-based ingress VLAN edit command. 19:16  
    10:7 IncomingTagStructure  Used together with the VLAN-Edit-Profile 
    11:11 discard
    12:12: outer-cep
    13:13 inner-cep
    19:14 reserved
*/
#define _BCM_DPP_PORT_TPID_CLASS_BITS_PER_TAG_STRCT (20)

/* number of U32 need, for all tag structures 2^5 = 32 ==> 20 U32 */
#define _BCM_DPP_PORT_TPID_CLASS_INFO_BUF_NOF_U32 (20)

typedef struct _bcm_dpp_port_tpid_class_info_s {
    uint32 buff[_BCM_DPP_PORT_TPID_CLASS_INFO_BUF_NOF_U32];
} _bcm_dpp_port_tpid_class_info_t;



typedef enum _bcm_petra_tpid_profile_e{
    _bcm_petra_tpid_profile_none = 0,
    _bcm_petra_tpid_profile_outer,
    _bcm_petra_tpid_profile_outer_inner,
    _bcm_petra_tpid_profile_outer_inner2,
    _bcm_petra_tpid_profile_outer_outer,
    _bcm_petra_tpid_profile_inner_outer,
    _bcm_petra_tpid_profile_inner_outer2, /* TPID1: outer + inner TPID2:outer */
    _bcm_petra_tpid_profile_outer_inner_same, /* TPID1: outer TPID1: inner */
    _bcm_petra_tpid_profile_outer_inner_same2, /* TPID2: outer TPID2: inner */
    _bcm_petra_tpid_profile_outer_c_tag, /* TPID1: C-tag*/
    _bcm_petra_tpid_profile_outer_outer_c_tag, /* TPID1: C-tag TPID2: C-tag*/
    _bcm_petra_tpid_profile_outer_inner_c_tag, /* TPID1: S-tag, TPID2: C-tag */
    _bcm_petra_tpid_profile_outer_trill_fgl, /* TPID1: S-tag, TPID2: C-tag */
    _bcm_petra_tpid_profile_count
} _bcm_petra_tpid_profile_t;

typedef enum _bcm_petra_dtag_mode_e{
    _bcm_petra_dtag_mode_accept = 0, /* Accept double tag */
    _bcm_petra_dtag_mode_ignore, /* Ignore double tag. Parse as one tag */    
    _bcm_petra_dtag_mode_count
} _bcm_petra_dtag_mode_t;

typedef enum _bcm_petra_ac_key_map_type_e{
    _bcm_petra_ac_key_map_type_normal = 0,
    _bcm_petra_ac_key_map_type_compressional,
    _bcm_petra_ac_key_map_type_count
} _bcm_petra_ac_key_map_type_t;

typedef enum _bcm_petra_port_mact_mgmt_action_profile_e{
    _bcm_petra_port_mact_mgmt_action_profile_none = 0,
    _bcm_petra_port_mact_mgmt_action_profile_drop,
    _bcm_petra_port_mact_mgmt_action_profile_trap,
    _bcm_petra_port_mact_mgmt_action_profile_trap_and_drop    
} _bcm_petra_port_mact_mgmt_action_profile_t;



typedef enum _bcm_dpp_port_map_type_e {
   _bcm_dpp_port_map_type_up_to_dp=0,/* mapping UP to DP */         
   _bcm_dpp_port_map_type_in_up_to_tc_and_de,
   _bcm_dpp_port_map_type_count       
} _bcm_dpp_port_map_type_t;


int _bcm_petra_outlif_profile_set(
   const int unit, 
   const uint32 local_outlif_id, 
   const uint32 arg, 
   const SOC_OCC_MGMT_OUTLIF_APP kind);


int _bcm_petra_port_outlif_profile_set(
   const int unit, 
   const bcm_port_t port, 
   const uint32 arg, 
   const SOC_OCC_MGMT_OUTLIF_APP kind);

#define _BCM_DPP_PORT_MAP_MAX_NOF_TBLS   (8)



/* number of different tpid values */
#define _BCM_PORT_NOF_TPID_VALS (4)
/* number of TPID profiles pair of TPIDs */
#define _BCM_PORT_NOF_TPID_PROFILES (4)

#define _BCM_PETRA_NOF_TPIDS_PER_PORT (2)

/* The NOF of usable TPIDs per port include the number of TPIDs per Port-Profile
   and possibly the MIM TPID */
#define _BCM_DPP_MAX_NOF_USABLE_TPIDS_PER_PORT (_BCM_PETRA_NOF_TPIDS_PER_PORT + 1)


typedef struct {

    uint16 tpid[_BCM_PETRA_NOF_TPIDS_PER_PORT][_BCM_PORT_NOF_TPID_PROFILES]; /* tpid 1/2 index <value> */
    int tpid_count[_BCM_PETRA_NOF_TPIDS_PER_PORT][_BCM_PORT_NOF_TPID_PROFILES]; /* number of users of this tpid in the profile */

} bcm_petra_port_tpid_info;





typedef struct bcm_dpp_port_config_s {

    int bcm_petra_port_init_arrays_done;

    int port_pp_initialized;

    int map_tbl_use[_bcm_dpp_port_map_type_count][_BCM_DPP_PORT_MAP_MAX_NOF_TBLS];

    /* for each learn trap what is the usage */
    int trap_to_flag[_BCM_PETRA_PORT_LEARN_NOF_TRAPS];

    bcm_petra_port_tpid_info *bcm_port_tpid_info;

    uint32 in_port_key_gen_var_32_msb[MAX_NUM_OF_CORES][SOC_TMC_NOF_FAP_PORTS_MAX];
} bcm_dpp_port_config_t;


typedef struct _bcm_dpp_port_tpid_class_acceptable_frame_type_info_s {
    uint32 buffer;
} _bcm_dpp_port_tpid_class_acceptable_frame_type_info_t;


extern pbmp_t _bcm_dpp_port_prbs_mac_mode[BCM_MAX_NUM_UNITS];
extern bcm_dpp_port_config_t _dpp_port_config[BCM_MAX_NUM_UNITS];

typedef struct {
    uint8 tpids_count[MAX_NUM_OF_CORES][SOC_TMC_NOF_FAP_PORTS_MAX];
    bcm_dpp_port_config_t dpp_port_config;
    pbmp_t prbs_mac_mode;
} bcm_dpp_port_info_t;
/*
 * External functions
 */
extern int bcm_petra_port_advert_remote_get(int unit, bcm_port_t port, bcm_port_abil_t *ability_mask);

int 
_bcm_petra_port_class_nof_vlan_domains_count(
    int unit, 
    uint32 class_id,
    int *nof_pp_ports);

extern uint32
soc_petra_ihp_prog_n00_load_unsafe(int unit);

extern int
_bcm_petra_port_link_get(int unit, bcm_port_t port, int hw, int *up);

extern int
_bcm_petra_port_link_get_and_hook_placeholder (int unit, bcm_port_t port, int hw, int *up);

extern int
_bcm_petra_port_find_free_port_and_allocate(int unit, bcm_core_t core, bcm_port_if_t interface, int with_id, bcm_port_t *port);

/* These functions below are required for cosq TM */

extern int
bcm_petra_port_vlan_priority_map_set(int unit, bcm_port_t port, int pkt_pri, int cfi, int internal_pri, bcm_color_t color);

extern int
bcm_petra_port_vlan_priority_map_get(int unit, bcm_port_t port, int pkt_pri, int cfi, int *internal_pri, bcm_color_t *color);

extern int
bcm_petra_port_vlan_priority_unmap_set(int unit, bcm_port_t port, int internal_pri, bcm_color_t color, int pkt_pri, int cfi);

extern int
bcm_petra_port_vlan_priority_unmap_get(int unit, bcm_port_t port, int internal_pri, bcm_color_t color, int *pkt_pri, int *cfi);

extern int
bcm_petra_port_vlan_pri_map_get(int unit, bcm_port_t port, bcm_vlan_t vid, int pkt_pri, int cfi, int *internal_pri, bcm_color_t *color);

extern int
bcm_petra_port_vlan_pri_map_set(int unit, bcm_port_t port, bcm_vlan_t vid, int pkt_pri, int cfi, int internal_pri, int color);

extern int
_bcm_petra_port_color_encode(int, bcm_color_t, int *);

extern int 
_bcm_petra_port_color_decode(int unit, int dp, bcm_color_t *color);


#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern int
_bcm_dpp_port_sw_dump(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */
 	 
int
_bcm_petra_port_discard_extend_dtag_mode_set(int unit,bcm_port_t port,_bcm_petra_dtag_mode_t dtag_mode);

int
_bcm_petra_port_discard_extend_get(int unit, bcm_port_t port, int *mode);

int
_bcm_petra_port_discard_extend_dtag_mode_get(int unit,bcm_port_t port,_bcm_petra_dtag_mode_t *dtag_mode);

int 
_bcm_petra_port_discard_extend_mode_set(int unit, bcm_port_t port, int mode);

int
bcm_petra_port_tpid_profile_exact_match_search(int unit, int index, uint16 tpids[_BCM_PETRA_NOF_TPIDS_PER_PORT], int nof_tpids, int inner_only, int* is_exact_match);

int
bcm_petra_port_tpid_profile_first_match_search(int unit, int index, uint16 tpids[_BCM_PETRA_NOF_TPIDS_PER_PORT], int nof_tpids, int inner_only, int* is_exact_match);

int
bcm_petra_port_tpid_profile_opposite_match_search(int unit, int indx, uint16 tpids[_BCM_PETRA_NOF_TPIDS_PER_PORT], int nof_tpids, int inner_only, int* is_exact_match);

int
bcm_petra_port_tpid_profile_similar_match_search(int unit, int tpid_profile_idx, uint16 tpids[_BCM_PETRA_NOF_TPIDS_PER_PORT], int nof_tpids, int inner_only, int* is_match);


/* given a lif, update its tpid profile index (to match given tpid) */
int
_bcm_petra_mim_tpid_profile_update(int unit, int lif, uint16 tpid);


extern int
bcm_petra_port_deinit(int unit);

/* Attributes taht can be controlled on BCM88650*/
#define _BCM_DPP_PORT_ATTRS     \
    (BCM_PORT_ATTR_ENABLE_MASK      | \
    BCM_PORT_ATTR_SPEED_MASK       | \
    BCM_PORT_ATTR_DUPLEX_MASK      | \
    BCM_PORT_ATTR_LINKSCAN_MASK    | \
    BCM_PORT_ATTR_VLANFILTER_MASK  | \
    BCM_PORT_ATTR_INTERFACE_MASK   | \
    BCM_PORT_ATTR_LOOPBACK_MASK    | \
    BCM_PORT_ATTR_STP_STATE_MASK   | \
    BCM_PORT_ATTR_PAUSE_TX_MASK    | \
    BCM_PORT_ATTR_PAUSE_RX_MASK)

int 
bcm_petra_tpid_profile_info_set(int unit, int port_profile,int port_profile_eg, _bcm_petra_tpid_profile_t profile_type, int accept_mode, _bcm_petra_dtag_mode_t dtag_mode, _bcm_petra_ac_key_map_type_t ac_key_map_type, uint8 cep_port, uint8 evb_port);

int _bcm_dpp_port_tpid_class_acceptable_frame_type_info_t_clear(int unit, _bcm_dpp_port_tpid_class_acceptable_frame_type_info_t *tpid_class_acceptable_frame_info);

void _bcm_dpp_port_tpid_class_info_t_to_bcm_dpp_port_tpid_class_acceptable_frame_type_info_t(int unit, _bcm_dpp_port_tpid_class_info_t *tpid_class_info, _bcm_dpp_port_tpid_class_acceptable_frame_type_info_t *tpid_class_acceptable_frame_info);

int bcm_petra_port_tpid_profile_alloc(int unit, uint16 tpids[_BCM_PETRA_NOF_TPIDS_PER_PORT], int nof_tpids, int inner_only, int *tpid_profile);

int _bcm_petra_port_nrdy_thresh_allocate(int unit, soc_port_t port);

int _bcm_petra_port_nrdy_thresh_free(int unit, soc_port_t port);

int  _bcm_petra_port_traps_init(int unit);

int _bcm_dpp_port_reassembly_context_get(int unit, bcm_port_t port, uint32 *port_termination_context, uint32 *reassembly_context);

/*
 * Function:
 *       _bcm_dpp_mod_port_to_sys_port
 * Description:
 *       map module-ID + mod-port to system port
 * Parameters:
 *       unit -    [IN] DPP device unit number (driver internal).
 *  modid -   [IN] module id
 *  mode_port -     [IN] module user port
 *  sys_port -     [OUT] unique System port, or SOC_TMC_SYS_PHYS_PORT_INVALID if
 *                                      given module or port on module not exist
 * Returns:
 *       BCM_E_XXX
 */

int 
 _bcm_dpp_port_mod_port_to_sys_port(int unit, bcm_module_t modid, bcm_port_t mode_port, unsigned int *sys_port);

/*
 * Function:
 *	_bcm_dpp_mod_port_from_sys_port
 * Description:
 *    map module-ID + mod-port to system port
 * Parameters:
 *    unit -    [IN] DPP device unit number (driver internal).
 *  modid -   [IN] module id
 *  mode_port -     [IN] module user port
 *  sys_port -     [OUT] unique System port, or SOC_TMC_SYS_PHYS_PORT_INVALID if
 *                       given module or port on module not exist
 * Returns:
 *    BCM_E_XXX
 */
int 
 _bcm_dpp_port_mod_port_from_sys_port(int unit, bcm_module_t *modid, bcm_port_t *mode_port, uint32 sys_port);


/*
 * Function:
 *      _bcm_dpp_port_pbmp_arm_get
 * Purpose:
 *      Get port pbmp for ARM core
 * Parameters:
 *      unit    - (IN) Device Number
 *      bcm_pbmp_t - (OUT) arm pbmp
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_dpp_port_pbmp_arm_get(int unit, bcm_pbmp_t *arm_pbmp);

/*
 * Function:
 *      _bcm_petra_port_nif_priority_set
 * Purpose:
 *      Set nif priority
 * Parameters:
 *      unit            - (IN) Device Number
 *      local_port      - (IN) port for which to set priority 
 *      flags           - (IN)
 *      allow_tdm       - (IN) allow set with tdm priority
 *      priority        - (IN) priority to set
 *      affected_ports  - (OUT other ports that were affected by this configuration
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_petra_port_nif_priority_set(int unit, bcm_gport_t local_port, uint32 flags, uint32 allow_tdm, bcm_port_nif_prio_t* priority, bcm_pbmp_t* affected_ports );


int
_bcm_petra_port_fabric_detach(int unit,
          bcm_pbmp_t pbmp,
          bcm_pbmp_t *detached);


int _bcm_petra_port_outlif_profile_set_pwe_port(
   const int unit, 
   const bcm_port_t port, 
   const uint32 arg, 
   const SOC_OCC_MGMT_OUTLIF_APP kind);



#define _BCM_DPP_PORT_DISCARD_MODE_BOTH         (0)
#define _BCM_DPP_PORT_DISCARD_MODE_EGRESS_ONLY  (1) /* Discard state should be set for Out-LIF only */
#define _BCM_DPP_PORT_DISCARD_MODE_INGRESS_ONLY (2) /* Discard state should be set for In-LIF only */


#define _BCM_DPP_PORT_DISCARD_MODE_SHIFT    (14)
#define _BCM_DPP_PORT_DISCARD_VAL_SHIFT     (0)
#define _BCM_DPP_PORT_DISCARD_MODE_MASK     (0x3)
#define _BCM_DPP_PORT_DISCARD_VAL_MASK      (0x3FFF)

#define DPP_PORT_DISCARD_VAL_GET(_mode)     ((_mode >> _BCM_DPP_PORT_DISCARD_VAL_SHIFT ) & _BCM_DPP_PORT_DISCARD_VAL_MASK)
#define DPP_PORT_DISCARD_MODE_GET(_mode)    ((_mode >> _BCM_DPP_PORT_DISCARD_MODE_SHIFT) & _BCM_DPP_PORT_DISCARD_MODE_MASK)

#endif /* _BCM_INT_PETRA_PORT_H_ */
