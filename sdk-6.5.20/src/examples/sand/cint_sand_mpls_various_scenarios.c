/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~mpls_various_scenarios~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_sand_mpls_various_scenarios.c
 * Purpose: Various Qos/ttl model for MPLS tunnel.
 *
 * Example to show different Qos and TTL model of mpls tunnel in 3 scenarios:
 * PWE->tunnel, IPv4->tunnel, MPLS->Tunnel
 *
 * Basic example:
 * cd ../../../../regress/bcm
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/dnx/field/cint_field_utils.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_multicast.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_mpls.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/dnx/utility/cint_dnx_utils_vpls.c
 * cint ../../src/examples/sand/cint_sand_mpls_various_scenarios.c
 * cint
 * mpls_various_scenarios_flag_mode=0,1,2,3;
 * mpls_various_scenarios_main(0,200,201);
 *
 * tx 1 psrc=202 data=0x00000000011a0000000001158100200fffff
 * Received packets on unit 0 should be:
 * Data: 0x000000000115001100000112810040108847002bc41400bb831400000000011a0000000001158100200fffff0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
 *
 * tx 1 psrc=202 data=0x001100000111000000000115810020100800451000140000000040009883a0000011a0a1a1a4
 * Received packets on unit 0 should be:
 * Data: 0x000000000115001100000112810040108847002bc51445100014000000003f009983a0000011a0a1a1a4000000000000000000000000000000000000000000000000
 *
 * tx 1 psrc=202 data=0x0011000001110000000001158100200f8847001f4314
 * Received packets on unit 0 should be:
 * Data: 0x000000000115001100000112810020108847003202140038431300000000000000000000000000000000000000000000000000000000000000000000000000000000
 */

 struct cint_mpls_various_scenarios_info_s
{
    int in_rif;                             /* access RIF */
    int out_rif;                            /* out RIF */
    int vrf;                                /* VRF */
    bcm_mac_t my_mac[2];                    /* mac for Incoming L3 intf */
    bcm_mac_t next_hop_mac;                 /* next hop mac for outgoing station */
    bcm_ip_t dip_routing_into_tunnel[3];       /* dip routing into tunnel */
    int tunnel_label;                       /* mpls tunnel label*/
    int mpls_tunnel_label[2];               /* mpls tunnel label*/
    int lsr_incoming_label;                 /* incoming label for forwarding*/
    int lsr_outgoing_label;                 /* outgoing label for forwarding*/
    int exp[4];                             /* exp for tunnel */
    int ttl;                                /* TTL for tunnel */
    int pwe_label[2];
    bcm_gport_t mpls_port_id_egress[3];
    bcm_if_t mpls_port_encap_id[3];
    bcm_mac_t pwe_mac[3];
    int mpls_tunnel_id[4];                  /* mpls tunnel id */
    int out_arp_id[4];                      /* Next hop intf ID */
    int fec_id_for_tunnel[4];               /* FEC ID for routing into common tunnel */
    bcm_gport_t vlan_port_id;

};


 cint_mpls_various_scenarios_info_s cint_mpls_various_scenarios_info=
{
    /*
     * In-RIF,
     */
    15,
    /*
     * Out-RIF,
     */
    16,
    /*
     * vrf,
     */
    1,
    /*
     * my_mac
     */
    {{0x00, 0x11, 0x00, 0x00, 0x01, 0x11},{0x00, 0x11, 0x00, 0x00, 0x01, 0x12}},
    /*
     * next_hop_mac
     */
    {0x00, 0x00, 0x00, 0x00, 0x01, 0x15},

    /*
     * dip_routing_into_tunnel
     * 160.161.161.162, 160.161.161.163,160.161.161.164
     */
    {0xA0A1A1A2,0xA0A1A1A3,0xA0A1A1A4},

    /*
     * tunnel_label
     */
     0,

    /*
     * mpls_tunnel_label,
     */
    {700,800},

    /*
     * lsr_incoming_label,
     */
    500,

    /*
     * lsr_outgoing_label,
     */
    900,
    /*
     * exp
     */
    {0,0,2,1},
    /*
     * TTL,
     */
    20,
    /*
     * pwe label,
     */
    {3000,4000},
    /*
     * egress mpls port id,
     */
    {8888,9000,9999},
    /*
     * mpls port encap id,
     */
    {8888,9000,9999},
    /*
     * pwe_mac,
     */
    {{0x00,0x00,0x00,0x00,0x01,0x18},{0x00,0x00,0x00,0x00,0x01,0x19},{0x00,0x00,0x00,0x00,0x01,0x1a}},
};

