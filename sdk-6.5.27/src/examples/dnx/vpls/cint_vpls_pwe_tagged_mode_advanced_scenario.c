/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2022 Broadcom Inc. All rights reserved. File: cint_vpls_pwe_tagged_mode_advanced_scenario.c Purpose: basic example for VPLS. PWE tagged mode operations
 */

/*
 *
 * Configuration:
 *
 * Test Scenario - start
 * cint;
 * cint_reset();
 * exit;
  cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
  cint ../../../../src/examples/sand/cint_ip_route_basic.c
  cint ../../../../src/examples/dnx/vpls/cint_vpls_pwe_tagged_mode_advanced_scenario.c
  cint ../../../../src/examples/sand/cint_qos_l2_phb.c
  cint ../../../../src/examples/sand/utility/cint_sand_utils_vlan_translate.c
  cint
  int unit = 0;
  int rv = 0;
  int access_port_1 = 200;
  int access_port_2 = 201;
  int access_port_3 = 202;
  int core_port = 203;
  rv = vpls_pwe_tagged_mode_advanced_scenario_main(unit,access_port_1,access_port_2,access_port_3,core_port);
  print rv;
 * Test Scenario - end
 * 
 *  Scenario configured in this cint:
 *
 *  - Unique PW_Labels are used depending on nof_service_tag it supports. See below chart.
 *  - For each of PW_Labels, unique PW_Lifs are derived both in ingress and egress side
 *  - Single core_port=203 is used as egress port for all kinds on PW_Labels
 *
 *    +------------------------------------+-----------------+----------------------+-----------------------+
 *    |nof_service_tag ||     PW_Label     | Ingress Access  |    Incoming          |     Outgoing          |
 *    |                || (Encapsulation)  |    Port         |      Pkt             |     inner Pkt         |
 *    +------------------------------------+-----------------+----------------------+-----------------------+
 *    |0 (service_1)   ||      3333        |    200          |C_TAG={0x8100,VID=100}|C_TAG={0x8100,VID=100} |
 *    |                ||                  |                 |C_TAG={0x9100,VID=200}|C_TAG={0x9100,VID=200} |
 *    +------------------------------------+-----------------+----------------------+-----------------------+
 *    |1 (service_2)   ||      6666        |    201          |S_TAG={0x8100,VID=300}|S_TAG={0x8100,VID=3000}|
 *    |                ||                  |                 |C_TAG={0x9100,VID=400}|C_TAG={0x9100,VID=400} |
 *    +------------------------------------+-----------------+----------------------+-----------------------+
 *    |2 (service_3)   ||      8888        |    202          |S_TAG={0x8100,VID=300}|S_TAG={0x8100,VID=4000}|
 *    |                ||                  |                 |S_TAG={0x9100,VID=400}|S_TAG={0x9100,VID=2000}|
 *    +------------------------------------+-----------------+----------------------+-----------------------+
 *
 *  - Single core_port=203 is used as ingress port for packets with all kinds of PW_Labels
 *    +------------------------------------+-----------------+-----------------------+-----------------------+
 *    |nof_service_tag ||     PW_Label     | Egress Access   |    Incoming inner     |     Outgoing          |
 *    |                || (Decapsulation)  |    Port         |        Pkt            |        Pkt            |
 *    +------------------------------------+-----------------+-----------------------+-----------------------+
 *    |0 (service_1)   ||      3333        |    200          |C_TAG={0x8100,VID=100} |C_TAG={0x8100,VID=100} |
 *    |                ||                  |                 |C_TAG={0x9100,VID=200} |C_TAG={0x9100,VID=200} |
 *    +------------------------------------+-----------------+-----------------------+-----------------------+
 *    |1 (service_2)   ||      6666        |    201          |S_TAG={0x8100,VID=3000}|S_TAG={0x8100,VID=300} |
 *    |                ||                  |                 |C_TAG={0x9100,VID=400} |C_TAG={0x9100,VID=400} |
 *    +------------------------------------+-----------------+-----------------------+-----------------------+
 *    |2 (service_3)   ||      8888        |    202          |S_TAG={0x8100,VID=4000}|S_TAG={0x8100,VID=300} |
 *    |                ||                  |                 |S_TAG={0x9100,VID=2000}|S_TAG={0x9100,VID=400} |
 *    +------------------------------------+-----------------+-----------------------+-----------------------+
 *
 * - For S-TAG+S-TAG packets, in encapsulation flow S+C_VLAN_Db will be used to derive In_AC_Lif i.e. second S-TAG should be parsed as C-TAG
 *
 *   Variables used in Encapsulation flow
 *   +-------------------------------------+----------------------------------+
 *   |  Objects                            | Variable name                    |
 *   +=====================================+==================================+
 *   |  PW Labels                          |  service_1/2/3_pwe_label         |
 *   +-------------------------------------+----------------------------------+
 *   |  VSIs                               |  service_vsi                     |
 *   +-------------------------------------+----------------------------------+
 *   |  In_Lifs                            |  access_ingress_vlan_port_id     |
 *   +-------------------------------------+----------------------------------+
 *   |  PW Out_Lifs                        |  service_1/2/3_mpls_port_id_egid |
 *   +-------------------------------------+----------------------------------+
 *   |  Native ETH Out_Lifs used in EVE    |  egress_native_ac_vlan_port_id   |
 *   +-------------------------------------+----------------------------------+
 *   |  Core Port Out_Lif                  |  outer_ac_vlan_port_id           |
 *   +-------------------------------------+----------------------------------+
 *
 *   Variables used in Termination flow
 *   +-------------------------------------+----------------------------------+
 *   |  Objects                            | Variable name                    |
 *   +=====================================+==================================+
 *   |  PW Labels                          |  service_1/2/3_pwe_label         |
 *   +-------------------------------------+----------------------------------+
 *   |  VSIs                               |  service_vsi                     |
 *   +-------------------------------------+----------------------------------+
 *   |  Inner AC In_Lifs                   |  ingress_native_ac_vlan_port_id  |
 *   +-------------------------------------+----------------------------------+
 *   |  PW Out_Lifs                        |  service_1/2/3_mpls_port_id_ing  |
 *   +-------------------------------------+----------------------------------+
 *   |  Native ETH Out_Lifs used in EVE    |  access_egress_vlan_port_id      |
 *   +-------------------------------------+----------------------------------+
 *
 * - Unindexed Native AC Lif is used in this cint
 * 
 *   Note: The LLVP configuration that uses various tag-formats assumes the same
 *   configuration for the Outer-Eth and Native-Eth headers. It would be best to
 *   separate those two configurations in order to create a more general example.
 */

int illegal_qos_id = 0xFFFF;

struct cint_vpls_pwe_tagged_mode_advanced_scenarios_info_s
{
    int access_port[3];         /* incoming port from acsess side */
    int core_port;              /* incoming port from core side */
    int core_eth_rif;           /* core RIF */
    bcm_vpn_t service_vsi[3];   /* vsi id */
    bcm_gport_t service_1_mpls_port_id_ing;     /* global lif encoded as MPLS port for service 1 (ingress side) */
    bcm_gport_t service_1_mpls_port_id_eg;      /* global lif encoded as MPLS port for service 1 (egress side) */
    bcm_gport_t service_2_mpls_port_id_ing;     /* global lif encoded as MPLS port for service 2 (ingress side) */
    bcm_gport_t service_2_mpls_port_id_eg;      /* global lif encoded as MPLS port for service 2 (egress side) */
    bcm_gport_t service_3_mpls_port_id_ing;     /* global lif encoded as MPLS port for service 3 (ingress side) */
    bcm_gport_t service_3_mpls_port_id_eg;      /* global lif encoded as MPLS port for service 3 (egress side) */
    bcm_gport_t service_1_vlan_port_id; /* vlan port id of the ac_port, used for ingress */
    bcm_gport_t outer_ac_vlan_port_id;  /* vlan port id of the ac_port, used for ingress */
    bcm_if_t service_1_pwe_encap_id;    /* The global lif of the EEDB PWE entry. */
    bcm_if_t service_2_pwe_encap_id;    /* The global lif of the EEDB PWE entry. */
    bcm_if_t service_3_pwe_encap_id;    /* The global lif of the EEDB PWE entry. */
    int core_arp_id;            /* Id for core ARP entry */
    int service_1_mpls_fec_id;  /* service 1 fec id for encapsulation entry - outlif for MPLS entry in EEDB */
    bcm_mac_t core_eth_rif_mac; /* mac for core RIF */
    bcm_mac_t core_next_hop_mac;        /* mac for next hop */
    bcm_mac_t customer_mac_1;   /* first mac address for service 1 */
    bcm_mac_t customer_mac_2;   /* second mac address for service 1 */
    int outer_tpid;             /* Outer tpid for application */
    int inner_tpid;             /* Inner tpid for application */
    bcm_if_t service_1_mpls_tunnel_id;  /* tunnel id for encapsulation entry */
    bcm_mpls_label_t service_1_mpls_tunnel_label_1;     /* pushed label */
    bcm_mpls_label_t service_1_mpls_tunnel_label_2;     /* pushed label */
    bcm_mpls_label_t service_1_pwe_label;       /* pwe label */
    bcm_mpls_label_t service_2_pwe_label;       /* pwe label */
    bcm_mpls_label_t service_3_pwe_label;       /* pwe label */
    bcm_vlan_t outer_ac_egress_outer_vlan;
    bcm_vlan_t outer_ac_egress_inner_vlan;
    bcm_vlan_t ingress_canonical_outer_vlan[3];
    bcm_vlan_t ingress_canonical_inner_vlan;
    bcm_vlan_t egress_canonical_outer_vlan[3];
    bcm_vlan_t egress_canonical_inner_vlan;
    bcm_port_tag_format_class_t tag_format[3];
    bcm_gport_t ingress_native_ac_vlan_port_id[3];      /* vlan port id of the inner ac_port, used for ingress */
    bcm_gport_t egress_native_ac_vlan_port_id[3];       /* vlan port id of the inner ac_port, used for egress */
    bcm_gport_t access_ingress_vlan_port_id[3]; /* vlan port id of the access port, used in ingress at encapsulation
                                                 * flow */
    bcm_gport_t access_egress_vlan_port_id[3];  /* vlan port id of the access port, used for egress at terination flow */
    uint32 egress_vlan_edit_profile[3];
    uint32 ingress_vlan_edit_profile[3];
    bcm_vlan_t inner_ac_ingress_outer_vlan;
    bcm_vlan_t inner_ac_ingress_inner_vlan;
};

