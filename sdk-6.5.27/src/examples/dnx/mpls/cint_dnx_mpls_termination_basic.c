/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2022 Broadcom Inc. All rights reserved. File: cint_mpls_basic.c Purpose: Various examples for MPLS/VPLS. 
 */

/*
 *  
 * Configuration:
 *  
 * Test Scenario - start
 * cint;                                                                  
 * cint_reset();                                                          
 * exit; 
  cint ../../../../src/examples/sand/utility/cint_dnx_utils_global.c
  cint ../../../../src/examples/sand/cint_dnx_ip_route_basic.c
  cint ../../../../src/examples/sand/cint_mpls_termination_basic.c
  cint ../../../../src/examples/dnx/field/cint_field_epmf_cs_outlif_profile.c
  cint                                                                   
  int unit = 0; 
  int rv = 0; 
  int in_port = 200; 
  int out_port = 201;
  rv = mpls_termination_basic_main(unit,in_port,out_port);
  print rv; 
 * Test Scenario - end
 * 
 *  Scenario configured in this cint:
 * 
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~
 *   Basic MPLS Termination
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~
 *  One MPLS label is terminated. Lookup in routing table results in  IP forwarding.
 *
 *  Traffic:
 * 
 *  Send the following traffic:
 *
 *  ##############################################################################################
 *  Sending packet from port == 200 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA      | SA           ||   MPLS   ||  SIP     ||  DIP          ||  TTL ||
 *   |    |0C:00:02:00:00|11:00:00:01:12||Label:6000||160.0.0.17||160.161.161.162|| 64   ||
 *   |    |              |              ||Exp:0     ||          ||               ||      ||
 *   |    |              |              ||TTL:20    ||          ||               ||      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *  Receiving packet on port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA      | SA           ||  SIP     ||  DIP          ||  TTL ||
 *   |    |00:00:00:CD:1D|0C:00:02:00:01||160.0.0.17||160.161.161.162|| 63   ||
 *   |    |              |              ||          ||               ||      ||
 *   |    |              |              ||          ||               ||      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ###############################################################################################
 *
 *  Terminate 5 label by extended termination flow:
 *  This Scenario is only valid at JR2 device
 *  Note: If only two labels can be terminated in ingress pipe, 3 passes required.
 *  THerefore, in the example below, two recycle ports will be configured and the packet will do 3 passes in the device.
 * Configuration:
 *
 * Test Scenario - start
  config add tm_port_header_type_in_40=RCH_0 (or IBCH1_MODE, if supported)
  config add tm_port_header_type_out_40=ETH
  config add ucode_port_40=RCY.0:core_0.40
 * If 2 recycle ports required:
  config add tm_port_header_type_in_41=RCH_0 (or IBCH1_MODE, if supported)
  config add tm_port_header_type_out_41=ETH
  config add ucode_port_41=RCY.0:core_0.41
  tr 141
 *
 * cint;
 * cint_reset();
 * exit; 
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_global.c
  cint ../../../../src/examples/dnx/l3/cint_dnx_ip_route_basic.c
  cint ../../../../src/examples/sand/cint_mpls_termination_basic.c
  cint ../../../../src/examples/dnx/mpls/cint_mpls_extended_term_flow_field.c
  cint
  int unit = 0; 
  int rv = 0; 
  int in_port = 200; 
  int out_port = 201;
  print mpls_termination_basic_main(unit,in_port,out_port);
  print mpls_field_extended_term_flow_set_ipmf1(unit, in_port, rcy_port, cint_termination_mpls_basic_info.rcy_extended_term_encap_id); 
 * Test Scenario - end
 *
 *  Traffic:
 * 
 *  Send the following traffic:
 *
 *  ##############################################################################################
 *  Sending packet from port == 200 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA      | SA           ||   MPLS   ||   MPLS   ||   MPLS   ||   MPLS   ||   MPLS   ||  SIP     ||  DIP          ||  TTL ||
 *   |    |0C:00:02:00:00|11:00:00:01:12||Label:6000||Label:6001||Label:6002||Label:6003||Label:6004||160.0.0.17||160.161.161.162|| 64   ||
 *   |    |              |              ||Exp:0     ||Exp:0     ||Exp:0     ||Exp:0     ||Exp:0     ||          ||               ||      ||
 *   |    |              |              ||TTL:20    ||TTL:20    ||TTL:20    ||TTL:20    ||TTL:20    ||          ||               ||      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *  Receiving packet on port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA      | SA           ||  SIP     ||  DIP          ||  TTL ||
 *   |    |00:00:00:CD:1D|0C:00:02:00:01||160.0.0.17||160.161.161.162|| 63   ||
 *   |    |              |              ||          ||               ||      ||
 *   |    |              |              ||          ||               ||      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ###############################################################################################
 *
 */

