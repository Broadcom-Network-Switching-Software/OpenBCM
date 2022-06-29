/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Multi-Dev MPLS LSR~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
 * File: cint_dnx_mpls_lsr.c
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
 * cint ../../src/examples/dnx/utility/cint_dnx_utils_l3.c
 * cint ../../src/examples/dnx/utility/cint_dnx_utility_mpls.c
 * cint ../../src/examples/dnx/l3/cint_dnx_ip_route_basic.c
 * cint ../../src/examples/dnx/mpls/cint_dnx_mpls_lsr.c
 * cint
 * multi_dev_mpls_lsr_swap_example(0,2,1811939531,1811939787);
 * mpls_lsr_mpls_tunnel_encap_configuration(0,2,1811939528);
 * mpls_lsr_tunnel_termination_entry_add(2,$term_label,0);
 * exit;

 *  AT_Dnx_Cint_mpls_lsr:
 * tx 1 psrc=200 data=0x0000000000110000b35ff1c68100006488470138811bc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 * Received packets on unit 0 should be:
 * Data: 0x00000022000000000011810000c8884701f4011ac5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c50000
 *
 * AT_Dnx_Cint_mpls_lsr_with_term
 * tx 1 psrc=200 data=0x000000000011000080212b08810000648847ddc00c1101388d11000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Received packets on unit 0 should be:
 * Data: 0x000000220000000000118100c0c8884701f40d10000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000
 *
 * AT_Dnx_Cint_mpls_lsr_with_tunnel
 * tx 1 psrc=200 data=0x000000000011000092d7d934810000228847a089d529000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Received packets on unit 0 should be:
 * Data: 0x00000022000000000011810040628847bc750528000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000
 */

struct cint_mpls_lsr_info_s
{
    int in_rif;                           /* access RIF */
    int out_rif;                          /* out RIF */
    int vrf;                              /*  VRF */
    bcm_mac_t my_mac;                     /* mac for Incoming L3 intf */
    bcm_mac_t next_hop_mac;               /* next hop mac for outgoing station */
    int dip_into_tunnel;                  /* dip into mpls tunnel from push command*/
    bcm_ip6_t dip_into_tunnel_ipv6;       /* IPv6 dip into mpls tunnel from push command*/
    int dip_into_tunnel_using_eedb;       /* dip into mpls tunnel from EEI-pointer*/
    int incoming_label[3];                /* Incoming mpls label */
    int mpls_tunnel_label[2];             /* Incoming mpls label to termination */
    int outgoing_label[2];                /*  swapped mpls label  */
    int encap_label[4];                   /*  mpls tunnel label */
    int acces_index[3];                   /* mpls termination label index */
    int pipe_mode_exp_set;                /* exp mode in pipe */
    int exp;                              /*  exp in pipe */
    int ttl;                              /*  ttl in pipe */
    int php_into_mpls_out_lif;            /*EEDB pointer for php on non-bos*/
    int php_into_ipvx_out_lif;            /*EEDB pointer for php on bos*/
    int arp_encap_id;                     /*Next hop intf ID*/ 

    int ingress_tunnel_id_indexed[3];     /* mpls indexed termination tunnel id */
    int ingress_tunnel_id;                /* mpls termination tunnel id */

    int fec_id_for_arp;                   /*FEC ID for arp, for case 1,2,3,4*/
    int egr_mpls_tunnel_encap_id;         /* encap_id for mpls tunnel, for case 2*/

    int is_ingress_mpls_raw;              /* mpls raw service */
    int is_egress_mpls_raw;               /* mpls raw service */

    int in_local_port;
    int out_local_port;

    int mc_group;                         /* Multicast group */
};

