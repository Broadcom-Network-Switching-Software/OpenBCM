/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
/*
 * Example of a simple-router scenario
 *
 * Test Scenario 1: Basic IPv4 lookup and forward
 *
 * Test Scenario - start
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_global.c
  cint ../../../../src/examples/dnx/l3/cint_dnx_ip_route_basic.c
 *
  cint
  dnx_basic_example(0, 13, 14, -1);
  exit;
 *
 * * ETH-RIF packet *
  tx 1 psrc=13 data=0x000c00020000000007000100080045000035000000008000fa45c08001017fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * * Received packets on unit 0 should be: *
 * * Data: 0x0000cd1d00123456789a81000000080045000035000000007f00fb45c08001017fffff02 *
 * Test Scenario - end
 * 
 * Test Scenario 2: Basic IPv6 lookup and forward
 *
 * Test Scenario - start
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_global.c
  cint ../../../../src/examples/dnx/l3/cint_dnx_ip_route_basic.c
 *
  cint
  dnx_basic_example_ipv6(0, 13, 14);
  exit;
 *
 * * ETH-RIF packet *
 *
  tx 1 psrc=13 data=000C0002000000010B0000008100000F86DD6030000000063BFFFE8000000000000002010BFFFE0000000000000000000000123400000000FF10000102030405FAB70642
 *
 * * Received packets on unit 0 should be: *
 *
 * Data:   00000000cd1d00123456789a8100306486dd6030000000063bfefe8000000000000002010bfffe0000000000000000000000123400000000ff10
 * Test Scenario - end
 *
 * DBAL Tables:
 * -----------
 *
 * dbal tbl dump tbl=IPV4_UNICAST_PRIVATE_LPM_FORWARD
 * dbal tbl dump tbl=IPV6_UNICAST_PRIVATE_LPM_FORWARD
 * dbal tbl dump tbl=IN_AC_INFO_DB
 * dbal tbl dump tbl=EEDB_ARP
 * dbal tbl dump tbl=EEDB_RIF_BASIC
 *
 * DIAG Commands:
 * --------------
 *
 * pp vis ppi core=1 - Packet being sent from IRPP 
 *
 * pp vis last core=1 - Packet being processed in different blocks of the IRPP
 *
 * pp vis ekleap core=1 - Packet being sent processed  in the ETPP
 *
 */

/*
 * invalid default ESEM, this value will be the default value for ESEM default entry ID, s.t. bcm_port_match_add
 *     will be called only if the test requires a match to be added
 */
int invalid_default_esem = -1;

int already_called = 0;
int default_vlan_port_id_out[4] = {invalid_default_esem, invalid_default_esem, invalid_default_esem, invalid_default_esem};;
int default_vlan_port_id_in[4] = {0, 0, 0, 0};;
int in_port_intf_num = 0;
bcm_gport_t in_port_intf[4] = {0, 0, 0, 0};

int ip_route_fec = 40961;
int ip_route_encap = 0x1384;
int add_ipv6_host_entry = 1;
int sbfd_reflector_ipv6_with_srv6_inner_mac = 0;

int mymac_ip_disabled_trap = 0;


int glob_intf_out = 0;
int glob_encap_id = 0;

struct cint_dnx_ip_route_basic_s {
    int intf_in;                       /** Ingress interface */
    int intf_out;                      /** Egress interface */
    bcm_mac_t intf_in_mac_address;     /* Ingress my-MAC */
    bcm_mac_t intf_out_mac_address;    /* Egress my-MAC */
    int force_vsi;                     /** Forced VSI ID for ARP */
    int is_arp_rif_mode;               /** ARP-RIF mode indication */
    uint8 svtag_enable;                /** Enable SVTAG for ARP+AC configuration*/
    uint8 host_format;                 /** host entry format */
    uint8 do_raw;                      /** if set, fec is loaded as a raw value */
    bcm_failover_t failover_id;        /** FEC protection pointer (0 means disable protection) */
    int failover_out_port;             /** primary protection port (relevant when failover_id != 0) */
};

cint_dnx_ip_route_basic_s cint_ip_route_info = {
    /** Ingress interface */
    15,
    /** Egress interface */
    100,
    /** Ingress my-MAC */
    { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 },
    /** Egress my-MAC */
    { 0x00, 0x12, 0x34, 0x56, 0x78, 0x9a },
    /** force VSI */
    0,
    /** is_arp_rif_mode */
    0,
    /** svtag_enable */
    0,
    /** host_format */
    0,
    /** do_raw */
    0,
    /** failover_id */
    0,
    /** failover_out_port */
    0
};

bcm_ip6_t cint_ip_route_basic_ipv6_route = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x10 };
bcm_ip6_t cint_ip_route_basic_ipv6_mask = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0 };
bcm_ip6_t cint_ip_route_basic_ipv6_host = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x13 };

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
    char *proc_name;

    proc_name = "in_port_intf_set";
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.port = in_port;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vlan_port.vsi = eth_rif;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;

    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, vlan_port), "");
    in_port_intf[in_port_intf_num % 4] = vlan_port.vlan_port_id;
    in_port_intf_num ++;
    default_vlan_port_id_in[unit] = vlan_port.vlan_port_id;

    printf("%s(): port = %d, in_lif = 0x%08X, in_port_intf_num = %d\n", proc_name, vlan_port.port, vlan_port.vlan_port_id, in_port_intf_num);

    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(unit, eth_rif, in_port, 0), "");

    return BCM_E_NONE;
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
    if (BCM_GPORT_IS_VLAN_PORT(default_vlan_port_id_out[unit]) && BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_DEFAULT(default_vlan_port_id_out[unit])) {
        bcm_vlan_port_t_init(&vlan_port);

        /* Find the vlan-port first to confirm it indeed exists already.*/
        vlan_port.vlan_port_id = default_vlan_port_id_out[unit];
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

    if ((default_vlan_port_id_out[unit] != invalid_default_esem) && (is_default_ac_existing == 0)) {
        bcm_vlan_port_t_init(&vlan_port);

        vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
        vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY;
        vlan_port.flags |= BCM_VLAN_PORT_DEFAULT;
        vlan_port.flags |= BCM_VLAN_PORT_VLAN_TRANSLATION;

        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), "");

        default_vlan_port_id_out[unit] = vlan_port.vlan_port_id;

        printf("%s(): bcm_vlan_port_create() result: port = %d, vlan_port_id = 0x%08X\r\n",proc_name, out_port, default_vlan_port_id_out[unit]);
    } else {
        printf("%s(): using the existing vlan_port: port = %d, vlan_port_id = 0x%08X\r\n",proc_name, out_port, default_vlan_port_id_out[unit]);
    }

    if(default_vlan_port_id_out[unit] != invalid_default_esem)
    {
        bcm_port_match_info_t_init(&match_info);
        match_info.match = BCM_PORT_MATCH_PORT;
        match_info.flags = BCM_PORT_MATCH_EGRESS_ONLY;
        match_info.port = out_port;
        BCM_IF_ERROR_RETURN_MSG(bcm_port_match_add(unit, default_vlan_port_id_out[unit], &match_info), "");
    }


    return BCM_E_NONE;
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

    bcm_l3_ingress_t_init(&l3_ing_if);
    l3_ing_if.flags = ingress_itf->flags | BCM_L3_INGRESS_WITH_ID;      /* must, as we update exist RIF */
    l3_ing_if.vrf = ingress_itf->vrf;
    l3_ing_if.qos_map_id = ingress_itf->qos_map;

    BCM_IF_ERROR_RETURN_MSG(bcm_l3_ingress_create(unit, l3_ing_if, ingress_itf->intf_id), "");

    return BCM_E_NONE;
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

    BCM_IF_ERROR_RETURN_MSG(bcm_l3_intf_create(unit, l3if), "");

    return BCM_E_NONE;
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

    BCM_IF_ERROR_RETURN_MSG(bcm_l3_intf_create(unit, &l3if), "");

    return BCM_E_NONE;
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

    BCM_IF_ERROR_RETURN_MSG(bcm_l3_intf_create(unit, l3if), "");

    return BCM_E_NONE;
}

/*
 * Create VSI ETH-RIF and set initial properties:
 * - my_mac - My-MAC address
 * - vrf - Virtual routing interface
 */
int
intf_eth_rif_create_vrf(
    int unit,
    int eth_rif_id,
    int vrf,
    bcm_mac_t my_mac)
{
    bcm_l3_intf_t l3if;

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
    l3if.l3a_vrf = vrf;
    BCM_IF_ERROR_RETURN_MSG(bcm_l3_intf_create(unit, l3if), "");

    return BCM_E_NONE;
}

/*
 * Add an IPv4 Route entry <vrf, addr, mask> --> intf
 * - addr: IP address 32 bit value
 * - mask 1: to consider 0: to ignore, for example for /24 mask = 0xffffff00 will mask the 24 MSBs of the DIP
 * - vrf: VRF ID
 * - intf: egress object created using create_l3_egress (FEC)
 *
 */
int
add_route_ipv4(
    int unit,
    uint32 addr,
    uint32 mask,
    int vrf,
    int intf)
{
    bcm_l3_route_t l3rt;
    bcm_l3_route_t_init(l3rt);

    l3rt.l3a_subnet = addr;
    l3rt.l3a_ip_mask = mask;
    l3rt.l3a_vrf = vrf;
    l3rt.l3a_intf = intf;       /* FEC-ID */

    if (is_jericho2_kbp_ipv4_split_rpf_enabled(unit) || (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL)))
    {
        l3rt.l3a_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_l3_route_add(unit, l3rt), "");
    return BCM_E_NONE;
}