/* Will define which combination of egress label flags are used */
int mpls_various_scenarios_flag_mode = -1 ;
int mpls_default_egress_label_value = 0 ;


/*
 * L3 intf init
 * INPUT:
 *   in_port    - traffic incoming port
 *   out_port - traffic outgoing port
 */
 int
mpls_various_scenarios_l3_intf_configuration(
    int unit,
    int in_port,
    int out_port)
{
    int rv;

    /*
     * in-Port default properties
     */
    rv = in_port_intf_set(unit, in_port, cint_mpls_various_scenarios_info.in_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set intf_in\n");
        return rv;
    }
    /*
     * Set Out-Port default properties, in case of ARP+AC no need
     */
    rv = out_port_set(unit, out_port);
    if (rv != BCM_E_NONE)
    {
        printf(" Error, out_port_set intf_out \n");
        return rv;
    }


    /*
     * in rif and out rif
     */

    sand_utils_l3_eth_rif_s eth_rif_structure;

    sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, cint_mpls_various_scenarios_info.in_rif, 0, 0, cint_mpls_various_scenarios_info.my_mac[0], cint_mpls_various_scenarios_info.vrf);
    rv = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
    if (rv != BCM_E_NONE)
    {
     printf("Error, sand_utils_l3_eth_rif_create \n");
     return rv;
    }

    sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, cint_mpls_various_scenarios_info.out_rif, 0, 0, cint_mpls_various_scenarios_info.my_mac[1], cint_mpls_various_scenarios_info.vrf);
    rv = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
    if (rv != BCM_E_NONE)
    {
     printf("Error, sand_utils_l3_eth_rif_create \n");
     return rv;
    }

    return rv;

}

/*
 * MPLS tunnel adding
 */
 int