cint_mpls_lsr_info_s cint_mpls_lsr_info =
{

    /*
    * In-RIF,
    */
    100,
    /*
    * Out-RIF,
    */
    200,
    /*
    * vrf,
    */
    1,
    /*
    * my_mac | next_hop_mac
    */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x11},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x22},
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
    {5000,1001,1002}, /* swap, php, P2MP*/

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
    * mpls_termination_0
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
    0x3EF81, /*outlif for php into ipvx*/
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
dnx_mpls_lsr_init(
    int ingress_unit,
    int egress_unit,
    int in_sys_port,
    int out_sys_port)
{
    int rv;
    int local_port,is_local;
    int egress_trunk_id;
    int egress_trunk_group;
    int egress_trunk_pool;

    /*
     * 0: Convert the sysport to local port
     */

    if (!BCM_GPORT_IS_TRUNK(in_sys_port)) {
        rv = bcm_port_local_get(ingress_unit, in_sys_port, &cint_mpls_lsr_info.in_local_port);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_port_local_get in local port\n");
            return rv;
        }

        rv = bcm_port_local_get(egress_unit, out_sys_port, &cint_mpls_lsr_info.out_local_port);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_port_local_get out local port\n");
            return rv;
        }
    } else {
        cint_mpls_lsr_info.in_local_port = in_sys_port;
        BCM_TRUNK_ID_GROUP_GET(egress_trunk_group,out_sys_port);

        BCM_TRUNK_ID_POOL_GET(egress_trunk_pool,out_sys_port);
        BCM_TRUNK_ID_SET(egress_trunk_id, egress_trunk_pool, egress_trunk_group);
        BCM_GPORT_TRUNK_SET(cint_mpls_lsr_info.out_local_port, egress_trunk_id);
    }

    /*
     * EncapID, use the minimal valid
     */
    BCM_L3_ITF_SET(cint_mpls_lsr_info.arp_encap_id,BCM_L3_ITF_TYPE_LIF, 0x4000);

    /*
     * mpls tunnel EEDB
     */
    BCM_L3_ITF_SET(cint_mpls_lsr_info.egr_mpls_tunnel_encap_id,BCM_L3_ITF_TYPE_LIF, 0x6001);

    return rv;
}

/**
 * port init
 * INPUT:
 *   in_port - traffic incoming port 
*/
 int
mpls_lsr_ingress_port_configuration(
    int unit,
    int in_port,
    int in_rif)
{
    int rv;

    rv = in_port_intf_set(unit, in_port, in_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Error, mpls_lsr_ingress_port_configuration \n");
        return rv;
    }

    return rv;
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

    return rv;
}

/**
 * L3 intf init
 * INPUT:
 *   in_port - traffic incoming port
*/
int
mpls_lsr_l3_intf_configuration(
    int unit,
    int in_port)
{
    int rv;
    bcm_l3_intf_t l3_intf;
    dnx_utils_l3_eth_rif_s eth_rif_structure;

    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_intf_id = cint_mpls_lsr_info.in_rif;
    rv = bcm_l3_intf_get(unit,&l3_intf);
    if (rv != BCM_E_NONE) {
        dnx_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, cint_mpls_lsr_info.in_rif, 0, 0, cint_mpls_lsr_info.my_mac, cint_mpls_lsr_info.vrf);
        rv = dnx_utils_l3_eth_rif_create(unit, &eth_rif_structure);
        if (rv != BCM_E_NONE)
        {
         printf("Error, dnx_utils_l3_eth_rif_create core_native_eth_rif\n");
         return rv;
        }
    }

    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_intf_id = cint_mpls_lsr_info.out_rif;
    rv = bcm_l3_intf_get(unit,&l3_intf);
    if (rv != BCM_E_NONE) {
        dnx_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, cint_mpls_lsr_info.out_rif, 0, 0, cint_mpls_lsr_info.my_mac, cint_mpls_lsr_info.vrf);
        rv = dnx_utils_l3_eth_rif_create(unit, &eth_rif_structure);
        if (rv != BCM_E_NONE)
        {
         printf("Error, dnx_utils_l3_eth_rif_create core_native_eth_rif\n");
         return rv;
        }

    }    

    return rv;
}