cint_vpls_pwe_tagged_mode_advanced_scenarios_info_s cint_vpls_pwe_tagged_mode_advanced_scenario_info = {
    /*
     * ports : access_port | core_port
     */
    {200, 201, 202}, 203,
    /*
     * core_eth_rif
     */
    30,
    /*
     * service_vsi
     */
    {10, 35, 50},
    /*
     * service_1_mpls_port_id_ing|eg | service_2_mpls_port_id_ing|eg | service_3_mpls_port_id_ing|eg |service_1_vlan_port_id |  outer_ac_vlan_port_id | service_1_pwe_encap_id | service_2_pwe_encap_id | service_3_pwe_encap_id
     */
    9999, 19999, 10000, 20000, 11111, 21111, 8888, 1111, 0, 0, 0,
    /*
     * core_arp_id
     */
    12290,
    /*
     * service_1_mpls_fec_id
     */
    50001,
    /*
     * core_eth_rif_mac
     */
    {0x00, 0x0F, 0x00, 0x02, 0x0A, 0x22},
    /*
     * core_next_hop_mac
     */
    {0x00, 0x00, 0x00, 0x00, 0xCD, 0x1D},
    /*
     *customer_mac_1   |   customer_mac_2
     */
    {0x40, 0x0C, 0x00, 0x02, 0x0C, 0x33}, {0x00, 0x0C, 0x00, 0x02, 0x0C, 0x44},
    /*
     * outer_tpid | inner_tpid
     */
    0x9100, 0x8100,
    /*
     * service_1_mpls_tunnel_id
     */
    16386,
    /*
     * service_1_mpls_tunnel_label_1  | service_1_mpls_tunnel_label_2 |  service_1_pwe_label |service_2_pwe_label |  service_3_pwe_label
     */
    4444, 5555, 3333, 6666, 8888,
    /*
     * outer_ac_egress_outer_vlan | outer_ac_egress_inner_vlan
     */
    327, 427,
    /*
     * ingress_canonical_outer_vlan | ingress_canonical_inner_vlan
     */
    {1000, 3000, 4000}, 2000,
    /*
     * egress_canonical_outer_vlan | egress_canonical_inner_vlan
     */
    {1000, 3000, 4000}, 2000,
    /*
     * tag_format
     */
    {0, 4, 16},
    /*
     * ingress_native_ac_vlan_port_id,egress_native_ac_vlan_port_id,access_ingress_vlan_port_id,access_egress_vlan_port_id
     */
    {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},
    /*
     * egress_vlan_edit_profile, ingress_vlan_edit_profile
     */
    {4, 5, 6}, {5, 6, 7},
    /*
     * inner_ac_ingress_outer_vlan, inner_ac_ingress_inner_vlan
     */
    300, 400
};

int verbose1 = 1;

void
vpls_pwe_tagged_mode_advanced_scenario_init(
    int access_port_1,
    int access_port_2,
    int access_port_3,
    int core_port)
{

    cint_vpls_pwe_tagged_mode_advanced_scenario_info.access_port[0] = access_port_1;
    cint_vpls_pwe_tagged_mode_advanced_scenario_info.access_port[1] = access_port_2;
    cint_vpls_pwe_tagged_mode_advanced_scenario_info.access_port[2] = access_port_3;
    cint_vpls_pwe_tagged_mode_advanced_scenario_info.core_port = core_port;

}

int
vpls_pwe_tagged_mode_advanced_scenario_configure_port_properties(
    int unit)
{
    char error_msg[200]={0,};
    int ac_intf = 100;

    /*
     * set class for both ports
     */
    snprintf(error_msg, sizeof(error_msg), "port=%d", cint_vpls_pwe_tagged_mode_advanced_scenario_info.access_port[0]);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, cint_vpls_pwe_tagged_mode_advanced_scenario_info.access_port[0], bcmPortClassId,
                            cint_vpls_pwe_tagged_mode_advanced_scenario_info.access_port[0]), error_msg);
    snprintf(error_msg, sizeof(error_msg), "port=%d", cint_vpls_pwe_tagged_mode_advanced_scenario_info.access_port[1]);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, cint_vpls_pwe_tagged_mode_advanced_scenario_info.access_port[1], bcmPortClassId,
                            cint_vpls_pwe_tagged_mode_advanced_scenario_info.access_port[1]), error_msg);
    snprintf(error_msg, sizeof(error_msg), "port=%d", cint_vpls_pwe_tagged_mode_advanced_scenario_info.access_port[2]);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, cint_vpls_pwe_tagged_mode_advanced_scenario_info.access_port[2], bcmPortClassId,
                            cint_vpls_pwe_tagged_mode_advanced_scenario_info.access_port[2]), error_msg);

    snprintf(error_msg, sizeof(error_msg), "port=%d", cint_vpls_pwe_tagged_mode_advanced_scenario_info.core_port);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, cint_vpls_pwe_tagged_mode_advanced_scenario_info.core_port, bcmPortClassId,
                            cint_vpls_pwe_tagged_mode_advanced_scenario_info.core_port), error_msg);

    /*
     * Create In_AC_Lif based on access_port[0] 
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_advanced_scenario_in_port_intf(unit,
                cint_vpls_pwe_tagged_mode_advanced_scenario_info.access_port[0],
                cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_vsi[0],
                cint_vpls_pwe_tagged_mode_advanced_scenario_info.inner_ac_ingress_outer_vlan), "");

    /*
     * Create In_AC_Lif based on S_Tag of incoming packet on access_port 
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_advanced_scenario_in_port_intf_set_s_tag(unit,
                cint_vpls_pwe_tagged_mode_advanced_scenario_info.access_port[1],
                cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_vsi[1],
                cint_vpls_pwe_tagged_mode_advanced_scenario_info.inner_ac_ingress_outer_vlan), "");

    /*
     * Set tag_format and TPID values on access_port
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_advanced_scenario_ive_delete_stag(unit,
                cint_vpls_pwe_tagged_mode_advanced_scenario_info.access_port[1]), "");

    /*
     * Create In_AC_Lif based on S_Tag+C_Tag(actually C_Tag here is second S-tag in packet) of incoming packet on
     * access_port. As JR2 don't have separate S+S_VLAN_Db to derive In_AC_Lif, S+C_VLAN_Db is used for S+S_Tag kind of 
     * packets
     */
    BCM_IF_ERROR_RETURN_MSG(cint_vpls_pwe_tagged_mode_advanced_scenario_in_port_intf_set_s_c_tag(unit,
                cint_vpls_pwe_tagged_mode_advanced_scenario_info.access_port[2],
                cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_vsi[2],
                cint_vpls_pwe_tagged_mode_advanced_scenario_info.inner_ac_ingress_outer_vlan,
                cint_vpls_pwe_tagged_mode_advanced_scenario_info.inner_ac_ingress_inner_vlan), "");

    /*
     * Set tag_format and TPID values on access_port
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_advanced_scenario_ive_delete_stag_ctag(unit,
                cint_vpls_pwe_tagged_mode_advanced_scenario_info.access_port[2]), "");

    /*
     * Set In-Port to In ETh-RIF
     */
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, cint_vpls_pwe_tagged_mode_advanced_scenario_info.core_port,
                          cint_vpls_pwe_tagged_mode_advanced_scenario_info.core_eth_rif), "intf_in");

    /*
     * Set Out-Port default properties
     */
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, cint_vpls_pwe_tagged_mode_advanced_scenario_info.access_port[0]), "intf_out");

    /*
     * Set tag_format and TPID values on access_port[0]
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_advanced_scenario_eve_none(unit,
                                                         cint_vpls_pwe_tagged_mode_advanced_scenario_info.access_port[0]), "");

    /*
     * Set tag_format and TPID values on access_port[1]
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_advanced_scenario_eve_add_stag(unit,
                                                             cint_vpls_pwe_tagged_mode_advanced_scenario_info.access_port[1]), "");

    /*
     * Set tag_format and TPID values on access_port[2]
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_advanced_scenario_eve_add_stag_ctag(unit,
                                                                  cint_vpls_pwe_tagged_mode_advanced_scenario_info.access_port[2]), "");

    /*
     * Set Out-Port default properties
     */
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, cint_vpls_pwe_tagged_mode_advanced_scenario_info.core_port), "intf_out");

    return BCM_E_NONE;
}