mpls_various_scenarios_tunnel_configuration(int unit)
{
    int rv, is_jr2;
    int tunnel_num,egress_label_flags;
    bcm_mpls_egress_label_t label_array[4];
    bcm_qos_egress_model_t egress_qos;
    is_jr2 = is_device_or_above(unit, JERICHO2);

    if(mpls_various_scenarios_flag_mode == 0) {
        if (!is_jr2) {
            egress_label_flags = BCM_MPLS_EGRESS_LABEL_TTL_SET|BCM_MPLS_EGRESS_LABEL_EXP_SET;

            rv = bcm_switch_control_set(unit, bcmSwitchMplsPipeTunnelLabelExpSet, 1);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in bcm_switch_control_set \n");
                return rv;
            }

        } else {
            egress_qos.egress_qos = bcmQosEgressModelPipeNextNameSpace;
            egress_qos.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
        }
    } else if(mpls_various_scenarios_flag_mode == 1){
        if (!is_jr2) {
            egress_label_flags = BCM_MPLS_EGRESS_LABEL_TTL_COPY|BCM_MPLS_EGRESS_LABEL_EXP_SET;
            rv = bcm_switch_control_set(unit, bcmSwitchMplsPipeTunnelLabelExpSet, 1);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in bcm_switch_control_set \n");
                return rv;
            }

        } else {
            egress_qos.egress_qos = bcmQosEgressModelPipeNextNameSpace;
            egress_qos.egress_ttl = bcmQosEgressModelUniform;
        }
    } else if(mpls_various_scenarios_flag_mode == 2){

        if (!is_jr2) {
            egress_label_flags = BCM_MPLS_EGRESS_LABEL_TTL_SET|BCM_MPLS_EGRESS_LABEL_EXP_COPY;
            rv = bcm_switch_control_set(unit, bcmSwitchMplsPipeTunnelLabelExpSet, 0);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in bcm_switch_control_set \n");
                return rv;
            }

        } else {
            egress_qos.egress_qos = bcmQosEgressModelUniform;
            egress_qos.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
        }

    } else if(mpls_various_scenarios_flag_mode == 3) {

        if (!is_jr2) {
            egress_label_flags = BCM_MPLS_EGRESS_LABEL_TTL_COPY|BCM_MPLS_EGRESS_LABEL_EXP_COPY;
            rv = bcm_switch_control_set(unit, bcmSwitchMplsPipeTunnelLabelExpSet, 0);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in bcm_switch_control_set \n");
                return rv;
            }

        } else {
            egress_qos.egress_qos = bcmQosEgressModelUniform;
            egress_qos.egress_ttl = bcmQosEgressModelUniform;
        }
    }

   /*
     * 1, create tunnel0, out_label=0,inner_label=0
 */
    for (tunnel_num = 0;tunnel_num < 2; tunnel_num++) {

        bcm_mpls_egress_label_t_init(&label_array[tunnel_num]);
        label_array[tunnel_num].label = cint_mpls_various_scenarios_info.tunnel_label;
        label_array[tunnel_num].flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
        label_array[tunnel_num].action = BCM_MPLS_EGRESS_ACTION_PUSH;
        label_array[tunnel_num].flags |= BCM_MPLS_EGRESS_LABEL_ACTION_VALID;

        if (mpls_various_scenarios_flag_mode == 0 || mpls_various_scenarios_flag_mode == 2 )
        {
            label_array[tunnel_num].ttl = cint_mpls_various_scenarios_info.ttl;
        }


        if (is_jr2) {
            label_array[tunnel_num].egress_qos_model = egress_qos;
            label_array[tunnel_num].l3_intf_id = 0;
        } else {
            label_array[tunnel_num].flags |= egress_label_flags;
            label_array[tunnel_num].l3_intf_id = cint_mpls_various_scenarios_info.out_rif;
        }

    }

    rv = sand_mpls_tunnel_initiator_create(unit, 0, 2, label_array);
    if (rv != BCM_E_NONE)
    {
         printf("Error, in sand_mpls_tunnel_initiator_create tunnel0\n");
         return rv;
    }

    cint_mpls_various_scenarios_info.mpls_tunnel_id[0] = label_array[0].tunnel_id;


    /*
     * 2, create tunnel1, egr label=0
 */

    bcm_mpls_egress_label_t_init(&label_array);
    label_array[0].label = cint_mpls_various_scenarios_info.tunnel_label;
    label_array[0].flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    label_array[0].action = BCM_MPLS_EGRESS_ACTION_PUSH;
    label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_ACTION_VALID;

    if (mpls_various_scenarios_flag_mode == 0 || mpls_various_scenarios_flag_mode == 2 )
    {
        label_array[0].ttl = cint_mpls_various_scenarios_info.ttl;
    }

    if (is_jr2) {
        label_array[0].egress_qos_model = egress_qos;
        label_array[0].l3_intf_id = 0;
    } else {
        label_array[0].flags |= egress_label_flags;
        label_array[0].l3_intf_id = cint_mpls_various_scenarios_info.out_rif;
    }

    rv = sand_mpls_tunnel_initiator_create(unit, 0, 1, label_array[0]);
    if (rv != BCM_E_NONE)
    {
         printf("Error, in sand_mpls_tunnel_initiator_create tunnel1\n");
         return rv;
    }

    cint_mpls_various_scenarios_info.mpls_tunnel_id[1] = label_array[0].tunnel_id;


    /*
     * 3, create tunnel2 and tunnel3
     * pwe and ip route to tunnel2, mpls swap then push tunnel3
     */
    for (tunnel_num = 0;tunnel_num < 2; tunnel_num++) {

        bcm_mpls_egress_label_t_init(&label_array[tunnel_num]);
        label_array[tunnel_num].label = cint_mpls_various_scenarios_info.mpls_tunnel_label[tunnel_num];
        label_array[tunnel_num].flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;

        label_array[tunnel_num].action = BCM_MPLS_EGRESS_ACTION_PUSH;
        if (mpls_various_scenarios_flag_mode == 0 || mpls_various_scenarios_flag_mode == 1 )
        {
            label_array[tunnel_num].exp = cint_mpls_various_scenarios_info.exp[tunnel_num+2];
        }
        if (mpls_various_scenarios_flag_mode == 0 || mpls_various_scenarios_flag_mode == 2 )
        {
            label_array[tunnel_num].ttl = cint_mpls_various_scenarios_info.ttl;
        }
        if (is_jr2) {
            label_array[tunnel_num].l3_intf_id = 0;
            label_array[tunnel_num].egress_qos_model = egress_qos;
        } else {
            label_array[tunnel_num].l3_intf_id = cint_mpls_various_scenarios_info.out_rif;
            label_array[tunnel_num].flags |= egress_label_flags;
        }
        print label_array[tunnel_num];
        rv = sand_mpls_tunnel_initiator_create(unit, 0, 1, label_array[tunnel_num]);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in sand_mpls_tunnel_initiator_create tunnel 2,3 \n");
            return rv;
        }

        cint_mpls_various_scenarios_info.mpls_tunnel_id[tunnel_num+2] = label_array[tunnel_num].tunnel_id;

    }

    return rv;
}

