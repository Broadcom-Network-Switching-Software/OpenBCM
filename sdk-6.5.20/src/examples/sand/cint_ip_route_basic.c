/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * Example of a simple-router scenario
 * Test Scenario
 *
 * ./bcm.user
 * cint src/examples/sand/utility/cint_sand_utils_global.c
 * cint src/examples/sand/cint_ip_route_basic.c
 * cint
 * basic_example(0,200,202);
 * exit;
 *
 * ETH-RIF packet
 * tx 1 psrc = 200 data = 0x000c00020000000007000100080045000035000000008000fa45c08001017fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Received packets on unit 0 should be:
 * Data:
 * 0x00000000cd1d000c0002000181000064080045000035000000007f00fb45c08001017fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 */

int already_called = 0;
int default_vlan_port_id_out = 0;
int default_vlan_port_id_in = 0;

int ip_route_fec = 40961;


/*
 * This structure is used to define an L3 ingress interface.
 * It is passed to the intf_ingress_rif_set function on order to create an instance of an L3 interface with ID intf_id.
 */
struct l3_ingress_intf
{
    int intf_id;
    int vrf;
    uint32 flags;
    int qos_map;
};

l3_ingress_intf ingress_rif;
/*
 * The structure describes a set of port, interface.
 * The interface may or may not be an uRPF one - indicated by the rpf_mode field.
 */
struct l3_port_intf
{
    int port;
    int vrf;
    int intf_id;
    uint32 rpf_mode;
    bcm_mac_t mac_addr;
};

/*
 * The structure describes an egress interface.
 * It includes the data that needs to be passed to l3__egress_only_fec__create
 * to create a FEC entry.
 */
struct l3_egress_intf
{
    int fec_id;
    int intf_id;
    int encap_id;
    uint32 flags;
    bcm_gport_t gport;
};

/*
 * Utilities APIs
 */
/*
 * Initialisation function for the l3_ingress_intf structure.
 * It puts down the default values for the fields.
 */
void
l3_ingress_intf_init(
    l3_ingress_intf * ingr_intf)
{
    ingr_intf->vrf = 0;
    ingr_intf->intf_id = 0;
    ingr_intf->flags = 0;
    ingr_intf->qos_map = 0;
}

/*
 * Initialise the fields of the l3_port_intf structure with the values of the given ones.
 */
void
l3_port_intf_init(
    l3_port_intf * intf,
    int port,
    int vrf,
    int intf_id,
    uint32 uc_rpf_mode,
    bcm_mac_t mac_address)
{
    intf->port = port;
    intf->vrf = vrf;
    intf->intf_id = intf_id;
    intf->rpf_mode = uc_rpf_mode;
    sal_memcpy(intf->mac_addr, mac_address, 6);
}

/*
 * Initialise the fields of the l3_egress_intf structure with the values given as input.
 */
void
l3_egress_intf_init(
    l3_egress_intf * intf,
    int fec_idx,
    int intf_idx,
    int encap_idx,
    uint32 flags,
    bcm_gport_t gport)
{
    intf->fec_id = fec_idx;
    intf->intf_id = intf_idx;
    intf->encap_id = encap_idx;
    intf->flags = flags;
    intf->gport = gport;
}

bcm_ip6_t cint_ip_route_basic_ipv6_route ={ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x10 };
bcm_ip6_t cint_ip_route_basic_ipv6_host = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x13 };
bcm_ip6_t cint_ip_route_basic_ipv6_mask = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0 };

/*
 * Add an IPv4 Route entry <vrf, addr, mask> --> intf
 * - addr: IP address 32 bit value
 * - mask 1: to consider 0: to ingore, for example for /24 mask = 0xffffff00 will mask the 24 MSBs of the DIP
 * - vrf: VRF ID
 * - intf: egress object created using create_l3_egress (FEC)
 */
int
add_route_ipv4(
    int unit,
    uint32 addr,
    uint32 mask,
    int vrf,
    int intf)
{
    int rc;
    bcm_l3_route_t l3rt;
    bcm_l3_route_t_init(l3rt);

    l3rt.l3a_subnet = addr;
    l3rt.l3a_ip_mask = mask;
    l3rt.l3a_vrf = vrf;
    l3rt.l3a_intf = intf;       /* FEC-ID */
    if (soc_property_get(unit, "enhanced_fib_scale_prefix_length", 0))
    {
        l3rt.l3a_flags2 |= BCM_L3_FLAGS2_SCALE_ROUTE;
    }
    if (is_jericho2_kbp_ipv4_split_rpf_enabled(unit))
    {
        l3rt.l3a_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }
    rc = bcm_l3_route_add(unit, l3rt);
    if (rc != BCM_E_NONE)
    {
        printf("bcm_l3_route_add failed: %x \n", rc);
    }
    return rc;
}

/*
 * Add an IPv4 Route entry <vrf, addr, mask> --> intf
 * - addr: IP address 32 bit value
 * - mask 1: to consider 0: to ingore, for example for /24 mask = 0xffffff00 will mask the 24 MSBs of the DIP
 * - vrf: VRF ID
 * - intf: egress object created using create_l3_egress (FEC)
 */
int
add_route_ipv4_rpf(
    int unit,
    uint32 addr,
    uint32 mask,
    int vrf,
    int intf)
{
    int rc;
    bcm_l3_route_t l3rt;
    bcm_l3_route_t_init(l3rt);

    l3rt.l3a_subnet = addr;
    l3rt.l3a_ip_mask = mask;
    l3rt.l3a_vrf = vrf;
    l3rt.l3a_intf = intf;       /* FEC-ID */
    if (soc_property_get(unit, "enhanced_fib_scale_prefix_length", 0))
    {
        l3rt.l3a_flags2 |= BCM_L3_FLAGS2_SCALE_ROUTE;
    }
    if (is_jericho2_kbp_ipv4_split_rpf_enabled(unit))
    {
        l3rt.l3a_flags2 |= BCM_L3_FLAGS2_RPF_ONLY;
    }
    rc = bcm_l3_route_add(unit, l3rt);
    if (rc != BCM_E_NONE)
    {
        printf("bcm_l3_route_add failed: %x \n", rc);
    }
    return rc;
}


/*
 * Add an IPv4 Route entry <vrf, addr, mask> --> intf, as a raw value
 * - addr: IP address 32 bit value
 * - mask 1: to consider 0: to ingore, for example for /24 mask = 0xffffff00 will mask the 24 MSBs of the DIP
 * - vrf: VRF ID
 * - intf: Raw value which is, actually, egress object created using create_l3_egress (FEC)
 */
int
add_route_ipv4_raw(
    int unit,
    uint32 addr,
    uint32 mask,
    int vrf,
    int intf)
{
    int rc;
    bcm_l3_route_t l3rt;
    bcm_l3_route_t_init(l3rt);
    char *proc_name;

    proc_name = "add_route_ipv4_raw";
    l3rt.l3a_subnet = addr;
    l3rt.l3a_ip_mask = mask;
    l3rt.l3a_vrf = vrf;
    l3rt.l3a_intf = intf;       /* Raw value: FEC-ID */
    l3rt.l3a_flags2 |= BCM_L3_FLAGS2_RAW_ENTRY;
    if (is_jericho2_kbp_ipv4_split_rpf_enabled(unit))
    {
        l3rt.l3a_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }
    rc = bcm_l3_route_add(unit, l3rt);
    if (rc != BCM_E_NONE)
    {
        printf("%s(): bcm_l3_route_add failed: %x \n", proc_name, rc);
    }
    return rc;
}


/*
 * Add an IPv4 Route entry <vrf, addr, mask> --> intf, as a raw value
 * - addr: IP address 32 bit value
 * - mask 1: to consider 0: to ingore, for example for /24 mask = 0xffffff00 will mask the 24 MSBs of the DIP
 * - vrf: VRF ID
 * - intf: Raw value which is, actually, egress object created using create_l3_egress (FEC)
 */
int
add_route_ipv4_raw_rpf(
    int unit,
    uint32 addr,
    uint32 mask,
    int vrf,
    int intf)
{
    int rc;
    bcm_l3_route_t l3rt;
    bcm_l3_route_t_init(l3rt);
    char *proc_name;

    proc_name = "add_route_ipv4_raw";
    l3rt.l3a_subnet = addr;
    l3rt.l3a_ip_mask = mask;
    l3rt.l3a_vrf = vrf;
    l3rt.l3a_intf = intf;       /* Raw value: FEC-ID */
    l3rt.l3a_flags2 |= BCM_L3_FLAGS2_RAW_ENTRY;
    if (is_jericho2_kbp_ipv4_split_rpf_enabled(unit))
    {
        l3rt.l3a_flags2 |= BCM_L3_FLAGS2_RPF_ONLY;
    }
    rc = bcm_l3_route_add(unit, l3rt);
    if (rc != BCM_E_NONE)
    {
        printf("%s(): bcm_l3_route_add failed: %x \n", proc_name, rc);
    }
    return rc;
}

/*
 * Add an IPv6 Route entry using <vrf, addr, mask> that will result to an intf.
 * - addr: IPv6 address 16 8-bit values
 * - mask: 1: to consider, 0: to ingore, for example for /64 mask = ffff:ffff:ffff:ffff:0000:0000:0000:0000
 *              masks the 64 MSB of the IPv6 DIP which will be considered.
 * - vrf: VRF ID
 * - intf: egress object created using create_l3_egress (FEC)
 */