int
add_route_ipv4_encap_dest(
    int unit,
    uint32 addr,
    uint32 mask,
    int vrf,
    int intf,
    int encap_id,
    int destination)
{
    int rc;
    bcm_l3_route_t l3rt;
    bcm_l3_route_t_init(l3rt);

    l3rt.l3a_subnet = addr;
    l3rt.l3a_ip_mask = mask;
    l3rt.l3a_vrf = vrf;
    if (is_jericho2_kbp_ipv4_split_rpf_enabled(unit) || (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL)))
    {
        l3rt.l3a_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }

    if (BCM_GPORT_IS_FORWARD_PORT(destination) && (intf != 0 || encap_id != 0))
    {
        if (intf != 0)
        {
            l3rt.l3a_intf = intf;
        }
        else
        {
            l3rt.l3a_intf = encap_id;
        }

        l3rt.l3a_port_tgid = destination;
    }
    else if(BCM_GPORT_IS_FORWARD_PORT(destination))
    {
        BCM_GPORT_FORWARD_GROUP_TO_L3_ITF_FEC(l3rt.l3a_intf, destination);
    }
    else if (intf != 0)
    {
        l3rt.l3a_intf = intf;
    }
    else
    {
        l3rt.l3a_intf = encap_id;
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
 *
 */
int
add_route_ipv4_raw(
    int unit,
    uint32 addr,
    uint32 mask,
    int vrf,
    int intf)
{
    bcm_l3_route_t l3rt;
    bcm_l3_route_t_init(l3rt);
    l3rt.l3a_subnet = addr;
    l3rt.l3a_ip_mask = mask;
    l3rt.l3a_vrf = vrf;
    l3rt.l3a_intf = intf;       /* Raw value: FEC-ID */
    l3rt.l3a_flags2 |= BCM_L3_FLAGS2_RAW_ENTRY;
    if (is_jericho2_kbp_ipv4_split_rpf_enabled(unit) || (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL)))
    {
        l3rt.l3a_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_l3_route_add(unit, l3rt), "");
    return BCM_E_NONE;
}

/*
 * Add an IPv4 Route entry <vrf, addr, mask> --> intf, as a raw value
 * - addr: IP address 32 bit value
 * - mask 1: to consider 0: to ingore, for example for /24 mask = 0xffffff00 will mask the 24 MSBs of the DIP
 * - vrf: VRF ID
 * - intf: Raw value which is, actually, egress object created using create_l3_egress (FEC)
 *
 */
int
add_route_ipv4_raw_rpf(
    int unit,
    uint32 addr,
    uint32 mask,
    int vrf,
    int intf)
{
    bcm_l3_route_t l3rt;
    bcm_l3_route_t_init(l3rt);
    l3rt.l3a_subnet = addr;
    l3rt.l3a_ip_mask = mask;
    l3rt.l3a_vrf = vrf;
    l3rt.l3a_intf = intf;       /* Raw value: FEC-ID */
    l3rt.l3a_flags2 |= BCM_L3_FLAGS2_RAW_ENTRY;
    if (is_jericho2_kbp_ipv4_split_rpf_enabled(unit) || (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL)))
    {
        l3rt.l3a_flags2 |= BCM_L3_FLAGS2_RPF_ONLY;
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_l3_route_add(unit, l3rt), "");
    return BCM_E_NONE;
}

/*
 * Add an IPv6 Route entry using <vrf, addr, mask> that will result to an intf.
 * - addr: IPv6 address 16 8-bit values
 * - mask: 1: to consider, 0: to ingore, for example for /64 mask = ffff:ffff:ffff:ffff:0000:0000:0000:0000
 *              masks the 64 MSB of the IPv6 DIP which will be considered.
 * - vrf: VRF ID
 * - intf: egress object created using create_l3_egress (FEC)
 *
 */
int
add_route_ipv6(
    int unit,
    bcm_ip6_t addr,
    bcm_ip6_t mask,
    int vrf,
    int intf)
{
    bcm_l3_route_t l3rt;
    bcm_l3_route_t_init(l3rt);

    l3rt.l3a_ip6_net = addr;
    l3rt.l3a_ip6_mask = mask;
    l3rt.l3a_vrf = vrf;
    l3rt.l3a_intf = intf;       /* FEC-ID */
    l3rt.l3a_flags = BCM_L3_IP6;
    if (is_jericho2_kbp_ipv6_split_rpf_enabled(unit) || (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL)))
    {
        l3rt.l3a_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }

    printf("Entering bcm_l3_route_add \n");
    BCM_IF_ERROR_RETURN_MSG(bcm_l3_route_add(unit, l3rt), "");
    return BCM_E_NONE;
}

int
add_route_ipv6_encap_dest(
    int unit,
    bcm_ip6_t addr,
    bcm_ip6_t mask,
    int vrf,
    int intf,
    int encap_id,
    int destination)
{
    int rc;
    bcm_l3_route_t l3rt;
    bcm_l3_route_t_init(l3rt);

    l3rt.l3a_ip6_net = addr;
    l3rt.l3a_ip6_mask = mask;
    l3rt.l3a_vrf = vrf;
    l3rt.l3a_flags = BCM_L3_IP6;
    if (is_jericho2_kbp_ipv6_split_rpf_enabled(unit) || (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL)))
    {
        l3rt.l3a_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }

    if (BCM_GPORT_IS_FORWARD_PORT(destination) && (intf != 0 || encap_id != 0))
    {
        if (intf != 0)
        {
            l3rt.l3a_intf = intf;
        }
        else
        {
            l3rt.l3a_intf = encap_id;
        }

        l3rt.l3a_port_tgid = destination;
    }
    else if(BCM_GPORT_IS_FORWARD_PORT(destination))
    {
        BCM_GPORT_FORWARD_GROUP_TO_L3_ITF_FEC(l3rt.l3a_intf, destination);
    }
    else if (intf != 0)
    {
        l3rt.l3a_intf = intf;
    }
    else
    {
        l3rt.l3a_intf = encap_id;
    }

    printf("Entering bcm_l3_route_add \n", rc);
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
 *
 */
int
add_route_ipv6_rpf(
    int unit,
    bcm_ip6_t addr,
    bcm_ip6_t mask,
    int vrf,
    int intf)
{
    bcm_l3_route_t l3rt;
    bcm_l3_route_t_init(l3rt);

    l3rt.l3a_ip6_net = addr;
    l3rt.l3a_ip6_mask = mask;
    l3rt.l3a_vrf = vrf;
    l3rt.l3a_intf = intf;       /* FEC-ID */
    l3rt.l3a_flags = BCM_L3_IP6;
    if (is_jericho2_kbp_ipv6_split_rpf_enabled(unit) || (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL)))
    {
        l3rt.l3a_flags2 |= BCM_L3_FLAGS2_RPF_ONLY;

        if (intf == 0)
        {
            l3rt.l3a_flags2 |= BCM_L3_FLAGS2_NO_PAYLOAD;
        }
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_l3_route_add(unit, l3rt), "");
    return BCM_E_NONE;
}

/*
 * Add an IPv6 Route entry using <vrf, addr, mask> that will result to an intf.
 * - addr: IPv6 address 16 8-bit values
 * - mask: 1: to consider, 0: to ingore, for example for /64 mask = ffff:ffff:ffff:ffff:0000:0000:0000:0000
 *              masks the 64 MSB of the IPv6 DIP which will be considered.
 * - vrf: VRF ID
 * - intf: Raw value of FEC (egress object created using create_l3_egress)
 *
 */
int
add_route_ipv6_raw(
    int unit,
    bcm_ip6_t addr,
    bcm_ip6_t mask,
    int vrf,
    int intf)
{
    bcm_l3_route_t l3rt;
    bcm_l3_route_t_init(l3rt);
    l3rt.l3a_ip6_net = addr;
    l3rt.l3a_ip6_mask = mask;
    l3rt.l3a_vrf = vrf;
    l3rt.l3a_intf = intf;       /* Raw value of FEC-ID */
    l3rt.l3a_flags |= BCM_L3_IP6;
    l3rt.l3a_flags2 |= BCM_L3_FLAGS2_RAW_ENTRY;
    if (is_jericho2_kbp_ipv6_split_rpf_enabled(unit) || (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL)))
    {
        l3rt.l3a_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_l3_route_add(unit, l3rt), "");
    return BCM_E_NONE;
}

/*
 * Add an IPv6 Route entry using <vrf, addr, mask> that will result to an intf.
 * - addr: IPv6 address 16 8-bit values
 * - mask: 1: to consider, 0: to ingore, for example for /64 mask = ffff:ffff:ffff:ffff:0000:0000:0000:0000
 *              masks the 64 MSB of the IPv6 DIP which will be considered.
 * - vrf: VRF ID
 * - intf: Raw value of FEC (egress object created using create_l3_egress)
 *
 */
int
add_route_ipv6_raw_rpf(
    int unit,
    bcm_ip6_t addr,
    bcm_ip6_t mask,
    int vrf,
    int intf)
{
    bcm_l3_route_t l3rt;
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
    BCM_IF_ERROR_RETURN_MSG(bcm_l3_route_add(unit, l3rt), "");
    return BCM_E_NONE;
}

/*
 * Add an IPv4 Route entry <vrf, addr, mask> --> intf
 * - addr: IP address 32 bit value
 * - mask 1: to consider 0: to ignore, for example for /24 mask = 0xffffff00 will mask the 24 MSBs of the DIP
 * - vrf: VRF ID
 * - intf: egress object created using create_l3_egress (FEC)
 *
 */
