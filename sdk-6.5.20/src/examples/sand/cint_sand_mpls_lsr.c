/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Multi-Dev MPLS LSR~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_sand_mpls_lsr.c
 * Purpose: Demo MPLS LSR under multi device system
 *
 * Multi-Device system require the ingress/egress configuration separately and relatively.
 * This cint demo how to configurer the MPLS LSR service under multi-device system.
 *  1. Create ARP and out-RIF configuration on egress device
 *  2. Create In-RIF and ILM entry configuration on ingress device
 *  3. Send MPLS packet to verify the result
  *
 *  3 cases are tested:
 *   1.MPLSoEth---------->MPLSoEth(SWAP)
 *   2.MPLSoEth---------->MPLSoMPLSoEth(1 MPLS label pushed )
 *   3.MPLSoMPLSoEth---------->MPLSoEth(1 labels terminated, 1 MPLS label swapped by EEI from ILM)

 *  Test example:
 *
 * Test Scenario
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/dnx/field/cint_field_utils.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_mpls.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/sand/cint_sand_multi_device_ipv4_uc.c
 * cint ../../src/examples/sand/cint_sand_mpls_lsr.c
 * cint
 * multi_dev_mpls_lsr_swap_example(0,2,1811939531,1811939787);
 * mpls_lsr_mpls_tunnel_encap_configuration(0,2,1811939528);
 * mpls_lsr_tunnel_termination_entry_add(2,$term_label,0);
 * exit;

 *  AT_Sand_Cint_mpls_lsr:
 * tx 1 psrc=200 data=0x0000000000110000b35ff1c68100006488470138811bc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 * Received packets on unit 0 should be:
 * Data: 0x00000022000000000011810000c8884701f4011ac5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c50000
 *
 * AT_Sand_Cint_mpls_lsr_with_term
 * tx 1 psrc=200 data=0x000000000011000080212b08810000648847ddc00c1101388d11000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Received packets on unit 0 should be:
 * Data: 0x000000220000000000118100c0c8884701f40d10000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000
 *
 * AT_Sand_Cint_mpls_lsr_with_tunnel
 * tx 1 psrc=200 data=0x000000000011000092d7d934810000228847a089d529000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Received packets on unit 0 should be:
 * Data: 0x00000022000000000011810040628847bc750528000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000
 */


  struct cint_mpls_lsr_info_s
 {
     int dip_into_tunnel;                  /* dip into mpls tunnel from push command*/
     bcm_ip6_t dip_into_tunnel_ipv6;       /* IPv6 dip into mpls tunnel from push command*/
     int dip_into_tunnel_using_eedb;                     /* dip into mpls tunnel from EEI-pointer*/
     int incoming_label[3];               /* Incoming mpls label */
     int mpls_tunnel_label[2];            /* Incoming mpls label to termination */
     int outgoing_label[2];               /*  swapped mpls label  */
     int encap_label [4];                   /*  mpls tunnel label */
     int acces_index[3];                  /* mpls termination label index */
     int pipe_mode_exp_set;        /* exp mode in pipe */
     int exp;                                       /*  exp in pipe */
     int ttl;                                          /*  ttl in pipe */
     int php_into_mpls_out_lif;        /*EEDB pointer for php on non-bos*/
     int php_into_ipvx_out_lif;        /*EEDB pointer for php on bos*/

     int ingress_tunnel_id_indexed [3];  /* mpls indexed termination tunnel id */
     int ingress_tunnel_id;                        /* mpls termination tunnel id */

     int fec_id_for_arp;                  /*FEC ID for arp, for case 1,2,3,4*/
     int egr_mpls_tunnel_encap_id;     /* encap_id for mpls tunnel, for case 2*/
     int is_ingress_mpls_raw;             /* mpls raw service */
     int is_egress_mpls_raw;             /* mpls raw service */

 };


  cint_mpls_lsr_info_s cint_mpls_lsr_info=
 {
     /*
     * dip_into_tunnel
     */
     0x7FFFFF05, /* 127.255.255.5 */
    /*
     * dip_into_tunnel_IPv6
     */
     {0x02,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03},
     /*
     * dip_into_tunnel_using_eedb
     */
     0x7FFFFF06, /* 127.255.255.6 */

     /*
     * Incoming mpls label,
     */
     {5000,1001,1002}, /* swap, php */

     /*
     * mpls tunnel label,
     */
     {2000,2001}, /* termination*/

     /*
     * swapped label,
     */
     {8000,8001},
         /*
         * encap label,
         */
     {200,400,600,800},

     /*
     * mpls_termination_label_index
     */
     {1,2,3},
     /*
     * uniform or pipe mode for exp and ttl
     */
     0,         /*pipe_mode_exp_set*/
     5,         /*exp*/
     20,        /*TTL*/

     /*
      * PHP from EEDB
      */
     0x3EF80, /*outlif for php into mpls*/
     0x3EF81,  /*outlif for php into ipvx*/

     0          /*default isn't mapl raw service*/


 };

/* If a specific tunnel_id is required for POP entry, it can be set here */
int static_in_tunnel_id = -1;

/**
 * Initiation for system parameter
 * INPUT:
 *   ingress_unit- traffic incoming unit
 *   egress_unit- traffic ougtoing unit
 *   in_sys_port- traffic incoming port
 *   out_sys_port- traffic outgoing port
*/
 int
 sand_mpls_lsr_init(
    int ingress_unit,
    int egress_unit,
    int in_sys_port,
    int out_sys_port)
{
    int rv,is_qax;
    rv = multi_dev_ipv4_uc_init(ingress_unit,egress_unit,in_sys_port,out_sys_port);
    if (rv != BCM_E_NONE)
    {
         printf("Error, in multi_dev_ipv4_uc_init\n");
         return rv;
    }

    /*
     * FEC for ARP
     */
    rv = is_qumran_ax(ingress_unit, &is_qax);
    if (rv != BCM_E_NONE) {
        printf("Error, in is_qumran_ax \n");
        return rv;
    }
    if (is_qax) {
        BCM_L3_ITF_SET(cint_mpls_lsr_info.fec_id_for_arp, BCM_L3_ITF_TYPE_FEC, 0x5000);
    } else {
        BCM_L3_ITF_SET(cint_mpls_lsr_info.fec_id_for_arp, BCM_L3_ITF_TYPE_FEC, 0x10000);
    }

    /*
     * mpls tunnel EEDB
     */
    BCM_L3_ITF_SET(cint_mpls_lsr_info.egr_mpls_tunnel_encap_id,BCM_L3_ITF_TYPE_LIF, 0x3001);

    return rv;
}

