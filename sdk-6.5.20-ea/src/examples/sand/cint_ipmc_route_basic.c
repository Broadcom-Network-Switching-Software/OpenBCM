/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * Example of a simple-router IPMC scenario
 * Test Scenario
 *
 * ./bcm.user
 * cint src/examples/sand/cint_ip_route_basic.c
 * cint src/examples/sand/cint_ipmc_route_basic.c
 * cint
 * basic_ipmc_example(0,0xCC,200,201,202);
 * exit;
 *
 * ETH-RIF packet
 * tx 1 psrc = 200 data = 01005e020202000007000100810000cc0800450000350000000080009743c0800101e0020202000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *
 *
 * Example of a simple-router IPv6 MC scenario
 *
 * ./bcm.user
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../../../src/examples/sand/cint_ipmc_route_basic.c
 * cint
 * basic_ipmc_v6_example(0,0xCC,200,201,202);
 * exit;
 *
 * ETH-RIF packet
 * tx 1 psrc=200 data=333300001111000007000100810000cc86dd600000000049068000000000000000000000333322221111ffff000000000000000000000000111100010203040506070809
 */

struct cint_ipmc_basic_info_s
{
    int in_port;                                        /** incoming port */
    int out_ports[4];                                   /** outgoing ports */
    int intf_in;                                        /** Incoming packet ETH-RIF */
    int intf_out;                                       /** Outgoing packet ETH-RIF */
    bcm_mac_t intf_in_mac_address;                      /** mac for in-RIF */
    bcm_mac_t intf_out_mac_address;                     /** mac for out-RIF */
    int encap_id;                                       /** encap ID */
    int nof_replications;                               /** Number of MC replications */
    int mc_group;                                       /** MC group ID*/
    bcm_ip_t mc_host_dip;                               /** MC host IP*/
    bcm_ip_t host_sip;                                  /** Source IP */
    int vrf;                                            /** VRF loose */
    bcm_if_t fec_id;                                    /** FEC id */
    bcm_ip6_t ipv6_dip_addr;                            /** IPv6 DIP */
    bcm_ip6_t ipv6_sip_addr;                            /** IPv6 SIP */
    uint16 compressed_sip;                              /** Compressed SIP for cascaded lookup */
};

cint_ipmc_basic_info_s cint_ipmc_info = {
    /*
     * ports : in_port
     */
    200,
    /*
     * ports: out_ports
     */
    {201, 202, 203, 200},
    /*
     * intf_in | intf_out
     */
    15, 0x100,
    /*
     * intf_in_mac_address | intf_out_mac_address
     */
    {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}, {0x00, 0x12, 0x34, 0x56, 0x78, 0x9a},
    /*
     * encap_id
     */
    0x1384,
    /*
     * nof_replications
     */
    2,
    /*
     * mc_group
     */
    6000,
    /*
     * mc_host_dip
     */
    0xE0020202 /** 224.2.2.2 */ ,
    /*
     * host_sip
     */
    0xc0800101 /** 192.128.1.1 */ ,
    /*
     * vrf
     */
    15,
    /*
     * fec_id
     */
    0x8001,
    /*
     * IPv6 DIP
     */
    {0xFF, 0xFF, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x11, 0x11},
    /*
     * IPv6 SIP
     */
    {0, 0, 0, 0x11, 0, 0, 0, 0, 0, 0, 0, 0, 0x22, 0x22, 0x11, 0x11},
    /*
     * Compressed SIP
     */
    123
};


/** A structure describing an IPMC entry. */
struct l3_ipmc_ipv4 {
    int vrf;
    uint32 dip;
    uint32 dip_mask;
    uint32 sip;
    uint32 sip_mask;
    int rif;
    int destination;
};

/* A flag to indicate external forwarding enabled or not on JR2 */
int kbp_fwd_enable_jr2 = 0;
/** Indicate whether the entry which is being added is a default one. */
int entry_is_default = 0;

/*
 * Utilities APIs
 */

/** Initialization function for the l3_ipmc_ipv4 structure. */
void l3_ipmc_ipv4_init(
    l3_ipmc_ipv4 * ipmc_entry,
    int vrf,
    uint32 dip,
    uint32 dip_mask,
    uint32 sip,
    uint32 sip_mask,
    int rif,
    int destination)
{
    ipmc_entry->vrf = vrf;
    ipmc_entry->dip = dip;
    ipmc_entry->dip_mask = dip_mask;
    ipmc_entry->sip = sip;
    ipmc_entry->sip_mask = sip_mask;
    ipmc_entry->rif = rif;
    ipmc_entry->destination = destination;
}

