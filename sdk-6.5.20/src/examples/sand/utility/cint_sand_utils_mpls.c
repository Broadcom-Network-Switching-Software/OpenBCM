/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_sand_utils_mpls.c Purpose: utility for VPLS.
 */


/*******************************************************************************
 *
 * Example translations for the usages of MPLS APIs from DPP(JR+) to DNX(JR2) begin
 * Add your example below between the frame if you have.
 * {
 */
/*
 * { mpls_port_add, backward compatible example -- begin
 *
 * This is an Example for porting JR+ bcm_mpls_port_add to JR2.
 * It can be used to replace the right BCM API in JR+ cints/applications
 * and works for both DPP(JR+) and DNX(JR2) on ingress and egress MPLS port instance.
 *
 * Remark:
 * In DPP, bcm_mpls_port_add can create mpls ingress, egress and FEC objects.
 * While in DNX, FEC object should be created with bcm_l3_egress_create sperately.
 * So, here FEC objects won't be returned by mpls_port_id.
 *
 * If both ingress and egress are created in the same call(No _INGRESS_ONLY and
 * _EGRESS_ONLY flags were set), then _WITH_ID should be set for ingress objects.
 */
 
/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization            -----------------
 */
/* Struct definitions */

struct mpls__egress_tunnel_utils_s
{
    int flags;
    int flags_out;
    int flags_out_2;
    int flags_out_3;
    int force_flags; /* Pass force_flags=1 for independent configuration; otherwise, flags will be configured automatically*/
    int force_flags_out;
    bcm_mpls_egress_action_t egress_action;
    bcm_mpls_label_t label_in;
    bcm_mpls_label_t label_out; /* 0 in case only one label is created */
    bcm_mpls_label_t label_out_2; /* 0 in case only 1-2 labels are created */
    bcm_mpls_label_t label_out_3; /* 0 in case only 1-3 labels are created */
    int next_pointer_intf;
    int tunnel_id; /* out parameter, created tunnel id */
    bcm_failover_t egress_failover_id;
    bcm_if_t egress_failover_if_id;
    uint8 ttl;
    uint8 exp;
    uint8 ext_ttl;
    uint8 ext_exp;
    int with_exp;
    bcm_mpls_special_label_push_type_t spl_label_push_type;
    bcm_encap_access_t encap_access;
};

struct mpls__ingress_tunnel_utils_s
{
    int flags;
    int flags2;
    bcm_mpls_label_t label; /* Incoming label value. */
    bcm_mpls_label_t second_label;  /* Incoming second label. */
    bcm_mpls_label_t swap_label; /* Label for egress, e.g. for swapping */
    bcm_mpls_switch_action_t action;    /* MPLS label action. */
    bcm_if_t tunnel_if; /* hierarchical interface, relevant for
                                           when action is
                                           BCM_MPLS_SWITCH_ACTION_POP. */
    bcm_vpn_t vpn; /* set this value only if action is pop */
    bcm_gport_t tunnel_id;              /* Tunnel ID. */
    bcm_failover_t failover_id; /* Failover id for protection */
    bcm_gport_t failover_tunnel_id; /* Failover Tunnel ID */
    bcm_if_t fec; /* FEC pointer */
};

struct mpls_lsr_info_s
{
    int in_port;                /* phy port */

    /*
     * ingress interface attributes
     */
    int in_vid;                 /* outer vlan */
    uint8 my_mac[6];
    int in_label;               /* incomming */

    /*
     * egress -attributes
     */
    int eg_port;                /* phy port */
    int eg_label;               /* egress label */
    int eg_vid;
    bcm_mac_t next_hop_mac;
    int out_to_tunnel;          /* If enable, gets out to Tunnel */
    int encap_id;               /* encapsulation id */
    int mpls_port_id;           /* port id */
    bcm_if_t egress_intf;       /* egress intf */
    bcm_if_t ingress_intf;      /* ingress l3 intf */
    uint8 use_multiple_mymac;   /* If true, use multiple mymac termination to terminate mpls packets instead of global
                                 * mymac. */
    int mymac_l2_station_id;

    int access_index_1 ;
    int use_custom_index_1;
    int access_index_2 ;
    int use_custom_index_2;
    int access_index_3 ;
    int use_custom_index_3;
};

mpls_lsr_info_s mpls_lsr_info_1;