int
add_route_ipv4_rpf(
    int unit,
    uint32 addr,
    uint32 mask,
    int vrf,
    int intf)
{
    bcm_l3_route_t l3rt;
    bcm_l3_route_t_init(l3rt);

    l3rt.l3a_subnet = addr;
    l3rt.l3a_ip_mask = mask;
    l3rt.l3a_vrf = vrf;
    l3rt.l3a_intf = intf;       /* FEC-ID */

    if (is_jericho2_kbp_ipv4_split_rpf_enabled(unit) || (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL)))
    {
        l3rt.l3a_flags2 |= BCM_L3_FLAGS2_RPF_ONLY;

        if (intf == 0)
        {
            l3rt.l3a_flags2 |= BCM_L3_FLAGS2_NO_PAYLOAD;
        }
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_l3_route_add(unit, l3rt), "");
    return BCM_E_NONE;
}

/*
 * Route packet arriving from in_port with the given sip and dip to out_port.
 * See dnx_basic_example_inner() for additional info
 */
int is_reflector_ipv6=0;
int is_config_fwd_for_in_port=0;
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
    char error_msg[200]={0,};
    int intf_in, intf_out;
    bcm_mac_t my_mac_in = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};    /* my-MAC */
    bcm_mac_t my_mac_out = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};    /* my-MAC */
    bcm_mac_t arp_next_hop_mac = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x00};        /* next_hop_mac */

    if (BCM_GPORT_IS_TRUNK(in_port)) {
        intf_in = (in_port & 0xFF) + 100;           /* Incoming packet ETH-RIF */
        my_mac_in[5] = in_port & 0xFF;     /* my-MAC */
    } else {
        intf_in = in_port + 100;           /* Incoming packet ETH-RIF */
        my_mac_in[5] = in_port;     /* my-MAC */
    }

    if (BCM_GPORT_IS_TRUNK(out_port)) {
        intf_out = (out_port & 0xFF) + 200;         /* Outgoing packet ETH-RIF */
        my_mac_out[5] = out_port & 0xFF;    /* my-MAC */
        arp_next_hop_mac[5] = out_port & 0xFF;        /* next_hop_mac */

    } else {
        intf_out = out_port + 200;         /* Outgoing packet ETH-RIF */
        my_mac_out[5] = out_port;    /* my-MAC */
        arp_next_hop_mac[5] = out_port;        /* next_hop_mac */
    }

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

    char *proc_name;

    proc_name = "dnx_basic_example_inner";
    /*
     * 1. Set In-Port to In ETh-RIF
     */
    if (!is_config_fwd_for_in_port)
    {
        BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, in_port, intf_in), "");
    }
    /*
     * 2. Set Out-Port default properties, in case of ARP+AC no need
     */
    snprintf(error_msg, sizeof(error_msg), "intf_out out_port %d", out_port);
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, out_port), error_msg);

    /*
     * 3. Create ETH-RIF and set its properties
     */
    if (!is_config_fwd_for_in_port)
    {
        snprintf(error_msg, sizeof(error_msg), "intf_in %d", intf_in);
        BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, intf_in, my_mac_in), error_msg);
    }
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, intf_out, my_mac_out), "intf_out");
    /*
     * 4. Set Incoming ETH-RIF properties
     */
    ingress_rif.vrf = vrf;
    ingress_rif.intf_id = intf_in;
    if (!is_config_fwd_for_in_port)
    {
        BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_rif), "intf_out");
        is_config_fwd_for_in_port = 1;
    }
    /*
     * 5. Configure next-hop MAC
     */
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create_inner(unit, 0, &encap_id, arp_next_hop_mac, vlan, 0, flags2),
        "create egress object ARP only");
    /*
     * 6. Create FEC and set its properties
     * only in case the host format is not "no-fec"
     */
    int fec_flags2 = 0;
    if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        fec_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }

    if (BCM_GPORT_IS_TRUNK(out_port)) {
        gport = out_port;
    } else {
        BCM_GPORT_LOCAL_SET(gport, out_port);
    }
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create_inner(unit, fec, intf_out, encap_id, gport, 0, fec_flags2, 0, 0, &encoded_fec),
        "create egress object FEC only");
    printf("%s(): fec 0x%08X encoded_fec 0x%08X.\r\n",proc_name, fec, encoded_fec);

    /*
     * 7. Add Route entry
     */
    if (!is_reflector_ipv6) {
        BCM_IF_ERROR_RETURN_MSG(add_route_ipv4(unit, dip, 0xffffffff, vrf, fec), "");
    } else {
        BCM_IF_ERROR_RETURN_MSG(add_route_ipv6(unit, cint_ip_route_basic_ipv6_host, cint_ip_route_basic_ipv6_mask, vrf, fec), "");
    }

    /*
     * 8. Add host entry
     */
    int _l3_itf;
    BCM_L3_ITF_SET(&_l3_itf, BCM_L3_ITF_TYPE_FEC, fec);
    if(*dnxc_data_get(unit, "l3", "fwd", "host_entry_support", NULL))
    {
        if (!is_reflector_ipv6) {
            BCM_IF_ERROR_RETURN_MSG(add_host_ipv4(unit, sip, vrf, _l3_itf, 0, 0), "");
        } else {
            BCM_IF_ERROR_RETURN_MSG(add_host_ipv6(unit, cint_ip_route_basic_ipv6_host, vrf, fec), "");
        }

    }
    return BCM_E_NONE;
}


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
 *
 */
/*
 * example:
 int unit = 0;
 int in_port = 200;
 int out_port = 201;
 */
int
dnx_basic_example_inner(
    int unit,
    int in_port,
    int out_port,
    int arp_plus_ac_type,
    int kaps_result)
{
    char error_msg[200]={0,};
    int intf_in = cint_ip_route_info.intf_in;           /* Incoming packet ETH-RIF */
    int intf_out = cint_ip_route_info.intf_out;         /* Outgoing packet ETH-RIF */
    /*
     * Set 'fec' as on the corresponding test. See, for example,
     * AT_Dnx_Cint_l3_ip_route_basic_raw
     */
    int fec = kaps_result;
    int vrf = 1;
    int encap_id = *dnxc_data_get(unit, "l3", "rif", "nof_rifs", NULL) + 0x384;
    bcm_mac_t intf_in_mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };     /* my-MAC 00:0c:00:02:00:00*/
    bcm_mac_t intf_out_mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x01 };    /* my-MAC */
    bcm_mac_t arp_next_hop_mac = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d };        /* next_hop_mac */
    bcm_gport_t gport;
    uint32 route = 0x7fffff00;
    uint32 host = 0x7fffff02;
    uint32 mask = 0xfffffff0;
    l3_ingress_intf_init(&ingress_rif);
    int fec_flags2 = 0;
    int vlan = cint_ip_route_info.intf_out;
    int failover_is_primary = 0;
    /*
     * used to pass fec in host.l3a_intf
     */
    int encoded_fec;

    char *proc_name;

    proc_name = "dnx_basic_example";
    printf("%s(): Enter. in_port %d out_port %d do_raw %d kaps_result 0x%08X\r\n", proc_name, in_port, out_port,cint_ip_route_info.do_raw, kaps_result);

    intf_out_mac_address[0] = 0x00;
    intf_out_mac_address[1] = 0x12;
    intf_out_mac_address[2] = 0x34;
    intf_out_mac_address[3] = 0x56;
    intf_out_mac_address[4] = 0x78;
    intf_out_mac_address[5] = 0x9a;

    printf("encap_id 0x%08X\r\n", encap_id);

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, in_port, intf_in), "intf_in");

    /*
     * 2. Set Out-Port default properties, in case of ARP+AC no need
     */
    snprintf(error_msg, sizeof(error_msg), "out_port %d", out_port);
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, out_port), error_msg);

    if (cint_ip_route_info.failover_id != 0)
    {
        snprintf(error_msg, sizeof(error_msg), "%d", cint_ip_route_info.failover_out_port);
        BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, cint_ip_route_info.failover_out_port), error_msg);
    }

    /*
     * 3. Create ETH-RIF and set its properties.
     */
    snprintf(error_msg, sizeof(error_msg), "intf_in %d", intf_in);
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create_vrf(unit, intf_in, 0, intf_in_mac_address), error_msg);

    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create_vrf(unit, intf_out, 0, intf_out_mac_address), "intf_out");

    /*
     * 4. Set Incoming ETH-RIF properties
     */
    ingress_rif.vrf = vrf;
    ingress_rif.intf_id = intf_in;
    if (mymac_ip_disabled_trap) {
        ingress_rif.flags = BCM_L3_INGRESS_ROUTE_DISABLE_IP4_UCAST|BCM_L3_INGRESS_ROUTE_DISABLE_IP6_UCAST|BCM_L3_INGRESS_ROUTE_DISABLE_MPLS;
    }

    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_rif), "");

    if (arp_plus_ac_type > 2)
    {
        /*
         * 5. Create ARP and AC, connect them set its properties
         */
        BCM_IF_ERROR_RETURN_MSG(create_arp_with_next_ac_type(unit, arp_next_hop_mac, &encap_id, vlan), "");
    }
    else
    {
        /*
         * 5. Create ARP and set its properties
         */
        BCM_IF_ERROR_RETURN_MSG(create_arp_plus_ac_type(unit, arp_plus_ac_type, arp_next_hop_mac, &encap_id, vlan), "");
    }
    glob_encap_id = encap_id;

    /*
     * 6. Create FEC and set its properties
          only in case the host format is not "no-fec"
     */
    if (cint_ip_route_info.host_format != 2)
    {
        printf("%s():. Create FEC and set its properties.\r\n", proc_name);
        if (BCM_GPORT_IS_SET(out_port))
        {
           gport = out_port;
        }
        else
        {
           BCM_GPORT_LOCAL_SET(gport, out_port);
        }

        if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
        {
            fec_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
        }

        BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create_inner(unit, fec, (cint_ip_route_info.is_arp_rif_mode ? 0 : intf_out), encap_id,
            gport, 0, fec_flags2, cint_ip_route_info.failover_id, failover_is_primary, &encoded_fec), "create egress object FEC only");

        printf("%s(): fec 0x%08X encoded_fec 0x%08X.\r\n", proc_name, fec, encoded_fec);

        if (cint_ip_route_info.failover_id != 0)
        {
            fec--;
            BCM_GPORT_LOCAL_SET(gport, cint_ip_route_info.failover_out_port);
            failover_is_primary = 1;
            BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create_inner(unit, fec, (cint_ip_route_info.is_arp_rif_mode ? 0 : intf_out), encap_id, 
                gport, 0, fec_flags2, cint_ip_route_info.failover_id, failover_is_primary, &encoded_fec), "create egress object FEC only");
            printf("%s(): Primary fec 0x%08X encoded_fec 0x%08X.\r\n",proc_name, fec, encoded_fec);
        }

        /*
         * 7. Add Route entry
         */
        printf("%s(): Add route entry.Raw %d. route 0x%08X mask 0x%08X vrf %d fec 0x%08X. \n", proc_name, cint_ip_route_info.do_raw, route, mask, vrf, fec);
        if (cint_ip_route_info.do_raw)
        {
            BCM_IF_ERROR_RETURN_MSG(add_route_ipv4_raw(unit, route, mask, vrf, fec), "");
        }
        else
        {
            BCM_IF_ERROR_RETURN_MSG(add_route_ipv4(unit, route, mask, vrf, fec), "");
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
    else
    {
       BCM_GPORT_LOCAL_SET(gport, out_port);
    }
    if (cint_ip_route_info.host_format == 1)
    {
        /* FEC(no RIF) + OUTRIF, cint_ip_route_info.is_arp_rif_mode=1 */
        BCM_IF_ERROR_RETURN_MSG(add_host_ipv4(unit, host, vrf, _l3_itf, intf_out, 0), "");
    }
    else if (cint_ip_route_info.host_format == 2)
    {
        /* System-Port, OutRIF, ARP (No-FEC) */
        BCM_IF_ERROR_RETURN_MSG(add_host_ipv4(unit, host, vrf, intf_out, encap_id /* arp id */ , gport), "");
    }
    else
    {
        /* Only System-Port from FEC(no RIF)*/
        BCM_IF_ERROR_RETURN_MSG( add_host_ipv4(unit, host, vrf, _l3_itf, 0, 0), "");
    }

    printf("%s(): Exit\r\n",proc_name);
    return BCM_E_NONE;
}


