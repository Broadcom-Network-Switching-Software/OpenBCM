/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_dnx_oam_fec_injection.c
 *
 * Purpose: basic example for oam over mpls-tp with fec injection.
 *
 * Examples:
 *   - 1. Example of Hierarchial LM (HLM) in egress with FEC injection
 */

/*
 * 1. Example of HLM in egress with FEC injection
 *
 * Purpose:
 *   In this example, HLM in egress with FEC injection is verified.
 *   Expect correct counter was increased after injecting data, CCM, DMM and LMM packets
 *
 * Objects to be created:
 *   A vpls model is setup, and
 *   oam group, lif proflie and 3 ACC endpoints are created,
 *   including one PWE endpoint, one MPLS-TP endpoint for primary, and one MPLS-Section endpoint for backup.
 *
 * Test Scenario
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/./examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/./examples/dnx/field/cint_field_utils.c
 * cint ../../src/./examples/sand/cint_ip_route_basic.c
 * cint ../../src/./examples/sand/cint_vpls_mp_basic.c
 * cint ../../src/./examples/sand/cint_sand_advanced_vlan_translation_mode.c
 * cint ../../src/./examples/dnx/crps/cint_crps_oam_config.c
 * cint ../../src/./examples/dnx/oam/cint_oam_basic.c
 * cint ../../src/./examples/dnx/oam/cint_oam_action.c
 * cint ../../src/./examples/dnx/oam/cint_dnx_oam_fec_injection.c
 * cint
 * cint_vpls_fec_info.cw_present=1;
 * oam_run_hlm_egress_with_fec_injection(0,200,201);
 * exit;
 *
 * Step 1. Load cint_vpls_pwe_protection_info to setup pwe protection model, and
 *         create oam group, Lif profile and two ACC MEPs. One for PWE primary, another for PWE backup
 * oam_run_hlm_egress_with_pwe_protection(0,200,201);
 * exit;
 *
 * Step 2. injected data packet
 * Step 2.1 injected data packet to mpls tunnel
 *          expected counter for MPLS-TP and MPLS-Section are increased
 *          packet format is Data over MPLS of LSP over ETH
 *
 * Step 2.2 injected data packet to pwe
 *          expected counter for PWE, MPLS-TP and MPLS-Section are increased
 *          packet format is Data over MPLS of PWE over MPLS of LSP over ETH
 *
 *
 * Step 3. injected ccm packet
 * Step 3.1 injected ccm packet from MPLS-Section endpoint
 *          expected counter for MPLS-Section are increased
 *          packet format is CCM over ACH over GAL over ETH
 *
 * Step 3.2 injected ccm packet from MPLS-TP endpoint
 *          expected counter for MPLS-TP and MPLS-Section are increased
 *          packet format is CCM over ACH over GAL over MPLS of LSP over ETH
 *
 * Step 3.3 injected ccm packet from PWE endpoint
 *          expected counter for PWE, MPLS-TP and MPLS-Section are increased
 *          packet format is CCM over ACH over MPLS of PWE over MPLS of LSP over ETH
 *
 *
 * Step 4. injected dmm packet
 * Step 4.1 injected dmm packet from MPLS-Section endpoint
 *          expected no counter is increased
 *          packet format is DMM over ACH over GAL over ETH
 *
 * Step 4.2 injected dmm packet from MPLS-TP endpoint
 *          expected counter for MPLS-Section are increased
 *          packet format is DMM over ACH over GAL over MPLS of LSP over ETH
 *
 * Step 4.3 injected dmm packet from PWE endpoint
 *          expected counter for MPLS-TP and MPLS-Section are increased
 *          packet format is DMM over ACH over MPLS of PWE over MPLS of LSP over ETH
 *
 *
 * Step 5. injected lmm packet
 * Step 5.1 injected lmm packet from MPLS-Section endpoint
 *          expected no counter is increased, and TxFCf of LMM packet is same as counter for MPLS-Section endpoint.
 *          packet format is LMM over ACH over GAL over ETH
 *
 * Step 5.2 injected lmm packet from MPLS-TP endpoint
 *          expected counter for MPLS-Section are increased, and TxFCf of LMM packet is same as counter for MPLS-TP endpoint.
 *          packet format is LMM over ACH over GAL over MPLS of LSP over ETH
 *
 * Step 5.3 injected lmm packet from PWE endpoint
 *          expected counter for MPLS-TP and MPLS-Section are increased, and TxFCf of LMM packet is same as counter for PWE endpoint.
 *          packet format is LMM over ACH over MPLS of PWE over MPLS of LSP over ETH
 */


struct cint_vpls_fec_info_s
{
    int ac_port;                                   /* incoming port from ac side */
    int pwe_port;                                  /* incoming port from pwe side */
    int pwe_intf;                                  /* pwe RIF */
    bcm_vpn_t vpn;                                 /* vsi id */
    bcm_gport_t mpls_port_id_ingress;              /* global lif encoded as MPLS port, used for ingress, must be as egress */
    bcm_gport_t mpls_port_id_egress;               /* global lif encoded as MPLS port, used for egress, must be as ingress */
    bcm_gport_t vlan_port_id;                      /* vlan port id of the ac_port, used for ingress */
    bcm_if_t encap_id;                             /* The global lif of the EEDB PWE entry. */
    int core_arp_id;                               /* Id for ac ARP entry */
    int tunnel_encap_fec_id;                       /* ac fec id for encapsulation entry - outlif for MPLS entry in EEDB */
    int mpls_encap_fec_id;                         /* ac fec id for encapsulation entry - outlif for MPLS entry in EEDB */
    bcm_mac_t ac_intf_mac_address;                 /* mac for ac RIF */
    bcm_mac_t ac_mac_address;                      /* mac for ingress next hop */
    bcm_mac_t pwe_intf_mac_address;                /* mac for pwe RIF */
    bcm_mac_t pwe_arp_next_hop_mac;                /* mac for egress next hop */
    bcm_if_t mpls_tunnel_id;                       /* tunnel id for encapsulation entry */
    bcm_mpls_label_t mpls_tunnel_label;            /* pushed label */
    bcm_mpls_label_t pwe_label;                    /* pwe label */
    bcm_mpls_label_t pwe_termination_label;        /* label to be terminated */
    int cw_present;                                /* indication to whether CW is present */
    bcm_if_t mpls_tunnel_id_ingress;               /* tunnel id for termination entry */
    int skip_auto_fec_allocation;                  /* In case the FEC ID needs to be allocated outside of the cint */
};

cint_vpls_fec_info_s cint_vpls_fec_info = {
    /*
     * ports : ac_port | pwe_port
     */
    200, 201,
    /*
     * pwe_intf
     */
    30,
    /*
     * vpn
     */
    5,
    /*
     * mpls_port_id_ingress |  mpls_port_id_egress | vlan_port_id
     */
    9999, 8888, 0,
    /*
     * encap_id
     */
    8888,
    /*
     * core_arp_id
     */
    9001,
    /*
     * tunnel_encap_fec_id
     */
    50000,
    /*
     * mpls_encap_fec_id
     */
    50008,
    /*
     * ac_intf_mac_address |  ac_mac_address
     */
    {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}, {0x00, 0x11, 0x00, 0x00, 0x01, 0x12},
    /*
     * pwe_intf_mac_address | pwe_arp_next_hop_mac
     */
    {0x00, 0x0c, 0x00, 0x02, 0x00, 0x01}, {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d},
    /*
     * mpls_tunnel_id
     */
    8194,
    /*
     * mpls_tunnel_label , pwe_label
     */
    3333, 3456,
    /*
     * pwe_termination_label
     */
    3333,
    /*
     * cw_present
     */
    0,
    /*
     * mpls_tunnel_id_ingress
     */
    0,
    /*
     * skip_auto_fec_allocation
     */
    0
};

