/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        gport_mgmt.h
 * Purpose:     GPORT Management internal definitions to the BCM library.
 */

#ifndef  INCLUDE_GPORT_MGMT_H
#define  INCLUDE_GPORT_MGMT_H

#include <bcm/types.h>
/*#include <bcm/vlan.h>*/
#include <bcm/mpls.h>
#include <bcm/mim.h>
#include <bcm/trill.h>
#include <bcm/tunnel.h>
#include <bcm/extender.h>

#include <bcm_int/dpp/port.h>
#include <bcm_int/dpp/trill.h>
#include <bcm_int/dpp/alloc_mngr_local_lif.h>

#include <soc/dpp/PPD/ppd_api_general.h>
#include <soc/dpp/PPD/ppd_api_lif.h>
#include <soc/dpp/PPD/ppd_api_frwrd_fec.h>
#include <soc/dpp/TMC/tmc_api_general.h>
#include <soc/dpp/PPD/ppd_api_mymac.h>
#include <soc/dpp/PPD/ppd_api_eg_ac.h>

#include <shared/hash_tbl.h>
#include <shared/swstate/sw_state.h>
#include <shared/swstate/sw_state_hash_tbl.h>


/* Number of LIFs as gports */
#define _BCM_DPP_NOF_GPORTS(unit) ((SOC_DPP_CONFIG(unit))->l2.nof_lifs)

/* Following enum presents LIF type (per application/feature) */
typedef enum _bcm_lif_type_s {
    _bcmDppLifTypeAny=0,       /* Any LIF, usually mean not exist LIF */
    _bcmDppLifTypeMplsPort,    /* MPLS LIF (PWE) */
    _bcmDppLifTypeMim,       /* MIM LIF */
    _bcmDppLifTypeTrill,     /* TRILL LIF */
    _bcmDppLifTypeVlan,      /* VLAN LIF (AC) */
    _bcmDppLifTypeMplsTunnel, /* MPLS Tunnel LIF */
    _bcmDppLifTypeIpTunnel, /* IP Tunnel LIF */
    _bcmDppLifTypeL2Gre, /* L2Gre LIF */
    _bcmDppLifTypeVxlan, /* VxLan LIF */
    _bcmDppLifTypeLinkLayer, /* Link layer LIF */
    _bcmDppLifTypeOverlayLinkLayer, /* Link layer for Overlay encapsulation, used only in ROO applications */
    _bcmDppLifTypeCpuL2Encap, /* Encapsulated Link layer for PUNT applications */
    _bcmDppLifTypeExtender,  /* Port Extender layer (802.1BR) */
    _bcmDppLifTypeNativeVlan, /* Native vlan lif */
    _bcmDppLifTypeLawfulInterception, /* Lawful Interception */
    _bcmDppLifTypeNopMplsTunnel,  /*NOP Mpls tunnel LIF*/
    _bcmDppLifTypeDataMplsTunnel,  /*Mpls tunnel LIF with Data entry, Push/Swap operation*/
    _bcmDppLifTypeL2SrcEncap,      /* L2 Source Mac Encap */
	_bcmDppLifTypeL2tpTunnelDataEntry,	/* L2TP egress data entry */
	_bcmDppLifTypeL2tpTunnel, 			/* L2TP tunnel terminator, ingress-only */
	_bcmDppLifTypePppoeTunnel			/* PPPoE tunnel terminator, ingress-only */
} _bcm_lif_type_e;

/* 
 * Gport resolution type information,
 * Used to retreive Forwarding Information of a given GPORT
 * Note: Not all logical gports have forwarding information 
 * (for example when GPORT is not expected to be located in Forwarding databases) 
 */ 

#define _BCM_DPP_GPORT_RESOLVE_TYPE_IS_FEC(resolve_type) ((resolve_type==_bcmDppGportResolveTypeProtectedFec) || (resolve_type==_bcmDppGportResolveTypeForwardFec))

typedef enum _bcm_dpp_gport_resolve_type_s {
    _bcmDppGportResolveTypeInvalid=0, /* invalid gport type or not initialized */
    _bcmDppGportResolveTypeProtectedFec=1, /* gport-id includes FEC only, FEC usually includes: port + EEP */
    _bcmDppGportResolveTypeAC=2, /* gport is AC-OutLIF: phy = physical port + encap-id = AC */
    _bcmDppGportResolveTypeMC=3, /* gport is MC, usually for 1+1 protection */
    _bcmDppGportResolveTypePhy=4, /* gport is only physical port, encap_id is not relevant */
    _bcmDppGportResolveTypeFecVC=5, /* gport is FEC+VC (For PWE application): phy = FEC + encap-id = VC */
    _bcmDppGportResolveTypePhyEEP=6, /* gport is Port+OutLIF (EEP) for PWE: phy = physical port + encap-id = EEP */
    _bcmDppGportResolveTypeFecEEP=7, /* gport is FEC+OutLIF (EEP) for PWE: phy = FEC + encap-id = EEP */
    _bcmDppGportResolveTypeMimMP=8, /* gport is FEC for MIM MP: phy = B-SA LSB + encap-id = {B-SA MSB,B-VID} */
    _bcmDppGportResolveTypeTrillMC=9, /* gport is MC for trill Multicast */
    _bcmDppGportResolveTypeMimP2P=10, /* gport is FEC for MIM P2P: phy = LIF + encap-id = B-VID */
    _bcmDppGportResolveTypeIPTunnel=11, /* gport is out-lif/EEP for ip-tunnel: phy = physical port + encap-id = EEP */
    _bcmDppGportResolveTypeL2Gre=12, /* l2gre gport: gport is in-lif for ip-tunnel: phy = physical port + encap-id = out-LIF */
    _bcmDppGportResolveTypeL2GreEgFec=13, /* l2gre gport: gport is in-lif for ip-tunnel: phy = FEC + encap-id = out-LIF */
    _bcmDppGportResolveTypeVxlan=14, /* vxlan gport: gport is in-lif for ip-tunnel: phy = physical port + encap-id = out-LIF */
    _bcmDppGportResolveTypeVxlanEgFec=15, /* vxlan gport: gport is in-lif for ip-tunnel: phy = FEC + encap-id = out-LIF */
    _bcmDppGportResolveTypeRing=16, /* ring gport for AC: phy = FEC + encap-id */
    _bcmDppGportResolveTypeForwardFec=17, /* gport-id includes FEC only, not used for protection */
    _bcmDppGportResolveTypeFecEEI=18, /* gport is FEC+EEI.outLIF for PWE: phy = FEC + encap-id */
    _bcmDppGportResolveTypeMimMPNoLearn=19 /* gport is FEC for MIM MP with NO LEARN entry for the mim_port: phy = B-SA LSB + encap-id = {B-SA MSB,B-VID} */
} _bcm_dpp_gport_resolve_type_e;

