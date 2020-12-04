/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Trill
 */

#ifndef _BCM_INT_DPP_TRILL_H
#define _BCM_INT_DPP_TRILL_H

#include <shared/swstate/sw_state.h>
#include <sal/types.h>
#include <bcm/types.h>
#include <soc/dpp/PPD/ppd_api_general.h>
#include <shared/swstate/sw_state_hash_tbl.h>

#define _BCM_PETRA_TRILL_MC_ROOT_NOF_SRC_RBRIDGE  10
#define _BCM_PETRA_TRILL_MC_ROOT_NOF_VLAN         20

#define _BCM_PETRA_TRILL_PORT_UNI                         0
#define _BCM_PETRA_TRILL_PORT_MC                          1
#define _BCM_PETRA_TRILL_PORT_MULTIPATH                   2

#define _BCM_TRILL_PORTS_ALLOC_SIZE                20

/* Used to identify trill fgl's tpid profile: outer tpid profile = inner tpid profile = 0x893B */
#define _BCM_PETRA_TRILL_NATIVE_TPID  (0x893B)

/* Default trill ethertype for device bringup */
#define _BCM_PETRA_TRILL_ETHERTYPE_DEFAULT (0x22f3)

/* virtual nickname gport define and macros */
#define _BCM_DPP_GPORT_TRILL_VIRTUAL_NICK_NAME_INDEX_MASK  0x3
#define _BCM_DPP_GPORT_TRILL_VIRTUAL_NICK_NAME_INDEX_SHIFT 16
#define _BCM_DPP_GPORT_TRILL_NICK_NAME_MASK                0xFFFF

/*trill port id: encoding: 0:15: nickname, 16:17: virtual index */
#define _BCM_DPP_GPORT_TRILL_VIRTUAL_NICK_NAME_PORT_ID_SET(_gport, index, nick_name)  \
    BCM_GPORT_TRILL_PORT_ID_SET(_gport, nick_name | \
       (((index) & _BCM_DPP_GPORT_TRILL_VIRTUAL_NICK_NAME_INDEX_MASK) << _BCM_DPP_GPORT_TRILL_VIRTUAL_NICK_NAME_INDEX_SHIFT))

#define _BCM_DPP_GPORT_TRILL_NICKNAME_GET(_gport) \
       (_gport & _BCM_DPP_GPORT_TRILL_NICK_NAME_MASK)

#define _BCM_DPP_GPORT_TRILL_VIRTUAL_NICK_NAME_INDEX_GET(_gport) \
       (((_gport) >> _BCM_DPP_GPORT_TRILL_VIRTUAL_NICK_NAME_INDEX_SHIFT) & _BCM_DPP_GPORT_TRILL_VIRTUAL_NICK_NAME_INDEX_MASK)

/* gport trill is virtual nickname if has an index != 0 (virtual nickname index) */
#define BCM_GPORT_TRILL_PORT_ID_IS_VIRTUAL(_gport) \
       _BCM_DPP_GPORT_TRILL_VIRTUAL_NICK_NAME_INDEX_GET(_gport)

/* gport trill is egress trill gport if:
 * - is trill gport
   - has a sub-type (aka encap-type): trill outlif */
#define BCM_GPORT_TRILL_PORT_ID_IS_EGRESS(_gport) \
       BCM_GPORT_IS_TRILL_PORT(_gport) && BCM_GPORT_SUB_TYPE_IS_TRILL_OUTLIF(_gport)


typedef struct _bcm_petra_trill_port_list_s {
    int                             allocated_cnt; 
    int                             port_cnt;  
    PARSER_HINT_ARR bcm_gport_t     *ports; 
} bcm_petra_trill_port_list_t;


/* Trill State structure */
typedef struct bcm_dpp_trill_state_s {
    int                                         init;
    int                                         mask_set;
    SOC_PPC_AC_ID                               trill_out_ac;   
    SOC_PPC_LIF_ID                              trill_local_in_lif;
    int                                         trill_global_in_lif;
    int                                         last_used_id;   
    PARSER_HINT_PTR bcm_petra_trill_port_list_t *trill_ports;
    /* For each root a list of sources is saved*/
    int                                         mc_trill_route_info_db_htb_handle;
    /* For each root a list of routes is saved */
    int                                         mc_trill_root_src_db_htb_handle;
} bcm_dpp_trill_state_t;

/*The source rbridge info*/
typedef struct _bcm_petra_trill_mc_src_info_s {
    bcm_trill_name_t    src_rbridge; 
    bcm_gport_t         port; 
} _bcm_petra_trill_mc_src_info_t;


