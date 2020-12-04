/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~MPLS LSR~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_mpls_tunnel_initator.c
 * Purpose: Example of different paths using the mpls tunnel initator.
 *
 * Explanation:
 *    For incoming packet, the Ethernet header is terminated.
 *    Labels are swapped and forwarded to egress port with new label/labels and Ethernet encapsulations.
 *
 * Calling Sequence:
 *     Port TPIDs setting
 *         Assuming default TPID setting, that is, all ports configured with TPID 0x8100.
 *     Create MPLS L3 interface.
 *         Set interface with my-MAC 00:00:00:00:00:11 and VID 200.
 *         Packet tunneled/routed to this L3 interface will be set with this MAC.
 *         Calls bcm_l3_intf_create().
 *     Create MPLS Tunnel initator objects.
 *         Set MPLS tunnels on MPLS L3 interface.
 *         Calls mpls_tunnel_initiator_run_with_defaults().
 *     Create egress object points to the above MPLS tunnel interface.
 *         Packet routed to this egress-object will be forwarded to egress port and encapsulated tunnel and
 *          with Ethernet Header.
 *             DA: next-hop-DA.
 *             SA/VID: according to interface definition.
 *         Calls bcm_l3_egress_create().
 *    Add ILM (ingress label mapping) entry (refer to mpls_add_switch_entry).
 *         Maps incoming label (5000) to egress label (8000) and points to egress object created above.
 *         Flags.
 *             BCM_MPLS_SWITCH_OUTER_TTL|BCM_MPLS_SWITCH_OUTER_EXP: both have to be present or
 *              absent; indicates Pipe/uniform models.
 *             BCM_MPLS_SWITCH_TTL_DECREMENT has to be present.
 *             BCM_MPLS_SWITCH_ACTION_SWAP: for LSR functionality.
 *         Calls bcm_mpls_tunnel_switch_add().
 *
 * To Activate Above Settings Run:
 *  	  BCM> cint ../sand/utility/cint_sand_utils_global.c
 *  	  BCM> cint ../sand/utility/cint_sand_utils_mpls.c
 *      BCM> cint cint_qos.c
 *      BCM> cint utility/cint_utils_l3.c
 *      BCM> cint cint_mpls_lsr.c
 *      BCM> cint cint_mpls_tunnel_initiator.c
 *      BCM> cint
 *      cint> int rv;
 *      cint> rv = mpls_tunnel_initator_run_with_defaults(unit, second_unit, outP);
 *
 *
 * Remarks:
 *     (To run a two-device system call with second_unit >= 0.) Two device systems are currently not supported
 *
 * Script adjustment:
 *  You can adjust the following attributes of the application:
 *    tunnel_label1:   MPLS label A
 *    tunnel_label2:   MPLS labels B
 *    num_tunnel_label2:  number of MPLS labels B
 *    tunnel_label3:   MPLS label C
 */
struct mpls_tunnel_initiator_info_s {

    /* egress -attributes */
    int tunnel_label1; /* egress label A */
    int tunnel_label1_id;

    int tunnel_label2[4]; /* egress labels B<1,2,3,4> that could point to A */
    int tunnel_label2_ids[4];
    int num_tunnel_label2;

    int tunnel_label3; /* egress labels C that could point to (B1,A) */
    int tunnel_label3_id;

    int ingress_intf_for_replace;
};

mpls_tunnel_initiator_info_s mpls_tunnel_initiator_info_1;

int mpls_tunnel_initiator_ttl = 20;
int mpls_egress_tunnel_replace_label = 0;
int outlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE;
int outlif_to_count = 0;
int tunnel_initiator_swap_action = 0;
int fec_point_to_tunnel = 0;

void
mpls_tunnel_initiator_init(int tunnel_label1, int* tunnel_label2, int num_tunnel_label2, int tunnel_label3) {

    int index = 0;
    mpls_tunnel_initiator_info_1.tunnel_label1 = tunnel_label1;
    mpls_tunnel_initiator_info_1.tunnel_label1_id = 0;

    if (num_tunnel_label2 > 4) {
      printf("number of tunnels that are defined for label2 are at most 4 in this cint\n",num_tunnel_label2);
      num_tunnel_label2 = 4;
    }

    for (index = 0; index < num_tunnel_label2; index++) {
      mpls_tunnel_initiator_info_1.tunnel_label2[index] = tunnel_label2[index];
      mpls_tunnel_initiator_info_1.tunnel_label2_ids[index] = 0;
    }
    mpls_tunnel_initiator_info_1.num_tunnel_label2 = num_tunnel_label2;

    mpls_tunnel_initiator_info_1.tunnel_label3 = tunnel_label3;
    mpls_tunnel_initiator_info_1.tunnel_label3_id = 0;

}

/*
 *  Configuration to set WITH_ID on tunnel IDs.
 *  Assumed mpls_tunnel_initiator_init being called first
 */
void
mpls_tunnel_initiator_init_with_id(int tunnel_label1_id, int* tunnel_label2_id, int tunnel_label3_id) {
    int index = 0;
    mpls_tunnel_initiator_info_1.tunnel_label1_id = tunnel_label1_id;

    for (index = 0; index < mpls_tunnel_initiator_info_1.num_tunnel_label2; index++) {
      mpls_tunnel_initiator_info_1.tunnel_label2_ids[index] = tunnel_label2_id[index];
    }
    mpls_tunnel_initiator_info_1.tunnel_label3_id = tunnel_label3_id;

}


/* set tunnel over this l3 interface, so packet forwarded to this interface will be tunneled.
   Update L3 interface accordingly */