/*
 * ARP adding
 */
int
 mpls_various_scenarios_arp_configuration(
    int unit)
{
    int rv,tunnel_num;
    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid_null;       /* not intersting */
    uint32 flags =  BCM_L3_EGRESS_ONLY;

    bcm_l3_egress_t_init(&l3eg);
    l3eg.mac_addr = cint_mpls_various_scenarios_info.next_hop_mac;
    l3eg.encap_id = 0;
    l3eg.vlan = cint_mpls_various_scenarios_info.out_rif;

    for (tunnel_num = 0;tunnel_num < 4; tunnel_num++) {

        l3eg.intf =  cint_mpls_various_scenarios_info.mpls_tunnel_id[tunnel_num];
        if (tunnel_num > 0)
        {
            flags |= BCM_L3_REPLACE |BCM_L3_WITH_ID;
        }

        rv = bcm_l3_egress_create(unit, flags, &l3eg, &l3egid_null);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_l3_egress_create\n");
            return rv;
        }

        cint_mpls_various_scenarios_info.out_arp_id[tunnel_num] = l3eg.encap_id;
    }

    return rv;

}


/*
 * FEC entry adding
 * INPUT:
 *   in_port  - traffic incoming port
 *   out_port - traffic outgoing port
 */
 int
 mpls_various_scenarios_fec_entry_configuration(
    int unit,
    int out_port)
{
    int rv;
    int tunnel_num = 0;
    sand_utils_l3_fec_s fec_structure;


    /*
     * 1, fec_id for tunnel0
     */
    sand_utils_l3_fec_s_init(unit, 0x0, &fec_structure);
    BCM_GPORT_LOCAL_SET(fec_structure.destination, out_port);
    fec_structure.tunnel_gport = cint_mpls_various_scenarios_info.mpls_tunnel_id[0];

    rv = sand_utils_l3_fec_create(unit, &fec_structure);
    if (rv != BCM_E_NONE)
    {
         printf("Error, in sand_utils_l3_fec_create\n");
         return rv;
    }
    cint_mpls_various_scenarios_info.fec_id_for_tunnel[0] = fec_structure.l3eg_fec_id;

    /*
     * 2, fec_id for tunnel1
     */
    sand_utils_l3_fec_s_init(unit, 0x0, &fec_structure);
    BCM_GPORT_LOCAL_SET(fec_structure.destination, out_port);
    fec_structure.tunnel_gport = cint_mpls_various_scenarios_info.mpls_tunnel_id[1];

    rv = sand_utils_l3_fec_create(unit, &fec_structure);
    if (rv != BCM_E_NONE)
    {
         printf("Error, in sand_utils_l3_fec_create\n");
         return rv;
    }
    cint_mpls_various_scenarios_info.fec_id_for_tunnel[1] = fec_structure.l3eg_fec_id;


    /*
     * 3, fec_id for tunnel2
     */
    sand_utils_l3_fec_s_init(unit, 0x0, &fec_structure);
    BCM_GPORT_LOCAL_SET(fec_structure.destination, out_port);
    fec_structure.tunnel_gport = cint_mpls_various_scenarios_info.mpls_tunnel_id[2];

    rv = sand_utils_l3_fec_create(unit, &fec_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in sand_utils_l3_fec_create\n");
        return rv;
    }
    cint_mpls_various_scenarios_info.fec_id_for_tunnel[2] = fec_structure.l3eg_fec_id;

    /*
     * 4, fec_id for tunnel3
     */
    sand_utils_l3_fec_s_init(unit, 0x0, &fec_structure);
    BCM_GPORT_LOCAL_SET(fec_structure.destination, out_port);
    fec_structure.tunnel_gport = cint_mpls_various_scenarios_info.mpls_tunnel_id[3];

    rv = sand_utils_l3_fec_create(unit, &fec_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in sand_utils_l3_fec_create\n");
        return rv;
    }
    cint_mpls_various_scenarios_info.fec_id_for_tunnel[3] = fec_structure.l3eg_fec_id;


    return rv;
}