int
add_route_ipv6(
    int unit,
    bcm_ip6_t addr,
    bcm_ip6_t mask,
    int vrf,
    int intf)
{
    int rc;
    bcm_l3_route_t l3rt;
    bcm_l3_route_t_init(l3rt);

    l3rt.l3a_ip6_net = addr;
    l3rt.l3a_ip6_mask = mask;
    l3rt.l3a_vrf = vrf;
    l3rt.l3a_intf = intf;       /* FEC-ID */
    l3rt.l3a_flags = BCM_L3_IP6;
    if (soc_property_get(unit, "enhanced_fib_scale_prefix_length", 0)
        || soc_property_get(unit, "enhanced_fib_scale_prefix_length_ipv6_long", 0))
    {
        l3rt.l3a_flags2 |= BCM_L3_FLAGS2_SCALE_ROUTE;
    }
    if (is_jericho2_kbp_ipv6_split_rpf_enabled(unit))
    {
        l3rt.l3a_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }
    rc = bcm_l3_route_add(unit, l3rt);
    if (rc != BCM_E_NONE)
    {
        printf("bcm_l3_route_add failed: %x \n", rc);
    }
    return rc;
}

/*
 * Add an IPv6 Route entry using <vrf, addr, mask> that will result to an intf.
 * - addr: IPv6 address 16 8-bit values
 * - mask: 1: to consider, 0: to ingore, for example for /64 mask = ffff:ffff:ffff:ffff:0000:0000:0000:0000
 *              masks the 64 MSB of the IPv6 DIP which will be considered.
 * - vrf: VRF ID
 * - intf: egress object created using create_l3_egress (FEC)
 */
int
add_route_ipv6_rpf(
    int unit,
    bcm_ip6_t addr,
    bcm_ip6_t mask,
    int vrf,
    int intf)
{
    int rc;
    bcm_l3_route_t l3rt;
    bcm_l3_route_t_init(l3rt);

    l3rt.l3a_ip6_net = addr;
    l3rt.l3a_ip6_mask = mask;
    l3rt.l3a_vrf = vrf;
    l3rt.l3a_intf = intf;       /* FEC-ID */
    l3rt.l3a_flags = BCM_L3_IP6;
    if (soc_property_get(unit, "enhanced_fib_scale_prefix_length", 0)
        || soc_property_get(unit, "enhanced_fib_scale_prefix_length_ipv6_long", 0))
    {
        l3rt.l3a_flags2 |= BCM_L3_FLAGS2_SCALE_ROUTE;
    }
    if (is_jericho2_kbp_ipv6_split_rpf_enabled(unit))
    {
        l3rt.l3a_flags2 |= BCM_L3_FLAGS2_RPF_ONLY;
    }
    rc = bcm_l3_route_add(unit, l3rt);
    if (rc != BCM_E_NONE)
    {
        printf("bcm_l3_route_add failed: %x \n", rc);
    }
    return rc;
}

/*
 * Add an IPv6 Route entry using <vrf, addr, mask> that will result to an intf.
 * - addr: IPv6 address 16 8-bit values
 * - mask: 1: to consider, 0: to ingore, for example for /64 mask = ffff:ffff:ffff:ffff:0000:0000:0000:0000
 *              masks the 64 MSB of the IPv6 DIP which will be considered.
 * - vrf: VRF ID
 * - intf: Raw value of FEC (egress object created using create_l3_egress)
 */
int
add_route_ipv6_raw(
    int unit,
    bcm_ip6_t addr,
    bcm_ip6_t mask,
    int vrf,
    int intf)
{
    int rc;
    bcm_l3_route_t l3rt;
    char *proc_name;

    proc_name = "add_route_ipv6_raw";
    bcm_l3_route_t_init(l3rt);
    l3rt.l3a_ip6_net = addr;
    l3rt.l3a_ip6_mask = mask;
    l3rt.l3a_vrf = vrf;
    l3rt.l3a_intf = intf;       /* Raw value of FEC-ID */
    l3rt.l3a_flags |= BCM_L3_IP6;
    l3rt.l3a_flags2 |= BCM_L3_FLAGS2_RAW_ENTRY;
    if (is_jericho2_kbp_ipv6_split_rpf_enabled(unit))
    {
        l3rt.l3a_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }
    rc = bcm_l3_route_add(unit, l3rt);
    if (rc != BCM_E_NONE)
    {
        printf("%s(): bcm_l3_route_add failed: %x \n", proc_name, rc);
    }
    return rc;
}

/*
 * Add an IPv6 Route entry using <vrf, addr, mask> that will result to an intf.
 * - addr: IPv6 address 16 8-bit values
 * - mask: 1: to consider, 0: to ingore, for example for /64 mask = ffff:ffff:ffff:ffff:0000:0000:0000:0000
 *              masks the 64 MSB of the IPv6 DIP which will be considered.
 * - vrf: VRF ID
 * - intf: Raw value of FEC (egress object created using create_l3_egress)
 */
int
add_route_ipv6_raw_rpf(
    int unit,
    bcm_ip6_t addr,
    bcm_ip6_t mask,
    int vrf,
    int intf)
{
    int rc;
    bcm_l3_route_t l3rt;
    char *proc_name;

    proc_name = "add_route_ipv6_raw";
    bcm_l3_route_t_init(l3rt);
    l3rt.l3a_ip6_net = addr;
    l3rt.l3a_ip6_mask = mask;
    l3rt.l3a_vrf = vrf;
    l3rt.l3a_intf = intf;       /* Raw value of FEC-ID */
    l3rt.l3a_flags |= BCM_L3_IP6;
    l3rt.l3a_flags2 |= BCM_L3_FLAGS2_RAW_ENTRY;
    if (is_jericho2_kbp_ipv6_split_rpf_enabled(unit))
    {
        l3rt.l3a_flags2 |= BCM_L3_FLAGS2_RPF_ONLY;
    }
    rc = bcm_l3_route_add(unit, l3rt);
    if (rc != BCM_E_NONE)
    {
        printf("%s(): bcm_l3_route_add failed: %x \n", proc_name, rc);
    }
    return rc;
}

/*
 * Add IPv4 Host entry <vrf, addr> --> intf
 * - addr: IP address 32 bit value
 * - vrf: VRF ID
 * - intf: egress object created using create_l3_egress (FEC)
 * - encap_id: arp_encap_id encapsulation ID of an l3 egress interface.
 * - destination: when host format is {System-Port, OutRIF, ARP} (No-FEC),
 *   this field should contain gport, and will be assigned to host.l3a_port_tgid,
 *   otherwise this field should contain 0.
 *
 * If encap_id = 0, the destination will be a FEC.
 * If encap_id != 0, the entry destination will be a FEC + out-LIF (ARP).
 */
int
add_host_ipv4(
    int unit,
    uint32 addr,
    int vrf,
    int intf,
    int encap_id,
    int destination)
{
    int rc;
    bcm_l3_host_t host;
    char *proc_name;

    proc_name = "add_host_ipv4";
    /* printf("%s(): Enter. addr 0x%08X intf 0x%08X destination 0x%08X\n",proc_name, addr, intf, destination); */
    bcm_l3_host_t_init(host);

    host.l3a_ip_addr = addr;
    host.l3a_vrf = vrf;
    if (is_device_or_above(unit, JERICHO2))
    {
        host.l3a_intf = (intf != 0) ? intf : encap_id;
        host.encap_id = (intf != 0) ? encap_id : 0;
    }
    else
    {
        host.l3a_intf = intf;       /* FEC-ID or OUTRIF */
        host.encap_id = encap_id;
    }
    if (destination)
    {
        host.l3a_port_tgid = destination;
    }
    rc = bcm_l3_host_add(unit, host);
    if (rc != BCM_E_NONE)
    {
        printf("%s(): bcm_l3_host_add failed: %x \n",proc_name, rc);
    }
    /* printf("%s(): Exit\n",proc_name); */
    return rc;
}

/*
 * Add IPv6 Host entry <vrf, addr> --> intf
 * - addr: IPv6 address 16 8-bit values
 * - vrf: VRF ID
 * - intf: egress object created using create_l3_egress (FEC)
 */
int
add_host_ipv6(
    int unit,
    bcm_ip6_t addr,
    int vrf,
    int intf)
{
    int rc;
    bcm_l3_host_t host;
    bcm_l3_host_t_init(host);

    host.l3a_ip6_addr = addr;
    host.l3a_vrf = vrf;
    host.l3a_intf = intf;       /* FEC-ID */
    host.l3a_flags = BCM_L3_IP6;
    rc = bcm_l3_host_add(unit, host);
    if (rc != BCM_E_NONE)
    {
        printf("bcm_l3_host_add failed: %x \n", rc);
    }
    return rc;
}

/*
 * Add IPv6 Host entry <vrf, addr> --> intf
 * - addr: IPv6 address 16 8-bit values
 * - vrf: VRF ID
 * - intf: egress object created using create_l3_egress (FEC)
 * - encap_id: arp_encap_id encapsulation ID of an l3 egress interface.
 * - destination: when host format is {System-Port, OutRIF, ARP} (No-FEC),
 *   this field should contain gport, and will be assigned to host.l3a_port_tgid,
 *   otherwise this field should contain 0.
 *
 * If encap_id = 0, the destination will be a FEC.
 * If encap_id != 0, the entry destination will be a FEC + out-LIF (ARP).
 */
int
add_host_ipv6_encap_dest(
    int unit,
    bcm_ip6_t addr,
    int vrf,
    int intf,
    int encap_id,
    int destination)
{
    int rc;
    bcm_l3_host_t host;

    char *proc_name;
    proc_name = "add_host_ipv6_encap_dest";

    bcm_l3_host_t_init(host);

    host.l3a_ip6_addr = addr;
    host.l3a_vrf = vrf;
    host.l3a_flags = BCM_L3_IP6;

    if (is_device_or_above(unit, JERICHO2))
    {
        host.l3a_intf = (intf != 0) ? intf : encap_id;
        host.encap_id = (intf != 0) ? encap_id : 0;
    }
    else
    {
        host.l3a_intf = intf;       /* FEC-ID or OUTRIF */
        host.encap_id = encap_id;
    }
    if (destination)
    {
        host.l3a_port_tgid = destination;
    }

    rc = bcm_l3_host_add(unit, host);
    if (rc != BCM_E_NONE)
    {
        printf("%s(): bcm_l3_host_add failed: %x \n", proc_name, rc);
    }
    return rc;
}
int in_port_intf_num = 0;
bcm_gport_t in_port_intf[4] = {0, 0, 0, 0};
/*
 * Set In-Port default ETH-RIF:
 * - in_port: Incoming port ID
 * - eth_rif: ETH-RIF
 */
