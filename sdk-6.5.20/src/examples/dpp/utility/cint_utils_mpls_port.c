/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file provides MPLS PORT basic functionality and defines MPLS_PORT global variables
 */

/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization            -----------------
 */
/* Struct definitions */

struct mpls_port__forward_group__info_s {
    
    int flags;
    int flags2;
    int encap_id; /* FEC will point to this value */
    int port;
    int second_heirarchy_fec; /* used in case the FEC is cascaded */
    int mpls_port_forward_group_id; /* out id parameter */

};


/* Globals */
mpls_port_utils_s g_mpls_port_utils = { 6202 /* vpn */,0,0,0,0,0,0,0,0,0,0,0,0,0};

/* *************************************************************************************************** */

/*
 *  mpls_port__fec_only__create
 *  For protected gport that allocate only FEC.
 *  The indication is by setting the encap_id with BCM_ENCAP_REMOTE_SET().
 *  from SDK implementation side if given bcm_mpls_port_create called with BCM_MPLS_PORT_ENCAP_WITH_ID
 *  where the encap_id parameter has BCM_ENCAP_REMOTE_SET(): then this encap used as remote-LIF
 *  i.e. it's pointed by FEC but not allocated or configured.
 *  For Jericho egress_tunnel_if can be used in case of H-VPLS.
 *  Note:
 *  - this call can be done only for protected mpls_port i.e. failover_id valid.
 *  - when configure attribute of LIF the user need to supply the gport_id returned
 *  when calling bcm_mpls_port_create for the LIF. if FEC gport provided error is returned
 */
int
mpls_port__fec_only__create(
    int unit,
    mpls_port__fec_only_info_s * mpls_port_fec_info
    )
{
    int rv;
    bcm_mpls_port_t mpls_port;
    
    bcm_mpls_port_t_init(&mpls_port);
  
    mpls_port.criteria = BCM_MPLS_PORT_MATCH_INVALID; /* not relevant for FEC */
    mpls_port.port = mpls_port_fec_info->port_id; /* physical port */
    mpls_port.encap_id = mpls_port_fec_info->encap_id;
    BCM_ENCAP_REMOTE_SET(&(mpls_port.encap_id));
    mpls_port.flags |= BCM_MPLS_PORT_ENCAP_WITH_ID; /* Must be used, specify LIF ID is remote */
    mpls_port.flags |= BCM_MPLS_PORT_EGRESS_TUNNEL; 
    mpls_port.failover_port_id = mpls_port_fec_info->failover_port_id;
    mpls_port.failover_id = mpls_port_fec_info->failover_id;
    mpls_port.flags |= mpls_port_fec_info->flags;
    mpls_port.flags2 |= mpls_port_fec_info->flags2;

    if (BCM_MULTICAST_IS_SET(mpls_port_fec_info->mc_group_id)) {
       mpls_port.failover_mc_group = mpls_port_fec_info->mc_group_id;
    } else {
       mpls_port.egress_tunnel_if = mpls_port_fec_info->egress_tunnel_if;
    }

    if (mpls_port_fec_info->gport!=0) {
        mpls_port.flags |= BCM_MPLS_PORT_WITH_ID;
        BCM_GPORT_MPLS_PORT_ID_SET(mpls_port.mpls_port_id,mpls_port_fec_info->gport); /* this is the FEC id */
    }
   
    rv = bcm_mpls_port_add(unit, 0 /* No VPN required in FEC creation */, mpls_port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mpls_port_add\n"); 
        return rv;
    }
    if(verbose >= 1) {
        if (BCM_MULTICAST_IS_SET(mpls_port_fec_info->mc_group_id)) {
          printf("unit (%d): created mpls_port (FEC) :0x%08x point to multicast group:0x%08x\n",unit,mpls_port.mpls_port_id,mpls_port_fec_info->mc_group_id);
        } else {
          printf("unit (%d): created mpls_port (FEC) :0x%08x point to remote LIF:0x%08x\n",unit,mpls_port.mpls_port_id,mpls_port_fec_info->encap_id);
        }
    }

    printf("mpls port id=0x%x.\n",mpls_port.mpls_port_id); 

    mpls_port_fec_info->gport = mpls_port.mpls_port_id;

    return BCM_E_NONE;
}



/*
 *  mpls_port__forward_group__create
 *  Allocating MPLS PORT non-protected FEC without allocating LIF.
 *  The indication is by setting the encap_id with BCM_ENCAP_REMOTE_SET() and flag BCM_MPLS_PORT_FORWARD_GROUP.
 */
int
mpls_port__forward_group__create(
    int unit,
    mpls_port__forward_group__info_s *forward_group_info
    )
{
    int rv;
    bcm_mpls_port_t mpls_port;

    bcm_mpls_port_t_init(&mpls_port);
  
    /* set port attribures */
    mpls_port.flags = BCM_MPLS_PORT_EGRESS_TUNNEL|BCM_MPLS_PORT_FORWARD_GROUP|BCM_MPLS_PORT_ENCAP_WITH_ID;
    mpls_port.flags |= forward_group_info->flags;
    mpls_port.flags2 |= forward_group_info->flags2;
    mpls_port.encap_id = forward_group_info->encap_id;
    BCM_ENCAP_REMOTE_SET(mpls_port.encap_id); /* in forwarding group remote bit should always be set */
    mpls_port.port = forward_group_info->port;
    if (forward_group_info->second_heirarchy_fec) {
        mpls_port.egress_tunnel_if = forward_group_info->second_heirarchy_fec;
    }

    rv = bcm_mpls_port_add(unit, 0, &mpls_port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mpls_port_add\n");
        print rv;
        return rv;
    }

    if(verbose){
        printf("MPLS PORT: FORWARD GROUP created: 0x%x.\n", mpls_port.mpls_port_id);
    }

    forward_group_info->mpls_port_forward_group_id = mpls_port.mpls_port_id;

    return BCM_E_NONE;
}