void
mpls_lsr_init_aux(
    int in_label,
    int out_label,
    int encap_label,
    int encap_label2,
    int pipe_mode_exp_set,
    int exp,
    int in_vid,
    int out_vid)
{

    int rv = BCM_E_NONE;
    cint_mpls_lsr_info.incoming_label[0] = in_label? in_label : cint_mpls_lsr_info.incoming_label[0];
    cint_mpls_lsr_info.outgoing_label[0] = out_label ? out_label : cint_mpls_lsr_info.outgoing_label[0]; /*for swap*/
    cint_mpls_lsr_info.encap_label[0] = encap_label? encap_label : cint_mpls_lsr_info.encap_label[0]; /*for tunnel*/
    cint_mpls_lsr_info.encap_label[1] = encap_label2;
    cint_mpls_lsr_info.pipe_mode_exp_set = pipe_mode_exp_set ? pipe_mode_exp_set : cint_mpls_lsr_info.pipe_mode_exp_set;
    if (pipe_mode_exp_set)
    {
        cint_mpls_lsr_info.exp = exp;
    }
    cint_ipv4_route_info.in_rif = in_vid ? in_vid : cint_ipv4_route_info.in_rif;
    cint_ipv4_route_info.out_rif = out_vid ?out_vid :  cint_ipv4_route_info.out_rif;
}

/**
 * port egress init
 * INPUT:
 *   out_port - traffic outgoing port
*/
 int
mpls_lsr_egress_port_configuration(
    int unit,
    int out_sys_port)
{
    int rv, flags;

    /*
     * 1. Set Out-Port default properties, in case of ARP+AC no need
     */
    rv = out_port_set(unit, out_sys_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in out_port_set \n");
        return rv;
    }

    /*
     * 2. vlan create untag port added
     */
    flags = BCM_VLAN_GPORT_ADD_UNTAGGED;
    if (is_device_or_above(unit, JERICHO2)) {
        flags |= BCM_VLAN_GPORT_ADD_EGRESS_ONLY;
    }
    rv = bcm_vlan_create(unit, cint_ipv4_route_info.out_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_create\n");
        return rv;
    }
    rv = bcm_vlan_gport_add(unit, cint_ipv4_route_info.out_rif, out_sys_port, flags);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_gport_add\n");
        return rv;
    }

    return rv;

}


/**
 * FEC entry adding
 * INPUT:
 *   in_port    - traffic incoming port
 *   out_sys_port - traffic outgoing port
*/
 int
 mpls_lsr_fec_for_arp_configuration(
    int unit,
    int out_sys_port)
{
    int rv;
    sand_utils_l3_fec_s fec_structure;

    /*
     * FEC for ARP
     */
    sand_utils_l3_fec_s_init(unit, 0x0, &fec_structure);
    fec_structure.destination  = out_sys_port;
    fec_structure.tunnel_gport = cint_ipv4_route_info.arp_encap_id;
    fec_structure.allocation_flags = BCM_L3_WITH_ID;
    fec_structure.fec_id = cint_mpls_lsr_info.fec_id_for_arp;
    rv = sand_utils_l3_fec_create(unit, &fec_structure);

    if (rv != BCM_E_NONE)
    {
        printf("Error, in sand_utils_l3_fec_create\n");
        return rv;
    }

    /*For JR2 below device, PHP need installed*/
    if (!is_device_or_above(unit, JERICHO2)) {
        sand_utils_l3_fec_s_init(unit, 0x0, &fec_structure);
        fec_structure.destination  = out_sys_port;
        BCM_L3_ITF_SET(fec_structure.tunnel_gport,BCM_L3_ITF_TYPE_LIF, cint_ipv4_route_info.arp_encap_id);        
        fec_structure.allocation_flags =   BCM_L3_WITH_ID  | BCM_L3_INGRESS_ONLY;
        fec_structure.fec_id = cint_mpls_lsr_info.fec_id_for_arp + 1;
        rv = sand_utils_l3_fec_create(unit, &fec_structure);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in sand_utils_l3_fec_create\n");
            return rv;
        }   
    }

    return rv;
}

/**
 * forward entry adding(ILM)
 * INPUT:
 *   unit    - traffic incoming unit
 *   in_port  - traffic incoming port
 *   out_port - traffic outgoing port
 *   enable   - enable mpls raw port
*/
int
mpls_raw_port_setting(
    int unit,
    int in_port,
    int out_port,
    int enable)
{
    int rv;
    int header_type;

    header_type = enable ? BCM_SWITCH_PORT_HEADER_TYPE_MPLS_RAW : BCM_SWITCH_PORT_HEADER_TYPE_ETH;

    rv = bcm_switch_control_port_set(unit, in_port, bcmSwitchPortHeaderType, header_type);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_switch_control_port_set port:%d\n", in_port);
        return rv;
    }
    rv = bcm_switch_control_port_set(unit, out_port, bcmSwitchPortHeaderType, header_type);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_switch_control_port_set port:%d\n", in_port);
        return rv;
    }

    return rv;
}

/**
 * Set ports as mpls port
 * INPUT:
 *   unit    - traffic incoming unit
 *   in_sys_port  - traffic incoming port
 *   out_sys_port - traffic outgoing port
*/
 int