/* Example of Pipe model */
int
create_stack_of_tunnel_initiators(int unit, int* outgoing_intf) {
    bcm_mpls_egress_label_t label_array[4];
    int num_labels, tunnel_A, tunnelA_index, tunnelB_index, tunnelC_index, index;
    int rv;
    int ttl = 20;

    for (index = 0; index < 4; index++) {
        bcm_mpls_egress_label_t_init(&label_array[index]);
    }

    /*
     * Tunnel initator application is decided upon number of B labels
     * In case no B labels (num_tunnel_label2=0), just create one tunnel (A).
     * In case one B label (num_tunnel_label2=1), create a bind tunnels (to reserve space) (B,A)
     * In case more than one B labels (num_tunnel_label2 > 1), create one tunnel (A), and several tunnels B such that B1=>A, B2=>A, B3=>A..
     */
    tunnelA_index = (mpls_tunnel_initiator_info_1.num_tunnel_label2 != 1) ? 0:1;

    /* Tunnel A */
    label_array[tunnelA_index].flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    if (is_device_or_above(unit, JERICHO2)) {
        label_array[tunnelA_index].egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
    } else {
        label_array[tunnelA_index].flags |= BCM_MPLS_EGRESS_LABEL_TTL_SET;
    }
    if (!is_device_or_above(unit,ARAD_PLUS) || mpls_pipe_mode_exp_set) {
        label_array[tunnelA_index].exp = mpls_exp;
        if (is_device_or_above(unit, JERICHO2)) {
            label_array[tunnelA_index].egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
        } else {
            label_array[tunnelA_index].flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
        }
    } else {
        if (is_device_or_above(unit, JERICHO2)) {
            label_array[tunnelA_index].egress_qos_model.egress_qos = bcmQosEgressModelUniform;
        } else {
            label_array[tunnelA_index].flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
        }
    }
    label_array[tunnelA_index].label = mpls_tunnel_initiator_info_1.tunnel_label1;

    /* On JR2, the ttl value should be same when two labels in mode TTL_SET */
    if (!is_device_or_above(unit, JERICHO2)){
        label_array[tunnelA_index].ttl = ttl++;
    } else {
        label_array[tunnelA_index].ttl = ttl;
    }

    if (mpls_tunnel_initiator_info_1.tunnel_label1_id != 0) {
        label_array[tunnelA_index].flags |= BCM_MPLS_EGRESS_LABEL_WITH_ID;
        label_array[tunnelA_index].tunnel_id = mpls_tunnel_initiator_info_1.tunnel_label1_id;
    }
    label_array[tunnelA_index].l3_intf_id = *outgoing_intf; /* Point to L3 interface MPLS */
    label_array[tunnelA_index].outlif_counting_profile = outlif_counting_profile;
    num_labels = 1;

    /* Tunnel B is in bind entry with tunnel A only if number of tunnels that points to A is one */
    if (mpls_tunnel_initiator_info_1.num_tunnel_label2 == 1) {
        tunnelB_index = 0;

        label_array[tunnelB_index].flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
        if (is_device_or_above(unit, JERICHO2)) {
            label_array[tunnelB_index].egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
        } else {
            label_array[tunnelB_index].flags |= BCM_MPLS_EGRESS_LABEL_TTL_SET;
        }
        if (!is_device_or_above(unit,ARAD_PLUS) || mpls_pipe_mode_exp_set) {
            label_array[tunnelB_index].exp = mpls_exp;
            if (is_device_or_above(unit, JERICHO2)) {
                label_array[tunnelB_index].egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
            } else {
                label_array[tunnelB_index].flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
            }
        } else {
            if (is_device_or_above(unit, JERICHO2)) {
                label_array[tunnelB_index].egress_qos_model.egress_qos = bcmQosEgressModelUniform;
            } else {
                label_array[tunnelB_index].flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
            }
        }

        if (tunnel_initiator_swap_action == 1) {
            label_array[tunnelB_index].flags |= BCM_MPLS_EGRESS_ACTION_SWAP;
            label_array[tunnelB_index].flags |= is_device_or_above(unit, JERICHO2) ? 0 : BCM_MPLS_EGRESS_LABEL_ACTION_VALID;
        }

        label_array[tunnelB_index].label = mpls_tunnel_initiator_info_1.tunnel_label2[0];
        if (mpls_tunnel_initiator_info_1.tunnel_label2_ids[0] != 0) {
            label_array[tunnelB_index].flags |= BCM_MPLS_EGRESS_LABEL_WITH_ID;
            label_array[tunnelB_index].tunnel_id = mpls_tunnel_initiator_info_1.tunnel_label2_ids[0];
        }

        /* On JR2, the ttl value should be same when two labels in mode TTL_SET */
        if (!is_device_or_above(unit, JERICHO2)){
            label_array[tunnelB_index].ttl = ttl++;
        } else {
            label_array[tunnelB_index].ttl = ttl;
        }

        if (is_device_or_above(unit, JERICHO2)) {
            /*
            * in JR2 l3_intf_id must be similar for all provided labels
            * in this case: tunnelA_index = 1, tunnelB_index = 0
            */
            label_array[tunnelA_index].l3_intf_id = label_array[tunnelB_index].l3_intf_id;
        }

        label_array[tunnelB_index].outlif_counting_profile = outlif_counting_profile;
        label_array[tunnelB_index].qos_map_id = qos_map_id_mpls_egress_get(unit);
        label_array[tunnelA_index].qos_map_id = qos_map_id_mpls_egress_get(unit);
        num_labels = 2;
    }
    rv = bcm_mpls_tunnel_initiator_create(unit,0,num_labels,label_array);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_mpls_tunnel_initiator_create\n");
        return rv;
    }

    *outgoing_intf = label_array[0].tunnel_id; /* updated outgoing intf to be tunnel interface */
    mpls_tunnel_initiator_info_1.ingress_intf_for_replace = label_array[0].tunnel_id;

    /* In case there is more than one tunnel B that points to A, then create tunnel initiator for each such B */
    if (mpls_tunnel_initiator_info_1.num_tunnel_label2 > 1) {
        tunnel_A = *outgoing_intf;
        for (index = 0; index < mpls_tunnel_initiator_info_1.num_tunnel_label2; index++) {
            tunnelB_index = 0;
            bcm_mpls_egress_label_t_init(&label_array[tunnelB_index]);

            label_array[tunnelB_index].flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
            if (is_device_or_above(unit, JERICHO2)) {
                label_array[tunnelB_index].egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
            } else {
                label_array[tunnelB_index].flags |= BCM_MPLS_EGRESS_LABEL_TTL_SET;
            }
            if (!is_device_or_above(unit,ARAD_PLUS) || mpls_pipe_mode_exp_set) {
                label_array[tunnelB_index].exp = mpls_exp;
                if (is_device_or_above(unit, JERICHO2)) {
                    label_array[tunnelB_index].egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
                } else {
                    label_array[tunnelB_index].flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
                }
            } else {
                if (is_device_or_above(unit, JERICHO2)) {
                    label_array[tunnelB_index].egress_qos_model.egress_qos = bcmQosEgressModelUniform;
                } else {
                    label_array[tunnelB_index].flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
                }
            }

            label_array[tunnelB_index].label = mpls_tunnel_initiator_info_1.tunnel_label2[index];
            label_array[tunnelB_index].ttl = ttl++;
            if (mpls_tunnel_initiator_info_1.tunnel_label2_ids[index] != 0) {
                label_array[tunnelB_index].flags |= BCM_MPLS_EGRESS_LABEL_WITH_ID;
                label_array[tunnelB_index].tunnel_id = mpls_tunnel_initiator_info_1.tunnel_label2_ids[index];
            }
            label_array[tunnelB_index].l3_intf_id = tunnel_A; /* Point to Tunnel A */
            label_array[tunnelB_index].outlif_counting_profile = outlif_counting_profile;
            num_labels = 1;
            rv = bcm_mpls_tunnel_initiator_create(unit,0,num_labels,label_array);
            if (rv != BCM_E_NONE) {
                printf("Error, in bcm_mpls_tunnel_initiator_create\n");
                return rv;
            }

            outgoing_intf[index] = label_array[0].tunnel_id; /* updated outgoing intf to be last defined tunnel B interface */
        }
    }

    /* Extend example. Also have C or C,D  */
    if (mpls_tunnel_initiator_info_1.tunnel_label3 != 0) {

        /* In case there is C or C,D tunnels create also tunnel initiator for them */
        /* only used when (B,A) are bind toughter */
        if (mpls_tunnel_initiator_info_1.num_tunnel_label2 > 1 && mpls_tunnel_initiator_info_1.tunnel_label3 != 0) {
            printf("Cant set more 3 entries of tunnels");
            return BCM_E_PARAM;
        }


        tunnelC_index = 0;

        if (mpls_tunnel_initiator_info_1.tunnel_label3 != 0) { /* C */
            bcm_mpls_egress_label_t_init(&label_array[tunnelC_index]);
            label_array[tunnelC_index].exp = mpls_exp;
            label_array[tunnelC_index].flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
            if (is_device_or_above(unit, JERICHO2)) {
                label_array[tunnelC_index].egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
            } else {
                label_array[tunnelC_index].flags |= BCM_MPLS_EGRESS_LABEL_TTL_SET;
            }
            if (!is_device_or_above(unit,ARAD_PLUS) || mpls_pipe_mode_exp_set) {
                label_array[tunnelC_index].exp = mpls_exp;
                if (is_device_or_above(unit, JERICHO2)) {
                    label_array[tunnelC_index].egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
                } else {
                    label_array[tunnelC_index].flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
                }
            } else {
                if (is_device_or_above(unit, JERICHO2)) {
                    label_array[tunnelC_index].egress_qos_model.egress_qos = bcmQosEgressModelUniform;
                } else {
                    label_array[tunnelC_index].flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
                }
            }
            label_array[tunnelC_index].label = mpls_tunnel_initiator_info_1.tunnel_label3;
            label_array[tunnelC_index].ttl = ttl++;
            if (mpls_tunnel_initiator_info_1.tunnel_label3_id != 0) {
                label_array[tunnelC_index].flags |= BCM_MPLS_EGRESS_LABEL_WITH_ID;
                label_array[tunnelC_index].tunnel_id = mpls_tunnel_initiator_info_1.tunnel_label3_id;
            }
            label_array[tunnelC_index].l3_intf_id = *outgoing_intf; /* Point to Tunnel B */
            label_array[tunnelC_index].outlif_counting_profile = outlif_counting_profile;
            num_labels = 1;
        }

        rv = bcm_mpls_tunnel_initiator_create(unit,0,num_labels,label_array);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_mpls_tunnel_initiator_create\n");
            return rv;
        }

        *outgoing_intf = label_array[0].tunnel_id;
    }

    return rv;
}