int verbose1 = 1;
int is_dual_ended_lm = 0;

int lm_pwe_port = 0;

bcm_oam_group_info_t    group_info_short_ma;
bcm_oam_endpoint_info_t mep_acc_pwe_info;
bcm_oam_endpoint_info_t mep_acc_mpls_info;
bcm_oam_endpoint_info_t mep_acc_section_info;

int mdl_pwe     = 7;
int mdl_mpls    = 7;
int mdl_section = 7;

int lm_counter_if_pwe     = 0;
int lm_counter_if_mpls    = 1;
int lm_counter_if_section = 2;
int lm_counter_base_id_pwe = 0;
int lm_counter_base_id_mpls = 0;
int lm_counter_base_id_section = 0;

bcm_oam_profile_t egress_profile_id_pwe = BCM_OAM_PROFILE_INVALID;
bcm_oam_profile_t egress_profile_id_mpls_section = BCM_OAM_PROFILE_INVALID;
bcm_oam_profile_t egress_profile_id_mpls_tp = BCM_OAM_PROFILE_INVALID;

bcm_mac_t pwe_mac_addr = {0x00, 0x00, 0x00, 0x00, 0x01, 0x18};

int
vpls_fec_configure_port_properties (
    int unit,
    int ac_port,
    int pwe_port)
{
    int rv = BCM_E_NONE;

    /*
     * Set class for both ports
     */
    rv = bcm_port_class_set(unit, ac_port, bcmPortClassId, ac_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_class_set, port=%d, \n", ac_port);
        return rv;
    }

    rv = bcm_port_class_set(unit, pwe_port, bcmPortClassId, pwe_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_class_set, port=%d, \n", pwe_port);
        return rv;
    }

    cint_vpls_fec_info.ac_port = ac_port;
    cint_vpls_fec_info.pwe_port = pwe_port;

    /*
     * Set Out-Port default properties
     */
    rv = out_port_set(unit, cint_vpls_fec_info.pwe_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_set intf_out\n");
        return rv;
    }

    /*
     * Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, cint_vpls_fec_info.pwe_port, cint_vpls_fec_info.pwe_intf);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set intf_in\n");
        return rv;
    }

    /*
     * Set Out-Port default properties
     */
    rv = out_port_set(unit, cint_vpls_fec_info.ac_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_set intf_out\n");
        return rv;
    }

    return rv;
}

int
vpls_fec_create_l3_interfaces (
    int unit)
{
    int rv;

    /*
     * Create ETH-RIF and set its properties
     */
    rv = intf_eth_rif_create(unit, cint_vpls_fec_info.pwe_intf, cint_vpls_fec_info.pwe_intf_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create pwe_intf\n");
    }

    return rv;
}

int
vpls_fec_create_arp_entry (
    int unit)
{
    int rv;

    /*
     * Configure ARP entry
     */
    rv = l3__egress_only_encap__create(unit, 0, cint_vpls_fec_info.core_arp_id,
                                       cint_vpls_fec_info.pwe_arp_next_hop_mac, cint_vpls_fec_info.pwe_intf);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create AC egress object ARP only\n");
    }

    return rv;

}

int
vpls_fec_create_tunnel_fec_entry (
    int unit)
{
    int rv;
    int temp_out_rif;
    int out_rif_or_tunnel;
    int flags = BCM_L3_CASCADED;

    if(!cint_vpls_fec_info.skip_auto_fec_allocation)
    {
        /*
         * Jericho 2 support static FEC allocation as other cints uses the first FEC ID add +8
         */
        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 1, &cint_vpls_fec_info.tunnel_encap_fec_id);
        cint_vpls_fec_info.tunnel_encap_fec_id += 8;
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }
    }
    else
    {
        cint_vpls_fec_info.tunnel_encap_fec_id = 10008;
    }

    /*
     * Create FEC:
     * 1) Give a fec id.
     * 2) Tunnel id of the encapsulation entry.
     * 3) Arp id will be given with the only_encap call, so give 0.
     * 4) Give the out port.
     */
    BCM_L3_ITF_SET(out_rif_or_tunnel, BCM_L3_ITF_TYPE_LIF, cint_vpls_fec_info.mpls_tunnel_id);
    rv = l3__egress_only_fec__create_inner(unit, cint_vpls_fec_info.tunnel_encap_fec_id,
                                           out_rif_or_tunnel, 0, cint_vpls_fec_info.pwe_port, flags, 0, 0, NULL);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only\n");
    }

    return rv;
}

int
vpls_fec_create_mpls_tunnel (
    int unit)
{

    bcm_mpls_egress_label_t label_array[1];
    int num_labels = 1;
    int rv;

    bcm_mpls_egress_label_t_init(&label_array[0]);

    /*
     * Label to be pushed as part of the MPLS tunnel.
    */
    label_array[0].label = cint_vpls_fec_info.mpls_tunnel_label;
    label_array[0].flags = BCM_MPLS_EGRESS_LABEL_WITH_ID;
    BCM_L3_ITF_SET(label_array[0].tunnel_id, BCM_L3_ITF_TYPE_LIF, cint_vpls_fec_info.mpls_tunnel_id);

    /*
     * Set the next pointer of this entry to be the arp. This configuration is new compared to Jericho, where the arp
     * pointer used to be connected to the EEDB entry via bcm_l3_egress_create (with egress flag indication).
     */
    label_array[0].l3_intf_id = cint_vpls_fec_info.core_arp_id;
    label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;

    rv = bcm_mpls_tunnel_initiator_create(unit, 0, num_labels, label_array);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_tunnel_initiator_create\n");
        return rv;
    }

    cint_vpls_fec_info.mpls_tunnel_id = label_array[0].tunnel_id;

    return rv;
}

int
vpls_fec_create_termination_stack (
    int unit)
{
    bcm_mpls_tunnel_switch_t entry;
    int rv;
    int propogation_profile_flags = BCM_MPLS_SWITCH_OUTER_EXP | BCM_MPLS_SWITCH_OUTER_TTL;
    /*
     * Create a stack of MPLS labels to be terminated
     */
    bcm_mpls_tunnel_switch_t_init(&entry);

    entry.action = BCM_MPLS_SWITCH_ACTION_POP;

    /*
     * incoming label
     * only the mpls tunnel label needs to be defined here.
     * pwe label will be handed as part of the ingress mpls_port_add api
     */
    entry.label = cint_vpls_fec_info.pwe_termination_label;

    entry.ingress_qos_model.ingress_phb = bcmQosIngressModelUniform;
    entry.ingress_qos_model.ingress_remark = bcmQosIngressModelUniform;
    entry.ingress_qos_model.ingress_ttl = bcmQosIngressModelUniform;

    rv = bcm_mpls_tunnel_switch_create(unit, &entry);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_tunnel_switch_create\n");
        return rv;
    }

    cint_vpls_fec_info.mpls_tunnel_id_ingress = entry.tunnel_id;

    return rv;
}

