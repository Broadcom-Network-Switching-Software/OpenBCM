/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * DNX FCOE implementation
 */
#include <soc/defs.h>
 
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_FCOE
#include <shared/bsl.h>
#include <bcm/stat.h>
#include <bcm/fcoe.h>
#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dpp/l3.h>
#include <bcm_int/dpp/error.h>
#include <soc/dpp/PPD/ppd_api_frwrd_fcf.h>
#include <soc/dpp/PPD/ppd_api_general.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm/rx.h>
#include <bcm_int/petra_dispatch.h>
#include <bcm_int/dpp/rx.h>
#include <bcm_int/dpp/field_int.h>


/* trap-code/ strength for zoning drop action */
#define _BCM_PETRA_FCOE_ZONE_DROP_STRENGTH  (4)
#define _BCM_PETRA_FCOE_ZONE_DROP_TRAP_CODE  (_BCM_PETRA_UD_DROP_TRAP)

/* trap-code/ strength for zoning allow action */
#define _BCM_PETRA_FCOE_ZONE_ALLOW_STRENGTH  (3)
#define _BCM_PETRA_FCOE_ZONE_ALLOW_TRAP_CODE  (_BCM_PETRA_UD_DFLT_TRAP)



/* prefix length of local domain */
#define _BCM_PETRA_FCOE_LOCAL_DOMAIN_PREFIX (8)
#define _BCM_PETRA_FCOE_LOCAL_HOST_PREFIX (24)


/* add local domain.
   prefix has to be 8 */
#define _BCM_PETRA_FOCE_ADD_LOCAL_DOMAIN              (BCM_FCOE_LOCAL_ADDRESS)
/* add local host: add local domain and local.
   prefix has to be 24 */
#define _BCM_PETRA_FOCE_ADD_LOCAL_HOST                (BCM_FCOE_LOCAL_ADDRESS|BCM_FCOE_HOST_ROUTE)
/* add remote domain: add to LPM
   prefix has to be 0 - 8*/
#define _BCM_PETRA_FOCE_ADD_REMOTE_DOMAIN_PREFIX      (BCM_FCOE_DOMAIN_ROUTE)

/* add local host: add local domain and local.
   prefix has to be 24 use N_Port DB */
#define _BCM_PETRA_FOCE_ADD_NPORT_HOST                (BCM_FCOE_SOURCE_ROUTE | BCM_FCOE_HOST_ROUTE)

/* add domain: add domain to LPM.
   prefix has to be 0 - 8 use N_Port DB */
#define _BCM_PETRA_FOCE_ADD_NPORT_DOMAIN              (BCM_FCOE_SOURCE_ROUTE | BCM_FCOE_DOMAIN_ROUTE)

/* add to LEM: local host or remote domain
   prefix has to be 8: for remote domain
   perfix has to be 24: for local host*/
#define _BCM_PETRA_FOCE_ADD_EXACT_KEY                 (BCM_FCOE_HOST_ROUTE)




/*
 * Function:
 *      _shr_ip_mask_length
 * Purpose:
 *      Return the mask length from IPv4 network address
 * Parameters:
 *      mask - The IPv4 mask as IP address
 * Returns:
 *      The IPv4 mask length
 */
STATIC int
bcm_petra_fcoe_mask_length(uint32 mask)
{
    int masklen, i;

    /* Convert netmask to number of bits */
    masklen = 0;

    for (i = 0; i < _BCM_PETRA_FCOE_LOCAL_HOST_PREFIX; i++) {
        if (mask & 0x800000) {
            masklen++;
            mask = mask << 1;
        } else {
            break;
        }
    }

    return (masklen);
}

/* Initialize FCOE module */
int bcm_petra_fcoe_init(
    int unit)
{

    SOC_PPC_FRWRD_FCF_GLBL_INFO fc_info;
    int rv = BCM_E_NONE;
    SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO trap_info;
    uint32 soc_sand_rv;
    int trap_id_sw;
    int trap_id_converted;

    BCMDNX_INIT_FUNC_DEFS;

    SOC_PPC_FRWRD_FCF_GLBL_INFO_clear(&fc_info);

    fc_info.enable = 1;
    
    unit = (unit);

    soc_sand_rv = soc_ppd_frwrd_fcf_glbl_info_set(unit,&fc_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    trap_id_sw = SOC_PPC_TRAP_CODE_FCOE_WA_FIX_OFFSET;
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_rx_trap_sw_id_to_hw_id(unit,trap_id_sw, &trap_id_converted));
    /* allocate user define trap */
    rv = bcm_petra_rx_trap_type_create(unit, BCM_RX_TRAP_REPLACE|BCM_RX_TRAP_WITH_ID, bcmRxTrapUserDefine, &trap_id_converted);
    /* if it's already exist then just use it*/
    if (rv == BCM_E_EXISTS || rv == BCM_E_RESOURCE) {
        rv = BCM_E_NONE;
    }
    BCMDNX_IF_ERR_EXIT(rv);
    SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO_clear(&trap_info);
    /* fix forwarding header by -14*/
    trap_info.processing_info.frwrd_offset_bytes_fix = -14;
    trap_info.processing_info.frwrd_offset_index = 2;
    trap_info.bitmap_mask = SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_FRWRD_OFFSET;

    /* this will not affect destination as destination-overwrite is not set, just to pass verify*/
    SOC_PPD_FRWRD_DECISION_DROP_SET(unit, &(trap_info.dest_info.frwrd_dest), soc_sand_rv);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    soc_sand_rv = soc_ppd_trap_frwrd_profile_info_set(unit,trap_id_sw,&trap_info,BCM_CORE_ALL);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
     BCMDNX_FUNC_RETURN;
}