int cint_termination_mpls_basic_l3_eg_map_id=0;
int fec_id_deviation = 0;
int udf_nof_labels = 0;
int mpls_term_nof_labels = 4;

struct cint_termination_mpls_basic_info_s
{
    int in_port;                /* incoming port */
    int out_port;               /* outgoing port */
    int intf_in;                /* in RIF */
    int intf_out;               /* out RIF */
    bcm_mac_t intf_in_mac_address;      /* mac for tin RIF */
    bcm_mac_t intf_out_mac_address;     /* mac for out RIF */
    bcm_mac_t arp_next_hop_mac; /* mac for next hop */
    int vrf;                    /* VRF */
    int arp_id;                 /* Id for ARP entry */
    bcm_ip_t dip;               /* dip 1 */
    uint32 mask;                /* mask for dip */
    int fec_id;                 /* fec id for termination scenario */
    bcm_mpls_label_t termination_stack[8];      /* stack of labels to be terminated */
    bcm_gport_t tunnel_id[8];   /* stack of tunnel_id created for the labels */
    bcm_if_t tunnel_if;         /* RIF, not relevant only for Jericho2 devices */
    bcm_mpls_label_t extend_term_label[2]; /*  extended term labels */
    bcm_if_t rcy_extended_term_encap_id; /* Encapsulation indexes of the recycle extended term */
    int vmpi;                    /*mpls vmpi*/
};

cint_termination_mpls_basic_info_s cint_termination_mpls_basic_info =
    /*
     * ports : in_port | out_port 
     */
{ 200, 201,
    /*
     * intf_in | intf_out 
     */
    40, 50,
    /*
     * intf_in_mac_address | intf_out_mac_address | arp_next_hop_mac | 
     */
    {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}, {0x00, 0x0c, 0x00, 0x02, 0x00, 0x01}, {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d},
    /*
     * vrf 
     */
    100,
    /*
     * arp_id 
     */
    39000,
    /*
     * dip
     */
    0xA0A1A1A2 /* 160.161.161.162 */ ,
    /*
     * mask
     */
    0xfffffff0,
    /*
     * fec_id 
     */
    0x2000c35B,
    /*
     * termination_stack 
     */
    {6000, 6001, 6002, 6003, 6004, 6005, 6006, 6007},
    /*
     * tunnel_term_id stack
     */
    {-1, -1, -1, -1, -1, -1, -1, -1},
    /*
     * tunnel_if
     */
    20,
    /*
     * extend_term_label
     */
    {0,0},
    /*
     * rcy_extended_term_encap_id
     */
    0,
    /*
     * vmpi
     */
    0,

};

/*
 * Main function for basic mpls termination scnenario in Jericho 2: Terminate up to 4 labels
 */
int
mpls_termination_basic_main(
    int unit,
    int port1,
    int port2)
{
    int is_qux;
    int fec_direction = 0;

    cint_termination_mpls_basic_info.in_port = port1;
    cint_termination_mpls_basic_info.out_port = port2;

    /**
     * Adjust the FEC ID for legacy device
     */
    if (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        fec_direction = 0;
    }
    else
    {
        fec_direction = 1;
    }
    BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, fec_direction, &cint_termination_mpls_basic_info.fec_id), "");
    cint_termination_mpls_basic_info.fec_id += udf_nof_labels * 2;

    /*
     * Configure port properties for this application 
     */
    BCM_IF_ERROR_RETURN_MSG(mpls_termination_basic_configure_port_properties(unit), "");

    /*
     * Configure L3 interfaces 
     */
    BCM_IF_ERROR_RETURN_MSG(mpls_termination_basic_create_l3_interfaces(unit), "");

    /*
     * Configure an ARP entry 
     */
    BCM_IF_ERROR_RETURN_MSG(mpls_termination_basic_create_arp_entry(unit), "");

    /*
     * Configure fec entries for all scenarios 
     */
    BCM_IF_ERROR_RETURN_MSG(mpls_termination_basic_create_fec_entry(unit), "");

    mpls_term_nof_labels = udf_nof_labels ? udf_nof_labels : mpls_term_nof_labels;
    BCM_IF_ERROR_RETURN_MSG(mpls_termination_basic_create_termination_stack(unit, mpls_term_nof_labels), "");
    
    /*
     * Create a l3 forwarding entry 
     */
    BCM_IF_ERROR_RETURN_MSG(mpls_termination_basic_create_l3_forwarding(unit), "");
    
    return BCM_E_NONE;
}