int
dnx_basic_example(
    int unit,
    int in_port,
    int out_port,
    int kaps_result)
{
    char *proc_name;

    proc_name = "dnx_basic_example";
    printf("%s(): Enter. in_port %d out_port %d kaps_result 0x%08X\r\n",proc_name, in_port, out_port, kaps_result);
    if (already_called)
    {
        return BCM_E_NONE;
    }
    already_called = 1;

    if(kaps_result == -1)
    {
        BCM_IF_ERROR_RETURN_MSG(get_default_fec_id(unit,&kaps_result), "");
    }

    BCM_IF_ERROR_RETURN_MSG(dnx_basic_example_inner(unit, in_port, out_port, 0, kaps_result), "");
    printf("%s(): Exit\r\n",proc_name);
    return BCM_E_NONE;
}

int
dnx_basic_example_raw(
    int unit,
    int in_port,
    int out_port,
    int kaps_result)
{
    char *proc_name;

    proc_name = "dnx_basic_example_raw";
    printf("%s(): Enter. in_port %d out_port %d kaps_result 0x%08X\r\n",proc_name, in_port, out_port,  kaps_result);
    if (already_called)
    {
        return BCM_E_NONE;
    }
    already_called = 1;
    BCM_IF_ERROR_RETURN_MSG(dnx_basic_example_inner(unit, in_port, out_port, 0, kaps_result), "");
    printf("%s(): Exit\r\n",proc_name);
    return BCM_E_NONE;
}

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
 *
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
dnx_basic_example_ipv4_inner(
    int unit,
    int in_port,
    int out_port,
    int kaps_result)
{
    char error_msg[200]={0,};
    int intf_in = cint_ip_route_info.intf_in;           /* Incoming packet ETH-RIF */
    int intf_out = cint_ip_route_info.intf_out;         /* Outgoing packet ETH-RIF */
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
    int vlan = cint_ip_route_info.intf_out;
    int failover_is_primary = 0;
    /*
     * used to pass fec in host.l3a_intf
     */
    int encoded_fec;

    char *proc_name;

    proc_name = "dnx_basic_example_ipv4_inner";

    /*
     * encap id for jr2 must be > 2k
     */
    encap_id = *dnxc_data_get(unit, "l3", "rif", "nof_rifs", NULL) + 0x384;

    intf_out_mac_address[0] = 0x00;
    intf_out_mac_address[1] = 0x12;
    intf_out_mac_address[2] = 0x34;
    intf_out_mac_address[3] = 0x56;
    intf_out_mac_address[4] = 0x78;
    intf_out_mac_address[5] = 0x9a;

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, in_port, intf_in), "intf_in");
    /*
     * 2. Set Out-Port default properties, in case of ARP+AC no need
     */
    snprintf(error_msg, sizeof(error_msg), "out_port %d", out_port);
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, out_port), error_msg);

    /*
     * 3. Create ETH-RIF and set its properties
     */
    snprintf(error_msg, sizeof(error_msg), "intf_in %d", intf_in);
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, intf_in, intf_in_mac_address), error_msg);
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, intf_out, intf_out_mac_address), "intf_out");
    /*
     * 4. Set Incoming ETH-RIF properties
     */
    ingress_rif.vrf = vrf;
    ingress_rif.intf_id = intf_in;
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_rif), "intf_out");

    /*
     * 5. Create ARP and set its properties
     */
    BCM_IF_ERROR_RETURN_MSG(create_arp_plus_ac_type(unit, 0, arp_next_hop_mac, &encap_id, vlan), "");

    /*
     * 6. Create FEC and set its properties
     * only in case the host format is not "no-fec"
     */

    printf("%s():. Create main FEC and set its properties.\r\n",proc_name);
    if (BCM_GPORT_IS_SET(out_port)) {
       gport = out_port;
    }
    else {
       BCM_GPORT_LOCAL_SET(gport, out_port);
    }

    int fec_flags2 = 0;
    if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        fec_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }

    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create_inner(unit, fec, (cint_ip_route_info.is_arp_rif_mode ? 0 : intf_out), encap_id, gport, 0,
        fec_flags2, 0, failover_is_primary,&encoded_fec), "create egress object FEC only");
    printf("%s(): fec 0x%08X encoded_fec 0x%08X.\r\n",proc_name, fec, encoded_fec);


    /*
     * 7. Add Route entry
     */
    printf("%s(): Add route entry.  route 0x%08X mask 0x%08X vrf %d fec 0x%08X. \n",proc_name, route, mask, vrf, fec);
    BCM_IF_ERROR_RETURN_MSG(add_route_ipv4(unit, route, mask, vrf, fec), "");

    printf("%s(): Exit\r\n",proc_name);
    return BCM_E_NONE;
}