/* De-initialize FCOE module */
int bcm_petra_fcoe_cleanup(
    int unit)
{
    return BCM_E_UNAVAIL;
}

/* enable/disable FCOE without clearing any FCOE table */
int bcm_petra_fcoe_enable_set(
    int unit)
{
    return BCM_E_UNAVAIL;
}


/* map bcm key to ppd key */
STATIC uint32 _bcm_petra_foce_route_key_to_ppd(
        int unit, 
        bcm_fcoe_route_t *route,
        SOC_PPC_FRWRD_FCF_ROUTE_KEY *route_key,
        uint32     *insertion_type
   ){
    uint32 db_flags;
    BCMDNX_INIT_FUNC_DEFS;

    SOC_PPC_FRWRD_FCF_ROUTE_KEY_clear(route_key);

    route_key->vfi = route->vrf;
    route_key->prefix_len = bcm_petra_fcoe_mask_length(route->nport_id_mask);
    route_key->d_id = route->nport_id;

    if (ARAD_LPM_CUSTOM_LOOKUP_ENABLED(unit)) { /* custom LPM lookup, we are done */
        route_key->flags |= SOC_PPC_FRWRD_FCF_ROUTE_DOMAIN; /* lookup in LPM */
        /* recalculate it as FCoE uses only 24bits */
        route_key->prefix_len = bcm_ip_mask_length(route->nport_id_mask);

        BCMDNX_FUNC_RETURN;
    }

    if (route_key->prefix_len > _BCM_PETRA_FCOE_LOCAL_HOST_PREFIX) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("max prefix is 24 i.e. 255.255.255.0")));
    }

    /* flags to determine used DB */
    db_flags = route->flags & (BCM_FCOE_HOST_ROUTE|BCM_FCOE_DOMAIN_ROUTE|BCM_FCOE_LOCAL_ADDRESS|BCM_FCOE_SOURCE_ROUTE);

    /* flags mapping */

    /* add local domain*/
    if (db_flags == (_BCM_PETRA_FOCE_ADD_LOCAL_DOMAIN)) {
        /* prefix has to be 8 */
        if (route_key->prefix_len != _BCM_PETRA_FCOE_LOCAL_DOMAIN_PREFIX) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("local domain (BCM_FCOE_LOCAL_ADDRESS) --> prefix has to be = 255.0.0.0")));
        }
        route_key->flags |= SOC_PPC_FRWRD_FCF_ROUTE_LOCAL_DOMAIN;
    }
    else if (db_flags == _BCM_PETRA_FOCE_ADD_NPORT_HOST) {
        /* prefix has to be 24 as this is host entry */
        if (route_key->prefix_len != _BCM_PETRA_FCOE_LOCAL_HOST_PREFIX) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("local host (BCM_FCOE_SOURCE_ROUTE|BCM_FCOE_HOST_ROUTE) --> prefix has to be = 255.255.255.255")));
        }
        route_key->flags |= SOC_PPC_FRWRD_FCF_ROUTE_HOST_NPORT;
    }
    else if (db_flags == _BCM_PETRA_FOCE_ADD_NPORT_DOMAIN) {
        /* prefix has to be 0-8 as the domain routing */
        if (route_key->prefix_len > _BCM_PETRA_FCOE_LOCAL_DOMAIN_PREFIX) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("local host (BCM_FCOE_SOURCE_ROUTE|BCM_FCOE_DOMAIN_ROUTE) --> prefix has to be = 0.0.0.0 - 255.0.0.0")));
        }
        route_key->flags |= SOC_PPC_FRWRD_FCF_ROUTE_DOMAIN_NPORT;
    }
    /* add local host: i.e. local domain and host local  */
    else if (db_flags == _BCM_PETRA_FOCE_ADD_LOCAL_HOST) {
        /* prefix has to be 24 as this is host entry */
        if (route_key->prefix_len != _BCM_PETRA_FCOE_LOCAL_HOST_PREFIX) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("local host (BCM_FCOE_LOCAL_ADDRESS|BCM_FCOE_HOST_ROUTE) --> prefix has to be = 255.255.255.255")));
        }
        route_key->flags |= SOC_PPC_FRWRD_FCF_ROUTE_HOST|SOC_PPC_FRWRD_FCF_ROUTE_LOCAL_DOMAIN;
    }
    /* add remote domain: add to LPM */
    else if (db_flags == _BCM_PETRA_FOCE_ADD_REMOTE_DOMAIN_PREFIX) {
        /* prefix has to be 0-8 as the domain routing*/
        if (route_key->prefix_len >= _BCM_PETRA_FCOE_LOCAL_DOMAIN_PREFIX) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("remote domain (_BCM_PETRA_FCOE_LOCAL_DOMAIN_PREFIX) --> prefix has to be = 0.0.0.0 - 255.0.0.0")));
        }
        route_key->flags |= SOC_PPC_FRWRD_FCF_ROUTE_DOMAIN;
    }
    /* add to host table: either one host (24) or exact remote domain to host-table */
    else if (db_flags == (_BCM_PETRA_FOCE_ADD_EXACT_KEY)) {
        /* prefix has to be 24 as this is host entry */
        if (route_key->prefix_len != _BCM_PETRA_FCOE_LOCAL_DOMAIN_PREFIX && route_key->prefix_len !=_BCM_PETRA_FCOE_LOCAL_HOST_PREFIX) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("remote domain (_BCM_PETRA_FCOE_LOCAL_DOMAIN_PREFIX) --> prefix has to be = 0.0.0.0 - 255.0.0.0")));
        }
        route_key->flags |= SOC_PPC_FRWRD_FCF_ROUTE_HOST;
    }

    *insertion_type = db_flags;