int
mpls_termination_basic_configure_port_properties(
    int unit)
{

    /*
     * Set In-Port to In ETh-RIF 
     */
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, cint_termination_mpls_basic_info.in_port, cint_termination_mpls_basic_info.intf_in), "intf_in");

    /*
     * Set Out-Port default properties 
     */
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, cint_termination_mpls_basic_info.out_port), "intf_out");

    return BCM_E_NONE;
}

int
mpls_termination_basic_create_l3_interfaces(
    int unit)
{
    l3_ingress_intf ingr_itf;
    l3_ingress_intf_init(&ingr_itf);

    /*
     * Create ETH-RIF and set its properties 
     */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_termination_mpls_basic_info.intf_in,
                             cint_termination_mpls_basic_info.intf_in_mac_address), "intf_in");
    
    if (cint_termination_mpls_basic_l3_eg_map_id == 0) {
        BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_termination_mpls_basic_info.intf_out,
            cint_termination_mpls_basic_info.intf_out_mac_address), "intf_out");
    } else {
        BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create_with_qos(unit, cint_termination_mpls_basic_info.intf_out, cint_termination_mpls_basic_l3_eg_map_id,
                             cint_termination_mpls_basic_info.intf_out_mac_address), "intf_out");
    }


    return BCM_E_NONE;
}

int
mpls_termination_basic_create_fec_entry(
    int unit)
{
    int flags2 = 0;

    if (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }

    /*
     * Create FEC for termination scenario; 
     * 1) Give a fec id. 
     * 2) Give the out rif. 
     * 3) Give the arp id. 
     * 4) Give the out port. 
     */
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, cint_termination_mpls_basic_info.fec_id,
                                     cint_termination_mpls_basic_info.intf_out, cint_termination_mpls_basic_info.arp_id,
                                     cint_termination_mpls_basic_info.out_port, 0, flags2),
                                     "create egress object FEC only");

    return BCM_E_NONE;
}

int
mpls_termination_basic_create_arp_entry(
    int unit)
{

    /*
     * Configure ARP entry 
     */
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create(unit, 0, &(cint_termination_mpls_basic_info.arp_id),
                                       cint_termination_mpls_basic_info.arp_next_hop_mac,
                                       cint_termination_mpls_basic_info.intf_out),
                                       "create egress object ARP only");

    return BCM_E_NONE;

}

int
mpls_termination_basic_create_termination_tunnel(
    int unit,
    bcm_mpls_label_t terminated_label,
    bcm_gport_t * tunnel_id)
{

    bcm_mpls_tunnel_switch_t entry;
    bcm_if_t l3_intf_id;
    l3_ingress_intf ingr_itf;
    l3_ingress_intf_init(&ingr_itf);

    bcm_mpls_tunnel_switch_t_init(&entry);

    entry.action = BCM_MPLS_SWITCH_ACTION_POP;
    entry.class_id = cint_termination_mpls_basic_info.vmpi;
    /*
     * incoming label 
     */
    entry.label = terminated_label;

    if ((terminated_label == cint_termination_mpls_basic_info.extend_term_label[0]) ||
        (terminated_label == cint_termination_mpls_basic_info.extend_term_label[1]))
    {
        entry.flags2 |= BCM_MPLS_SWITCH2_EXTENDED_TERMINATION;
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_mpls_tunnel_switch_create(unit, &entry), "");

    *tunnel_id = entry.tunnel_id;

    /*
     * Set Incoming Tunnel-RIF properties 
     */
    /* In case of J2 the RIF is the mpls tunnel id */
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(l3_intf_id, entry.tunnel_id);
    ingr_itf.intf_id = l3_intf_id;

    ingr_itf.vrf = cint_termination_mpls_basic_info.vrf;
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingr_itf), "");

    return BCM_E_NONE;

}