/*
 * forward entry adding(ILM, IPv4 Route)
 */
 int
mpls_various_scenarios_forward_entry_configuration(int unit)
{
    int rv;
    bcm_mpls_tunnel_switch_t entry;
    int index = 0;

    /*
     * 1. dip[0] routing into fec[0]
     */
    rv = add_host_ipv4(unit, cint_mpls_various_scenarios_info.dip_routing_into_tunnel[0], cint_mpls_various_scenarios_info.vrf, cint_mpls_various_scenarios_info.fec_id_for_tunnel[0], 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in add_host_ipv4\n", rv);
        return rv;
    }

    /*
     * 2. dip[1] routing into fec[1]
     */
    rv = add_host_ipv4(unit, cint_mpls_various_scenarios_info.dip_routing_into_tunnel[1], cint_mpls_various_scenarios_info.vrf, cint_mpls_various_scenarios_info.fec_id_for_tunnel[1], 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in add_host_ipv4\n", rv);
        return rv;
    }

    /*
     * 3. dip[2] routing into fec[2]
     */
    rv = add_host_ipv4(unit, cint_mpls_various_scenarios_info.dip_routing_into_tunnel[2], cint_mpls_various_scenarios_info.vrf, cint_mpls_various_scenarios_info.fec_id_for_tunnel[2], 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in add_host_ipv4\n", rv);
        return rv;
    }


    /*
     * FEC for swap
     */
    bcm_mpls_tunnel_switch_t_init(&entry);
    entry.action = BCM_MPLS_SWITCH_ACTION_SWAP;
    entry.label = cint_mpls_various_scenarios_info.lsr_incoming_label;
    entry.egress_label.label = cint_mpls_various_scenarios_info.lsr_outgoing_label;
    entry.flags |= BCM_MPLS_SWITCH_TTL_DECREMENT;
    BCM_L3_ITF_SET(entry.egress_if,BCM_L3_ITF_TYPE_FEC,cint_mpls_various_scenarios_info.fec_id_for_tunnel[3]);
    rv = bcm_mpls_tunnel_switch_create(unit, &entry);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_mpls_tunnel_switch_create\n", rv);
        return rv;
    }
    return rv;
}