int
vpls_pwe_tagged_mode_advanced_scenario_create_vsis(
    int unit)
{

    char error_msg[200]={0,};

    /*
     * create vlan based on the vsi (vpn)
     */
    snprintf(error_msg, sizeof(error_msg), "Failed to create VLAN %d", cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_vsi[0]);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_create(unit, cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_vsi[0]), error_msg);

    snprintf(error_msg, sizeof(error_msg), "Failed to create VLAN %d", cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_vsi[1]);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_create(unit, cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_vsi[1]), error_msg);
    snprintf(error_msg, sizeof(error_msg), "Failed to create VLAN %d", cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_vsi[2]);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_create(unit, cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_vsi[2]), error_msg);

    /*
     * create vlan based on the vsi (vpn)
     */
    snprintf(error_msg, sizeof(error_msg), "Failed to create VLAN %d", cint_vpls_pwe_tagged_mode_advanced_scenario_info.core_eth_rif);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_create(unit, cint_vpls_pwe_tagged_mode_advanced_scenario_info.core_eth_rif), error_msg);

    return BCM_E_NONE;
}

int
vpls_pwe_tagged_mode_advanced_scenario_create_l3_interfaces(
    int unit)
{


    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_vpls_pwe_tagged_mode_advanced_scenario_info.core_eth_rif,
                             cint_vpls_pwe_tagged_mode_advanced_scenario_info.core_eth_rif_mac), "pwe_intf");

    return BCM_E_NONE;
}

int
vpls_pwe_tagged_mode_advanced_scenario_create_arp_entry(
    int unit)
{

    /*
     * Configure ARP entry
     */
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create(unit, 0, cint_vpls_pwe_tagged_mode_advanced_scenario_info.core_arp_id,
                                       cint_vpls_pwe_tagged_mode_advanced_scenario_info.core_next_hop_mac,
                                       cint_vpls_pwe_tagged_mode_advanced_scenario_info.core_eth_rif),
                                       "create AC egress object ARP only");

    return BCM_E_NONE;

}

int
vpls_pwe_tagged_mode_advanced_scenario_create_fec_entry(
    int unit)
{
    int temp_out_rif;
    int flags = 0;
    int flags2 = 0;
    /**
     * Under below scenarios, this case tests the FEC format of dest+EEI:
     * 1. JR2 works under JR1 system headers mode ;
     */
    if (!dnx_utils_is_jr2_system_header_mode(unit))
    {
        flags |= BCM_L3_ENCAP_SPACE_OPTIMIZED;
    }

    /*
     * Create FEC:
     * 1) Give a fec id.
     * 2) Tunnel id of the encapsulation entry.
     * 3) Arp id will be given with the only_encap call, so give 0.
     * 4) Give the out port.
     */
    if (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_1_mpls_fec_id,
                                     cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_1_mpls_tunnel_id, 0,
                                     cint_vpls_pwe_tagged_mode_advanced_scenario_info.core_port, flags, flags2),
                                     "create egress object FEC only");

    /*
     * The termination scenario doesn't need FEC.
     * We bridge out of the core with no usage of FEC.
     */

    return BCM_E_NONE;
}

int
vpls_pwe_tagged_mode_advanced_scenario_create_mpls_tunnel(
    int unit,
    bcm_mpls_label_t outer_label,
    bcm_mpls_label_t inner_label,
    int num_labels,
    int arp_id,
    bcm_if_t * tunnel_id)
{

    bcm_mpls_egress_label_t label_array[2];

    bcm_mpls_egress_label_t_init(&label_array[0]);
    bcm_mpls_egress_label_t_init(&label_array[1]);

    /*
     * Label to be pushed as part of the MPLS tunnel.
     */
    label_array[0].label = outer_label;

    /*
     * Set the next pointer of this entry to be the arp. This configuration is new compared to Jericho, where the arp
     * pointer used to be connected to the EEDB entry via bcm_l3_egress_create (with egress flag indication).
     */
    label_array[0].l3_intf_id = arp_id;

    label_array[0].flags = BCM_MPLS_EGRESS_LABEL_WITH_ID | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    BCM_L3_ITF_SET(label_array[0].tunnel_id, BCM_L3_ITF_TYPE_LIF, *tunnel_id);
    if (num_labels == 2)
    {
        label_array[1].label = inner_label;
        label_array[1].flags = BCM_MPLS_EGRESS_LABEL_WITH_ID | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
        BCM_L3_ITF_SET(label_array[1].tunnel_id, BCM_L3_ITF_TYPE_LIF, *tunnel_id);
        label_array[1].l3_intf_id = label_array[0].l3_intf_id;
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_mpls_tunnel_initiator_create(unit, 0, num_labels, label_array), "");

    *tunnel_id = label_array[0].tunnel_id;

    return BCM_E_NONE;

}

int
vpls_pwe_tagged_mode_advanced_scenario_create_termination_stack(
    int unit,
    bcm_mpls_label_t * labels,
    int nof_labels)
{
    bcm_mpls_tunnel_switch_t entry;
    int i;
    /*
     * Create a stack of MPLS labels to be terminated
     */

    for (i = 0; i < nof_labels; i++)
    {
        bcm_mpls_tunnel_switch_t_init(&entry);

        entry.action = BCM_MPLS_SWITCH_ACTION_POP;

        /*
         * incoming label
         * only the mpls tunnel label needs to be defined here.
         * pwe label will be handed as part of the ingress mpls_port_add api
         */
        entry.label = labels[i];

        BCM_IF_ERROR_RETURN_MSG(bcm_mpls_tunnel_switch_create(unit, &entry), "");
    }

    return BCM_E_NONE;

}

int
vpls_pwe_tagged_mode_advanced_scenario_l2_addr_add(
    int unit)
{
    char error_msg[200]={0,};

    /*
     * MACT entry insertion for customer_mac_2(DA in Encapsulation flow) + VSI(service[0]) resulting in PWE_label_1
     * encapsulation
     */
    bcm_l2_addr_t l2addr;

    bcm_l2_addr_t_init(&l2addr, cint_vpls_pwe_tagged_mode_advanced_scenario_info.customer_mac_2,
                       cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_vsi[0]);
    l2addr.flags |= BCM_L2_STATIC;
    l2addr.encap_id = cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_1_pwe_encap_id;
    BCM_GPORT_FORWARD_PORT_SET(l2addr.port, cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_1_mpls_fec_id);
    snprintf(error_msg, sizeof(error_msg), "for service_vsi=%d", cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_vsi[0]);
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, l2addr), error_msg);

    /*
     * MACT entry insertion for customer_mac_2(DA in Encapsulation flow) + VSI(service[1]) resulting in PWE_label_2
     * encapsulation
     */
    bcm_l2_addr_t_init(&l2addr, cint_vpls_pwe_tagged_mode_advanced_scenario_info.customer_mac_2,
                       cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_vsi[1]);
    l2addr.flags |= BCM_L2_STATIC;
    l2addr.encap_id = cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_2_pwe_encap_id;
    BCM_GPORT_FORWARD_PORT_SET(l2addr.port, cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_1_mpls_fec_id);
    snprintf(error_msg, sizeof(error_msg), "for service_vsi=%d", cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_vsi[1]);
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, l2addr), error_msg);

    /*
     * MACT entry insertion for customer_mac_2(DA in Encapsulation flow) + VSI(service[2]) resulting in PWE_label_3
     * encapsulation
     */
    bcm_l2_addr_t_init(&l2addr, cint_vpls_pwe_tagged_mode_advanced_scenario_info.customer_mac_2,
                       cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_vsi[2]);
    l2addr.flags |= BCM_L2_STATIC;
    l2addr.encap_id = cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_3_pwe_encap_id;
    BCM_GPORT_FORWARD_PORT_SET(l2addr.port, cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_1_mpls_fec_id);
    snprintf(error_msg, sizeof(error_msg), "for service_vsi=%d", cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_vsi[2]);
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, l2addr), error_msg);

    return BCM_E_NONE;
}