exit:
     BCMDNX_FUNC_RETURN;
}



/* map bcm FCoE route to ppd info */
STATIC uint32 _bcm_petra_foce_route_info_to_ppd(
        int                             unit, 
        bcm_fcoe_route_t                *route,
        SOC_PPC_FRWRD_FCF_ROUTE_INFO    *route_info,
        uint32                          insertion_type
   ){
    /* 0:FEC, 1:port+eep, 2:FEC+eep  */
    int frwrd_type = 0;
    SOC_PPC_FEC_ID fec_id=0;
    uint32
        rif_entry_id;
    int eep_val=0;
    int rv = BCM_E_NONE;
    uint32 soc_sand_rv;

    BCMDNX_INIT_FUNC_DEFS;

    SOC_PPC_FRWRD_FCF_ROUTE_INFO_clear(route_info);

    if (ARAD_LPM_CUSTOM_LOOKUP_ENABLED(unit)) { /* custom LPM lookup */
        route_info->frwrd_decision.dest_id = route->port;
        BCMDNX_FUNC_RETURN; /* we are done */
    }

    /* forwarding info */
    /* no FEC is used and RIF is valid: RIF (DA + vlan) + port  */
    if(BCM_L3_ITF_TYPE_IS_RIF(route->intf)) {
      frwrd_type = 3;/* point to RIF */
    }
    /* EEP is valid this host points to: FEC + EEP */
    else if(route->encap_id != 0){
      frwrd_type = 2;
    }
    else{/* simply host points to FEC */
      frwrd_type = 0;
    }

    if (frwrd_type != 0 && insertion_type == _BCM_PETRA_FOCE_ADD_REMOTE_DOMAIN_PREFIX) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Adding to LPM DB, forwarding information has to be FEC")));
    }

    unit = (unit);

    /* verify parameters according to mode */
    if(frwrd_type == 3) {/* no FEC used  */
      /* port has to be valid forwarding is port */
      if(route->port == BCM_GPORT_TYPE_NONE) {
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("not Passing through FEC, port has to be valid")));
      }
      if(route->encap_id == 0) {
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("not Passing through FEC, encap has to be valid")));
       }
    }

    /* destination is FEC */
    if(BCM_L3_ITF_TYPE_IS_FEC(route->intf)) {
        _bcm_l3_intf_to_fec(unit, route->intf, &fec_id);
        SOC_PPD_FRWRD_DECISION_FEC_SET(unit, &route_info->frwrd_decision, fec_id, soc_sand_rv);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
    else { /* destination is port */
        /*  port has to be valid then use it and point to oultif */
        rv = _bcm_dpp_gport_to_fwd_decision(unit,route->port,&route_info->frwrd_decision);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* set additional info */
    if(frwrd_type == 3) { /* port + outRIF */
        rif_entry_id = BCM_L3_ITF_VAL_GET(route->intf);
        route_info->frwrd_decision.additional_info.outlif.type = SOC_PPC_OUTLIF_ENCODE_TYPE_RAW;
        route_info->frwrd_decision.additional_info.outlif.val = rif_entry_id;
    }
    /* FEC + MAC --> forward decision = FEC + ARP (EEI)*/
    else if(frwrd_type == 2) {
        route_info->frwrd_decision.additional_info.eei.type = SOC_PPC_EEI_TYPE_OUTLIF;
        _bcm_l3_encap_id_to_eep(unit, route->encap_id, &eep_val);
        route_info->frwrd_decision.additional_info.eei.val.outlif = eep_val;        
    }

exit:
     BCMDNX_FUNC_RETURN;
}





/* map bcm FCoE route to ppd info */
STATIC uint32 _bcm_petra_foce_route_info_from_ppd(
        int                             unit, 
        bcm_fcoe_route_t                *route,
        SOC_PPC_FRWRD_FCF_ROUTE_INFO    *route_info
   ){
    /* 0:FEC, 1:port+eep, 2:FEC+eep  */
    int frwrd_type = 0;
    int eep_val=0;
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;


    /* calc forwarding type */
    /* Default: forwarding info is FEC */
    frwrd_type = 0;
    /* FEC + encap (MAC) --> forward decision = FEC + ARP (EEI)*/
    if (route_info->frwrd_decision.additional_info.eei.type == SOC_PPC_EEI_TYPE_OUTLIF) {
        eep_val = route_info->frwrd_decision.additional_info.eei.val.outlif;
        if(eep_val != 0){
            frwrd_type = 2;
        }
    }/* EEP + port */
    else if (route_info->frwrd_decision.additional_info.outlif.type == SOC_PPC_OUTLIF_ENCODE_TYPE_RAW) {
        eep_val = route_info->frwrd_decision.additional_info.outlif.val;
        if(eep_val != 0){
            frwrd_type = 1;
        }
    }
    if((frwrd_type == 0 || frwrd_type == 2) && (route_info->frwrd_decision.type != SOC_PPC_FRWRD_DECISION_TYPE_FEC)){
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("expect destination type to be FEC")));
    }

    /* get FEC */
    if (frwrd_type == 0 || frwrd_type == 2) {
        _bcm_l3_fec_to_intf(unit, route_info->frwrd_decision.dest_id, &route->intf);
    }
    else{ /* get dest-port*/
        rv = _bcm_dpp_gport_and_encap_from_fwd_decision(unit,&route->port,NULL,&(route_info->frwrd_decision),-1/*encap_usage*/,0/*force_destination*/);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* get additional info */
    /* get RIF*/
    if (frwrd_type == 2) { 
        BCM_L3_ITF_SET(route->intf,BCM_L3_ITF_TYPE_RIF,eep_val);
    }
    /* set encap */
    if (frwrd_type == 1) { 
        BCM_L3_ITF_SET(route->intf,BCM_L3_ITF_TYPE_LIF,eep_val);
    }

exit:
     BCMDNX_FUNC_RETURN;
}