/*
 * MPLS port egress create
 */
int
mpls_various_scenarios_pwe_encapsulation(int unit)
{
    int rv = BCM_E_NONE;
    int pwe_num,pwe_encap_id,label_count;
    bcm_mpls_port_t mpls_port;
    bcm_gport_t gport_id;
    bcm_mpls_egress_label_t label_array[4];

    bcm_mpls_port_t_init(&mpls_port);
    mpls_port.flags = BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_EGRESS_TUNNEL | BCM_MPLS_PORT_ENCAP_WITH_ID;
    mpls_port.flags2 = BCM_MPLS_PORT2_EGRESS_ONLY;

    if (is_device_or_above(unit, JERICHO2)) {
        mpls_port.egress_label.egress_qos_model.egress_qos = bcmQosEgressModelUniform;
        mpls_port.egress_label.egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
    } else {
        mpls_port.egress_label.flags |= BCM_MPLS_EGRESS_LABEL_TTL_SET;
    }

    mpls_port.egress_label.flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    mpls_port.egress_label.ttl = 20;


    /*PWE1*/
    mpls_port.encap_id = cint_mpls_various_scenarios_info.mpls_port_encap_id[0];
    BCM_GPORT_SUB_TYPE_LIF_SET(gport_id, 0, cint_mpls_various_scenarios_info.mpls_port_id_egress[0]);
    BCM_GPORT_MPLS_PORT_ID_SET(mpls_port.mpls_port_id, gport_id);
    mpls_port.egress_label.label = cint_mpls_various_scenarios_info.pwe_label[0];
    mpls_port.egress_tunnel_if = cint_mpls_various_scenarios_info.mpls_tunnel_id[0];

    rv = bcm_mpls_port_add(unit, 0, mpls_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in bcm_mpls_port_add pwe1 encapsulation\n", rv);
        return rv;
    }
    cint_mpls_various_scenarios_info.mpls_port_id_egress[0] = mpls_port.mpls_port_id;
    cint_mpls_various_scenarios_info.mpls_port_encap_id[0] = mpls_port.encap_id;


    /*PWE2*/
    mpls_port.encap_id = cint_mpls_various_scenarios_info.mpls_port_encap_id[1];
    BCM_GPORT_SUB_TYPE_LIF_SET(gport_id, 0, cint_mpls_various_scenarios_info.mpls_port_id_egress[1]);
    BCM_GPORT_MPLS_PORT_ID_SET(mpls_port.mpls_port_id, gport_id);
    mpls_port.egress_label.label = cint_mpls_various_scenarios_info.pwe_label[1];
    mpls_port.egress_tunnel_if = cint_mpls_various_scenarios_info.mpls_tunnel_id[0];

    rv = bcm_mpls_port_add(unit, 0, mpls_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in bcm_mpls_port_add pwe2 encapsulation\n", rv);
        return rv;
    }
    cint_mpls_various_scenarios_info.mpls_port_id_egress[1] = mpls_port.mpls_port_id;
    cint_mpls_various_scenarios_info.mpls_port_encap_id[1] = mpls_port.encap_id;

    if (!is_device_or_above(unit, JERICHO2)) {
        bcm_mpls_egress_label_t_init(&label_array[0]);
        bcm_mpls_egress_label_t_init(&label_array[1]);

        BCM_L3_ITF_SET(pwe_encap_id, BCM_L3_ITF_TYPE_LIF, cint_mpls_various_scenarios_info.mpls_port_encap_id[1]);
        rv = bcm_mpls_tunnel_initiator_get(unit, pwe_encap_id, 1, &label_array, &label_count); /* 1 */
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_mpls_tunnel_initiator_get\n");
            return rv;
        }

        label_count = 2;
        /* PWE label - leaving as is */
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_WITH_ID | BCM_MPLS_EGRESS_LABEL_REPLACE;
        /* Add MPLS label in the second place in the same EEDB entry */
        label_array[1].flags |= BCM_MPLS_EGRESS_LABEL_WITH_ID | BCM_MPLS_EGRESS_LABEL_REPLACE;
        label_array[1].flags |= BCM_MPLS_EGRESS_LABEL_TTL_SET|BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
        label_array[1].tunnel_id = pwe_encap_id;
        label_array[1].l3_intf_id = cint_mpls_various_scenarios_info.out_rif;
        label_array[1].label = 0;
        label_array[1].ttl = 20;

        rv = bcm_mpls_tunnel_initiator_create(unit, 0, label_count, label_array); /* 1 */
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_mpls_tunnel_initiator_create\n");
            return rv;
        }

    }

    /*PWE3*/
    mpls_port.encap_id = cint_mpls_various_scenarios_info.mpls_port_encap_id[2];
    BCM_GPORT_SUB_TYPE_LIF_SET(gport_id, 0, cint_mpls_various_scenarios_info.mpls_port_id_egress[2]);
    BCM_GPORT_MPLS_PORT_ID_SET(mpls_port.mpls_port_id, gport_id);
    mpls_port.egress_label.label = cint_mpls_various_scenarios_info.pwe_label[0];
    if (!is_device_or_above(unit, JERICHO2)) {
        mpls_port.egress_label.flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
    }

    mpls_port.egress_tunnel_if = cint_mpls_various_scenarios_info.mpls_tunnel_id[2];

    rv = bcm_mpls_port_add(unit, 0, mpls_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in bcm_mpls_port_add pwe2 encapsulation\n", rv);
        return rv;
    }
    cint_mpls_various_scenarios_info.mpls_port_id_egress[2] = mpls_port.mpls_port_id;
    cint_mpls_various_scenarios_info.mpls_port_encap_id[2] = mpls_port.encap_id;

    return rv;
}