int
vpls_pwe_tagged_mode_advanced_scenario_mpls_port_add_encapsulation(
    int unit)
{
    bcm_mpls_port_t mpls_port;
    bcm_gport_t gport_id;

    bcm_mpls_port_t_init(&mpls_port);
    mpls_port.flags |= BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_EGRESS_TUNNEL;
    mpls_port.flags2 |= BCM_MPLS_PORT2_EGRESS_ONLY | BCM_MPLS_PORT2_PLATFORM_NAMESPACE;

    BCM_GPORT_SUB_TYPE_LIF_SET(gport_id, 0, cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_1_mpls_port_id_eg);
    BCM_GPORT_MPLS_PORT_ID_SET(mpls_port.mpls_port_id, gport_id);
    mpls_port.egress_label.label = cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_1_pwe_label;
    mpls_port.egress_label.flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;

    BCM_IF_ERROR_RETURN_MSG(bcm_mpls_port_add(unit, 0, mpls_port), "");

    cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_1_mpls_port_id_eg = mpls_port.mpls_port_id;
    cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_1_pwe_encap_id = mpls_port.encap_id;

    printf("Encapsulation: PWE_Lif created for Service_1=%x \n",
           cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_1_mpls_port_id_eg);
    printf("Encapsulation: Corresponding Encap_id for Service_1=%x \n",
           cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_1_pwe_encap_id);

    BCM_GPORT_SUB_TYPE_LIF_SET(gport_id, 0, cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_2_mpls_port_id_eg);
    BCM_GPORT_MPLS_PORT_ID_SET(mpls_port.mpls_port_id, gport_id);
    mpls_port.egress_label.label = cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_2_pwe_label;

    BCM_IF_ERROR_RETURN_MSG(bcm_mpls_port_add(unit, 0, mpls_port), "");

    cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_2_mpls_port_id_eg = mpls_port.mpls_port_id;
    cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_2_pwe_encap_id = mpls_port.encap_id;

    printf("Encapsulation: PWE_Lif created for Service_2=%x \n",
           cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_2_mpls_port_id_eg);
    printf("Encapsulation: Corresponding Encap_id for Service_2=%x \n",
           cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_2_pwe_encap_id);

    BCM_GPORT_SUB_TYPE_LIF_SET(gport_id, 0, cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_3_mpls_port_id_eg);
    BCM_GPORT_MPLS_PORT_ID_SET(mpls_port.mpls_port_id, gport_id);
    mpls_port.egress_label.label = cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_3_pwe_label;

    BCM_IF_ERROR_RETURN_MSG(bcm_mpls_port_add(unit, 0, mpls_port), "");

    cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_3_mpls_port_id_eg = mpls_port.mpls_port_id;
    cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_3_pwe_encap_id = mpls_port.encap_id;

    printf("Encapsulation: PWE_Lif created for Service_3=%x \n",
           cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_3_mpls_port_id_eg);
    printf("Encapsulation: Corresponding Encap_id for Service_3=%x \n",
           cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_3_pwe_encap_id);

    return BCM_E_NONE;
}

int
vpls_pwe_tagged_mode_advanced_scenario_mpls_port_add_termination(
    int unit)
{
    bcm_mpls_port_t mpls_port;
    bcm_gport_t gport_id;

    bcm_mpls_port_t_init(&mpls_port);

    mpls_port.flags |= BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_EGRESS_TUNNEL;
    mpls_port.flags2 |= BCM_MPLS_PORT2_INGRESS_ONLY |
            (cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_vsi[0] ? 0 : BCM_MPLS_PORT2_CROSS_CONNECT);
    mpls_port.nof_service_tags = 0;

    BCM_GPORT_SUB_TYPE_LIF_SET(gport_id, 0,
                               cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_1_mpls_port_id_ing);
    BCM_GPORT_MPLS_PORT_ID_SET(mpls_port.mpls_port_id, gport_id);
    /*
     * encap_id is the egress outlif - used for learning
     */
    mpls_port.encap_id = cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_1_pwe_encap_id;
    mpls_port.criteria = BCM_MPLS_PORT_MATCH_LABEL;
    mpls_port.match_label = cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_1_pwe_label;
    /** connect PWE entry to created MPLS encapsulation entry for learning */
    BCM_L3_ITF_SET(mpls_port.egress_tunnel_if, BCM_L3_ITF_TYPE_FEC,
                   cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_1_mpls_fec_id);

    BCM_IF_ERROR_RETURN_MSG(bcm_mpls_port_add(unit, cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_vsi[0], mpls_port),
        "termination service_1_pwe_label");

    cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_1_mpls_port_id_ing = mpls_port.mpls_port_id;

    printf("Termination: PWE_Lif created for Service_1=%x \n",
           cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_1_mpls_port_id_ing);

    /*
     * Add mpls_port which has nof_service_tags=1
     */
    bcm_mpls_port_t_init(&mpls_port);
    mpls_port.flags |= BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_SERVICE_TAGGED | BCM_MPLS_PORT_EGRESS_TUNNEL;
    mpls_port.flags2 |= BCM_MPLS_PORT2_INGRESS_ONLY |
            (cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_vsi[1] ? 0 : BCM_MPLS_PORT2_CROSS_CONNECT);
    mpls_port.nof_service_tags = 1;

    BCM_GPORT_SUB_TYPE_LIF_SET(gport_id, 0,
                               cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_2_mpls_port_id_ing);
    BCM_GPORT_MPLS_PORT_ID_SET(mpls_port.mpls_port_id, gport_id);
    mpls_port.criteria = BCM_MPLS_PORT_MATCH_LABEL;
    mpls_port.encap_id = cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_2_pwe_encap_id;
    mpls_port.match_label = cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_2_pwe_label;
    BCM_L3_ITF_SET(mpls_port.egress_tunnel_if, BCM_L3_ITF_TYPE_FEC,
                   cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_1_mpls_fec_id);

    BCM_IF_ERROR_RETURN_MSG(bcm_mpls_port_add(unit, cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_vsi[1], mpls_port),
        "for service_2_pwe_label");
    cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_2_mpls_port_id_ing = mpls_port.mpls_port_id;
    printf("Termination: PWE_Lif created for Service_2=%x \n",
           cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_2_mpls_port_id_ing);

    /*
     * Add mpls_port which has nof_service_tags=1
     */
    bcm_mpls_port_t_init(&mpls_port);
    mpls_port.flags |= BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_SERVICE_TAGGED | BCM_MPLS_PORT_EGRESS_TUNNEL;
    mpls_port.flags2 |= BCM_MPLS_PORT2_INGRESS_ONLY |
            (cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_vsi[2] ? 0 : BCM_MPLS_PORT2_CROSS_CONNECT);
    mpls_port.nof_service_tags = 2;

    BCM_GPORT_SUB_TYPE_LIF_SET(gport_id, 0,
                               cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_3_mpls_port_id_ing);
    BCM_GPORT_MPLS_PORT_ID_SET(mpls_port.mpls_port_id, gport_id);
    mpls_port.criteria = BCM_MPLS_PORT_MATCH_LABEL;
    mpls_port.encap_id = cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_3_pwe_encap_id;
    mpls_port.match_label = cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_3_pwe_label;
    BCM_L3_ITF_SET(mpls_port.egress_tunnel_if, BCM_L3_ITF_TYPE_FEC,
                   cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_1_mpls_fec_id);

    BCM_IF_ERROR_RETURN_MSG(bcm_mpls_port_add(unit, cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_vsi[2], mpls_port),
        "for service_3_pwe_label");
    cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_3_mpls_port_id_ing = mpls_port.mpls_port_id;
    printf("Termination: PWE_Lif created for Service_3=%x \n",
           cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_3_mpls_port_id_ing);

    return BCM_E_NONE;
}

int
vpls_pwe_tagged_mode_advanced_scenario_vswitch_add_core_outer_ac(
    int unit,
    bcm_gport_t port)
{
    bcm_vlan_port_t vlan_port;
    int is_two_dev = 0;
    /*
     * add port, according to port_vlan_vlan
     */
    bcm_vlan_port_t_init(&vlan_port);

    /*
     * set port attribures, key <port-vlan-vlan>
     */
    vlan_port.port = port;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.vsi = cint_vpls_pwe_tagged_mode_advanced_scenario_info.core_eth_rif;

    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), "");
    cint_vpls_pwe_tagged_mode_advanced_scenario_info.outer_ac_vlan_port_id = vlan_port.vlan_port_id;

    printf("outer_ac_vlan_port_id: 0x%08x\n", cint_vpls_pwe_tagged_mode_advanced_scenario_info.outer_ac_vlan_port_id);

    return BCM_E_NONE;
}

