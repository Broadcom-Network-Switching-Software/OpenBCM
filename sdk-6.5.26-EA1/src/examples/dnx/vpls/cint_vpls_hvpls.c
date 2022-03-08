/*~~~~~~~~~~~~~~~~~~~~~~~Mulitpoint VPLS Service~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
 *
 * File: cint_vpls_hvpls.c
 * Purpose: Example of Open Multi-Point VPLS service with Hierarchical PWE-MPLS protection.
 * Hierarchical PWE-MPLS protection provides the possibility to support both PWE 1:1 protection
 * and MPLS 1:1 protection i.e. 2-layer protection.
 *
 *   Following figure shows HW model for Hierarchical VPLS:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *|                                           Ingress-Egress           EEDB            |
 *|                                 Outlif           |     +----+  +----------+        |
 *|                        ------------------------------->|- - |->|   PWE    |        |
 *|                        |                         |     |    |  |----------|        |
 *| +-------+   FEC    +------+  FEC MPLS  +------+ EEI    |GLEM|  |          |        |
 *| |       |   PWE    |      |----------->|      |(Outlif)|    |  |----------|        |
 *| | MACT  | -------->| FEC  |            | FEC  |------->|- - |->|   MPLS   |- |     | 
 *| |       |          | PWE  |            | MPLS |  |     |    |  |----------|  |     |
 *| +-------+          +------+            +------+        +----+  |          |  |     |
 *|                                                                |          |  |     | 
 *|                                                                |----------|  |     |
 *|                                                                |Link-Layer|<-|     |  
 *|                                                                +----------+        |
 *|                                                                                    |
 *|------------------------------------------------------------------------------------|
 *
 * Headers:
 *
 * Primary path:
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  |
 *   |  | DA |SA||TIPD1 |Prio|VID||  MPLS    |  MPLS    ||   PWE    ||Eth||Data ||
 *   |  |0:11|  ||0x8847|    |100||Label:2000|Label:1000||Lable:2010||   ||     || 
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  |
 *   |                +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+                  |
 *   |                | Figure 1 : Packets Received from PWE  |                  | 
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+    |
 *   |  |DA|SA||TIPD1 |Prio|VID||  MPLS    |  MPLS    ||   PWE    ||Eth||Data |  |
 *   |  |  |  ||0x8847|    |200||Label:2000|Label:1000||Lable:1982||   ||     |  | 
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+    |
 *   |                +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+                 |
 *   |                | Figure 2 : Packets Transmitted to PWE  |                 | 
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  
 *
 * Protected path:
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  |
 *   |  | DA |SA||TIPD1 |Prio|VID||  MPLS    |  MPLS    ||   PWE    ||Eth||Data ||
 *   |  |0:11|  ||0x8847|    |100||Label:2001|Label:1001||Lable:2020||   ||     || 
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  |
 *   |                +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+                  |
 *   |                | Figure 1 : Packets Received from PWE  |                  | 
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+    |
 *   |  |DA|SA||TIPD1 |Prio|VID||  MPLS    |  MPLS    ||   PWE    ||Eth||Data |  |
 *   |  |  |  ||0x8847|    |200||Label:2001|Label:1001||Lable:1992||   ||     |  | 
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+    |
 *   |                +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+                 |
 *   |                | Figure 2 : Packets Transmitted to PWE  |                 | 
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  
 * 
 * Access side packets:
 * 
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |                       tag1                 tag2                           |
 *   |   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+   |
 *   |   | DA | SA || TIPD1 | Prio | VID || TIPD2 | Prio | VID ||    Data    |   |
 *   |   |    |    || 0x8100|      | 10  || 0x9100|      | 20  ||            |   |
 *   |   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+   |
 *   |        +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+       |
 *   |        | Figure 3 : Packets Received/Transmitted on Access Port   |       |
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  
 *
  * Test Scenario
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_global.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_l3.c
 * cint ../../src/./examples/dnx/l3/cint_dnx_ip_route_basic.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_multicast.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_vlan_translate.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utility_mpls.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_vpls.c
 * cint ../../src/./examples/dnx/vpls/cint_vpls_hvpls.c
 * cint
 * vpls_hvpls_example(0,201,203,0);
 *
 *   To set the protection properties use vpls_hvpls_failover_set
 */