mpls_lsr_forward_entry_configuration(
    int unit,
    int in_sys_port, 
    int out_sys_port)
{
    int rv;
    bcm_mpls_tunnel_switch_t entry;

    /*
     * swap over MPLS
     */
    bcm_mpls_tunnel_switch_t_init(&entry);

    /** incoming label */
    entry.label = cint_mpls_lsr_info.incoming_label[0];
    entry.action = BCM_MPLS_SWITCH_ACTION_SWAP;
    entry.egress_label.label = cint_mpls_lsr_info.outgoing_label[0];
    entry.flags = BCM_MPLS_SWITCH_TTL_DECREMENT;

    if (cint_mpls_lsr_info.is_egress_mpls_raw)
    {
        /*DESTINATION(port) + EEI*/
        entry.egress_if = 0;
        entry.port = out_sys_port;
    }
    else 
    {
        /*DESTINATION(FEC) + EEI*/
        entry.egress_if = cint_mpls_lsr_info.fec_id_for_arp; /** FEC entry pointing to fec for arp */
    }
    /* This is relevant only in case of mpls context port */
    if (!is_device_or_above(unit, JERICHO2)){
        entry.port = in_sys_port;
    }
    rv = bcm_mpls_tunnel_switch_create(unit, &entry);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_tunnel_switch_create\n");
        return rv;
    }
     return rv;
}

/**
 * forward entry adding(ILM)
 * INPUT:
 *   unit    - traffic incoming unit
*/
 int
mpls_lsr_forward_entry_configuration_with_context(
    int unit,
    int out_sys_port,
    int rif)
{
    int rv, mpls_termination_label_index_enable;
    bcm_mpls_tunnel_switch_t entry;
    mpls_termination_label_index_enable = soc_property_get(unit, "mpls_termination_label_index_enable", 0);

    /*
     * swap over MPLS
     */
    bcm_mpls_tunnel_switch_t_init(&entry);

    /** incoming label */
    if (mpls_termination_label_index_enable) {
        BCM_MPLS_INDEXED_LABEL_SET(entry.label,cint_mpls_lsr_info.incoming_label[0],1);
    } else {
        entry.label = cint_mpls_lsr_info.incoming_label[0];
    }
    entry.action = BCM_MPLS_SWITCH_ACTION_SWAP;
    entry.egress_label.label = cint_mpls_lsr_info.outgoing_label[0];
    entry.flags = BCM_MPLS_SWITCH_TTL_DECREMENT;
    if(rif) {
       entry.ingress_if = rif;
       entry.flags |= BCM_MPLS_SWITCH_LOOKUP_L3_INGRESS_INTF;
    }
    /** FEC entry pointing to fec for arp */
    entry.egress_if = cint_mpls_lsr_info.fec_id_for_arp;
    rv = bcm_mpls_tunnel_switch_create(unit, &entry);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_tunnel_switch_create\n");
        return rv;
    }

    /*
    * Set Incoming ETH-RIF properties
    */
    if (is_device_or_above(unit, JERICHO2))
    {
        bcm_l3_ingress_t l3_ing_if;
        bcm_l3_ingress_t_init(&l3_ing_if);
        bcm_l3_ingress_get(unit, rif, l3_ing_if);
        if (rv != BCM_E_NONE)
        {
             printf("Error (%d), bcm_l3_ingress_get \n", rv);
             return rv;
        }
        l3_ing_if.flags = l3_ing_if.flags |BCM_L3_INGRESS_WITH_ID |BCM_L3_INGRESS_MPLS_INTF_NAMESPACE;
        rv = bcm_l3_ingress_create(unit,l3_ing_if, rif);
        if (rv != BCM_E_NONE)
        {
             printf("Error (%d), bcm_l3_ingress_create \n", rv);
             return rv;
        }
    }
    return rv;
}

/**
 * mpls tunnel termination entry adding
 * INPUT:
 *   unit    - traffic incoming unit
*/
 int
mpls_lsr_termination_entry_add(
    int unit,
    int term_label,
    uint32 next_prtcl)
{
    int rv, i, mpls_termination_label_index_enable,tunnel_id,access_levels;
    int flags = 0;
    bcm_mpls_tunnel_switch_t entry;

    /*
     * read mpls index soc property
     */
    mpls_termination_label_index_enable = soc_property_get(unit, "mpls_termination_label_index_enable", 0);

    bcm_mpls_tunnel_switch_t_init(&entry);
    entry.action = BCM_MPLS_SWITCH_ACTION_POP;
    entry.flags |= flags;

    /*
     * Uniform: inherit TTL and EXP,
     * in general valid options:
     * both present (uniform) or none of them (Pipe)
     */
    if (is_device_or_above(unit, JERICHO2))
    {
        entry.ingress_qos_model.ingress_phb = bcmQosIngressModelUniform;
        entry.ingress_qos_model.ingress_remark = bcmQosIngressModelUniform;
        entry.ingress_qos_model.ingress_ttl = bcmQosIngressModelUniform;
    }
    else
    {
        entry.flags |= BCM_MPLS_SWITCH_OUTER_TTL | BCM_MPLS_SWITCH_OUTER_EXP;
    }

    entry.flags |= next_prtcl;

    if (mpls_termination_label_index_enable) {

        if (cint_mpls_lsr_info.acces_index[2] != cint_mpls_lsr_info.acces_index[1])  {
            access_levels = 3;
        } else {
            access_levels = 2;
        }
        for (i = 0; i < access_levels; i++)
        {
            BCM_MPLS_INDEXED_LABEL_SET(entry.label, term_label, cint_mpls_lsr_info.acces_index[i]);

            rv = bcm_mpls_tunnel_switch_create(unit, &entry);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in bcm_mpls_tunnel_switch_create\n");
                return rv;
            }
            cint_mpls_lsr_info.ingress_tunnel_id_indexed[i] = entry.tunnel_id;
        }
    } else {
        entry.label = term_label;
        if (static_in_tunnel_id != -1)
        {
            entry.flags = BCM_MPLS_SWITCH_WITH_ID;
            entry.tunnel_id = static_in_tunnel_id;
        }
        rv = bcm_mpls_tunnel_switch_create(unit, &entry);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_mpls_tunnel_switch_create\n");
            return rv;
        }
        cint_mpls_lsr_info.ingress_tunnel_id = entry.tunnel_id;
    }

    return rv;
}


/*
 * l3 FEC replace
 */