int
vpls_pwe_tagged_mode_advanced_scenario_eve_swap(
    int unit,
    int port)
{
    char error_msg[200]={0,};
    /*
     * Tag_format for EVE of outer L2 header does not depend on inner L2 header's tag format
     */

    snprintf(error_msg, sizeof(error_msg), "port=%d", port);
    BCM_IF_ERROR_RETURN_MSG(vlan_translate_ive_eve_translation_set(unit,
                                                cint_vpls_pwe_tagged_mode_advanced_scenario_info.outer_ac_vlan_port_id,
                                                cint_vpls_pwe_tagged_mode_advanced_scenario_info.outer_tpid,
                                                cint_vpls_pwe_tagged_mode_advanced_scenario_info.inner_tpid,
                                                bcmVlanActionAdd, bcmVlanActionAdd,
                                                cint_vpls_pwe_tagged_mode_advanced_scenario_info.outer_ac_egress_outer_vlan,
                                                cint_vpls_pwe_tagged_mode_advanced_scenario_info.outer_ac_egress_inner_vlan,
                                                8, cint_vpls_pwe_tagged_mode_advanced_scenario_info.tag_format[0], 0),
                                                error_msg);

    bcm_port_tpid_class_t port_tpid_class;
    /*
     * set tag format for c-tag packets
     */
    bcm_port_tpid_class_t_init(&port_tpid_class);
    port_tpid_class.port = port;
    port_tpid_class.tpid1 = cint_vpls_pwe_tagged_mode_advanced_scenario_info.inner_tpid;
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    port_tpid_class.tag_format_class_id = 8;
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY;
    port_tpid_class.vlan_translation_action_id = 0;
    BCM_IF_ERROR_RETURN_MSG(bcm_port_tpid_class_set(unit, &port_tpid_class), "");

    return BCM_E_NONE;
}

int
vpls_pwe_tagged_mode_advanced_scenario_ive_delete_stag(
    int unit,
    int port)
{
    char error_msg[200]={0,};

    bcm_port_tpid_class_t port_tpid_class;
    /*
     * set tag format for D-TAG packets
     */
    bcm_port_tpid_class_t_init(&port_tpid_class);
    port_tpid_class.port = port;
    port_tpid_class.tpid1 = cint_vpls_pwe_tagged_mode_advanced_scenario_info.outer_tpid;
    port_tpid_class.tpid2 = cint_vpls_pwe_tagged_mode_advanced_scenario_info.inner_tpid;
    port_tpid_class.tag_format_class_id = 16;    /* Tag_format for DTAG packet */
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY;
    port_tpid_class.vlan_translation_action_id = 0;
    snprintf(error_msg, sizeof(error_msg), "port=%d", port);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_tpid_class_set(unit, &port_tpid_class), error_msg);

    snprintf(error_msg, sizeof(error_msg), "in IVE configuration on Lif=%d",
        cint_vpls_pwe_tagged_mode_advanced_scenario_info.access_ingress_vlan_port_id[1]);
    BCM_IF_ERROR_RETURN_MSG(vlan_translate_ive_eve_translation_set(unit,
                                                cint_vpls_pwe_tagged_mode_advanced_scenario_info.access_ingress_vlan_port_id
                                                [1], cint_vpls_pwe_tagged_mode_advanced_scenario_info.outer_tpid,
                                                cint_vpls_pwe_tagged_mode_advanced_scenario_info.inner_tpid,
                                                bcmVlanActionDelete, bcmVlanActionNone,
                                                cint_vpls_pwe_tagged_mode_advanced_scenario_info.outer_ac_egress_outer_vlan,
                                                cint_vpls_pwe_tagged_mode_advanced_scenario_info.outer_ac_egress_inner_vlan,
                                                cint_vpls_pwe_tagged_mode_advanced_scenario_info.ingress_vlan_edit_profile
                                                [1], 16, 1), error_msg);

    return BCM_E_NONE;
}

int
vpls_pwe_tagged_mode_advanced_scenario_ive_delete_stag_ctag(
    int unit,
    int port)
{
    char error_msg[200]={0,};

    bcm_port_tpid_class_t port_tpid_class;
    /*
     * set tag format for D-tag packets
     */
    bcm_port_tpid_class_t_init(&port_tpid_class);
    port_tpid_class.port = port;
    port_tpid_class.tpid1 = cint_vpls_pwe_tagged_mode_advanced_scenario_info.outer_tpid;
    port_tpid_class.tpid2 = cint_vpls_pwe_tagged_mode_advanced_scenario_info.inner_tpid;
    port_tpid_class.tag_format_class_id = cint_vpls_pwe_tagged_mode_advanced_scenario_info.tag_format[2];
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY;
    port_tpid_class.vlan_translation_action_id = 0;
    snprintf(error_msg, sizeof(error_msg), "port=%d", port);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_tpid_class_set(unit, &port_tpid_class), error_msg);

    snprintf(error_msg, sizeof(error_msg), "in IVE configuration on Lif=%d",
        cint_vpls_pwe_tagged_mode_advanced_scenario_info.access_ingress_vlan_port_id[2]);
    BCM_IF_ERROR_RETURN_MSG(vlan_translate_ive_eve_translation_set(unit,
                                                cint_vpls_pwe_tagged_mode_advanced_scenario_info.access_ingress_vlan_port_id
                                                [2], cint_vpls_pwe_tagged_mode_advanced_scenario_info.outer_tpid,
                                                cint_vpls_pwe_tagged_mode_advanced_scenario_info.inner_tpid,
                                                bcmVlanActionDelete, bcmVlanActionDelete,
                                                cint_vpls_pwe_tagged_mode_advanced_scenario_info.outer_ac_egress_outer_vlan,
                                                cint_vpls_pwe_tagged_mode_advanced_scenario_info.outer_ac_egress_inner_vlan,
                                                cint_vpls_pwe_tagged_mode_advanced_scenario_info.ingress_vlan_edit_profile
                                                [2], cint_vpls_pwe_tagged_mode_advanced_scenario_info.tag_format[2], 1), error_msg);

    return BCM_E_NONE;
}
int
vpls_pwe_tagged_mode_advanced_scenario_eve_none(
    int unit,
    int port)
{
    char error_msg[200]={0,};

    snprintf(error_msg, sizeof(error_msg), "in IVE configuration on Lif=%d",
        cint_vpls_pwe_tagged_mode_advanced_scenario_info.access_egress_vlan_port_id[0]);
    BCM_IF_ERROR_RETURN_MSG(vlan_translate_ive_eve_translation_set(unit,
                                                cint_vpls_pwe_tagged_mode_advanced_scenario_info.access_egress_vlan_port_id
                                                [0], cint_vpls_pwe_tagged_mode_advanced_scenario_info.outer_tpid,
                                                cint_vpls_pwe_tagged_mode_advanced_scenario_info.inner_tpid,
                                                bcmVlanActionNone, bcmVlanActionNone,
                                                cint_vpls_pwe_tagged_mode_advanced_scenario_info.inner_ac_ingress_outer_vlan,
                                                cint_vpls_pwe_tagged_mode_advanced_scenario_info.inner_ac_ingress_inner_vlan,
                                                cint_vpls_pwe_tagged_mode_advanced_scenario_info.egress_vlan_edit_profile
                                                [0], 0, 0), error_msg);
    return BCM_E_NONE;
}

int
vpls_pwe_tagged_mode_advanced_scenario_eve_add_stag(
    int unit,
    int port)
{
    char error_msg[200]={0,};

    bcm_port_tpid_class_t port_tpid_class;
    /*
     * set tag format for STAG packets
     */
    bcm_port_tpid_class_t_init(&port_tpid_class);
    port_tpid_class.port = port;
    port_tpid_class.tpid1 = cint_vpls_pwe_tagged_mode_advanced_scenario_info.inner_tpid;
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    port_tpid_class.tag_format_class_id = 8;
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY;
    port_tpid_class.vlan_translation_action_id = 0;
    snprintf(error_msg, sizeof(error_msg), "port=%d", port);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_tpid_class_set(unit, &port_tpid_class), error_msg);

    snprintf(error_msg, sizeof(error_msg), "in EVE configuration on Lif=%d",
        cint_vpls_pwe_tagged_mode_advanced_scenario_info.access_egress_vlan_port_id[1]);
    BCM_IF_ERROR_RETURN_MSG(vlan_translate_ive_eve_translation_set_with_pri_action(unit,
                                                                cint_vpls_pwe_tagged_mode_advanced_scenario_info.access_egress_vlan_port_id
                                                                [1],
                                                                cint_vpls_pwe_tagged_mode_advanced_scenario_info.outer_tpid,
                                                                cint_vpls_pwe_tagged_mode_advanced_scenario_info.inner_tpid,
                                                                bcmVlanTpidActionNone, bcmVlanTpidActionNone,
                                                                bcmVlanActionAdd, bcmVlanActionNone, bcmVlanActionAdd,
                                                                bcmVlanActionNone,
                                                                cint_vpls_pwe_tagged_mode_advanced_scenario_info.inner_ac_ingress_outer_vlan,
                                                                cint_vpls_pwe_tagged_mode_advanced_scenario_info.inner_ac_ingress_inner_vlan,
                                                                cint_vpls_pwe_tagged_mode_advanced_scenario_info.egress_vlan_edit_profile
                                                                [1], 8, 0), error_msg);
    return BCM_E_NONE;
}