/* 
 * Struct present the forwarding information of a given GPORT 
 * Including: 
 * - _bcm_dpp_gport_resolve_type_e - The forwarding information type 
 * - phy_gport - The physical information of a gport. May be for example destination-port,trunk or FEC. 
 *    According to _bcm_dpp_gport_resolve_type_e.
 * - encap_id - The encapsulation information of a gport. May be for example OutLIF, EEI or additional information required by a specific application. 
 *    According to _bcm_dpp_gport_resolve_type_e.
 */
typedef struct {
    _bcm_dpp_gport_resolve_type_e type;
    int phy_gport;/* physical information, according to resolve type */
    int encap_id;/* encapsulation information or additional info , according to resolve type */
    int fec_id;  /* fec id which is allocated by bcm_l3_egress_create() API, only used for vlan ingress 1+1 protection learning*/
} _BCM_GPORT_PHY_PORT_INFO;

/* per LIF what is match key */

/* Flags for _bcm_dpp_sw_resources_inlif_t */
#define _BCM_DPP_INFLIF_MATCH_INFO_MPLS_PORT_NO_OUTLIF      (0x1)
#define _BCM_DPP_INFLIF_MATCH_INFO_P2P                      (0x2)

/* Flags for _bcm_dpp_sw_resources_outlif_t */
#define _BCM_DPP_OUTLIF_MATCH_INFO_MPLS_EXTENDED_LABEL     (0x1)
#define _BCM_DPP_OUTLIF_IP_TUNNEL_MATCH_INFO_GRE_WITH_LB   (0x2)
#define _BCM_DPP_OUTLIF_MATCH_INFO_MPLS_EXTENDED_LABEL_TAGGED     (0x4)

#define GPORT_HASH_VLAN_NOT_FOUND(phy_port,rv)                    BCM_FAILURE(rv)

#define _BCM_DPP_LIF_TO_GPORT_INGRESS       0x02
#define _BCM_DPP_LIF_TO_GPORT_EGRESS        0x04
#define _BCM_DPP_LIF_TO_GPORT_GLOBAL_LIF    0x08
#define _BCM_DPP_LIF_TO_GPORT_LOCAL_LIF     0x10

/* for LIF-index, what is the match creteria */
/* Note: some attributes can be reused, for example vsi is for mp only and gport_id for p2p only */
typedef struct _bcm_dpp_sw_resources_inlif_s {
    _bcm_lif_type_e lif_type;
    uint32          flags;
    int             criteria;
    bcm_port_t      port;          /* Port */
    int             match1;
    int             match2;
    int             match_tunnel; /* For VLAN-Port used for PON application, for MPLS-Port used for label2 applications */
    int             match_ethertype;
                                   /* for MP lif --> gport, can be resoted from HW, from learning info*/
    bcm_gport_t     gport_id;      /* relevant only for p2p, given lif return gport*/
    int             key1;          /* only for p2p, part of the lif entry key */
    bcm_gport_t     peer_gport;    /* only for p2p, gport_id of the peer gport  */

    bcm_gport_t     learn_gport_id; /*  only for MP, the learning gport, needed when configure LIF and the learning info is FEC
                                    (maybe shared with gport_id) */ 
    _bcm_petra_tpid_profile_t tpid_profile_type;
                                   /* tpid profile is used for the case of PWE/I-SID. 
                                      Tpid profile state let us know the status of tpid profile assignment.
                                      By default tpid profile type should be none as PWE/I-SID doesn't use any assigned tpid profile.
                                      In order to change tpid profile type use APIs bcm_port_tpid_* */
    int             vsi;           /* only for MP service tell what the VSI LIF is attached to,
                                      can be restored/read from HW, set in SW to improve performance */
    uint32          additional_flags;/* Additional flags to represent more inlif options. */

} _bcm_dpp_sw_resources_inlif_t;