int
mpls_lsr_l3_egress_fec_replace(
    int unit,
    int out_sys_port)
{
    int rv;
    sand_utils_l3_fec_s fec_structure;

    sand_utils_l3_fec_s_init(unit, 0x0, &fec_structure);
    fec_structure.destination  = out_sys_port;
    fec_structure.tunnel_gport = cint_mpls_lsr_info.egr_mpls_tunnel_encap_id;
    fec_structure.allocation_flags =   BCM_L3_WITH_ID | BCM_L3_REPLACE | BCM_L3_INGRESS_ONLY;
    fec_structure.fec_id = cint_mpls_lsr_info.fec_id_for_arp;
    rv = sand_utils_l3_fec_create(unit, &fec_structure);

    if (rv != BCM_E_NONE)
    {
        printf("Error, in sand_utils_l3_fec_create\n");
        return rv;
    }

    /*For JR2 below device, PHP need installed*/
    if (!is_device_or_above(unit, JERICHO2)) {
        sand_utils_l3_fec_s_init(unit, 0x0, &fec_structure);
        fec_structure.destination  = out_sys_port;
        BCM_L3_ITF_SET(fec_structure.tunnel_gport,BCM_L3_ITF_TYPE_LIF, cint_mpls_lsr_info.php_into_ipvx_out_lif);        
        fec_structure.allocation_flags =   BCM_L3_WITH_ID  | BCM_L3_REPLACE | BCM_L3_INGRESS_ONLY;
        fec_structure.fec_id = cint_mpls_lsr_info.fec_id_for_arp + 1;
        rv = sand_utils_l3_fec_create(unit, &fec_structure);

    }


    return rv;

}

/*
 * l3 ARP replace
 */
int
mpls_lsr_l3_egress_arp_replace(
    int unit)
{
    int rv;
    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid_null;       /* not intersting */
    uint32 flags = BCM_L3_WITH_ID | BCM_L3_REPLACE | BCM_L3_EGRESS_ONLY | BCM_L3_KEEP_DSTMAC | BCM_L3_KEEP_VLAN;

    bcm_l3_egress_t_init(&l3eg);

    l3eg.encap_id = cint_ipv4_route_info.arp_encap_id;
    l3eg.intf = cint_mpls_lsr_info.egr_mpls_tunnel_encap_id;

    rv = bcm_l3_egress_create(unit, flags, &l3eg, &l3egid_null);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_l3_egress_create\n");
        return rv;
    }
    printf("Created ARP-id = 0x%08x, allocation_flags = 0x%08x , flags = 0x%08x, flags2 = 0x%08x \n", l3eg.encap_id, flags, l3eg.flags, l3eg.flags2);

    return rv;

}


/**
 * MPLS tunnel creating for label push
*/
int
mpls_lsr_encap_entry_create(
    int unit,
    uint32 force_flags)
{
    int rv, num_of_labels, i;
    bcm_mpls_egress_label_t label_array[4];

    if (cint_mpls_lsr_info.encap_label[1] ) {
        num_of_labels = 2;
    } else {
        num_of_labels = 1;
    }

    if (!is_device_or_above(unit, JERICHO2) ) {
        rv = bcm_switch_control_set(unit, bcmSwitchMplsPipeTunnelLabelExpSet, cint_mpls_lsr_info.pipe_mode_exp_set);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_switch_control_set\n");
            return rv;
        }
    }

    for (i = 0; i <= num_of_labels; i++)
    {
        bcm_mpls_egress_label_t_init(&label_array[i]);
        label_array[i].flags = force_flags;
        if (!is_device_or_above(unit, JERICHO2)) {
            label_array[i].l3_intf_id = cint_ipv4_route_info.out_rif;
        } else {
            label_array[i].l3_intf_id = 0;
        }
        label_array[i].label = cint_mpls_lsr_info.encap_label[i];

        if((!is_device_or_above(unit,ARAD_PLUS)) || cint_mpls_lsr_info.pipe_mode_exp_set ) {
            if (is_device_or_above(unit, JERICHO2)) {
                label_array[i].egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
            } else {
                label_array[i].flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET  ;
            }
            label_array[i].exp = cint_mpls_lsr_info.exp;
        } else {
            if (is_device_or_above(unit, JERICHO2)) {
                label_array[i].egress_qos_model.egress_qos = bcmQosEgressModelUniform;
            } else {
                label_array[i].flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY  ;
            }
        }
        label_array[i].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
        if (is_device_or_above(unit, JERICHO2)) {

            label_array[i].egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
        } else {
            label_array[i].flags |= BCM_MPLS_EGRESS_LABEL_TTL_SET;
        }
        /* In JR2 two labels in one eedb entry label_1_ttl should be same with label_2_ttl*/
        if (!is_device_or_above(unit, JERICHO2) && i) {
            label_array[i].ttl = 40;
        } else {
            label_array[i].ttl = cint_mpls_lsr_info.ttl;
        }

        if (!is_device_or_above(unit, JERICHO2)) {
            label_array[i].action = BCM_MPLS_EGRESS_ACTION_PUSH;
            label_array[i].flags |= BCM_MPLS_EGRESS_LABEL_ACTION_VALID;
        }
        label_array[i].tunnel_id = cint_mpls_lsr_info.egr_mpls_tunnel_encap_id;
        label_array[i].flags |= BCM_MPLS_EGRESS_LABEL_WITH_ID;
    }
    /** Create the mpls tunnel*/
    rv = sand_mpls_tunnel_initiator_create(unit, 0, num_of_labels, label_array);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in sand_mpls_tunnel_initiator_create\n");
        return rv;
    }

    return rv;
}
/**
 * MPLS tunnel creating for php
 * INPUT:
*/
int
mpls_lsr_eedb_php_entry_create(
    int unit,
    uint32 force_flags)
{
    int rv, num_of_labels, i;
    bcm_mpls_egress_label_t egr_label;

    bcm_mpls_egress_label_t_init(&egr_label);
    egr_label.flags = force_flags;
    egr_label.l3_intf_id = cint_ipv4_route_info.arp_encap_id;
    if (!is_device_or_above(unit, JERICHO2)) {
        egr_label.label = cint_mpls_lsr_info.incoming_label[1];
        egr_label.flags |=BCM_MPLS_EGRESS_LABEL_TTL_COPY|BCM_MPLS_EGRESS_LABEL_EXP_COPY;
    } else {
        egr_label.label = BCM_MPLS_LABEL_INVALID;
    }
    egr_label.flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT ;

    egr_label.action = BCM_MPLS_EGRESS_ACTION_PHP;

    /*
     * NOTe:Only under IOP mode, 
     * PHP from EEDB should use dedicated LIF range.
     * to simplify the configuration, we align it under
     * all mode
     */
    BCM_L3_ITF_SET(cint_mpls_lsr_info.egr_mpls_tunnel_encap_id,BCM_L3_ITF_TYPE_LIF, cint_mpls_lsr_info.php_into_mpls_out_lif);

    egr_label.tunnel_id = cint_mpls_lsr_info.egr_mpls_tunnel_encap_id;
    egr_label.flags |= BCM_MPLS_EGRESS_LABEL_WITH_ID |BCM_MPLS_EGRESS_LABEL_ACTION_VALID;

    /** Create the mpls tunnel*/
    rv = sand_mpls_tunnel_initiator_create(unit, 0, 1, egr_label);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in sand_mpls_tunnel_initiator_create\n");
        return rv;
    }

    bcm_mpls_egress_label_t_init(&egr_label);
    egr_label.flags = force_flags;
    egr_label.l3_intf_id = cint_ipv4_route_info.arp_encap_id;
    if (!is_device_or_above(unit, JERICHO2)) {
        egr_label.label = cint_mpls_lsr_info.incoming_label[1];
        egr_label.flags |=BCM_MPLS_EGRESS_LABEL_TTL_COPY|BCM_MPLS_EGRESS_LABEL_EXP_COPY;
    } else {
        egr_label.label = BCM_MPLS_LABEL_INVALID;
    }
    egr_label.flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT |BCM_MPLS_EGRESS_LABEL_PHP_IPV4;

    egr_label.action = BCM_MPLS_EGRESS_ACTION_PHP;

    /*
     * NOTe:Only under IOP mode, 
     * PHP from EEDB should use dedicated LIF range.
     * to simplify the configuration, we align it under
     * all mode
     */
    BCM_L3_ITF_SET(egr_label.tunnel_id,BCM_L3_ITF_TYPE_LIF, cint_mpls_lsr_info.php_into_ipvx_out_lif);

    egr_label.flags |= BCM_MPLS_EGRESS_LABEL_WITH_ID |BCM_MPLS_EGRESS_LABEL_ACTION_VALID;

    /** Create the mpls tunnel*/
    rv = sand_mpls_tunnel_initiator_create(unit, 0, 1, egr_label);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in sand_mpls_tunnel_initiator_create\n");
        return rv;
    }

    return rv;
}