/*
 * Add an IPv4 entry
 * - mc_ip_addr - the MC group DIP
 * - mc_ip_mask - the MC group DIP mask
 * - s_ip_addr - the SIP
 * - s_ip_mask - SIP mask
 * - eth_rif_id - the ETH RIF
 * - vrf -VRF
 * - mc_group - the MC group
 */
int
add_ipv4_ipmc(
    int unit,
    bcm_ip_t mc_ip_addr,
    bcm_ip_t mc_ip_mask,
    bcm_ip_t s_ip_addr,
    bcm_ip_t s_ip_mask,
    int eth_rif_id,
    int vrf,
    bcm_multicast_t mc_group,
    int fec,
    uint32 flags)
{
    int IPMC_NOF_ETH_RIFS = 0x1000;
    int rv = BCM_E_NONE;
    bcm_ipmc_addr_t data;
    char *proc_name;

    proc_name = "add_ipv4_ipmc";
    bcm_ipmc_addr_t_init(&data);
    data.mc_ip_addr = mc_ip_addr;
    data.mc_ip_mask = mc_ip_mask;
    data.s_ip_addr = s_ip_addr;
    data.s_ip_mask = s_ip_mask;
    if (eth_rif_id < IPMC_NOF_ETH_RIFS)
    {
        data.vid = eth_rif_id;
    }
    else
    {
        data.ing_intf = eth_rif_id;
    }

    data.vrf = vrf;
    data.l3a_intf = fec;
    data.group = mc_group;
    data.flags = flags;
    if (kbp_fwd_enable_jr2)
    {
        /* JR2 requires flag BCM_IPMC_TCAM if external forwarding enabled */
        data.flags |= BCM_IPMC_TCAM;
    }
    printf("%s(): Enter. data.l3a_intf 0x%08X, data.group 0x%08X \r\n",proc_name,data.l3a_intf,data.group);
    /*
     * Creates the entry 
     */
    rv = bcm_ipmc_add(unit, &data);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_ipmc_add \n",proc_name);
        return rv;
    }

    return rv;
}

/*
 * Add an IPv4 raw entry. Entry value is 'fec'
 * - mc_ip_addr - the MC group DIP
 * - mc_ip_mask - the MC group DIP mask
 * - s_ip_addr - the SIP
 * - s_ip_mask - SIP mask
 * - eth_rif_id - the ETH RIF
 * - vrf -VRF
 * - mc_group - the MC group
 *
 * Note:
 *   Raw access is forced via the 'BCM_IPMC_RAW_ENTRY' flag.
 */
int
add_ipv4_ipmc_raw(
    int unit,
    bcm_ip_t mc_ip_addr,
    bcm_ip_t mc_ip_mask,
    bcm_ip_t s_ip_addr,
    bcm_ip_t s_ip_mask,
    bcm_vrf_t eth_rif_id,
    int vrf,
    bcm_multicast_t mc_group,
    int fec)
{

    int rv = BCM_E_NONE;
    bcm_ipmc_addr_t data;
    char *proc_name;

    proc_name = "add_ipv4_ipmc_raw";
    bcm_ipmc_addr_t_init(&data);
    data.mc_ip_addr = mc_ip_addr;
    data.mc_ip_mask = mc_ip_mask;
    data.s_ip_addr = s_ip_addr;
    data.s_ip_mask = s_ip_mask;
    data.vid = eth_rif_id;
    data.vrf = vrf;
    data.l3a_intf = fec;
    data.flags |=  BCM_IPMC_RAW_ENTRY;
    if (kbp_fwd_enable_jr2)
    {
        /* JR2 requires flag BCM_IPMC_TCAM if external forwarding enabled */
        data.flags |= BCM_IPMC_TCAM;
    }
    data.group = mc_group;
    printf("%s(): Enter. data.l3a_intf 0x%08X, data.group 0x%08X \r\n",proc_name,data.l3a_intf,data.group);
    /*
     * Creates the entry 
     */
    rv = bcm_ipmc_add(unit, &data);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_ipmc_add \n",proc_name);
        return rv;
    }

    return rv;
}

/*
 * Add an IPv4 bridge entry
 * - mc_ip_addr - the MC group DIP
 * - mc_ip_mask - the MC group DIP mask
 * - s_ip_addr - the SIP
 * - s_ip_mask - SIP mask
 * - vlan - the vlan ID
 * - fwd_id -forwarding Id
 * - mc_group - the MC group
 * - fec - the fec ID
 * - flags - control flags for the entry
 */