int
vpls_pwe_tagged_mode_advanced_scenario_eve_add_stag_ctag(
    int unit,
    int port)
{
    char error_msg[200]={0,};

    bcm_port_tpid_class_t port_tpid_class;
    /*
     * set tag format for untagged packets
     */
    bcm_port_tpid_class_t_init(&port_tpid_class);
    port_tpid_class.port = port;
    port_tpid_class.tpid1 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    port_tpid_class.tag_format_class_id = 0;
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY;
    port_tpid_class.vlan_translation_action_id = 0;
    snprintf(error_msg, sizeof(error_msg), "for untag packet on port=%d", port);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_tpid_class_set(unit, &port_tpid_class), error_msg);

    snprintf(error_msg, sizeof(error_msg), "in EVE configuration for Lif=%x",
        cint_vpls_pwe_tagged_mode_advanced_scenario_info.access_egress_vlan_port_id[2]);
    BCM_IF_ERROR_RETURN_MSG(vlan_translate_ive_eve_translation_set(unit,
                                                cint_vpls_pwe_tagged_mode_advanced_scenario_info.access_egress_vlan_port_id
                                                [2], cint_vpls_pwe_tagged_mode_advanced_scenario_info.outer_tpid,
                                                cint_vpls_pwe_tagged_mode_advanced_scenario_info.inner_tpid,
                                                bcmVlanActionAdd, bcmVlanActionAdd,
                                                cint_vpls_pwe_tagged_mode_advanced_scenario_info.inner_ac_ingress_outer_vlan,
                                                cint_vpls_pwe_tagged_mode_advanced_scenario_info.inner_ac_ingress_inner_vlan,
                                                cint_vpls_pwe_tagged_mode_advanced_scenario_info.egress_vlan_edit_profile
                                                [2], 0, 0), error_msg);
    return BCM_E_NONE;
}

/*
 * For Ingress:
 * Define AC for inner ETH validation on packets going from core to customer.
 * The resulting vlan_port_id is saved in cint_vpls_pwe_tagged_mode_advanced_scenario_info.ingress_native_ac_vlan_port_id.
 * For Egress:
 * Define a VLAN port to forward the native eth DA to.
 * Connect the Native DA to the egress vlan port.
 */
int
vpls_pwe_tagged_mode_advanced_scenario_add_network_ingress_native_ac(
    int unit)
{
    bcm_vlan_port_t vlan_port;
    bcm_l2_addr_t l2addr;
    /*
     * add port.
     */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags = BCM_VLAN_PORT_NATIVE | BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    vlan_port.vsi = cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_vsi[0];
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), "");
    printf("bcm_vlan_port_create: nof_service_tag = 0, inner_AC_lif = 0x%08X\n", vlan_port.vlan_port_id);
    /*
     * Save the created vlan_port_id
     */
    cint_vpls_pwe_tagged_mode_advanced_scenario_info.ingress_native_ac_vlan_port_id[0] = vlan_port.vlan_port_id;

    /*
     * Add Match
     */
    bcm_port_match_info_t match_info;
    bcm_port_match_info_t_init(&match_info);

    bcm_vlan_port_t_init(&vlan_port);

    /*
     * Connect the native DA to the egress vlan port
     */
    bcm_l2_addr_t_init(&l2addr, cint_vpls_pwe_tagged_mode_advanced_scenario_info.customer_mac_1,
                       cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_vsi[0]);
    l2addr.flags |= BCM_L2_STATIC;
    l2addr.port = cint_vpls_pwe_tagged_mode_advanced_scenario_info.access_egress_vlan_port_id[0];
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, l2addr), "");

    /*
     * add port.
     */
    vlan_port.flags = BCM_VLAN_PORT_NATIVE | BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    vlan_port.vsi = cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_vsi[1];
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), "");
    printf("bcm_vlan_port_create: nof_service_tag = 1, inner_AC_lif = 0x%08X\n", vlan_port.vlan_port_id);
    /*
     * Save the created vlan_port_id
     */
    cint_vpls_pwe_tagged_mode_advanced_scenario_info.ingress_native_ac_vlan_port_id[1] = vlan_port.vlan_port_id;

    /*
     * Add Match
     */
    match_info.flags = BCM_PORT_MATCH_INGRESS_ONLY | BCM_PORT_MATCH_NATIVE;
    match_info.port = cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_2_mpls_port_id_ing;
    match_info.match = BCM_PORT_MATCH_PORT_VLAN;        /* Match 1 tag */
    match_info.match_vlan = cint_vpls_pwe_tagged_mode_advanced_scenario_info.ingress_canonical_outer_vlan[1];
    BCM_IF_ERROR_RETURN_MSG(bcm_port_match_add(unit, cint_vpls_pwe_tagged_mode_advanced_scenario_info.ingress_native_ac_vlan_port_id[1],
                            &match_info), "for service_2_mpls_port_id_ing");

    /*
     * Connect the native DA to the egress vlan port
     */
    bcm_l2_addr_t_init(&l2addr, cint_vpls_pwe_tagged_mode_advanced_scenario_info.customer_mac_1,
                       cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_vsi[1]);
    l2addr.flags |= BCM_L2_STATIC;
    l2addr.port = cint_vpls_pwe_tagged_mode_advanced_scenario_info.access_egress_vlan_port_id[1];
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, l2addr), "");

    /*
     * add port.
     */
    vlan_port.flags = BCM_VLAN_PORT_NATIVE | BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    vlan_port.vsi = cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_vsi[2];
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), "");
    printf("bcm_vlan_port_create: nof_service_tag = 2, inner_AC_lif = 0x%08X\n", vlan_port.vlan_port_id);
    /*
     * Save the created vlan_port_id
     */
    cint_vpls_pwe_tagged_mode_advanced_scenario_info.ingress_native_ac_vlan_port_id[2] = vlan_port.vlan_port_id;

    /*
     * Add Match
     */
    match_info.flags = BCM_PORT_MATCH_INGRESS_ONLY | BCM_PORT_MATCH_NATIVE;
    match_info.port = cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_3_mpls_port_id_ing;
    match_info.match = BCM_PORT_MATCH_PORT_VLAN_STACKED;        /* Match 2 tag */
    match_info.match_vlan = cint_vpls_pwe_tagged_mode_advanced_scenario_info.ingress_canonical_outer_vlan[2];
    match_info.match_inner_vlan = cint_vpls_pwe_tagged_mode_advanced_scenario_info.ingress_canonical_inner_vlan;
    BCM_IF_ERROR_RETURN_MSG(bcm_port_match_add(unit, cint_vpls_pwe_tagged_mode_advanced_scenario_info.ingress_native_ac_vlan_port_id[2],
                            &match_info), "for service_3_mpls_port_id_ing");

    /*
     * Connect the native DA to the egress vlan port
     */
    bcm_l2_addr_t_init(&l2addr, cint_vpls_pwe_tagged_mode_advanced_scenario_info.customer_mac_1,
                       cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_vsi[2]);
    l2addr.flags |= BCM_L2_STATIC;
    l2addr.port = cint_vpls_pwe_tagged_mode_advanced_scenario_info.access_egress_vlan_port_id[2];
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, l2addr), "");

    return BCM_E_NONE;
}

/*
 * Define AC for inner ETH validation on packets going from customer to core.
 * This creates a SW DB entry that is later used by VLAN translation APIs for setting inner ETH editting.
 * The resulting vlan_port_id is saved in cint_vpls_pwe_tagged_mode_advanced_scenario_info.egress_native_ac_vlan_port_id.
 */
int
vpls_pwe_tagged_mode_advanced_scenario_add_core_native_ac(
    int unit)
{
    bcm_vlan_port_t vlan_port;
    /*
     * add port.
     */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags = BCM_VLAN_PORT_NATIVE | BCM_VLAN_PORT_VLAN_TRANSLATION | BCM_VLAN_PORT_CREATE_EGRESS_ONLY;
    vlan_port.vsi = cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_vsi[0];
    vlan_port.port = cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_1_mpls_port_id_eg;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), "");
    /*
     * Save the created vlan_port_id
     */
    cint_vpls_pwe_tagged_mode_advanced_scenario_info.egress_native_ac_vlan_port_id[0] = vlan_port.vlan_port_id;

    vlan_port.vsi = cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_vsi[1];
    vlan_port.port = cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_2_mpls_port_id_eg;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), "");
    cint_vpls_pwe_tagged_mode_advanced_scenario_info.egress_native_ac_vlan_port_id[1] = vlan_port.vlan_port_id;
    printf("bcm_vlan_port_create: pwe_port = %d, out_lif = 0x%08X\n",
           cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_2_mpls_port_id_eg, vlan_port.vlan_port_id);

    vlan_port.vsi = cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_vsi[2];
    vlan_port.port = cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_3_mpls_port_id_eg;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), "");
    cint_vpls_pwe_tagged_mode_advanced_scenario_info.egress_native_ac_vlan_port_id[2] = vlan_port.vlan_port_id;
    printf("bcm_vlan_port_create: pwe_port = %d, out_lif = 0x%08X\n",
           cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_3_mpls_port_id_eg, vlan_port.vlan_port_id);

    return BCM_E_NONE;
}