/* to run a two-device system call with second_unit >= 0 */
int
mpls_tunnel_initiator_config(int unit, int second_unit){

    int CINT_NO_FLAGS = 0;
    int ingress_intf[4];
    int egress_intf;
    int encap_id;
    int rv;
    int num_labels;
    bcm_pbmp_t pbmp;
    int index, number_of_lsrs;
    create_l3_intf_s intf;

    number_of_lsrs = (mpls_tunnel_initiator_info_1.num_tunnel_label2 > 1) ? mpls_tunnel_initiator_info_1.num_tunnel_label2:1;

    /* Incoming L2 settings */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, mpls_lsr_info_1.in_port);

    rv = bcm_vlan_destroy(unit,mpls_lsr_info_1.in_vid);
    rv = bcm_vlan_create(unit,mpls_lsr_info_1.in_vid);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_create\n");
        return rv;
    }
    rv = bcm_vlan_port_add(unit, mpls_lsr_info_1.in_vid, pbmp, pbmp);
    if (BCM_FAILURE(rv)) {
        printf("BCM FAIL %d: %s\n", rv, bcm_errmsg(rv));
        return CMD_FAIL;
    }

    /* Outgoing L2 settings */
    if (second_unit >= 0)
    {
        rv = bcm_vlan_create(second_unit,mpls_lsr_info_1.eg_vid);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_create\n");
            return rv;
        }

        BCM_PBMP_CLEAR(pbmp);
        BCM_PBMP_PORT_ADD(pbmp, BCM_GPORT_MODPORT_PORT_GET(mpls_lsr_info_1.eg_port));

        rv = bcm_vlan_port_add(second_unit, mpls_lsr_info_1.eg_vid, pbmp, pbmp);
        if (BCM_FAILURE(rv)) {
            printf("BCM FAIL %d: %s\n", rv, bcm_errmsg(rv));
            return CMD_FAIL;
        }
    } else {
        rv = bcm_vlan_create(unit,mpls_lsr_info_1.eg_vid);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_create\n");
            return rv;
        }
        BCM_PBMP_CLEAR(pbmp);
        BCM_PBMP_PORT_ADD(pbmp, mpls_lsr_info_1.eg_port);

        rv = bcm_vlan_port_add(unit, mpls_lsr_info_1.eg_vid, pbmp, pbmp);
        if (BCM_FAILURE(rv)) {
            printf("BCM FAIL %d: %s\n", rv, bcm_errmsg(rv));
            return CMD_FAIL;
        }
    }
    /* Set In-Port default AC*/
    bcm_vlan_port_t vlan_port;
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.port = mpls_lsr_info_1.in_port;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vlan_port.vsi = mpls_lsr_info_1.in_vid;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;

    rv  = bcm_vlan_port_create(unit, vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_port_create\n");
        return rv;
    }

    /* Set ouot-Port default AC*/

    if (second_unit >= 0)
    {
        if (is_device_or_above(second_unit, JERICHO2))
        {
            bcm_vlan_port_t_init(&vlan_port);
            vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
            vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY;
            vlan_port.flags |= BCM_VLAN_PORT_DEFAULT;
            vlan_port.flags |= BCM_VLAN_PORT_VLAN_TRANSLATION;
            rv = bcm_vlan_port_create(second_unit , vlan_port);
            if (rv != BCM_E_NONE)
            {
                printf("Error, bcm_vlan_port_create vlan_port\n");
                return rv;
            }

            bcm_port_match_info_t match_info;
            bcm_port_match_info_t_init(&match_info);
            match_info.match = BCM_PORT_MATCH_PORT;
            match_info.flags = BCM_PORT_MATCH_EGRESS_ONLY;
            match_info.port = mpls_lsr_info_1.eg_port;

            rv = bcm_port_match_add(second_unit , vlan_port.vlan_port_id, &match_info);
            if (rv != 0)
            {
                printf("in bcm_port_match_add \n");
                return rv;
            }
        }
    } else {
        if (is_device_or_above(unit, JERICHO2))
        {

            bcm_vlan_port_t_init(&vlan_port);
            vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
            vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY;
            vlan_port.flags |= BCM_VLAN_PORT_DEFAULT;
            vlan_port.flags |= BCM_VLAN_PORT_VLAN_TRANSLATION;

            rv = bcm_vlan_port_create(unit, vlan_port);
            if (rv != BCM_E_NONE)
            {
                printf("Error, bcm_vlan_port_create vlan_port\n");
                return rv;
            }

            bcm_port_match_info_t match_info;
            bcm_port_match_info_t_init(&match_info);
            match_info.match = BCM_PORT_MATCH_PORT;
            match_info.flags = BCM_PORT_MATCH_EGRESS_ONLY;
            match_info.port = mpls_lsr_info_1.eg_port;

            rv = bcm_port_match_add(unit, vlan_port.vlan_port_id, &match_info);
            if (rv != 0)
            {
                printf("in bcm_port_match_add \n");
                return rv;
            }

        }
    }

    /* L3 interface for incoming mpls rounting */
    intf.vsi = mpls_lsr_info_1.in_vid;
    intf.my_global_mac = mpls_lsr_info_1.my_mac;
    intf.my_lsb_mac = mpls_lsr_info_1.my_mac;
    intf.skip_mymac = mpls_lsr_info_1.use_multiple_mymac;

    rv = l3__intf_rif__create(unit, &intf);
    ingress_intf[0] = intf.rif;
    if (rv != BCM_E_NONE) {
    	printf("Error, in l3__intf_rif__create\n");
    	return rv;
    }

    /* L3 interface for outgoing mpls routing */
    intf.vsi = mpls_lsr_info_1.eg_vid;
    if (second_unit >= 0) {
    	rv = l3__intf_rif__create(second_unit, &intf);
    	ingress_intf[0] = intf.rif;
    } else {
        rv = l3__intf_rif__create(unit, &intf);
    	ingress_intf[0] = intf.rif;
    }
    if (rv != BCM_E_NONE) {
    	printf("Error, in l3__intf_rif__create\n");
    	return rv;
    }

    /* Egress Tunnel initator settings */
    if (second_unit >= 0) {
        rv = create_stack_of_tunnel_initiators(second_unit, ingress_intf);
        if (rv != BCM_E_NONE) {
           printf("Error, in create_stack_of_tunnel_initiators\n");
           return rv;
        }
    } else {
        rv = create_stack_of_tunnel_initiators(unit, ingress_intf);
        if (rv != BCM_E_NONE) {
           printf("Error, in create_stack_of_tunnel_initiators\n");
           return rv;
        }
    }

    BCM_GPORT_TUNNEL_ID_SET(outlif_to_count, ingress_intf[0]);
    for (index = 0; index < number_of_lsrs; index++) {
        /* Egress object settings */
        if (second_unit >= 0) {
            /* set Flag to indicate not to allocate or update FEC entry */
            create_l3_egress_s l3eg2;
            sal_memcpy(l3eg2.next_hop_mac_addr, mpls_lsr_info_1.next_hop_mac , 6);
            l3eg2.out_tunnel_or_rif = ingress_intf[index];
            l3eg2.out_gport =  mpls_lsr_info_1.eg_port;
            l3eg2.vlan =  mpls_lsr_info_1.eg_vid;
            l3eg2.arp_encap_id = encap_id;

            rv = l3__egress_only_encap__create(second_unit,&l3eg2);
            if (rv != BCM_E_NONE) {
                printf("Error, l3__egress_only_encap__create\n");
                return rv;
            }

            encap_id = l3eg2.arp_encap_id;
        }

        /* Allocate number of Egress objects (FEC) according to Number of Outgoing Tunnels */
        /* First allocate ARP (and point tunnel to ARP) */
        create_l3_egress_s l3_egress;
        l3_egress.out_gport = mpls_lsr_info_1.eg_port;
        l3_egress.vlan = mpls_lsr_info_1.eg_vid;
        l3_egress.next_hop_mac_addr = mpls_lsr_info_1.next_hop_mac;
        l3_egress.out_tunnel_or_rif = ingress_intf[index];
        l3_egress.arp_encap_id = encap_id;
        l3_egress.fec_id = egress_intf;
        l3_egress.qos_map_id = qos_map_id_mpls_egress_get(unit);
        rv = l3__egress_only_encap__create(unit, &l3_egress);
        if (rv != BCM_E_NONE) {
            printf("Error, in l3__egress_only_encap__create\n");
            return rv;
        }

        /* Second allocate FEC.*/
        /* FEC must be format B when ILM is used. Tunnel is pointed from FEC */
        encap_id = l3_egress.arp_encap_id;
        l3_egress.out_tunnel_or_rif = ingress_intf[index];
        l3_egress.arp_encap_id = 0;
        rv = l3__egress_only_fec__create(unit, &l3_egress);
        if (rv != BCM_E_NONE) {
            printf("Error, in l3__egress_only_fec__create\n");
            return rv;
        }

        egress_intf = l3_egress.fec_id;
        if (mpls_egress_tunnel_replace_label) {
            rv = mpls_egress_tunnel_replace_label(unit);
            if (rv != BCM_E_NONE) {
                printf("Error, mpls_egress_tunnel_replace_label\n");
                return rv;
            }
        }

        /* add switch entry to perform LSR */
        /* swap, in-label, with egress label, and point to egress object */
        rv = mpls_add_switch_entry(unit,mpls_lsr_info_1.in_label++, mpls_lsr_info_1.eg_label++, egress_intf);
        if (rv != BCM_E_NONE) {
            printf("Error, in mpls_add_switch_entry\n");
            return rv;
        }
    }

    return rv;
}