/**
 * ARP adding
 * INPUT:
 *   in_port - traffic incoming port 
 *   out_port - traffic outgoing port
*/
int
mpls_lsr_arp_configuration(
    int unit,
    int out_port)
{
    int rv;

    dnx_utils_l3_arp_s arp_structure;

    dnx_utils_l3_arp_s_common_init(unit, 0, &arp_structure, cint_mpls_lsr_info.arp_encap_id, 0, BCM_L3_FLAGS2_VLAN_TRANSLATION , cint_mpls_lsr_info.next_hop_mac, cint_mpls_lsr_info.out_rif);
    rv = dnx_utils_l3_arp_create(unit, &arp_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create AC egress object ARP only\n");
        return rv;
    }

    return rv;
}

/**
 * FEC entry adding
 * INPUT:
 *   in_port - traffic incoming port
 *   out_sys_port - traffic outgoing port
*/
int
mpls_lsr_fec_for_arp_configuration(
    int unit,
    int out_sys_port)
{
    int rv;
    dnx_utils_l3_fec_s fec_structure;

    /*
     * FEC for ARP
     */
    dnx_utils_l3_fec_s_init(unit, 0x0, &fec_structure);
    fec_structure.destination  = out_sys_port;
    fec_structure.tunnel_gport = cint_mpls_lsr_info.arp_encap_id;
    if (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        fec_structure.flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }
    rv = dnx_utils_l3_fec_create(unit, &fec_structure);

    if (rv != BCM_E_NONE)
    {
        printf("Error, in dnx_utils_l3_fec_create\n");
        return rv;
    }

    cint_mpls_lsr_info.fec_id_for_arp = fec_structure.l3eg_fec_id;
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
    bcm_l3_ingress_t l3_ing_if;
    bcm_l3_ingress_t_init(&l3_ing_if);
    bcm_l3_ingress_get(unit, rif, l3_ing_if);
    if (rv != BCM_E_NONE)
    {
         printf("Error (%d), bcm_l3_ingress_get \n", rv);
         return rv;
    }

    l3_ing_if.flags = l3_ing_if.flags | BCM_L3_INGRESS_WITH_ID |BCM_L3_INGRESS_MPLS_INTF_NAMESPACE;
    rv = bcm_l3_ingress_create(unit,l3_ing_if, rif);
    if (rv != BCM_E_NONE)
    {
         printf("Error (%d), bcm_l3_ingress_create \n", rv);
         return rv;
    }

    return rv;
}

/**
 * mpls tunnel termination entry adding
 * INPUT:
 *   unit - traffic incoming unit
*/
 int
mpls_lsr_termination_entry_add(
    int unit,
    int term_label)
{
    int rv;
    int flags = 0;
    bcm_mpls_tunnel_switch_t entry;
    l3_ingress_intf ingress_itf;
    l3_ingress_intf_init(&ingress_itf);

    bcm_mpls_tunnel_switch_t_init(&entry);
    entry.action = BCM_MPLS_SWITCH_ACTION_POP;
    entry.flags |= flags;

    /*
     * Uniform: inherit TTL and EXP,
     * in general valid options:
     * both present (uniform) or none of them (Pipe)
     */
    entry.ingress_qos_model.ingress_phb = bcmQosIngressModelUniform;
    entry.ingress_qos_model.ingress_remark = bcmQosIngressModelUniform;
    entry.ingress_qos_model.ingress_ttl = bcmQosIngressModelUniform;

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

    /*
     * Set Incoming ETH-RIF properties
     */
    ingress_itf.vrf = cint_mpls_lsr_info.vrf;
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(ingress_itf.intf_id,entry.tunnel_id);
    rv = intf_ingress_rif_set(unit, &ingress_itf);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in intf_ingress_rif_set\n");
        return rv;
    }

    return rv;
}

/*
 * l3 FEC replace
 *
 */