int
in_port_intf_set(
    int unit,
    int in_port,
    int eth_rif)
{
    bcm_vlan_port_t vlan_port;
    int rc;
    char *proc_name;

    proc_name = "in_port_intf_set";
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.port = in_port;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vlan_port.vsi = eth_rif;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;

    rc = bcm_vlan_port_create(unit, vlan_port);
    if (rc != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_vlan_port_create\n", proc_name);
        return rc;
    }
    in_port_intf[in_port_intf_num % 4] = vlan_port.vlan_port_id;
    in_port_intf_num ++;
    default_vlan_port_id_in = vlan_port.vlan_port_id;

    printf("%s(): port = %d, in_lif = 0x%08X, in_port_intf_num = %d\n", proc_name, vlan_port.port, vlan_port.vlan_port_id, in_port_intf_num);

    rc = bcm_vlan_gport_add(unit, eth_rif, in_port, 0);
    if (rc != BCM_E_NONE)
    {
        printf("Error in %s(): bcm_vlan_gport_add \n", proc_name);
        return rc;
    }

    return rc;
}

int
in_port_intf_reset(
    int unit)
{
    int i;
    int rc = BCM_E_NONE;
    char *proc_name;

    proc_name = "in_port_intf_reset";

    for (i = 0; i < 4; i++)
    {
        if(0 != in_port_intf[i])
        {
            rc = bcm_vlan_port_destroy(unit, in_port_intf[i]);
            if (rc != BCM_E_NONE)
            {
                printf("Error in %s(): bcm_vlan_port_destroy \n", proc_name);
                return rc;
            }
            in_port_intf[i] = 0;
        }
    }

    in_port_intf_num = 0;

    return rc;
}

int
in_port_intf_get(
    int unit,
    int index,
    bcm_gport_t *in_intf)
{
   int rc = BCM_E_NONE;
   char *proc_name;

   proc_name = "in_port_intf_get";

   if (index <0 || index > 3) {
      printf("Error in %s(): index = %d is out of range [0:3] \n", proc_name, index);
      return rc;
   }

   *in_intf = in_port_intf[index];

   printf("%s(): in_intf = 0x%08X\n", proc_name, *in_intf);

   return rc;
}

/*
 * Set Out-Port default properties:
 * - out_port: Outgoing port ID
 */
int
out_port_set(
    int unit,
    int out_port)
{
    bcm_vlan_port_t vlan_port;
    bcm_port_match_info_t match_info;
    int rc, is_default_ac_existing = 0;
    char *proc_name;

    proc_name = "out_port_set";

    /* Try to use the same vlan port as egress default in JR2 if possible. */
    if (is_device_or_above(unit,JERICHO2) && BCM_GPORT_IS_VLAN_PORT(default_vlan_port_id_out) && BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_DEFAULT(default_vlan_port_id_out)) {
        bcm_vlan_port_t_init(&vlan_port);

        /* Find the vlan-port first to confirm it indeed exists already.*/
        vlan_port.vlan_port_id = default_vlan_port_id_out;
        rc = bcm_vlan_port_find(unit, &vlan_port);
        if (rc == BCM_E_NOT_FOUND) {
            is_default_ac_existing = 0;
        } else if (rc != BCM_E_NONE) {
            printf("%s(): Error, bcm_vlan_port_find vlan_port\n",proc_name);
            return rc;
        } else if (vlan_port.flags & BCM_VLAN_PORT_NATIVE) {
            /** If the AC can be found, but not for port default or is for native default, we need to recreate one.*/
            is_default_ac_existing = 0;
        } else {
            is_default_ac_existing = 1;
        }
    }

    if (is_default_ac_existing == 0) {
        bcm_vlan_port_t_init(&vlan_port);

        vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
        vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY;
        if (is_device_or_above(unit, JERICHO2))
        {
            vlan_port.flags |= BCM_VLAN_PORT_DEFAULT;
            vlan_port.flags |= BCM_VLAN_PORT_VLAN_TRANSLATION;
        }

        rc = bcm_vlan_port_create(unit, &vlan_port);
        if (rc != BCM_E_NONE)
        {
            printf("%s(): Error, bcm_vlan_port_create vlan_port\n",proc_name);
            return rc;
        }

        default_vlan_port_id_out = vlan_port.vlan_port_id;

        printf("%s(): bcm_vlan_port_create() result: port = %d, vlan_port_id = 0x%08X\r\n",proc_name, out_port, default_vlan_port_id_out);
    } else {
        printf("%s(): using the existing vlan_port: port = %d, vlan_port_id = 0x%08X\r\n",proc_name, out_port, default_vlan_port_id_out);
    }

    bcm_port_match_info_t_init(&match_info);
    match_info.match = BCM_PORT_MATCH_PORT;
    match_info.flags = BCM_PORT_MATCH_EGRESS_ONLY;
    match_info.port = out_port;

    rc = bcm_port_match_add(unit, default_vlan_port_id_out, &match_info);
    if (rc != 0)
    {
        printf("%s(): Error, in bcm_port_match_add \n",proc_name);
        return rc;
    }

    return rc;
}

/*
 * Set In-VSI ETH-RIF properties:
 * - eth_rif_id: ETH-RIF ID
 * - vrf: VRF ID
 * - qos_map_id: QOS_MAP_ID
 */
int
intf_ingress_rif_set(
    int unit,
    l3_ingress_intf * ingress_itf)
{
    bcm_l3_ingress_t l3_ing_if;
    int rc;

    bcm_l3_ingress_t_init(&l3_ing_if);
    l3_ing_if.flags = ingress_itf->flags | BCM_L3_INGRESS_WITH_ID;      /* must, as we update exist RIF */
    l3_ing_if.vrf = ingress_itf->vrf;
    l3_ing_if.qos_map_id = ingress_itf->qos_map;

    rc = bcm_l3_ingress_create(unit, l3_ing_if, ingress_itf->intf_id);
    if (rc != BCM_E_NONE)
    {
        printf("Error, bcm_l3_ingress_create\n");
        return rc;
    }

    return rc;
}


/*
 * Create VSI ETH-RIF and set initial properties:
 * - my_mac - My-MAC address
 */
int
intf_eth_rif_create(
    int unit,
    int eth_rif_id,
    bcm_mac_t my_mac)
{
    bcm_l3_intf_t l3if;
    int rc;

    /*
     * Initialize a bcm_l3_intf_t structure.
     */
    bcm_l3_intf_t_init(&l3if);
    l3if.l3a_flags = BCM_L3_WITH_ID;
    /*
     * My-MAC
     */
    l3if.l3a_mac_addr = my_mac;
    l3if.l3a_intf_id = l3if.l3a_vid = eth_rif_id;
    /* set qos map id to 0 as default */
    l3if.dscp_qos.qos_map_id = 0;

    rc = bcm_l3_intf_create(unit, l3if);
    if (rc != BCM_E_NONE)
    {
        printf("Error, bcm_l3_intf_create %d\n", rc);
        return rc;
    }

    return rc;
}

/*
 * Create VSI ETH-RIF and set initial properties:
 * - my_mac - My-MAC address
 */
int
intf_eth_rif_create_with_ttl(
    int unit,
    int eth_rif_id,
    bcm_mac_t my_mac,
    int ttl_threshold)
{
    bcm_l3_intf_t l3if;
    int rc;

    /*
     * Initialize a bcm_l3_intf_t structure.
     */
    bcm_l3_intf_t_init(&l3if);
    l3if.l3a_flags = BCM_L3_WITH_ID;
    /*
     * My-MAC
     */
    l3if.l3a_mac_addr = my_mac;
    l3if.l3a_intf_id = l3if.l3a_vid = eth_rif_id;
    /* set qos map id to 0 as default */
    l3if.dscp_qos.qos_map_id = 0;
    /* set ttl */
    l3if.l3a_ttl = ttl_threshold;

    rc = bcm_l3_intf_create(unit, &l3if);
    if (rc != BCM_E_NONE)
    {
        printf("Error, bcm_l3_intf_create %d\n", rc);
        return rc;
    }

    return rc;
}


/*
 * Create VSI ETH-RIF and set initial properties:
 * - my_mac - My-MAC address
 */
int
intf_eth_rif_create_with_qos(
    int unit,
    int eth_rif_id,
    int qos_map_id,
    bcm_mac_t my_mac)
{
    bcm_l3_intf_t l3if;
    int rc;

    /*
     * Initialize a bcm_l3_intf_t structure.
     */
    bcm_l3_intf_t_init(&l3if);
    l3if.l3a_flags = BCM_L3_WITH_ID;

    /*
     * My-MAC
     */
    l3if.l3a_mac_addr = my_mac;
    l3if.l3a_intf_id = l3if.l3a_vid = eth_rif_id;
    l3if.dscp_qos.qos_map_id = qos_map_id;

    rc = bcm_l3_intf_create(unit, l3if);
    if (rc != BCM_E_NONE)
    {
        printf("Error, bcm_l3_intf_create %d\n", rc);
        return rc;
    }

    return rc;
}

/*
 * Create VSI ETH-RIF with qos uniform model :
 */
int
intf_eth_rif_create_with_qos_uniform(
    int unit,
    int eth_rif_id,
    bcm_mac_t my_mac)
{
    bcm_l3_intf_t l3if;
    int rc;

    /*
     * Initialize a bcm_l3_intf_t structure.
     */
    bcm_l3_intf_t_init(&l3if);
    l3if.l3a_flags = BCM_L3_WITH_ID;

    /*
     * My-MAC
     */
    l3if.l3a_mac_addr = my_mac;
    l3if.l3a_intf_id = l3if.l3a_vid = eth_rif_id;
    l3if.ingress_qos_model.ingress_phb = bcmQosIngressModelUniform;
    l3if.ingress_qos_model.ingress_remark = bcmQosIngressModelUniform;
    l3if.ingress_qos_model.ingress_ttl = bcmQosIngressModelUniform;

    rc = bcm_l3_intf_create(unit, l3if);
    if (rc != BCM_E_NONE)
    {
        printf("Error, bcm_l3_intf_create %d\n", rc);
        return rc;
    }

    return rc;
}