int
vpls_fec_l2_addr_add (
    int unit,
    bcm_vpn_t vpn)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t l2addr;

    bcm_l2_addr_t_init(&l2addr, cint_vpls_fec_info.ac_mac_address, vpn);
    l2addr.flags |= BCM_L2_STATIC;
    l2addr.port = cint_vpls_fec_info.vlan_port_id;

    rv = bcm_l2_addr_add(unit, l2addr);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in bcm_l2_addr_add\n", rv);
        return rv;
    }

    return rv;
}

int
vpls_fec_create_mpls_fec_entry (
    int unit)
{
    int rv;
    int temp_out_rif;
    int out_rif_or_tunnel;
    bcm_gport_t out_gport;
    int flags = 0;

    if(!cint_vpls_fec_info.skip_auto_fec_allocation)
    {
        /*
         * Jericho 2 support static FEC allocation as other cints uses the first FEC ID add +16
         */
        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_vpls_fec_info.mpls_encap_fec_id);
        cint_vpls_fec_info.mpls_encap_fec_id += 16;
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }
    }
    else
    {
        cint_vpls_fec_info.mpls_encap_fec_id = 10016;
    }

    /*
     * Create FEC:
     * 1) Give a fec id.
     * 2) Tunnel id of the encapsulation entry.
     * 3) Arp id will be given with the only_encap call, so give 0.
     * 4) Give the out port.
     */
    BCM_L3_ITF_SET(out_rif_or_tunnel, BCM_L3_ITF_TYPE_LIF, cint_vpls_fec_info.encap_id);
    BCM_GPORT_FORWARD_PORT_SET(out_gport, BCM_L3_ITF_VAL_GET(cint_vpls_fec_info.tunnel_encap_fec_id));
    rv = l3__egress_only_fec__create_inner(unit, cint_vpls_fec_info.mpls_encap_fec_id,
                                           out_rif_or_tunnel, 0, out_gport, flags, 0, 0, NULL);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only\n");
    }

    return rv;
}


int
vpls_fec_mpls_port_add_encapsulation (
    int unit)
{
    int rv = BCM_E_NONE;
    bcm_mpls_port_t mpls_port;
    bcm_gport_t gport_id;


    bcm_mpls_port_t_init(&mpls_port);
    mpls_port.flags |= BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_EGRESS_TUNNEL;
    mpls_port.flags2 |= BCM_MPLS_PORT2_EGRESS_ONLY;

    if (cint_vpls_fec_info.cw_present)
    {
        mpls_port.flags |= BCM_MPLS_PORT_CONTROL_WORD;
    }

    BCM_GPORT_SUB_TYPE_LIF_SET(gport_id, 0, cint_vpls_fec_info.mpls_port_id_egress);
    BCM_GPORT_MPLS_PORT_ID_SET(mpls_port.mpls_port_id, gport_id);
    mpls_port.egress_label.label = cint_vpls_fec_info.pwe_label;
    mpls_port.egress_label.flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;

    rv = bcm_mpls_port_add(unit, 0, mpls_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in bcm_mpls_port_add encapsulation\n", rv);
        return rv;
    }

    cint_vpls_fec_info.mpls_port_id_egress = mpls_port.mpls_port_id;
    cint_vpls_fec_info.encap_id = mpls_port.encap_id;

    return rv;
}

int
vpls_fec_mpls_port_add_termination (
    int unit,
    bcm_vpn_t vpn)
{
    int rv = BCM_E_NONE;
    bcm_mpls_port_t mpls_port;
    bcm_gport_t gport_id;


    bcm_mpls_port_t_init(&mpls_port);
    mpls_port.flags |= BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_EGRESS_TUNNEL;

    if (cint_vpls_fec_info.cw_present)
    {
        mpls_port.flags |= BCM_MPLS_PORT_CONTROL_WORD;
    }

    mpls_port.flags2 |= BCM_MPLS_PORT2_INGRESS_ONLY | (vpn ? 0 : BCM_MPLS_PORT2_CROSS_CONNECT);
    mpls_port.ingress_qos_model.ingress_phb = bcmQosIngressModelUniform;
    mpls_port.ingress_qos_model.ingress_remark = bcmQosIngressModelUniform;
    mpls_port.ingress_qos_model.ingress_ttl = bcmQosIngressModelUniform;

    BCM_GPORT_SUB_TYPE_LIF_SET(gport_id, 0, cint_vpls_fec_info.mpls_port_id_ingress);
    BCM_GPORT_MPLS_PORT_ID_SET(mpls_port.mpls_port_id, gport_id);

    /*
     * encap_id is the egress outlif - used for learning
     */
    mpls_port.encap_id = cint_vpls_fec_info.encap_id;
    mpls_port.criteria = BCM_MPLS_PORT_MATCH_LABEL;
    mpls_port.match_label = cint_vpls_fec_info.pwe_label;

    /** connect PWE entry to created MPLS encapsulation entry for learning */
    BCM_L3_ITF_SET(mpls_port.egress_tunnel_if, BCM_L3_ITF_TYPE_FEC, cint_vpls_fec_info.mpls_encap_fec_id);

    rv = bcm_mpls_port_add(unit, vpn, mpls_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in bcm_mpls_port_add termination\n", rv);
        return rv;
    }

    cint_vpls_fec_info.mpls_port_id_ingress = mpls_port.mpls_port_id;

    return rv;
}

int
vpls_fec_vswitch_add_access_port (
    int unit,
    bcm_gport_t port_id)
{
    int rv = BCM_E_NONE;
    bcm_vlan_port_t vlan_port;

    /*
     * add port, according to port_vlan_vlan
     */
    bcm_vlan_port_t_init(&vlan_port);

    /*
     * set port attribures, key <port-vlan-vlan>
     */
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = port_id;
    vlan_port.match_vlan = cint_vpls_fec_info.vpn;
/*    vlan_port.egress_vlan = cint_vpls_fec_info.vpn; */
    vlan_port.vsi = 0;
    vlan_port.flags = 0;

    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_port_create\n");
        return rv;
    }
    cint_vpls_fec_info.vlan_port_id = vlan_port.vlan_port_id;

    rv = bcm_vswitch_port_add(unit, cint_vpls_fec_info.vpn, vlan_port.vlan_port_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vswitch_port_add\n");
        return rv;
    }
    rv = bcm_vlan_gport_add(unit, cint_vpls_fec_info.vpn, port_id, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_gport_add \n");
        return rv;
    }

    return rv;
}