int
vpls_pwe_tagged_mode_advanced_scenario_in_port_intf(
    int unit,
    int in_port,
    int vsi,
    int vlan)
{
    /*
     * Add VLAN port in ingress and egress
     */
    char error_msg[200]={0,};
    bcm_vlan_port_t vlan_port;
    bcm_vlan_port_t_init(&vlan_port);

    vlan_port.vsi = 0;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
    vlan_port.port = in_port;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), "");

    printf("bcm_vlan_port_create: port = %d, in_lif = 0x%08X\n", vlan_port.port, vlan_port.vlan_port_id);
    cint_vpls_pwe_tagged_mode_advanced_scenario_info.access_ingress_vlan_port_id[0] = vlan_port.vlan_port_id;
    cint_vpls_pwe_tagged_mode_advanced_scenario_info.access_egress_vlan_port_id[0] = vlan_port.vlan_port_id;

    BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_port_add(unit, vsi, vlan_port.vlan_port_id), "");

    snprintf(error_msg, sizeof(error_msg), "port = %d, vsi = %d", in_port, vlan);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(unit, vlan, in_port, 0), error_msg);

    /*
     * Add Match
     */
    bcm_port_match_info_t match_info;
    bcm_port_match_info_t_init(&match_info);
    match_info.flags = BCM_PORT_MATCH_INGRESS_ONLY;
    match_info.match = BCM_PORT_MATCH_PORT;
    match_info.port = in_port;
    BCM_IF_ERROR_RETURN_MSG(bcm_port_match_add(unit, vlan_port.vlan_port_id, &match_info), "for service_2_mpls_port_id_ing");

    return BCM_E_NONE;

}

int
vpls_pwe_tagged_mode_advanced_scenario_in_port_intf_set_s_tag(
    int unit,
    int in_port,
    int vsi,
    int vlan)
{
    bcm_vlan_port_t vlan_port;
    char error_msg[200]={0,};

    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = in_port;
    vlan_port.match_vlan = vlan;
    vlan_port.vsi = 0;

    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, vlan_port), "");

    printf("bcm_vlan_port_create: port = %d, in_lif = 0x%08X\n", vlan_port.port, vlan_port.vlan_port_id);

    cint_vpls_pwe_tagged_mode_advanced_scenario_info.access_ingress_vlan_port_id[1] = vlan_port.vlan_port_id;
    cint_vpls_pwe_tagged_mode_advanced_scenario_info.access_egress_vlan_port_id[1] = vlan_port.vlan_port_id;

    snprintf(error_msg, sizeof(error_msg), "for access port=%d", in_port);
    BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_port_add(unit, vsi, vlan_port.vlan_port_id), error_msg);

    snprintf(error_msg, sizeof(error_msg), "port = %d, vsi = %d", in_port, vlan);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(unit, vlan, in_port, 0), error_msg);
    
    return BCM_E_NONE;

}

int
cint_vpls_pwe_tagged_mode_advanced_scenario_in_port_intf_set_s_c_tag(
    int unit,
    int in_port,
    int vsi,
    int vlan,
    int inner_vlan)
{
    bcm_vlan_port_t vlan_port;
    char error_msg[200]={0,};

    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
    vlan_port.port = in_port;
    vlan_port.match_vlan = vlan;
    vlan_port.match_inner_vlan = inner_vlan;
    vlan_port.vsi = 0;

    snprintf(error_msg, sizeof(error_msg), "for port=%d,vlan=%d,inner_vlan=%d", in_port, vlan, inner_vlan);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, vlan_port), error_msg);

    printf("bcm_vlan_port_create: port = %d, in_lif = 0x%08X\n", vlan_port.port, vlan_port.vlan_port_id);

    cint_vpls_pwe_tagged_mode_advanced_scenario_info.access_ingress_vlan_port_id[2] = vlan_port.vlan_port_id;
    cint_vpls_pwe_tagged_mode_advanced_scenario_info.access_egress_vlan_port_id[2] = vlan_port.vlan_port_id;

    snprintf(error_msg, sizeof(error_msg), "for access port=%d, vsi=%d", vlan_port.vlan_port_id, vsi);
    BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_port_add(unit, vsi, vlan_port.vlan_port_id), error_msg);

    snprintf(error_msg, sizeof(error_msg), "port = %d, vsi = %d", in_port, vlan);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(unit, vlan, in_port, 0), error_msg);
    
    return BCM_E_NONE;

}

int
cint_vpls_pwe_tagged_mode_advanced_scenario_set_qos_mapping(
    int unit)
{
    int ingress_qos_id = 5;
    int egress_qos_id = 7;
    int opcode_ingress = -1;
    int opcode_egress = -1;
    BCM_IF_ERROR_RETURN_MSG(cint_vpls_pwe_tagged_mode_advanced_scenario_qos_map_l2_vpn(unit, &ingress_qos_id, &egress_qos_id,
                                                                    &opcode_ingress, &opcode_egress), "");

    BCM_IF_ERROR_RETURN_MSG(qos_map_gport(unit, cint_vpls_pwe_tagged_mode_advanced_scenario_info.access_ingress_vlan_port_id[1],
                       ingress_qos_id, -1), "");


    BCM_IF_ERROR_RETURN_MSG(qos_map_gport(unit, cint_vpls_pwe_tagged_mode_advanced_scenario_info.egress_native_ac_vlan_port_id[1], -1,
                       egress_qos_id), "");


    return BCM_E_NONE;
}

/* - Main function for QOS settings
 * - In encapsulation flow where PW is for nof_service_tag=1, the following qos_mapping APIs will make sure that
 *   incoming S-TAG's PCP+DE values are preserved in new outgoing S-TAG's PCP+DE values
 * - Create QOS ingress profile and set its mapping ETH VLAN fields to TC, DP (PHB QOS)
 * - Create QOS egress profile and set its mapping TC,DP to ETH VLAN fields
 * - Using ingress_qos_id (i.e Ingress Remark Profile) incoming PCP(=3)+DE(=1) bits are mapped to internal NWK_QOS
 * - Using egress_qos_id (i.e. Egress Remark Profile), NWK_QOS is mapped back to outgoing PCP(=3)+DE(=1) bits
 * - Refer $SDK/src/examples/sand/cint_qos_l2_phb.c for incoming PCP(=3)+DE(=1) => {TC=5, DP=2},NWK_QOS(=7) => outgoing PCP(=3)+DE(=1)
 */
int
cint_vpls_pwe_tagged_mode_advanced_scenario_qos_map_l2_vpn(
    int unit,
    int *ingress_qos_id,
    int *egress_qos_id,
    int *ingress_opcode_id,
    int *egress_opcode_id)
{
    uint32 flags = 0;
    int opcode = -1;

    if (ingress_qos_id != illegal_qos_id)
    {
        flags = (BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_REMARK);
        if (*ingress_qos_id != -1)
        {
            flags |= BCM_QOS_MAP_WITH_ID;
        }
        BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_create(unit, flags, ingress_qos_id), "in L3 ingress");


        flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_OPCODE;
        if (*ingress_opcode_id != -1)
        {
            flags |= BCM_QOS_MAP_WITH_ID;
            opcode = *ingress_opcode_id;            
        }
        BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_create(unit, flags, &opcode), "in ingress l3 opcode");

        BCM_IF_ERROR_RETURN_MSG(qos_map_ingress_profile_map(unit, *ingress_qos_id, opcode,
                                         BCM_QOS_MAP_PHB | BCM_QOS_MAP_L2_OUTER_TAG), "");
        /*
         * To configure nwk-qos 
         */
        
        flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
        opcode = -1;
        if (*ingress_opcode_id != -1)
        {
            flags |= BCM_QOS_MAP_WITH_ID;
            opcode = *ingress_opcode_id;            
        }
        BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_create(unit, flags, &opcode), "in ingress l3 opcode");
        BCM_IF_ERROR_RETURN_MSG(qos_map_ingress_profile_map(unit, *ingress_qos_id, opcode,
            
                                         BCM_QOS_MAP_REMARK | BCM_QOS_MAP_L2_OUTER_TAG), "");
    }

    if (*egress_qos_id != illegal_qos_id)
    {
        flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK;
        if (*egress_qos_id != -1)
        {
            flags |= BCM_QOS_MAP_WITH_ID;
        }
        BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_create(unit, flags, egress_qos_id), "in L3 egress");


        flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
        if (*egress_opcode_id != -1)
        {
            flags |= BCM_QOS_MAP_WITH_ID;
        }
        BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_create(unit, flags, egress_opcode_id), "in egress l3 opcode");

        BCM_IF_ERROR_RETURN_MSG(qos_map_egress_profile_map(unit, *egress_qos_id, *egress_opcode_id,
                                        BCM_QOS_MAP_REMARK | BCM_QOS_MAP_L2_OUTER_TAG), "");
    }
    return BCM_E_NONE;
}