/* Globals */
uint8 mpls_pipe_mode_exp_set = 0;
uint8 qax_egress_label_custom_flags = 0;

/* global exp value for a label*/
int mpls_exp = 0;
int ext_mpls_exp = 4;
int mpls_ttl = 20;
int ext_mpls_ttl = 60;

int mpls_default_egress_label_value = 0;
int nof_labels;

/*
 * Internal functions
 */

void init_outer_label(int unit, bcm_mpls_egress_label_t* label_conf,
                      mpls__egress_tunnel_utils_s *mpls_tunnel_properties,
                      bcm_mpls_label_t label, uint32 flags) {

    bcm_mpls_egress_label_t_init(label_conf);

    if (mpls_tunnel_properties->force_flags) {
        label_conf->flags = flags;
    } else {
        label_conf->flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT | flags;
        label_conf->ttl = mpls_tunnel_properties->ext_ttl ? mpls_tunnel_properties->ext_ttl : ext_mpls_ttl;
        if (is_device_or_above(unit, JERICHO2)) {
            label_conf->egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
        } else {
            label_conf->flags |= BCM_MPLS_EGRESS_LABEL_TTL_SET;
        }
    }
    label_conf->tunnel_id = mpls_tunnel_properties->tunnel_id;
    if (!is_device_or_above(unit,ARAD_PLUS) || mpls_pipe_mode_exp_set) {
        label_conf->exp = mpls_tunnel_properties->with_exp ? mpls_tunnel_properties->ext_exp : ext_mpls_exp;
        if (is_device_or_above(unit, JERICHO2)) {
            label_conf->egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
        } else {
            label_conf->flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
        }
    } else {
        if (is_device_or_above(unit, JERICHO2)) {
            label_conf->egress_qos_model.egress_qos = bcmQosEgressModelUniform;
        } else {
            label_conf->flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
        }
    }
    label_conf->label = label;
    label_conf->l3_intf_id = mpls_tunnel_properties->next_pointer_intf;

    if (mpls_tunnel_properties->flags & BCM_MPLS_EGRESS_LABEL_PROTECTION) {
        label_conf->egress_failover_id = mpls_tunnel_properties->egress_failover_id;
        label_conf->egress_failover_if_id = mpls_tunnel_properties->egress_failover_if_id;
    }

}
/* Sets mpls pipe mode exp*/
int mpls__mpls_pipe_mode_exp_set(int unit) {
    int rv = BCM_E_NONE;
    /*JR2 does not support the switch propertity, uniform or pipe is configured by API per LIF*/
    if (!is_device_or_above(unit, JERICHO2)){
        rv = bcm_switch_control_set(unit, bcmSwitchMplsPipeTunnelLabelExpSet, mpls_pipe_mode_exp_set);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_switch_control_set\n");
            print rv;
            return rv;
        }
    }
    return rv;
}

/*
 * Internal functions
 */

/* Init function for this file*/
int
mpls__init(int unit)
{
    int rv;

    rv = mpls__mpls_pipe_mode_exp_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls__mpls_pipe_mode_exp_set\n");
        return rv;
    }

    return rv;
}

bcm_mac_t
mpls_lsr_my_mac_get(
    )
{
    return mpls_lsr_info_1.my_mac;
}

int
mpls_lsr_init(
    int in_sysport,
    int out_sysport,
    int my_mac_lsb,
    int next_hop_lsb,
    int in_label,
    int out_label,
    int in_vlan,
    int out_vlan,
    int out_to_tunnel)
{
    uint8 mac_1[6] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    uint8 mac_2[6] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    mac_1[5] = my_mac_lsb;
    mac_2[5] = next_hop_lsb;

    /*
     * incomming
     */
    mpls_lsr_info_1.in_port = in_sysport;
    mpls_lsr_info_1.in_vid = in_vlan;
    sal_memcpy(mpls_lsr_info_1.my_mac, mac_1, 6);
    mpls_lsr_info_1.in_label = in_label;
    /*
     * eg atrributes
     */
    mpls_lsr_info_1.eg_label = out_label;

    mpls_lsr_info_1.eg_port = out_sysport;

    mpls_lsr_info_1.eg_vid = out_vlan;

    sal_memcpy(mpls_lsr_info_1.next_hop_mac, mac_2, 6);

    mpls_lsr_info_1.out_to_tunnel = out_to_tunnel;

    if (!mpls_lsr_info_1.use_custom_index_1)
        mpls_lsr_info_1.access_index_1 = 1;
    if (!mpls_lsr_info_1.use_custom_index_2)
        mpls_lsr_info_1.access_index_2 = 2;
    if (!mpls_lsr_info_1.use_custom_index_3)
        mpls_lsr_info_1.access_index_3 = 3;

    return 0;

}