/* OutLIF match information. Managed by WB SW DB */
typedef struct _bcm_dpp_sw_resources_outlif_s {
    _bcm_lif_type_e lif_type;
    uint32          flags;
} _bcm_dpp_sw_resources_outlif_t;

typedef struct {
    _bcm_dpp_sw_resources_inlif_t  in_lif_sw_resources; /* Global-LIF, Invalid when not exist */
    _bcm_dpp_sw_resources_outlif_t out_lif_sw_resources; /* Global-LIF, Invalid when not exist */
} _bcm_dpp_gport_sw_resources;

typedef struct {
    PARSER_HINT_ARR uint32 *fec_remote_lif;
    PARSER_HINT_ARR bcm_gport_t *bcm_dpp_sw_resources_fec_to_gport;	
    PARSER_HINT_ARR _bcm_dpp_sw_resources_outlif_t *outlif_info;
    PARSER_HINT_ARR _bcm_dpp_sw_resources_inlif_t *inlif_info;
    int             vlan_port_db_htb_handle;
    /* Trill port info : map trill to mc_id or fec*/
    int             trill_info_db_htb_handle;
    /* map MC to trill*/
    int             mc_trill_db_htb_handle;
    int             trill_vpn_db_htb_handle;
}bcm_dpp_gport_mgmt_info_t;


/* 
 * Defines
 */

#define _BCM_GPORT_NOF_LIFS         DPP_WB_ENGINE_NOF_LIFS
#define _BCM_GPORT_NOF_VSIS         ((SOC_DPP_CONFIG(unit))->l2.nof_vsis)
#define _BCM_PPD_VSI_P2P_SERVICE    ((SOC_DPP_CONFIG(unit))->l2.nof_vsis - 2)

#define _BCM_GPORT_ENCAP_ID_LIF_INVALID  (-1)

/* In-LIF Protection defines */
/* '-2' value is reserved for Invalid. In Coupled mode, there's one bit less for the Protection-Pointer */
#define _BCM_INGRESS_PROTECTION_POINTER_INVALID ((SOC_DPP_DEFS_GET(unit, failover_ingress_last_hw_id) >>                         \
                                                 (SOC_IS_JERICHO(unit) ? (SOC_DPP_IS_PROTECTION_INGRESS_COUPLED(unit)) : 0)) - 1)
#define _BCM_INGRESS_PROTECTION_POINTER_DEFAULT (0)

/* 
 * MACROS
 */


#define _BCM_PPD_IS_VALID_FAILOVER_ID(failover_id)  ((failover_id)>0)

/*  set failover id in case of encoding*/
#define _BCM_PPD_FAILOVER_ID_PLUS_1_TO_ID(_failover_id, _ingress_failover_id)  (DPP_FAILOVER_ID_GET((_failover_id), (_ingress_failover_id)))

/* is the given gport working gport in protection, assuming gport has protection*/
#define _BCM_PPD_GPORT_IS_WORKING_PORT(gport) ((gport)->failover_port_id != BCM_GPORT_TYPE_NONE)

/* is the given gport working gport in ingress protection, assuming gport has protection*/
#define _BCM_PPD_GPORT_IS_INGRESS_WORKING_PORT(gport) ((gport)->ingress_failover_port_id != BCM_GPORT_TYPE_NONE)

/* is the given gport working gport in egress protection, assuming gport has egress protection */
#define _BCM_PPD_GPORT_IS_EGRESS_WORKING_PORT(gport)    ((gport)->egress_failover_port_id != 0)

/* is the given gport of type mpls tunnel working gport in ingress protection, assuming gport has ingress protection */
#define _BCM_PPD_GPORT_IS_INGRESS_WORKING_TUNNEL(tunnel)    ((tunnel)->failover_tunnel_id != 0)

/* given FEC for protecting gport returns FEC for working gport*/
#define _BCM_PPD_GPORT_PROTECT_TO_WORK_FEC(protec_fec) (protec_fec-1)

/* given FEC for working gport returns FEC for protection gport*/
#define _BCM_PPD_GPORT_WORK_TO_PROTECT_FEC(work_fec) (work_fec+1)

/* given FEC returns working FEC */
#define _BCM_PPD_GPORT_FEC_TO_WORK_FEC(fec) ((((fec)%2) == 1)?(fec-1):(fec))

/* indicate if the gport is an overlay gport: vxlan, l2gre, trill, vpls (identify by mpls gport) */
#define _BCM_GPORT_IS_OVERLAY(_gport) \
        _SHR_GPORT_IS_VXLAN_PORT(_gport) \
        || _SHR_GPORT_IS_L2GRE_PORT(_gport) \
        || _SHR_GPORT_IS_TRILL_PORT(_gport) \
        || _SHR_GPORT_IS_MPLS_PORT(_gport) \
        || _SHR_GPORT_IS_FORWARD_PORT(_gport)


/* 
 * Functions 
 */

/*
 * GENERAL GPORT MGMT FUNCTIONS
 */


/*
 * Function:
 *      _bcm_dpp_gport_mgmt_init
 * Purpose:
 *      init Gport module
 * Parameters:
 * Note:
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_dpp_gport_mgmt_init(int                     unit);

extern int
_bcm_dpp_gport_mgmt_sw_state_cleanup(int                     unit);


/* 
 * Gport compare type information,
 * Used to specify the criteria for gport compare
 */ 