int
add_ipv4_ipmc_bridge(
    int unit,
    bcm_ip_t mc_ip_addr,
    bcm_ip_t mc_ip_mask,
    bcm_ip_t s_ip_addr,
    bcm_ip_t s_ip_mask,
    int vlan,
    int fwd_id,
    bcm_multicast_t mc_group,
    int fec,
    uint32 flags)
{
    int rv = BCM_E_NONE;
    bcm_ipmc_addr_t data;
    char *proc_name;

    proc_name = "add_ipv4_ipmc";
    bcm_ipmc_addr_t_init(&data);
    data.mc_ip_addr = mc_ip_addr;
    data.mc_ip_mask = mc_ip_mask;
    data.s_ip_addr = s_ip_addr;
    data.s_ip_mask = s_ip_mask;
    data.vid = fwd_id;

    data.l3a_intf = fec;
    data.group = mc_group;
    data.flags = flags | BCM_IPMC_L2;
    printf("%s(): Enter. data.l3a_intf 0x%08X, data.group 0x%08X \r\n",proc_name,data.l3a_intf, data.group);
    /*
     * Creates the entry
     */
    rv = bcm_ipmc_add(unit, &data);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_ipmc_add \n",proc_name);
        return rv;
    }

    return rv;
}

/*
 * Add an IPv6 bridge entry
 * - mc_ip6_addr - the MC group DIP
 * - mc_ip6_mask - the MC group DIP mask
 * - s_ip6_addr - the SIP
 * - s_ip6_mask - SIP mask
 * - vlan - the vlan ID
 * - fwd_id -forwarding Id
 * - mc_group - the MC group
 * - fec - the fec ID
 * - flags - control flags for the entry
 */
int
add_ipv6_ipmc_bridge(
    int unit,
    bcm_ip6_t mc_ip6_addr,
    bcm_ip6_t mc_ip6_mask,
    bcm_ip6_t s_ip6_addr,
    bcm_ip6_t s_ip6_mask,
    int vlan,
    int fwd_id,
    int compressed_sip,
    bcm_multicast_t mc_group,
    int fec,
    uint32 flags)
{
    int rv = BCM_E_NONE;
    bcm_ipmc_addr_t data;
    char *proc_name;

    proc_name = "add_ipv6_ipmc";
    bcm_ipmc_addr_t_init(&data);
    data.mc_ip6_addr = mc_ip6_addr;
    data.mc_ip6_mask = mc_ip6_mask;
    data.s_ip6_addr = s_ip6_addr;
    data.s_ip6_mask = s_ip6_mask;
    data.vid = fwd_id;
    data.mod_id = vlan;
    data.s_ip_addr = compressed_sip;
    if (compressed_sip)
    {
        data.s_ip_mask = 0xFFFF;
    }

    data.l3a_intf = fec;
    data.group = mc_group;
    data.flags = flags | BCM_IPMC_L2 | BCM_IPMC_IP6;
    printf("%s(): Enter. data.l3a_intf 0x%08X, data.group 0x%08X \r\n",proc_name,data.l3a_intf, data.group);
    /*
     * Creates the entry
     */
    rv = bcm_ipmc_add(unit, &data);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_ipmc_add \n",proc_name);
        return rv;
    }
    /** Call API to define the cascaded lookup - compressed SIP result */
    if (compressed_sip)
    {
        bcm_ipmc_addr_t config;
        bcm_ipmc_addr_t_init(&config);
        config.s_ip_addr = compressed_sip;
        config.s_ip6_addr = s_ip6_addr;
        config.s_ip6_mask = s_ip6_mask;
        config.flags = BCM_IPMC_L2;
        rv = bcm_ipmc_config_add(unit, &config);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_ipmc_config_add \n");
            return rv;
        }
    }
    return rv;
}

/*
 * Add an IPv6 entry
 * - mc_ip6_addr - the MC group DIP
 * - mc_ip6_mask - the MC group DIP mask
 * - s_ip6_addr - the SIP
 * - s_ip6_mask - SIP mask
 * - eth_rif_id - the ETH RIF
 * - vrf -VRF
 * - mc_group - the MC group
 */