int
sand_mpls_port_add(
    int unit,
    bcm_vpn_t vpn,
    bcm_mpls_port_t * mpls_port)
{
    int rv;
    /*
     * In DPP devices, bcm_mpls_port_add can add termination, encapsulation and FEC HW in a single call.
     */
    if (!is_device_or_above(unit, JERICHO2))
    {
        rv = bcm_mpls_port_add(unit, vpn, mpls_port);
        if (rv != BCM_E_NONE)
        {
            printf("bcm_mpls_port_add failed: %d \n", rv);
            return rv;
        }

        return rv;
    }

    /*
     * In DNX devices, the same configurations must be achived by:
     * 1) Create egress encap object with BCM_MPLS_PORT_EGRESS_ONLY
     * 2) Create FEC object
     * 3) Create Ingress termination object with BCM_MPLS_PORT_INGRESS_ONLY
     */

    int is_ingress, is_egress;
    bcm_mpls_port_t mpls_port_ing, mpls_port_egr;
    int is_forward_group, is_1to1_protect;
    bcm_if_t if_fec_id;
    uint32 flags, flags2;
    uint8 is_jr2_mode = soc_property_get(unit, spn_SYSTEM_HEADERS_MODE, 1);

    flags = 0;
    /** Available flags in JR2:*/
    flags |= (mpls_port->flags & BCM_MPLS_PORT_WITH_ID) ? BCM_MPLS_PORT_WITH_ID : 0;
    flags |= (mpls_port->flags & BCM_MPLS_PORT_CONTROL_WORD) ? BCM_MPLS_PORT_CONTROL_WORD : 0;
    flags |= (mpls_port->flags & BCM_MPLS_PORT_NETWORK) ? BCM_MPLS_PORT_NETWORK : 0;
    flags |= (mpls_port->flags & BCM_MPLS_PORT_REPLACE) ? BCM_MPLS_PORT_REPLACE : 0;
    flags |= (mpls_port->flags & BCM_MPLS_PORT_ENTROPY_ENABLE) ? BCM_MPLS_PORT_ENTROPY_ENABLE : 0;
    flags |= (mpls_port->flags & BCM_MPLS_PORT_EGRESS_TUNNEL) ? BCM_MPLS_PORT_EGRESS_TUNNEL : 0;
    flags |= (mpls_port->flags & BCM_MPLS_PORT_ENCAP_WITH_ID) ? BCM_MPLS_PORT_ENCAP_WITH_ID : 0;

    is_forward_group = (mpls_port->flags & BCM_MPLS_PORT_FORWARD_GROUP) ? 1 : 0;
    is_1to1_protect = (mpls_port->failover_id > 0) && (mpls_port->failover_id >> 29 == 2) && !(mpls_port->flags2 & BCM_MPLS_PORT2_INGRESS_ONLY);

    is_ingress = (mpls_port->flags2 & BCM_MPLS_PORT2_EGRESS_ONLY) ? FALSE : TRUE;
    is_egress = (mpls_port->flags2 & BCM_MPLS_PORT2_INGRESS_ONLY) ? FALSE : TRUE;

    if (is_forward_group)
    {
        is_ingress = is_egress = FALSE;
    }

    /** 1) Encapsulation using bcm_mpls_port_add with BCM_MPLS_PORT_EGRESS_ONLY flag.*/
    if (is_egress)
    {
        bcm_mpls_port_t_init(&mpls_port_egr);

        /** Available flags in JR2:*/
        mpls_port_egr.flags |= flags;

        mpls_port_egr.flags2 |= BCM_MPLS_PORT2_EGRESS_ONLY;

        mpls_port_egr.criteria = BCM_MPLS_PORT_MATCH_INVALID;
        mpls_port_egr.port = BCM_GPORT_INVALID;
        mpls_port_egr.match_label = 0;

        /** Outlif encap params. */
        sal_memcpy(&(mpls_port_egr.egress_label), &(mpls_port->egress_label), sizeof(mpls_port->egress_label));
        sal_memcpy(&(mpls_port_egr.egress_tunnel_label), &(mpls_port->egress_tunnel_label), sizeof(mpls_port->egress_tunnel_label));

        mpls_port_egr.egress_label.flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
        mpls_port_egr.encap_id = mpls_port->encap_id;
        if (BCM_L3_ITF_TYPE_IS_LIF(mpls_port->egress_tunnel_if))
        {
            mpls_port_egr.egress_tunnel_if = mpls_port->egress_tunnel_if;
        }
        else if (BCM_L3_ITF_TYPE_IS_FEC(mpls_port->egress_tunnel_if))
        {
            bcm_l3_egress_t egr;
            bcm_l3_egress_t_init(&egr);

            /** get the tunnel encap from FEC.*/
            rv = bcm_l3_egress_get(unit, mpls_port->egress_tunnel_if, &egr);
            if (rv != BCM_E_NONE)
            {
                printf("bcm_l3_egress_get failed for getting tunnel encap id from FEC: %d \n", rv);
                return rv;
            }

            mpls_port_egr.egress_tunnel_if = egr.intf;
        }
        mpls_port_egr.mpls_port_id = mpls_port->mpls_port_id;
        mpls_port_egr.network_group_id = mpls_port->network_group_id;

        /** Outlif 1:1 protection params. */
        mpls_port_egr.egress_failover_id = mpls_port->egress_failover_id;
        mpls_port_egr.egress_failover_port_id = mpls_port->egress_failover_port_id;

        rv = bcm_mpls_port_add(unit, 0, &mpls_port_egr);
        if (rv != BCM_E_NONE)
        {
            printf("bcm_mpls_port_add failed for egress: %d \n", rv);
            return rv;
        }

        /** Save the result in mpls-port*/
        mpls_port->encap_id = mpls_port_egr.encap_id;
        mpls_port->mpls_port_id = mpls_port_egr.mpls_port_id;

        if(verbose >= 2) {
            printf("mpls port add egress objects complete, mpls_port_id = 0x%08X, encap_id = 0x%08X\n", mpls_port->mpls_port_id, mpls_port->encap_id);
        }
    }

    /** 2) FEC using bcm_l3_egress_create.*/
    if (is_forward_group || is_1to1_protect)
    {
        uint32 l3eg_flags;
        bcm_l3_egress_t l3eg;
        bcm_if_t l3egid;

        bcm_l3_egress_t_init(l3eg);

        l3eg.port = mpls_port->port;
        l3eg.encap_id = mpls_port->encap_id;

        /** FEC 1:1 params */
        l3eg.failover_id = mpls_port->failover_id;
        l3eg.failover_if_id = mpls_port->egress_failover_port_id;
        l3eg.failover_mc_group = mpls_port->failover_mc_group;

        if(mpls_port.flags & BCM_MPLS_PORT_EGRESS_TUNNEL)
        {
            l3eg.intf = mpls_port->egress_tunnel_if;
        }

        /** For mpls_port -> flags */
        if (mpls_port->flags & BCM_MPLS_PORT_WITH_ID)
        {
            if (BCM_GPORT_SUB_TYPE_IS_FORWARD_GROUP(mpls_port->mpls_port_id))
            {
                l3eg_flags |= BCM_L3_WITH_ID;
                BCM_GPORT_FORWARD_GROUP_TO_L3_ITF_FEC(if_fec_id, mpls_port->mpls_port_id);
            }
            else if(is_1to1_protect)
            {
                l3eg_flags |= BCM_L3_WITH_ID;
                if_fec_id = BCM_GPORT_MPLS_PORT_ID_GET(mpls_port->mpls_port_id);
                BCM_L3_ITF_SET(*if_fec_id, BCM_L3_ITF_TYPE_FEC, if_fec_id);
            }

        }
        if (mpls_port->flags & BCM_MPLS_PORT_REPLACE)
        {
            l3eg_flags |= BCM_L3_REPLACE;
        }

        /** For mpls_port -> flags2*/
        if(mpls_port->flags2 & BCM_MPLS_PORT2_CASCADED)
        {
            l3eg.flags |= BCM_L3_2ND_HIERARCHY;
        }

        l3eg_flags = BCM_L3_INGRESS_ONLY;
        rv = bcm_l3_egress_create(unit, l3eg_flags, &l3eg, &if_fec_id);
        if (rv != BCM_E_NONE)
        {
            printf("bcm_l3_egress_create FEC failed: %d \n", rv);
            return rv;
        }

        printf("Example FEC objects created by bcm_l3_egress_create for mpls port add, fec-id = 0x%08X \n", if_fec_id);
        printf("Attention!! FEC objects should be created by bcm_l3_egress_create separately. It can't be resturned by mpls_port_id in JR2.\n");
    }

    /** 3) Termination using bcm_mpls_port_add with BCM_MPLS_PORT_INGRESS_ONLY flag. */
    if (is_ingress)
    {
        bcm_mpls_port_t_init(&mpls_port_ing);

        /** Available flags in JR2:*/
        mpls_port_ing.flags |= flags;

        /** Available flags2 in JR2:*/
        mpls_port_ing.flags2 |= BCM_MPLS_PORT2_INGRESS_ONLY | (vpn ? 0 : BCM_MPLS_PORT2_CROSS_CONNECT);

        if (BCM_GPORT_IS_MPLS_PORT(mpls_port->mpls_port_id))
        {
            mpls_port_ing.flags |= BCM_MPLS_PORT_WITH_ID;
        }

        mpls_port_ing.port = mpls_port->port;
        mpls_port_ing.criteria = mpls_port->criteria;
        mpls_port_ing.match_label = mpls_port->match_label;
        mpls_port_ing.mpls_port_id = mpls_port->mpls_port_id;
        
        mpls_port_ing.failover_id = mpls_port->failover_id;
        mpls_port_ing.failover_port_id = mpls_port->failover_port_id;
        mpls_port_ing.failover_mc_group = mpls_port->failover_mc_group;
        mpls_port_ing.ingress_failover_id = mpls_port->ingress_failover_id;
        mpls_port_ing.ingress_failover_port_id = mpls_port->ingress_failover_port_id;
      
        mpls_port_ing.network_group_id = mpls_port->network_group_id;
        mpls_port_ing.vccv_type = mpls_port->vccv_type;
        sal_memcpy(&(mpls_port_ing.ingress_qos_model), &(mpls_port->ingress_qos_model), sizeof(mpls_port->ingress_qos_model));

        /*
         * If egress_tunnel_if is valid for ingress, it must be FEC for learning.
         * But if 1+1 protection, egress_tunnel_if can't be FEC since learning info will be bicast mc-group.
         */
        mpls_port_ing.egress_tunnel_if = BCM_L3_ITF_TYPE_IS_FEC(mpls_port->egress_tunnel_if) && (mpls_port->failover_mc_group == 0) ? mpls_port->egress_tunnel_if : 0;
        mpls_port_ing.encap_id = mpls_port->failover_mc_group? 0 : (BCM_L3_ITF_TYPE_IS_FEC(mpls_port_ing.egress_tunnel_if) ? 0 : mpls_port->encap_id);
        if (!is_jr2_mode && BCM_L3_ITF_TYPE_IS_FEC(mpls_port->egress_tunnel_if) && (mpls_port->failover_mc_group == 0)) {
            sal_memcpy(&(mpls_port_ing.egress_label), &(mpls_port->egress_label), sizeof(mpls_port->egress_label));
        }

        rv = bcm_mpls_port_add(unit, vpn, &mpls_port_ing);
        if (rv != BCM_E_NONE)
        {
            printf("bcm_mpls_port_add failed for ingress: %d \n", rv);
            return rv;
        }

        mpls_port->mpls_port_id = mpls_port_ing.mpls_port_id;
        mpls_port->encap_id = mpls_port_ing.encap_id;

        if(verbose >= 2) {
            printf("mpls port add ingress objects complete, mpls_port_id = 0x%08X, encap_id = 0x%08X, egress_tunnel_if = 0x%08X\n", mpls_port_ing.mpls_port_id, mpls_port_ing.encap_id, mpls_port_ing.egress_tunnel_if);
        }
    }

    return rv;
}
/*
 * } mpls port add -- end
 */