/**
 * field for smart PHP
 * In JR1, PHP solution use PMF to recognize
 * PHP into IPvX/MPLS.
 * INPUT:
*/
int
mpls_lsr_eedb_php_field_install(
    int unit
)
{
    int rv = 0;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_group_t grp;
    bcm_field_entry_t ent;
    bcm_field_presel_set_t psset;
    int presel_flags = BCM_FIELD_QUALIFY_PRESEL;
    bcm_gport_t gport=0;

    /*JR2 is part of appl reference*/
    if (is_device_or_above(unit, JERICHO2)) {
        return rv;
    }

    bcm_field_presel_create_id(unit, 0);
    bcm_field_qualify_ForwardingType(unit, 0 | presel_flags, bcmFieldForwardingTypeMpls);
    BCM_FIELD_PRESEL_INIT(psset);
    BCM_FIELD_PRESEL_ADD(psset, 0);

    /* Define the QSET */
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDstL3Egress);


    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyMplsForwardingLabelBos);

    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionRedirect);

    rv = bcm_field_group_create(unit, qset, BCM_FIELD_GROUP_PRIO_ANY, &grp);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_group_create\n");
        return rv;
    }

    /* Attach a Preselection set */
    rv = bcm_field_group_presel_set(unit, grp, &psset);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_group_create\n");
        return rv;
    }

    /*  Attach the action set */
    rv = bcm_field_group_action_set(unit, grp, aset);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_group_action_set\n");
        return rv;
    }


    rv = bcm_field_entry_create(unit, grp, &ent);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_entry_create\n");
        return rv;
    }

    bcm_field_qualify_MplsForwardingLabelBos(unit, ent, 1, 0x1);
    BCM_L3_ITF_SET(gport,BCM_L3_ITF_TYPE_FEC,cint_mpls_lsr_info.fec_id_for_arp);    
    bcm_field_qualify_DstL3Egress(unit, ent, gport);

    BCM_GPORT_FORWARD_PORT_SET(gport, cint_mpls_lsr_info.fec_id_for_arp + 1);
    rv = bcm_field_action_add(unit, ent, bcmFieldActionRedirect, 0, gport);   
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_field_action_add\n");
        return rv;
    }

    rv = bcm_field_entry_install(unit, ent);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_field_entry_install\n");
        return rv;
    }


    return rv;    
}


/**
 * MPLS ILM PHP
*/
 int
mpls_lsr_ilm_php_entry_create(
    int unit,
    int out_sys_port)
{
    int rv;
    bcm_mpls_tunnel_switch_t entry;
    bcm_mpls_tunnel_switch_t_init(&entry);

    /** incoming label */
    entry.label = cint_mpls_lsr_info.incoming_label[1];
    entry.action = BCM_MPLS_SWITCH_ACTION_PHP;
    entry.flags = BCM_MPLS_SWITCH_TTL_DECREMENT;
    /*
     *TTL/EXP as uniform mode
     *JR2 uses ingress_qos_model to replace of flags for qos propagation
     */
    if (is_device_or_above(unit, JERICHO2)) {
        entry.ingress_qos_model.ingress_phb = bcmQosIngressModelUniform;
        entry.ingress_qos_model.ingress_remark = bcmQosIngressModelUniform;
        entry.ingress_qos_model.ingress_ttl = bcmQosIngressModelUniform;
    } else {
        entry.flags |= BCM_MPLS_SWITCH_OUTER_TTL | BCM_MPLS_SWITCH_OUTER_EXP;

    }
    if ((!soc_property_get(unit, "system_headers_mode", 1) && is_device_or_above(unit, JERICHO2))
        ||!is_device_or_above(unit, JERICHO2) )
    {
        entry.flags |= BCM_MPLS_SWITCH_NEXT_HEADER_IPV4;
    }
    entry.egress_if = cint_mpls_lsr_info.fec_id_for_arp;
    rv = bcm_mpls_tunnel_switch_create(unit, &entry);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_tunnel_switch_create\n");
        return rv;
    }

    return rv;
}