int
mpls_tunnel_initiator_run_with_defaults_aux(int unit, int second_unit, int in_port, int out_port, int pipe_mode_exp, int expected){

    mpls_pipe_mode_exp_set = pipe_mode_exp;

    mpls_exp = expected;
	ext_mpls_exp = expected;

    return mpls_tunnel_initiator_run_with_defaults(unit,second_unit,in_port,out_port);
}

/*
 *  Default Example, Set MPLS LSR to Outgoing Tunnels B (1001) ,A (1000)
 *  Send Packet:
 *  DA = {0x00, 0x00, 0x00, 0x00, 0x00, 0x33}
 *  VID = 200
 *  MPLS label 6000
 *  Expected Packet:
 *  DA = {0x00, 0x00, 0x00, 0x00, 0x00, 0x44}
 *  SA = {0x00, 0x00, 0x00, 0x00, 0x00, 0x33}
 *  MPLS label 9000, MPLS label 1001, MPLS label 1000
 */
int
mpls_tunnel_initiator_run_with_defaults(int unit, int second_unit, int in_port, int out_port){
    int tunnel_label2[1] = {1001};
    int rv = BCM_E_NONE;

    rv = mpls__mpls_pipe_mode_exp_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls__mpls_pipe_mode_exp_set\n");
        return rv;
    }

    mpls_lsr_init(in_port, out_port, 0x33, 0x44, 6000, 9000, 200,400,0);
    mpls_tunnel_initiator_init(1000, tunnel_label2, 1, 0);
    return mpls_tunnel_initiator_config(unit,second_unit);
}