int
dnx_basic_example_ipv4(
    int unit,
    int in_port,
    int out_port,
    int kaps_result)
{
    char *proc_name;

    proc_name = "dnx_basic_example_ipv4";
    printf("%s(): Enter. in_port %d out_port %d kaps_result 0x%08X\r\n",proc_name, in_port, out_port, kaps_result);

    BCM_IF_ERROR_RETURN_MSG(dnx_basic_example_ipv4_inner(unit, in_port, out_port, kaps_result), "");
    printf("%s(): Exit\r\n",proc_name);
    return BCM_E_NONE;
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

    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid_null;       /* not intersting */
    /*char *proc_name;

    proc_name = "l3__egress_only_encap__create_inner";
    printf("%s(): Enter. \r\n", proc_name); */

    bcm_l3_egress_t_init(&l3eg);

    l3eg.mac_addr = da;
    l3eg.encap_id = *arp_id;
    l3eg.vlan = vsi;
    l3eg.flags = flags;
    if (flags2 & BCM_L3_FLAGS2_VLAN_TRANSLATION)
    {
        l3eg.flags2 |= BCM_L3_FLAGS2_VLAN_TRANSLATION;
        if(cint_ip_route_info.svtag_enable)
        {
            l3eg.flags3 |= BCM_L3_FLAGS3_EGRESS_SVTAG_PORT_ENABLE;
        }
    }
    if (flags2 & BCM_L3_FLAGS2_VLAN_TRANSLATION_TWO_VLAN_TAGS)
    {
        l3eg.flags2 |= BCM_L3_FLAGS2_VLAN_TRANSLATION_TWO_VLAN_TAGS;
    }
    if (flags2 & BCM_L3_FLAGS2_EGRESS_STAT_ENABLE)
    {
        l3eg.flags2 |= BCM_L3_FLAGS2_EGRESS_STAT_ENABLE;
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
    BCM_IF_ERROR_RETURN_MSG(bcm_l3_egress_create(unit, allocation_flags, &l3eg, &l3egid_null), "create egress object");

    /* printf("%s(): Resultant encap_id = 0x%08X, intf = 0x%08X\r\n", proc_name, l3eg.encap_id, l3eg.intf); */

    *arp_id = l3eg.encap_id;

    return BCM_E_NONE;
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
    if ((allocation_flags & ~(BCM_L3_EGRESS_ONLY | BCM_L3_FLAGS2_VLAN_TRANSLATION | BCM_L3_INGRESS_ONLY | BCM_L3_FLAGS2_VIRTUAL_EGRESS_POINTED)) != 0)
    {
        printf("%s(): Error, some bits on 'allocation_flags' (0x%08X) are not allowed. Allowed mask is 0x%08X\r\n",
            proc_name, allocation_flags,
            BCM_L3_EGRESS_ONLY | BCM_L3_INGRESS_ONLY | BCM_L3_FLAGS2_VIRTUAL_EGRESS_POINTED);
        return BCM_E_PARAM;
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
    if (allocation_flags & BCM_L3_FLAGS2_VLAN_TRANSLATION)
    {
        flags2 |= BCM_L3_FLAGS2_VLAN_TRANSLATION;
        allocation_flags &= ~BCM_L3_FLAGS2_VLAN_TRANSLATION;
    }
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create_inner(unit, allocation_flags, arp_id, da, vsi, 0, flags2), "");
    return BCM_E_NONE;
}

/*
 * Set a FEC entry, without allocating ARP entry
 * - fec_id - FEC-ID
 * - out_rif - Outgoing ETH-RIF or the MPLS tunnel
 * - arp - Outgoing ARP pointer
 * - gport - destination gport
 * - flags - BCM_L3_xxx
 * - flags2 - BCM_L3_FLAGS2_xxx
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
    int flags2,
    int failover_id,
    int failover_is_primary,
    int* interface_id)
{
    char error_msg[200]={0,};
    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid;
    /*char *proc_name;

    proc_name = "l3__egress_only_fec__create_inner";
    printf("%s(): Enter. fec_id 0x%08X\r\n", proc_name, fec_id); */
    bcm_l3_egress_t_init(&l3eg);
    l3eg.intf = out_rif_or_tunnel ? out_rif_or_tunnel : arp;
    l3eg.encap_id = out_rif_or_tunnel ? arp : 0;

    if (failover_is_primary)
    {
        l3eg.failover_if_id = fec_id + 1;
    }
    l3eg.failover_id = failover_id;
    l3eg.port = gport;
    l3eg.flags = flags;
    l3eg.flags2 = flags2;
    l3egid = fec_id;

    snprintf(error_msg, sizeof(error_msg), "create egress object, unit=%d", unit);
    BCM_IF_ERROR_RETURN_MSG(bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY | BCM_L3_WITH_ID, &l3eg, &l3egid), error_msg);
    if (interface_id != NULL)
    {
        *interface_id = l3egid;
    }
    /*
     * The code below is not operationally required and is added for debug purposes only.
     * It may be removed, if necessary.
     */
    snprintf(error_msg, sizeof(error_msg), "unit=%d", unit);
    BCM_IF_ERROR_RETURN_MSG(bcm_l3_egress_get(unit, l3egid, &l3eg), error_msg);

    /* printf("%s(): Exit.\r\n", proc_name); */
    return BCM_E_NONE;
}

int
l3__egress_only_fec__create(
    int unit,
    int fec_id,
    int out_rif_or_tunnel,
    int arp,
    bcm_gport_t out_port,
    int flags,
    int flags2)
{
    return l3__egress_only_fec__create_inner(unit, fec_id, out_rif_or_tunnel, arp, out_port, flags, flags2, 0, 0, NULL);
}

/* Get the FEC IDs allocation range based on the provided hierarchy and direction*/
int
get_fec_range(
    int unit,
    int hierarchy,
    int direction,
    int *start,
    int *nof_fecs)
{
    bcm_switch_fec_property_config_t fec_config;

    /* Set flags based on the FEC hierarchy*/
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

    if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        /* Set flags based on the FEC direction - FWD(0) or RPF(1)*/
        if (direction == 0)
        {
            fec_config.flags |= BCM_SWITCH_FEC_PROPERTY_FWD;
        }
        else if (direction == 1)
        {
            fec_config.flags |= BCM_SWITCH_FEC_PROPERTY_RPF;
        }
        else
        {
            printf("Error, Unsupported FEC direction %d \n",direction);
            return BCM_E_PARAM;
        }
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_switch_fec_property_get(unit,&fec_config), "");

    *start = fec_config.start;
    *nof_fecs = fec_config.end - fec_config.start + 1;
    return BCM_E_NONE;
}

/*
 * Get the first FEC ID of the given hierarchy and direction which isn't in the ECMP range
 */
int
get_first_fec_in_range_which_not_in_ecmp_range(
    int unit,
    int hierarchy,
    int direction,
    int *fec_id)
{
    int nof_fecs;
    uint32 ecmp_range;

    ecmp_range = *(dnxc_data_get(unit, "l3", "ecmp", "nof_ecmp", NULL));

    BCM_IF_ERROR_RETURN_MSG(get_fec_range(unit, hierarchy, direction, fec_id, &nof_fecs), "");
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

    return BCM_E_NONE;
}

/*
 * This is a function that would set the configuration for basic forwarding of an IPv6oETH packet.
 * Packet should match on the created IPv6 route entry and will be forwarded to the resulting FEC and
 * it will be routed from in_port to out-port.
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
 *
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
    bcm_l3_host_t host;
    /*char *proc_name;*/

    if (!*dnxc_data_get(unit, "l3", "fwd", "host_entry_support", NULL))
    {
        BCM_IF_ERROR_RETURN_MSG(add_route_ipv4_encap_dest(unit, addr, 0xffffffff, vrf, intf, encap_id, destination), "");
        return BCM_E_NONE;
    }

    /*proc_name = "add_host_ipv4";
    printf("%s(): Enter. addr 0x%08X intf 0x%08X destination 0x%08X\n",proc_name, addr, intf, destination); */
    bcm_l3_host_t_init(host);

    host.l3a_ip_addr = addr;
    host.l3a_vrf = vrf;
    host.l3a_intf = (intf != 0) ? intf : encap_id;
    host.encap_id = (intf != 0) ? encap_id : 0;

    if (destination)
    {
        host.l3a_port_tgid = destination;
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_l3_host_add(unit, host), "");
    /* printf("%s(): Exit\n",proc_name); */
    return BCM_E_NONE;
}

/*
 * Add IPv6 Host entry <vrf, addr> --> intf
 * - addr: IPv6 address 16 8-bit values
 * - vrf: VRF ID
 * - intf: egress object created using create_l3_egress (FEC)
 *
 */
int
add_host_ipv6(
    int unit,
    bcm_ip6_t addr,
    int vrf,
    int intf)
{
    bcm_l3_host_t host;
    bcm_l3_host_t_init(host);
    bcm_ip6_t full_mask = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    if (!*dnxc_data_get(unit, "l3", "fwd", "host_entry_support", NULL))
    {
        BCM_IF_ERROR_RETURN_MSG(add_route_ipv6(unit, addr, full_mask, vrf, intf), "");
    }
    else
    {

        host.l3a_ip6_addr = addr;
        host.l3a_vrf = vrf;
        host.l3a_intf = intf;       /* FEC-ID */
        host.l3a_flags = BCM_L3_IP6;
        BCM_IF_ERROR_RETURN_MSG(bcm_l3_host_add(unit, host), "");
    }
    return BCM_E_NONE;
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
 *
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

    if (!*dnxc_data_get(unit, "l3", "fwd", "host_entry_support", NULL))
    {
        bcm_ip6_t full_mask = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

        BCM_IF_ERROR_RETURN_MSG(add_route_ipv6_encap_dest(unit, addr, full_mask, vrf, intf, encap_id, destination), "");
    }
    else
    {
        bcm_l3_host_t host;
        bcm_l3_host_t_init(host);
        host.l3a_ip6_addr = addr;
        host.l3a_vrf = vrf;
        host.l3a_flags = BCM_L3_IP6;
        host.l3a_intf = (intf != 0) ? intf : encap_id;
        host.encap_id = (intf != 0) ? encap_id : 0;
        if (destination)
        {
            host.l3a_port_tgid = destination;
        }
        BCM_IF_ERROR_RETURN_MSG(bcm_l3_host_add(unit, host), "");
    }
    return BCM_E_NONE;
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
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), "");
    vlan_port_gport = vlan_port.vlan_port_id;
    printf("egress_vlan_port_id: 0x%08x\n", vlan_port_gport);

    printf("%s(): Creating arp with encap_id 0x%08X\n", proc_name, *encap_id);

    bcm_l3_egress_t_init(&l3eg);

    l3eg.mac_addr = arp_next_hop_mac;
    l3eg.encap_id = *encap_id;
    if (cint_ip_route_info.is_arp_rif_mode)
    {
        l3eg.vlan = vlan;
    }

    l3eg.flags = 0;
    l3eg.vlan_port_id = vlan_port_gport;

    BCM_IF_ERROR_RETURN_MSG(bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY, &l3eg, &l3egid_null), "create egress object");

    printf("%s(): Result: encap_id = 0x%08X, intf = 0x%08X\r\n", proc_name, l3eg.encap_id, l3eg.intf);

    *encap_id = l3eg.encap_id;

    /*
     * use bcm_vlan_port_translation_set for vlan translation
     */

    printf("%s(): Call bcm_vlan_port_translation_set() on vlan port\n",proc_name);
    BCM_IF_ERROR_RETURN_MSG(vlan_translate_ive_eve_translation_set(unit, vlan_port_gport,         /* lif  */
                                                       0x9100,               /* outer_tpid */
                                                       0x8100,               /* inner_tpid */
                                                       bcmVlanActionAdd,  /* outer_action */
                                                       bcmVlanActionAdd,  /* inner_action */
                                                       vlan,                 /* new_outer_vid */
                                                       2*vlan,           /* new_inner_vid */
                                                       7,    /* vlan_edit_profile */
                                                       0,                    /* tag_format - must be untag! */
                                                       FALSE                 /* is_ive */
                                                       ), "create egress vlan editing");

    printf("%s(): Exit\r\n",proc_name);
    return BCM_E_NONE;
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
    char *proc_name;
    bcm_gport_t vlan_port_gport;
    bcm_vlan_port_t vlan_port;
    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid_null;       /* not interesting */

    proc_name = "create_arp_with_next_ac_type_vlan_translate_none";
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
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), "");
    vlan_port_gport = vlan_port.vlan_port_id;
    printf("egress_vlan_port_id: 0x%08x\n", vlan_port_gport);

    printf("%s(): Creating arp with encap_id 0x%08X\n", proc_name, *encap_id);

    bcm_l3_egress_t_init(&l3eg);

    l3eg.mac_addr = arp_next_hop_mac;
    l3eg.encap_id = *encap_id;
    l3eg.vlan = vlan;
    l3eg.flags = 0;
    l3eg.vlan_port_id = vlan_port_gport;

    BCM_IF_ERROR_RETURN_MSG(bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY, &l3eg, &l3egid_null), "create egress object");

    printf("%s(): Result: encap_id = 0x%08X, intf = 0x%08X\r\n", proc_name, l3eg.encap_id, l3eg.intf);

    *encap_id = l3eg.encap_id;

    printf("%s(): Exit\r\n",proc_name);
    return BCM_E_NONE;
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
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create_inner(unit, 0, encap_id, arp_next_hop_mac,
                                             ((arp_plus_ac_type || cint_ip_route_info.is_arp_rif_mode) ? vlan : 0),
                                             0, flags2), "create egress object ARP only");

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
        BCM_IF_ERROR_RETURN_MSG(vlan_translate_ive_eve_translation_set(unit, arp_ac_gport,         /* lif  */
                                                          0x9100,               /* outer_tpid */
                                                          0x8100,               /* inner_tpid */
                                                          bcmVlanActionArpVlanTranslateAdd,  /* outer_action */
                                                          inner_action,  /* inner_action */
                                                          vlan,                 /* new_outer_vid */
                                                          inner_vlan,           /* new_inner_vid */
                                                          vlan_edit_profile,    /* vlan_edit_profile */
                                                          0,                    /* tag_format - must be untag! */
                                                          FALSE                 /* is_ive */
                                                          ), "");

        /*
        * Check that the VLAN Edit info was set correctly:
        */
        bcm_vlan_port_translation_t lif_translation_info_get;
        lif_translation_info_get.flags = BCM_VLAN_ACTION_SET_EGRESS;
        lif_translation_info_get.gport = arp_ac_gport;

        lif_translation_info_get.new_outer_vlan = 0xFFFFFFFF;
        lif_translation_info_get.new_inner_vlan = 0xFFFFFFFF;
        lif_translation_info_get.vlan_edit_class_id = 0xFFFFFFFF;

        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_translation_get(unit, &lif_translation_info_get), "");

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
    return BCM_E_NONE;
}