int
mpls_lsr_l3_egress_fec_replace(
    int unit,
    int out_sys_port)
{
    int rv;
    dnx_utils_l3_fec_s fec_structure;

    dnx_utils_l3_fec_s_init(unit, 0x0, &fec_structure);
    fec_structure.destination  = out_sys_port;
    fec_structure.tunnel_gport = cint_mpls_lsr_info.egr_mpls_tunnel_encap_id;
    fec_structure.allocation_flags = BCM_L3_WITH_ID | BCM_L3_REPLACE | BCM_L3_INGRESS_ONLY;
    fec_structure.fec_id = cint_mpls_lsr_info.fec_id_for_arp;
    if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
       fec_structure.flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }
    rv = dnx_utils_l3_fec_create(unit, &fec_structure);

    if (rv != BCM_E_NONE)
    {
        printf("Error, in dnx_utils_l3_fec_create\n");
        return rv;
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

    l3eg.encap_id = cint_mpls_lsr_info.arp_encap_id;
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
    int num_of_labels,
    uint32 force_flags)
{
    int rv, i;
    mpls_util_mpls_tunnel_initiator_create_s mpls_encap_tunnel[1];

    mpls_tunnel_initiator_create_s_init(mpls_encap_tunnel,1);

    for (i = 0; i < num_of_labels; i++)
    {
        mpls_encap_tunnel[0].label[i] = cint_mpls_lsr_info.encap_label[i];
    }

    mpls_encap_tunnel[0].num_labels = num_of_labels;
    mpls_encap_tunnel[0].flags |= force_flags | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT | BCM_MPLS_EGRESS_LABEL_WITH_ID;
    mpls_encap_tunnel[0].l3_intf_id = &cint_mpls_lsr_info.arp_encap_id;

    if(cint_mpls_lsr_info.pipe_mode_exp_set) {
        mpls_encap_tunnel[0].egress_qos_model.egress_qos = bcmQosEgressModelPipeMyNameSpace;
        mpls_encap_tunnel[0].exp0 = cint_mpls_lsr_info.exp;
        mpls_encap_tunnel[0].exp1 = (cint_mpls_lsr_info.exp + 1) % 7;
    } else {
        mpls_encap_tunnel[0].egress_qos_model.egress_qos = bcmQosEgressModelUniform;
    }
    mpls_encap_tunnel[0].egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
    mpls_encap_tunnel[0].ttl = cint_mpls_lsr_info.ttl;
    mpls_encap_tunnel[0].tunnel_id = cint_mpls_lsr_info.egr_mpls_tunnel_encap_id;

    rv = mpls_create_initiator_tunnels(unit, mpls_encap_tunnel, 1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_create_initiator_tunnels\n");
        return rv;
    }

    return rv;
}

/**
 * double MPLS tunnels for labels push
 */