typedef enum _bcm_dpp_gport_compare_type_s {
    _bcmDppGportCompareTypeSystemPort=0, /* compare system port */
    _bcmDppGportCompareTypeInLif=1, /* compare Global In Lif */
    _bcmDppGportCompareTypeOutLif=2 /* compare Global Out Lif */
} _bcm_dpp_gport_compare_type_e;

/*
 * Function:
 *    _bcm_dpp_gport_compare
 * Description:
 *    compare two gports
 * Parameters:
 *    unit -          [IN] DPP device unit number (driver internal).
 *  gport1 -          [IN] general port 1
 *  gport2 -          [IN] general port 2 
 *  is_equal -         [OUT] result of comparison
 * Returns:
 *    BCM_E_XXX
 */
int 
_bcm_dpp_gport_compare(int unit, bcm_gport_t gport1, bcm_gport_t gport2, _bcm_dpp_gport_compare_type_e type, uint8 * is_equal);

/*
 * given vpn and iter return next gport-value in vpn 
 * caller still need to call  
 */
int _bcm_dpp_vpn_get_next_gport(
    int                  unit,
    bcm_vpn_t            vpn, /* if -1 then get next gport belongs to any vsi */
    int                  *iter,
    bcm_gport_t          *port_val,
    int                  *cur_vsi /* the vsi of the return LIF*/
 );

/* Gport parse type */
typedef enum _bcm_dpp_gport_parse_type_s {
    _bcmDppGportParseTypeProtectedFec=0, /* Fec used for bundle protection */       
    _bcmDppGportParseTypeEncap, /* LIF ID */    
    _bcmDppGportParseTypeMC, /* Multicast-ID */      
    _bcmDppGportParseTypeSimple, /* Simple , physical */
    _bcmDppGportParseTypeForwardFec, /* Fec used for forwarding and not protected */
    _bcmDppGportParseTypePushProfile /* Push-profile ID, for PWE application*/
} _bcm_dpp_gport_parse_type_e;

/* Additional information on parsing */
#define _BCM_DPP_GPORT_PARSE_PRIMARY      (1 << 0) /* May be valid only when type is MC */
#define _BCM_DPP_GPORT_PARSE_SECONDARY    (1 << 1) /* May be valid only when type is MC */
#define _BCM_DPP_GPORT_PARSE_INGRESS_ONLY (1 << 2) /* May be valid only when type is ENCAP */
#define _BCM_DPP_GPORT_PARSE_EGRESS_ONLY  (1 << 3) /* May be valid only when type is ENCAP */

typedef struct _bcm_dpp_gport_parse_info_s {
    _bcm_dpp_gport_parse_type_e type; /* type of the object */
    int val; /* Object ID */
    uint32     flags; /* Additional information on object: _BCM_DPP_GPORT_PARSE_XXX */    
} _bcm_dpp_gport_parse_info_t;
/* 
 *  Function:
 *    _bcm_dpp_gport_parse
 * Description:
 *  Parse gport-id encoding to indicate the type of the object and its value.
 *  Done only according to gport-id encoding (without any SW DB).
 *  Input: gport 
 *  Output: Structure gport_parse includes:
 *  Type: _bcmDppGportParseTypeProtectedFec, Val: FEC-ID, Flags: None
 *  Type: _bcmDppGportParseTypeForwardFec,   Val: FEC-ID, Flags: None
 *  Type: _bcmDppGportParseTypeMC,           Val: MC-ID,  Flags: Primary/Secondary
 *  Type: _bcmDppGportParseTypeEncap,        Val: LIF-ID, Flags: Ingress/Egress Only
 *  Type: _bcmDppGportParseTypeSimple        Val: physical-port
 *  Type: _bcmDppGportParseTypePushProfile   Val: Push-profile ID
 *  Note:
 *    For more encoding information of gport see drv.h
 */
int _bcm_dpp_gport_parse(int unit, bcm_gport_t port, _bcm_dpp_gport_parse_info_t *gport_parse_info);

int _bcm_dpp_gport_is_protected(int unit, bcm_gport_t gport, int  *is_protected);

/* if set then don't check if gport already exist, should be in construction */
#define _BCM_DPP_GPORT_RESOLVE_FLAG_NO_CHECK (0x1)

/* 
 * Should be internal function. 
 * Note: Structure should be used only when gport is supposed to be located in forwarding databases. 
 *       For example GPORT of symetric AC (VLAN-Port) may reside in MACT, MC-DB , etc.
 *       But when GPORT of AC is assymetric (INGRESS / EGRESS ONLY) and used only for VLAN translation then no forwarding information is needed
 */
int 
_bcm_dpp_gport_resolve(int unit, bcm_gport_t gport, int flags, SOC_SAND_PP_DESTINATION_ID *dest_id, int *encap_id,_bcm_dpp_gport_resolve_type_e *gport_type);


typedef struct _bcm_dpp_gport_info_s {
    bcm_pbmp_t pbmp_local_ports;
    bcm_port_t local_port; /* one of the ports in pbmp_local_ports*/
    uint32     sys_port;
    uint32     flags;
    int        lane;
    int        phyn;
} _bcm_dpp_gport_info_t;

#define _BCM_DPP_GPORT_INFO_F_IS_LOCAL_PORT 0x01
#define _BCM_DPP_GPORT_INFO_F_IS_LAG        0x02
#define _BCM_DPP_GPORT_INFO_F_IS_SYS_SIDE   0x08
#define _BCM_DPP_GPORT_INFO_F_IS_BLACK_HOLE 0x10