int mc_in_fec = 0;/*whether use a MC group as destination of FEC entry*/

struct cint_vpls_hvpls_info_s {

    /* tunnels info */
    int in_tunnel_label_primary;  /* inner primary MPLS tunnel label */
    int in_tunnel_label_protected; /* inner secondary MPLS tunnel label */
    int out_tunnel_label_primary; /* outer primary MPLS tunnel label */
    int out_tunnel_label_protected; /* outer secondary MPLS tunnel label */

    bcm_if_t mpls_fec; /* mpls FEC */
    bcm_gport_t pwe_fec; /* PWE FEC */

    bcm_if_t mpls_tunnel_id_primary; /* Interface id of primary MPLS tunnel */
    bcm_if_t mpls_tunnel_id_protected; /* Interface id of secondary MPLS tunnel */

    int pwe_outlif_primary; /* outlif of primary pwe */
    int pwe_outlif_protected; /* outlif of secondary pwe */

    int in_vc_label_primary; /* pwe label primary */
    int in_vc_label_protected; /* pwe label protected */
    int eg_vc_label_primary; /* pwe label primary */
    int eg_vc_label_protected; /* pwe label protected */

    bcm_failover_t pwe_failover_id_fec;
    bcm_failover_t pwe_failover_id_outlif;

    bcm_failover_t mpls_failover_id_fec;
    bcm_failover_t mpls_failover_id_outlif;

    bcm_gport_t mpls_out_port_id; /* pwe mpls out port primary */
    bcm_gport_t mpls_port_id; /* pwe mpls port primary */
};

cint_vpls_hvpls_info_s vpls_hvpls_info;
mpls_util_mpls_tunnel_initiator_create_s mpls_encap_tunnel_1[1]; /* Primary */
mpls_util_mpls_tunnel_initiator_create_s mpls_encap_tunnel_2[1]; /* Secondary */

void
vpls_hvpls_info_init(int unit, int port1, int port2) {

    int rv = BCM_E_NONE;
    int first_fec_in_hier;

    vpn = 6202;

    /*
     * l2, mac, arp, fec parameters
     */
    pwe_port.intf = 200;
    pwe_port.mac_addr = mac1;
    pwe_port.next_hop = mac2;
    pwe_port.port = port2;
    pwe_port.arp = 9001;

    ac_port.intf = 5;
    ac_port.mac_addr = mac3;
    ac_port.next_hop = mac4;
    ac_port.port = port1;
    ac_port.vlan_port_id = 0;

    /* VC label */
    vpls_hvpls_info.in_vc_label_primary = 2010;
    vpls_hvpls_info.in_vc_label_protected = 2020;
    vpls_hvpls_info.eg_vc_label_primary = 1982;
    vpls_hvpls_info.eg_vc_label_protected = 1992;

    /* tunnel info */
    vpls_hvpls_info.in_tunnel_label_primary = 1000;
    vpls_hvpls_info.in_tunnel_label_protected = 1001;
    vpls_hvpls_info.out_tunnel_label_primary = 2000;
    vpls_hvpls_info.out_tunnel_label_protected = 2001;

    vpls_hvpls_info.mpls_tunnel_id_primary = 0x4002;
    vpls_hvpls_info.mpls_tunnel_id_protected = 0x4004;

    vpls_hvpls_info.pwe_outlif_primary = 0x7000;
    vpls_hvpls_info.pwe_outlif_protected = 0x7002;

    /* MPLS tunnel FEC, 2nd hierachy */
    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 1, 0, &first_fec_in_hier);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }
    /* Secondary FEC has to be odd */
    vpls_hvpls_info.mpls_fec = first_fec_in_hier + 1;

    /* PWE tunnel FEC, 1st hierachy */
    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &first_fec_in_hier);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }
    vpls_hvpls_info.pwe_fec = first_fec_in_hier + 1;

    return rv;
}


/*
 * Create L2 VPN (VSI and MC group)
 */
int
vpls_hvpls_vswitch_create(
    int unit,
    int vpn)
{
    int rv = BCM_E_NONE;

    printf("Create VSI\n");
    rv = bcm_vswitch_create_with_id(unit, vpn);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_vswitch_create_with_id\n");
        return rv;
    }

    printf("Create MC group\n");
    rv = multicast__open_mc_group(unit, &vpn, 0);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in multicast__open_mc_group\n");
        return rv;
    }

    return rv;
}