/**
 * Delete a VSI ETH-RIF instance
 */
int
intf_eth_rif_delete(
    int unit,
    int eth_rif_id)
{
    bcm_l3_intf_t l3if;
    int rc;

    /*
     * Initialize a bcm_l3_intf_t structure.
     */
    bcm_l3_intf_t_init(&l3if);
    l3if.l3a_flags = BCM_L3_WITH_ID;
    l3if.l3a_intf_id = l3if.l3a_vid = eth_rif_id;
    rc = bcm_l3_intf_delete(unit, l3if);
    if (rc != BCM_E_NONE)
    {
        printf("Error, bcm_l3_intf_create %d\n", rc);
        return rc;
    }

    return rc;
}

/*
 * Set a FEC entry, without allocating ARP entry
 * - fec_id - FEC-ID
 * - out_rif - Outgoing ETH-RIF or the MPLS tunnel
 * - arp - Outgoing ARP pointer
 * - out_port - destination Outgoing port
 * - host_format_fec_outlif - flag that tells if host format is FEC+OUTLIF
 * - failover_id - protection pointer
 * - failover_is_primary - relevant when failover_id != 0. 1 for primary path, 0 for secondary path
 * - interface_id - output parameter, the encoded interface id value returned from bcm_l3_egress_create
 */

int
l3__egress_only_fec__create_inner(
    int unit,
    int fec_id,
    int out_rif_or_tunnel,
    int arp,
    bcm_gport_t gport,
    int flags,
    int failover_id,
    int failover_is_primary,
    int* interface_id)
{
    int rc;
    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid;
    char *proc_name;

    proc_name = "l3__egress_only_fec__create_inner";
    /* printf("%s(): Enter. fec_id 0x%08X\r\n", proc_name, fec_id); */
    bcm_l3_egress_t_init(&l3eg);
    if (is_device_or_above(unit, JERICHO2)) {
        l3eg.intf = out_rif_or_tunnel ? out_rif_or_tunnel : arp;
        l3eg.encap_id = out_rif_or_tunnel ? arp : 0;
    } else {
        l3eg.intf = out_rif_or_tunnel;
        l3eg.encap_id = arp;
    }

    if (failover_is_primary)
    {
        l3eg.failover_if_id = fec_id + 1;
    }
    l3eg.failover_id = failover_id;
    l3eg.port = gport;
    l3eg.flags = flags;
    l3egid = fec_id;

    rc = bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY | BCM_L3_WITH_ID, &l3eg, &l3egid);
    if (interface_id != NULL)
    {
        *interface_id = l3egid;
    }
    if (rc != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object, unit=%d, \n", proc_name, unit);
        return rc;
    }
    /*
     * The code below is not operationally required and is added for debug purposes only.
     * It may be removed, if necessary.
     */
    rc = bcm_l3_egress_get(unit, l3egid, &l3eg);
    if (rc != BCM_E_NONE)
    {
        printf("%s(): Error, in bcm_l3_egress_get(), unit=%d, \n", proc_name, unit);
        return rc;
    }

    /* printf("%s(): Exit. rc=%d\r\n", proc_name, rc); */
    return rc;
}

int
l3__egress_only_fec__create(
    int unit,
    int fec_id,
    int out_rif_or_tunnel,
    int arp,
    bcm_gport_t out_port,
    int flags)
{
    return l3__egress_only_fec__create_inner(unit, fec_id, out_rif_or_tunnel, arp, out_port, flags, 0, 0,NULL);
}

/*
 * Set an ARP entry, without allocating FEC entry
 * - allocation_flags - with_id, replace, ingress_only, egress_only
 * - arp_id - ARP-ID
 * - da - Destination MAC address.
 * - vsi - vs ifield for arp extended format.
 * - flags - flags for arp creation (native or outer ethernet).
 * - flags2 - can be used for customized usage, for example in order
 * to configure ARP+AC entry instead of normal ARP, pass the flag BCM_L3_FLAGS2_VLAN_TRANSLATION
 */
int
l3__egress_only_encap__create_inner(
    int unit,
    uint32 allocation_flags,
    int *arp_id,
    bcm_mac_t da,
    int vsi,
    uint32 flags,
    uint32 flags2)
{

    int rc;
    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid_null;       /* not intersting */
    char *proc_name;

    proc_name = "l3__egress_only_encap__create_inner";
    /* printf("%s(): Enter. \r\n", proc_name); */

    bcm_l3_egress_t_init(&l3eg);

    l3eg.mac_addr = da;
    l3eg.encap_id = *arp_id;
    l3eg.vlan = vsi;
    l3eg.flags = flags;
    if (flags2 & BCM_L3_FLAGS2_VLAN_TRANSLATION)
    {
        l3eg.flags2 |= BCM_L3_FLAGS2_VLAN_TRANSLATION;
    }
    if (flags2 & BCM_L3_FLAGS2_VLAN_TRANSLATION_TWO_VLAN_TAGS)
    {
        l3eg.flags2 |= BCM_L3_FLAGS2_VLAN_TRANSLATION_TWO_VLAN_TAGS;
    }
    if (flags2 & BCM_L3_FLAGS2_VIRTUAL_EGRESS_POINTED)
    {
        l3eg.flags2 |= BCM_L3_FLAGS2_VIRTUAL_EGRESS_POINTED;
        /* printf("%s(): Going to call bcm_l3_egress_create() WITH VIRTUAL_EGRESS_POINTED.\r\n", proc_name); */
    }
    allocation_flags |= BCM_L3_EGRESS_ONLY;

    /*
     * printf("%s(): Going to call bcm_l3_egress_create() for l3eg.flags2 = 0x%08X, allocation_flags = 0x%08X\r\n==> encap_id = 0x%08X, intf = 0x%08X\r\n",
     *           proc_name, l3eg.flags2, allocation_flags, l3eg.encap_id, l3eg.intf);
     */
    rc = bcm_l3_egress_create(unit, allocation_flags, &l3eg, &l3egid_null);
    if (rc != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object, \n", proc_name);
        return rc;
    }

    /* printf("%s(): Resultant encap_id = 0x%08X, intf = 0x%08X\r\n", proc_name, l3eg.encap_id, l3eg.intf); */

    *arp_id = l3eg.encap_id;

    return rc;
}
/*
 * On allocation_flags, ONLY the following are acceptable:
 *   BCM_L3_EGRESS_ONLY
 *   BCM_L3_INGRESS_ONLY
 *   BCM_L3_FLAGS2_VIRTUAL_EGRESS_POINTED
 */
int
l3__egress_only_encap__create(
    int unit,
    uint32 allocation_flags,
    int *arp_id,
    bcm_mac_t da,
    int vsi)
{
    char *proc_name;
    uint32 flags2;

    proc_name = "l3__egress_only_encap__create";
    /*
     * Only specified bits are allowed on 'allocation_flags'
     */
    if ((allocation_flags & ~(BCM_L3_EGRESS_ONLY | BCM_L3_INGRESS_ONLY | BCM_L3_FLAGS2_VIRTUAL_EGRESS_POINTED)) != 0)
    {
        printf("%s(): Error, some bits on 'allocation_flags' (0x%08X) are not allowed. Allowed mask is 0x%08X\r\n",
            proc_name, allocation_flags,
            BCM_L3_EGRESS_ONLY | BCM_L3_INGRESS_ONLY | BCM_L3_FLAGS2_VIRTUAL_EGRESS_POINTED);
        rc = BCM_E_PARAM;
        return rc;
    }
    flags2 = 0;
    /*
     * Convert some 'allocation_flags' to their 'flags2' equivalent and clear them.
     */
    if (allocation_flags & BCM_L3_FLAGS2_VIRTUAL_EGRESS_POINTED)
    {
        flags2 |= BCM_L3_FLAGS2_VIRTUAL_EGRESS_POINTED;
        allocation_flags &= ~BCM_L3_FLAGS2_VIRTUAL_EGRESS_POINTED;
    }
    return l3__egress_only_encap__create_inner(unit, allocation_flags, arp_id, da, vsi, 0, flags2);
}

/**
 * Utility function: create an AC and ARP pointing to it
 * double VLAN translation
 */
int create_arp_with_next_ac_type(
        int unit,
        bcm_mac_t arp_next_hop_mac,
        int *encap_id,
        int vlan)
{
    int rv;
    char *proc_name;
    bcm_gport_t vlan_port_gport;
    bcm_vlan_port_t vlan_port;
    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid_null;       /* not interesting */

    proc_name = "create_arp_with_next_ac_type";
    printf("%s(): Enter. arp_nex_hop_mac %02X:%02X:%02X:%02X:%02X:%02X encap_id 0x%04X vlan %d\r\n",
            proc_name, arp_next_hop_mac[0], arp_next_hop_mac[1], arp_next_hop_mac[2],
            arp_next_hop_mac[3], arp_next_hop_mac[4], arp_next_hop_mac[5], *encap_id, vlan);

    printf("%s(): Creating vlan port\n", proc_name);
    /*
     * Add VLAN port in egress
     */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.vsi = 0;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in bcm_vlan_port_create\n", rv);
        return rv;
    }
    vlan_port_gport = vlan_port.vlan_port_id;
    printf("egress_vlan_port_id: 0x%08x\n", vlan_port_gport);

    printf("%s(): Creating arp with encap_id 0x%08X\n", proc_name, *encap_id);

    bcm_l3_egress_t_init(&l3eg);

    l3eg.mac_addr = arp_next_hop_mac;
    l3eg.encap_id = *encap_id;
    l3eg.vlan = vlan;
    l3eg.flags = 0;
    l3eg.vlan_port_id = vlan_port_gport;

    rv = bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY, &l3eg, &l3egid_null);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object, \n", proc_name);
        return rv;
    }

    printf("%s(): Result: encap_id = 0x%08X, intf = 0x%08X\r\n", proc_name, l3eg.encap_id, l3eg.intf);

    *encap_id = l3eg.encap_id;

    /*
     * use bcm_vlan_port_translation_set for vlan translation
     */

    printf("%s(): Call bcm_vlan_port_translation_set() on vlan port\n",proc_name);
    rv = vlan_translate_ive_eve_translation_set(unit, vlan_port_gport,         /* lif  */
                                                       0x9100,               /* outer_tpid */
                                                       0x8100,               /* inner_tpid */
                                                       bcmVlanActionAdd,  /* outer_action */
                                                       bcmVlanActionAdd,  /* inner_action */
                                                       vlan,                 /* new_outer_vid */
                                                       2*vlan,           /* new_inner_vid */
                                                       7,    /* vlan_edit_profile */
                                                       0,                    /* tag_format - must be untag! */
                                                       FALSE                 /* is_ive */
                                                       );
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress vlan editing, \n", proc_name);
        return rv;
    }

    printf("%s(): Exit\r\n",proc_name);
    return rv;
}