#define _BCM_DPP_GPORT_INFO_IS_LOCAL_PORT(gport_info) (gport_info.flags & _BCM_DPP_GPORT_INFO_F_IS_LOCAL_PORT ? 1: 0)
#define _BCM_DPP_GPORT_INFO_IS_LAG(gport_info) (gport_info.flags & _BCM_DPP_GPORT_INFO_F_IS_LAG ? 1: 0)
#define _BCM_DPP_GPORT_INFO_IS_SYS_SIDE(gport_info) (gport_info.flags & _BCM_DPP_GPORT_INFO_F_IS_SYS_SIDE ? 1: 0)

/*
 * Function:
 *    _bcm_dpp_gport_to_phy_port
 * Description:
 *    map gport to PPD local and system port
 * Parameters:
 *    unit  -       [IN] DPP device unit number (driver internal).
 *    gport -       [IN] general port
 *    operations -  [IN] see _BCM_DPP_GPORT_TO_PHY_OP_...
 *    gport_info -  [OUT] Retrive information
 * Returns:
 *    BCM_E_XXX
 */

#define _BCM_DPP_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT    0x1
#define _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY  0x2
#define _BCM_DPP_GPORT_TO_PHY_OP_IGNORE_DISABLED_PORTS  0x4
int 
_bcm_dpp_gport_to_phy_port(int unit, bcm_gport_t gport, uint32 operations, _bcm_dpp_gport_info_t* gport_info);

/* given gport, return if it's local in this unit */
int 
_bcm_dpp_gport_is_local(int unit, bcm_gport_t port,  int *is_local);



/*
 * L2 INTERFACE FUNCTIONS
 */

/* Internal functions for l2 interface APIs (vlan_port_create and mpld_port_add) */
int _bcm_dpp_gport_l2_intf_determine_ingress_egress(bcm_gport_t gport_id, uint8 *is_ingress, uint8 *is_egress);

int _bcm_dpp_gport_l2_intf_calculate_act_fec(void *gport, int *act_fec, int gport_type);

int _bcm_dpp_gport_l2_intf_calculate_port_id(int unit, int act_fec, SOC_PPC_LIF_ID global_lif_id, void *gport, int gport_type);

int _bcm_dpp_gport_l2_intf_get_global_lif_from_port_id(void *gport, uint8 is_protected, uint8 is_mc_ingress, SOC_PPC_LIF_ID *global_lif_id, int gport_type);

int _bcm_dpp_gport_l2_intf_ingress_egress_only_validation(void *gport, uint8 validate, uint8 *is_ingress, uint8 *is_egress, int gport_type);

int _bcm_dpp_gport_l2_intf_protection_validation(int unit, void *gport,  int gport_type, uint8 validate, int *is_protected, uint8 *is_ingress_protected, 
                                           uint8 *is_mc_ingress, uint8 *is_1_plus_1, uint8 *is_egress_protected);

int _bcm_dpp_gport_l2_intf_gport_has_remote_destination(int unit, bcm_gport_t gport_id, uint8 *remote_destination);


int _bcm_dpp_lif_to_gport(
    int                 unit,
    int                 lif_id, /* InLIF ID */
    uint32              flags, 
    bcm_gport_t         *gport_id
 );

/*
 * FORWARDING INFORMATION FUNCTIONS
 */

/*
 * Function:
 *       _bcm_dpp_gport_to_port_encap
 * Description:
 *       map gport to forwarding info <phy-port (can be FEC) and encap that store LIF or EEI)
 * Parameters:
 *       unit -           [IN] DPP device unit number (driver internal).
 *  gport -          [IN] general port
 *  dest_port -      [OUT] dest_port is physical TM port or FEC. i.e. pp-destination
 *  encap_id -       [OUT] encap-info includes eei or outlif
 * Returns:
 *       BCM_E_XXX
 */int 
_bcm_dpp_gport_to_port_encap(int unit, bcm_gport_t gport, int *dest_port, int *encap_id);
/*
 * Function:
 *       _bcm_dpp_gport_encap_to_fwd_decision
 * Description:
 *       Parse gport+encap to soc-ppd-forwarding decision
 * Parameters:
 *       unit -           [IN] DPP device unit number (driver internal).
 *  gport -          [IN] general port
 *  encap_info -       [IN] encap_id EEI or outlif.
 *                          if invalid then 
 *  fwd_decsion -    [OUT] PPD forwarding decision
 * Returns:
 *       BCM_E_XXX
 */
int 
_bcm_dpp_port_encap_to_fwd_decision(int unit, bcm_gport_t gport, int encap_id, SOC_PPC_FRWRD_DECISION_INFO  *fwd_decsion);


/*
 * Function:
 *       _bcm_dpp_gport_and_encap_from_fwd_decision
 * Description:
 *       Parse soc-ppd-forwarding decision to gport+encap
 * Parameters:
 *       unit -           [IN] DPP device unit number (driver internal).
 *  gport -          [OUT] general port
 *  encap_id -       [OUT] encap_id EEI or outlif.
 *                          if invalid then 
 *  fwd_decsion -    [IN] PPD forwarding decision
 *  encap_usage -    [IN] AC,PWE,MIM, General, -1 in case encap is not required
 *  force_destination - [IN] Return: gport as destination port (not outlif) + encap_id
 * Returns:
 *       BCM_E_XXX
 */