/* map bcm FCoE route to ppd info */
STATIC uint32 _bcm_petra_foce_route_info_clear(
        int                             unit, 
        bcm_fcoe_route_t                *route)
{
    bcm_fcoe_route_t
        clean_route;

    clean_route.vrf = route->vrf;
    clean_route.nport_id_mask = route->nport_id_mask;
    clean_route.nport_id = route->nport_id;
    clean_route.flags = route->flags;
    bcm_fcoe_route_t_init(route);

    route->vrf = clean_route.vrf;
    route->nport_id_mask = clean_route.nport_id_mask;
    route->nport_id = clean_route.nport_id;
    route->flags = clean_route.flags;
    return BCM_E_NONE;
}


/* Zoning utilities*/
/* map bcm key to ppd key */
STATIC uint32 _bcm_petra_foce_zone_key_to_ppd(
   int                             unit, 
   bcm_fcoe_zone_entry_t           *zone,
   SOC_PPC_FRWRD_FCF_ZONING_KEY    *zone_key
)
{
    BCMDNX_INIT_FUNC_DEFS;

    SOC_PPC_FRWRD_FCF_ZONING_KEY_clear(zone_key);

    zone_key->d_id = zone->d_id;
    zone_key->s_id = zone->s_id;
    zone_key->vfi = zone->vsan_id;

     BCMDNX_FUNC_RETURN;
}