/**
 * Utility function: create an AC and ARP pointing to it 
 * VLAN translation is NONE
 */
int create_arp_with_next_ac_type_vlan_translate_none(
        int unit,
        bcm_mac_t arp_next_hop_mac,
        int *encap_id,
        int vlan)
{
    int rv;
    char *proc_name;
    bcm_gport_t vlan_port_gport;
    bcm_vlan_port_t vlan_port;
    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid_null;       /* not interesting */

    proc_name = "create_arp_with_next_ac_type";
    printf("%s(): Enter. arp_nex_hop_mac %02X:%02X:%02X:%02X:%02X:%02X encap_id 0x%04X vlan %d\r\n",
            proc_name, arp_next_hop_mac[0], arp_next_hop_mac[1], arp_next_hop_mac[2],
            arp_next_hop_mac[3], arp_next_hop_mac[4], arp_next_hop_mac[5], *encap_id, vlan);

    printf("%s(): Creating vlan port\n", proc_name);
    /*
     * Add VLAN port in egress
     */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.vsi = 0;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in bcm_vlan_port_create\n", rv);
        return rv;
    }
    vlan_port_gport = vlan_port.vlan_port_id;
    printf("egress_vlan_port_id: 0x%08x\n", vlan_port_gport);

    printf("%s(): Creating arp with encap_id 0x%08X\n", proc_name, *encap_id);

    bcm_l3_egress_t_init(&l3eg);

    l3eg.mac_addr = arp_next_hop_mac;
    l3eg.encap_id = *encap_id;
    l3eg.vlan = vlan;
    l3eg.flags = 0;
    l3eg.vlan_port_id = vlan_port_gport;

    rv = bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY, &l3eg, &l3egid_null);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object, \n", proc_name);
        return rv;
    }

    printf("%s(): Result: encap_id = 0x%08X, intf = 0x%08X\r\n", proc_name, l3eg.encap_id, l3eg.intf);

    *encap_id = l3eg.encap_id;

    printf("%s(): Exit\r\n",proc_name);
    return rv;
}

/**
 * Utility function: create an ARP with
 * single or double VLAN translation or
 * without it
 */
int create_arp_plus_ac_type(
        int unit,
        int arp_plus_ac_type,
        bcm_mac_t arp_next_hop_mac,
        int *encap_id,
        int vlan)
{
    int rv;
    uint32 flags2 = 0;
    char *proc_name;
    proc_name = "create_arp_plus_ac_type";
    printf("%s(): Enter. arp_plus_ac_type %d arp_nex_hop_mac %02X:%02X:%02X:%02X:%02X:%02X encap_id 0x%04X vlan %d\r\n",
            proc_name, arp_plus_ac_type, arp_next_hop_mac[0], arp_next_hop_mac[1], arp_next_hop_mac[2],
            arp_next_hop_mac[3], arp_next_hop_mac[4], arp_next_hop_mac[5], *encap_id, vlan);

    if (arp_plus_ac_type)
    {
        flags2 |= BCM_L3_FLAGS2_VLAN_TRANSLATION;
    }
    if (arp_plus_ac_type == 2)
    {
        flags2 |= BCM_L3_FLAGS2_VLAN_TRANSLATION_TWO_VLAN_TAGS;
    }
    printf("%s(): Going to call l3__egress_only_encap__create_inner() with encap_id 0x%08X, flags2 0x%08X\n",
                proc_name, *encap_id, flags2);
    rv = l3__egress_only_encap__create_inner(unit, 0, encap_id, arp_next_hop_mac, vlan, 0, flags2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object ARP only\n",proc_name);
        return rv;
    }

    if (arp_plus_ac_type)
    {
        /*
         * for arp+ac, bcm_vlan_port_translation_set is the API which sets the AC part in
         * the ARP+AC entry in EEDB
         */
        int inner_action = (arp_plus_ac_type == 2) ? bcmVlanActionArpVlanTranslateAdd : bcmVlanActionNone;
        int inner_vlan = (arp_plus_ac_type == 2) ? 2*vlan : 0;

        printf("%s(): We have arp_plus_ac. Call bcm_vlan_port_translation_set()\n",proc_name);
        bcm_vlan_port_translation_t lif_translation_info;
        bcm_vlan_port_translation_t_init(&lif_translation_info);
        int arp_outlif;
        int vlan_edit_profile = 7;
        bcm_gport_t arp_ac_gport;

        arp_outlif = BCM_L3_ITF_VAL_GET(*encap_id);
        BCM_GPORT_SUB_TYPE_L3_VLAN_TRANSLATION_SET(arp_ac_gport, arp_outlif);
        BCM_GPORT_VLAN_PORT_ID_SET(arp_ac_gport, arp_ac_gport);

        /*
        * Perform vlan editing:
        */
        rv = vlan_translate_ive_eve_translation_set(unit, arp_ac_gport,         /* lif  */
                                                          0x9100,               /* outer_tpid */
                                                          0x8100,               /* inner_tpid */
                                                          bcmVlanActionArpVlanTranslateAdd,  /* outer_action */
                                                          inner_action,  /* inner_action */
                                                          vlan,                 /* new_outer_vid */
                                                          inner_vlan,           /* new_inner_vid */
                                                          vlan_edit_profile,    /* vlan_edit_profile */
                                                          0,                    /* tag_format - must be untag! */
                                                          FALSE                 /* is_ive */
                                                          );

        /*
        * Check that the VLAN Edit info was set correctly:
        */
        bcm_vlan_port_translation_t lif_translation_info_get;
        lif_translation_info_get.flags = BCM_VLAN_ACTION_SET_EGRESS;
        lif_translation_info_get.gport = arp_ac_gport;

        lif_translation_info_get.new_outer_vlan = 0xFFFFFFFF;
        lif_translation_info_get.new_inner_vlan = 0xFFFFFFFF;
        lif_translation_info_get.vlan_edit_class_id = 0xFFFFFFFF;

        rv = bcm_vlan_port_translation_get(unit, &lif_translation_info_get);

        if (rv != BCM_E_NONE) {
            printf("%s(): Error, lif_translation_info_get return error = %d\n", proc_name, rv);
            return rv;
        }

        if (lif_translation_info_get.new_outer_vlan != vlan) {
            printf("%s(): Error, get.new_outer_vlan = %d  set.new_outer_vlan = %d\n", proc_name,
                   lif_translation_info_get.new_outer_vlan, vlan);

            return BCM_E_INTERNAL;
        }

        if (lif_translation_info_get.new_inner_vlan != inner_vlan) {
            printf("%s(): Error, get.new_inner_vlan = %d  set.new_inner_vlan = %d\n", proc_name,
                   lif_translation_info_get.new_inner_vlan, inner_vlan);

            return BCM_E_INTERNAL;
        }

        if (lif_translation_info_get.vlan_edit_class_id != vlan_edit_profile) {
            printf("%s(): Error, get.vlan_edit_class_id = %d  set.vlan_edit_class_id = %d\n", proc_name,
                   lif_translation_info_get.vlan_edit_class_id, vlan_edit_profile);

            return BCM_E_INTERNAL;
        }
    }

    printf("%s(): Exit\r\n",proc_name);
    return rv;
}

/******* Run example ******/

/*
 * packet will be routed from in_port to out-port
 *
 * Route:
 * packet to send:
 *  - in port = in_port
 *  - DA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}
 *  - DIP = 0x7fffff00-0x7fffff0f except 0x7fffff03
 * expected:
 *  - out port = out_port
 *  - vlan = 100.
 *  - DA = {0x20, 0x00, 0x00, 0x00, 0xcd, 0x1d}
 *  - SA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x01}
 *  TTL decremented
 */
/*
 * host_format options:
 * host_format = 0 : FEC
 * host_format = 1 : FEC + OUTLIF
 * host_format = 2 : System-Port, OutRIF, ARP (No-FEC)
 *                   FEC (destination), OutRIF, ARP
 * host_format = 3 : Only System-Port from FEC - NO RIF
 */
/*
 * protection info options:
 * failover_id - FEC protection pointer (0 means disable protection)
 * failover_out_port - primary protection port (relevant when failover_id != 0)
 *
 * If 'do_raw' is non-zero then 'fec' is used as is (raw) within 'add_route_ipv4()'
 */
/*
 * example:
 int unit = 0;
 int in_port = 200;
 int out_port = 201;
 int in_qos_map_id = 7;
 */