int
vpls_hvpls_add_access_port(
    int unit,
    l2_port_properties_s * port,
    bcm_vpn_t vpn)
{
    int rv = BCM_E_NONE;
    bcm_vlan_port_t vlan_port;

    /*
     * add port, according to port_vlan_vlan
     */
    bcm_vlan_port_t_init(&vlan_port);

    /*
     * set port attribures, key <port-vlan>
     */
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = port->port;
    vlan_port.match_vlan = port->intf;
    vlan_port.vsi = 0;
    vlan_port.flags = 0;
    if (is_contain_wide_data)
    {
        vlan_port.flags |= BCM_VLAN_PORT_INGRESS_WIDE;
    }

    if (port->nwk_group_valid)
    {
        vlan_port.ingress_network_group_id = port->ac_nwk_group_id;
        vlan_port.egress_network_group_id = port->ac_nwk_group_id;
    }

    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_port_create\n");
        return rv;
    }

    port->vlan_port_id = vlan_port.vlan_port_id;
    port->encap_id = vlan_port.encap_id;

    rv = bcm_vswitch_port_add(unit, vpn, vlan_port.vlan_port_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vswitch_port_add\n");
        return rv;
    }

    /*
     * Add port to VLAN membership
     */
    rv = bcm_vlan_gport_add(unit, port->intf, port->port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_gport_add \n");
        return rv;
    }

    return rv;
}

int
vpls_hvpls_add_core_port(
    int unit,
    l2_port_properties_s * port)
{
    int rv = BCM_E_NONE;
    bcm_vlan_port_t vlan_port;

    /*
     * add port, according to port_vlan_vlan
     */
    bcm_vlan_port_t_init(&vlan_port);

    /*
     * set port attribures, key <port-vlan>
     */
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = port->port;
    vlan_port.match_vlan = port->intf;
    vlan_port.vsi = vpn;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;

    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_port_create\n");
        return rv;
    }

    /*
     * Set Out-Port default properties 
     */
    rv = out_port_set(unit, port->port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_set intf_out, port=%d\n", port->port);
        return rv;
    }

    /*
     * Add port to VLAN membership
     */
    rv = bcm_vlan_gport_add(unit, port->intf, port->port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_gport_add \n");
        return rv;
    }

    return rv;
}