int 
_bcm_dpp_gport_and_encap_from_fwd_decision(int unit, bcm_gport_t *gport, int *encap_id, SOC_PPC_FRWRD_DECISION_INFO  *fwd_decsion, uint32 encap_usage, uint8 force_destination);

/*
 * Function:
 *       _bcm_dpp_gport_to_fwd_decision
 * Description:
 *       Parse gport
 * Parameters:
 *       unit -           [IN] DPP device unit number (driver internal).
 *  gport -          [IN] general port
 *  encap_id -       [IN] encap_id considered only if gport is logical port
 *  fwd_decsion -    [OUT] PPD forwarding decision
 * Returns:
 *       BCM_E_XXX
 */
int 
_bcm_dpp_gport_to_fwd_decision(int unit, bcm_gport_t gport, SOC_PPC_FRWRD_DECISION_INFO  *fwd_decsion);

/*
 * Function:
 *    _bcm_dpp_gport_to_sand_pp_dest
 * Description:
 *    convert gport from soc_sand-pp-destination
 * Parameters:
 *    unit -           [IN] DPP device unit number (driver internal).
 *  gport -          [IN] general port
 *  dest_id -       [IN] SOC_SAND destination
 * Returns:
 *    BCM_E_XXX
 */
int 
_bcm_dpp_gport_to_sand_pp_dest(int unit, bcm_gport_t gport, SOC_SAND_PP_DESTINATION_ID  *dest_id);


/*
 * Function:
 *    _bcm_dpp_gport_to_fwd_decision
 * Description:
 *    convert gport to forwardubg decision (destination + editting information)
 * Parameters:
 *    unit -           [IN] DPP device unit number (driver internal).
 *  gport -          [IN] general port
 *  encap_id -       [IN] encap_id considered only if gport is logical port
 *  fwd_decsion -    [OUT] PPD forwarding decision
 * Returns:
 *    BCM_E_XXX
 */
int 
_bcm_dpp_gport_to_tm_dest_info(int unit, bcm_gport_t gport, SOC_TMC_DEST_INFO  *soc_petra_dest_info);

/*
 * Function:
 *    _bcm_dpp_gport_to_tm_dest_info
 * Description:
 *    convert gport from TM dest information
 * Parameters:
 *  unit -           [IN] DPP device unit number (driver internal).
 *  gport -          [OUT] general port
 *  soc_petra_dest_info - [OUT] Soc_petra destination info
 * Returns:
 *    BCM_E_XXX
 */
int 
_bcm_dpp_gport_from_tm_dest_info(int unit, bcm_gport_t *gport, SOC_TMC_DEST_INFO  *soc_petra_dest_info);

/*
 * Function:
 *    _bcm_dpp_gport_from_sand_pp_dest
 * Description:
 *    convert gport from soc_sand-pp-destination
 * Parameters:
 *    unit -           [IN] DPP device unit number (driver internal).
 *  gport -          [IN] general port
 *  dest_id -       [IN] SOC_SAND destination
 * Returns:
 *    BCM_E_XXX
 */
int 
_bcm_dpp_gport_from_sand_pp_dest(int unit, bcm_gport_t *gport, SOC_SAND_PP_DESTINATION_ID  *dest_id);



/*
 * SOFTWARE RESOURCES FUNCTIONS
 */

/* _BCM_DPP_GPORT_SW_RESOURCES_XXX information, represents which SW resource to retreive */
#define _BCM_DPP_GPORT_SW_RESOURCES_INGRESS (1 << 0) /* Retreive Ingress SW resources */
#define _BCM_DPP_GPORT_SW_RESOURCES_EGRESS (1 << 1) /* Retreive Egress SW resources */

/*  Use this macros when iterating over the egress sw resources. 
 *  See documenation of ARAD_PP_EG_ENCAP_EEDB_INDEX_TO_OUTLIF for details on this macro.
 */

#define _BCM_DPP_GPORT_LIF_SW_RESOURCES_EGRESS_ITER_START(_unit)    \
    (ARAD_PP_EG_ENCAP_EEDB_INDEX_TO_OUTLIF(_unit, 0))

#define _BCM_DPP_GPORT_LIF_SW_RESOURCES_EGRESS_ITER_END(_unit)  \
    (ARAD_PP_EG_ENCAP_EEDB_INDEX_TO_OUTLIF(_unit, SOC_DPP_DEFS_GET(unit, nof_out_lifs)))

/*
 * Function:
 *    _bcm_dpp_local_lif_to_sw_resources
 * Description:
 *    map local_lif to its SW resources
 * Parameters:
 *    unit -    [IN] DPP device unit number (driver internal).
 *    local_in_lif -  [IN] in lif
 *    local_out_lif -  [IN] out lif
 *    flags -   [IN] See .h for _BCM_DPP_GPORT_SW_RESOURCES_XXX information
 *    gport_sw_resources [OUT] GPORT SW resources. See .h for structure information
 * Notes: 
 */
int
_bcm_dpp_local_lif_to_sw_resources(int unit, int local_in_lif, int local_out_lif, uint32 flags, _bcm_dpp_gport_sw_resources *gport_sw_resources);

/*
 * Function:
 *    _bcm_dpp_local_lif_sw_resources_set
 * Description:
 *    fill set SW resources according to lif
 * Parameters:
 *    unit -    [IN] DPP device unit number (driver internal).
 *    local_in_lif -  [IN] in lif
 *    local_out_lif -  [IN] out lif
 *    flags -   [IN] See .h for _BCM_DPP_GPORT_SW_RESOURCES_XXX information
 *    gport_sw_resources [OUT] GPORT SW resources. See .h for structure information
 * Notes: 
 */