/*
 * AC port create and multicast add
 */
int
mpls_various_scenarios_access_port_create(
    int unit,
    int port_id)
{
    int vpn = cint_mpls_various_scenarios_info.in_rif;
    int vid = cint_mpls_various_scenarios_info.out_rif;
    int rv = BCM_E_NONE;
    bcm_vlan_port_t vlan_port;
    bcm_vlan_port_t_init(&vlan_port);

    /*
     * set port attribures, key <port-vlan>
     */
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = port_id;
    vlan_port.match_vlan = vid;
    vlan_port.egress_vlan = is_device_or_above(unit, JERICHO2) ? 0 : vid;
    vlan_port.vsi = 0;
    vlan_port.flags = 0;

    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_port_create\n");
        return rv;
    }
    cint_mpls_various_scenarios_info.vlan_port_id = vlan_port.vlan_port_id;

    rv = bcm_vswitch_port_add(unit, vpn, vlan_port.vlan_port_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vswitch_port_add\n");
        return rv;
    }

    rv = bcm_vlan_gport_add(unit, vpn, port_id, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_gport_add \n");
        return rv;
    }

    return rv;
}


/*
 *static mac binding to mpls port
 */
int mpls_various_scenarios_l2_entry_create(
    int unit,
    int out_port)
{

    int rv;
    bcm_l2_addr_t l2_address;

    /*
     * pwe_mac[0] to mpls_port0+tunnel0
     */
    bcm_l2_addr_t_init(&l2_address,cint_mpls_various_scenarios_info.pwe_mac[0],cint_mpls_various_scenarios_info.in_rif);
    l2_address.flags = BCM_L2_STATIC;
    l2_address.port = out_port;

    l2_address.encap_id = cint_mpls_various_scenarios_info.mpls_port_encap_id[0];
    rv = bcm_l2_addr_add(unit,&l2_address);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add\n");
        return rv;
    }

    /*
     * pwe_mac[1] to mpls_port1+tunnel0
     */
    bcm_l2_addr_t_init(&l2_address,cint_mpls_various_scenarios_info.pwe_mac[1],cint_mpls_various_scenarios_info.in_rif);
    l2_address.flags = BCM_L2_STATIC;
    l2_address.port = out_port;
    l2_address.encap_id = cint_mpls_various_scenarios_info.mpls_port_encap_id[1];
    rv = bcm_l2_addr_add(unit,&l2_address);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add\n");
        return rv;
    }


    /*
     * pwe_mac[2] to mpls_port0+tunnel2
     */
    bcm_l2_addr_t_init(&l2_address,cint_mpls_various_scenarios_info.pwe_mac[2],cint_mpls_various_scenarios_info.in_rif);
    l2_address.flags = BCM_L2_STATIC;
    l2_address.encap_id = cint_mpls_various_scenarios_info.mpls_port_encap_id[2];
    l2_address.port = out_port;
    rv = bcm_l2_addr_add(unit,&l2_address);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add\n");
        return rv;
    }
    return rv;
}