int
mpls_termination_basic_create_termination_stack(
    int unit,
    int nof_labels)
{
    int i;
    bcm_l2_egress_t recycle_entry;
    uint32 egress_filters_enable_per_recycle_port_supported = *dnxc_data_get(unit, "port_pp", "filters", "egress_filters_enable_per_recycle_port_supported", NULL);

    /** assume that if extend_term_label is set, last stage is not allowed to do MPLS term. (this is not mandatory)  */
    /** in any case, if extended termination is set, need to create RCH header */
    if (cint_termination_mpls_basic_info.extend_term_label[0])
    {
        /* Set before second stage parsing as MPLS last termination stage */
        BCM_IF_ERROR_RETURN_MSG(bcm_switch_control_set(unit, bcmSwitchMplsLastTermMode, 1),
            "with type == bcmSwitchMplsLastTermMode");
        bcm_l2_egress_t_init(&recycle_entry);
        recycle_entry.flags = BCM_L2_EGRESS_RECYCLE_HEADER;
        recycle_entry.recycle_app = bcmL2EgressRecycleAppExtendedTerm;
        BCM_IF_ERROR_RETURN_MSG(bcm_l2_egress_create(unit, &recycle_entry),
            "with recycle_app is bcmL2EgressRecycleAppExtendedTerm");
        cint_termination_mpls_basic_info.rcy_extended_term_encap_id = recycle_entry.encap_id;
        printf("RCH Extended termination out lif was added. encap_id=%d \n", recycle_entry.encap_id);

        /** need to disable egress filters explicitly for ExtTerm and Drop&Cont, base on the RCH out-lif */
        if (egress_filters_enable_per_recycle_port_supported == 0)
        {
            int cs_profile_id = 5;
            BCM_IF_ERROR_RETURN_MSG(cint_field_epmf_cs_outlif_profile_disable_egress_filters_main(unit, recycle_entry.encap_id, cs_profile_id), "");
        }
    }

    /*
     * Create a stack of MPLS labels to be terminated 
     */
    for (i = 0; i < nof_labels; i++)
    {
        BCM_IF_ERROR_RETURN_MSG(mpls_termination_basic_create_termination_tunnel(unit,
                                                              cint_termination_mpls_basic_info.termination_stack[i],
                                                              &cint_termination_mpls_basic_info.tunnel_id[i]), "");

    }

    return BCM_E_NONE;

}

/*
 * Get tunnel_id stack
 */
int
mpls_termination_basic_tunnel_id_stack_get(
    int unit,
    int nof_labels,
    bcm_gport_t * tunnel_id)
{
    int layer;
    for (layer = 0; layer < nof_labels; layer++)
    {
        tunnel_id[layer] = cint_termination_mpls_basic_info.tunnel_id[layer];
    }

    return BCM_E_NONE;
}

int
mpls_termination_basic_create_l3_forwarding(
    int unit)
{

    /*
     * Create a routing entry : 1) Give the Destination IP (the ip in the exposed header after
     * termination) 2) Give the mask for the sub net of the DIP 3) Give VRF 4) Give the fec id associated with this
     * forwarding instance. 
     */
    if (*dnxc_data_get(unit, "l3", "fwd", "host_entry_support", NULL))
    {
        BCM_IF_ERROR_RETURN_MSG(add_host_ipv4(unit, cint_termination_mpls_basic_info.dip, cint_termination_mpls_basic_info.vrf, cint_termination_mpls_basic_info.fec_id,
                            0     /* arp id */ , 0), "");
    }
    else
    {
        BCM_IF_ERROR_RETURN_MSG(add_route_ipv4(unit, cint_termination_mpls_basic_info.dip, 0xFFFFFFFF, cint_termination_mpls_basic_info.vrf,
                           cint_termination_mpls_basic_info.fec_id), "");
    }

    return BCM_E_NONE;
}