/* Example of Multiple Tunnels B. In this example we have two tunnel Bs (1001,1002) that point to tunnel A (1000)
 * We create here two swap labels where first swap points out to Tunnel B1, and second swap points to tunnel B2
 *
 *  Send Packet 1:
 *  DA = {0x00, 0x00, 0x00, 0x00, 0x00, 0x33}
 *  VID = 200
 *  MPLS label 6000
 *  Expected Packet:
 *  DA = {0x00, 0x00, 0x00, 0x00, 0x00, 0x44}
 *  SA = {0x00, 0x00, 0x00, 0x00, 0x00, 0x33}
 *  MPLS label 9000, MPLS label 1001, MPLS label 1000
 *
 *  Send Packet 2:
 *  DA = {0x00, 0x00, 0x00, 0x00, 0x00, 0x33}
 *  VID = 200
 *  MPLS label 6001
 *  Expected Packet 2:
 *  DA = {0x00, 0x00, 0x00, 0x00, 0x00, 0x44}
 *  SA = {0x00, 0x00, 0x00, 0x00, 0x00, 0x33}
 *  MPLS label 9001, MPLS label 1002, MPLS label 1000
 */
int
mpls_tunnel_initiator_example_of_multiple_B(int unit, int second_unit, int in_port, int out_port){
    int tunnel_label2[2] = {1001,1002};
    int rv;

    rv = mpls__mpls_pipe_mode_exp_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls__mpls_pipe_mode_exp_set\n");
        return rv;
    }

    mpls_lsr_init(in_port, out_port, 0x33, 0x44, 6000, 9000, 200,400,0);
    mpls_tunnel_initiator_init(1000, tunnel_label2, 2, 0);
    rv = mpls_tunnel_initiator_config(unit,second_unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls_tunnel_initiator_config\n");
        return rv;
    }

    return rv;
}

/* Example of Multiple Tunnels B. In this example we have two tunnel Bs (1010,1020) that point to tunnel A (1005) */
/* We create here two swap labels where first swap points out to Tunnel B1, and second swap points to tunnel B2*/
int
mpls_tunnel_initiator_example_of_multiple_B_with_ID(int unit, int second_unit, int in_port, int out_port){
    int tunnel_label2[2] = {1001,1002};
    int tunnel_label2_ids[2] = {12288, 12290};
    int tunnel_label1_id = 8192;
    int rv;

    rv = mpls__mpls_pipe_mode_exp_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls__mpls_pipe_mode_exp_set\n");
        return rv;
    }

    mpls_lsr_init(in_port, out_port, 0x33, 0x44, 6000, 9000, 200,400,0);
    mpls_tunnel_initiator_init(1000, tunnel_label2, 2, 0);
    mpls_tunnel_initiator_init_with_id(tunnel_label1_id, tunnel_label2_ids, 0);
    rv = mpls_tunnel_initiator_config(unit,second_unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls_tunnel_initiator_config\n");
        return rv;
    }

    return rv;
}

/* Runs mpls_tunnel_switch_push_example. In case we wish to perform egress tunnel push,
   we should pass an eedb index to mpls_tunnel_switch_push_example (this is the allocated egress tunnel)
   In this case, we pass an eedb index that is extracted from cint_vswitch_cross_connect_p2p.c */
int run_tunnel_switch_push_example(int unit, int second_unit, int in_port, int out_port, int is_ingress_push){

    int rv = BCM_E_NONE, eedb_index = 0;

    if (!is_ingress_push) {
        eedb_index = get_vswitch_p2p_encap_id();
        printf("eedb_index : %d\n\n", eedb_index);
    }

    rv = mpls_tunnel_switch_push_example(unit, second_unit, in_port, out_port, is_ingress_push, eedb_index);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls_tunnel_switch_push_example\n");
        return rv;
    }


    return rv;
}

/* Add switch entry to perform PUSH
 * Push egress label.
 * Point to egress-object: egress_intf, returned by create_l3_egress
 * Push can be done in ingress (set is_ingress_push = 1) or egress (set set is_ingress_push = 0)
 */