typedef struct _bcm_petra_trill_mc_trill_src_list_s {
     int    nof_src_rbridge;
    _bcm_petra_trill_mc_src_info_t src_rbridge[_BCM_PETRA_TRILL_MC_ROOT_NOF_SRC_RBRIDGE ];
} _bcm_petra_trill_mc_trill_src_list_t;


/*The route info*/
typedef struct _bcm_petra_trill_mc_route_info_s {
    uint32              flags; 
    bcm_trill_name_t    root_name; 
    bcm_vlan_t          vlan; 
    bcm_multicast_t     group;
} _bcm_petra_trill_mc_route_info_t;

typedef struct _bcm_petra_trill_mc_route_list_s {
     int    nof_routes;
    _bcm_petra_trill_mc_route_info_t route[_BCM_PETRA_TRILL_MC_ROOT_NOF_VLAN];
} _bcm_petra_trill_mc_trill_route_list_t;

typedef struct _bcm_petra_trill_info_s {
    int type;
    int fec_id;
    int use_cnt;  /* Used for fecless port (id = name) - Counts  ports that share the same name */
} _bcm_petra_trill_info_t;

typedef struct _bcm_petra_trill_vpn_info_s {
    uint32     flags; 
    bcm_vlan_t high_vid;                /* Outer vlan */
    bcm_vlan_t low_vid;                 /* Inner vlan */
} _bcm_petra_trill_vpn_info_t;

int
_bcm_dpp_mc_to_trill_add(
    int	unit,
    bcm_multicast_t group, 
    bcm_gport_t port);

int
_bcm_dpp_mc_to_trill_remove(
    int unit,
    bcm_multicast_t group, 
    bcm_trill_name_t nick_name);

int
_bcm_dpp_dt_nickname_trill_get(
    int unit,
    bcm_gport_t trill_port_id, 
    int         *dt_nickname);

int
_bcm_dpp_mc_to_trill_get(
    int	unit,
    bcm_multicast_t group, 
    bcm_gport_t     *port);

int
_bcm_dpp_trill_port_add(
    int	unit,
    bcm_trill_port_t *trill_port,
    int              is_replace);

int
_bcm_dpp_trill_port_delete(
    int	unit,
    bcm_gport_t trill_port_id,
    int reset_local_info);

int
_bcm_petra_trill_port_fec_id_get (
    int unit, 
    bcm_gport_t trill_port_id, 
    int *fec_id);

int
_bcm_petra_add_to_trill_port_list(
     int unit,
     bcm_gport_t gport);

int     _bcm_petra_trill_get_trill_out_ac(int unit, SOC_PPC_AC_ID *out_ac);
int     _bcm_petra_trill_set_trill_out_ac(int unit, SOC_PPC_AC_ID out_ac);


int     _bcm_dpp_trill_port_set_local_info(int unit, bcm_trill_name_t port_name, uint32 is_skip_ethernet);
int     _bcm_dpp_trill_port_reset_local_info(int unit, bcm_trill_name_t port_name);
int     _bcm_dpp_trill_port_is_local(int unit, bcm_trill_name_t port_name, uint32 *is_local);

int     _bcm_dpp_trill_mc_route_get(int unit, bcm_trill_name_t root_name, _bcm_petra_trill_mc_route_info_t **route_list, int *cnt);

int      bcm_petra_trill_init_data(int unit);
int      bcm_petra_trill_deinit_data(int unit);
int     _bcm_dpp_trill_alloc_port_id(int unit, int *id);


int _ppd_trill_multicast_route_remove(
        int unit, 
        uint32              flags,
        bcm_trill_name_t    root_name, 
        bcm_vlan_t          vlan, 
        bcm_vlan_t          inner_vlan, 
        bcm_multicast_t     group,
        bcm_trill_name_t    src_name);

int _ppd_trill_multicast_route_add(
        int unit, 
        uint32              flags,
        bcm_trill_name_t    root_name, 
        bcm_vlan_t          vlan, 
        bcm_vlan_t          inner_vlan, 
        bcm_multicast_t     group,
        bcm_trill_name_t    src_name, 
        bcm_gport_t         src_port);

int _ppd_trill_multicast_route_get(
        int unit, 
        uint32              flags,
        bcm_trill_name_t    root_name, 
        bcm_vlan_t          vlan, 
        bcm_vlan_t          inner_vlan,     
        bcm_trill_name_t    src_name, 
        bcm_gport_t         src_port,
        bcm_multicast_t     *group,
        uint8               *found);

/*
 * Function:
 *       _bcm_dpp_trill_gport_to_fwd_decision
 * Description:
 *         convert trill gport to forwarding decision (destination + editing information)
 * Parameters:
 *  unit -           [IN] DPP device unit number (driver internal).
 *  gport -          [IN] trill port
 *  mc_id    -       [IN] multicast id
 *  fwd_decsion -    [OUT] PPD forwarding decision
 * Returns:
 *       BCM_E_XXX
 */