int
vpls_fec_ive_swap (
    int unit,
    int port)
{
    int rv = BCM_E_NONE;
    bcm_vlan_port_t vlan_port;
    bcm_port_tpid_class_t port_tpid_class;
    int next_layer_network_domain;


    /* set tag format 0 for untagged packets */
    bcm_port_tpid_class_t_init(&port_tpid_class);
    port_tpid_class.port = port;
    port_tpid_class.tpid1 = 0x8100;
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    port_tpid_class.tag_format_class_id = 4;
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_OUTER_NOT_PRIO | BCM_PORT_TPID_CLASS_INGRESS_ONLY;
    port_tpid_class.vlan_translation_action_id = 0;
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_tpid_class_set, port=%d, \n", port);
        return rv;
    }

    /* configure ive swap */
    rv = ive_eve_translation_set(unit,  cint_vpls_fec_info.vlan_port_id, 0x8100, 0, bcmVlanActionReplace, bcmVlanActionNone,  1111,  0, 5 /*edit_profile*/, 4 /*tag_format*/, 1);
    if (rv != BCM_E_NONE) {
        printf("Error(%d), in ive_eve_translation_set\n", rv);
        return rv;
    }

    next_layer_network_domain = vpls_mpls_next_layer_network_domain_value(unit, 3);
    rv = bcm_port_class_set(unit, cint_vpls_fec_info.mpls_port_id_ingress, bcmPortClassIngress, next_layer_network_domain);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_class_set, port=0x%x, \n", cint_vpls_fec_info.mpls_port_id_ingress);
        return rv;
    }

    /*
     * add port.
     */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags = BCM_VLAN_PORT_NATIVE | BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    vlan_port.flags |= BCM_VLAN_PORT_VLAN_TRANSLATION;
    vlan_port.vsi = 5;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in bcm_vlan_port_create\n", rv);
        return rv;
    }
    /*
     * Save the created vlan_port_id
     */
    printf("native ac = 0x%08x\n", vlan_port.vlan_port_id);

    /*
     * Add Match
     */
    bcm_port_match_info_t match_info;
    bcm_port_match_info_t_init(&match_info);
    match_info.flags = BCM_PORT_MATCH_INGRESS_ONLY | BCM_PORT_MATCH_NATIVE;
    match_info.port = cint_vpls_fec_info.mpls_port_id_ingress;
    match_info.match = BCM_PORT_MATCH_PORT;        /* Match 2 tags */
    rv = bcm_port_match_add(unit, vlan_port.vlan_port_id, &match_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in bcm_port_match_add\n", rv);
        return rv;
    }
    printf("match_port done\n");

    /* configure ive swap */
    rv = ive_eve_translation_set( unit,  vlan_port.vlan_port_id, 0x8100, 0, bcmVlanActionReplace, bcmVlanActionNone,  1111,  0, 5 /*edit_profile*/, 8 /*tag_format*/, 1);
    if (rv != BCM_E_NONE) {
        printf("Error(%d), in ive_eve_translation_set\n", rv);
        return rv;
    }

    return rv;
}

/*
 * Main function for vpls fec injection scenario.
 */
int
vpls_fec_main (
    int unit,
    int ac_port,
    int pwe_port)
{
    int rv = BCM_E_NONE;

    cint_vpls_fec_info.ac_port = ac_port;
    cint_vpls_fec_info.pwe_port = pwe_port;

    /*
     * Configure AC and PWE ports
     */
    rv = vpls_fec_configure_port_properties(unit, ac_port, pwe_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_fec_configure_port_properties\n", rv);
        return rv;
    }

    /*
     * create vlan based on the vsi (vpn)
     */
    rv = bcm_vlan_create(unit, cint_vpls_fec_info.vpn);
    if (rv != BCM_E_NONE)
    {
        printf("Failed (%d) to create VLAN %d\n", rv, cint_vpls_fec_info.vpn);
        return rv;
    }

    /*
     * Configure L3 interfaces
     */
    rv = vpls_fec_create_l3_interfaces(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_fec_create_l3_interfaces\n", rv);
        return rv;
    }

    /*
     * Configure an ARP entries
     */
    rv = vpls_fec_create_arp_entry(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_fec_create_arp_entry\n", rv);
        return rv;
    }

    /*
     * Configure a push entry.
     */
    rv = vpls_fec_create_mpls_tunnel(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_fec_create_mpls_tunnel\n", rv);
        return rv;
    }

    /*
     * Configure fec entry
     */
    rv = vpls_fec_create_tunnel_fec_entry(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_fec_create_tunnel_fec_entry\n", rv);
        return rv;
    }

    /*
     * Configure a termination label for the ingress flow
     */
    rv = vpls_fec_create_termination_stack(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_fec_create_termination_stack\n", rv);
        return rv;
    }

    /*
     * configure PWE tunnel - egress flow
     */
    rv = vpls_fec_mpls_port_add_encapsulation(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_fec_mpls_port_add_encapsulation\n", rv);
        return rv;
    }

    /*
     * configure PWE tunnel - ingress flow
     */
    rv = vpls_fec_mpls_port_add_termination(unit, cint_vpls_fec_info.vpn);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_fec_mpls_port_add_termination\n", rv);
        return rv;
    }

    /*
     * Configure fec entry
     */
    rv = vpls_fec_create_mpls_fec_entry(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_fec_create_mpls_fec_entry\n", rv);
        return rv;
    }


    /** define ac and pwe ports */
    rv = vpls_fec_vswitch_add_access_port(unit, ac_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_fec_vswitch_add_access_port\n", rv);
        return rv;
    }

    /*
     * add l2 addresses to be defined as static - no learning needed
     */
    rv = vpls_fec_l2_addr_add(unit, cint_vpls_fec_info.vpn);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_fec_l2_addr_add\n", rv);
        return rv;
    }

    return rv;
}

/**
 * DNX ONLY.
 * Configure ingress and egress profiles for MPLS Section LIF.
 * Use cint_oam_action.c to create action profiles for OAM
 * section.
 *
 *
 * @param unit
 * @param gport - MPLS In-LIF
 * @param mpls_out_gport - MPLS/PWE Out-LIF
 * @param is_dual_ended_lm - Used for Dual-Ended LM profile configuration
 * @param mep_info - Used for setting mep information
 *
 * @return int
 */
int config_oam_section_profile(
    int unit,
    bcm_gport_t gport,
    bcm_gport_t mpls_out_gport,
    int is_dual_ended_lm,
    bcm_oam_endpoint_info_t *mep_info)
{
    int rv;
    int lm_type;
    uint32 flags=0;
    bcm_oam_profile_t ingress_profile_id = 1, acc_ingress_profile = 1, egress_profile_id = BCM_OAM_PROFILE_INVALID;

    /** Set lm type */
    lm_type = is_dual_ended_lm ? 1 : 0;

    /** Create ingress oam section profile */
    rv = oam_section_action_profile_create(unit,lm_type,&ingress_profile_id,&acc_ingress_profile);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in oam_set_of_action_profiles_create.\n", rv);
        return rv;
    }

    mep_info->acc_profile_id = acc_ingress_profile;
    if (gport != BCM_GPORT_INVALID)
    {
        flags = 0;
        rv = bcm_oam_lif_profile_set(unit, flags, gport, ingress_profile_id, egress_profile_id);
        if (rv != BCM_E_NONE)
        {
           printf("Error no %d, in bcm_oam_lif_profile_set.\n", rv);
           return rv;
        }
    }

    if (mpls_out_gport != BCM_GPORT_INVALID)
    {
        /** Create egress profile - profile is used, but no actions are performed. */
        egress_profile_id = egress_profile_id_mpls_section;
        rv = bcm_oam_profile_id_get_by_type(unit, egress_profile_id, bcmOAMProfileEgressLIF, &flags, &egress_profile_id);
        if (rv != BCM_E_NONE)
        {
           printf("bcm_oam_profile_id_get_by_type(egress) \n");
           return rv;
        }

        rv = bcm_oam_lif_profile_set(unit, flags, mpls_out_gport, BCM_OAM_PROFILE_INVALID, egress_profile_id);
        if (rv != BCM_E_NONE)
        {
           printf("Error no %d, in bcm_oam_lif_profile_set.\n", rv);
           return rv;
        }
    }

  return rv;
}