int
vpls_hvpls_mpls_tunnels_create(int unit)
{

    int rv;

    /* Create an MPLS Tunnel Egress failover ID */
    rv = bcm_failover_create(unit, BCM_FAILOVER_ENCAP, &vpls_hvpls_info.mpls_failover_id_outlif);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_failover_create for MPLS Egress\n");
        return rv;
    }

    /* Protected */
    mpls_encap_tunnel_2[0].label[0] = vpls_hvpls_info.in_tunnel_label_protected;
    mpls_encap_tunnel_2[0].label[1] = vpls_hvpls_info.out_tunnel_label_protected;
    mpls_encap_tunnel_2[0].num_labels = 2;
    mpls_encap_tunnel_2[0].l3_intf_id = &pwe_port.arp;
    mpls_encap_tunnel_2[0].encap_access = bcmEncapAccessTunnel2;
    mpls_encap_tunnel_2[0].flags |= BCM_MPLS_EGRESS_LABEL_WITH_ID | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT | BCM_MPLS_EGRESS_LABEL_PROTECTION;
    mpls_encap_tunnel_2[0].tunnel_id = vpls_hvpls_info.mpls_tunnel_id_protected;
    mpls_encap_tunnel_2[0].egress_failover_id = vpls_hvpls_info.mpls_failover_id_outlif;
    mpls_encap_tunnel_2[0].egress_failover_if_id = 0;

    rv = mpls_create_initiator_tunnels(unit, mpls_encap_tunnel_2, 1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_create_initiator_tunnels for secondary MPLS tunnel\n");
        return rv;
    }

    vpls_hvpls_info.mpls_tunnel_id_protected = mpls_encap_tunnel_2[0].tunnel_id;

    /* Primary */
    mpls_encap_tunnel_1[0].label[0] = vpls_hvpls_info.in_tunnel_label_primary;
    mpls_encap_tunnel_1[0].label[1] = vpls_hvpls_info.out_tunnel_label_primary;
    mpls_encap_tunnel_1[0].num_labels = 2;
    mpls_encap_tunnel_1[0].l3_intf_id = &pwe_port.arp;
    mpls_encap_tunnel_1[0].encap_access = bcmEncapAccessTunnel2;
    mpls_encap_tunnel_1[0].flags |= BCM_MPLS_EGRESS_LABEL_WITH_ID | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT | BCM_MPLS_EGRESS_LABEL_PROTECTION;
    mpls_encap_tunnel_1[0].tunnel_id = vpls_hvpls_info.mpls_tunnel_id_primary;
    mpls_encap_tunnel_1[0].egress_failover_id = vpls_hvpls_info.mpls_failover_id_outlif;
    mpls_encap_tunnel_1[0].egress_failover_if_id = vpls_hvpls_info.mpls_tunnel_id_protected;

    rv = mpls_create_initiator_tunnels(unit, mpls_encap_tunnel_1, 1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_create_initiator_tunnels for primary MPLS tunnel\n");
        return rv;
    }

    vpls_hvpls_info.mpls_tunnel_id_primary = mpls_encap_tunnel_1[0].tunnel_id;

    /* Configure termination labels for the ingress flow */
    vpls_mpls_tunnel_switch_create_s_init(mpls_term_tunnel);
    mpls_term_tunnel[0].action = BCM_MPLS_SWITCH_ACTION_POP;
    mpls_term_tunnel[0].label = vpls_hvpls_info.in_tunnel_label_primary;

    mpls_term_tunnel[1].action = BCM_MPLS_SWITCH_ACTION_POP;
    mpls_term_tunnel[1].label = vpls_hvpls_info.in_tunnel_label_protected;

    mpls_term_tunnel[2].action = BCM_MPLS_SWITCH_ACTION_POP;
    mpls_term_tunnel[2].label = vpls_hvpls_info.out_tunnel_label_primary;

    mpls_term_tunnel[3].action = BCM_MPLS_SWITCH_ACTION_POP;
    mpls_term_tunnel[3].label = vpls_hvpls_info.out_tunnel_label_protected;

    rv = vpls_create_termination_stack(unit, mpls_term_tunnel);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in vpls_create_termination_stack\n");
        return rv;
    }

    return rv;
}


int
vpls_hvpls_mpls_fec_create(int unit)
{
    int rv;
    bcm_gport_t gport;
    int encoded_fec;

    /* create failover id for MPLS */
    int flags = BCM_FAILOVER_FEC | BCM_FAILOVER_FEC_2ND_HIERARCHY;
    rv = bcm_failover_create(unit, flags, &vpls_hvpls_info.mpls_failover_id_fec);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_failover_create for MPLS FEC\n");
        return rv;
    }
    printf("MPLS Failover id: 0x%x\n", vpls_hvpls_info.mpls_failover_id_fec);

    /* Secondary FEC is odd */
    BCM_GPORT_LOCAL_SET(gport, pwe_port.port);
    rv = l3__egress_only_fec__create_inner(unit, vpls_hvpls_info.mpls_fec, vpls_hvpls_info.mpls_tunnel_id_protected,
                                           0, gport, BCM_L3_2ND_HIERARCHY, 0, vpls_hvpls_info.mpls_failover_id_fec, 0, &encoded_fec);

    if (rv != BCM_E_NONE)
    {
        printf("Error, in l3__egress_only_fec__create_inner for secondary FEC\n");
        return rv;
    }

    /* Primary FEC is even */
    vpls_hvpls_info.mpls_fec = vpls_hvpls_info.mpls_fec - 1;
    rv = l3__egress_only_fec__create_inner(unit, vpls_hvpls_info.mpls_fec, vpls_hvpls_info.mpls_tunnel_id_primary,
                                           0, gport, BCM_L3_2ND_HIERARCHY, 0, vpls_hvpls_info.mpls_failover_id_fec, 1, &encoded_fec);

    if (rv != BCM_E_NONE)
    {
        printf("Error, in l3__egress_only_fec__create_inner for primary FEC\n");
        return rv;
    }

    return rv;
}