int
dnx_basic_example_inner(
    int unit,
    int in_port,
    int out_port,
    int host_format,
    int arp_plus_ac_type,
    bcm_failover_t failover_id,
    int failover_out_port,
    int do_raw,
    int kaps_result)
{
    int rv;
    int intf_in = 15;           /* Incoming packet ETH-RIF */
    int intf_out = 100;         /* Outgoing packet ETH-RIF */
    /*
     * Set 'fec' as on the coresponding test. See, for example,
     * AT_Dnx_Cint_l3_ip_route_basic_raw
     */
    int fec = kaps_result;
    int vrf = 1;
    int encap_id = 8193;         /* ARP-Link-layer (needs to be higher than 8192 for Jer Plus) */
    bcm_mac_t intf_in_mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };     /* my-MAC */
    bcm_mac_t intf_out_mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x01 };    /* my-MAC */
    bcm_mac_t arp_next_hop_mac = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d };        /* next_hop_mac */
    bcm_gport_t gport;
    uint32 route = 0x7fffff00;
    uint32 host = 0x7fffff02;
    uint32 mask = 0xfffffff0;
    l3_ingress_intf_init(&ingress_rif);
    uint32 flags2 = 0;
    int vlan = 100;
    int failover_is_primary = 0;
    /*
     * used to pass fec in host.l3a_intf
     */
    int encoded_fec;

    char *err_proc_name;
    char *proc_name;

    proc_name = "dnx_basic_example_inner";
    printf("%s(): Enter. in_port %d out_port %d do_raw %d host_format %d kaps_result 0x%08X\r\n",proc_name, in_port, out_port, do_raw, host_format,kaps_result);
    if (is_device_or_above(unit, JERICHO2))
    {
        /*
         * encap id for jr2 must be > 2k
         */
        encap_id = *dnxc_data_get(unit, "l3", "rif", "nof_rifs", NULL) + 0x384;

        /*
         * Jr2 myMac assignment is more flexible than in Jer1
         */
        intf_out_mac_address[0] = 0x00;
        intf_out_mac_address[1] = 0x12;
        intf_out_mac_address[2] = 0x34;
        intf_out_mac_address[3] = 0x56;
        intf_out_mac_address[4] = 0x78;
        intf_out_mac_address[5] = 0x9a;
    }

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, in_port, intf_in);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set intf_in\n",proc_name);
        return rv;
    }
    /*
     * 2. Set Out-Port default properties, in case of ARP+AC no need
     */
    rv = out_port_set(unit, out_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set intf_out out_port %d\n",proc_name,out_port);
        return rv;
    }
    if (failover_id != 0)
    {
        rv = out_port_set(unit, failover_out_port);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, out_port_set failover_out_port %d\n",proc_name, failover_out_port);
            return rv;
        }
    }
    /*
     * 3. Create ETH-RIF and set its properties
     */
    rv = intf_eth_rif_create(unit, intf_in, intf_in_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in %d\n",proc_name,intf_in);
        return rv;
    }
    rv = intf_eth_rif_create(unit, intf_out, intf_out_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
        return rv;
    }
    /*
     * 4. Set Incoming ETH-RIF properties
     */
    ingress_rif.vrf = vrf;
    ingress_rif.intf_id = intf_in;
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
        return rv;
    }

    if (arp_plus_ac_type > 2)
    {
        /*
         * 5. Create ARP and AC, connect them set its properties
         */
        rv = create_arp_with_next_ac_type(unit, arp_next_hop_mac, &encap_id, vlan);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, create_arp_with_next_ac_type\n",proc_name);
            return rv;
        }
    }
    else
    {
        /*
         * 5. Create ARP and set its properties
         */
        rv = create_arp_plus_ac_type(unit, arp_plus_ac_type, arp_next_hop_mac, &encap_id, vlan);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, create_arp_plus_ac_type\n",proc_name);
            return rv;
        }
    }

    /*
     * 6. Create FEC and set its properties
     * only in case the host format is not "no-fec"
     */
    if (host_format != 2)
    {
        printf("%s(): host_format %d. Create main FEC and set its properties.\r\n",proc_name, host_format);
        if (BCM_GPORT_IS_SET(out_port)) {
           gport = out_port;
        }
        else {
           BCM_GPORT_LOCAL_SET(gport, out_port);
        }
        rv = l3__egress_only_fec__create_inner(unit, fec, ((host_format == 1) || (host_format == 3) ) ? 0 : intf_out, encap_id, gport, 0,
                                               failover_id, failover_is_primary,&encoded_fec);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, create egress object FEC only\n",proc_name);
            return rv;
        }
        printf("%s(): fec 0x%08X encoded_fec 0x%08X.\r\n",proc_name, fec, encoded_fec);
        /*
         * if protection is enabled , also create another fec(primary)
         */
        if (failover_id != 0)
        {
            fec--;
            printf("%s(): host_format %d failover_id %d. Create main FEC and set its properties.\r\n",proc_name, host_format, failover_id);
            BCM_GPORT_LOCAL_SET(gport, failover_out_port);
            failover_is_primary = 1;
            rv = l3__egress_only_fec__create_inner(unit, fec, (host_format == 1) ? 0 : intf_out, encap_id, gport, 0,
                                                   failover_id, failover_is_primary,&encoded_fec);
            if (rv != BCM_E_NONE)
            {
                printf("%s(): Error, create egress object FEC only\n",proc_name);
                return rv;
            }
            printf("%s(): Failover, fec 0x%08X encoded_fec 0x%08X.\r\n",proc_name, fec, encoded_fec);
        }
        /*
         * 7. Add Route entry
         */
        printf("%s(): Add route entry. Raw %d. route 0x%08X mask 0x%08X vrf %d fec 0x%08X. \n",proc_name, do_raw, route, mask, vrf, fec);
        if (do_raw != 0)
        {
            rv = add_route_ipv4_raw(unit, route, mask, vrf, fec);
            err_proc_name = "add_route_ipv4_raw";
        }
        else
        {
            rv = add_route_ipv4(unit, route, mask, vrf, fec);
            err_proc_name = "add_route_ipv4";
        }
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in function %s(), \n",proc_name,err_proc_name);
            return rv;
        }
    }

    /*
     * 8. Add host entry
     */
    int _l3_itf;
    BCM_L3_ITF_SET(&_l3_itf, BCM_L3_ITF_TYPE_FEC, fec);
    if (BCM_GPORT_IS_SET(out_port)) {
       gport = out_port;
    }
    else {
       BCM_GPORT_LOCAL_SET(gport, out_port);
    }
    if (host_format == 1)
    {
        rv = add_host_ipv4(unit, host, vrf, _l3_itf, intf_out, 0);
    }
    else if (host_format == 2)
    {
        rv = add_host_ipv4(unit, host, vrf, intf_out, encap_id /* arp id */ , gport);
    }
    else
    {
        rv = add_host_ipv4(unit, host, vrf, _l3_itf, 0, 0);
    }
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function add_host_ipv4(), \n",proc_name);
        return rv;
    }

    printf("%s(): Exit\r\n",proc_name);
    return rv;
}

/*
 * Route packet arriving from in_port with the given sip and dip to out_port.
 * See dnx_basic_example_inner() for additional info
 */
int is_reflector_ipv6=0;
int
dnx_basic_example_for_reflector(
    int unit,
    int in_port,
    int out_port,
    uint32 sip,
    uint32 dip,
    int encap_id,
    int kaps_result)
{
    int rv;
    int intf_in = in_port + 100;           /* Incoming packet ETH-RIF */
    int intf_out = out_port + 200;         /* Outgoing packet ETH-RIF */

    bcm_mac_t my_mac_in = { 0x00, 0x0c, 0x00, 0x02, 0x00, in_port};     /* my-MAC */
    bcm_mac_t my_mac_out = { 0x00, 0x0c, 0x00, 0x02, 0x00, out_port};    /* my-MAC */
    bcm_mac_t arp_next_hop_mac = { 0x00, 0x00, 0x00, 0x00, 0xcd, out_port};        /* next_hop_mac */
    /*
     * Set 'fec' as on the corresponding test. See, for example,
     * AT_Dnx_Cint_l3_ip_route_basic_raw
     */
    int fec = kaps_result;
    int vrf = 1;
    bcm_gport_t gport;

    l3_ingress_intf ingress_rif;
    l3_ingress_intf_init(&ingress_rif);
    uint32 flags2 = 0;
    int vlan = 100;
    /*
     * used to pass fec in host.l3a_intf
     */
    int encoded_fec;

    char *err_proc_name;
    char *proc_name;

    proc_name = "dnx_basic_example_inner";
    /*
     * 1. Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, in_port, intf_in);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set intf_in\n",proc_name);
        return rv;
    }
    /*
     * 2. Set Out-Port default properties, in case of ARP+AC no need
     */
    rv = out_port_set(unit, out_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set intf_out out_port %d\n",proc_name,out_port);
        return rv;
    }

    /*
     * 3. Create ETH-RIF and set its properties
     */
    rv = intf_eth_rif_create(unit, intf_in, my_mac_in);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in %d\n",proc_name,intf_in);
        return rv;
    }
    rv = intf_eth_rif_create(unit, intf_out, my_mac_out);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
        return rv;
    }
    /*
     * 4. Set Incoming ETH-RIF properties
     */
    ingress_rif.vrf = vrf;
    ingress_rif.intf_id = intf_in;
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
        return rv;
    }
    /*
     * 5. Configure next-hop MAC
     */
    rv = l3__egress_only_encap__create_inner(unit, 0, &encap_id, arp_next_hop_mac, vlan, 0, flags2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object ARP only\n",proc_name);
        return rv;
    }
    /*
     * 6. Create FEC and set its properties
     * only in case the host format is not "no-fec"
     */
    BCM_GPORT_LOCAL_SET(gport, out_port);
    rv = l3__egress_only_fec__create_inner(unit, fec, intf_out, encap_id, gport, 0, 0, 0 ,&encoded_fec);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object FEC only\n",proc_name);
        return rv;
    }
    printf("%s(): fec 0x%08X encoded_fec 0x%08X.\r\n",proc_name, fec, encoded_fec);

    /*
     * 7. Add Route entry
     */
    if (!is_reflector_ipv6) {
        rv = add_route_ipv4(unit, dip, 0xffffffff, vrf, fec);
        err_proc_name = "add_route_ipv4";
    } else {
        rv = add_route_ipv6(unit, cint_ip_route_basic_ipv6_host, cint_ip_route_basic_ipv6_mask, vrf, fec);
        err_proc_name = "add_route_ipv6";
    }

    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function %s(), \n",proc_name,err_proc_name);
        return rv;
    }
    /*
     * 8. Add host entry
     */
    int _l3_itf;
    BCM_L3_ITF_SET(&_l3_itf, BCM_L3_ITF_TYPE_FEC, fec);
    if (!is_reflector_ipv6) {
        rv = add_host_ipv4(unit, sip, vrf, _l3_itf, 0, 0);
    } else {
        rv = add_host_ipv6(unit, cint_ip_route_basic_ipv6_host, vrf, fec);
    }

    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function add_host_ipv4(), \n",proc_name);
        return rv;
    }

    printf("%s(): Exit\r\n",proc_name);
    return rv;
}


