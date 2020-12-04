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

struct vswitch_vpls_shared_info_s {

    /* don't modify used by script*/
    int egress_intf;
    int egress_intf2;
    bcm_vlan_t vpn;

};

struct mpls_port_utils_s
{
    uint32 vpn;
    int flags;
    int flags2;
    bcm_gport_t mpls_port_id;
    bcm_mpls_label_t egress_pwe_label;
    bcm_mpls_label_t ingress_pwe_label;
    bcm_if_t encap_id;
    bcm_mpls_port_match_t ingress_matching_criteria;
    bcm_if_t fec;
    bcm_if_t egress_mpls_tunnel_id;
    bcm_switch_network_group_t network_group;
    uint8 ttl;
    uint8 exp;
    int egress_label_flags;
    bcm_gport_t port;
    int vccv_type;

    bcm_failover_t failover_id;
    bcm_gport_t    failover_port_id;
    bcm_multicast_t failover_mc_group;

};

struct mpls_port__ingress_only_info_s {

    uint32 vpn;
    bcm_gport_t mpls_port_id;
    uint32 flags;
    uint32 flags2;
    bcm_mpls_port_match_t ingress_matching_criteria;
    bcm_gport_t port;
    bcm_if_t learn_egress_if;
    bcm_failover_t failover_id;
    bcm_gport_t failover_port_id;
    bcm_if_t encap_id;
    bcm_mpls_label_t ingress_pwe_label;
    bcm_gport_t gal_primary_tunnel_id;
    bcm_mpls_egress_label_t learn_egress_label; /* PWE Label for ingress learning */
    bcm_switch_network_group_t network_group;
    bcm_multicast_t failover_mc_group;
    bcm_failover_t ingress_failover_id;

};

struct mpls_port__egress_only_info_s {

    uint32 vpn;
    bcm_gport_t mpls_port_id;
    uint32 flags;
    uint32 flags2;
    bcm_if_t egress_tunnel_if;
    bcm_failover_t failover_id;
    bcm_gport_t failover_port_id;
    bcm_if_t encap_id;
    bcm_mpls_egress_label_t egress_label;
    int encap_optimized;
    bcm_switch_network_group_t network_group;
    int vccv_type;
};

struct mpls_port__fec_only_info_s {
    bcm_port_t  port_id;
    bcm_gport_t failover_id;
    bcm_gport_t failover_port_id; /* secondary, if this is primary*/
    bcm_if_t    encap_id; /* outLif ID */
    bcm_gport_t gport; /* InOut */
    bcm_if_t egress_tunnel_if;
    bcm_multicast_t mc_group_id;
    uint32 flags;
    uint32 flags2;
};

vswitch_vpls_shared_info_s vswitch_vpls_shared_info_1;

int
mpls_port__init(int unit)
{
    int rv;
    bcm_info_t info;

    rv = mpls__mpls_pipe_mode_exp_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls__mpls_pipe_mode_exp_set\n");
        return rv;
    }

    rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_info_get\n");
        print rv;
        return rv;
    }

    return rv;
}


/* create VPN */
int
mpls_port__vswitch_vpls_vpn_create__set(int unit, bcm_vpn_t vpn){
    bcm_mpls_vpn_config_t vpn_info;
    bcm_mpls_vpn_config_t_init(&vpn_info);
    vpn_info.vpn = vpn;
    int mc_group;
    int rv;

    printf("vswitch_vpls_vpn_create %d\n", vpn);

    /* destroy before create, just to be sure it will not fail */
    rv = bcm_mpls_vpn_id_destroy(unit,vpn);

    /* VPLS VPN with user supplied VPN ID */
    vpn_info.flags = BCM_MPLS_VPN_VPLS|BCM_MPLS_VPN_WITH_ID;
    vpn_info.broadcast_group = vpn_info.vpn;
    vpn_info.unknown_multicast_group = vpn_info.vpn;
    vpn_info.unknown_unicast_group = vpn_info.vpn;
    rv = bcm_mpls_vpn_id_create(unit,&vpn_info);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_mpls_vpn_id_create\n");
        return rv;
    }
    if(verbose1){
        printf("created new VPN %d\n", vpn);
    }

    vswitch_vpls_shared_info_1.vpn = vpn;
    /* after it was created, set MC for MP */

    /* 2. create multicast:
     *     - to be used for VPN flooding
     *     - has to be same id as vpn
     */
    mc_group = vpn;
    rv = multicast__open_mc_group(unit, &mc_group, BCM_MULTICAST_TYPE_L2);
    if (rv != BCM_E_NONE) {
        printf("Error, multicast__open_mc_group\n");
        return rv;
    }
    if(verbose1){
        printf("created multicast   0x%08x\n\r",vpn);
    }

    return rv;
}