/*
 * VPN create
 */
int
mpls_various_scenarios_vpn_create(int unit){

    bcm_mpls_vpn_config_t vpn_info;
    bcm_mpls_vpn_config_t_init(&vpn_info);
    vpn_info.vpn = cint_mpls_various_scenarios_info.in_rif;
    int mc_group;
    int rv;

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

    return rv;
}


/**
 * Main entrance for mpls various scenaroies
 * INPUT:
 *   in_port  - traffic incoming port
 *   out_port - traffic outgoing port
*/
 int
mpls_various_scenarios_main(
    int unit,
    int in_port,
    int out_port)
{
    int rv;


    rv = mpls_various_scenarios_vpn_create(unit);
    if (rv != BCM_E_NONE)
    {
         printf("Error, mpls_various_scenarios_vpn_create\n");
         return rv;
    }

    rv = mpls_various_scenarios_l3_intf_configuration(unit,in_port,out_port);
    if (rv != BCM_E_NONE)
    {
         printf("Error, mpls_various_scenarios_l3_intf_configuration\n");
         return rv;
    }

    rv = mpls_various_scenarios_tunnel_configuration(unit);
    if (rv != BCM_E_NONE)
    {
         printf("Error, mpls_various_scenarios_tunnel_configuration\n");
         return rv;
    }

    rv = mpls_various_scenarios_arp_configuration(unit);
    if (rv != BCM_E_NONE)
    {
         printf("Error, mpls_various_scenarios_arp_configuration\n");
         return rv;
    }


    rv = mpls_various_scenarios_fec_entry_configuration(unit,out_port);
    if (rv != BCM_E_NONE)
    {
         printf("Error, mpls_various_scenarios_fec_entry_configuration\n");
         return rv;
    }

    rv = mpls_various_scenarios_forward_entry_configuration(unit);
    if (rv != BCM_E_NONE)
    {
         printf("Error, mpls_check_ver_forward_entry_configuration\n");
         return rv;
    }

    rv = mpls_various_scenarios_access_port_create(unit,in_port);
    if (rv != BCM_E_NONE)
    {
         printf("Error, mpls_various_scenarios_access_port_create\n");
         return rv;
    }

    rv = mpls_various_scenarios_pwe_encapsulation(unit);
    if (rv != BCM_E_NONE)
    {
         printf("Error, mpls_various_scenarios_pwe_encapsulation\n");
         return rv;
    }

    rv = mpls_various_scenarios_l2_entry_create(unit,out_port);
    if (rv != BCM_E_NONE)
    {
         printf("Error, mpls_various_scenarios_l2_entry_create\n");
         return rv;
    }

    return rv;
}