int
_bcm_dpp_local_lif_sw_resources_set(int unit, int in_local_lif, int local_out_lif, uint32 flags, _bcm_dpp_gport_sw_resources *gport_sw_resources);

/*
 * Function:
 *    _bcm_dpp_lif_sw_resources_delete
 * Description:
 *    delete SW resources according to lif
 * Parameters:
 *    unit -    [IN] DPP device unit number (driver internal).
 *    local_in_lif -  [IN] in lif
 *    local_out_lif -  [IN] out lif
 *    flags -   [IN] See .h for _BCM_DPP_GPORT_SW_RESOURCES_XXX information
 * Notes: 
 */
int
_bcm_dpp_local_lif_sw_resources_delete(int unit, int local_in_lif, int local_out_lif, uint32 flags);

/* LIF Usage */
int _bcm_dpp_local_lif_sw_resources_lif_usage_get(int unit, int local_in_lif, int local_out_lif, _bcm_lif_type_e *in_lif_usage, _bcm_lif_type_e *out_lif_usage);
int _bcm_dpp_local_lif_sw_resources_lif_usage_set(int unit, int in_lif, int out_lif, _bcm_lif_type_e in_lif_usage, _bcm_lif_type_e out_lif_usage);
int _bcm_dpp_lif_sw_resources_gport_usage_get(int unit, bcm_gport_t gport, _bcm_lif_type_e *used_type);

/* Learn (InLIF only) */
int _bcm_dpp_local_lif_sw_resources_learn_gport_set(int unit, int local_in_lif, int learn_gport);
int _bcm_dpp_local_lif_sw_resources_learn_gport_get(int unit, int local_in_lif, int * learn_gport);

/* store and get vsi */
int _bcm_dpp_local_lif_sw_resources_vsi_set(int unit, int local_in_lif, int vsi);
int _bcm_dpp_local_lif_sw_resources_vsi_get(int unit, int local_in_lif, int *vsi);

/* get flags */
int _bcm_dpp_local_outlif_sw_resources_flags_get(int unit, int local_out_lif, uint32 *flags);

/* *** FEC SW DB ***/

/* Mark the FEC as remote */
int
bcm_dpp_gport_mgmt_fec_remote_destination_set(
   int unit,
   uint32 fec_id,
   uint8 is_remote);

/* Add FEC to GPORT mapping */
int
bcm_dpp_gport_mgmt_sw_resources_fec_to_gport_set(
   int unit,
   uint32 fec_id,
   uint32 gport);

/* Get FEC to GPORT mapping */
int
bcm_dpp_gport_mgmt_sw_resources_fec_to_gport_get(
   int unit,
   uint32 fec_id,
   bcm_gport_t *gport);

/*
 * HARDWARE RESOURCES FUNCTIONS
 */

typedef struct {
    int global_in_lif; /* Global-LIF, Invalid when not exist */
    int global_out_lif; /* Global-LIF, Invalid when not exist */
    int local_in_lif; /* Local-LIF, Invalid when not exist */
    int local_out_lif; /* Local-LIF, Invaild when not exist */
    int fec_id; /* FEC, invalid when not exist */    
} _bcm_dpp_gport_hw_resources;

/* _BCM_DPP_GPORT_HW_RESOURCES_XXX information, represents which HW resource to retreive */
#define _BCM_DPP_GPORT_HW_RESOURCES_FEC (1 << 0) /* Retreive FEC resource */
#define _BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS (1 << 1)  /* Retreive Ingress Global LIF */
#define _BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS (1 << 2) /* Retreive Egress Global LIF */
#define _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS (1 << 3)  /* Retreive Ingress Local LIF */
#define _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS (1 << 4) /* Retreive Egress Local LIF */
#define _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK (1 << 5) /* Return error in case required and invalid resource */
/* Retreive Local LIF resources */
#define _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF (_BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS | _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS)
/* Retreive Global LIF resources */
#define _BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF (_BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS | _BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS)
/* Retreive Local and Global LIF resources */
#define _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_AND_GLOBAL_LIF (_BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF | _BCM_DPP_GPORT_HW_RESOURCES_LOCAL_LIF)

/*
 * Function:
 *    _bcm_dpp_gport_to_hw_resources
 * Description:
 *    map gport to its HW resources (LIF, FEC)
 * Parameters:
 *    unit -    [IN] DPP device unit number (driver internal).
 *    gport -   [IN] general port
 *    flags -   [IN] See .h for _BCM_DPP_GPORT_HW_RESOURCES_XXX information
 *    gport_hw_resources [OUT] GPORT HW resources. See .h for structure information
 * Notes: 
 */
int
_bcm_dpp_gport_to_hw_resources(int unit, bcm_gport_t gport, uint32 flags, _bcm_dpp_gport_hw_resources *gport_hw_resources);

/*
 * Function:
 *    _bcm_dpp_gport_from_hw_resources
 * Description:
 *    get gport from its HW resources (LIF, FEC)
 * Parameters:
 *    unit -    [IN] DPP device unit number (driver internal).
 *    gport -   [OUT] general port
 *    flags -   [IN] See .h for _BCM_DPP_GPORT_HW_RESOURCES_XXX information
 *    gport_hw_resources [IN] GPORT HW resources. See .h for structure information
 * Notes: 
 */