STATIC uint32 _bcm_petra_foce_zone_info_to_ppd(
        int unit, 
        bcm_fcoe_zone_entry_t *zone,
        SOC_PPC_FRWRD_FCF_ZONING_INFO *zone_info
   )
{
    int rv = BCM_E_NONE;
    uint32 soc_sand_rv;
    BCMDNX_INIT_FUNC_DEFS;

    SOC_PPC_FRWRD_FCF_ZONING_INFO_clear(zone_info);

    switch (zone->action) {
        /* point to trap code with allow packet */
        case bcmFcoeZoneActionAllow:
            SOC_PPD_FRWRD_DECISION_TRAP_SET(unit, &zone_info->frwrd_decision, _BCM_PETRA_FCOE_ZONE_ALLOW_TRAP_CODE, _BCM_PETRA_FCOE_ZONE_ALLOW_STRENGTH, 0, soc_sand_rv);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        break;
        /* point to trap code with drop packet */
        case bcmFcoeZoneActionDeny:
            SOC_PPD_FRWRD_DECISION_TRAP_SET(unit, &zone_info->frwrd_decision, _BCM_PETRA_FCOE_ZONE_DROP_TRAP_CODE, _BCM_PETRA_FCOE_ZONE_DROP_STRENGTH, 0, soc_sand_rv);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        break;
        /* map dest port to fwd-desicion */
        case bcmFcoeZoneActionRedirect:
            rv = _bcm_dpp_gport_to_fwd_decision(unit,zone->dest_port,&zone_info->frwrd_decision);
            BCMDNX_IF_ERR_EXIT(rv);
        break;
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not supported Action")));
    }
exit:
     BCMDNX_FUNC_RETURN;
}

STATIC uint32 _bcm_petra_foce_zone_info_from_ppd(
        int unit, 
        bcm_fcoe_zone_entry_t *zone,
        SOC_PPC_FRWRD_FCF_ZONING_INFO *zone_info
   )
{
    int rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    /* if drop/allow trap */
    if (zone_info->frwrd_decision.type == SOC_PPC_FRWRD_DECISION_TYPE_TRAP) {
        if (zone_info->frwrd_decision.additional_info.trap_info.action_profile.trap_code == _BCM_PETRA_FCOE_ZONE_ALLOW_TRAP_CODE) {
            zone->action = bcmFcoeZoneActionAllow;
            zone->dest_port = 0;
            BCM_EXIT;
        }
        else if (zone_info->frwrd_decision.additional_info.trap_info.action_profile.trap_code == _BCM_PETRA_FCOE_ZONE_DROP_TRAP_CODE) {
            zone->action = bcmFcoeZoneActionDeny;
            zone->dest_port = 0;
            BCM_EXIT;
        }
    }

    /* not drop/allow trap then this is redirect */
    zone->action = bcmFcoeZoneActionRedirect;
    rv = _bcm_dpp_gport_and_encap_from_fwd_decision(unit,&zone->dest_port,NULL,&(zone_info->frwrd_decision),-1/*encap_usage*/,0/*force_destination*/);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
     BCMDNX_FUNC_RETURN;
}



/* Add an entry into the FCOE routing table */
int bcm_petra_fcoe_route_add(
    int unit, 
    bcm_fcoe_route_t *route)
{
    SOC_PPC_FRWRD_FCF_ROUTE_KEY 
        route_key;
    SOC_PPC_FRWRD_FCF_ROUTE_INFO 
        route_info;
    uint32     
        insertion_type;
    SOC_SAND_SUCCESS_FAILURE
        failure_indication;
    int rv = BCM_E_NONE;
    uint32 soc_sand_rv;

    BCMDNX_INIT_FUNC_DEFS;

    /* map to PPD key */
    rv = _bcm_petra_foce_route_key_to_ppd(unit,route,&route_key,&insertion_type);
    BCMDNX_IF_ERR_EXIT(rv);

    /* map to PPD info*/
    rv = _bcm_petra_foce_route_info_to_ppd(unit,route,&route_info,insertion_type);
    BCMDNX_IF_ERR_EXIT(rv);

    unit = (unit);

    /* make insertion */
    soc_sand_rv = soc_ppd_frwrd_fcf_route_add(unit,&route_key,&route_info,&failure_indication);
    SOC_SAND_IF_FAIL_RETURN(failure_indication);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
     BCMDNX_FUNC_RETURN;
}