int
vpls_hvpls_mpls_port_add_encapsulation(int unit)
{

    int rv;
    bcm_mpls_port_t mpls_port_egress;

    /* create egress failover id for PWE */
    rv = bcm_failover_create(unit, BCM_FAILOVER_ENCAP, &vpls_hvpls_info.pwe_failover_id_outlif);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_failover_create for PWE Egress\n");
        return rv;
    } 
    printf("PWE egress Failover id: 0x%x\n", vpls_hvpls_info.pwe_failover_id_outlif);

    bcm_mpls_port_t_init(&mpls_port_egress);
    mpls_port_egress.flags = BCM_MPLS_PORT_EGRESS_TUNNEL | BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_ENCAP_WITH_ID;
    mpls_port_egress.flags2 = BCM_MPLS_PORT2_EGRESS_ONLY | BCM_MPLS_PORT2_EGRESS_PROTECTION;
    if (pwe_cw) {
        mpls_port_egress.flags |= BCM_MPLS_PORT_CONTROL_WORD;
    }

    /* qos info */
    mpls_port_egress.egress_label.ttl = 100;
    mpls_port_egress.egress_label.flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    mpls_port_egress.egress_label.egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;

    /* ***** PWE outlif protected **** */
    mpls_port_egress.egress_failover_id = vpls_hvpls_info.pwe_failover_id_outlif;
    mpls_port_egress.egress_failover_port_id = 0;
    mpls_port_egress.egress_label.label = vpls_hvpls_info.eg_vc_label_protected;
    mpls_port_egress.encap_id = vpls_hvpls_info.pwe_outlif_protected; /* encap id is the outlif */
    /* mpls_port_id is the outlif id encapsulated as MPLS_PORT */
    BCM_GPORT_SUB_TYPE_LIF_SET(mpls_port_egress.mpls_port_id, BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY, mpls_port_egress.encap_id);
    BCM_GPORT_MPLS_PORT_ID_SET(mpls_port_egress.mpls_port_id, mpls_port_egress.mpls_port_id);

    rv = bcm_mpls_port_add(unit, 0, &mpls_port_egress);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_port_add for secondary PWE\n");
        return rv;
    }

    /* ***** PWE outlif primary **** */
    mpls_port_egress.egress_failover_id = vpls_hvpls_info.pwe_failover_id_outlif;
    mpls_port_egress.egress_failover_port_id = mpls_port_egress.encap_id;
    mpls_port_egress.egress_label.label = vpls_hvpls_info.eg_vc_label_primary;
    mpls_port_egress.encap_id = vpls_hvpls_info.pwe_outlif_primary; /* encap id is the outlif */
    /* mpls_port_id is the outlif id encapsulated as MPLS_PORT */
    BCM_GPORT_SUB_TYPE_LIF_SET(mpls_port_egress.mpls_port_id, BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY, mpls_port_egress.encap_id);
    BCM_GPORT_MPLS_PORT_ID_SET(mpls_port_egress.mpls_port_id, mpls_port_egress.mpls_port_id);

    rv = bcm_mpls_port_add(unit, 0, &mpls_port_egress);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_port_add for primary PWE\n");
        return rv;
    }

    vpls_hvpls_info.mpls_out_port_id = mpls_port_egress.mpls_port_id;

    return rv;
}