/*
 * Main function for basic mpls vpls scenario.
 */
int
vpls_pwe_tagged_mode_advanced_scenario_main(
    int unit,
    int access_port_1,
    int access_port_2,
    int access_port_3,
    int core_port)
{
    char error_msg[200]={0,};
    int nof_labels = 2;

    BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_1_mpls_fec_id), "");

    vpls_pwe_tagged_mode_advanced_scenario_init(access_port_1, access_port_2, access_port_3, core_port);

   /* 
   * TPID configuration: 
   * Note: 
   * TPID and Native/non-native LLVP profiles are configured by default. 
   * If clearing and setting TPID , need to re-configure all LLVP profiles (native and non-native) from the beginning!!! 
   * Thus, avoid TPID settings, use default! 
   */

    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_advanced_scenario_create_vsis(unit), "");

    /*
     * Configure AC and PWE ports
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_advanced_scenario_configure_port_properties(unit), "");

    /*
     * Configure L3 interfaces
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_advanced_scenario_create_l3_interfaces(unit), "");

    /*
     * Configure an ARP entries
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_advanced_scenario_create_arp_entry(unit), "");

    /*
     * Configure a push entry.
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_advanced_scenario_create_mpls_tunnel(unit,
                                                                   cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_1_mpls_tunnel_label_1,
                                                                   cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_1_mpls_tunnel_label_2,
                                                                   2,
                                                                   cint_vpls_pwe_tagged_mode_advanced_scenario_info.core_arp_id,
                                                                   &cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_1_mpls_tunnel_id), "");

    /*
     * Configure fec entry
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_advanced_scenario_create_fec_entry(unit), "");

    /*
     * Configure a termination label for the ingress flow
     */
    bcm_mpls_label_t label_array[2];
    label_array[0] = cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_1_mpls_tunnel_label_1;
    label_array[1] = cint_vpls_pwe_tagged_mode_advanced_scenario_info.service_1_mpls_tunnel_label_2;
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_advanced_scenario_create_termination_stack(unit, label_array, 2), "");

    /*
     * configure PWE tunnel - egress flow
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_advanced_scenario_mpls_port_add_encapsulation(unit), "");

    /*
     * configure PWE tunnel - ingress flow
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_advanced_scenario_mpls_port_add_termination(unit), "");

    /** define ac and pwe ports */
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_advanced_scenario_vswitch_add_core_outer_ac(unit,
        cint_vpls_pwe_tagged_mode_advanced_scenario_info.core_port), "");

    /*
     * add l2 addresses to be defined as static - no learning needed
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_advanced_scenario_l2_addr_add(unit), "");

    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_advanced_scenario_eve_swap(unit,
        cint_vpls_pwe_tagged_mode_advanced_scenario_info.core_port), "");

    /*
     * Add AC for inner ETH IVE (removing/replacing service tags)
     * For Ingress flow
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_advanced_scenario_add_network_ingress_native_ac(unit), "");
    /*
     * Add IVE for the AC created in the previous step
     */
    snprintf(error_msg, sizeof(error_msg), "port=0x%08x",
        cint_vpls_pwe_tagged_mode_advanced_scenario_info.ingress_native_ac_vlan_port_id[0]);
    BCM_IF_ERROR_RETURN_MSG(vlan_translate_ive_eve_translation_set(unit, cint_vpls_pwe_tagged_mode_advanced_scenario_info.ingress_native_ac_vlan_port_id[0], 0, 0, bcmVlanActionNone, bcmVlanActionNone, 0, 0, cint_vpls_pwe_tagged_mode_advanced_scenario_info.ingress_vlan_edit_profile[0], cint_vpls_pwe_tagged_mode_advanced_scenario_info.tag_format[0],
        1    /* is_ive*/ ), error_msg);

    snprintf(error_msg, sizeof(error_msg), "port=0x%08x",
        cint_vpls_pwe_tagged_mode_advanced_scenario_info.ingress_native_ac_vlan_port_id[1]);
    BCM_IF_ERROR_RETURN_MSG(vlan_translate_ive_eve_translation_set(unit, cint_vpls_pwe_tagged_mode_advanced_scenario_info.ingress_native_ac_vlan_port_id[1], 0, 0, bcmVlanActionDelete, bcmVlanActionNone, 0, 0, cint_vpls_pwe_tagged_mode_advanced_scenario_info.ingress_vlan_edit_profile[1], 16,
        1       /* is_ive */ ), error_msg);

    snprintf(error_msg, sizeof(error_msg), "port=0x%08x",
        cint_vpls_pwe_tagged_mode_advanced_scenario_info.ingress_native_ac_vlan_port_id[2]);
    BCM_IF_ERROR_RETURN_MSG(vlan_translate_ive_eve_translation_set(unit, cint_vpls_pwe_tagged_mode_advanced_scenario_info.ingress_native_ac_vlan_port_id[2], 0, 0, bcmVlanActionDelete, bcmVlanActionDelete, 0, 0, cint_vpls_pwe_tagged_mode_advanced_scenario_info.ingress_vlan_edit_profile[2], 16,
        1     /* is_ive */ ), error_msg);

    /*
     * Add AC for inner ETH EVE (adding service tags)
     * For Egress flow
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_advanced_scenario_add_core_native_ac(unit), "");
    /*
     * Add EVE for the AC created in the previous step
     */
    snprintf(error_msg, sizeof(error_msg), "port=0x%08x",
        cint_vpls_pwe_tagged_mode_advanced_scenario_info.egress_native_ac_vlan_port_id[0]);
    BCM_IF_ERROR_RETURN_MSG(vlan_translate_ive_eve_translation_set(unit, cint_vpls_pwe_tagged_mode_advanced_scenario_info.egress_native_ac_vlan_port_id[0], cint_vpls_pwe_tagged_mode_advanced_scenario_info.outer_tpid, cint_vpls_pwe_tagged_mode_advanced_scenario_info.inner_tpid, bcmVlanActionNone, bcmVlanActionNone, cint_vpls_pwe_tagged_mode_advanced_scenario_info.egress_canonical_outer_vlan[0], cint_vpls_pwe_tagged_mode_advanced_scenario_info.egress_canonical_inner_vlan, cint_vpls_pwe_tagged_mode_advanced_scenario_info.egress_vlan_edit_profile[0], cint_vpls_pwe_tagged_mode_advanced_scenario_info.tag_format[0],
        0 /* is_ive */ ), error_msg);

    /*EVE action on OutLif correponding to inner L2 header in encapsulation flow*/
    snprintf(error_msg, sizeof(error_msg), "port=0x%08x",
        cint_vpls_pwe_tagged_mode_advanced_scenario_info.egress_native_ac_vlan_port_id[1]);
    BCM_IF_ERROR_RETURN_MSG(vlan_translate_ive_eve_translation_set_with_pri_action(unit, cint_vpls_pwe_tagged_mode_advanced_scenario_info.egress_native_ac_vlan_port_id[1], cint_vpls_pwe_tagged_mode_advanced_scenario_info.outer_tpid, cint_vpls_pwe_tagged_mode_advanced_scenario_info.inner_tpid, bcmVlanTpidActionNone, bcmVlanTpidActionNone, bcmVlanActionAdd, bcmVlanActionNone, bcmVlanActionAdd, bcmVlanActionNone, cint_vpls_pwe_tagged_mode_advanced_scenario_info.egress_canonical_outer_vlan[1], cint_vpls_pwe_tagged_mode_advanced_scenario_info.egress_canonical_inner_vlan, cint_vpls_pwe_tagged_mode_advanced_scenario_info.egress_vlan_edit_profile[1], 8,
        0    /* is_ive */), error_msg);


    /*
     * Add EVE for the AC created in the previous step
     */
    snprintf(error_msg, sizeof(error_msg), "port=0x%08x",
        cint_vpls_pwe_tagged_mode_advanced_scenario_info.egress_native_ac_vlan_port_id[2]);
    BCM_IF_ERROR_RETURN_MSG(vlan_translate_ive_eve_translation_set(unit, cint_vpls_pwe_tagged_mode_advanced_scenario_info.egress_native_ac_vlan_port_id[2], cint_vpls_pwe_tagged_mode_advanced_scenario_info.outer_tpid, cint_vpls_pwe_tagged_mode_advanced_scenario_info.inner_tpid, bcmVlanActionAdd, bcmVlanActionAdd, cint_vpls_pwe_tagged_mode_advanced_scenario_info.egress_canonical_outer_vlan[2], cint_vpls_pwe_tagged_mode_advanced_scenario_info.egress_canonical_inner_vlan, cint_vpls_pwe_tagged_mode_advanced_scenario_info.egress_vlan_edit_profile[2], 0,
        0        /* is_ive */), error_msg);

    /*
     * QoS mapping configuration on access port Lifs 
     */
    BCM_IF_ERROR_RETURN_MSG(cint_vpls_pwe_tagged_mode_advanced_scenario_set_qos_mapping(unit), "");

    return BCM_E_NONE;
}