int
add_ipv6_ipmc(
    int unit,
    bcm_ip6_t mc_ip_addr,
    bcm_ip6_t mc_ip_mask,
    bcm_ip6_t s_ip_addr,
    bcm_ip6_t s_ip_mask,
    bcm_vrf_t eth_rif_id,
    int vrf,
    uint16 compressed_sip,
    uint16 compressed_sip_mask,
    bcm_multicast_t mc_group,
    int fec,
    uint32 flags)
{

    int rv = BCM_E_NONE;
    bcm_ipmc_addr_t data;

    if (is_device_or_above(unit, JERICHO2))
    {
        if ((*dnxc_data_get(unit, "elk", "application", "ipv6", NULL)))
        {
            /* In Jericho 2 with KBP only TCAM mode is supported for IPv6 MC */
            flags |= BCM_IPMC_TCAM;
        }
    }
    else
    {
        flags |= BCM_IPMC_SOURCE_PORT_NOCHECK;
    }

    bcm_ipmc_addr_t_init(&data);
    data.mc_ip6_addr = mc_ip_addr;
    data.mc_ip6_mask = mc_ip_mask;
    data.s_ip6_addr = s_ip_addr;
    data.s_ip6_mask = s_ip_mask;
    data.vid = eth_rif_id;
    data.vrf = vrf;
    data.s_ip_addr = compressed_sip;
    data.s_ip_mask = compressed_sip_mask;

    data.l3a_intf = fec;
    data.group = mc_group;
    data.flags = BCM_IPMC_IP6 | flags;

    /*
     * Creates the entry
     */
    rv = bcm_ipmc_add(unit, &data);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_ipmc_add \n");
        return rv;
    }

    if (compressed_sip && is_device_or_above(unit, JERICHO2))
    {
        bcm_ipmc_addr_t config;
        bcm_ipmc_addr_t_init(&config);
        config.s_ip_addr = compressed_sip;
        config.s_ip6_addr = s_ip_addr;
        config.s_ip6_mask = s_ip_mask;
        config.vid = eth_rif_id;
        rv = bcm_ipmc_config_add(unit, &config);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_ipmc_config_add \n");
            return rv;
        }

    }
    return rv;
}

/*
 * Add an IPv4 entry that results to a port.
 * This is valid for EM and TCAM entries.
 * - mc_ip_addr - the MC group DIP
 * - mc_ip_mask - the MC group DIP mask
 * - s_ip_addr - the SIP
 * - s_ip_mask - SIP mask
 * - eth_rif_id - the ETH RIF
 * - vrf -VRF
 * - port - destination port
 */
int
add_ipv4_ipmc_to_port(
    int unit,
    bcm_ip_t mc_ip_addr,
    bcm_ip_t mc_ip_mask,
    bcm_ip_t s_ip_addr,
    bcm_ip_t s_ip_mask,
    bcm_vrf_t eth_rif_id,
    int vrf,
    int port)
{

    int rv = BCM_E_NONE;
    bcm_ipmc_addr_t data;
    char *proc_name;

    proc_name = "add_ipv4_ipmc_to_port";
    bcm_ipmc_addr_t_init(&data);
    data.mc_ip_addr = mc_ip_addr;
    data.mc_ip_mask = mc_ip_mask;
    data.s_ip_addr = s_ip_addr;
    data.s_ip_mask = s_ip_mask;
    data.vid = eth_rif_id;
    data.vrf = vrf;
    data.port_tgid = port;
    printf("%s(): Enter. data.l3a_intf 0x%08X, data.group 0x%08X data.port_tgid 0x%08X\r\n",proc_name,data.l3a_intf,data.group,data.port_tgid);
    /*
     * Creates the entry 
     */
    rv = bcm_ipmc_add(unit, &data);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_ipmc_add \n");
        return rv;
    }

    return rv;
}

/*
 * Create a single MC replication
 * replications - pointer to a replication to create
 * port - the port that is assign to this replication.
 */
int
set_multicast_replication(
    bcm_multicast_replication_t * replications,
    bcm_gport_t port,
    int encap)
{
    bcm_multicast_replication_t_init(replications);
    replications->encap1 = encap;
    replications->port = port;
    return BCM_E_NONE;
}

/*
 * Create an MC group
 * - arp_id - ARP-ID
 * - replications - an array of replications.
 * - mc_group - the MC group
 */
int
create_multicast(
    int unit,
    bcm_multicast_replication_t * replications,
    int nof_replications,
    bcm_multicast_t mc_group)
{
    int rv = BCM_E_NONE;
    int flags = BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID;

    rv = bcm_multicast_create(unit, flags, &mc_group);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }

    rv = bcm_multicast_add(unit, mc_group, BCM_MULTICAST_INGRESS_GROUP, nof_replications, replications);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_multicast_ingress_add\n");
        return rv;
    }

    return rv;
}