/* Create a standard PWE Out-LIF*/
int mpls_port__egress_only_create(int unit, mpls_port__egress_only_info_s *mpls_egress_only) {

    int rv = BCM_E_NONE;
    bcm_mpls_port_t mpls_port;

    bcm_mpls_port_t_init(&mpls_port);

    mpls_port.flags = mpls_egress_only->flags;
    mpls_port.flags |= BCM_MPLS_PORT_EGRESS_TUNNEL|BCM_MPLS_PORT_ENCAP_WITH_ID;
    mpls_port.flags |= is_device_or_above(unit, JERICHO2) ? 0 : BCM_MPLS_PORT_NETWORK;

    /* Setting default flags for egress only */
    mpls_port.flags2 = mpls_egress_only->flags2;
    mpls_port.flags2 |= BCM_MPLS_PORT2_EGRESS_ONLY;

    /* Set WITH_ID flag if mpls_port_id is given*/
    if(mpls_egress_only->mpls_port_id) {
        mpls_port.flags |= BCM_MPLS_PORT_WITH_ID;
        mpls_port.mpls_port_id = mpls_egress_only->mpls_port_id;
    }

    if(mpls_egress_only->egress_tunnel_if) {
        mpls_port.egress_tunnel_if = mpls_egress_only->egress_tunnel_if;
    }

    if(mpls_egress_only->encap_id) {
        mpls_port.encap_id = mpls_egress_only->encap_id;
    }
    /*Setting match port to invalid EGRESS_ONLY*/
    mpls_port.criteria = BCM_MPLS_PORT_MATCH_INVALID;
    mpls_port.port = BCM_GPORT_INVALID;

    /*Setting match label to invalid for EGRESS_ONLY*/
    mpls_port.match_label = 0;

    mpls_port.egress_label = mpls_egress_only->egress_label;

    /* Traps packet if TTL is 1 */
    if(mpls_egress_only->vccv_type) {
        mpls_port.vccv_type = bcmMplsPortControlChannelTtl;
    }

    int encap_opt = mpls_egress_only->encap_optimized;
    mpls_port__egress_tunnel_if_egress_only__set(unit, mpls_port.egress_tunnel_if, encap_opt, &mpls_port);

    if(!mpls_egress_only->network_group) {
        rv = mpls_port__update_network_group_id(unit, &mpls_port);
        if (rv != BCM_E_NONE) {
            printf("Error, Egress mpls_port__update_network_group_id\n");
            return rv;
        }
    } else {
        if (is_device_or_above(unit,JERICHO) && soc_property_get(unit , "split_horizon_forwarding_groups_mode",1)) {
            mpls_port.network_group_id = mpls_egress_only->network_group;    
        }
    }

    rv = bcm_mpls_port_add(unit, mpls_egress_only->vpn, &mpls_port);
    if(rv != BCM_E_NONE) {
        printf("Error, bcm_mpls_port_add EGRESS ONLY\n");
        return rv;
    }
    mpls_egress_only->mpls_port_id = mpls_port.mpls_port_id;

    mpls_egress_only->encap_id = mpls_port.encap_id;

    return rv;

}


/* Create a standard multi point mpls port binded with a regular mpls label in one entry
   User is expected to pass the mpls label information in mpls_tunnel_properties*/
int mpls_port__mp_create_egress_mpls_port_binded_with_mpls(int unit, mpls_port_utils_s *mpls_port_properties, mpls__egress_tunnel_utils_s *mpls_tunnel_properties){

    int rv = BCM_E_NONE;
    bcm_mpls_port_t mpls_port;
    
    bcm_mpls_egress_label_t label_array[1];
    int label_count;
    bcm_if_t pwe_encap_id;

    /* Create pwe */
    rv = mpls_port__mp_create_ingress_egress_mpls_port(unit,mpls_port_properties);
    if (rv != BCM_E_NONE) {
        printf("Error, mpls_port__mp_create_ingress_egress_mpls_port\n");
        print rv;
        return rv;
    }

    /* Get the pwe */
    BCM_L3_ITF_SET(pwe_encap_id, BCM_L3_ITF_TYPE_LIF, mpls_port_properties->encap_id);
    rv = bcm_mpls_tunnel_initiator_get(unit, pwe_encap_id, 1, &label_array, &label_count);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_mpls_tunnel_initiator_get\n");
        return rv;
    }

    /* Bind the pwe to the mpls label */
    mpls_tunnel_properties->label_in = label_array[0].label;
    mpls_tunnel_properties->with_exp = 1;
    mpls_tunnel_properties->exp = label_array[0].exp;
    mpls_tunnel_properties->ttl = label_array[0].ttl;
    mpls_tunnel_properties->tunnel_id = pwe_encap_id;
    mpls_tunnel_properties->egress_action = BCM_MPLS_EGRESS_ACTION_PUSH;
    mpls_tunnel_properties->flags |= BCM_MPLS_EGRESS_LABEL_ACTION_VALID;
    rv = mpls__create_tunnel_initiator__set(unit,mpls_tunnel_properties);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls__create_tunnel_initiator__set\n");
        return rv;
    }

    return rv;

}