/*
 * Packet will be routed from in_port to redir_port
 * This procedure is for JR2 only!
 *
 * Route:
 * packet to send:
 *  - in port = in_port
 *  - DA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}  (my_mac)
 *  - DIP range = 0x7fffff00-0x7fffff0f
 *  - DIP = 0x7FFFFF02
 * expected:
 *  - out port = redir_port
 *  - vlan = 100.
 *  - DA = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}
 *  - SA = {0x00, 0x12, 0x34, 0x56, 0x78, 0x9A}
 *  TTL decremented
 */
/*
 * host_format options:
 * host_format = 0 : FEC
 * host_format = 1 : FEC + OUTLIF
 * host_format = 2 : System-Port, OutRIF, ARP (No-FEC)
 *                   FEC (destination), OutRIF, ARP
 * host_format = 3 : Only System-Port from FEC - NO RIF
 *
 * This procedure supports only 'host_format = 0'
 */
/*
 * protection info options:
 * failover_id - FEC protection pointer (0 means disable protection)
 *
 * This procedure is for 'raw' FEC only. 'Fec' is used as is (raw) within 'add_route_ipv4()'
 * 'do_redirect' is non-zero.
 * 'redir_port' is another port to which the packet may be directed to and 'modid'
 * is the module id for all involved ports.
 * 'redir_encap_id' is the identifier of the allocated ARP-ID. Encap id for jr2 must be > 2k
 */
/*
 * example:
 int unit = 0;
 int in_port = 200;
 int out_port = 201;
 int redir_port = 203;
 */
int
dnx_basic_example_inner_redirect(
    int unit,
    int in_port,
    int out_port,
    int redir_port,
    int modid,
    int kaps_result,
    int redir_kaps_result,
    int redir_encap_id)
{
    int host_format;
    bcm_failover_t failover_id;
    int rv;
    int intf_in;          /* Incoming packet ETH-RIF */
    int intf_out;         /* Outgoing packet ETH-RIF */
    int fec;
    int vrf;
    int encap_id;         /* ARP-Link-layer */
    bcm_mac_t intf_in_mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };     /* my-MAC   */
    bcm_mac_t intf_out_mac_address = { 0x00, 0x12, 0x34, 0x56, 0x78, 0x9a };    /* dest-MAC */
    bcm_mac_t arp_next_hop_mac = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d };        /* next_hop_mac */
    bcm_gport_t gport;
    uint32 route;
    uint32 host;
    uint32 mask;
    l3_ingress_intf ingress_rif;
    uint32 flags2;
    int vlan;
    int failover_is_primary;
    /*
     * used to pass fec in host.l3a_intf
     */
    int encoded_fec;
    uint32 redir_host;
    int redir_encoded_fec;
    bcm_gport_t redir_gport;
    int redir_fec;
    char *err_proc_name;
    char *proc_name;

    l3_ingress_intf_init(&ingress_rif);

    flags2 = 0;
    vlan = 100;
    failover_is_primary = 0;
    intf_in = 15;           /* Incoming packet ETH-RIF */
    intf_out = 100;         /* Outgoing packet ETH-RIF */
    /*
     * Set 'fec' as on the coresponding test. See, for example,
     * AT_Dnx_Cint_l3_ip_route_basic_raw_redirect
     */
    fec = kaps_result;
    vrf = 1;
    route = 0x7FFFFF00;
    host  = 0x7FFFFF02;
    mask  = 0xFFFFFFF0;
    encap_id = 900;         /* ARP-Link-layer */
    failover_id = 0;
    host_format = 0;
    redir_host = host;
    redir_fec = redir_kaps_result;

    proc_name = "dnx_basic_example_inner_redirect";
    printf("%s(): Enter. in_port %d out_port %d redir_port %d modid %d\r\n",proc_name, in_port, out_port, redir_port, modid);
    printf("==> kaps_result 0x%08X redir_kaps_result 0x%08X redir_encap_id 0x%08X\r\n", kaps_result, redir_kaps_result, redir_encap_id);
    if (is_device_or_above(unit, JERICHO2))
    {
        /*
         * encap id for jr2 must be > 2k
         */
        encap_id = 0x1384;
    }
    else
    {
        rv = BCM_E_PARAM;
        printf("%s(): Error, this procedure is for JR2 only!\n",proc_name);
        return rv;
    }
    /*
     * 1. Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, in_port, intf_in);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set intf_in\n",proc_name);
        return rv;
    }
    /*
     * 2. Set Out-Port default properties, in case of ARP+AC no need
     */
    {
        /*
         * For original packet.
         */
        rv = out_port_set(unit, out_port);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, out_port_set intf_out out_port %d\n",proc_name,out_port);
            return rv;
        }
    }
    {
        /*
         * For redirected packet.
         */
        rv = out_port_set(unit, redir_port);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, out_port_set redir_port %d\n",proc_name, redir_port);
            return rv;
        }
    }
    /*
     * 3. Create ETH-RIF and set its properties
     */
    rv = intf_eth_rif_create(unit, intf_in, intf_in_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in %d\n",proc_name,intf_in);
        return rv;
    }
    rv = intf_eth_rif_create(unit, intf_out, intf_out_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
        return rv;
    }
    /*
     * 4. Set Incoming ETH-RIF properties
     */
    ingress_rif.vrf = vrf;
    ingress_rif.intf_id = intf_in;
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
        return rv;
    }
    /*
     * 5. Create FEC and set its properties
     */
    {
        {
            /*
             * This is for the original packet to be received.
             */
            printf("%s(): host_format %d. Create main FEC and set its properties.\r\n",proc_name, host_format);
            BCM_GPORT_LOCAL_SET(gport, out_port);
            rv = l3__egress_only_fec__create_inner(unit, fec, intf_out, encap_id, gport, 0,
                                               failover_id, failover_is_primary,&encoded_fec);
            if (rv != BCM_E_NONE)
            {
                printf("%s(): Error, create egress object FEC only\n",proc_name);
                return rv;
            }
            printf("%s(): fec 0x%08X encoded_fec 0x%08X.\r\n",proc_name, fec, encoded_fec);
        }
        {
            /*
             * This is for the redirected packet to be received.
             */
            printf("%s(): host_format %d. Create redir FEC and set its properties.\r\n",proc_name, host_format);
            BCM_GPORT_LOCAL_SET(redir_gport, redir_port);
            rv = l3__egress_only_fec__create_inner(unit, redir_fec, intf_out, redir_encap_id, redir_gport, 0,
                                                   failover_id, failover_is_primary,&redir_encoded_fec);
            if (rv != BCM_E_NONE)
            {
                printf("%s(): Error, create egress object FEC only\n",proc_name);
                return rv;
            }
            printf("%s(): redir_fec 0x%08X redir_encoded_fec 0x%08X.\r\n",proc_name, redir_fec, redir_encoded_fec);
        }
    }
    /*
     * 6. Create ARP and set its properties
     */
    {
        rv = l3__egress_only_encap__create_inner(unit, 0, &redir_encap_id, arp_next_hop_mac, vlan, 0, flags2);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, create egress object ARP only\n",proc_name);
            return rv;
        }
    }
    /*
     * 7. Add Route entry
     */
    {
        printf("%s(): Add route entry. route 0x%08X mask 0x%08X vrf %d fec 0x%08X. \n",proc_name, route, mask, vrf, fec);
        {
            rv = add_route_ipv4_raw(unit, route, mask, vrf, fec);
            err_proc_name = "add_route_ipv4_raw";
        }
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in function %s(), \n",proc_name,err_proc_name);
            return rv;
        }
    }
    /*
     * 8. Configure PMF to convert 'kaps_result' to 'redir_kaps_result'
     */
    {
        rv = dnx_kaps_raw_data_redirect(unit,kaps_result,1,redir_kaps_result,0/*redir_port*/);
        err_proc_name = "dnx_kaps_raw_data_redirect";
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in function %s(), \n",proc_name,err_proc_name);
            return rv;
        }
    }
    /*
     * 9. Add host entry
     */
    {
        rv = add_host_ipv4(unit, host, vrf, encoded_fec, 0, 0);

        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in function add_host_ipv4(), \n",proc_name);
            return rv;
        }
    }
    printf("%s(): Exit\r\n",proc_name);
    return rv;
}


int
dnx_basic_example(
    int unit,
    int in_port,
    int out_port,
    int kaps_result)
{
    int rv;
    char *proc_name;

    proc_name = "dnx_basic_example";
    printf("%s(): Enter. in_port %d out_port %d kaps_result 0x%08X\r\n",proc_name, in_port, out_port, kaps_result);
    if (already_called)
    {
        return 0;
    }
    already_called = 1;

    if(kaps_result == -1)
    {
        rv = get_default_fec_id(unit,&kaps_result);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in function get_default_fec_id(), \n",proc_name);
            return rv;
        }
    }

    rv = dnx_basic_example_inner(unit, in_port, out_port, 0, 0, 0, 0, 0, kaps_result);
    printf("%s(): Exit\r\n",proc_name);
    return rv;
}