int
vpls_hvpls_pwe_fec_create(int unit)
{
    int rv;
    bcm_gport_t gport;
    int encoded_fec;
    int lif_encode;
    int mc_group = vpn;

    /* create failover id for PWE */
    rv = bcm_failover_create(unit, BCM_FAILOVER_FEC, &vpls_hvpls_info.pwe_failover_id_fec);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_failover_create for PWE FEC\n");
        return rv;
    }
    printf("PWE Failover id: 0x%x\n", vpls_hvpls_info.pwe_failover_id_fec);

    /* Secondary */
    if (mc_in_fec) {
        BCM_GPORT_MCAST_SET(gport, mc_group);
        rv = l3__egress_only_fec__create_inner(unit, vpls_hvpls_info.pwe_fec, 0,
                                               0, gport, 0, 0, vpls_hvpls_info.pwe_failover_id_fec, 0, &encoded_fec);
    } else {
        BCM_GPORT_FORWARD_PORT_SET(gport, vpls_hvpls_info.mpls_fec);
        BCM_L3_ITF_SET(lif_encode, BCM_L3_ITF_TYPE_LIF, vpls_hvpls_info.pwe_outlif_protected);
        rv = l3__egress_only_fec__create_inner(unit, vpls_hvpls_info.pwe_fec, lif_encode,
                                               0, gport, 0, 0, vpls_hvpls_info.pwe_failover_id_fec, 0, &encoded_fec);
    }
    if (rv != BCM_E_NONE)
    {
        printf("Error, in l3__egress_only_fec__create_inner for secondary FEC\n");
        return rv;
    }

    /* Primary */
    vpls_hvpls_info.pwe_fec = vpls_hvpls_info.pwe_fec - 1;
    if (mc_in_fec) {
        rv = l3__egress_only_fec__create_inner(unit, vpls_hvpls_info.pwe_fec, 0,
                                               0, gport, 0, 0, vpls_hvpls_info.pwe_failover_id_fec, 1, &encoded_fec);
    } else {
        BCM_L3_ITF_SET(lif_encode, BCM_L3_ITF_TYPE_LIF, vpls_hvpls_info.pwe_outlif_primary);
        rv = l3__egress_only_fec__create_inner(unit, vpls_hvpls_info.pwe_fec, lif_encode,
                                               0, gport, 0, 0, vpls_hvpls_info.pwe_failover_id_fec, 1, &encoded_fec);
    }
    if (rv != BCM_E_NONE)
    {
        printf("Error, in l3__egress_only_fec__create_inner for primary FEC\n");
        return rv;
    }

    return rv;
}

int
vpls_hvpls_mpls_port_add_termination(int unit)
{
    int rv;
    bcm_mpls_port_t mpls_port_ingress;
    int is_primary;

    /* 
     *  Ingress_only - primary and protected
     */
    for (is_primary=0; is_primary <= 1; is_primary++) {
        bcm_mpls_port_t_init(&mpls_port_ingress);
        mpls_port_ingress.flags = BCM_MPLS_PORT_EGRESS_TUNNEL | BCM_MPLS_PORT_WITH_ID;

        mpls_port_ingress.flags2 = BCM_MPLS_PORT2_INGRESS_ONLY;

        /* Set parameters for ingress mpls port */
        mpls_port_ingress.criteria = BCM_MPLS_PORT_MATCH_LABEL;
        mpls_port_ingress.match_label = is_primary ? vpls_hvpls_info.in_vc_label_primary : vpls_hvpls_info.in_vc_label_protected;
        mpls_port_ingress.egress_tunnel_if = 0; /* PWE In-LIF in this case do not care about MPLS egress_if. It is not part of learning information. */
        mpls_port_ingress.failover_port_id = vpls_hvpls_info.pwe_fec; /* Learning - PWE FEC */

        /* Only one inlif is configured. It should be equal to one of the Outlifs to get same-interface filtering (Multicast group member) */
        mpls_port_ingress.encap_id = is_primary ? vpls_hvpls_info.pwe_outlif_primary : vpls_hvpls_info.pwe_outlif_protected; 
        BCM_GPORT_SUB_TYPE_LIF_SET(mpls_port_ingress.mpls_port_id, BCM_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY, mpls_port_ingress.encap_id);
        mpls_port_ingress.encap_id = 0;
        BCM_GPORT_MPLS_PORT_ID_SET(mpls_port_ingress.mpls_port_id, mpls_port_ingress.mpls_port_id);

        rv = bcm_mpls_port_add(unit, vpn, &mpls_port_ingress);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_mpls_port_add\n");
            return rv;
        }

        vpls_hvpls_info.mpls_port_id = mpls_port_ingress.mpls_port_id;
    }

    return rv;
}

/*
 * VPLS hvpls scenario
 */