/**
 * DNX ONLY.
 * Configure ingress and egress profiles for MPLS or PWE LIFs.
 * Use profiles created in cint_oam_action.c. *
 *
 * @param unit
 * @param gport - MPLS/PWE In-LIF
 * @param mpls_out_gport - MPLS/PWE Out-LIF
 * @param is_dual_ended_lm - Used for Dual-Ended LM profile configuration
 * @param mep_info - Used for setting mep information
 *
 * @return int
 */
int config_oam_mpls_or_pwe_profile(
    int unit,
    bcm_gport_t gport,
    bcm_gport_t mpls_out_gport,
    int is_dual_ended_lm,
    bcm_oam_endpoint_info_t *mep_info)
{
    int rv;
    uint32 flags=0;
    bcm_oam_profile_t ingress_profile_id = 1, acc_ingress_profile = 1, egress_profile_id = BCM_OAM_PROFILE_INVALID;

    if(mep_info->type==bcmOAMEndpointTypeBHHMPLS || mep_info->type==bcmOAMEndpointTypeBHHPweGAL)
    {
        /** Profiles for MPLS-TP */
        if (is_dual_ended_lm)
        {
            ingress_profile_id = oam_lif_profiles.oam_profile_mpls_dual_ended;
            acc_ingress_profile = oam_acc_lif_profiles.oam_acc_profile_mpls_dual_ended;
        }
        else
        {
            ingress_profile_id = oam_lif_profiles.oam_profile_mpls_single_ended;
            acc_ingress_profile = oam_acc_lif_profiles.oam_acc_profile_mpls_single_ended;
        }
    }
    else
    {
        /** Profiles for PWE */
        if (is_dual_ended_lm)
        {
            ingress_profile_id = oam_lif_profiles.oam_profile_pwe_dual_ended;
            acc_ingress_profile = oam_acc_lif_profiles.oam_acc_profile_pwe_dual_ended;
        }
        else
        {
            ingress_profile_id = oam_lif_profiles.oam_profile_pwe_single_ended;
            acc_ingress_profile = oam_acc_lif_profiles.oam_acc_profile_pwe_single_ended;
        }
    }

    mep_info->acc_profile_id = acc_ingress_profile;

    if (gport != BCM_GPORT_INVALID)
    {
        flags = 0;
        printf("Gport is 0x%08X\n", gport);
        rv = bcm_oam_lif_profile_set(unit, flags, gport, ingress_profile_id, egress_profile_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %d, in bcm_oam_lif_profile_set.\n", rv);
            return rv;
        }
    }

    if (mpls_out_gport != BCM_GPORT_INVALID)
    {
        /** Create egress profile - profile is used, but no actions are performed. */
        egress_profile_id = (mep_info->type==bcmOAMEndpointTypeBHHMPLS || mep_info->type==bcmOAMEndpointTypeBHHPweGAL) ?
            egress_profile_id_mpls_tp : 1;

        rv = bcm_oam_profile_id_get_by_type(unit, egress_profile_id, bcmOAMProfileEgressLIF, &flags, &egress_profile_id);
        if (rv != BCM_E_NONE)
        {
            printf("bcm_oam_profile_id_get_by_type(egress) \n");
            return rv;
        }
        printf("egress_profile_id (%d) \n", egress_profile_id);

        rv = bcm_oam_lif_profile_set(unit, flags, mpls_out_gport, BCM_OAM_PROFILE_INVALID, egress_profile_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %d, in bcm_oam_lif_profile_set for gport(0x%8.8x)\n", rv, mpls_out_gport);
            return rv;
        }
    }

    return rv;
}

/*
 * 1. Example of oamp injection with FEC injection
 */
int
oam_run_with_fec_injection (
    int unit,
    int ac_port,
    int pwe_port)
{
    int rv;

    bcm_oam_group_info_t *group_info;
    uint8 short_name[BCM_OAM_GROUP_NAME_LENGTH] = {1, 3, 2, 0xab, 0xcd};
    bcm_oam_endpoint_info_t mep_acc_info;
    int gport;
    int mpls_tunnel_gport;
    int mpls_out_gport;
    int counter_base;
    int counter_if;
    bcm_oam_loss_t loss_info;
    int loss_id = 65560;
    bcm_oam_delay_t delay_info;

    /*
     * Setup VPLS fec injection model
     */
    rv = vpls_fec_main(unit, ac_port, pwe_port);
    if (rv != BCM_E_NONE) {
        printf("Error rv(%d), in vpls_fec_main\n", rv);
        return rv;
    }

    bcm_oam_group_info_t_init(&group_info_short_ma);
    sal_memcpy(group_info_short_ma.name, short_name, BCM_OAM_GROUP_NAME_LENGTH);
    rv = bcm_oam_group_create(unit, &group_info_short_ma);
    if (rv != BCM_E_NONE) {
        printf("Error rv(%d), in bcm_oam_group_create.\n", rv);
        return rv;
    }
    group_info = &group_info_short_ma;
    printf("Created group short name format\n");

    rv = oam_set_of_action_profiles_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error rv(%d), in oam_set_of_action_profiles_create.\n", rv);
        return rv;
    }

    counter_if = 0;
    rv = set_counter_resource(unit, pwe_port, counter_if, 1, &counter_base);
    if (rv != BCM_E_NONE) {
        printf("Error rv(%d), in set_counter_resource.\n", rv);
        return rv;
    }

    /*
     * Adding acc MEP for PWE
     */
    gport = cint_vpls_fec_info.mpls_port_id_ingress;
    mpls_tunnel_gport = cint_vpls_fec_info.mpls_encap_fec_id;
    BCM_L3_ITF_SET(mpls_tunnel_gport, BCM_L3_ITF_TYPE_FEC, mpls_tunnel_gport);
    mpls_out_gport = cint_vpls_fec_info.mpls_port_id_egress;

    bcm_oam_endpoint_info_t_init(&mep_acc_info);
    mep_acc_info.type = bcmOAMEndpointTypeBHHPwe;
    mep_acc_info.endpoint_memory_type = bcmOamEndpointMemoryTypeSelfContained;

    /** Lif Profile */
    rv = config_oam_mpls_or_pwe_profile(unit, gport, mpls_out_gport, 0, &mep_acc_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error rv(%d), in config_oam_mpls_or_pwe_profile.\n", rv);
        return rv;
    }

    /*RX*/
    mep_acc_info.group = group_info->id;
    mep_acc_info.opcode_flags |= BCM_OAM_OPCODE_CCM_IN_HW;
    mep_acc_info.level = 7;                         /*Y1731 level*/
    mep_acc_info.gport = gport;                     /* in lif */
    mep_acc_info.mpls_out_gport = mpls_out_gport;   /* out lif */
    mep_acc_info.lm_counter_if = counter_if;
    mep_acc_info.lm_counter_base_id = counter_base;

    /*TX*/
    mep_acc_info.tx_gport = BCM_GPORT_INVALID;
    mep_acc_info.name = 0x1235;
    mep_acc_info.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;

    mep_acc_info.intf_id = mpls_tunnel_gport;
    mep_acc_info.egress_label.label = 3000;         /* Constant label for PWE tunneling */
    mep_acc_info.egress_label.ttl = 0xa;
    mep_acc_info.egress_label.exp = 1;

    printf("bcm_oam_endpoint_create mep_acc_info\n");
    rv = bcm_oam_endpoint_create(unit, &mep_acc_info);
    if (rv != BCM_E_NONE) {
        printf("Error rv(%d), in bcm_oam_endpoint_create\n", rv);
        return rv;
    }

    sal_memcpy(&mep_acc_pwe_info, &mep_acc_info, sizeof(mep_acc_info));
    printf("created acc MEP for pwe with id %d\n", mep_acc_info.id);

    bcm_oam_loss_t_init(&loss_info);
    loss_info.id = mep_acc_info.id;
    if (is_dual_ended_lm) {
        loss_info.flags = BCM_OAM_LOSS_WITH_ID;
    } else {
        loss_info.flags = BCM_OAM_LOSS_WITH_ID | BCM_OAM_LOSS_SINGLE_ENDED;
    }

    loss_info.loss_id = loss_id;

    loss_info.period = 0;
    printf("bcm_oam_loss_add loss_info\n");
    rv = bcm_oam_loss_add(unit, &loss_info);
    if (rv != BCM_E_NONE) {
        printf("Error rv(%d), in bcm_oam_loss_add\n", rv);
        return rv;
    }

    bcm_oam_delay_t_init(&delay_info);
    delay_info.id = mep_acc_info.id;
    delay_info.flags = 0;
    delay_info.timestamp_format = bcmOAMTimestampFormatIEEE1588v1;
    delay_info.period = 0;
    printf("bcm_oam_delay_add delay_info\n");
    rv = bcm_oam_delay_add(unit, &delay_info);
    if (rv != BCM_E_NONE) {
        printf("Error rv(%d), in bcm_oam_delay_add\n", rv);
        return rv;
    }

    return rv;
}