/*
 * { mpls tunnel initiator create -- begin
 *
 * This is an Example for porting JR+ bcm_mpls_tunnel_initiator_create to JR2.
 *
 * Remark:
 * Refer to the backward compatibility document for more informations about
 * the API changes.
 */
int
sand_mpls_tunnel_initiator_create(
    int unit,
    bcm_if_t intf,
    int num_labels,
    bcm_mpls_egress_label_t * label_array)
{
    int rv;
    /*
     * In DPP devices, bcm_mpls_tunnel_initiator_create can add 4 label entries
     * with different qos inheritance mode and label actions.
     */
    if (!is_device_or_above(unit, JERICHO2))
    {
        rv = bcm_mpls_tunnel_initiator_create(unit, intf, num_labels, label_array);
        if (rv != BCM_E_NONE)
        {
            printf("bcm_mpls_tunnel_initiator_create failed: %d \n", rv);
            return rv;
        }

        return rv;
    }

    /*
     * In DNX devices, bcm_mpls_tunnel_initiator_create can add 2 label entries
     * with the same qos inheritance mode.
     * PUSH and SWAP action is per forward-code, rather than tunnel attributes.
     * No changes to PHP action.
     */
    int label_index, skip_count, nof_labels;
    int tunnel_id, access_id;
    bcm_mpls_egress_label_t outer_label_array[2];
    uint32 couple_flags, qos_model_diff =0, ttl_model_diff=0;
    uint32 exp_diff = 0,ttl_diff = 0,qos_id_diff = 0, failover_id_diff = 0,failover_lif_diff = 0;
    bcm_if_t l3_if_id;
    uint8 is_tandem = 1;
    bcm_encap_access_t access_phase[7] = {bcmEncapAccessNativeArp,
                                          bcmEncapAccessTunnel1, bcmEncapAccessTunnel2,
                                          bcmEncapAccessTunnel3, bcmEncapAccessTunnel4,
                                          bcmEncapAccessArp, bcmEncapAccessInvalid};

    for (label_index = 0; label_index < num_labels; label_index++)
    {
        /** Only PHP is supported, other action is resolved by hard-logic.*/
        if(label_array[label_index].action != BCM_MPLS_EGRESS_ACTION_PHP)
        {
            label_array[label_index].action = BCM_MPLS_EGRESS_ACTION_SWAP;
            label_array[label_index].flags &= ~BCM_MPLS_EGRESS_LABEL_ACTION_VALID;
        }

        /** This flag must be set in JR2.*/
        label_array[label_index].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;

        if (label_array[label_index].flags & BCM_MPLS_EGRESS_LABEL_PRESERVE)
        {
            printf("In JR2, up to 12 labels can be encapsolated in PP, LABEL_PRESERVE solution is not needed!\n");
            return BCM_E_PARAM;
        }

        if (label_array[label_index].flags & (BCM_MPLS_EGRESS_LABEL_IML | BCM_MPLS_EGRESS_LABEL_EVPN))
        {
            printf("Refer to the EVPN UM for solution to EVPN in JR2!\n");
            return BCM_E_PARAM;
        }

        /** Remove the unused flags*/
        label_array[label_index].flags &= ~BCM_MPLS_EGRESS_LABEL_ECN_TO_EXP_MAP;
        label_array[label_index].flags &= ~BCM_MPLS_EGRESS_LABEL_INT_CN_TO_EXP_MAP;
        label_array[label_index].flags &= ~BCM_MPLS_EGRESS_LABEL_ECN_EXP_MAP_TRUST;
        label_array[label_index].flags &= ~BCM_MPLS_EGRESS_LABEL_EXP_REMARK;
        label_array[label_index].flags &= ~BCM_MPLS_EGRESS_LABEL_WIDE;

        /** In JR2, next protocol depends on the next nibble in PHP. */
        label_array[label_index].flags &= ~(BCM_MPLS_EGRESS_LABEL_PHP_IPV4 |
                                            BCM_MPLS_EGRESS_LABEL_PHP_IPV6 |
                                            BCM_MPLS_EGRESS_LABEL_PHP_L2);
    }

    /** Suppose we needed four tunnel encap */
    access_id = 4;

    skip_count = 0;
    tunnel_id = 0;
    bcm_mpls_egress_label_t_init(&outer_label_array[0]);
    bcm_mpls_egress_label_t_init(&outer_label_array[1]);

    l3_if_id = label_array[num_labels-1].l3_intf_id;

    for (label_index = num_labels - 1; label_index >= 0; label_index = label_index - 1 - skip_count)
    {
        if (label_index >= 1)
        {
            nof_labels = 2;
            sal_memcpy(outer_label_array, &label_array[label_index- 1], sizeof(label_array[0]) * nof_labels);
            couple_flags = outer_label_array[0].flags ^ outer_label_array[1].flags;
            if (is_device_or_above(unit, JERICHO2))
            {
                qos_model_diff = (outer_label_array[0].egress_qos_model.egress_qos != outer_label_array[1].egress_qos_model.egress_qos) ? TRUE : FALSE;
                ttl_model_diff = (outer_label_array[0].egress_qos_model.egress_ttl != outer_label_array[1].egress_qos_model.egress_ttl) ? TRUE : FALSE;
            }
            else
            {
                qos_model_diff = ((couple_flags & BCM_MPLS_EGRESS_LABEL_EXP_SET) || (couple_flags & BCM_MPLS_EGRESS_LABEL_EXP_COPY)) ? TRUE :FALSE;
                ttl_model_diff = ((couple_flags & BCM_MPLS_EGRESS_LABEL_TTL_SET) || (couple_flags & BCM_MPLS_EGRESS_LABEL_TTL_COPY)) ? TRUE :FALSE;
            }
            exp_diff = outer_label_array[0].exp ^ outer_label_array[1].exp;
            ttl_diff = outer_label_array[0].ttl ^ outer_label_array[1].ttl;
            qos_id_diff = outer_label_array[0].qos_map_id ^ outer_label_array[1].qos_map_id;
            failover_id_diff = outer_label_array[0].egress_failover_id ^ outer_label_array[1].egress_failover_id;
            failover_lif_diff = outer_label_array[0].egress_failover_if_id ^ outer_label_array[1].egress_failover_if_id;
            if ( qos_model_diff || ttl_model_diff ||
                (couple_flags & BCM_MPLS_EGRESS_LABEL_PROTECTION) ||
                exp_diff || ttl_diff || qos_id_diff ||
                failover_id_diff || failover_lif_diff)
            {
                /** Label 0 and lable 1 can't be created by the same call.*/
                nof_labels = 1;
                skip_count = 0;
                sal_memcpy(outer_label_array, &label_array[label_index], sizeof(label_array[0]) * nof_labels);
            }
            else
            {
                skip_count = 1;
            }
        }
        else
        {
            /** Only 1 label in the array.*/
            nof_labels = 1;
            skip_count = 0;
            sal_memcpy(outer_label_array, &label_array[label_index], sizeof(label_array[0]) * nof_labels);
        }

        /** Apply the input next-eep (poosibly arp encap-id) for the out-most entry*/
        if (l3_if_id != 0)
        {
            tunnel_id = l3_if_id;
            l3_if_id = 0;
        }

        /** Initialize the next-eep*/
        outer_label_array[0].l3_intf_id = tunnel_id;
        if (nof_labels == 2)
        {
            outer_label_array[1].l3_intf_id = tunnel_id;
        }

        /** Consider tandem encap to reduce the encap stages used by mpls.*/
        if (label_index - 1 - skip_count > 0)
        {
            uint8 tandem_feasible;

            /** Tandem label should has the same EXP and TTL with prior label*/
            exp_diff = outer_label_array[0].exp ^ label_array[label_index - 1 - skip_count - 1].exp;
            ttl_diff = outer_label_array[0].ttl ^ label_array[label_index - 1 - skip_count - 1].ttl;
            qos_id_diff = outer_label_array[0].qos_map_id ^ label_array[label_index - 1 - skip_count - 1].qos_map_id;

            tandem_feasible = !(exp_diff | ttl_diff | qos_id_diff);
            if (tandem_feasible)
            {
                /** It's not the last mpls encap entry, type it with tandem.*/
                if (is_tandem)
                {
                    outer_label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_TANDEM;
                    if (nof_labels == 2)
                    {
                        outer_label_array[1].flags |= BCM_MPLS_EGRESS_LABEL_TANDEM;
                    }
                }

                /** Revert the tandem indicator to make sure the prior mpls entry is not tandem.*/
                is_tandem = !is_tandem;
            }
        }

        /** Create the initiator. */
        outer_label_array[0].encap_access = access_phase[access_id];
        if (nof_labels == 2)
        {
            outer_label_array[1].encap_access = access_phase[access_id];
        }
        rv = bcm_mpls_tunnel_initiator_create(unit, intf, nof_labels, outer_label_array);
        if (rv != BCM_E_NONE)
        {
            printf("bcm_mpls_port_add failed: %d \n", rv);
            return rv;
        }

        tunnel_id = outer_label_array[0].tunnel_id;
        label_array[label_index].tunnel_id = tunnel_id;
        if (nof_labels == 2)
        {
            label_array[label_index - 1].tunnel_id = tunnel_id;
        }

        /** Update encap access*/
        access_id --;
    }

    return rv;
}
/*
 * } mpls tunnel initiator create -- end
 */