/*
 * This is a function that would set the configuration for basic forwarding of an IPv6oETH packet.
 * Packet should match on the created IPv6 route entry and will be forwarded to the resulting FEC and
 * it will be routed from in_port to out-port.
 *
 * Route:
 * packet to send:
 *  - in port = in_port
 *  - DA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}
 *  - DIP = 0000:0000:0000:0000:1234:0000:0000:FF10 to 0000:0000:0000:0000:1234:0000:0000:FF1F
 *          without 0000:0000:0000:0000:1234:0000:0000:FF13
 * expected:
 *  - out port = out_port
 *  - vlan = 100
 *  - DA = {0x20, 0x00, 0x00, 0x00, 0xcd, 0x1d}
 *  - SA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x01}
 *  TTL decremented
 *
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
dnx_basic_example_ipv6(
    int unit,
    int in_port,
    int out_port,
    int arp_plus_ac_type)
{
    char error_msg[200]={0,};
    int intf_in = cint_ip_route_info.intf_in;           /* Incoming packet ETH-RIF */
    int intf_out = cint_ip_route_info.intf_out;         /* Outgoing packet ETH-RIF */
    int fec = ip_route_fec;
    int vrf = 1;
    int vsi = cint_ip_route_info.intf_out;
    int encap_id = *dnxc_data_get(unit, "l3", "rif", "nof_rifs", NULL) + 0x384;  /* ARP-Link-layer */
    int fec_flags2 = 0;
    int failover_is_primary = 0;
    int encoded_fec;

    bcm_gport_t gport = 0;
    bcm_mac_t intf_in_mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };     /* my-MAC */
    bcm_mac_t intf_out_mac_address = { 0x00, 0x12, 0x34, 0x56, 0x78, 0x9a };    /* my-MAC */
    bcm_mac_t arp_next_hop_mac = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d };        /* next_hop_mac */
    char *proc_name;
    proc_name = "dnx_basic_example_ipv6";
    l3_ingress_intf ingress_itf;
    l3_ingress_intf_init(&ingress_itf);
    bcm_l3_intf_t l3if;

    BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &fec), "");

    fec += 0x500;
    ip_route_fec = fec;

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    snprintf(error_msg, sizeof(error_msg), "in_port = %d, intf_in = %d", in_port, intf_in);
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, in_port, intf_in), error_msg);

    /*
     * 2. Set Out-Port default properties
     */
    snprintf(error_msg, sizeof(error_msg), "out_port = %d", out_port);
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, out_port), error_msg);


    /*
     * 3. Create ETH-RIF and set its properties
     */
    snprintf(error_msg, sizeof(error_msg), "intf_in = %d", intf_in);
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create_vrf(unit, intf_in, 0, intf_in_mac_address), error_msg);
    snprintf(error_msg, sizeof(error_msg), "intf_out = %d", intf_out);
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create_vrf(unit, intf_out, 0, intf_out_mac_address), error_msg);

    /*
     * 4. Set Incoming ETH-RIF properties
     */
    ingress_itf.vrf = vrf;
    ingress_itf.intf_id = intf_in;
    snprintf(error_msg, sizeof(error_msg), "intf_in = %d, vrf = %d", intf_in, vrf);
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_itf), error_msg);

    /*
     * 5. Create ARP and set its properties
     */
    BCM_IF_ERROR_RETURN_MSG(create_arp_plus_ac_type(unit, arp_plus_ac_type, arp_next_hop_mac, &encap_id, vsi), "");
    glob_encap_id = encap_id;

    /*
     * 6. Create FEC and set its properties
     */
    BCM_GPORT_LOCAL_SET(gport, out_port);

    if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        fec_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }

    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create_inner(unit, fec, (cint_ip_route_info.is_arp_rif_mode ? 0 : intf_out), encap_id, gport, 0,
        fec_flags2, 0, failover_is_primary, &encoded_fec), "create egress object FEC only");

    printf("%s(): fec 0x%08X encoded_fec 0x%08X.\r\n", proc_name, fec, encoded_fec);

    /*
     * 7. Add Route entry
     */
    snprintf(error_msg, sizeof(error_msg), "vrf = %d, fec = %d", vrf, fec);
    BCM_IF_ERROR_RETURN_MSG(add_route_ipv6(unit, cint_ip_route_basic_ipv6_route, cint_ip_route_basic_ipv6_mask, vrf, fec), error_msg);

    return BCM_E_NONE;
}

/*
 * Utilities APIs
 */
/*
 * Set RPF traps:
 */