int
mpls_lsr_2_encap_entries_create(
    int unit,
    int num_of_labels,
    uint32 force_flags)
{
    int rv;
    int label_index,i;
    int tunnel_ttl[2] = {20,40};
    mpls_util_mpls_tunnel_initiator_create_s mpls_encap_tunnel[2]; /* 1 ~ 4 labels */

    mpls_tunnel_initiator_create_s_init(mpls_encap_tunnel,2);

    for (label_index = 1; label_index >= 0; label_index--)
    {
        for (i = 0; i < num_of_labels; i++)
        {
            mpls_encap_tunnel[label_index].label[i] = cint_mpls_lsr_info.encap_label[(1-label_index)*2 + i];
        }
        mpls_encap_tunnel[label_index].num_labels = num_of_labels;
        mpls_encap_tunnel[label_index].flags |= force_flags | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
        if (label_index == 1) {
            mpls_encap_tunnel[label_index].l3_intf_id = &cint_mpls_lsr_info.arp_encap_id;
        } else {
            mpls_encap_tunnel[label_index].l3_intf_id = &mpls_encap_tunnel[label_index+1].tunnel_id;
        }
        mpls_encap_tunnel[label_index].egress_qos_model.egress_qos = bcmQosEgressModelUniform;
        mpls_encap_tunnel[label_index].egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
        mpls_encap_tunnel[label_index].ttl = tunnel_ttl[label_index];
        mpls_encap_tunnel[label_index].encap_access = bcmEncapAccessTunnel1 + label_index;
    }

    rv = mpls_create_initiator_tunnels(unit, mpls_encap_tunnel, 2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_create_initiator_tunnels\n");
        return rv;
    }

    cint_mpls_lsr_info.egr_mpls_tunnel_encap_id = mpls_encap_tunnel[0].tunnel_id;

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
    int rv;
    mpls_util_mpls_tunnel_initiator_create_s mpls_encap_tunnel[1];
    mpls_tunnel_initiator_create_s_init(mpls_encap_tunnel,1);

    mpls_encap_tunnel[0].label[0] = BCM_MPLS_LABEL_INVALID;
    mpls_encap_tunnel[0].num_labels = 1;
    mpls_encap_tunnel[0].flags |= force_flags | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT | BCM_MPLS_EGRESS_LABEL_WITH_ID | BCM_MPLS_EGRESS_LABEL_ACTION_VALID;
    mpls_encap_tunnel[0].l3_intf_id = &cint_mpls_lsr_info.arp_encap_id;
    mpls_encap_tunnel[0].action = BCM_MPLS_EGRESS_ACTION_PHP;

    BCM_L3_ITF_SET(cint_mpls_lsr_info.egr_mpls_tunnel_encap_id, BCM_L3_ITF_TYPE_LIF, cint_mpls_lsr_info.php_into_mpls_out_lif);
    mpls_encap_tunnel[0].tunnel_id = cint_mpls_lsr_info.egr_mpls_tunnel_encap_id;

    rv = mpls_create_initiator_tunnels(unit, mpls_encap_tunnel, 1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_create_initiator_tunnels\n");
        return rv;
    }

    mpls_tunnel_initiator_create_s_init(mpls_encap_tunnel,1);

    mpls_encap_tunnel[0].label[0] = BCM_MPLS_LABEL_INVALID;
    mpls_encap_tunnel[0].num_labels = 1;
    mpls_encap_tunnel[0].flags |= force_flags | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT | BCM_MPLS_EGRESS_LABEL_WITH_ID | BCM_MPLS_EGRESS_LABEL_ACTION_VALID;
    mpls_encap_tunnel[0].l3_intf_id = &cint_mpls_lsr_info.arp_encap_id;
    mpls_encap_tunnel[0].action = BCM_MPLS_EGRESS_ACTION_PHP;

    BCM_L3_ITF_SET(mpls_encap_tunnel[0].tunnel_id,BCM_L3_ITF_TYPE_LIF, cint_mpls_lsr_info.php_into_ipvx_out_lif);

    rv = mpls_create_initiator_tunnels(unit, mpls_encap_tunnel, 1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_create_initiator_tunnels\n");
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
     */
    entry.ingress_qos_model.ingress_phb = bcmQosIngressModelUniform;
    entry.ingress_qos_model.ingress_remark = bcmQosIngressModelUniform;
    entry.ingress_qos_model.ingress_ttl = bcmQosIngressModelUniform;

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
 * MPLS ILM P2MP
*/
int
mpls_lsr_ilm_p2mp_entry_create(
    int unit,
    int in_label,
    int eg_label)
{
   int rv;
    bcm_mpls_tunnel_switch_t entry;
    
    bcm_mpls_tunnel_switch_t_init(&entry);
    entry.action = BCM_MPLS_SWITCH_ACTION_SWAP;
    /* TTL decrement has to be present */
    entry.flags = BCM_MPLS_SWITCH_TTL_DECREMENT | BCM_MPLS_SWITCH_P2MP;

    /* incomming label */
    entry.label = in_label;

    /* egress attribures*/
    entry.egress_label.label = eg_label;

    /* add multicast group to entry */
    entry.mc_group = cint_mpls_lsr_info.mc_group;

    rv = bcm_mpls_tunnel_switch_create(unit, &entry);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in bcm_mpls_tunnel_switch_create\n");
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

    rv = dnx_mpls_lsr_init(ingress_unit,egress_unit,in_sys_port,out_sys_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, dnx_mpls_lsr_init\n");
     return rv;
    }

    /*
     * step 1.ingress L2/L3 configuration(Port,In-RIF)
     */
    rv = bcm_vlan_create(ingress_unit,cint_mpls_lsr_info.in_rif);
    if (rv != BCM_E_NONE)
    {
     printf("Error, bcm_vlan_create\n");
     return rv;
    }

    rv = mpls_lsr_ingress_port_configuration(ingress_unit,in_sys_port,cint_mpls_lsr_info.in_rif);
    if (rv != BCM_E_NONE)
    {
     printf("Error, mpls_lsr_ingress_port_configuration\n");
     return rv;
    }

    rv = mpls_lsr_l3_intf_configuration(ingress_unit,in_sys_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, mpls_lsr_l3_intf_configuration\n");
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

    rv = mpls_lsr_l3_intf_configuration(egress_unit,cint_mpls_lsr_info.out_local_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, mpls_lsr_l3_intf_configuration\n");
     return rv;
    }

    rv = mpls_lsr_arp_configuration(egress_unit,cint_mpls_lsr_info.out_local_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, mpls_lsr_arp_configuration\n");
     return rv;
    }

    return rv;
}

/**
 * MPLS LSR SWAP on multi-device
 * INPUT:
 *   in_sys_port - traffic incoming port
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
        rv = dnx_mpls_lsr_init(unit, unit, in_port, out_port);
        if (rv != BCM_E_NONE)
        {
            printf("Error, dnx_mpls_lsr_init\n");
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
 *   in_sys_port - traffic incoming port
 *   out_sys_port - traffic outgoing port
*/
int
multi_dev_mpls_lsr_swap_with_context_example(
    int ingress_unit,
    int egress_unit,
    int in_sys_port,
    int out_sys_port)
{
    int rv;

    rv = dnx_mpls_lsr_init(ingress_unit,egress_unit,in_sys_port,out_sys_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, dnx_mpls_lsr_init\n");
     return rv;
    }

    /*
     * step 1.ingress L2/L3 configuration(Port,In-RIF)
     */
    rv = mpls_lsr_ingress_port_configuration(ingress_unit, in_sys_port,cint_mpls_lsr_info.in_rif);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls_lsr_ingress_port_configuration, vlan=%d, \n", cint_mpls_lsr_info.in_rif);
        return rv;
    }

    rv = mpls_lsr_l3_intf_configuration(ingress_unit,in_sys_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, mpls_lsr_l3_intf_configuration\n");
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
        rv = bcm_vlan_create(egress_unit, cint_mpls_lsr_info.out_rif);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_create with vlan %d continue \n", out_rif);
        }
    }
    /* add port to vlan  */
    rv = bcm_vlan_gport_add(egress_unit, cint_mpls_lsr_info.out_rif, out_sys_port, BCM_VLAN_GPORT_ADD_EGRESS_ONLY);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_gport_add, vlan=%d, \n", out_rif);
        return rv;
    }

    rv = mpls_lsr_l3_intf_configuration(egress_unit,cint_mpls_lsr_info.out_local_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, mpls_lsr_l3_intf_configuration\n");
     return rv;
    }

    rv = mpls_lsr_arp_configuration(egress_unit,cint_mpls_lsr_info.out_local_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, mpls_lsr_arp_configuration\n");
     return rv;
    }

    /*
     * step 4. ingress FWD configuration
     */
    rv = mpls_lsr_forward_entry_configuration_with_context(ingress_unit,out_sys_port,cint_mpls_lsr_info.in_rif);
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
 *   in_sys_port - traffic incoming port
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
 *   in_sys_port - traffic incoming port
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
     * step 3: update FEC point to tunnel
     */
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
    entry.label = cint_mpls_lsr_info.incoming_label[1];
    entry.egress_if = cint_mpls_lsr_info.fec_id_for_arp;
    rv = bcm_mpls_tunnel_switch_create(ingress_unit, &entry);
    if (BCM_E_NONE != rv) {
      printf("Error in bcm_mpls_tunnel_switch_create %x\n",rv);
      return rv;
    }

    return rv;
}