/*
* remove BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE from flags for jericho and below devices.
*/
void
unset_entropy_flag(int unit, uint32* flags)
{
    if(!is_device_or_above(unit,JERICHO_PLUS)) {
        /* for non jericho_plus - entropy flag can be placed only on BOS label */
        *flags &= ~BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE;
    }
}


/* Add switch entry
 */
int
mpls__add_switch_entry(int unit, mpls__ingress_tunnel_utils_s *mpls_tunnel_properties)
{
    int rv;
    bcm_mpls_tunnel_switch_t entry;

    bcm_mpls_tunnel_switch_t_init(&entry);
    entry.action = mpls_tunnel_properties->action;
    /* TTL decrement has to be present for JR1
     * Uniform: inherit TTL and EXP,
     * in general valid options:
     * both present (uniform) or none of them (Pipe)
     */
    entry.flags = mpls_tunnel_properties->flags;
    entry.flags2 = mpls_tunnel_properties->flags2;
    if (is_device_or_above(unit, JERICHO2)) {
        entry.ingress_qos_model.ingress_phb = bcmQosIngressModelUniform;
        entry.ingress_qos_model.ingress_remark = bcmQosIngressModelUniform;
        entry.ingress_qos_model.ingress_ttl = bcmQosIngressModelUniform;
        /** In case SWAP/NOP/PHP were requested verify that TTL_DECREMENT is set */
        if ((entry.action == BCM_MPLS_SWITCH_ACTION_SWAP) ||
            (entry.action == BCM_MPLS_SWITCH_ACTION_NOP) ||
            (entry.action == BCM_MPLS_SWITCH_ACTION_PHP) ||
            (entry.action == BCM_MPLS_SWITCH_ACTION_POP_DIRECT)) {

            /** TTL always decremented (even if inherited/copied)*/
            entry.flags |= BCM_MPLS_SWITCH_TTL_DECREMENT;
        }
    } else {
        entry.flags |= BCM_MPLS_SWITCH_OUTER_TTL|BCM_MPLS_SWITCH_OUTER_EXP;
        entry.flags |= BCM_MPLS_SWITCH_TTL_DECREMENT;
    }

    /* incoming label */
    entry.label = mpls_tunnel_properties->label;
    entry.second_label = mpls_tunnel_properties->second_label;
    if (mpls_tunnel_properties->swap_label != 0) {
        entry.egress_label.label = mpls_tunnel_properties->swap_label;
    }
    entry.tunnel_id = mpls_tunnel_properties->tunnel_id;
    entry.failover_id = mpls_tunnel_properties->failover_id;
    entry.failover_tunnel_id = mpls_tunnel_properties->failover_tunnel_id;
    entry.egress_if = mpls_tunnel_properties->fec;

    /* populate vpn information */
    entry.vpn = mpls_tunnel_properties->vpn;
    if (is_device_or_above(unit, JERICHO2) && (entry.vpn == 0) &&
            (entry.flags & (BCM_MPLS_SWITCH_NEXT_HEADER_L2 | BCM_MPLS_SWITCH_EVPN_IML))) {
        entry.flags2 |= BCM_MPLS_SWITCH2_CROSS_CONNECT;
    }

    rv = bcm_mpls_tunnel_switch_create(unit,&entry);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_mpls_tunnel_switch_create with lable %d\n", entry.label);
        return rv;
    }

    mpls_tunnel_properties->tunnel_id = entry.tunnel_id;
    return rv;
}

/*******************************************************************************
 *
 * Add your example above between the frame if you have.
 * Examples translation for the usages of L3 APIs from DPP(JR+) to DNX(JR2) end
 * }
 */