int
dnx_l3_ip_rpf_config_traps(
    int unit)
{
    int rv = BCM_E_NONE, i;
    bcm_rx_trap_config_t config;
    int flags = 0;
    int rpf_uc_strict_trap = 0, rpf_uc_loose_trap = 0;
    /*
     * RPF types:
     */
    /*
     * bcmRxTrapUcLooseRpfFail, : Forwarding Code is IPv4 UC and RPF FEC Pointer Valid is not set.
     */
    /*
     * bcmRxTrapUcStrictRpfFail, : UC-RPF-Mode is 'Strict' and OutRIF is not equal to packet InRIF .
     */

    /*
     * change dest port for trap
     */
    bcm_rx_trap_config_t_init(&config);
    config.flags |= (BCM_RX_TRAP_UPDATE_DEST);
    config.trap_strength = 7;
    config.dest_port = BCM_GPORT_BLACK_HOLE;

    /*
     * set uc strict to drop packet
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_get(unit, 0, bcmRxTrapUcStrictRpfFail, &rpf_uc_strict_trap), "rpf_uc_strict_trap");
    rv = bcm_rx_trap_set(unit, rpf_uc_strict_trap, &config);
    if (rv != BCM_E_NONE)
    {
        BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_create(unit, 0, bcmRxTrapUcStrictRpfFail, &rpf_uc_strict_trap),
            "trap bcmRxTrapUcStrictRpfFail");
        BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_set(unit, rpf_uc_strict_trap, &config), "rpf_uc_strict_trap");
    }

    /*
     * set uc loose to drop packet
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_get(unit, 0, bcmRxTrapUcLooseRpfFail, &rpf_uc_loose_trap), "rpf_uc_loose_trap");
    rv = bcm_rx_trap_set(unit, rpf_uc_loose_trap, &config);
    if (rv != BCM_E_NONE)
    {
        BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_create(unit, 0, bcmRxTrapUcLooseRpfFail, &rpf_uc_loose_trap),
            "trap bcmRxTrapUcLooseRpfFail");
        BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_set(unit, rpf_uc_loose_trap, &config), "rpf_uc_loose_trap");
    }
    return BCM_E_NONE;
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
 *
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
    char error_msg[200]={0,};
    int intf_in = cint_ip_route_info.intf_in;           /* Incoming packet ETH-RIF */
    int intf_out = cint_ip_route_info.intf_out;         /* Outgoing packet ETH-RIF */
    int fec = ip_route_fec;
    int vrf = 1;
    int vsi = cint_ip_route_info.force_vsi;
    int encap_id = *dnxc_data_get(unit, "l3", "rif", "nof_rifs", NULL) + 0x384;  /* ARP-Link-layer */
    bcm_gport_t gport = 0;
    int vlan = cint_ip_route_info.intf_out;
    bcm_mac_t arp_next_hop_mac = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d };        /* next_hop_mac */
    l3_ingress_intf ingress_itf;
    l3_ingress_intf_init(&ingress_itf);

    BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &fec), "");
    fec += 0x500;

    ip_route_fec = fec;
    if(sbfd_reflector_ipv6_with_srv6_inner_mac)
    {
        cint_ip_route_info.intf_out_mac_address[0] = 0;
        cint_ip_route_info.intf_out_mac_address[1] = 0;
        cint_ip_route_info.intf_out_mac_address[2] = 0;
        cint_ip_route_info.intf_out_mac_address[3] = 0;
        cint_ip_route_info.intf_out_mac_address[4] = 0xcd;
        cint_ip_route_info.intf_out_mac_address[5] = 0x1d;
    }
    /*
     * 1. Set In-Port to In ETh-RIF
     */
    snprintf(error_msg, sizeof(error_msg), "in_port = %d, intf_in = %d", in_port, intf_in);
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, in_port, intf_in), error_msg);

    /*
     * 2. Set Out-Port default properties
     */
    snprintf(error_msg, sizeof(error_msg), "out_port = %d", out_port);
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, out_port), error_msg);

    /*
     * 3. Create ETH-RIF and set its properties
     */
    snprintf(error_msg, sizeof(error_msg), "intf_in = %d", intf_in);
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, intf_in, cint_ip_route_info.intf_in_mac_address), error_msg);
    snprintf(error_msg, sizeof(error_msg), "intf_out = %d", intf_out);
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, intf_out, cint_ip_route_info.intf_out_mac_address), error_msg);

    /*
     * 4. Set Incoming ETH-RIF properties
     */
    ingress_itf.vrf = vrf;
    ingress_itf.intf_id = intf_in;
    if(global_route)
    {
        ingress_itf.flags = BCM_L3_INGRESS_GLOBAL_ROUTE;
    }
    snprintf(error_msg, sizeof(error_msg), "intf_in = %d, vrf = %d", intf_in, vrf);
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_itf), error_msg);

    /*
     * 5. Create FEC and set its properties
     */
    if (BCM_GPORT_IS_TRUNK(out_port))
    {
        gport = out_port;
    } else
    {
        BCM_GPORT_LOCAL_SET(gport, out_port);
    }
    int fec_flags2 = 0;
    if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        fec_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }
    snprintf(error_msg, sizeof(error_msg), "create egress object FEC only: fec = %d, intf_out = %d, encap_id = %d, out_port = %d",
        fec, intf_out, encap_id, out_port);
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, fec, intf_out, encap_id, gport, 0, fec_flags2), error_msg);

    /*
     * 6. Create ARP and set its properties
     */
    if(arp_plus_ac_type == 0)
    {
        snprintf(error_msg, sizeof(error_msg), "create egress object ARP only: encap_id = %d, vsi = %d", encap_id, vsi);
        BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create(unit, 0, encap_id, arp_next_hop_mac, vsi), error_msg);
    }
    else
    {
        snprintf(error_msg, sizeof(error_msg), "create egress object ARP only: arp_plus_ac_type = %d, encap_id = %d, vlan = %d",
            arp_plus_ac_type, encap_id, vlan);
        BCM_IF_ERROR_RETURN_MSG(create_arp_plus_ac_type(unit, arp_plus_ac_type, arp_next_hop_mac, &encap_id, vlan), error_msg);
    }

    /*
     * 7. Add Route entry
     */
    snprintf(error_msg, sizeof(error_msg), "vrf = %d, fec = %d", vrf, fec);
    if (do_raw)
    {
        /*
         * If 'do_raw' is set then load 'fec' as is (raw value).
         */
        BCM_IF_ERROR_RETURN_MSG(add_route_ipv6_raw(unit, cint_ip_route_basic_ipv6_route, cint_ip_route_basic_ipv6_mask, vrf, fec), error_msg);
    }
    else
    {
        BCM_IF_ERROR_RETURN_MSG(add_route_ipv6(unit, cint_ip_route_basic_ipv6_route, cint_ip_route_basic_ipv6_mask, vrf, fec), error_msg);
    }

    /*
     * 7. Add host entry
     */
    if (*dnxc_data_get(unit, "l3", "fwd", "host_entry_support", NULL) && add_ipv6_host_entry)
    {
        snprintf(error_msg, sizeof(error_msg), "vrf = %d, fec = %d", vrf, fec);
        BCM_IF_ERROR_RETURN_MSG(add_host_ipv6(unit, cint_ip_route_basic_ipv6_host, vrf, fec), error_msg);
    }

    return BCM_E_NONE;
}



int
basic_example_ipv6_no_rif(
    int unit,
    int in_port,
    int out_port,
    int do_raw,
    int global_route,
    int arp_plus_ac_type)
{
    char error_msg[200]={0,};
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
    l3_ingress_intf ingress_itf;
    l3_ingress_intf_init(&ingress_itf);

    BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &fec), "");
    fec += 0x500;

    ip_route_fec = fec;

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    snprintf(error_msg, sizeof(error_msg), "in_port = %d, intf_in = %d", in_port, intf_in);
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, in_port, intf_in), error_msg);

    /*
     * 2. Set Out-Port default properties
     */
    snprintf(error_msg, sizeof(error_msg), "out_port = %d", out_port);
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, out_port), error_msg);

    /*
     * 3. Create ETH-RIF and set its properties
     */
    snprintf(error_msg, sizeof(error_msg), "intf_in = %d", intf_in);
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, intf_in, intf_in_mac_address), error_msg);
    snprintf(error_msg, sizeof(error_msg), "intf_out = %d", intf_out);
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, intf_out, intf_out_mac_address), error_msg);

    /*
     * 4. Set Incoming ETH-RIF properties
     */
    ingress_itf.vrf = vrf;
    ingress_itf.intf_id = intf_in;
    if(global_route)
    {
        ingress_itf.flags = BCM_L3_INGRESS_GLOBAL_ROUTE;
    }
    snprintf(error_msg, sizeof(error_msg), "intf_in = %d, vrf = %d", intf_in, vrf);
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_itf), error_msg);

    /*
     * 5. Create FEC and set its properties
     */

    int fec_flags2 = 0;
    if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        fec_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }
    BCM_GPORT_LOCAL_SET(gport, out_port);
    int encap_id_itf;
    BCM_L3_ITF_SET(encap_id_itf, BCM_L3_ITF_TYPE_LIF, encap_id);
    snprintf(error_msg, sizeof(error_msg), "create egress object FEC only: fec = %d, intf_out = %d, encap_id = %d, out_port = %d",
        fec, intf_out, encap_id, out_port);
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, fec, 0, encap_id_itf, gport, 0, fec_flags2), error_msg);
    /*
     * 6. Create ARP and set its properties
     */
    snprintf(error_msg, sizeof(error_msg), "create egress object ARP only: encap_id = %d, vsi = %d", encap_id, vsi);
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create(unit, BCM_L3_FLAGS2_VLAN_TRANSLATION, encap_id, arp_next_hop_mac, intf_out), error_msg);

    /*
     * 7. Add Route entry
     */
    snprintf(error_msg, sizeof(error_msg), "vrf = %d, fec = %d", vrf, fec);
    if (do_raw)
    {
        /*
         * If 'do_raw' is set then load 'fec' as is (raw value).
         */
        BCM_IF_ERROR_RETURN_MSG(add_route_ipv6_raw(unit, cint_ip_route_basic_ipv6_route, cint_ip_route_basic_ipv6_mask, vrf, fec), error_msg);
    }
    else
    {
        BCM_IF_ERROR_RETURN_MSG(add_route_ipv6(unit, cint_ip_route_basic_ipv6_route, cint_ip_route_basic_ipv6_mask, vrf, fec), error_msg);
    }

    /*
     * 7. Add host entry
     */
    snprintf(error_msg, sizeof(error_msg), "vrf = %d, fec = %d", vrf, fec);
    BCM_IF_ERROR_RETURN_MSG(add_host_ipv6(unit, cint_ip_route_basic_ipv6_host, vrf, fec), error_msg);

    return BCM_E_NONE;
}

/*
 * Get a default first hierarchy FEC
 */
int
get_default_fec_id(
    int unit,
    int *fec_id)
{
    BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, fec_id), "");

    return BCM_E_NONE;
}

/*
 * This is a function that would set the configuration for basic forwarding of an IPv6oETH packet.
 * Packet should match on the created IPv6 route entry and will be forwarded to the resulting FEC and
 * it will be routed from in_port to out-port.
 * If 'do_raw' is non-zero then 'fec' is loaded as is (as a raw value').
 */