/* Create a standard multi point mpls port both in ingress and egress */
int mpls_port__mp_create_ingress_egress_mpls_port(int unit, mpls_port_utils_s *mpls_port_properties){

    int rv = BCM_E_NONE;
    bcm_mpls_port_t mpls_port;
    bcm_mpls_port_t_init(&mpls_port);
    int index;
    int mpls_label_indexed_enable = soc_property_get(unit , "mpls_termination_label_index_enable",0);
    int mpls_termination_label_index_database_mode = soc_property_get(unit, spn_BCM886XX_MPLS_TERMINATION_DATABASE_MODE, 2);

    /* Set default flags */
    mpls_port.flags = BCM_MPLS_PORT_EGRESS_TUNNEL;
    mpls_port.flags |= mpls_port_properties->flags;
    mpls_port.flags2 = mpls_port_properties->flags2;

    /* Set (default or given) match criteria */
    if (!mpls_port_properties->ingress_matching_criteria) {
        mpls_port.criteria = BCM_MPLS_PORT_MATCH_LABEL;
    } else {
        mpls_port.criteria = mpls_port_properties->ingress_matching_criteria;
    }
    mpls_port.match_label = mpls_port_properties->ingress_pwe_label;

    /* Traps packet if TTL is 1 */
    if(mpls_port_properties->vccv_type) {
        mpls_port.vccv_type = bcmMplsPortControlChannelTtl;
    }

    /* Set orientation /network group id*/
    if (is_device_or_above(unit,JERICHO) && soc_property_get(unit , "split_horizon_forwarding_groups_mode",1)) {
        if (mpls_port_properties->network_group) {
            mpls_port.network_group_id = mpls_port_properties->network_group;
            mpls_port.flags |= is_device_or_above(unit, JERICHO2) ? 0 : BCM_MPLS_PORT_NETWORK;
        }
    }
    else {
        mpls_port.flags |= is_device_or_above(unit, JERICHO2) ? 0 : BCM_MPLS_PORT_NETWORK;
    }


    /* Set the mpls tunnel to which the pwe entry points two.
       It can be taken from two optional sources:
       1) If we learn the fec, the pointer to the egress mpls tunnel is taken from the fec
       2) Else, the pointer is taken directly from the given egress mpls tunnel id.
    */
    if (mpls_port_properties->fec) {
        mpls_port.egress_tunnel_if = mpls_port_properties->fec;
    } else {
        mpls_port.egress_tunnel_if = mpls_port_properties->egress_mpls_tunnel_id;
    }

    /* Set flags for egress_label configuration (given or default) */
    if(mpls_port_properties->egress_label_flags) {
        mpls_port.egress_label.flags = mpls_port_properties->egress_label_flags;
    } else {
        if (!is_device_or_above(unit, JERICHO2)) {
           mpls_port.egress_label.flags = BCM_MPLS_EGRESS_LABEL_TTL_SET;
        }
    }

    if (is_device_or_above(unit, JERICHO2)) {
        mpls_port.egress_label.egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
    }

    /* Set (given or default) ttl */
    if (!mpls_port_properties->ttl) {
        mpls_port.egress_label.ttl = 20;
    } else {
        mpls_port.egress_label.ttl = mpls_port_properties->ttl;
    }

    /* Set exp */
    mpls_port.egress_label.exp = mpls_port_properties->exp;

    /* Set the egress pwe label */
    mpls_port.egress_label.label = mpls_port_properties->egress_pwe_label;


    mpls_port.match_label = mpls_port_properties->ingress_pwe_label;
    /* We check if mpls_termination_label_index_enable is set, if so, we check if the label passed to the utility call has been idnexed already.
       If so, we do nothing, if not, we index it with index=2, as PWE labels are mostly looked at in SEM B */
    if (mpls_label_indexed_enable) {
        index = BCM_MPLS_INDEXED_LABEL_INDEX_GET(mpls_port.match_label);
        if (!index) {
            int label_val = mpls_port_properties->ingress_pwe_label;
            if ((mpls_termination_label_index_database_mode >= 20) && (mpls_termination_label_index_database_mode <= 21)) {
                /* PWE label is Application */
                BCM_MPLS_INDEXED_LABEL_SET(mpls_port.match_label, label_val, 3);
            }
            else
                BCM_MPLS_INDEXED_LABEL_SET(mpls_port.match_label, label_val, 2);
        }
    }

    /* Set the  port */
    mpls_port.port = mpls_port_properties->port;

    /* failover */
    if (mpls_port_properties->failover_id) {
        if (is_device_or_above(unit, JERICHO2)) {
            /** JR2 use ingress_* for 1+1 receiving selection in egress node*/
            mpls_port.ingress_failover_id          = mpls_port_properties->failover_id;
            mpls_port.ingress_failover_port_id     = mpls_port_properties->failover_port_id;
        } else {
            mpls_port.failover_id          = mpls_port_properties->failover_id;
            mpls_port.failover_port_id     = mpls_port_properties->failover_port_id;
        }

       mpls_port.failover_mc_group    = mpls_port_properties->failover_mc_group;
    }

    if(mpls_port_properties->mpls_port_id) {
        mpls_port.mpls_port_id = mpls_port_properties->mpls_port_id;
    }

    if (!is_device_or_above(unit, JERICHO2)) {
        rv = bcm_mpls_port_add(unit,mpls_port_properties->vpn,&mpls_port);
    } else {
        rv = sand_mpls_port_add(unit,mpls_port_properties->vpn,&mpls_port);
    }
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mpls_port_add\n");
        print rv;
        return rv;
    }

    mpls_port_properties->mpls_port_id = mpls_port.mpls_port_id;
    mpls_port_properties->encap_id = mpls_port.encap_id;


    if (mpls_label_indexed_enable && index != 1) {
        if (!((mpls_termination_label_index_database_mode >= 20) && (mpls_termination_label_index_database_mode <= 23))) {
          int label_val = mpls_port_properties->ingress_pwe_label;
          BCM_MPLS_INDEXED_LABEL_SET(mpls_port.match_label, label_val, 1);
          rv = bcm_mpls_port_add(unit,mpls_port_properties->vpn,&mpls_port);
          if (rv != BCM_E_NONE) {
            printf("Error, bcm_mpls_port_add\n");
            print rv;
            return rv;
          }
        }
    }


    return rv;
}