/**
 * Creates a list of replications which is used to create the MC group.
 *   - mc_group - Multicast group ID
 *   - ports - an array of out_ports to which the replications will point to.
 */
int
create_multicast_group_and_replications(
    int unit,
    int mc_group,
    int *ports)
{
    int rv = BCM_E_NONE;
    int idx;
    bcm_multicast_replication_t replications[cint_ipmc_info.nof_replications];
    int intf_out = cint_ipmc_info.intf_out;

    /*
     * 4. Create a multicast replications
     */
    for (idx = 0; idx < cint_ipmc_info.nof_replications; idx++)
    {
        /*
         * 4.1 Create an L3 interface
         */
        /*
         * in case intf_out == eth_rif_id it should be already created
         */
        if (intf_out != cint_ipmc_info.intf_in)
        {
            rv = intf_eth_rif_create(unit, intf_out, cint_ipmc_info.intf_out_mac_address);
            if (rv != BCM_E_NONE)
            {
                printf("intf_eth_rif_create\n");
                return rv;
            }
        }

        /*
         * 4.2 Set the replication.
         */
        set_multicast_replication(&replications[idx], ports[idx], intf_out);

        /*
         * 4.3 Set some changes for the next replication.
         */
        intf_out++;
        cint_ipmc_info.intf_out_mac_address[5]++;
    }

    /*
     * 5. Create a multicast group
     */
    rv = create_multicast(unit, replications, cint_ipmc_info.nof_replications, mc_group);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function create_multicast, \n");
        return rv;
    }
    return rv;
}

/******* Run example ******/
/*
 * Note:
 *   If 'do_raw' is non zero then loading of LPM table is
 *   done using a 'raw' value (flag BCM_IPMC_RAW_ENTRY) rather
 *   than an encoded value.
 */