int
mpls_tunnel_switch_push_entry(
    int unit,
    int in_label,
    int eg_label,
    bcm_if_t egress_intf,
    int is_ingress_push)
{
    int rv;
    bcm_mpls_tunnel_switch_t entry;

    bcm_mpls_tunnel_switch_t_init(&entry);

    if (is_ingress_push)
    {   /* push is done in ingress */
        entry.action = BCM_MPLS_SWITCH_EGRESS_ACTION_PUSH;

        entry.flags = 0;

        /*
         * PIPE: Set TTL and EXP,
         * in general valid options:
         * both present (uniform) or none of them (Pipe)
         */
        entry.egress_label.flags |= (BCM_MPLS_EGRESS_LABEL_TTL_SET | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT);
        if (!is_device_or_above(unit, ARAD_PLUS) || mpls_pipe_mode_exp_set)
        {
            entry.egress_label.flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
        }
        else
        {
            entry.egress_label.flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
        }
        entry.egress_label.ttl = 20;
        entry.egress_label.exp = mpls_exp;

        /*
         * egress attributes
         */
        entry.egress_label.label = eg_label;
    }
    else
    {   /* push is done in egress */
        entry.action = BCM_MPLS_SWITCH_ACTION_NOP;
        entry.flags = BCM_MPLS_SWITCH_TTL_DECREMENT;
    }

    /*
     * incoming label 
     */
    entry.label = in_label;
    entry.egress_if = egress_intf;

    rv = bcm_mpls_tunnel_switch_create(unit, &entry);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_tunnel_switch_create\n");
        return rv;
    }
    return rv;
}

/*
 * Example of MPLS tunnel action from the Tunnel Switch LSR.
 * push can be performed either in:
 * 1) ingress (set is_ingress_push = 1). In this case, tunnel switch will result with action PUSH.
 * or :
 * 2) egress (set is_ingress_push = 0 and pass an eedb_index according to which the label entry will be allocated).
 * In this case,  tunnel switch will result with action NOP. PUSH will be done in egress.
 * If PUSH action is done at the ingress we don't use additional resources of label from EEDB.
 * Traffic example:
 * Send traffic from incoming port
 * ETH: DA 0x33
 * MPLS: Incoming Label 6123
 * Capture from the outoging port
 * ETH: DA 0x44, SA 0x33
 * MPLS: Labels:
 * Label 1: 6123 TTL not decrement,
 * Label 2: 7000 EXP 2 TTL 20
 */
int
mpls_tunnel_switch_push_example(int unit, int second_unit, int in_port, int out_port, int is_ingress_push, int eedb_index) {
    int CINT_NO_FLAGS = 0;
    int ingress_intf;
    int egress_intf;
    int encap_id;
    int rv;
    int num_labels;
    bcm_pbmp_t pbmp;
    create_l3_intf_s intf;
    create_l3_egress_s l3_egress;

    rv = mpls__mpls_pipe_mode_exp_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls__mpls_pipe_mode_exp_set\n");
        return rv;
    }

    mpls_lsr_init(in_port, out_port, 0x33, 0x44, 6123, 7000, 300,500,0);

    /* Incoming L2 settings */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, mpls_lsr_info_1.in_port);

    rv = bcm_vlan_destroy(unit,mpls_lsr_info_1.in_vid);
    rv = bcm_vlan_create(unit,mpls_lsr_info_1.in_vid);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_create\n");
        return rv;
    }
    rv = bcm_vlan_port_add(unit, mpls_lsr_info_1.in_vid, pbmp, pbmp);
    if (BCM_FAILURE(rv)) {
        printf("BCM FAIL %d: %s\n", rv, bcm_errmsg(rv));
        return CMD_FAIL;
    }

    /* Outgoing L2 settings */
    if (second_unit >= 0)
    {
        rv = bcm_vlan_create(second_unit,mpls_lsr_info_1.eg_vid);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_create\n");
            return rv;
        }

        BCM_PBMP_CLEAR(pbmp);
        BCM_PBMP_PORT_ADD(pbmp, BCM_GPORT_MODPORT_PORT_GET(mpls_lsr_info_1.eg_port));

        rv = bcm_vlan_port_add(second_unit, mpls_lsr_info_1.eg_vid, pbmp, pbmp);
        if (BCM_FAILURE(rv)) {
            printf("BCM FAIL %d: %s\n", rv, bcm_errmsg(rv));
            return CMD_FAIL;
        }
    } else {
        rv = bcm_vlan_create(unit,mpls_lsr_info_1.eg_vid);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_create\n");
            return rv;
        }
        BCM_PBMP_CLEAR(pbmp);
        BCM_PBMP_PORT_ADD(pbmp, mpls_lsr_info_1.eg_port);

        rv = bcm_vlan_port_add(unit, mpls_lsr_info_1.eg_vid, pbmp, pbmp);
        if (BCM_FAILURE(rv)) {
            printf("BCM FAIL %d: %s\n", rv, bcm_errmsg(rv));
            return CMD_FAIL;
        }

        if (!is_device_or_above(unit, JERICHO2)) {
            rv = bcm_vlan_gport_add(unit, mpls_lsr_info_1.eg_vid, mpls_lsr_info_1.eg_port, BCM_VLAN_GPORT_ADD_MEMBER_DO_NOT_UPDATE|BCM_VLAN_GPORT_ADD_UNKNOWN_UCAST_DO_NOT_ADD);
            if (rv != BCM_E_NONE) {
                printf("Error, in bcm_vlan_gport_add\n");
                return rv;
            }
        } else {
            rv = bcm_vlan_gport_add(unit, mpls_lsr_info_1.eg_vid, mpls_lsr_info_1.eg_port, BCM_VLAN_GPORT_ADD_MEMBER_DO_NOT_UPDATE);
            if (rv != BCM_E_NONE) {
                printf("Error, in bcm_vlan_gport_add\n");
                return rv;
            }
        }
    }

    /* L3 interface for incoming mpls rounting */
    intf.vsi = mpls_lsr_info_1.in_vid;
    intf.my_global_mac = mpls_lsr_info_1.my_mac;
    intf.my_lsb_mac = mpls_lsr_info_1.my_mac;
    intf.skip_mymac = mpls_lsr_info_1.use_multiple_mymac;
    rv = l3__intf_rif__create(unit, &intf);
    ingress_intf = intf.rif;
    if (rv != BCM_E_NONE) {
    	printf("Error, in l3__intf_rif__create\n");
    	return rv;
    }

    /* L3 interface for outgoing mpls routing */
    intf.vsi = mpls_lsr_info_1.eg_vid;
    if (second_unit >= 0) {
    	rv = l3__intf_rif__create(second_unit, &intf);
    	ingress_intf = intf.rif;
    } else {
        rv = l3__intf_rif__create(unit, &intf);
    	ingress_intf = intf.rif;
    }
    if (rv != BCM_E_NONE) {
    	printf("Error, in l3__intf_rif__create\n");
    	return rv;
    }

    /* Allocate Egress object (FEC) */
    /* First allocate ARP (and point tunnel to ARP) */
    l3_egress.out_gport = mpls_lsr_info_1.eg_port;
    l3_egress.vlan = mpls_lsr_info_1.eg_vid;
    l3_egress.next_hop_mac_addr = mpls_lsr_info_1.next_hop_mac;
    l3_egress.out_tunnel_or_rif = ingress_intf;
    l3_egress.arp_encap_id = 0; /* will be created by API */
    l3_egress.qos_map_id = qos_map_id_mpls_egress_get(unit);
    rv = l3__egress_only_encap__create(unit, &l3_egress);
    if (rv != BCM_E_NONE) {
        printf("Error, in l3__egress_only_encap__create\n");
        return rv;
    }


    /* Egress Tunnel PUSH settings */
    /* Egress object settings */
    /* In this case, we perform egress tunnel push, and convert the eedb index to encap format*/
    if (!is_ingress_push) {
        ingress_intf = l3_egress.arp_encap_id;
        rv = mpls_egress_tunnel_config(unit, eedb_index, 7000, ingress_intf);
        if (rv != BCM_E_NONE) {
            printf("Error, in mpls_egress_tunnel_config\n");
            return rv;
        }
        ingress_intf = eedb_index | (1<<30);
    }

    if (is_device_or_above(unit, JERICHO2)) {
        l3_egress.arp_encap_id = 0;
    }

    /* Second allocate FEC.*/
    /* FEC must be format B when ILM is used. ARP is pointed from FEC */
    l3_egress.out_tunnel_or_rif = fec_point_to_tunnel ? ingress_intf : 0;
    rv = l3__egress_only_fec__create(unit, &l3_egress);
    if (rv != BCM_E_NONE) {
        printf("Error, in l3__egress_only_fec__create\n");
        return rv;
    }

    egress_intf = l3_egress.fec_id;

    /* add switch entry to perform LSR */
    /* PUSH, egress label, and point to egress object */
    rv = mpls_tunnel_switch_push_entry(unit,mpls_lsr_info_1.in_label, mpls_lsr_info_1.eg_label, egress_intf, is_ingress_push);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls_add_switch_entry\n");
        return rv;
    }
    return rv;
}