/*
 * Functions calling BCM apis
 */

/* set egress action over this l3 interface, so packet forwarded to this interface will be tunneled/swapped/popped */
int
mpls__create_tunnel_initiator__set(int unit, mpls__egress_tunnel_utils_s *mpls_tunnel_properties) {

    if (is_device_or_above(unit, JERICHO2)){
        /*JR2 does not support to create more than 2 labels one time*/
        nof_labels=2;
        /* In JR2 two labels in one eedb entry in mode of TTL_SET, label_1 ttl should be same with label_2 ttl*/
        ext_mpls_ttl = 20;
    } else {
        nof_labels=4;
    }
    bcm_mpls_egress_label_t label_array[nof_labels];
    int num_labels;
    int num_labels2;
    int rv;

    rv = mpls__init(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls__init\n");
        return rv;
    }

    bcm_mpls_egress_label_t_init(&label_array[0]);


    if(mpls_tunnel_properties->force_flags) {
        label_array[0].flags = mpls_tunnel_properties->flags;
    } else {
        label_array[0].flags = mpls_tunnel_properties->flags | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
        if (!is_device_or_above(unit, JERICHO2)) {
            label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_TTL_SET;
        }
    }
    if (is_device_or_above(unit, JERICHO2)) {
        label_array[0].egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
    }
    if (!is_device_or_above(unit,ARAD_PLUS) || mpls_pipe_mode_exp_set) {
        label_array[0].exp = mpls_tunnel_properties->with_exp ? mpls_tunnel_properties->exp : mpls_exp;
        if (is_device_or_above(unit, JERICHO2)) {
            label_array[0].egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
        } else {
            label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
        }
    } else {
        if (is_device_or_above(unit, JERICHO2)) {
            label_array[0].egress_qos_model.egress_qos = bcmQosEgressModelUniform;
        } else {
            label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
        }
    }
    /* In JR2 there is no need to specify header above PHP */
    if (!is_device_or_above(unit, JERICHO2) && (mpls_tunnel_properties->egress_action==BCM_MPLS_EGRESS_ACTION_PHP)) {
      label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_PHP_IPV4;
    }
    if (mpls_tunnel_properties->flags & BCM_MPLS_EGRESS_LABEL_ACTION_VALID) {
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_ACTION_VALID;
        label_array[0].action = mpls_tunnel_properties->egress_action;
    }
    label_array[0].label = mpls_tunnel_properties->label_in;
    label_array[0].ttl = mpls_tunnel_properties->ttl ? mpls_tunnel_properties->ttl : mpls_ttl;

    label_array[0].spl_label_push_type = mpls_tunnel_properties->spl_label_push_type;


    if (mpls_tunnel_properties->tunnel_id != 0) {
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_WITH_ID;
        label_array[0].tunnel_id = mpls_tunnel_properties->tunnel_id;
    }
    num_labels = 1;

    if (mpls_tunnel_properties->flags & BCM_MPLS_EGRESS_LABEL_PROTECTION) {
        label_array[0].egress_failover_id = mpls_tunnel_properties->egress_failover_id;
        label_array[0].egress_failover_if_id = mpls_tunnel_properties->egress_failover_if_id;
    }

    if (mpls_tunnel_properties->label_out != mpls_default_egress_label_value) {

        uint32 flags = 0;

        flags = mpls_tunnel_properties->flags_out;

        if (!mpls_tunnel_properties->force_flags_out) {
           unset_entropy_flag(unit, &flags);
        }
        init_outer_label(unit, &label_array[1], mpls_tunnel_properties, mpls_tunnel_properties->label_out,flags);
        num_labels++;

        if ((mpls_tunnel_properties->label_out_2 != mpls_default_egress_label_value) &&!is_device_or_above(unit, JERICHO2)){

            flags = mpls_tunnel_properties->flags_out_2 ;
            unset_entropy_flag(unit, &flags);
            init_outer_label(unit, &label_array[2], mpls_tunnel_properties, mpls_tunnel_properties->label_out_2,flags);
            num_labels++;

            if (mpls_tunnel_properties->label_out_3 != mpls_default_egress_label_value) {

                flags = mpls_tunnel_properties->flags_out_3;
                unset_entropy_flag(unit, &flags);
                init_outer_label(unit, &label_array[3], mpls_tunnel_properties, mpls_tunnel_properties->label_out_3,flags);
                num_labels++;
            }
        }
    }

    if ((mpls_tunnel_properties->label_out_2 != mpls_default_egress_label_value) && is_device_or_above(unit, JERICHO2)){
        uint32 flags = 0;
        bcm_mpls_egress_label_t label_array2[nof_labels];
        bcm_mpls_egress_label_t_init(&label_array2[0]);
        flags = mpls_tunnel_properties->flags_out_2 ;
        /*Encap stage need to be set explicitly in JR2 if more than 1 tunnel EEDB entries created*/
        mpls_tunnel_properties->encap_access=2;
        unset_entropy_flag(unit, &flags);
        init_outer_label(unit, &label_array2[0], mpls_tunnel_properties, mpls_tunnel_properties->label_out_2,flags);
        num_labels2 = 1;
        if (mpls_tunnel_properties->label_out_3 != mpls_default_egress_label_value) {
            flags = mpls_tunnel_properties->flags_out_3;
            unset_entropy_flag(unit, &flags);
            init_outer_label(unit, &label_array2[1], mpls_tunnel_properties, mpls_tunnel_properties->label_out_3,flags);
            num_labels2++;
        }

        rv = bcm_mpls_tunnel_initiator_create(unit,0,num_labels2,label_array2);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_mpls_tunnel_initiator_create\n");
            return rv;
        }

        mpls_tunnel_properties->tunnel_id = label_array2[0].tunnel_id;
        mpls_tunnel_properties->next_pointer_intf= label_array2[0].tunnel_id;
        mpls_tunnel_properties->encap_access=1;

    }

    label_array[0].encap_access = mpls_tunnel_properties->encap_access;
    label_array[0].l3_intf_id = mpls_tunnel_properties->next_pointer_intf;
    if (is_device_or_above(unit, JERICHO2)){
        /* in JR2 l3_intf_id and encap_access must be similar for all provided labels */
        label_array[1].l3_intf_id = label_array[0].l3_intf_id;
        label_array[1].encap_access = label_array[0].encap_access;
    }

    rv = bcm_mpls_tunnel_initiator_create(unit,0,num_labels,label_array);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_mpls_tunnel_initiator_create\n");
        return rv;
    }

    mpls_tunnel_properties->tunnel_id = label_array[0].tunnel_id;

    return rv;
}