int
basic_ipmc_example_inner(
    int unit,
    int vid_in,
    int in_port,
    int out_port1,
    int out_port2,
    int is_fallback_to_bridge,
    int do_raw)
{
    int rv = BCM_E_NONE;
    int fec_id;
    cint_ipmc_info.intf_in = vid_in;
    cint_ipmc_info.vrf = vid_in;
    cint_ipmc_info.in_port = in_port;
    uint32 dip_mask = 0xFFFFFFFF;
    uint32 sip_mask = 0x0;
    uint32 sip_part_mask = 0xFFFF0000;
    l3_ingress_intf ingr_itf;
    char *proc_name;
    char *err_proc_name;
    int mc_id_kaps_prefix;
    uint32 flags = 0;
    uint32 default_flags = 0;

    proc_name = "basic_ipmc_example_inner";
    /*
     * These are the 'prefix' bits which are added to field of type MC_ID_KAPS.
     * This variable is used for the 'raw' FEC testing for LPM table. In that
     * case, the MC 'group' needs to be 'encoded' by adding the prefix.
     */
    mc_id_kaps_prefix = 0x0E0000;
    l3_ingress_intf_init(&ingr_itf);
    cint_ipmc_info.out_ports[0] = out_port1;
    cint_ipmc_info.out_ports[1] = out_port2;

    if (is_fallback_to_bridge)
    {
        /*
         * make the outrif equal to inrif
         */
        cint_ipmc_info.intf_out = cint_ipmc_info.intf_in;
    }

    /*
     * 1. Set In-Port to In ETh-RIF
     * 2. Set Out-Port
     */
    rv = in_port_intf_set(unit, cint_ipmc_info.in_port, cint_ipmc_info.intf_in);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set intf_in\n",proc_name);
        return rv;
    }

    rv = out_port_set(unit, out_port1);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set\n",proc_name);
        return rv;
    }

    rv = out_port_set(unit, out_port2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set\n",proc_name);
        return rv;
    }

    /*
     * 2. Create ETH-RIF and set its properties
     */
    rv = intf_eth_rif_create(unit, cint_ipmc_info.intf_in, cint_ipmc_info.intf_in_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in\n",proc_name);
        return rv;
    }
    /*
     * 3. Set Incoming ETH-RIF properties
     */
    ingr_itf.intf_id = cint_ipmc_info.intf_in;
    ingr_itf.vrf = cint_ipmc_info.vrf;
    rv = intf_ingress_rif_set(unit, &ingr_itf);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
        return rv;
    }
    /*
     * 4. Create a multicast group and its replications
     */
    rv = create_multicast_group_and_replications(unit, cint_ipmc_info.mc_group, cint_ipmc_info.out_ports);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create_multicast_group_and_replications\n",proc_name);
        return rv;
    }
    /*
     * 5. Add IPv4 IPMC entry
     */
    if (entry_is_default == 1)
    {
        rv = create_ipv4_default_mc_entries(unit);
    }
    else
    {
        if (is_device_or_above(unit, JERICHO2)) {
            flags = BCM_IPMC_TCAM;
        }
        if (!(soc_property_get(unit, "ext_ipv4_fwd_enable", 0) || soc_property_get(unit, "ext_ipv6_fwd_enable", 0)))
        {
            rv = add_ipv4_ipmc(unit, cint_ipmc_info.mc_host_dip, dip_mask, cint_ipmc_info.host_sip, sip_mask,
                               cint_ipmc_info.intf_in, cint_ipmc_info.vrf, cint_ipmc_info.mc_group, 0, 0);
            if (rv != BCM_E_NONE)
            {
                printf("%s(): Error, in function add_ipv4_ipmc to LEM table, \n",proc_name);
                return rv;
            }
            /*
             * Note that 'raw' is only valid for LPM table.
             */
            if (do_raw)
            {
                rv = add_ipv4_ipmc_raw(unit, cint_ipmc_info.mc_host_dip + 1, dip_mask, cint_ipmc_info.host_sip, sip_part_mask, 0,
                               cint_ipmc_info.vrf, 0, cint_ipmc_info.mc_group | mc_id_kaps_prefix);
                err_proc_name = "add_ipv4_ipmc_raw";
            }
            else
            {
                rv = add_ipv4_ipmc(unit, cint_ipmc_info.mc_host_dip + 1, dip_mask, cint_ipmc_info.host_sip, sip_part_mask, 0,
                               cint_ipmc_info.vrf, cint_ipmc_info.mc_group, 0, 0);
                err_proc_name = "add_ipv4_ipmc";
            }
            if (rv != BCM_E_NONE)
            {
                printf("%s(): Error, in function %s to LPM table, \n",proc_name,err_proc_name);
                return rv;
            }
        }

        rv = add_ipv4_ipmc(unit, cint_ipmc_info.mc_host_dip + 2, dip_mask, cint_ipmc_info.host_sip, sip_part_mask,
                           cint_ipmc_info.intf_in, cint_ipmc_info.vrf, cint_ipmc_info.mc_group, 0, flags);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in function add_ipv4_ipmc to TCAM table, \n",proc_name);
            return rv;
        }
    }
    return rv;
}

/** Create IPMC entries using the BCM_IPMC_DEFAULT_ENTRY flag. */
int
create_ipv4_default_mc_entries(
        int unit)
{
    int rv = 0;
    int fec_id;
    uint32 flags = 0;
    uint32 default_flags = 0;
    uint32 dip_mask = 0xFFFFFFFF;
    uint32 sip_part_mask = 0xFFFF0000;

    bcm_gport_t gport;

    get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &fec_id);
    BCM_GPORT_MCAST_SET(gport, cint_ipmc_info.mc_group);
    rv = l3__egress_only_fec__create(unit, fec_id, cint_ipmc_info.intf_out, 0, gport, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only: fec = %d, intf_out = %d - err_id = %d\n", fec_id, cint_ipmc_info.intf_out, rv);
        return rv;
    }
    rv = l3__egress_only_fec__create(unit, fec_id + 1, cint_ipmc_info.intf_out, 0, gport, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only: fec = %d, intf_out = %d - err_id = %d\n", fec_id+1, cint_ipmc_info.intf_out, rv);
        return rv;
    }

    if (is_device_or_above(unit, JERICHO2)) {
        default_flags = BCM_IPMC_DEFAULT_ENTRY;
    }
    rv = add_ipv4_ipmc(unit, cint_ipmc_info.mc_host_dip, dip_mask, cint_ipmc_info.host_sip, 0,
                       0xCC, cint_ipmc_info.vrf, 0, fec_id + 1, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_ipv4_ipmc to LEM table, \n");
        return rv;
    }
    rv = add_ipv4_ipmc(unit, cint_ipmc_info.mc_host_dip, dip_mask, cint_ipmc_info.host_sip, dip_mask, 0xCC,
                   cint_ipmc_info.vrf, 0, fec_id, default_flags);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_ipv4_ipmc to LPM table - default, \n");
        return rv;
    }

    return rv;
}