/**
 * IPv4 route to tunnel with entropy
 * INPUT:
 *   in_sys_port - traffic incoming port
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
    if (with_entropy == 1)
    {
         tunnel_force_flags=  BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE;
    }

    if (with_entropy >= 2)
    {
         tunnel_force_flags= BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE | BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE;
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
    if (with_entropy > 2) {
        rv = mpls_lsr_2_encap_entries_create(egress_unit,2,tunnel_force_flags);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in mpls_lsr_encap_entry_create\n", rv);
            return rv;
        }
    } else {
        rv = mpls_lsr_encap_entry_create(egress_unit,2,tunnel_force_flags);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in mpls_lsr_encap_entry_create\n", rv);
            return rv;
        }
    }

    /*
     * step 3: update FEC point to tunnel
     */
    rv = mpls_lsr_l3_egress_fec_replace(ingress_unit, out_sys_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_lsr_l3_egress_fec_replace \n");
        return rv;
    }

    /*
     * step 4: push over IPv4
     */
    rv = add_host_ipv4(ingress_unit, cint_mpls_lsr_info.dip_into_tunnel, cint_mpls_lsr_info.vrf, cint_mpls_lsr_info.fec_id_for_arp, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in add_host_ipv4\n", rv);
        return rv;
    }

    /*
     * step 5: push over IPv6
     */
    rv = add_host_ipv6(ingress_unit, cint_mpls_lsr_info.dip_into_tunnel_ipv6, cint_mpls_lsr_info.vrf, cint_mpls_lsr_info.fec_id_for_arp);
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
 *   in_sys_port - traffic incoming port
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
    rv = dnx_qos_map_l3_ingress_profile(ingress_unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in dnx_qos_map_l3_ingress_profile \n", rv);
        return rv;
    }

    /*
     * step 2: egress mpls qos remarking and marking
     */
    rv = dnx_qos_map_mpls_egress_profile(egress_unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in dnx_qos_map_mpls_egress_profile \n", rv);
        return rv;
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
    rv = bcm_l3_ingress_get(ingress_unit, cint_mpls_lsr_info.in_rif, &l3i);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_l3_ingress_get\n", rv);
        return rv;
    }

    /* attch the ingress QoS profile. */
    l3i.qos_map_id = l3_in_map_id;
    l3i.flags |= BCM_L3_INGRESS_WITH_ID;

    /* set the configuration of the InRIF. */
    rv = bcm_l3_ingress_create(ingress_unit, l3i, cint_mpls_lsr_info.in_rif);
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
    rv = bcm_l3_egress_get(egress_unit, cint_mpls_lsr_info.arp_encap_id, &l3eg);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l3_egress_get\n");
        return rv;
    }

    l3eg.qos_map_id = mpls_eg_map_id;
    l3eg.encap_id = cint_mpls_lsr_info.arp_encap_id;

    rv = bcm_l3_egress_create(egress_unit, flags, &l3eg, &l3egid_null);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_l3_egress_create\n");
        return rv;
    }
    return rv;
}