/* Delete an nport_id entry from the FCOE routing table */
int bcm_petra_fcoe_route_delete(
    int unit, 
    bcm_fcoe_route_t *route)
{
    SOC_PPC_FRWRD_FCF_ROUTE_KEY 
        route_key;
    SOC_PPC_FRWRD_FCF_ROUTE_INFO 
        route_info;
    SOC_PPC_FRWRD_FCF_ROUTE_STATUS
        route_status;
    uint32     
        insertion_type;
    uint8
        found;
    SOC_SAND_SUCCESS_FAILURE
        failure_indication;
    int rv = BCM_E_NONE;
    uint32 soc_sand_rv;

    BCMDNX_INIT_FUNC_DEFS;

    /* map to PPD key */
    rv = _bcm_petra_foce_route_key_to_ppd(unit,route,&route_key,&insertion_type);
    BCMDNX_IF_ERR_EXIT(rv);

    /* map to PPD info*/
    rv = _bcm_petra_foce_route_info_to_ppd(unit,route,&route_info,insertion_type);
    BCMDNX_IF_ERR_EXIT(rv);

    unit = (unit);

    if (ARAD_LPM_CUSTOM_LOOKUP_ENABLED(unit)) { /* custom LPM lookup */
        soc_sand_rv = soc_ppd_frwrd_fcf_route_remove(unit,&route_key,&failure_indication);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        SOC_SAND_IF_FAIL_RETURN(failure_indication);
        BCMDNX_FUNC_RETURN;
    }

    /* check if exist */
    soc_sand_rv = soc_ppd_frwrd_fcf_route_get(unit,&route_key,TRUE,&route_info,&route_status,&found);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("route is not found")));
    }

    /* remove it */
    soc_sand_rv = soc_ppd_frwrd_fcf_route_remove(unit,&route_key,&failure_indication);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    SOC_SAND_IF_FAIL_RETURN(failure_indication);

exit:
     BCMDNX_FUNC_RETURN;
}

/* Delete FCOE entries based on Masked Domain prefix (network) */
int bcm_petra_fcoe_route_delete_by_prefix(
    int unit, 
    bcm_fcoe_route_t *route)
{
    return BCM_E_UNAVAIL;
}

/* Delete FCOE entries that do/don't match a specified L3 interface */
int bcm_petra_fcoe_route_delete_by_interface(
    int unit, 
    bcm_fcoe_route_t *route)
{
    return BCM_E_UNAVAIL;
}

/* Deletes all FCOE routing table entries */
int bcm_petra_fcoe_route_delete_all(
    int unit)

{
    uint32 soc_sand_rv;

    BCMDNX_INIT_FUNC_DEFS;

    unit = (unit);

    /* delete all host/routes in LEM, LPM (not clearing SEM)*/
    soc_sand_rv = soc_ppd_frwrd_fcf_routing_table_clear(unit,0,0);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
     BCMDNX_FUNC_RETURN;
}


/* Look up an FCOE routing table entry based on nport id */
int bcm_petra_fcoe_route_find(
    int unit, 
    bcm_fcoe_route_t *route)
{
    SOC_PPC_FRWRD_FCF_ROUTE_KEY 
        route_key;
    SOC_PPC_FRWRD_FCF_ROUTE_INFO 
        route_info;
    SOC_PPC_FRWRD_FCF_ROUTE_STATUS
        route_status;
    uint32     
        insertion_type;
    uint8
        found;
    int rv = BCM_E_NONE;
    uint32 soc_sand_rv;

    BCMDNX_INIT_FUNC_DEFS;

    /* map to PPD key */
    rv = _bcm_petra_foce_route_key_to_ppd(unit,route,&route_key,&insertion_type);
    BCMDNX_IF_ERR_EXIT(rv);

    _bcm_petra_foce_route_info_clear(unit,route);

    unit = (unit);

    /* call ppd find */
    soc_sand_rv = soc_ppd_frwrd_fcf_route_get(unit,&route_key,TRUE,&route_info,&route_status,&found);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("route is not found")));
    }

    rv = _bcm_petra_foce_route_info_from_ppd(unit,route,&route_info);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
     BCMDNX_FUNC_RETURN;
}



/* Allocate and configure a VSAN */
int bcm_petra_fcoe_vsan_create(
    int unit, 
    uint32 options,
    bcm_fcoe_vsan_t *vsan,
    bcm_fcoe_vsan_id_t *vsan_id)
{
    return BCM_E_UNAVAIL;
}

/* Destroy a VSAN */
int bcm_petra_fcoe_vsan_destroy(
    int unit, 
    bcm_fcoe_vsan_id_t vsan_id)
{
    return BCM_E_UNAVAIL;
}

/* Destroy all VSANs */
int bcm_petra_fcoe_vsan_destroy_all(
    int unit)
{
    return BCM_E_UNAVAIL;
}

/* Get VSAN controls */
int bcm_petra_fcoe_control_vsan_get(
    int unit, 
    uint32 vsan_id, 
    bcm_fcoe_vsan_control_t type, 
    int *arg)
{
    return BCM_E_UNAVAIL;
}