/**
 * DNX ONLY.
 * Create MEP for PWE or MPLS-TP or MPLS-Section, based on mep_type
 */
int
oam_mep_over_tunnel_create (
    int unit,
    bcm_oam_endpoint_info_t *mep_acc_info,
    int mep_type,
    int group,
    int counter_base,
    int counter_if,
    int gport,
    int out_gport,
    int mpls_tunnel_id,
    int tx_gport,
    int is_mep_with_id,
    int mep_id,
    int mdl,
    int is_dual_ended_lm)
{
    bcm_error_t rv;
    bcm_oam_loss_t loss_info;
    bcm_oam_delay_t delay_info;
    int loss_id = 0;

    /* 1. Create OAM MPLS/PWE MEP */
    bcm_oam_endpoint_info_t_init(mep_acc_info);
    mep_acc_info->type = mep_type;
    mep_acc_info->endpoint_memory_type = bcmOamEndpointMemoryTypeSelfContained;

    /*RX*/
    mep_acc_info->group = group;
    mep_acc_info->opcode_flags |= BCM_OAM_OPCODE_CCM_IN_HW;
    mep_acc_info->level = mdl;     /*Y1731 level*/
    mep_acc_info->gport = gport; /* in lif */
    mep_acc_info->mpls_out_gport = out_gport; /* out lif */

    /*TX*/
    if (tx_gport){
        BCM_GPORT_SYSTEM_PORT_ID_SET(mep_acc_info->tx_gport, tx_gport); /* port that the traffic will be transmitted on */
    }
    mep_acc_info->name = 0x1234;
    mep_acc_info->ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;

    mep_acc_info->intf_id = mpls_tunnel_id;
    mep_acc_info->egress_label.label = 3000;  /* Constant label for PWE tunneling */
    mep_acc_info->egress_label.ttl = 0x1e;
    mep_acc_info->egress_label.exp = 1;

    if (bcmOAMEndpointTypeBhhSection == mep_type) {
        rv = config_oam_section_profile(unit, gport, out_gport, is_dual_ended_lm, mep_acc_info);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %d, in config_oam_section_profile.\n", rv);
            return rv;
        }
    } else {
        rv = config_oam_mpls_or_pwe_profile(unit, gport, out_gport, is_dual_ended_lm, mep_acc_info);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %d, in config_oam_mpls_or_pwe_profile.\n", rv);
            return rv;
        }
    }

    printf("bcm_oam_endpoint_create mep_acc_info\n");
    rv = bcm_oam_endpoint_create(unit, mep_acc_info);
    if (rv != BCM_E_NONE) {
        printf("Error rv(%d), in bcm_oam_endpoint_create\n", rv);
        return rv;
    }

    /* 2. Add loss on MEP */
    bcm_oam_loss_t_init(&loss_info);
    loss_info.id = mep_acc_info->id;
    if (is_dual_ended_lm) {
        loss_info.flags = BCM_OAM_LOSS_WITH_ID;
    } else {
        loss_info.flags = BCM_OAM_LOSS_WITH_ID | BCM_OAM_LOSS_SINGLE_ENDED;
    }

    if (bcmOAMEndpointTypeBHHPwe == mep_type) {
        loss_id = 65552;
    } else if (bcmOAMEndpointTypeBHHMPLS == mep_type) {
        loss_id = 65556;
    } else {
        loss_id = 65560;
    }
    loss_info.loss_id = loss_id;

    loss_info.period = 0;
    printf("bcm_oam_loss_add loss_info\n");
    rv = bcm_oam_loss_add(unit, &loss_info);
    if (rv != BCM_E_NONE) {
        printf("Error rv(%d), in bcm_oam_loss_add\n", rv);
        return rv;
    }

    /* 3. Add delay on MEP */
    bcm_oam_delay_t_init(&delay_info);
    delay_info.id = mep_acc_info->id;
    delay_info.flags = 0;
    delay_info.timestamp_format = bcmOAMTimestampFormatIEEE1588v1;
    delay_info.period = 0;
    printf("bcm_oam_delay_add delay_info\n");
    rv = bcm_oam_delay_add(unit, &delay_info);
    if (rv != BCM_E_NONE) {
        printf("Error rv(%d), in bcm_oam_delay_add\n", rv);
        return rv;
    }

    return rv;
}

/*
 * 1. Example of HLM in egress with fec injection
 */
int
oam_run_hlm_egress_with_fec_egress_profile_create (
    int unit)
{
    bcm_error_t rv;

    rv = bcm_oam_profile_create(unit, 0, bcmOAMProfileEgressLIF, &egress_profile_id_mpls_tp);
    if (BCM_FAILURE(rv))
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_oam_profile_create failed. Error:%d (%s)\n"), rv, bcm_errmsg(rv)));
        return rv;
    }

    rv = bcm_oam_profile_create(unit, 0, bcmOAMProfileEgressLIF, &egress_profile_id_mpls_section);
    if (BCM_FAILURE(rv))
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_oam_profile_create failed. Error:%d (%s)\n"), rv, bcm_errmsg(rv)));
        return rv;
    }

    egress_profile_id_pwe = 1 | bcmOAMProfileEgressLIF<<24 | 1<<31;

    return rv;
}


/*
 * Add l2 addr for data traffic to pwe.
 */
int
oam_run_hlm_egress_with_fec_pwe_l2_addr_add (
    int unit,
    bcm_vpn_t vpn)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t l2addr;
    bcm_gport_t fec_gport;

    bcm_l2_addr_t_init(&l2addr, pwe_mac_addr, vpn);
    l2addr.flags |= BCM_L2_STATIC;
    BCM_GPORT_FORWARD_PORT_SET(fec_gport, cint_vpls_fec_info.mpls_encap_fec_id);
    l2addr.port = fec_gport;

    rv = bcm_l2_addr_add(unit, l2addr);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in bcm_l2_addr_add\n", rv);
        return rv;
    }

    printf("Added l2 addr with vsi(%d) fec_id(0x%8.8x) \n", vpn, cint_vpls_fec_info.mpls_encap_fec_id);

    return rv;
}