int 
_bcm_dpp_trill_gport_to_fwd_decision(int unit, bcm_gport_t gport, bcm_multicast_t mc_id, SOC_PPC_FRWRD_DECISION_INFO  *fwd_decsion);

#ifdef BCM_ARAD_SUPPORT 
/* Function:
 *      _bcm_dpp_trill_config_vpn
 * Purpose:
 *      Set the vpn configuration.
 *     
 * Parameters:
 *      unit  - (IN)  Device Number
 *      vsi   - (IN)  VSI id
 *      port  - (IN)  port id
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_dpp_trill_config_vpn(
        int unit, 
        bcm_vlan_t vsi, 
        bcm_gport_t port);
int _bcm_dpp_trill_unconfig_vpn(
        int unit, 
        bcm_vlan_t vsi, 
        bcm_gport_t port);

/*
 * Function:
 *      _bcm_dpp_trill_config_vpn_access
 * Purpose:
 *      Set the vpn information - Access side.
 *      
 *      
 * Parameters:
 *      unit  - (IN)  Device Number
 *      vsi   - (IN)  VSI id
 *      port  - (IN)  port id
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_dpp_trill_config_vpn_access(
        int unit, 
        bcm_vlan_t vsi, 
        bcm_gport_t port);
int _bcm_dpp_trill_delete_vpn_access(
        int unit, 
        bcm_vlan_t vsi, 
        bcm_gport_t port);

/*
 * Function:
 *      _bcm_dpp_trill_config_vpn_network
 * Purpose:
 *      Set the vpn information - Network side.
 *      
 *      
 * Parameters:
 *      unit  - (IN)  Device Number
 *      vsi   - (IN)  VSI id
 *      port  - (IN)  port id
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_dpp_trill_config_vpn_network(
        int unit, 
        bcm_vlan_t vsi, 
        bcm_gport_t port);
int _bcm_dpp_trill_delete_vpn_network(
        int unit, 
        bcm_vlan_t vsi, 
        bcm_gport_t port);
/*
 * Function:
 *      _bcm_dpp_trill_vlan_editing_create
 * Purpose:
 *      map VSI to outer-vid and inner-vid
 * Parameters:
 *      unit  - (IN)  Device Number
 *      vsi   - (IN)  VSI id
 *      port  - (IN)  port id
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_dpp_trill_vlan_editing_create(int unit, bcm_vlan_t vsi, bcm_gport_t port);
int _bcm_dpp_trill_vlan_editing_delete(int unit, bcm_vlan_t vsi, bcm_gport_t port);

int _bcm_dpp_trill_get_vsi_info(
        int unit, 
        bcm_vlan_t vsi, 
        _bcm_petra_trill_vpn_info_t *vsi_info);


int
_bcm_dpp_trill_sw_db_hash_mc_trill_find(int unit, sw_state_htb_key_t key, sw_state_htb_data_t data, int remove);

int 
_bcm_dpp_trill_sw_db_hash_mc_trill_insert(int unit, sw_state_htb_key_t key, sw_state_htb_data_t data);
int 
_bcm_dpp_trill_sw_db_hash_trill_route_info_destroy(int unit);

int 
_bcm_dpp_trill_sw_db_hash_mc_trill_destroy(int unit);

int
_bcm_dpp_trill_sw_db_hash_trill_info_find(int unit, sw_state_htb_key_t key, sw_state_htb_data_t data, int remove);

int 
_bcm_dpp_trill_sw_db_hash_trill_info_insert(int unit, sw_state_htb_key_t key, sw_state_htb_data_t data);

int 
_bcm_dpp_trill_sw_db_hash_trill_vpn_insert(int unit, sw_state_htb_key_t key, sw_state_htb_data_t data);
int
_bcm_dpp_trill_sw_db_hash_trill_vpn_find(int unit, sw_state_htb_key_t key, sw_state_htb_data_t data, int remove);

int _bcm_dpp_trill_sw_db_hash_trill_info_iterate(int unit, sw_state_htb_cb_t restore_cb);
int _bcm_dpp_trill_sw_db_hash_mc_trill_iterate(int unit, sw_state_htb_cb_t restore_cb);
int _bcm_dpp_trill_sw_db_hash_trill_route_info_iterate(int unit, sw_state_htb_cb_t restore_cb);
int _bcm_dpp_trill_sw_db_hash_trill_src_iterate(int unit, sw_state_htb_cb_t restore_cb);
int _bcm_dpp_trill_sw_db_hash_trill_vpn_iterate(int unit, sw_state_htb_cb_t restore_cb);


/* Alocate a new trill port id*/
#endif  /*BCM_ARAD_SUPPORT*/
#endif /* _BCM_INT_DPP_TRILL_H */