/**
 * MPLS tunnel adding and update FEC point to tunnel
 * INPUT:
*/
int
mpls_lsr_tunnel_encap_configuration(
    int ingress_unit,
    int egress_unit,
    int out_sys_port)
{
    int rv;
    int flags = 0 ;

    /*
     * step 1: common tunnel.
     */
    rv =  mpls_lsr_encap_entry_create(egress_unit,flags);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_lsr_egress_tunnel_create\n");
        return rv;
    }

    /*
     * step 2: update FEC point to tunnel, tunnel point to ARP.
     */
    rv = mpls_lsr_l3_egress_arp_replace(egress_unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_lsr_l3_egress_arp_replace \n");
        return rv;
    }

    rv = mpls_lsr_l3_egress_fec_replace(ingress_unit, out_sys_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_lsr_l3_egress_fec_replace \n");
        return rv;
    }

    return rv;
}

 /**
  * Port, RIF and ARP configuration for mpls application
 */
 int
 mpls_lsr_l2_l3_configuration(
    int ingress_unit,
    int egress_unit,
    int in_sys_port,
    int out_sys_port)
{
    int rv;

    rv = sand_mpls_lsr_init(ingress_unit,egress_unit,in_sys_port,out_sys_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, sand_mpls_lsr_init\n");
     return rv;
    }

    /*
     * step 1.ingress L2/L3 configuration(Port,In-RIF)
     */
    rv = bcm_vlan_create(ingress_unit,cint_ipv4_route_info.in_rif);
     if (rv != BCM_E_NONE)
    {
     printf("Error, bcm_vlan_create\n");
     return rv;
    }
    rv = multi_dev_ingress_port_configuration(ingress_unit,in_sys_port,cint_ipv4_route_info.in_rif);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_ingress_port_configuration\n");
     return rv;
    }

    rv = multi_dev_ipv4_uc_l3_intf_configuration(ingress_unit,in_sys_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_ipv4_uc_l3_intf_configuration\n");
     return rv;
    }
    printf("0\n");

    /*
     * step 2. ingress FEC configuration
     */
    rv = mpls_lsr_fec_for_arp_configuration(ingress_unit,out_sys_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, mpls_lsr_fec_for_arp_configuration\n");
     return rv;
    }

    /*
     * step 3. egress L2/L3 configuration(Port, Out-RIF, ARP)
     */

    rv = mpls_lsr_egress_port_configuration(egress_unit,out_sys_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, mpls_lsr_egress_port_configuration\n");
     return rv;
    }

    rv = multi_dev_ipv4_uc_l3_intf_configuration(egress_unit,cint_ipv4_route_info.out_local_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_ipv4_uc_l3_intf_configuration\n");
     return rv;
    }

    rv = multi_dev_ipv4_uc_arp_configuration(egress_unit,cint_ipv4_route_info.out_local_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_ipv4_uc_arp_configuration\n");
     return rv;
    }

    return rv;

}

/**
 * MPLS LSR SWAP on multi-device
 * INPUT:
 *   in_sys_port	- traffic incoming port
 *   out_sys_port - traffic outgoing port
*/
 int
 multi_dev_mpls_lsr_swap_example(
    int ingress_unit,
    int egress_unit,
    int in_sys_port,
    int out_sys_port)
{
    int rv;

    /*
     * step 1.port and rif configuration
     */
    rv = mpls_lsr_l2_l3_configuration(ingress_unit,egress_unit, in_sys_port,out_sys_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, mpls_lsr_l2_l3_configuration\n");
     return rv;
    }

    /*
     * step 2. ingress FWD configuration
     */
    rv = mpls_lsr_forward_entry_configuration(ingress_unit,in_sys_port, out_sys_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, mpls_lsr_forward_entry_configuration\n");
     return rv;
    }

    return rv;

}

/**
 * MPLS LSR SWAP on single-device
 * INPUT:
 *   unit - test unit
 *   in_port - traffic incoming port
 *   out_port - traffic outgoing port
*/
 int
 single_dev_mpls_lsr_swap_example(
    int unit,
    int in_port,
    int out_port)
{
    int rv;


    if (cint_mpls_lsr_info.is_ingress_mpls_raw && cint_mpls_lsr_info.is_egress_mpls_raw)
    {
        /*
         * step 1.Initiation system parameter
         */
        rv = sand_mpls_lsr_init(unit, unit, in_port, out_port);
        if (rv != BCM_E_NONE)
        {
            printf("Error, sand_mpls_lsr_init\n");
            return rv;
        }
    }
    else
    {
        /*
         * step 1.Initiation system parameter, port and rif configuration
         */
        rv = mpls_lsr_l2_l3_configuration(unit, unit, in_port,out_port);
        if (rv != BCM_E_NONE)
        {
            printf("Error, mpls_lsr_l2_l3_configuration\n");
            return rv;
        }
    }

    /*
     * step 2. ingress FWD configuration
     */
    rv = mpls_lsr_forward_entry_configuration(unit,in_port, out_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, mpls_lsr_forward_entry_configuration\n");
        return rv;
    }

    /*
     * step 3. Set in/out ports as mpls raw port
     */
    if (cint_mpls_lsr_info.is_ingress_mpls_raw)
    {
        rv = mpls_raw_port_setting(unit, in_port, out_port, 1);
    }

    return rv;

}