int
   basic_example_inner_zeros(
    int unit,
    int in_port,
    int out_port,
    int host_format,
    int arp_plus_ac_type,
    bcm_failover_t failover_id,
    int failover_out_port,
    int kaps_result)
{
    int rv;
    char *proc_name;

    proc_name = "basic_example_inner_zeros";
    printf("%s(): Enter. in_port %d out_port %d host_format %d\r\n",proc_name, in_port, out_port, host_format);
    rv = dnx_basic_example_inner(unit, in_port, out_port, host_format, arp_plus_ac_type, failover_id, failover_out_port, 0, kaps_result);
    printf("%s(): Exit\r\n",proc_name);
    return rv;
}

int
dnx_basic_example_raw(
    int unit,
    int in_port,
    int out_port,
    int kaps_result)
{
    int rv;
    char *proc_name;

    proc_name = "dnx_basic_example_raw";
    printf("%s(): Enter. in_port %d out_port %d kaps_result 0x%08X\r\n",proc_name, in_port, out_port,  kaps_result);
    if (already_called)
    {
        return 0;
    }
    already_called = 1;
    rv = dnx_basic_example_inner(unit, in_port, out_port, 0, 0, 0, 0, 1, kaps_result);
    printf("%s(): Exit\r\n",proc_name);
    return rv;
}

/*
 * This is a function that would set the configuration for basic forwarding of an IPv6oETH packet.
 * Packet should match on the created IPv6 route entry and will be forwarded to the resulting FEC and
 * it will be routed from in_port to out-port.
 * If 'do_raw' is non-zero then 'fec' is loaded as is (as a raw value').
 *
 * Route:
 * packet to send:
 *  - in port = in_port
 *  - DA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}
 *  - DIP = 0000:0000:0000:0000:1234:0000:0000:FF10 to 0000:0000:0000:0000:1234:0000:0000:FF1F
 *          without 0000:0000:0000:0000:1234:0000:0000:FF13
 * expected:
 *  - out port = out_port
 *  - vlan = 100.
 *  - DA = {0x20, 0x00, 0x00, 0x00, 0xcd, 0x1d}
 *  - SA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x01}
 *  TTL decremented
 */
/*
 * example:
 int unit = 0;
 int in_port = 200;
 int out_port = 201;
 */
/*
 * The arp_plus_ac_type argument supports tagging:
 * 1 for single tagged packets, 2 for double tagged
 * 0 for no tagging
 */
int
basic_example_ipv6(
    int unit,
    int in_port,
    int out_port,
    int do_raw,
    int global_route,
    int arp_plus_ac_type)
{
    int rv;
    int intf_in = 15;           /* Incoming packet ETH-RIF */
    int intf_out = 100;         /* Outgoing packet ETH-RIF */
    int fec = ip_route_fec;
    int vrf = 1;
    int vsi = 0;
    int encap_id = *dnxc_data_get(unit, "l3", "rif", "nof_rifs", NULL) + 0x384;  /* ARP-Link-layer */
    bcm_gport_t gport = 0;
    int vlan = 100;
    bcm_mac_t intf_in_mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };     /* my-MAC */
    bcm_mac_t intf_out_mac_address = { 0x00, 0x12, 0x34, 0x56, 0x78, 0x9a };    /* my-MAC */
    bcm_mac_t arp_next_hop_mac = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d };        /* next_hop_mac */
    char *proc_name;
    char *err_proc_name;
    proc_name = "basic_example_ipv6";
    l3_ingress_intf ingress_itf;
    l3_ingress_intf_init(&ingress_itf);


    if (is_device_or_above(unit, JERICHO2))
    {
        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &fec);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }
        fec += 0x500;
    }

    ip_route_fec = fec;

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, in_port, intf_in);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set in_port = %d, intf_in = %d, err_id = %d\n", proc_name, in_port, intf_in,
               rv);
        return rv;
    }

    /*
     * 2. Set Out-Port default properties
     */
    rv = out_port_set(unit, out_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_intf_set out_port = %d, err_id = %d\n", proc_name, out_port, rv);
        return rv;
    }

    /*
     * 3. Create ETH-RIF and set its properties
     */
    rv = intf_eth_rif_create(unit, intf_in, intf_in_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in = %d, err_id = %d\n", proc_name, intf_in, rv);
        return rv;
    }
    rv = intf_eth_rif_create(unit, intf_out, intf_out_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out = %d, err_id = %d\n", proc_name, intf_out, rv);
        return rv;
    }

    /*
     * 4. Set Incoming ETH-RIF properties
     */
    ingress_itf.vrf = vrf;
    ingress_itf.intf_id = intf_in;
    if(global_route)
    {
        ingress_itf.flags = BCM_L3_INGRESS_GLOBAL_ROUTE;
    }
    rv = intf_ingress_rif_set(unit, &ingress_itf);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in = %d, vrf = %d, err_id = %d\n", proc_name, intf_in, vrf, rv);
        return rv;
    }

    /*
     * 5. Create FEC and set its properties
     */
    BCM_GPORT_LOCAL_SET(gport, out_port);
    rv = l3__egress_only_fec__create(unit, fec, intf_out, encap_id, gport, 0);
    if (rv != BCM_E_NONE)
    {
        printf
            ("%s(): Error, create egress object FEC only: fec = %d, intf_out = %d, encap_id = %d, out_port = %d, err_id = %d\n",
             proc_name, fec, intf_out, encap_id, out_port, rv);
        return rv;
    }
    /*
     * 6. Create ARP and set its properties
     */
    if(arp_plus_ac_type == 0)
    {
        rv = l3__egress_only_encap__create(unit, 0, encap_id, arp_next_hop_mac, vsi);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, create egress object ARP only: encap_id = %d, vsi = %d, err_id = %d\n", proc_name,
                   encap_id, vsi, rv);
            return rv;
        }
    }
    else
    {
        rv = create_arp_plus_ac_type(unit, arp_plus_ac_type, arp_next_hop_mac, &encap_id, vlan);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, create egress object ARP only: arp_plus_ac_type = %d, encap_id = %d, vlan = %d, err_id = %d\n",
                   proc_name, arp_plus_ac_type, encap_id, vlan, rv);
            return rv;
        }
    }

    /*
     * 7. Add Route entry
     */
    if (do_raw)
    {
        /*
         * If 'do_raw' is set then load 'fec' as is (raw value).
         */
        rv = add_route_ipv6_raw(unit, cint_ip_route_basic_ipv6_route, cint_ip_route_basic_ipv6_mask, vrf, fec);
        err_proc_name = "add_route_ipv6_raw";
    }
    else
    {
        rv = add_route_ipv6(unit, cint_ip_route_basic_ipv6_route, cint_ip_route_basic_ipv6_mask, vrf, fec);
        err_proc_name = "add_route_ipv6";
    }
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function %s: vrf = %d, fec = %d, err_id = %d \n", proc_name, err_proc_name, vrf, fec, rv);
        return rv;
    }

    /*
     * 7. Add host entry
     */
    rv = add_host_ipv6(unit, cint_ip_route_basic_ipv6_host, vrf, fec);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function add_host_ipv6: vrf = %d, fec = %d, err_id = %d\n", proc_name, vrf, fec, rv);
        return rv;
    }

    return rv;
}



int
get_fec_range(
    int unit,
    int hierarchy,
    int *start,
    int *nof_fecs)
{
    int rv;
    bcm_switch_fec_property_config_t fec_config;

    if(hierarchy == 0)
    {
        fec_config.flags = BCM_SWITCH_FEC_PROPERTY_1ST_HIERARCHY;
    }
    else if(hierarchy == 1)
    {
        fec_config.flags = BCM_SWITCH_FEC_PROPERTY_2ND_HIERARCHY;
    }
    else if(hierarchy == 2)
    {
        fec_config.flags = BCM_SWITCH_FEC_PROPERTY_3RD_HIERARCHY;
    }
    else
    {
        printf("Error, Unsupported hierarchy %d \n",hierarchy);
        return BCM_E_PARAM;
    }


    rv = bcm_switch_fec_property_get(unit,&fec_config);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_switch_fec_property_get\n");
        return rv;
    }

    *start = fec_config.start;
    *nof_fecs = fec_config.end - fec_config.start + 1;
    return rv;
}

int
get_first_fec_in_range(
    int unit,
    int hierarchy,
    int *fec_id)
{
    int rv;
    int nof_fecs;

    rv = get_fec_range(unit, hierarchy, fec_id, &nof_fecs);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range\n");
        return rv;
    }

    return rv;
}
/*
 * Get the first FEC ID of the given hierarchy which isn't in the ECMP range
 */
int
get_first_fec_in_range_which_not_in_ecmp_range(
    int unit,
    int hierarchy,
    int *fec_id)
{
    int rv;
    int nof_fecs;
    uint32 ecmp_range;

    if (!is_device_or_above(unit, JERICHO2)) {
        *fec_id=0x1000;
        return 0;
    }
    ecmp_range = *(dnxc_data_get(unit, "l3", "ecmp", "total_nof_ecmp", NULL));
    rv = get_fec_range(unit, hierarchy, fec_id, &nof_fecs);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range\n");
        return rv;
    }
    if(*fec_id < ecmp_range)
    {
        if(nof_fecs > ecmp_range)
        {
            *fec_id = ecmp_range;
        }
        else
        {
            printf("Error, can't find a FEC ID outside of the ECMP range\n");
            return BCM_E_NOT_FOUND;
        }
    }

    return rv;
}

/*
 * Get a default first hierarchy FEC
 */
int
get_default_fec_id(
    int unit,
    int *fec_id)
{
    int rv = BCM_E_NONE;
    if (is_device_or_above(unit, JERICHO2))
    {
        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, fec_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }
    }
    else
    {
        *fec_id = 0xA000;
    }

    return rv;
}