/*
 * Create PWE MEP for HLM in egress with fec injection.
 */
int
oam_run_hlm_egress_with_fec_pwe_mep_create (
    int unit,
    int is_mep_with_id)
{
    bcm_error_t rv;
    bcm_oam_endpoint_info_t mep_acc_info;
    int mep_id = 0;
    int mep_type;
    int gport;
    int mpls_tunnel_gport;
    int mpls_out_gport;
    int tx_port;
    int counter_base;
    int counter_if;

    counter_if = lm_counter_if_pwe;
    rv = set_counter_resource(unit, lm_pwe_port, counter_if, 1, &lm_counter_base_id_pwe);
    BCM_IF_ERROR_RETURN(rv);

    /*
     * Adding acc PWE MEP
     */
    mep_type = bcmOAMEndpointTypeBHHPwe;
    gport = BCM_GPORT_INVALID;
    mpls_out_gport = cint_vpls_fec_info.mpls_port_id_egress;
    mpls_tunnel_gport = cint_vpls_fec_info.mpls_encap_fec_id;
    BCM_L3_ITF_SET(mpls_tunnel_gport, BCM_L3_ITF_TYPE_FEC, mpls_tunnel_gport);
    tx_port = 0;
    counter_if = lm_counter_if_pwe;
    counter_base = lm_counter_base_id_pwe;
    mep_id = is_mep_with_id ? mep_acc_pwe_info.id : 0;;
    rv = oam_mep_over_tunnel_create(unit, &mep_acc_info, mep_type, group_info_short_ma.id, counter_base, counter_if,
             gport, mpls_out_gport, mpls_tunnel_gport, tx_port,
             is_mep_with_id, mep_id, mdl_pwe, is_dual_ended_lm);
    if (rv != BCM_E_NONE) {
        printf("Error rv(%d), in bcm_oam_endpoint_create\n", rv);
        return rv;
    }

    sal_memcpy(&mep_acc_pwe_info, &mep_acc_info, sizeof(mep_acc_info));
    printf("created acc MEP for PWE with id %d\n", mep_acc_info.id);

    return rv;
}

/*
 * Create mpls-tp MEP for HLM in egress with fec injection.
 */
int
oam_run_hlm_egress_with_fec_mpls_tp_mep_create (
    int unit,
    int is_mep_with_id)
{
    bcm_error_t rv;
    bcm_oam_endpoint_info_t mep_acc_info;
    int mep_id = 0;
    int mep_type;
    int gport;
    int mpls_tunnel_gport;
    int mpls_out_gport;
    int tx_port;
    int counter_base;
    int counter_if;


    counter_if = lm_counter_if_mpls;
    rv = set_counter_resource(unit, lm_pwe_port, counter_if, 1, &lm_counter_base_id_mpls);
    BCM_IF_ERROR_RETURN(rv);

    /*
     * Adding two acc MEPs, one for MPLS-TP backup, another for MPLS-TP primary
     */

    mep_type = bcmOAMEndpointTypeBHHMPLS;
    gport = BCM_GPORT_INVALID;
    mpls_out_gport = cint_vpls_fec_info.mpls_tunnel_id;
    BCM_GPORT_TUNNEL_ID_SET(mpls_out_gport, mpls_out_gport);
    mpls_tunnel_gport = cint_vpls_fec_info.tunnel_encap_fec_id;
    BCM_L3_ITF_SET(mpls_tunnel_gport, BCM_L3_ITF_TYPE_FEC, mpls_tunnel_gport);
    tx_port = 0;
    counter_if = lm_counter_if_mpls;
    counter_base = lm_counter_base_id_mpls;
    mep_id = is_mep_with_id ? mep_acc_mpls_info.id : 0;
    rv = oam_mep_over_tunnel_create(unit, &mep_acc_info, mep_type, group_info_short_ma.id, counter_base, counter_if,
             gport, mpls_out_gport, mpls_tunnel_gport, tx_port,
             is_mep_with_id, mep_id, mdl_mpls, is_dual_ended_lm);
    if (rv != BCM_E_NONE) {
        printf("Error rv(%d), in bcm_oam_endpoint_create\n", rv);
        return rv;
    }

    sal_memcpy(&mep_acc_mpls_info, &mep_acc_info, sizeof(mep_acc_info));
    printf("created acc MEP for MPLS-TP with id %d\n", mep_acc_info.id);


    return rv;
}


/*
 * Create mpls-section MEP for HLM in egress with fec injection.
 */
int
oam_run_hlm_egress_with_fec_mpls_section_mep_create (
    int unit,
    int is_mep_with_id)
{
    bcm_error_t rv;
    bcm_oam_endpoint_info_t mep_acc_info;
    int mep_id = 0;
    int mep_type;
    int gport;
    int mpls_tunnel_gport;
    int mpls_out_gport;
    int tx_port;
    int counter_base;
    int counter_if;

printf ("Enter oam_run_hlm_egress_with_fec_mpls_section_mep_create\n");
    counter_if = lm_counter_if_section;
    rv = set_counter_resource(unit, lm_pwe_port, counter_if, 1, &lm_counter_base_id_section);
    BCM_IF_ERROR_RETURN(rv);

    /*
     * Adding MPLS-SECTION acc MEP
     */
    mep_type = bcmOAMEndpointTypeBhhSection;
    gport = BCM_GPORT_INVALID;
    mpls_out_gport = cint_vpls_fec_info.core_arp_id;
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(mpls_out_gport, mpls_out_gport);
    mpls_tunnel_gport = cint_vpls_fec_info.core_arp_id;
    tx_port = lm_pwe_port;
    counter_if = lm_counter_if_section;
    counter_base = lm_counter_base_id_section ;
    mep_id = is_mep_with_id ? mep_acc_section_info.id : 0;
    rv = oam_mep_over_tunnel_create(unit, &mep_acc_info, mep_type, group_info_short_ma.id, counter_base, counter_if,
             gport, mpls_out_gport, mpls_tunnel_gport, tx_port,
             is_mep_with_id, mep_id, mdl_section, is_dual_ended_lm);
    if (rv != BCM_E_NONE) {
        printf("Error rv(%d), in bcm_oam_endpoint_create\n", rv);
        return rv;
    }

    sal_memcpy(&mep_acc_section_info, &mep_acc_info, sizeof(mep_acc_info));
    printf("created acc MEP for MPLS-SECTION  with id %d\n", mep_acc_info.id);


    return rv;
}

/*
 * 1. Example of HLM in egress with fec injection
 */