int mpls_egress_tunnel_config(
    int unit,
    int encap_id ,
    int received_label,
    int ingress_intf) {
    int rv;
    int mpls_termination_label_index_enable;
    bcm_mpls_egress_label_t label_array[1];

    bcm_mpls_egress_label_t_init(&label_array[0]);


    label_array[0].exp = 0; /* Set it statically 0*/
    label_array[0].flags = (BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT|BCM_MPLS_EGRESS_LABEL_WITH_ID);
    if (is_device_or_above(unit, JERICHO2)) {
        label_array[0].egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
    } else {
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_TTL_SET;
        label_array[0].action = BCM_MPLS_EGRESS_ACTION_PUSH;
    }
    if (!is_device_or_above(unit,ARAD_PLUS) || mpls_pipe_mode_exp_set) {
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
    label_array[0].label = received_label;
    label_array[0].ttl = mpls_tunnel_initiator_ttl;
    if (encap_id != 0) {
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_WITH_ID;
        label_array[0].tunnel_id = encap_id;
    }
    label_array[0].l3_intf_id = ingress_intf;
    label_array[0].outlif_counting_profile = outlif_counting_profile;
    rv = bcm_mpls_tunnel_initiator_create(unit,0,1,label_array);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_mpls_tunnel_initiator_create\n");
        print rv;
        return rv;
    }

    return rv;
}

/* REPLACE LABEL */
int mpls_egress_tunnel_replace_label(int unit)
{
    int rv;
    bcm_mpls_egress_label_t label_array[2];
    int label_count_temp;

    rv =  bcm_mpls_tunnel_initiator_get(unit, mpls_tunnel_initiator_info_1.ingress_intf_for_replace, 2, &label_array, &label_count_temp);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_mpls_tunnel_initiator_create\n");
        print rv;
        return rv;
    }
    label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_REPLACE | BCM_MPLS_EGRESS_LABEL_WITH_ID;
    label_array[0].label = 2002;
    label_array[1].flags |= BCM_MPLS_EGRESS_LABEL_REPLACE | BCM_MPLS_EGRESS_LABEL_WITH_ID;
    label_array[1].label = 3002;
    rv = bcm_mpls_tunnel_initiator_create(unit, 0, 2, &label_array);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_mpls_tunnel_initiator_create\n");
        print rv;
        return rv;
    }

    return rv;
}

/*
 * Secenario:
 * 1). MPLS tunnel encapsulation with 2 tunnel labels in the same encap entry
 * 2). MPLS tunnel encapsulation with 2 tunnel labels in different encap entry
 *
 * Configuration:
 *
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../../../src/examples/sand/cint_mpls_encapsulation_basic.c
 * cint ../../../../src/examples/sand/cint_sand_mpls_tunnel_initiator.c
 * cint
 * int unit = 0;
 * int rv = 0;
 * int in_port = 200;
 * int out_port = 201;
 * rv = mpls_tunnel_initiator_with_2_labels_main(unit,in_port,out_port);
 * print rv;
 */