/**
 * MPLS LSR SWAP on multi-device
 * INPUT:
 *   in_sys_port - traffic incoming port
 *   out_sys_port - traffic outgoing port
*/
 int
 multi_dev_mpls_lsr_mc_example(
    int ingress_unit,
    int egress_unit,
    int in_sys_port,
    int out_sys_port)
{
    int rv;
    cint_mpls_lsr_info.mc_group = 5000;

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
     * step 2. ingress MPLS P2MP configuration
     */
    rv = mpls_lsr_ilm_p2mp_entry_create(ingress_unit,cint_mpls_lsr_info.incoming_label[2],cint_mpls_lsr_info.outgoing_label[1]);
    if (rv != BCM_E_NONE)
    {
     printf("Error, mpls_lsr_ilm_p2mp_entry_create\n");
     return rv;
    }

    /*
     * step 3. Create multicast group
     */
    rv = multicast__open_mc_group(ingress_unit, &cint_mpls_lsr_info.mc_group, 0);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in multicast__open_mc_group\n");
        return rv;
    }

    /*
     * step 4. MPLS tunnel config
     */
    /* 1st tunnel, push 1 label */
    BCM_L3_ITF_SET(cint_mpls_lsr_info.egr_mpls_tunnel_encap_id,BCM_L3_ITF_TYPE_LIF, 0x6001);
    rv = mpls_lsr_encap_entry_create(egress_unit, 1, 0);
    if (rv != BCM_E_NONE)
    {
     printf("Error, mpls_lsr_ilm_p2mp_entry_create\n");
     return rv;
    }

    /* Add tunnel to multicast group */
    rv = multicast__add_multicast_entry(ingress_unit, cint_mpls_lsr_info.mc_group, &out_sys_port, &(cint_mpls_lsr_info.egr_mpls_tunnel_encap_id& 0x3fffff), 1, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, in multicast__add_multicast_entry for tunnel\n");
        return rv;
    }

    /* 2nd tunnel, push 2 labels */
    BCM_L3_ITF_SET(cint_mpls_lsr_info.egr_mpls_tunnel_encap_id,BCM_L3_ITF_TYPE_LIF, 0x6002);
    rv = mpls_lsr_encap_entry_create(egress_unit, 2, 0);
    if (rv != BCM_E_NONE)
    {
     printf("Error, mpls_lsr_ilm_p2mp_entry_create\n");
     return rv;
    }

    /* Add tunnel to multicast group */
    rv = multicast__add_multicast_entry(ingress_unit, cint_mpls_lsr_info.mc_group, &out_sys_port, &(cint_mpls_lsr_info.egr_mpls_tunnel_encap_id& 0x3fffff), 1, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, in multicast__add_multicast_entry for tunnel\n");
        return rv;
    }

    return rv;
}