/**
 * MPLS LSR SWAP on multi-device
 * INPUT:
 *   in_sys_port	- traffic incoming port
 *   out_sys_port - traffic outgoing port
*/
 int
 multi_dev_mpls_lsr_swap_with_context_example(
    int ingress_unit,
    int egress_unit,
    int in_sys_port,
    int out_sys_port)
{
    int rv, in_rif, out_rif;

    rv = sand_mpls_lsr_init(ingress_unit,egress_unit,in_sys_port,out_sys_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, sand_mpls_lsr_init\n");
     return rv;
    }

    /*
     * step 1.ingress L2/L3 configuration(Port,In-RIF)
     */
    in_rif = cint_ipv4_route_info.in_rif;
    rv = bcm_vlan_create(ingress_unit, in_rif);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_create with vlan %d continue \n", in_rif);
        print rv;
    }

    /* add port to vlan */
    rv = bcm_vlan_gport_add(ingress_unit, in_rif, in_sys_port, BCM_VLAN_GPORT_ADD_INGRESS_ONLY);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_add, vlan=%d, \n", in_rif);
        return rv;
    }

    out_rif = cint_ipv4_route_info.out_rif;
    rv = bcm_vlan_create(ingress_unit, out_rif);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_create with vlan %d continue \n", out_rif);
        print rv;
    }

    /* add port to vlan  */
    rv = bcm_vlan_gport_add(ingress_unit, out_rif, in_sys_port, BCM_VLAN_GPORT_ADD_INGRESS_ONLY);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_gport_add, vlan=%d, \n", out_rif);
        print rv;
        return rv;
    }

    rv = multi_dev_ipv4_uc_l3_intf_configuration(ingress_unit,in_sys_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_ipv4_uc_l3_intf_configuration\n");
     return rv;
    }

    /*
     * step 2.ingress FEC configuration
     */
    rv = mpls_lsr_fec_for_arp_configuration(ingress_unit,out_sys_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, mpls_lsr_fec_for_arp_configuration\n");
     return rv;
    }

    /*
     * step 3. egress L2/L3 configuration(Port, Out-RIF, ARP)
     */
    if (ingress_unit != egress_unit)
    {
        rv = bcm_vlan_create(egress_unit, out_rif);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_create with vlan %d continue \n", out_rif);
        }
    }
    /* add port to vlan  */
    rv = bcm_vlan_gport_add(egress_unit, out_rif, out_sys_port, BCM_VLAN_GPORT_ADD_EGRESS_ONLY);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_gport_add, vlan=%d, \n", out_rif);
        return rv;
    }

    rv = multi_dev_ipv4_uc_l3_intf_configuration(egress_unit,cint_ipv4_route_info.out_local_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_ipv4_uc_l3_intf_configuration\n");
     return rv;
    }

    rv = multi_dev_ipv4_uc_arp_configuration(egress_unit,cint_ipv4_route_info.out_local_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_ipv4_uc_arp_configuration\n");
     return rv;
    }

    /*
     * step 4. ingress FWD configuration
     */
    rv = mpls_lsr_forward_entry_configuration_with_context(ingress_unit,out_sys_port,cint_ipv4_route_info.in_rif);
    if (rv != BCM_E_NONE)
    {
     printf("Error, mpls_lsr_forward_entry_configuration_with_context\n");
     return rv;
    }

    return rv;

}

/**
 *  MPLS LSR PHP on multi-device
 * INPUT:
 *   in_sys_port	- traffic incoming port
 *   out_sys_port - traffic outgoing port
*/
 int
 multi_dev_mpls_lsr_ilm_php_example(
    int ingress_unit,
    int egress_unit,
    int in_sys_port,
    int out_sys_port)
{
    int rv;

    /*
     * step 1.port and rif configuration
     */
    rv = mpls_lsr_l2_l3_configuration(ingress_unit,egress_unit, in_sys_port,out_sys_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, mpls_lsr_l2_l3_configuration\n");
     return rv;
    }

    /*
     * step 2. ingress FWD configuration
     */
    rv = mpls_lsr_ilm_php_entry_create(ingress_unit,out_sys_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, mpls_lsr_ilm_php_entry_create\n");
     return rv;
    }

    return rv;

}
/**
 *  MPLS LSR PHP on multi-device
 * INPUT:
 *   in_sys_port	- traffic incoming port
 *   out_sys_port - traffic outgoing port
*/
 int
 multi_dev_mpls_lsr_eedb_php_example(
    int ingress_unit,
    int egress_unit,
    int in_sys_port,
    int out_sys_port)
{
    int rv;


    /*
     * step 1.port and rif configuration
     */
    rv = mpls_lsr_l2_l3_configuration(ingress_unit,egress_unit, in_sys_port,out_sys_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, mpls_lsr_l2_l3_configuration\n");
     return rv;
    }

    /*
     * step 2. EEDB entry configuration
     */
    rv = mpls_lsr_eedb_php_entry_create(egress_unit,0);
    if (rv != BCM_E_NONE)
    {
     printf("Error, mpls_lsr_eedb_php_entry_create\n");
     return rv;
    }

    /*
     * step 3: update tunnel point to ARP,FEC point to tunnel
     */
    rv = mpls_lsr_l3_egress_arp_replace(egress_unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_lsr_l3_egress_arp_replace \n");
        return rv;
    }
    rv = mpls_lsr_l3_egress_fec_replace(ingress_unit, out_sys_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_lsr_l3_egress_fec_replace \n");
        return rv;
    }

    /*
     * step 4. Create the MPLS LSR entry
     */
    bcm_mpls_tunnel_switch_t entry;
    bcm_mpls_tunnel_switch_t_init(&entry);
    entry.action = BCM_MPLS_SWITCH_ACTION_NOP;
    entry.flags = BCM_MPLS_SWITCH_TTL_DECREMENT;
    if (!is_device_or_above(ingress_unit, JERICHO2))
    {
        entry.flags |= BCM_MPLS_SWITCH_OUTER_TTL|BCM_MPLS_SWITCH_OUTER_EXP;
    }

    entry.label = cint_mpls_lsr_info.incoming_label[1];
    entry.egress_if = cint_mpls_lsr_info.fec_id_for_arp;
    rv = bcm_mpls_tunnel_switch_create(ingress_unit, &entry);
    if (BCM_E_NONE != rv) {
      printf("Error in bcm_mpls_tunnel_switch_create %x\n",rv);
      return rv;
    }

    /*
     * step 5. install PHP
     */
    rv = mpls_lsr_eedb_php_field_install(ingress_unit);
    if (rv != BCM_E_NONE)
    {
     printf("Error, mpls_lsr_eedb_php_field_install\n");
     return rv;
    }

    return rv;

}