int
vpls_hvpls_example(
    int unit,
    int port1,
    int port2,
    int no_learning)
{
    int rv = BCM_E_NONE;
    char *proc_name;
    proc_name = "vpls_hvpls_example";
    bcm_multicast_replication_t reps[5] = {{0}};
    bcm_mac_t ac_mac = {0x07,0x08,0x09,0x0A,0x0B,0x0C};
    bcm_mac_t pwe_mac = {0x01,0x02,0x03,0x04,0x05,0x06};
    int mc_id;
    int flags;
    int gport;

    /* Init parameters */
    vpls_hvpls_info_init(unit, port1, port2);

    /*
     * create VPN and multicast group
     */
    rv = vpls_hvpls_vswitch_create(unit, vpn);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vpls_vswitch_create \n", proc_name);
        return rv;
    }

    /*
     * Configure AC and PWE ports
     */
    rv = vpls_hvpls_add_access_port(unit, ac_port, vpn);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_add_access_port ac_port\n", proc_name);
        return rv;
    }

    rv = vpls_hvpls_add_core_port(unit, pwe_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_add_core_port pwe_port\n", proc_name);
        return rv;
    }

    /*
     * Configure L3 interfaces
     */
    rv = vpls_create_l3_interfaces(unit, &pwe_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_create_l3_interfaces\n", proc_name);
        return rv;
    }

    /*
     * Configure an ARP entries
     */
    rv = vpls_create_arp_entry(unit, &pwe_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_create_arp_entry\n", proc_name);
        return rv;
    }

    /*
     * Configure MPLS tunnels
     */
    rv = vpls_hvpls_mpls_tunnels_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_hvpls_mpls_tunnels_create\n", proc_name);
        return rv;
    }

    /*
     * Configure fec entry
     */
    rv = vpls_hvpls_mpls_fec_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_hvpls_mpls_fec_create\n", proc_name);
        return rv;
    }

    /*
     * configure PWE tunnel - egress flow, set next pointer to MPLS tunnel
     */
    rv = vpls_hvpls_mpls_port_add_encapsulation(unit);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_hvpls_mpls_port_add_encapsulation\n", proc_name);
        return rv;
    }

    /*
     * configure PWE FEC
     */
    rv = vpls_hvpls_pwe_fec_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_hvpls_pwe_fec_create\n", proc_name);
        return rv;
    }

    /*
     * configure PWE tunnel - ingress flow
     */
    rv = vpls_hvpls_mpls_port_add_termination(unit);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_hvpls_mpls_port_add_termination\n", proc_name);
        return rv;
    }

    if (no_learning)
    {
        /* add l2 addresses to be defined as static - no learning needed */
        BCM_GPORT_FORWARD_PORT_SET(gport, vpls_hvpls_info.pwe_fec);
        rv = vpls_l2_addr_add(unit, vpn, pwe_mac, gport, ac_port.stat_id, ac_port.stat_pp_profile);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in vpls_l2_addr_add\n", proc_name);
            return rv;
        }

        rv = vpls_l2_addr_add(unit, vpn, ac_mac, ac_port.vlan_port_id, pwe_port.stat_id, pwe_port.stat_pp_profile);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in vpls_l2_addr_add\n", proc_name);
            return rv;
        }
    } else {
        mc_id = vpn;

        /* Add AC and PWE to multicast group */
        /* AC*/
        BCM_GPORT_LOCAL_SET(reps[0].port, ac_port.port);
        reps[0].encap1 = ac_port.encap_id;

        int pwe_if_primary, pwe_if_protected;
        bcm_if_t cuds[2];
        bcm_if_t rep_index;

        BCM_L3_ITF_SET(pwe_if_primary, BCM_L3_ITF_TYPE_LIF, vpls_hvpls_info.pwe_outlif_primary);
        BCM_L3_ITF_SET(pwe_if_protected, BCM_L3_ITF_TYPE_LIF, vpls_hvpls_info.pwe_outlif_protected);

        /* PWE outlif primery with second CUD for MPLS tunnel primery */
        cuds[0] = pwe_if_primary;
        cuds[1] = vpls_hvpls_info.mpls_tunnel_id_primary;
        rv = bcm_multicast_encap_extension_create(unit, 0, &rep_index, 2, cuds);
        if (rv != BCM_E_NONE) {
            printf("Error in 2nd copy, bcm_multicast_encap_extension_create. cud0 = 0x%08X, cud1 = 0x%08X\n", cuds[0],cuds[1]);
            return rv;
        }
        reps[1].encap1 = rep_index;
        BCM_GPORT_LOCAL_SET(reps[1].port, pwe_port.port);

        /* PWE outlif primery with second CUD for MPLS tunnel protected */
        cuds[0] = pwe_if_primary;
        cuds[1] = vpls_hvpls_info.mpls_tunnel_id_protected;
        rv = bcm_multicast_encap_extension_create(unit, 0, &rep_index, 2, cuds);
        if (rv != BCM_E_NONE) {
            printf("Error in 3rd copy, bcm_multicast_encap_extension_create. cud0 = 0x%08X, cud1 = 0x%08X\n", cuds[0],cuds[1]);
            return rv;
        }
        reps[2].encap1 = rep_index;
        BCM_GPORT_LOCAL_SET(reps[2].port, pwe_port.port);

        /* PWE outlif protected with second CUD for MPLS tunnel primery */
        cuds[0] = pwe_if_protected;
        cuds[1] = vpls_hvpls_info.mpls_tunnel_id_primary;
        rv = bcm_multicast_encap_extension_create(unit, 0, &rep_index, 2, cuds);
        if (rv != BCM_E_NONE) {
            printf("Error in 4th copy, bcm_multicast_encap_extension_create. cud0 = 0x%08X, cud1 = 0x%08X\n", cuds[0],cuds[1]);
            return rv;
        }
        reps[3].encap1 = rep_index;
        BCM_GPORT_LOCAL_SET(reps[3].port, pwe_port.port);

        /* PWE outlif protected with second CUD for MPLS tunnel protected */
        cuds[0] = pwe_if_protected;
        cuds[1] = vpls_hvpls_info.mpls_tunnel_id_protected;
        rv = bcm_multicast_encap_extension_create(unit, 0, &rep_index, 2, cuds);
        if (rv != BCM_E_NONE) {
            printf("Error in 5th copy, bcm_multicast_encap_extension_create. cud0 = 0x%08X, cud1 = 0x%08X\n", cuds[0],cuds[1]);
            return rv;
        }
        reps[4].encap1 = rep_index;
        BCM_GPORT_LOCAL_SET(reps[4].port, pwe_port.port);

        flags = egress_mc ? BCM_MULTICAST_EGRESS_GROUP : BCM_MULTICAST_INGRESS_GROUP;
        if (egress_mc) {
            rv = multicast__open_fabric_mc_or_ingress_mc_for_egress_mc(unit, mc_id, mc_id);
            if (rv != BCM_E_NONE)
            {
                printf("Error, multicast__open_fabric_mc_or_ingress_mc_for_egress_mc \n");
                return rv;
            }
        }

        rv = bcm_multicast_set(unit, mc_id, flags, 5, reps);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_multicast_set\n");
            return rv;
        }
    }

    return rv;
}