int
oam_run_hlm_egress_with_update_counter_in_egr_profile (
    int unit,
    int profile_id,
    int opcode,
    int counter_enable)
{
    bcm_error_t rv;
    uint32 flags = 0;
    bcm_oam_action_key_t    oam_action_key;
    bcm_oam_action_result_t oam_action_result;
    int i = 0;

    for (i = 0; i < 2; i++) {
    oam_action_key.dest_mac_type = bcmOAMDestMacTypeUknownUcast;
    oam_action_key.endpoint_type = bcmOAMMatchTypePassive;
    oam_action_key.inject = 1;
    oam_action_key.opcode = opcode;

    /*
     * Get original profile configuration
     */
    rv = bcm_oam_profile_action_get(unit, flags, profile_id, &oam_action_key, &oam_action_result);
    if (rv != BCM_E_NONE) {
        printf("Error rv, in bcm_oam_profile_action_get\n");
        return rv;
    }

    oam_action_result.counter_increment = counter_enable;
    oam_action_result.destination = BCM_GPORT_INVALID;

    /*
     * Set new profile configuration
     */
    rv = bcm_oam_profile_action_set(unit, flags, profile_id, &oam_action_key, &oam_action_result);
    if (rv != BCM_E_NONE) {
        printf("Error rv, in bcm_oam_profile_action_set\n");
        return rv;
    }
    }

    printf("set counter enable(%d) for profile(%d)\n", counter_enable, profile_id);

    return rv;
}

/*
 * 1. Example of HLM in egress with fec injection
 */
int
oam_run_hlm_egress_with_update_counter_in_egr_acc_profile (
    int unit,
    int opcode,
    int counter_enable)
{
    bcm_error_t rv;
    uint32 flags = 0;
    bcm_oam_action_key_t    oam_action_key;
    bcm_oam_action_result_t oam_action_result;
    int reserved_acc_egr_profile_id = *(dnxc_data_get(unit, "oam", "general", "oam_egr_acc_action_profile_id_for_inject_mpls", NULL));
    int profile_id = 0;

    bcm_oam_action_key_t_init(&oam_action_key);
    bcm_oam_action_result_t_init(&oam_action_result);

    profile_id = reserved_acc_egr_profile_id | bcmOAMProfileEgressAcceleratedEndpoint<<24 | 1<<31;;

    oam_action_key.dest_mac_type = bcmOAMDestMacTypeUknownUcast;
    oam_action_key.endpoint_type = bcmOAMMatchTypeMEP;
    oam_action_key.inject = 1;
    oam_action_key.opcode = opcode;

    /*
     * Get original profile configuration
     */
    rv = bcm_oam_profile_action_get(unit, flags, profile_id, &oam_action_key, &oam_action_result);
    if (rv != BCM_E_NONE) {
        printf("Error rv, in bcm_oam_profile_action_get\n");
        return rv;
    }

    oam_action_result.counter_increment = counter_enable;
    oam_action_result.destination = BCM_GPORT_INVALID;

    /*
     * Set new profile configuration
     */
    rv = bcm_oam_profile_action_set(unit, flags, profile_id, &oam_action_key, &oam_action_result);
    if (rv != BCM_E_NONE) {
        printf("Error rv, in bcm_oam_profile_action_set\n");
        return rv;
    }

    return rv;
}

/*
 * 1. Example of HLM in egress with fec injection
 */
int
oam_run_hlm_egress_with_fec_injection (
    int unit,
    int ac_port,
    int pwe_port)
{
    bcm_error_t rv;
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t value;

    bcm_oam_group_info_t *group_info;
    uint8 short_name[BCM_OAM_GROUP_NAME_LENGTH] = {1, 3, 2, 0xab, 0xcd};

    /*
     * Setup VPLS fec injection model
     */
    rv = vpls_fec_main(unit, ac_port, pwe_port);
    if (rv != BCM_E_NONE) {
        printf("Error rv(%d), in vpls_fec_main\n", rv);
        return rv;
    }
    lm_pwe_port = pwe_port;

    bcm_oam_group_info_t_init(&group_info_short_ma);
    sal_memcpy(group_info_short_ma.name, short_name, BCM_OAM_GROUP_NAME_LENGTH);
    rv = bcm_oam_group_create(unit, &group_info_short_ma);
    if (rv != BCM_E_NONE) {
        printf("Error rv(%d), in bcm_oam_group_create.\n", rv);
        return rv;
    }
    group_info = &group_info_short_ma;
    printf("Created group short name format\n");

    rv = oam_run_hlm_egress_with_fec_egress_profile_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error rv(%d), in oam_run_hlm_egress_with_fec_egress_profile_create.\n", rv);
        return rv;
    }

    rv = oam_set_of_action_profiles_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error rv(%d), in oam_set_of_action_profiles_create.\n", rv);
        return rv;
    }

    rv = bcm_oam_mpls_tp_channel_type_tx_set(unit, bcmOamMplsTpChannelPweonoam, 0x8902);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in bcm_oam_mpls_tp_channel_type_tx_set.\n", rv);
        return rv;
    }

    /*
     * Adding two acc MEPs, one for PWE backup, another for PWE primary
     */
    rv = oam_run_hlm_egress_with_fec_pwe_mep_create(unit, 0);
    if (rv != BCM_E_NONE) {
        printf("Error rv(%d), in oam_run_hlm_egress_with_fec_pwe_mep_create\n", rv);
        return rv;
    }

    /*
     * Adding two acc MEPs, one for MPLS-TP backup, another for MPLS-TP primary
     */
    rv = oam_run_hlm_egress_with_fec_mpls_tp_mep_create(unit, 0);
    if (rv != BCM_E_NONE) {
        printf("Error rv(%d), in oam_run_hlm_egress_with_fec_mpls_tp_mep_create\n", rv);
        return rv;
    }

    /*
     * Adding two acc MEPs, one for MPLS-SECTION backup, another for MPLS-SECTION primary
     */
    rv = oam_run_hlm_egress_with_fec_mpls_section_mep_create(unit, 0);
    if (rv != BCM_E_NONE) {
        printf("Error rv(%d), in oam_run_hlm_egress_with_fec_mpls_section_mep_create\n", rv);
        return rv;
    }

    /*
     * Adding l2 addr
     */
    rv = oam_run_hlm_egress_with_fec_pwe_l2_addr_add(unit, cint_vpls_fec_info.vpn);
    if (rv != BCM_E_NONE) {
        printf("Error rv(%d), in oam_run_hlm_egress_with_fec_pwe_l2_addr_add\n", rv);
        return rv;
    }

    /*
     * update counter increase
     */
    rv = oam_run_hlm_egress_with_update_counter_in_egr_acc_profile(unit, bcmOamOpcodeCCM, 1);
    if (rv != BCM_E_NONE) {
        printf("Error rv, in oam_run_hlm_egress_with_update_counter_in_egr_acc_profile\n");
        return rv;
    }

    rv = oam_run_hlm_egress_with_update_counter_in_egr_acc_profile(unit, bcmOamOpcodeLMM, 0);
    if (rv != BCM_E_NONE) {
        printf("Error rv, in oam_run_hlm_egress_with_update_counter_in_egr_acc_profile\n");
        return rv;
    }

    rv = oam_run_hlm_egress_with_update_counter_in_egr_acc_profile(unit, bcmOamOpcodeDMM, 0);
    if (rv != BCM_E_NONE) {
        printf("Error rv, in oam_run_hlm_egress_with_update_counter_in_egr_acc_profile\n");
        return rv;
    }

    /*
     * Speculate CW protocol aftre the MPLS stack
     */
    key.type = bcmSwitchMplsSpeculativeNibbleMap;
    key.index = 1;
    value.value = bcmSwitchMplsNextProtocolControlWord;
    rv = bcm_switch_control_indexed_set (unit, key, value);
    if (rv != BCM_E_NONE) {
        printf("Error rv, in bcm_switch_control_indexed_set\n");
        return rv;
    }

    return rv;
}