/*
 * See AT_Dnx_Cint_l3_ipmc_basic
 */
int
basic_ipmc_example(
    int unit,
    int vid_in,
    int in_port,
    int out_port,
    int out_port2)
{
    return basic_ipmc_example_inner(unit, vid_in, in_port, out_port, out_port2, 0, 0);
}
/*
 * See AT_Dnx_Cint_l3_ipmc_basic_raw
 */
int
basic_ipmc_example_raw(
    int unit,
    int vid_in,
    int in_port,
    int out_port,
    int out_port2)
{
    return basic_ipmc_example_inner(unit, vid_in, in_port, out_port, out_port2, 0, 1);
}

/*
 * See AT_Dnx_Cint_l3_ipmc_v6_basic
 */
int
basic_ipmc_v6_example(
    int unit,
    int vid_in,
    int in_port,
    int out_port1,
    int out_port2)
{

    int rv = BCM_E_NONE;
    cint_ipmc_info.intf_in = vid_in;
    cint_ipmc_info.vrf = vid_in;
    cint_ipmc_info.in_port = in_port;

    l3_ingress_intf ingr_itf;
    l3_ingress_intf_init(&ingr_itf);

    cint_ipmc_info.out_ports[0] = out_port1;
    cint_ipmc_info.out_ports[1] = out_port2;

    /*
     * 1. Set In-Port to In ETh-RIF
     * 2. Set Out-Port
     */
    rv = in_port_intf_set(unit, cint_ipmc_info.in_port, cint_ipmc_info.intf_in);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set intf_in\n");
        return rv;
    }

    rv = out_port_set(unit, cint_ipmc_info.out_ports[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_set\n");
        return rv;
    }

    rv = out_port_set(unit, cint_ipmc_info.out_ports[1]);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_set\n");
        return rv;
    }

    /*
     * 2. Create ETH-RIF and set its properties
     */
    rv = intf_eth_rif_create(unit, cint_ipmc_info.intf_in, cint_ipmc_info.intf_in_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_in\n");
        return rv;
    }

    /*
     * 3. Set Incoming ETH-RIF properties
     */
    ingr_itf.intf_id = cint_ipmc_info.intf_in;
    ingr_itf.vrf = cint_ipmc_info.vrf;
    rv = intf_ingress_rif_set(unit, &ingr_itf);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_out\n");
        return rv;
    }

    /*
     * 4. Create a multicast group and its replications
     */
    rv = create_multicast_group_and_replications(unit, cint_ipmc_info.mc_group, cint_ipmc_info.out_ports);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create_multicast_group_and_replications\n");
        return rv;
    }
    /*
     * 5. Add IPv6 IPMC entry
     */
    if (entry_is_default)
    {
        rv = create_ipv6_default_mc_entries(unit);
    }
    else
    {
        rv = create_ipv6_mc_entries(unit);
    }
    if (rv != BCM_E_NONE)
    {
        printf("Error, create_ipv6_mc_entries\n");
        return rv;
    }

    return rv;
}

/** Create IPMC entries using the BCM_IPMC_DEFAULT_ENTRY flag. */
int
create_ipv6_default_mc_entries(
    int unit)
{
    int rv = BCM_E_NONE;
    int fec_id;
    int intf_out = cint_ipmc_info.intf_out;
    bcm_ip6_t no_sip = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    bcm_ip6_t full_mask = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    bcm_ip6_t part_mask = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0, 0, 0, 0, 0, 0, 0, 0};
    bcm_gport_t gport;

    get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &fec_id);
    fec_id += 2;
    BCM_GPORT_MCAST_SET(gport, cint_ipmc_info.mc_group);
    rv = l3__egress_only_fec__create(unit, fec_id, intf_out, 0, gport, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only: fec = %d, intf_out = %d - err_id = %d\n", fec_id, intf_out, rv);
        return rv;
    }
    rv = l3__egress_only_fec__create(unit, fec_id + 1, intf_out, 0, gport, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only: fec = %d, intf_out = %d - err_id = %d\n", fec_id+1, intf_out, rv);
        return rv;
    }

    rv = add_ipv6_ipmc(unit, cint_ipmc_info.ipv6_dip_addr, full_mask, cint_ipmc_info.ipv6_sip_addr, full_mask,
                       0xCC, cint_ipmc_info.vrf, cint_ipmc_info.compressed_sip, 0xFFFF, 0, fec_id, BCM_IPMC_DEFAULT_ENTRY);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_ipv6_ipmc to LPM table, \n");
        return rv;
    }
    rv = add_ipv6_ipmc(unit, cint_ipmc_info.ipv6_dip_addr, full_mask, no_sip, no_sip, 0xCC, cint_ipmc_info.vrf, 0, 0, 0, fec_id + 1, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_ipv6_ipmc to LEM table, \n");
        return rv;
    }

    return rv;
}