/**
 * IPv4 route to tunnel with entropy
 * INPUT:
 *   in_sys_port	- traffic incoming port
 *   out_sys_port - traffic outgoing port
*/
 int
 multi_dev_mpls_ip_route_tunnel_example(
    int ingress_unit,
    int egress_unit,
    int in_sys_port,
    int out_sys_port,
    int with_entropy)
{

    int rv = BCM_E_NONE;
    int tunnel_force_flags = 0;
    if (with_entropy)
    {
         tunnel_force_flags= BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE |BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE;
    }

    /*
     * step 1: create in and out rif
     */
    rv = mpls_lsr_l2_l3_configuration(ingress_unit, egress_unit,in_sys_port,out_sys_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in mpls_lsr_l2_l3_configuration\n", rv);
        return rv;
    }

    /*
     * step 2: tunnel with entropy
     */
     rv =  mpls_lsr_encap_entry_create(egress_unit,tunnel_force_flags);
     if (rv != BCM_E_NONE)
     {
         printf("Error (%d), in mpls_lsr_encap_entry_create\n", rv);
         return rv;
     }

     /*
      * step 3: update tunnel point to ARP,FEC point to tunnel
      */
     rv = mpls_lsr_l3_egress_arp_replace(egress_unit);
     if (rv != BCM_E_NONE)
     {
         printf("Error, in mpls_lsr_l3_egress_arp_replace \n");
         return rv;
     }
     rv = mpls_lsr_l3_egress_fec_replace(ingress_unit, out_sys_port);
     if (rv != BCM_E_NONE)
     {
         printf("Error, in mpls_lsr_l3_egress_fec_replace \n");
         return rv;
     }

    /*
     * step 4: push over IPv4
     */
    rv = add_host_ipv4(ingress_unit, cint_mpls_lsr_info.dip_into_tunnel, cint_ipv4_route_info.vrf, cint_mpls_lsr_info.fec_id_for_arp, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in add_host_ipv4\n", rv);
        return rv;
    }

    /*
     * step 5: push over IPv6
     */
    rv = add_host_ipv6(ingress_unit, cint_mpls_lsr_info.dip_into_tunnel_ipv6, cint_ipv4_route_info.vrf, cint_mpls_lsr_info.fec_id_for_arp);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in add_host_ipv6\n", rv);
        return rv;
    }
    return rv;

}
/**
 * IPv4 route to tunnel with qos
 * INPUT:
 *   in_sys_port	- traffic incoming port
 *   out_sys_port - traffic outgoing port
*/
 int
 multi_dev_mpls_ip_route_tunnel_qos_example(
    int ingress_unit,
    int egress_unit,
    int in_sys_port,
    int out_sys_port)
{

    int rv = BCM_E_NONE;

    /*
     * step 1: ingress l3 qos remarking and PHB
     */
    if (is_device_or_above(ingress_unit, JERICHO2) ) {
        rv = dnx_qos_map_l3_ingress_profile(ingress_unit);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in dnx_qos_map_l3_ingress_profile \n", rv);
            return rv;
        }
    } else {
        rv = dpp_qos_map_l3_ingress_profile(ingress_unit);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in dpp_qos_map_l3_ingress_profile \n", rv);
            return rv;
        }

    }


    /*
     * step 2: egress mpls qos remarking and marking
     */
    if (is_device_or_above(egress_unit, JERICHO2) ) {
        rv = dnx_qos_map_mpls_egress_profile(egress_unit);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in dnx_qos_map_mpls_egress_profile \n", rv);
            return rv;
        }
    } else {
        rv = dpp_qos_map_mpls_egress_encap_profile(egress_unit);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in dpp_qos_map_mpls_egress_encap_profile \n", rv);
            return rv;
        }

    }


    /*
     * step 3: ip route to tunnel
     */
    rv = multi_dev_mpls_ip_route_tunnel_example(ingress_unit, egress_unit,in_sys_port,out_sys_port,0);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in multi_dev_mpls_ip_route_tunnel_example\n", rv);
        return rv;
    }

    /*
     * step 4: update ingress qos map
     */
    /* get the configuration of the InRIF. */
    bcm_l3_ingress_t l3i;
    rv = bcm_l3_ingress_get(ingress_unit, cint_ipv4_route_info.in_rif, &l3i);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_l3_ingress_get\n", rv);
        return rv;
    }

    /* attch the ingress QoS profile. */
    l3i.qos_map_id = l3_in_map_id;
    l3i.flags |= BCM_L3_INGRESS_WITH_ID;

    /* set the configuration of the InRIF. */
    rv = bcm_l3_ingress_create(ingress_unit, l3i, cint_ipv4_route_info.in_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_l3_ingress_create\n", rv);
        return rv;
    }

    /*
     * step 5: update egress qos map
     */
    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid_null;       /* not intersting */
    uint32 flags = BCM_L3_WITH_ID | BCM_L3_REPLACE | BCM_L3_EGRESS_ONLY ;

    bcm_l3_egress_t_init(&l3eg);
    rv = bcm_l3_egress_get(egress_unit, cint_ipv4_route_info.arp_encap_id, &l3eg);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l3_egress_get\n");
        return rv;
    }

    l3eg.qos_map_id = mpls_eg_map_id;
    l3eg.encap_id = cint_ipv4_route_info.arp_encap_id;

    rv = bcm_l3_egress_create(egress_unit, flags, &l3eg, &l3egid_null);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_l3_egress_create\n");
        return rv;
    }
    return rv;

}
/*
  * qos map cleanup
  */
int
multi_dev_mpls_ip_route_tunnel_qos_clean (
    int ingress_unit,
    int egress_unit)
{
    if (is_device_or_above(ingress_unit, JERICHO2) ) {
        print bcm_qos_map_destroy(ingress_unit, l3_in_map_id);
        print bcm_qos_map_destroy(ingress_unit, l3_qos_in_internal_map_id_phb);
        print bcm_qos_map_destroy(ingress_unit, l3_qos_in_internal_map_id_remark);
    } else {
        print bcm_qos_map_destroy(ingress_unit, l3_in_map_id);
    }
    if (is_device_or_above(egress_unit, JERICHO2) ) {
        print bcm_qos_map_destroy(egress_unit, mpls_qos_eg_internal_map_id);
        print bcm_qos_map_destroy(egress_unit, mpls_eg_map_id);
    } else {
        print bcm_qos_map_destroy(egress_unit, mpls_eg_map_id);
    }
}