int
_bcm_dpp_gport_from_hw_resources(int unit, bcm_gport_t *gport, uint32 flags, _bcm_dpp_gport_hw_resources *gport_hw_resources);

/*
 * Function:
 *      _bcm_dpp_gport_alloc_global_and_local_lif
 * Purpose:
 *       Allocates global lif and local lifs according to given arguements:
 *          If in_lif_data == NULL && out_lif_data == NULL returns error.
 *          If in_lif_data != NULL, allocates a local in lif and a global in lif.
 *          If out_lif_data != NULL, allocates a local out lif, and if lif is mapped allocates a global lif and writes to GLEM.
 *  
 * Parameters:
 *      unit                - (IN) Device Number
 *      flags               - (IN) Only BCM_DPP_AM_FLAG_ALLOC_WITH_ID is supported.
 *      global_lif_id       - (INOUT) Fill with the global lif id if WITH_ID, otherwise will be filled with the allocated globall if id.
 *      local_inlif_info    - (INOUT) Local inlif data, will be filled with the allocated local in lif id.
 *      local_outlif_info   - (INOUT) Local outlif data, will be filled with the allocated local out lif id.
 *  
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_dpp_gport_alloc_global_and_local_lif(int                               unit, 
                                          uint32                            flags,
                                          int                               *global_lif_id, 
                                          bcm_dpp_am_local_inlif_info_t     *local_in_lif_info, 
                                          bcm_dpp_am_local_out_lif_info_t   *local_out_lif_info);

/*
 * Function:
 *      _bcm_dpp_gport_delete_global_and_local_lif
 * Purpose:
 *       Given global and local lif, removes them. If remove_glem_entry is set, removes the glem entry according to global lif as well.
 * Parameters:
 *      unit                - (IN) Device Number
 *      global_lif          - (IN) Global lif to be deallocated.
 *      local_inlif_id      - (IN) Local inlif to be deallocated.
 *      local_outlif_id     - (IN) Local outlif to be deallocated.
 *  
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_dpp_gport_delete_global_and_local_lif(int unit, int global_lif, int local_inlif_id, int local_outlif_id);


/*
 * Function:
 *     _bcm_dpp_gport_to_global_and_local_lif
 * Purpose:
 *      Given a gport id, returns the global and local lifs for this gport, as well as fec and remote lif indication.
 *  
 * Parameters:
 *     unit                 - (IN) Device number
 *     gport_id             - (IN) Gport to be checked.
 *     global_lif           - (OUT) Global lif for this gport.
 *     local_in_lif         - (OUT) Local in lif for this gport.
 *     local_out_lif        - (OUT) LOCAL out lif for this gport. If port is a pon vlan port and soc is arad b1 and below,
 *                                                                  this would hold the out ac.
 *     fec_id               - (OUT) Fec for this gport.
 *     is_local             - (OUT) Local/remote lif indication.
 *  
 * Returns: 
 *     BCM_E_NONE       - If the value was retrieved successfully..
 *     BCM_E_*          - Otherwise.
 */
int _bcm_dpp_gport_to_global_and_local_lif(int      unit,
                                            bcm_gport_t          gport_id,
                                            int                  *global_lif,
                                            int                  *local_in_lif,
                                            int                  *local_out_lif,
                                            int                  *fec_id,
                                            int                  *is_local);

/*
 * Function:
 *      _bcm_dpp_local_lif_to_vsi
 * Purpose:
 *       Given lif return VSI this lif is assoicated to, return -1 if there is no VSI
 *  
 * Parameters:
 *      unit                - (IN) Device Number
 *      local_lif           - (IN) Lif to check.
 *      vsi                 - (OUT) Fill with the VSI the lif is assosiated with.
 *      sw_state            - (IN) If set retrieve from SW state (optimized call).
 *      gport               - (OUT) Return gport as well. Valid for mp and p2p, for p2p read from SW db.
 *  
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_dpp_local_lif_to_vsi(
    int          unit,
    int          local_lif,
    int          *vsi,
    bcm_gport_t  *gport
   );



/*
 * MISC functions 
 */

/* VLAN HASH */

int
_bcm_dpp_sw_db_hash_vlan_find(int unit, sw_state_htb_key_t key, sw_state_htb_data_t data, int remove);

int 
_bcm_dpp_sw_db_hash_vlan_insert(int unit, sw_state_htb_key_t key, sw_state_htb_data_t data);

int 
_bcm_dpp_sw_db_hash_vlan_iterate(int unit, sw_state_htb_cb_t restore_cb);


/* 1+1 protection*/
int 
_bcm_dpp_gport_plus_1_mc_to_id(int mc_id, int is_primary, int *gport_val);
int 
_bcm_dpp_gport_plus_1_id_to_mc(int gport_val, int *mc_id , int *is_primary);


/* FEC handling */
#define BCM_DPP_GPORT_TYPE_EGRESS_IF (0x50)

int 
_bcm_dpp_l2_gport_fill_fec(int unit, void *gport, int gport_type, SOC_PPC_FEC_ID fec_id, SOC_PPC_FRWRD_FEC_ENTRY_INFO *work_fec);

int 
_bcm_dpp_l2_fec_fill_gport(int unit, SOC_PPC_FEC_ID fec_id, void *gport, int gport_type);

#endif /* INCLUDE_GPORT_MGMT_H */