/**
 * IPv4 route to tunnel with null labels
 * INPUT:
 *   in_sys_port - traffic incoming port
 *   out_sys_port - traffic outgoing port
*/
int
multi_dev_mpls_ip_route_tunnel_egress_null_value(
    int ingress_unit,
    int egress_unit,
    int in_sys_port,
    int out_sys_port)
{

    int rv = BCM_E_NONE;
    cint_mpls_lsr_info.encap_label[0] = 0;
    cint_mpls_lsr_info.encap_label[1] = 0;

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
     * step 2: tunnel with 1 null label
     */
    rv =  mpls_lsr_encap_entry_create(egress_unit,1,0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_lsr_encap_entry_create\n");
        return rv;
    }

    /*
     * step 3: update FEC point to tunnel
     */
    rv = mpls_lsr_l3_egress_fec_replace(ingress_unit, out_sys_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_lsr_l3_egress_fec_replace \n");
        return rv;
    }

    /*
     * step 4: push over IPv4
     */
    rv = add_host_ipv4(ingress_unit, cint_mpls_lsr_info.dip_into_tunnel, cint_mpls_lsr_info.vrf, cint_mpls_lsr_info.fec_id_for_arp, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in add_host_ipv4\n", rv);
        return rv;
    }

    /*
     * step 5: tunnel with 2 null labels
     */
    BCM_L3_ITF_SET(cint_mpls_lsr_info.egr_mpls_tunnel_encap_id,BCM_L3_ITF_TYPE_LIF, 0x6002);
    rv =  mpls_lsr_encap_entry_create(egress_unit,2,0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_lsr_encap_entry_create\n");
        return rv;
    }

    /*
     * step 6: create another FEC and update FEC point to tunnel
     */
    rv = mpls_lsr_fec_for_arp_configuration(ingress_unit,out_sys_port);
    if (rv != BCM_E_NONE)
    {
     printf("Error, mpls_lsr_fec_for_arp_configuration\n");
     return rv;
    }

    rv = mpls_lsr_l3_egress_fec_replace(ingress_unit, out_sys_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_lsr_l3_egress_fec_replace \n");
        return rv;
    }

    /*
     * step 7: push over IPv4
     */

    rv = add_host_ipv4(ingress_unit, cint_mpls_lsr_info.dip_into_tunnel+1, cint_mpls_lsr_info.vrf, cint_mpls_lsr_info.fec_id_for_arp, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in add_host_ipv4\n", rv);
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
    print bcm_qos_map_destroy(ingress_unit, l3_in_map_id);
    print bcm_qos_map_destroy(ingress_unit, l3_qos_in_internal_map_id_phb);
    print bcm_qos_map_destroy(ingress_unit, l3_qos_in_internal_map_id_remark);

    print bcm_qos_map_destroy(egress_unit, mpls_qos_eg_internal_map_id);
    print bcm_qos_map_destroy(egress_unit, mpls_eg_map_id);
}