int
mpls_encapsulation_simple_tunnels_create(
    int unit,
    int *labels,
    int nof_labels,
    int is_same_qos_mode)
 {
    bcm_mpls_egress_label_t label_array[4];
    int label_index, num_labels;
    int egress_qos_profile;
    int ttl = 10, exp = 5;
    int rv;

    num_labels = nof_labels;

    if (num_labels > 4)
    {
        printf("Error, labels in label_array should be less than 4!\n");
        return rv;
    }

    /** Init the label array.*/
    for (label_index = 0; label_index < num_labels; label_index++)
    {
        bcm_mpls_egress_label_t_init(&label_array[label_index]);

        /** Labels with the biggest member index*/
        label_array[label_index].label = labels[label_index];
        label_array[label_index].ttl = ttl;
        label_array[label_index].exp = exp;

        label_array[label_index].action = BCM_MPLS_EGRESS_ACTION_PUSH;
        label_array[label_index].action = BCM_MPLS_EGRESS_ACTION_PUSH;
        label_array[label_index].action = BCM_MPLS_EGRESS_ACTION_PUSH;

         label_array[label_index].l3_intf_id = cint_mpls_encapsulation_basic_info.arp_id_push;

        if (is_same_qos_mode == 0)
        {
            if (label_index % 2) {
                if (is_device_or_above(unit, JERICHO2)) {
                    label_array[label_index].egress_qos_model.egress_qos = bcmQosEgressModelUniform;
                    label_array[label_index].egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
                } else {
                    label_array[label_index].flags |= BCM_MPLS_EGRESS_LABEL_TTL_SET | BCM_MPLS_EGRESS_LABEL_EXP_COPY;
                }
                label_array[label_index].exp = 0;
            } else {
                if (is_device_or_above(unit, JERICHO2)) {
                    label_array[label_index].egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
                    label_array[label_index].egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
                } else {
                    label_array[label_index].flags |= BCM_MPLS_EGRESS_LABEL_TTL_COPY | BCM_MPLS_EGRESS_LABEL_EXP_SET;
                }
                label_array[label_index].ttl = 0;
            }
        }
        else
        {
            if (is_device_or_above(unit, JERICHO2)) {
                label_array[label_index].egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
                label_array[label_index].egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
            } else {
                label_array[label_index].flags |= BCM_MPLS_EGRESS_LABEL_TTL_SET;
                label_array[label_index].flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
            }
        }
    }

    /** Create the mpls tunnel with one call.*/
    rv = sand_mpls_tunnel_initiator_create(unit, 0, num_labels, label_array);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_tunnel_initiator_create\n");
        return rv;
    }

    cint_mpls_encapsulation_basic_info.push_tunnel_id = label_array[0].tunnel_id;

    return rv;
}


/*
 * Two MPLS tunnel labels are encapsulated with the same EEDB entry or two
 * different EEDB entries. This depends on the Qos and protection parameters.
 */
int
mpls_tunnel_initiator_with_2_labels_main(
    int unit,
    int port1,
    int port2)
{
    int rv = BCM_E_NONE;
    int labels1[2] = {2000, 1000};
    int labels2[2] = {2001, 1001};
    int tunnel_id1, tunnel_id2;
    uint32 dip1 = 0xA0A1A1A3, dip1_mask = 0xFFFFFFF0;
    uint32 dip2 = 0xA1A1A1A3, dip2_mask = 0xFFFFFFF0;
    int fec_id_enc1 = 0x2000c380, fec_id_enc2 = 0x2000c381;
    int nof_labels = 2;


    if (is_device_or_above(unit, JERICHO2))
    {
        int first_fec_in_hier;
        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &first_fec_in_hier);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }
        fec_id_enc1 = first_fec_in_hier++;
        fec_id_enc2 = first_fec_in_hier++;
    }

    rv = mpls_encapsulation_basic_check_support(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in mpls_encapsulation_basic_check_support\n", rv);
        return rv;
    }

    cint_mpls_encapsulation_basic_info.in_port = port1;
    cint_mpls_encapsulation_basic_info.out_port = port2;

    /*
     * Configure port properties for this application
     */
    rv = mpls_encapsulation_basic_configure_port_properties(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_basic_configure_port_properties\n");
        return rv;
    }

    /*
     * Configure L3 interfaces
     */
    rv = mpls_encapsulation_basic_create_l3_interfaces(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_basic_create_l3_interfaces\n");
         return rv;
    }

    /*
     * Configure an ARP entry
     */
    rv = mpls_encapsulation_basic_create_arp_entry(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_basic_create_arp_entry\n");
        return rv;
    }

    /*
     * Create two mpls encap labels with the same qos parameters.
     * They should reside in the same encap entry (two-labels-entry).
     */
    rv = mpls_encapsulation_simple_tunnels_create(unit, labels1, nof_labels, 1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_encapsulation_basic_create_tunnels\n");
        return rv;
    }
    tunnel_id1 = cint_mpls_encapsulation_basic_info.push_tunnel_id;

    /*
     * Create two mpls encap labels with different qos parameters.
     * They should reside in different encap entries (one-labels-entry).
     */
    rv = mpls_encapsulation_simple_tunnels_create(unit, labels2, nof_labels, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_encapsulation_basic_create_tunnels\n");
        return rv;
    }
    tunnel_id2 = cint_mpls_encapsulation_basic_info.push_tunnel_id;

    /*
     * Configure fec entry for two-labels-entry encapsulation flow
     */
    cint_mpls_encapsulation_basic_info.push_tunnel_id = tunnel_id1;
    cint_mpls_encapsulation_basic_info.fec_id_enc = fec_id_enc1;
    rv = mpls_encapsulation_basic_create_fec_entry_enc(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_encapsulation_basic_create_fec_entry_push\n");
        return rv;
    }

    /*
     * Configure fec entry for one-label-entry (two entries) encapsulation flow
     */
    cint_mpls_encapsulation_basic_info.push_tunnel_id = tunnel_id2;
    cint_mpls_encapsulation_basic_info.fec_id_enc = fec_id_enc2;
    rv = mpls_encapsulation_basic_create_fec_entry_enc(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_encapsulation_basic_create_fec_entry_push\n");
        return rv;
     }

    /*
     * Create l3 forwarding entry for the pushed label: two-labels-entry
     */
    cint_mpls_encapsulation_basic_info.fec_id_enc = fec_id_enc1;
    cint_mpls_encapsulation_basic_info.dip = dip1;
    cint_mpls_encapsulation_basic_info.mask = dip1_mask;
    rv = mpls_encapsulation_basic_create_l3_forwarding(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_encapsulation_basic_create_l3_forwarding\n");
        return rv;
    }

    /*
     * Create l3 forwarding entry for the pushed label: one-label-entry
     */
    cint_mpls_encapsulation_basic_info.fec_id_enc = fec_id_enc2;
    cint_mpls_encapsulation_basic_info.dip = dip2;
    cint_mpls_encapsulation_basic_info.mask = dip2_mask;
    rv = mpls_encapsulation_basic_create_l3_forwarding(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_encapsulation_basic_create_l3_forwarding\n");
        return rv;
    }
    return rv;
}