/*
 * In Jericho when using split_horizon_forwarding_groups_mode=1, if a MPLS port has
 * the flag BCM_MPLS_PORT_NETWORK, it needs to have a forwarding group which is not 0.
 */
int mpls_port__update_network_group_id(int unit, bcm_mpls_port_t* mpls_port) {
    int rv = BCM_E_NONE;

    mpls_port->network_group_id = 0;

    if(mpls_port->flags & BCM_MPLS_PORT_NETWORK) {
        if (is_device_or_above(unit,JERICHO) && soc_property_get(unit, "split_horizon_forwarding_groups_mode", 1)) {
            mpls_port->network_group_id = 1;
        }
    }

    return rv;
}

/* Create a standard PWE In-LIF*/
int mpls_port__ingress_only_create(int unit, mpls_port__ingress_only_info_s *mpls_ingress_only) {

    int rv = BCM_E_NONE;
    int mpls_label_indexed_enable = soc_property_get(unit , "mpls_termination_label_index_enable",0);
    int mpls_termination_label_index_database_mode = soc_property_get(unit, spn_BCM886XX_MPLS_TERMINATION_DATABASE_MODE, 2);
    int is_jr2_mode = is_device_or_above(unit, JERICHO2) ? *(dnxc_data_get(unit,  "headers", "system_headers", "system_headers_mode", NULL)) : 0;
    int index;
    bcm_mpls_port_t mpls_port;

    bcm_mpls_port_t_init(&mpls_port);

    if(!mpls_ingress_only->vpn) {
        mpls_ingress_only->vpn = g_mpls_port_utils.vpn;
    }

    mpls_port.flags2 = mpls_ingress_only->flags2;
    /* Setting default flags for ingress only */
    mpls_port.flags2 |= BCM_MPLS_PORT2_INGRESS_ONLY;

    mpls_port.flags = mpls_ingress_only->flags;
    mpls_port.flags |= BCM_MPLS_PORT_EGRESS_TUNNEL;
    mpls_port.flags |= is_device_or_above(unit, JERICHO2) ? 0 : BCM_MPLS_PORT_NETWORK;

    mpls_port.criteria = mpls_ingress_only->ingress_matching_criteria;

    /* Set WITH_ID flag if mpls_port_id is given*/
    if(mpls_ingress_only->mpls_port_id) {
        mpls_port.flags |= BCM_MPLS_PORT_WITH_ID;
        mpls_port.mpls_port_id = mpls_ingress_only->mpls_port_id;
    }

    if(mpls_ingress_only->learn_egress_if) {
        mpls_port.egress_tunnel_if = mpls_ingress_only->learn_egress_if;
    }

    if(mpls_ingress_only->failover_id) {
        mpls_port.failover_id = mpls_ingress_only->failover_id;
    }

    if(mpls_ingress_only->failover_port_id) {
        mpls_port.failover_port_id = mpls_ingress_only->failover_port_id;
    }

    if(mpls_ingress_only->failover_mc_group) {
        mpls_port.failover_mc_group = mpls_ingress_only->failover_mc_group;
    }

    if(mpls_ingress_only->ingress_failover_id) {
        mpls_port.ingress_failover_id = mpls_ingress_only->ingress_failover_id;
    }

    if(mpls_ingress_only->encap_id) {
        mpls_port.encap_id = mpls_ingress_only->encap_id;
    }

    /* Check if GAL is enabled and GAL tunnel is passed */
    if(mpls_ingress_only->gal_primary_tunnel_id) {
        mpls_port.tunnel_id = mpls_ingress_only->gal_primary_tunnel_id;
    }

    mpls_port.port = mpls_ingress_only->port;

    /*Setting VC match label*/
    mpls_port.match_label = mpls_ingress_only->ingress_pwe_label;
    /* We check if mpls_termination_label_index_enable is set, if so, we check if the label passed to the utility call has been idnexed already.
       If so, we do nothing, if not, we index it with index=2, as PWE labels are mostly looked at in SEM B */
    if (mpls_label_indexed_enable) {
        index = BCM_MPLS_INDEXED_LABEL_INDEX_GET(mpls_port.match_label);
        if(!index) {
             int label_val = mpls_ingress_only->ingress_pwe_label;
            if ((mpls_termination_label_index_database_mode >= 20) && (mpls_termination_label_index_database_mode <= 21)) {
                /* PWE label is Application */
                BCM_MPLS_INDEXED_LABEL_SET(mpls_port.match_label, label_val, 3);
            } else {
                BCM_MPLS_INDEXED_LABEL_SET(mpls_port.match_label, label_val, 2);
            }
        }

    }

    /* Egress Learn Label. Can be added only if BCM_MPLS_PORT2_LEARN_ENCAP or BCM_MPLS_PORT2_LEARN_ENCAP_EEI is absent.
       In this case, we learn the encap id, no use fro VC label information. */
     if (!(mpls_ingress_only->flags2 & (BCM_MPLS_PORT2_LEARN_ENCAP | BCM_MPLS_PORT2_LEARN_ENCAP_EEI)) &&
         (!is_device_or_above(unit, JERICHO2) || !is_jr2_mode)) {
         /* Egress label is not used in case of ingress only.*/
         mpls_port.egress_label = mpls_ingress_only->learn_egress_label;

         if (is_device_or_above(unit, JERICHO2)) {
            mpls_port.egress_label.egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
         }
     }

     if(!mpls_ingress_only->network_group) {
         rv = mpls_port__update_network_group_id(unit, &mpls_port);
        if (rv != BCM_E_NONE) {
            printf("Error, Ingress mpls_port__update_network_group_id\n");
            return rv;
        }
    } else {

        if (is_device_or_above(unit,JERICHO) && soc_property_get(unit , "split_horizon_forwarding_groups_mode",1)) {
            mpls_port.network_group_id = mpls_ingress_only->network_group;
        }
    }

    rv = bcm_mpls_port_add(unit, mpls_ingress_only->vpn, &mpls_port);
    if(rv != BCM_E_NONE) {
        printf("Error, bcm_mpls_port_add INGRESS ONLY\n");
        print rv;
        return rv;
    }
    mpls_ingress_only->mpls_port_id = mpls_port.mpls_port_id;

    mpls_ingress_only->encap_id = mpls_port.encap_id;

    if (mpls_label_indexed_enable && (index != 1)) {
         int label_val = mpls_ingress_only->ingress_pwe_label;
         BCM_MPLS_INDEXED_LABEL_SET(mpls_port.match_label, label_val, 1);
         mpls_port.mpls_port_id = mpls_ingress_only->mpls_port_id + 1;
         rv = bcm_mpls_port_add(unit, mpls_ingress_only->vpn, &mpls_port);
        if(rv != BCM_E_NONE) {
            printf("Error, bcm_mpls_port_add INGRESS ONLY\n");
            print rv;
            return rv;
        }
    }

    return rv;

}

/*
 * When using VPLS application with forwarding two pointers (one for PWE and the second for MPLS):
 * In ARAD egress tunnel if must be valid since multicast case include one pointer only.
 * In Jericho next pointer must be null, in multicast case we use two pointers (one for PWE, second for MPLS).
 */
void
mpls_port__egress_tunnel_if_egress_only__set(int unit, int tunnel_id, int use_two_pointers, bcm_mpls_port_t *mpls_port) {

    if (use_two_pointers && is_device_or_above(unit, JERICHO)) {
        /* In Jericho next pointer is taken from the second pointer */
        mpls_port->egress_tunnel_if = 0;
    } else {
        mpls_port->egress_tunnel_if = tunnel_id;
    }
}