int
basic_example_ipv6_multidevice(
    int ingress_unit,
    int egress_unit,
    int in_port,
    int out_port)
{
    char error_msg[200]={0,};
    int intf_in = 15;           /* Incoming packet ETH-RIF */
    int intf_out = 100;         /* Outgoing packet ETH-RIF */
    int fec = ip_route_fec;
    int vrf = 1;
    int vsi = 0;
    int encap_id = *dnxc_data_get(ingress_unit, "l3", "rif", "nof_rifs", NULL) + 0x384;  /* ARP-Link-layer */
    bcm_gport_t gport = 0;
    int vlan = 100;
    bcm_mac_t intf_in_mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };     /* my-MAC */
    bcm_mac_t intf_out_mac_address = { 0x00, 0x12, 0x34, 0x56, 0x78, 0x9a };    /* my-MAC */
    bcm_mac_t arp_next_hop_mac = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d };        /* next_hop_mac */
    l3_ingress_intf ingress_itf;
    l3_ingress_intf_init(&ingress_itf);
    bcm_l3_intf_t l3if;
    int fec_flags2 = 0;

    BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(ingress_unit, 0, 0, &fec), "");
    fec += 0x500;

    ip_route_fec = fec;

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    snprintf(error_msg, sizeof(error_msg), "in_port = %d, intf_in = %d", in_port, intf_in);
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(ingress_unit, in_port, intf_in), error_msg);

    /*
     * 2. Set Out-Port default properties
     */
    snprintf(error_msg, sizeof(error_msg), "out_port = %d", out_port);
    BCM_IF_ERROR_RETURN_MSG(out_port_set(egress_unit, out_port), error_msg);

    /*
     * 3. Create ETH-RIF and set its properties
     */
    snprintf(error_msg, sizeof(error_msg), "intf_in = %d", intf_in);
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(ingress_unit, intf_in, intf_in_mac_address), error_msg);
    snprintf(error_msg, sizeof(error_msg), "intf_out = %d", intf_out);
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(egress_unit, intf_out, intf_out_mac_address), error_msg);

    /*
     * 4. Set Incoming ETH-RIF properties
     */
    ingress_itf.vrf = vrf;
    ingress_itf.intf_id = intf_in;
    snprintf(error_msg, sizeof(error_msg), "intf_in = %d, vrf = %d", intf_in, vrf);
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(ingress_unit, &ingress_itf), error_msg);

    /*
     * 5. Create FEC and set its properties
     */
    if(*dnxc_data_get(ingress_unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        fec_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }
    snprintf(error_msg, sizeof(error_msg), "create egress object FEC only: fec = %d, intf_out = %d, encap_id = %d, out_port = %d",
        fec, intf_out, encap_id, out_port);
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(ingress_unit, fec, intf_out, encap_id, out_port, 0, fec_flags2), error_msg);
    /*
     * 6. Create ARP and set its properties
     */
    snprintf(error_msg, sizeof(error_msg), "create egress object ARP only: encap_id = %d, vsi = %d", encap_id, vsi);
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create(egress_unit, 0, encap_id, arp_next_hop_mac, vsi), error_msg);

    /*
     * 7. Add Route entry
     */
    snprintf(error_msg, sizeof(error_msg), "vrf = %d, fec = %d", vrf, fec);
    BCM_IF_ERROR_RETURN_MSG(add_route_ipv6(ingress_unit, cint_ip_route_basic_ipv6_route, cint_ip_route_basic_ipv6_mask, vrf, fec), error_msg);

    /*
     * 8. Add host entry
     */
    snprintf(error_msg, sizeof(error_msg), "vrf = %d, fec = %d", vrf, fec);
    BCM_IF_ERROR_RETURN_MSG(add_host_ipv6(ingress_unit, cint_ip_route_basic_ipv6_host, vrf, fec), error_msg);

    return BCM_E_NONE;
}

int
dnx_basic_example_arp_plus_ac(
    int unit,
    int in_port,
    int out_port,
    int arp_plus_ac_type,
    int kaps_result)
{
    char *proc_name;

    proc_name = "basic_example_inner_zeros";
    printf("%s(): Enter. in_port %d out_port %d\r\n", proc_name, in_port, out_port);
    BCM_IF_ERROR_RETURN_MSG(dnx_basic_example_inner(unit, in_port, out_port, arp_plus_ac_type, kaps_result), "");
    printf("%s(): Exit\r\n", proc_name);
    return BCM_E_NONE;
}

int
dnx_in_port_intf_reset(
    int unit)
{
    int i;

    for (i = 0; i < 4; i++)
    {
        if(0 != in_port_intf[i])
        {
            BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_destroy(unit, in_port_intf[i]), "");
            in_port_intf[i] = 0;
        }
    }

    in_port_intf_num = 0;

    return BCM_E_NONE;
}

/*
 * Packet will be routed from in_port to redir_port
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
 *
 */
/*
 * This procedure is for 'raw' FEC only. 'Fec' is used as is (raw) within 'add_route_ipv4()'
 * 'do_redirect' is non-zero.
 * 'redir_port' is another port to which the packet may be directed to and 'modid'
 * is the module id for all involved ports.
 * 'redir_encap_id' is the identifier of the allocated ARP-ID.
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
    char error_msg[200]={0,};
    int intf_in = 15;          /* Incoming packet ETH-RIF */
    int intf_out = 100;         /* Outgoing packet ETH-RIF */
    int fec;
    int vrf;
    int encap_id = *dnxc_data_get(unit, "l3", "rif", "nof_rifs", NULL) + 0x384; /* ARP-Link-layer */
    bcm_mac_t intf_in_mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };     /* my-MAC   */
    bcm_mac_t intf_out_mac_address = { 0x00, 0x12, 0x34, 0x56, 0x78, 0x9a };    /* dest-MAC */
    bcm_mac_t arp_next_hop_mac = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d };        /* next_hop_mac */
    bcm_gport_t gport;
    uint32 route;
    uint32 host;
    uint32 mask;
    l3_ingress_intf ingress_rif;
    uint32 flags2 = 0;
    int vlan = 100;
    int failover_is_primary = 0;
    int encoded_fec;
    uint32 redir_host;
    int redir_encoded_fec;
    bcm_gport_t redir_gport;
    int redir_fec;
    char *proc_name;

    l3_ingress_intf_init(&ingress_rif);
    /*
     * Set 'fec' as on the coresponding test. See, for example,
     * AT_Dnx_Cint_l3_ip_route_basic_raw_redirect
     */
    fec = kaps_result;
    vrf = 1;
    route = 0x7FFFFF00;
    host  = 0x7FFFFF02;
    mask  = 0xFFFFFFF0;
    failover_id = 0;
    host_format = 0;
    redir_host = host;
    redir_fec = redir_kaps_result;

    proc_name = "dnx_basic_example_inner_redirect";
    printf("%s(): Enter. in_port %d out_port %d redir_port %d modid %d\r\n",proc_name, in_port, out_port, redir_port, modid);
    printf("==> kaps_result 0x%08X redir_kaps_result 0x%08X redir_encap_id 0x%08X\r\n", kaps_result, redir_kaps_result, redir_encap_id);

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, in_port, intf_in), "");
    /*
     * 2. Set Out-Port default properties, in case of ARP+AC no need
     */
    {
        /*
         * For original packet.
         */
        snprintf(error_msg, sizeof(error_msg), "intf_out out_port %d", out_port);
        BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, out_port), error_msg);
    }
    {
        /*
         * For redirected packet.
         */
        snprintf(error_msg, sizeof(error_msg), "redir_port %d", redir_port);
        BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, redir_port), error_msg);
    }
    /*
     * 3. Create ETH-RIF and set its properties
     */
    snprintf(error_msg, sizeof(error_msg), "intf_in %d", intf_in);
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, intf_in, intf_in_mac_address), error_msg);
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, intf_out, intf_out_mac_address), "intf_out");
    /*
     * 4. Set Incoming ETH-RIF properties
     */
    ingress_rif.vrf = vrf;
    ingress_rif.intf_id = intf_in;
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_rif), "intf_out");
    /*
     * 5. Create FEC and set its properties
     */
    {
        uint32 fec_flags2 = 0;
        if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
        {
            fec_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
        }

        {
            /*
             * This is for the original packet to be received.
             */
            printf("%s(): host_format %d. Create main FEC and set its properties.\r\n",proc_name, host_format);
            BCM_GPORT_LOCAL_SET(gport, out_port);
            BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create_inner(unit, fec, intf_out, encap_id, gport, 0, fec_flags2,
                failover_id, failover_is_primary,&encoded_fec), "create egress object FEC only");
            printf("%s(): fec 0x%08X encoded_fec 0x%08X.\r\n",proc_name, fec, encoded_fec);
        }
        {
            /*
             * This is for the redirected packet to be received.
             */
            printf("%s(): host_format %d. Create redir FEC and set its properties.\r\n",proc_name, host_format);
            BCM_GPORT_LOCAL_SET(redir_gport, redir_port);
            BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create_inner(unit, redir_fec, intf_out, redir_encap_id, redir_gport, 0, fec_flags2,
                failover_id, failover_is_primary,&redir_encoded_fec), "create egress object FEC only");
            printf("%s(): redir_fec 0x%08X redir_encoded_fec 0x%08X.\r\n",proc_name, redir_fec, redir_encoded_fec);
        }
    }
    /*
     * 6. Create ARP and set its properties
     */
    {
        BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create_inner(unit, 0, &redir_encap_id, arp_next_hop_mac, vlan, 0, flags2),
            "create egress object ARP only");
    }
    /*
     * 7. Add Route entry
     */
    {
        printf("%s(): Add route entry. route 0x%08X mask 0x%08X vrf %d fec 0x%08X. \n",proc_name, route, mask, vrf, fec);
        BCM_IF_ERROR_RETURN_MSG(add_route_ipv4_raw(unit, route, mask, vrf, fec), "");
    }
    /*
     * 8. Configure PMF to convert 'kaps_result' to 'redir_kaps_result'
     */
    {
        BCM_IF_ERROR_RETURN_MSG(dnx_kaps_raw_data_redirect(unit,kaps_result,1,redir_kaps_result,0/*redir_port*/), "");
    }
    /*
     * 9. Add host entry
     */
    {
        BCM_IF_ERROR_RETURN_MSG(add_host_ipv4(unit, host, vrf, encoded_fec, 0, 0), "");
    }
    printf("%s(): Exit\r\n",proc_name);
    return BCM_E_NONE;
}