/* Set VSAN controls */
int bcm_petra_fcoe_control_vsan_set(
    int unit, 
    uint32 vsan_id, 
    bcm_fcoe_vsan_control_t type, 
    int arg)
{
    return BCM_E_UNAVAIL;
}

/* Get VSAN properties associated with a VSAN ID */
int bcm_petra_fcoe_vsan_get(
    int unit, 
    uint32 vsan_id, 
    bcm_fcoe_vsan_t *vsan)
{
    return BCM_E_UNAVAIL;
}

/* Update VSAN properties associated with a VSAN ID */
int bcm_petra_fcoe_vsan_set(
    int unit, 
    uint32 vsan_id, 
    bcm_fcoe_vsan_t *vsan)
{
    return BCM_E_UNAVAIL;
}
/* Add Zone check entry */
int bcm_petra_fcoe_zone_add(
    int unit, 
    bcm_fcoe_zone_entry_t *zone)
{ 
    SOC_PPC_FRWRD_FCF_ZONING_KEY 
        zone_key;
    SOC_PPC_FRWRD_FCF_ZONING_INFO 
        zone_info;
    SOC_SAND_SUCCESS_FAILURE
        failure_indication;
    int rv = BCM_E_NONE;
    uint32 soc_sand_rv;

    BCMDNX_INIT_FUNC_DEFS;

    /* map to PPD key */
    rv = _bcm_petra_foce_zone_key_to_ppd(unit,zone,&zone_key);
    BCMDNX_IF_ERR_EXIT(rv);

    if (zone->action != bcmFcoeZoneActionAllow) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not supported Action")));
    }

    /* map to PPD info*/
    rv = _bcm_petra_foce_zone_info_to_ppd(unit,zone,&zone_info);
    BCMDNX_IF_ERR_EXIT(rv);

    unit = (unit);

    /* make insertion */
    soc_sand_rv = soc_ppd_frwrd_fcf_zoning_add(unit,&zone_key,&zone_info,&failure_indication);
    SOC_SAND_IF_FAIL_RETURN(failure_indication);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
     BCMDNX_FUNC_RETURN;
}

/* Delete all zone check entries matching VSAN ID */
int bcm_petra_fcoe_zone_delete_by_vsan(
    int unit, 
    bcm_fcoe_zone_entry_t *zone)
{
    SOC_PPC_FRWRD_FCF_ZONING_KEY 
        zone_key;
    SOC_PPC_FRWRD_FCF_ZONING_INFO 
        zone_info;
    SOC_PPC_FRWRD_FCF_ROUTE_STATUS
        entry_status;
    uint8
        found;
    int rv = BCM_E_NONE;
    uint32 soc_sand_rv;

    BCMDNX_INIT_FUNC_DEFS;

    /* map to PPD key */
    rv = _bcm_petra_foce_zone_key_to_ppd(unit,zone,&zone_key);
    BCMDNX_IF_ERR_EXIT(rv);

    unit = (unit);

    /* check if exist */
    soc_sand_rv = soc_ppd_frwrd_fcf_zoning_get(unit,&zone_key,&zone_info,&entry_status,&found);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("zone entry is not found")));
    }

    /* make insertion */
    soc_sand_rv = soc_ppd_frwrd_fcf_zoning_remove(unit,&zone_key);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
     BCMDNX_FUNC_RETURN;
}

/* Delete all zone check entries with matching Source ID */
int bcm_petra_fcoe_zone_delete_by_sid(
    int unit, 
    bcm_fcoe_zone_entry_t *zone)
{
    SOC_PPC_FRWRD_FCF_ZONING_KEY
        remove_key;
    SOC_PPC_FRWRD_FCF_ZONING_INFO
        remove_info;
    uint32 
        remove_flags;
    
    int rv = BCM_E_NONE;
    uint32 soc_sand_rv;

    BCMDNX_INIT_FUNC_DEFS;

    unit = (unit);

    /* map to PPD key */
    rv = _bcm_petra_foce_zone_key_to_ppd(unit,zone,&remove_key);
    BCMDNX_IF_ERR_EXIT(rv);

    if (zone->action != bcmFcoeZoneActionAllow) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not supported Action")));
    }

    /* map to PPD info*/
    rv = _bcm_petra_foce_zone_info_to_ppd(unit,zone,&remove_info);
    BCMDNX_IF_ERR_EXIT(rv);

    remove_flags = SOC_PPC_FRWRD_FCF_ZONE_REMOVE_BY_VFT;

    /* delete all zoning in LEM according to VFI */
    soc_sand_rv = soc_ppd_frwrd_fcf_zoning_table_clear(unit,remove_flags,&remove_key,&remove_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
     BCMDNX_FUNC_RETURN;

}