int
create_ipv6_mc_entries(
    int unit)
{
    int rv = BCM_E_NONE;
    uint32 flags = 0;
    bcm_ip6_t no_sip = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    bcm_ip6_t full_mask = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    bcm_ip6_t part_mask = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0, 0, 0, 0, 0, 0, 0, 0};

    if (is_device_or_above(unit, JERICHO2) && (soc_property_get(unit, "ext_ipv6_fwd_enable", 0)))
    {
        flags = BCM_IPMC_TCAM;
    }
    if (!(soc_property_get(unit, "ext_ipv4_fwd_enable", 0) || soc_property_get(unit, "ext_ipv6_fwd_enable", 0)))
    {
        rv = add_ipv6_ipmc(unit, cint_ipmc_info.ipv6_dip_addr, full_mask, no_sip, no_sip,
                           cint_ipmc_info.intf_in, cint_ipmc_info.vrf, 0, 0, cint_ipmc_info.mc_group, 0, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in function add_ipv6_ipmc to LEM table, \n");
            return rv;
        }

        cint_ipmc_info.ipv6_dip_addr[15] ++;
        rv = add_ipv6_ipmc(unit, cint_ipmc_info.ipv6_dip_addr, full_mask, cint_ipmc_info.ipv6_sip_addr, part_mask, 0,
                           cint_ipmc_info.vrf, cint_ipmc_info.compressed_sip, 0xFFFF, cint_ipmc_info.mc_group, 0, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in function add_ipv6_ipmc to LPM table, \n");
            return rv;
        }
    }
    else
    {
        cint_ipmc_info.ipv6_dip_addr[15] ++;
    }
    cint_ipmc_info.ipv6_dip_addr[15] ++;
    rv = add_ipv6_ipmc(unit, cint_ipmc_info.ipv6_dip_addr, full_mask, cint_ipmc_info.ipv6_sip_addr, part_mask,
                       cint_ipmc_info.intf_in, cint_ipmc_info.vrf, 0, 0, cint_ipmc_info.mc_group, 0, flags);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_ipv6_ipmc to LPM table, \n");
        return rv;
    }
    return rv;
}

/**
 * This function creates two IPV6 MC entries in LPM table + three cascaded entries, one of which is a default one.
 */
int create_ipv6_mc_cascaded_entries(
    int unit)
{
    int rv = BCM_E_NONE;
    bcm_ip6_t no_sip = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    bcm_ip6_t full_mask = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    bcm_ip6_t part_mask = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0, 0, 0, 0, 0, 0, 0, 0};
    cint_ipmc_info.compressed_sip = 0x1234;
    rv = add_ipv6_ipmc(unit, cint_ipmc_info.ipv6_dip_addr, full_mask, cint_ipmc_info.ipv6_sip_addr, full_mask, 0xCC,
                       cint_ipmc_info.vrf, cint_ipmc_info.compressed_sip, 0xFFFF, cint_ipmc_info.mc_group, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_ipv6_ipmc to LPM table, \n");
        return rv;
    }

    cint_ipmc_info.ipv6_dip_addr[15]++;
    cint_ipmc_info.compressed_sip = 0x4567;
    rv = add_ipv6_ipmc(unit, cint_ipmc_info.ipv6_dip_addr, full_mask, cint_ipmc_info.ipv6_sip_addr, part_mask, 0,
                       cint_ipmc_info.vrf, cint_ipmc_info.compressed_sip, 0xFF00, cint_ipmc_info.mc_group, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_ipv6_ipmc to LPM table, \n");
        return rv;
    }
    bcm_ipmc_addr_t config;
    bcm_ipmc_addr_t_init(&config);
    config.s_ip_addr = cint_ipmc_info.compressed_sip;
    config.s_ip6_addr = no_sip;
    config.s_ip6_mask = no_sip;
    config.vid = 0;
    rv = bcm_ipmc_config_add(unit, &config);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_ipmc_config_add \n");
        return rv;
    }
    return rv;
}