/*
 * VPLS hvpls scenario
 */
int
vpls_mc_in_fec_example(
    int unit,
    int port1,
    int port2,
    int port3,
    int no_learning)
{
    int rv = BCM_E_NONE;
    char *proc_name;
    proc_name = "vpls_mc_in_fec_example";
    l2_port_properties_s ac_port_2;

    mc_in_fec = 1;

    vpls_hvpls_example(unit,port1,port2,no_learning);

    /* Create a 2nd AC */
    ac_port_2.intf = 6;
    ac_port_2.port = port3;

    rv = vpls_hvpls_add_access_port(unit, ac_port_2, vpn);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vpls_add_access_port ac_port\n", proc_name);
        return rv;
    }

    /* Add AC to multicast group */
    rv = multicast__gport_encap_add(unit, vpn, ac_port_2.port, ac_port_2.vlan_port_id, egress_mc);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, in multicast__gport_encap_add for AC\n", proc_name);
        return rv;
    }

    return rv;
}


/*
 * Failover Set
 */
int vpls_hvpls_failover_set(int unit, uint8 is_pwe, int enable)
{
    int rv;
    bcm_failover_t failover_id, egress_failover_id;
    int egress_enable;

    failover_id = is_pwe ? vpls_hvpls_info.pwe_failover_id_fec : vpls_hvpls_info.mpls_failover_id_fec;
    egress_failover_id = is_pwe ? vpls_hvpls_info.pwe_failover_id_outlif : vpls_hvpls_info.mpls_failover_id_outlif;

    rv = bcm_failover_set(unit, failover_id, enable);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_failover_set failover_id:  0x%08x \n", failover_id);
        return rv;
    }

    egress_enable = enable;
    rv = bcm_failover_set(unit, egress_failover_id, egress_enable);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_failover_set failover_id:  0x%08x \n", egress_failover_id);
        return rv;
    }

    return rv;
}