/* Delete all zone check entries with matching Destination ID */
int bcm_petra_fcoe_zone_delete_by_did(
    int unit, 
    bcm_fcoe_zone_entry_t *zone)
{
    SOC_PPC_FRWRD_FCF_ZONING_KEY
        remove_key;
    SOC_PPC_FRWRD_FCF_ZONING_INFO
        remove_info;
    uint32 
        remove_flags;
    
    int rv = BCM_E_NONE;
    uint32 soc_sand_rv;

    BCMDNX_INIT_FUNC_DEFS;

    unit = (unit);

    /* map to PPD key */
    rv = _bcm_petra_foce_zone_key_to_ppd(unit,zone,&remove_key);
    BCMDNX_IF_ERR_EXIT(rv);

    if (zone->action != bcmFcoeZoneActionAllow) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not supported Action")));
    }

    /* map to PPD info*/
    rv = _bcm_petra_foce_zone_info_to_ppd(unit,zone,&remove_info);
    BCMDNX_IF_ERR_EXIT(rv);

    remove_flags = SOC_PPC_FRWRD_FCF_ZONE_REMOVE_BY_D_ID;

    /* delete all zoning in LEM according to DID */
    soc_sand_rv = soc_ppd_frwrd_fcf_zoning_table_clear(unit,remove_flags,&remove_key,&remove_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
     BCMDNX_FUNC_RETURN;
}

/* Delete all zone check entries */
int bcm_petra_fcoe_zone_delete_all(
    int unit)
{ 
    SOC_PPC_FRWRD_FCF_ZONING_KEY
        remove_key;
    SOC_PPC_FRWRD_FCF_ZONING_INFO
        remove_info;
    uint32 
        remove_flags;
    
    uint32 soc_sand_rv;

    BCMDNX_INIT_FUNC_DEFS;

    unit = (unit);


    SOC_PPC_FRWRD_FCF_ZONING_KEY_clear(&remove_key);
    SOC_PPC_FRWRD_FCF_ZONING_INFO_clear(&remove_info);

    remove_flags = 0;

    /* delete all zoning entries in LEM */
    soc_sand_rv = soc_ppd_frwrd_fcf_zoning_table_clear(unit,remove_flags,&remove_key,&remove_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
     BCMDNX_FUNC_RETURN;
}

/* Delete all zone check entries */
int bcm_petra_fcoe_zone_delete(
    int unit, 
    bcm_fcoe_zone_entry_t *zone)
{ 
    SOC_PPC_FRWRD_FCF_ZONING_KEY 
        zone_key;
    SOC_PPC_FRWRD_FCF_ZONING_INFO 
        zone_info;
    SOC_PPC_FRWRD_FCF_ROUTE_STATUS
        entry_status;
    uint8
        found;
    int rv = BCM_E_NONE;
    uint32 soc_sand_rv;

    BCMDNX_INIT_FUNC_DEFS;

    /* map to PPD key */
    rv = _bcm_petra_foce_zone_key_to_ppd(unit,zone,&zone_key);
    BCMDNX_IF_ERR_EXIT(rv);

    unit = (unit);

    /* check if exist */
    soc_sand_rv = soc_ppd_frwrd_fcf_zoning_get(unit,&zone_key,&zone_info,&entry_status,&found);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("zone entry is not found")));
    }


    soc_sand_rv = soc_ppd_frwrd_fcf_zoning_remove(unit,&zone_key);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);


exit:
     BCMDNX_FUNC_RETURN;
}

/* Retrieve Zone info */
int bcm_petra_fcoe_zone_get(
    int unit, 
    bcm_fcoe_zone_entry_t *zone)
{ 
    SOC_PPC_FRWRD_FCF_ZONING_KEY 
        zone_key;
    SOC_PPC_FRWRD_FCF_ZONING_INFO 
        zone_info;
    SOC_PPC_FRWRD_FCF_ROUTE_STATUS
        entry_status;
    uint8
        found;
    int rv = BCM_E_NONE;
    uint32 soc_sand_rv;

    BCMDNX_INIT_FUNC_DEFS;

    /* map to PPD key */
    rv = _bcm_petra_foce_zone_key_to_ppd(unit,zone,&zone_key);
    BCMDNX_IF_ERR_EXIT(rv);

    unit = (unit);

    /* check if exist */
    soc_sand_rv = soc_ppd_frwrd_fcf_zoning_get(unit,&zone_key,&zone_info,&entry_status,&found);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("zone entry is not found")));
    }

    rv = _bcm_petra_foce_zone_info_from_ppd(unit,zone,&zone_info);
    BCMDNX_IF_ERR_EXIT(rv);


exit:
     BCMDNX_FUNC_RETURN;
}

