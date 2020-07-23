/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~BFD test~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_sand_bfd_ipv6.c
 * Purpose: Example of using BFD IPv6 APIs that tests for BFD connection to bring sessions UP.
 *
 * Soc Properties configuration (only for JERICHO or below):
 *     mcs_load_uc0=bfd_bhh to configure to load the mcs image.
 *     custom_feature_bfd_ipv6_mode=2 to configure to support the BFD over IPv6 by uKernel
 *     bfd_simple_password_keys=8 to configure the simple password auth key number.
 *     bfd_sha1_keys=8 to configure the sha1 auth key number
 *     port_priorities.BCM88650=8 to configure the number of priorities at egq.
 *     num_queues_pci=32 to configure the ARAD cosq numbers, from 0-32.
 *     num_queues_uc0=8 to configure the ARM core 0 cosq numbers, from 32-40.
 *     ucode_port_204.BCM88650=CPU.32 to configure local port 204 to send the DSP packet.
 *     tm_port_header_type_in_204.BCM88650=INJECTED_2_PP
 *     tm_port_header_type_out_204.BCM88650=CPU to trap the BFD packet to uKernel with the FTMH and PPH header
 *
 * Service Model:
 *     Back-to-back tests using 2 FAPS(A, B):
 *     1) Send the BFD IPv6 packet from port_a of fap_a to port_b of fap_b.
 *     2) Send the BFD IPv6 packet from port_b of fap_b to port_a of fap_a.
 * Service connection:
 *     --------------             -------------
 *     |    fap_a    |<--------->|    fap_b    |
 *     --------------             -------------
 *         port_a                     port_b
 *
 * To run IPV6 test:

 
 * for JERICHO devices or below:
 *     BCM> cint examples/sand/utility/cint_sand_utils_global.c
 *     BCM> cint examples/sand/utility/cint_sand_utils_l3.c
 *     BCM> cint examples/dpp/utility/cint_utils_l3.c
 *     BCM> cint examples/dpp/cint_ip_route.c
 *     BCM> cint examples/dpp/cint_bfd.c
 *     BCM> cint examples/dpp/cint_bfd_ipv6.c
 *     BCM> cint examples/dpp/cint_field_bfd_ipv6_single_hop.c
 *     BCM> cint
 *     print bfd_ipv6_single_hop_field_action(unit);
 *     print bfd_ipv6_service_init(unit, in_port, out_port, type);
 *     print bfd_ipv6_run_with_defaults(unit, endpoint_id, is_acc, is_single_hop, auth_type, auth_index, punt_rate);
 *
 * for JERICHO2 devices or above:
 * 
 *     BCM> #for create_l3_intf_s and create_l3_egress_s
 *     BCM> cint examples/sand/utility/cint_sand_utils_l3.c
 *     BCM> cint examples/sand/utility/cint_sand_utils_vlan.c
 *     BCM> cint examples/sand/utility/cint_sand_utils_global.c
 *     BCM> cint examples/sand/cint_ip_route_basic.c
 *     BCM> cint examples/sand/cint_sand_bfd_ipv6.c
 *     BCM> cint
 *     print bfd_ipv6_service_init(unit, in_port, out_port, type);
 *     print bfd_ipv6_run_with_defaults(unit, endpoint_id, is_acc, is_single_hop, auth_type, auth_index, punt_rate);
 *
 *
 * comments for JERICHO or below:
 * 1) Normally, bfd_ipv6_trap(unit) is not used.
 *     But if the bfd_ipv6_single_hop_field_action(unit) not works to trap the BFD packet to uKernel,
 *     we can use the bfd_ipv6_trap(unit) instead to trap all BFD single hop packet to uKernel for
 *     debug purpose.
 * 2) For simple password auth and sha1 auth, you can select the key by key_index.
 *
 * authentication is not supported in JERICHO2 or above
 *
 * look at bfd_ipv6_service_init to see which local discriminator, remote discriminator,
 *    source IP and destination IP values are in use.
 *
 * packet for example:
 *
 *   Send packet from OAMP
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |   +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+   |
 *   |   |      DA                 | SA                    ||   IPv6                                      || UDP  || BFD  ||  |   |
 *   |   |00:00:00:00:CD:1D        |00:0C:00:02:00:00      ||SIP:0000:0000:0000:0000:1234:0000:0000:FF13  ||      ||      ||  |   |
 *   |   |                         |                       ||DIP:0000:0000:0000:0000:0000:FFFF:C080:0101  ||      ||      ||  |   |
 *   |   +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+   |
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *
 *   Trap packet to OAMP
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |   +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+   |
 *   |   |      DA                 | SA                    ||   IPv6                                      || UDP  || BFD  ||  |   |
 *   |   |00:0C:00:02:00:00        |00:00:00:00:CD:1D      ||SIP:0000:0000:0000:0000:0000:FFFF:C080:0101  ||      ||      ||  |   |
 *   |   |                         |                       ||DIP:0000:0000:0000:0000:1234:0000:0000:FF13  ||      ||      ||  |   |
 *   |   +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+   |
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

/**
 * Trap code returned from creating a trap
 * for a non-accelerated endpoint
 */

/** In case of JR2 only, we can decide not to set valid tx_gport */
int use_invalid_tx_gport=0;

/* Endpoint info */
struct dnx_oam__ep_info_s {
    int id;
    int rmep_id;
    bcm_gport_t gport;
    bcm_gport_t mpls_out_gport;
    int out_lif;
    int label;
    int lm_counter_base_id;
    int lm_counter_if;
};

/* Test Type */
int type = 1; /* 1 - Single Tagged; 2 - Double Tagged */

vlan_edit_utils_s g_eve_edit_double_tagged = {3,    /* edit profile */
                                            0,    /* tag format */
                                            8};   /* action ID */

vlan_edit_utils_s g_eve_edit_lag = {4,    /* edit profile */
                                    0,    /* tag format */
                                    9};   /* action ID */
int bfd_in_port[2] = {13,15};
int bfd_out_port[2] = {14,16};
int bfd_remote_port = BCM_GPORT_INVALID;
int use_invalid_tx_gport = 0;
int user_defined_trap_code = 0;
int echo_trap_code;
int bfd_dsp_port = 204;/* the bfd_dsp_port directly connect to the ARM core 0 */
int bfd_encap_id;
int non_bfd_fwd_vrf;
uint32 local_disc;
/* used for dnx tests */
uint32 bfd_local_ipv6_disc = 0;
uint32 remote_disc;
/** Created endpoint information */
dnx_oam__ep_info_s bfd_ep1;

bcm_ip6_t src_ipv6;
bcm_ip6_t dst_ipv6;
/** BFD transmission period */
int bfd_tx_period = 0;

/* int_pri of bfd_ipv6 endpoint */
int int_pri = 0;

/* Trunk info */
int is_trunk = 0;
int member_count=2;
int is_micro_bfd=0;
bcm_trunk_member_t  member[member_count];
bcm_trunk_info_t trunk_info[2];
uint32  trunk_flags = 0;
int trunk_gport[2];
bcm_trunk_t trunk_id[2];
/* BFD event count */
int array_num = bcmBFDEventCount;
uint32 bfd_event_count[array_num] = {0};
/*Echo indication */
int is_bfd_echo=0;
/*Micro BFD*/
int is_micro_bfd=0;
/* On Demond indication */
int is_on_demond = 0;

bcm_field_presel_t  cint_bfd_ipv6_echo_presel_id = 11;

/* context id initialization, will be retrieved from field_presel_fwd_layer_main */
bcm_field_context_t cint_bfd_ipv6_echo_context_id_ipmf1 = BCM_FIELD_CONTEXT_ID_INVALID;

/* simple password auth configuration */
uint8 auth_sp_num = 4;
uint8 auth_sp_len[4] = { 8, 8, 16, 16};
uint8 auth_sp_key[4][16]= {
    {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f},
    {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f},
    {0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f},
    {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f}};

/* sha1 auth configuration */
uint8 auth_sha1_num = 4;
uint32 auth_sha1_seq[4] = {0x11223344, 0x55667788, 0x99aabbcc, 0xddeeff00 };
uint8 auth_sha1_key[4][20]= {
    {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13},
    {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23},
    {0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33},
    {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0x40, 0x41, 0x42, 0x43}};

/* When sending BFD packet to remote port,
 * If you want the PPH to be stamped correctly with the forwarding
 * header offset, set this value to 1 from your cint.
 */
uint8 bfd_fwd_off_en = 0;

/*
 * Used for BFD trap and snoop configuration.
 * If set to 1, bfd_trap_code will be set as
 * remote_gport.
 */
int use_trap_with_snoop = 0;
/*
 * Valid BFD trap ID associated
 * with a snoop command.
 */
int bfd_trap_code = 0;
int trap_strength = 7;
int snoop_strength = 0;

/**
 * Destination: FEC
 * Note: there is no need to define
 * an actual FEC, but make sure that
 * you do not plan to use a FEC with
 * this number elsewhere
 */
uint32 my_bfd_dip_destination = 0xabcd;

/**
* When set to "1" the MEPs will be created with RX statistics.
* RX packets are the packets that are received in the OAMP.
*/
int is_rx_statisitcs_enabled=0;
/**
* When set to "1" the MEPs will be created with TX statistics
* TX packets are the packets transmitted from the OAMP.
*/
int is_tx_statisitcs_enabled=0;
/**
* When set to "1" allocate 8k counters in database 5 for TX packets.
* When set to "2" allocate 8k counters in database 6 for RX packets.
* When set to "3" allocate 8k counters in database 5 for TX packets and 8k counters in database 6 for RX packets.
*/
int enable_oamp_counter_allocation=0;

int last_trap_code_created = 0;

/**
 * JR2 and up - since SBFD packets are generated using
 * the OAMP-PE, combining with IPv6 is possible
 */
uint8 is_s_bfd_ipv6_init_ep = 0;

/** Inject BFD over IPv6 PDUs from the OAMP to an MPLS outLIF (as
 *  opposed to L2 outLIF in the standard case). Jr2 and
 *  above only. */
int bfd_over_ipv6_transmit_into_mpls =0;

/**
 * When this flag is set, a VXLAN FEC is created and used as the EP's egress_if.
 */
int use_vxlan6_fec = 0;


/** ID of FEC created for this purpose */
bcm_if_t vxlan6_roo_fec;


int bfd_create_trap_from_gport(int unit, bcm_gport_t gport, int* trap_code) {
    bcm_error_t rv;
    bcm_rx_trap_config_t trap_config;

    rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, trap_code);
    if (rv != BCM_E_NONE) {
       printf("(%s) \n",bcm_errmsg(rv));
       return rv;
    }

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP;
    trap_config.dest_port = gport;

    rv = bcm_rx_trap_set(unit, *trap_code, &trap_config);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    last_trap_code_created = *trap_code;
    printf("Trap created trap_code=%d \n", *trap_code);

    return rv;
}

int bfdv6_non_bfd_fwd_add_route(int unit, int out_port) {
    int rv;

    int intf_out = 150;         /* Outgoing packet ETH-RIF */
    int fec = 24575;            /* 24575=24*1024-1 */
    int encap_id = 0x1384;      /* ARP-Link-layer */
    bcm_gport_t gport = 0;
    bcm_mac_t intf_out_mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x03 };    /* my-MAC */
    bcm_ip6_t mask = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
    char *proc_name;
    proc_name = "bfdv6_non_bfd_fwd_add_route";
    l3_ingress_intf ingress_itf;
    l3_ingress_intf_init(&ingress_itf);

    if (is_device_or_above(unit,JERICHO2)) {
        encap_id = *dnxc_data_get(unit, "l3", "rif", "nof_rifs", NULL) + 0x384;      /* ARP-Link-layer */
    }

   /*
     * 1. Set Out-Port default properties - for the non_bfd_forwarding port
     */
    rv = out_port_set(unit, out_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_intf_set out_port = %d, err_id = %d\n", proc_name, out_port, rv);
        return rv;
    }

    rv = intf_eth_rif_create(unit, intf_out, intf_out_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out = %d, err_id = %d\n", proc_name, intf_out, rv);
        return rv;
    }

    /*
     * 2. Create FEC and set its properties
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
     * 3. Add Route entry
     */
    rv = add_route_ipv6(unit, src_ipv6, mask, non_bfd_fwd_vrf, fec);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function add_route_ipv6: vrf = %d, fec = %d, err_id = %d \n", proc_name, non_bfd_fwd_vrf, fec, rv);
        return rv;
    }
    return rv;
}


int bfd_ipv6_non_bfd_fwd_setup(int unit,int in_port,int out_port)
{
    int rv;
    int intf_in = 15;           /* Incoming packet ETH-RIF */
    int intf_out = 150;         /* Outgoing packet ETH-RIF */
    int fec = 24575;            /* 24575=24*1024-1 */
    int vrf = 1;
    int vsi = 0;
    int encap_id = 0x1384;    /* ARP-Link-layer */
    bcm_gport_t gport = 0;
    bcm_mac_t intf_in_mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x02 };     /* my-MAC */
    bcm_mac_t intf_out_mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x03 };    /* my-MAC */
    bcm_mac_t arp_next_hop_mac = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d };        /* next_hop_mac */
    bcm_ip6_t route =
        { 0xfe, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02 };
    bcm_ip6_t host = { 0xfe, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x02, 0x00, 0x15 };
    bcm_ip6_t mask = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0 };
    char *proc_name;
    proc_name = "basic_example_ipv6";
    l3_ingress_intf ingress_itf;
    l3_ingress_intf_init(&ingress_itf);

    if (is_device_or_above(unit,JERICHO2)) {
        encap_id = *dnxc_data_get(unit, "l3", "rif", "nof_rifs", NULL) + 0x384;      /* ARP-Link-layer */
    }

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
     * 6. Add Route entry
     */
    rv = add_route_ipv6(unit, route, mask, vrf, fec);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function add_route_ipv6: vrf = %d, fec = %d, err_id = %d \n", proc_name, vrf, fec, rv);
        return rv;
    }

    return rv;
}


/*
 * Add IPV6 Host
 */
int add_ipv6_host(int unit, bcm_ip6_t *addr, int vrf, int fec, int intf) {
  int rc;
  bcm_l3_host_t l3host;

  bcm_l3_host_t_init(l3host);

  l3host.l3a_flags =  BCM_L3_IP6;
  sal_memcpy(&(l3host.l3a_ip6_addr),(addr),16);
  l3host.l3a_vrf = vrf;
  /* dest is FEC + OutLif */
  l3host.l3a_intf = fec; /* fec */
  /* encap_id == 0 for IPv6 host entries in KAPS (Jericho and above) */
  l3host.encap_id = is_device_or_above(unit, JERICHO) ? 0 : intf;
  l3host.l3a_modid = 0;
  l3host.l3a_port_tgid = 0;

  rc = bcm_l3_host_add(unit, l3host);
  if (rc != BCM_E_NONE) {
    printf ("bcm_l3_host_add failed: %d \n", rc);
  }
  return rc;
}


/*
* Init the Local/Remote Discriminator, and Src/Dst IPv6 Address
*/
int bfd_ipv6_basic_param_init(int unit) {
    int rv = BCM_E_NONE;
    int is_qux = 0;
    uint8 is_local_dicr_as_session_id = 0;

    if(is_device_or_above(unit, JERICHO2)) {
        if(bfd_local_ipv6_disc != 0)
        {
            local_disc = bfd_local_ipv6_disc;
        }
        else
        {
            local_disc = 0x2008;
        }
        remote_disc = 0x2004;
        bcm_ip6_t src_ipv6_tmp = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x13};
        bcm_ip6_t dst_ipv6_tmp = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xc0, 0x80, 0x01, 0x01};
        sal_memcpy(src_ipv6, src_ipv6_tmp, sizeof(src_ipv6));
        sal_memcpy(dst_ipv6, dst_ipv6_tmp, sizeof(dst_ipv6));
    } else {
        is_local_dicr_as_session_id = soc_property_get(unit , "bfd_use_local_discriminator_as_session_id", 0);
        is_qumran_ux_only(unit, &is_qux);

        local_disc = 5010;
        if (is_local_dicr_as_session_id && is_qux) {
            local_disc = 200;
        }
        remote_disc = 10;
        bcm_ip6_t src_ipv6_tmp = { 0xfe, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02};
        bcm_ip6_t dst_ipv6_tmp = { 0xfe, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01};
        sal_memcpy(src_ipv6, src_ipv6_tmp, sizeof(src_ipv6));
        sal_memcpy(dst_ipv6, dst_ipv6_tmp, sizeof(dst_ipv6));
    }
    return rv;
}


/*
 *Initialize the BFD application configuration for local or remote device.
 */
int bfd_ipv6_service_init(int unit, int in_port, int in_port_2, int out_port, int out_port_2, int is_trunk, int remote_port) {
    int rv = 0;
    int i;

    bfd_in_port[0] = in_port;
    bfd_out_port[0] = out_port;

    bfd_remote_port = remote_port;

    if (is_trunk) {
        bfd_in_port[1] = in_port_2;
        bfd_out_port[1] = out_port_2;

        for (i = 0; i < 2; i++ ) {
            /* Create Trunk Port */
            rv = bcm_trunk_create(unit, trunk_flags, &trunk_id[i]);
            if (rv != BCM_E_NONE) {
                printf("Error,create trunk id:0x0%x failed $rv\n",trunk_id[i]);
                return rv;
            }

            rv = bcm_trunk_psc_set(unit, trunk_id[i], BCM_TRUNK_PSC_ROUND_ROBIN);
            if (rv != BCM_E_NONE) {
                printf("Error in bcm_trunk_psc_set, trunk id:0x0%x failed $rv\n",trunk_id[i]);
                return rv;
            }

            BCM_GPORT_TRUNK_SET(trunk_gport[i], trunk_id[i]);
            printf("create trunk id:0x0%x, trunk_gport:0x0%x\n",trunk_id[i],trunk_gport[i]);

             /* Add port member into Trunk */
            sal_memset(&trunk_info[i], 0, sizeof(trunk_info[i]));
            sal_memset(member, 0, sizeof(member));
            trunk_info[i].psc= BCM_TRUNK_PSC_ROUND_ROBIN;
            if (!i) {
                BCM_GPORT_SYSTEM_PORT_ID_SET(member[0].gport, bfd_in_port[0]);
                BCM_GPORT_SYSTEM_PORT_ID_SET(member[1].gport, bfd_in_port[1]);
            } else {
                BCM_GPORT_SYSTEM_PORT_ID_SET(member[0].gport, bfd_out_port[0]);
                BCM_GPORT_SYSTEM_PORT_ID_SET(member[1].gport, bfd_out_port[1]);
            }
            rv = bcm_trunk_set(unit, trunk_id[i], &trunk_info[i], member_count, member);
            if (rv != BCM_E_NONE) {
                printf("Error,bcm_trunk_set failed $rv\n");
                return rv;
            }

            printf("configure trunk member for trunk id:0x0%x, trunk_gport:0x0%x\n",trunk_id[i],trunk_gport[i]);
            printf("trunk member gport: 0x0%x, 0x0%x\n",member[0].gport,member[1].gport);

        }
    }
    /*
     * configure the local/remote discriminator and src/dst ipv6 addr
     */
    rv = bfd_ipv6_basic_param_init(unit);
    if (rv != BCM_E_NONE) {
        printf("Error,bfd_ipv6_basic_param_init failed $rv\n");
        return rv;
    }
    return rv;
}

/*
 *creating l3 interface - ingress/egress
 */
int bfd_ipv6_intf_init(int unit, int type, int is_trunk){
    int rv;
    int is_adv_vt;
    int in_vlan = 2;
    int out_vlan = 200;
    int out_vsi = 11;
    int trunk_member_num = 1;
    int i;
    int inner_vlan;
    int profile;
    int action_id;

    if (is_trunk) {
        trunk_member_num = 2;
    }

    /*   Double Tagged   */
    int in_inner_vlan = 100;
    int out_inner_vlan = 400;
    int vrf = 0;
    int ing_intf_in;
    int ing_intf_out;
    int l3_eg_int;
    int fec;
    int encap_id;
    bcm_mac_t mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x02};  /* my-MAC */
    bcm_mac_t next_hop_mac  = {0x00, 0x00, 0x00, 0x01, 0x02, 0x03}; /* next_hop_mac1 */
    bcm_vlan_port_t out_vlan_port[trunk_member_num];
    bcm_vlan_action_set_t action;

    /*  Advanced VLAN Translation Mode or Not  */
    is_adv_vt = soc_property_get(unit , "bcm886xx_vlan_translate_mode",0);

    for ( i = 0; i < trunk_member_num; i++ ) {
        rv = bcm_port_class_set(unit, bfd_out_port[i], bcmPortClassId, bfd_out_port[i]);
        if (rv != BCM_E_NONE) {
            printf("fail set port(%d) class\n", bfd_out_port[i]);
          return rv;
        }

        bcm_vlan_port_t_init(&out_vlan_port[i]);
        out_vlan_port[i].flags |= (BCM_VLAN_PORT_OUTER_VLAN_PRESERVE | BCM_VLAN_PORT_INNER_VLAN_PRESERVE);
        if (type == 1) {
            out_vlan_port[i].criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
        } else if (type == 2) {
            out_vlan_port[i].criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
            out_vlan_port[i].match_inner_vlan = out_inner_vlan;
            out_vlan_port[i].egress_inner_vlan = 55;
        }
        out_vlan_port[i].vsi = out_vsi;
        out_vlan_port[i].port = bfd_out_port[i];
        out_vlan_port[i].match_vlan = out_vsi;
        out_vlan_port[i].egress_vlan = is_device_or_above(unit, JERICHO2) ? 0 : 33;
        rv =  bcm_vlan_port_create(unit, &out_vlan_port[i]);
        if (rv != BCM_E_NONE) {
            printf("fail create port(%d) vlan(%d)\n", bfd_out_port[i], in_vlan);
          return rv;
        }
    }

    /*** create ingress router interface ***/
    rv = vlan__open_vlan_per_mc(unit, in_vlan, 0x1);
    if (rv != BCM_E_NONE) {
	printf("Error, open_vlan=%d, in unit %d \n", in_vlan, unit);
    }

    for ( i = 0; i < trunk_member_num ; i++ ) {
        rv = bcm_vlan_gport_add(unit, in_vlan, bfd_in_port[i], 0);
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
            printf("fail add port(0x%08x) to vlan(%d)\n", bfd_in_port[i], in_vlan);
          return rv;
        }
    }

    create_l3_intf_s intf;
    intf.vsi = in_vlan;
    intf.my_global_mac = mac_address;
    intf.my_lsb_mac = mac_address;
    intf.vrf_valid = 1;
    intf.vrf = vrf;
    intf.mtu_valid = 1;
    intf.mtu = 0;
    intf.mtu_forwarding = 0;

    rv = l3__intf_rif__create(unit, &intf);
    ing_intf_in = intf.rif;
    if (rv != BCM_E_NONE) {
	printf("Error, l3__intf_rif__create\n");
    }

    rv = vlan__open_vlan_per_mc(unit, out_vlan, 0x1);
    if (rv != BCM_E_NONE) {
	printf("Error, open_vlan=%d, in unit %d \n", out_vlan, unit);
    }

    for ( i = 0; i < trunk_member_num ; i++ ) {
        rv = bcm_vlan_gport_add(unit, out_vlan, bfd_out_port[i], 0);
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
            printf("fail add port(0x%08x) to vlan(%d)\n", bfd_out_port[i], out_vlan);
          return rv;
        }
    }

    intf.vsi = out_vsi;
    non_bfd_fwd_vrf = intf.vrf;

    rv = l3__intf_rif__create(unit, &intf);
    ing_intf_out = intf.rif;
    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
    }

    l3_eg_int = ing_intf_out;

    /*** Create egress object1 ***/
    create_l3_egress_s l3eg;
    l3eg.out_tunnel_or_rif = l3_eg_int;
    sal_memcpy(l3eg.next_hop_mac_addr, next_hop_mac, 6);
    l3eg.vlan   = out_vsi;
    l3eg.arp_encap_id = encap_id;
    l3eg.fec_id = fec;
    l3eg.allocation_flags = 0;
    l3eg.out_gport = is_trunk ? trunk_gport[1] : bfd_out_port[0];

    rv = l3__egress__create(unit,&l3eg);
     if (rv != BCM_E_NONE) {
        printf("Error, create egress object, sysport=%d, in unit %d\n", sysport, unit);
    }

    encap_id = l3eg.arp_encap_id;
    fec = l3eg.fec_id;
    bfd_encap_id = encap_id;

    rv = add_ipv6_host(unit, &src_ipv6, vrf, fec, bfd_encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, add ipv6 host, in unit %d\n", unit);
    }

    /* set port translation info*/
    if (is_adv_vt) {
        for ( i = 0; i < trunk_member_num ; i++ ) {
            inner_vlan = (type == 1) ? 0 : out_inner_vlan;
            profile = is_trunk ? g_eve_edit_lag.edit_profile : ((type == 1) ? g_eve_edit_utils.edit_profile: g_eve_edit_double_tagged.edit_profile);
            rv = vlan_port_translation_set(unit, out_vlan, inner_vlan, out_vlan_port[i].vlan_port_id, profile, 0);
            if (rv != BCM_E_NONE) {
                printf("Error, vlan_port_translation_set\n");
                return rv;
            }
        }

        /* Create action IDs */
        action_id = is_trunk ? g_eve_edit_lag.action_id : ((type == 1) ? g_eve_edit_utils.action_id: g_eve_edit_double_tagged.action_id);
        rv = bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_EGRESS | BCM_VLAN_ACTION_SET_WITH_ID, &action_id);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_id_create\n");
            return rv;
        }

        /* Set translation action 1. outer action, set TPID 0x8100. */
        bcm_vlan_action_set_t_init(&action);
        action.dt_outer = bcmVlanActionAdd;
        if (type == 1) {
            action.dt_inner = bcmVlanActionNone;
        } else if (type == 2) {
            action.dt_inner = bcmVlanActionAdd;
            action.dt_inner_pkt_prio = bcmVlanActionAdd;
            action.inner_tpid_action = bcmVlanTpidActionModify;
            action.outer_tpid_action = bcmVlanTpidActionNone;
            action.inner_tpid = 0x8100;
            action.new_outer_vlan = out_vlan;
            action.new_inner_vlan = out_inner_vlan;
            action.priority = 0;
        }

        action.dt_outer_pkt_prio = bcmVlanActionAdd;
        action.outer_tpid = 0x8100;

        rv = bcm_vlan_translate_action_id_set( unit,
                                                   BCM_VLAN_ACTION_SET_EGRESS,
                                                   action_id,
                                                   &action);

        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_id_set\n");
            return rv;
        }


        /* set action class */
        if (!is_trunk) {
            if (type == 1) {
                rv = vlan_default_translate_action_class_set(unit, g_eve_edit_utils.action_id);
                if (rv != BCM_E_NONE) {
                    printf("Error, vlan_default_translate_action_class_set\n");
                    return rv;
                }
            } else if (type == 2) {
                bcm_vlan_translate_action_class_t action_class;
                bcm_vlan_translate_action_class_t_init(&action_class);
                action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
                action_class.vlan_edit_class_id = g_eve_edit_double_tagged.edit_profile;
                action_class.vlan_translation_action_id = g_eve_edit_double_tagged.action_id;
                action_class.tag_format_class_id = g_eve_edit_double_tagged.tag_format;
                rv = bcm_vlan_translate_action_class_set(unit, &action_class);
                if (rv != BCM_E_NONE) {
                    printf("Error, vlan_default_translate_action_class_set\n");
                    return rv;
                }
            }
        } else {
            bcm_vlan_translate_action_class_t action_class;
            bcm_vlan_translate_action_class_t_init(&action_class);

            action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
            action_class.vlan_edit_class_id = g_eve_edit_lag.edit_profile;
            action_class.vlan_translation_action_id = g_eve_edit_lag.action_id;
            action_class.tag_format_class_id = g_eve_edit_lag.tag_format;

            rv = bcm_vlan_translate_action_class_set(unit, &action_class);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_vlan_translate_action_class_set\n");
                return rv;
            }
        }
    } else {
        bcm_vlan_action_set_t_init(&action);
        action.ut_outer = bcmVlanActionAdd;
        action.ut_outer_pkt_prio = bcmVlanActionAdd;
        action.new_outer_vlan = out_vlan;
        action.priority = 0;

        if (type == 2) {
            action.ut_inner = bcmVlanActionAdd;
            action.ut_inner_pkt_prio = bcmVlanActionAdd;
            action.inner_tpid_action = bcmVlanTpidActionModify;
            action.inner_tpid = 0x8100;
            action.new_inner_vlan = out_inner_vlan;
        }

        for ( i = 0; i < trunk_member_num ; i++ ) {
            rv = bcm_vlan_translate_egress_action_add(unit, out_vlan_port[i].vlan_port_id, BCM_VLAN_NONE, BCM_VLAN_NONE, &action);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_vlan_translate_egress_action_add in unit %d sys_port %d \n", unit, bfd_out_port[i]);
            }
        }
    }

    if(verbose >= 1) {
        printf("created FEC-id =0x%08x, in unit %d \n", fec, unit);
        printf("next hop mac at encap-id %08x, in unit %d\n", bfd_encap_id, unit);
    }

    return rv;
}
/*
 *Set the simple password auth for BFD over IPv6.
 */
int bfd_ipv6_auth_sp_set(int unit) {
    int rv = BCM_E_NONE;
    int i;
    bcm_bfd_auth_simple_password_t auth;

    for (i = 0; i < auth_sp_num; i++) {
        auth.length = auth_sp_len[i];
        sal_memcpy(auth.password, &auth_sp_key[i][0], auth.length);

        rv = bcm_bfd_auth_simple_password_set(unit, i, &auth);
        if (BCM_E_NONE != rv) {
		printf("Error in bcm_bfd_auth_simple_password_set Err %x\n",rv);
            return rv;
	}
    }

    return rv;
}

/*
 *Set the sha1 auth for BFD over IPv6.
 */
int bfd_ipv6_auth_sha1_set(int unit) {
    int rv = BCM_E_NONE;
    int i;
    bcm_bfd_auth_sha1_t auth;

    for (i = 0; i < auth_sha1_num; i++) {
        auth.enable = 1;
        auth.sequence = auth_sha1_seq[i];
        sal_memcpy(auth.key, &auth_sha1_key[i][0], 20);

        rv = bcm_bfd_auth_sha1_set(unit, i, &auth);
        if (BCM_E_NONE != rv) {
		printf("Error in bfd_ipv6_auth_sha1_set Err %x\n",rv);
            return rv;
	}
    }

    return rv;
}

/*
 *Set the callback for BFD over IPv6.
 */
int bfd_ipv6_cb(int unit, uint32 flags, bcm_bfd_event_types_t event_types,
    bcm_bfd_endpoint_t endpoint, void *user_data) {
    int event_i;

    printf("endpoint:%d\n", endpoint);
    for (event_i = 0; event_i < bcmBFDEventCount; event_i++) {
        if (BCM_BFD_EVENT_TYPE_GET(event_types, event_i)) {
            bfd_event_count[event_i]++;
            printf("    event:%d\n", event_i);
        }
    }

    return BCM_E_NONE;
}

/*
 *Set the event check for BFD over IPv6.
 */
int bfd_ipv6_event_check(int unit, bcm_bfd_event_type_t event, int expeted_value) {
    if (bfd_event_count[event] != expeted_value) {
        printf("Expected_event:%d, Actual event:%d\n", expeted_value, bfd_event_count[event]);

		return BCM_E_FAIL;
    }

    return BCM_E_NONE;
}

/*
 *Set the events register for BFD over IPv6.
 */
int bcm_bfd_ipv6_events_register(int unit) {
    bcm_error_t rv;
    bcm_bfd_event_types_t e;

    BCM_BFD_EVENT_TYPE_SET(e, bcmBFDEventStateChange);
    BCM_BFD_EVENT_TYPE_SET(e, bcmBFDEventRemoteStateDiag);
    BCM_BFD_EVENT_TYPE_SET(e, bcmBFDEventSessionDiscriminatorChange);
    BCM_BFD_EVENT_TYPE_SET(e, bcmBFDEventAuthenticationChange);
    BCM_BFD_EVENT_TYPE_SET(e, bcmBFDEventParameterChange);
    BCM_BFD_EVENT_TYPE_SET(e, bcmBFDEventSessionError);
    BCM_BFD_EVENT_TYPE_SET(e, bcmBFDEventEndpointTimeout);
    BCM_BFD_EVENT_TYPE_SET(e, bcmBFDEventEndpointTimein);
    BCM_BFD_EVENT_TYPE_SET(e, bcmBFDEventEndpointRemotePollBitSet);
    BCM_BFD_EVENT_TYPE_SET(e, bcmBFDEventEndpointRemoteFinalBitSet);
    rv = bcm_bfd_event_register(unit, e, bfd_ipv6_cb, NULL);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_bfd_event_register Err (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

/*
 *Initialize the BFD IPv6 application.
 */
int bfd_ipv6_example_init(int unit, bcm_bfd_auth_type_t auth_type, int type, int is_trunk) {
    int rv;

    if(verbose >= 1) {
        printf("Step: bfd_ipv6_intf_init\n");
    }
    rv = bfd_ipv6_intf_init(unit, type, is_trunk);
    if (rv != BCM_E_NONE) {
        printf("Error in bfd_ipv6_intf_init (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    if (auth_type == bcmBFDAuthTypeSimplePassword) {
        if(verbose >= 1) {
            printf("Step: bfd_ipv6_auth_sp_set\n");
        }
        rv = bfd_ipv6_auth_sp_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_auth_sp_set (%s) \n",bcm_errmsg(rv));
            return rv;
        }
    } else if (auth_type == bcmBFDAuthTypeKeyedSHA1) {
        if(verbose >= 1) {
            printf("Step: bfd_ipv6_auth_sha1_set\n");
        }
        rv = bfd_ipv6_auth_sha1_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_auth_sha1_set (%s) \n",bcm_errmsg(rv));
            return rv;
        }
    }

    if(verbose >= 1) {
        printf("Step: bcm_bfd_ipv6_events_register\n");
    }
    rv = bcm_bfd_ipv6_events_register(unit);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_bfd_ipv6_events_register (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

/*
 *Trap the BFD IPv6.
 *  - ARAD hardware don't support the BFD IPv6.
 *  - Trap the BFD IPv6 single-hop packet by UDP destination port(3784) and IP ttl(255) to uKernel.
 */
int bfd_ipv6_trap(int unit) {

    int trap_id=0;
    int rv = BCM_E_NONE;
    bcm_rx_trap_config_t trap_config;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent;
    bcm_field_group_t grp;
    bcm_gport_t gport;


    /* create trap */
    rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &trap_id);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rx_trap_type_create (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /* configure trap attribute tot update destination */
    sal_memset(&trap_config, 0, sizeof(trap_config));
    trap_config.flags = (BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP | BCM_RX_TRAP_REPLACE);
    trap_config.trap_strength = 0;
    trap_config.dest_port = bfd_dsp_port;

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rx_trap_set (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIp6);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIpProtocol);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyL4DstPort);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIp6HopLimit);
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionTrap);

    rv = bcm_field_group_create(unit, qset, 2, &grp);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_group_create (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_group_action_set(unit, grp, aset);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_group_action_set (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    BCM_GPORT_TRAP_SET(gport, trap_id, 7, 0);

    rv = bcm_field_entry_create(unit, grp, &ent);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_entry_create (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /* IP protocal: 17(UDP) */
    rv = bcm_field_qualify_IpProtocol(unit, ent, 0x11, 0xff);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_qualify_IpProtocol (%s) \n",bcm_errmsg(rv));
        return rv;
    }
    /* UDP destionation port: 3784(1-hop) */
    rv = bcm_field_qualify_L4DstPort(unit, ent, 3784, 0xffff);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_qualify_L4DstPort (%s) \n",bcm_errmsg(rv));
        return rv;
    }
    /* IP TTL: 255(1-hop) */
    rv = bcm_field_qualify_Ip6HopLimit(unit, ent, 255, 0xff);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_qualify_Ip6HopLimit (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_action_add(unit, ent, bcmFieldActionTrap, gport, 0);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_action_add (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_entry_install(unit, ent);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_entry_install (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

/**
 * Enable punt packets.
 *
 *  - Remote state change will PUNT the OAM/BFD packet.
 *  - The OAM/BFD PUNT destination can be configured by this function.
 *  - All types of accelerated MEPs (OAM/BFD) will be affected.
 */
int bfd_ipv6_run_with_punt(int unit, int dest_port,int in_port_1, int in_port_2,int flag_mc) {
    bcm_rx_trap_config_t trap_config_protection;
    bcm_multicast_replication_t rep_array;
    int trap_code=0x480; /* trap code on FHEI  will be (trap_code & 0xff), in this case 1.*/
    int flags;
    /* valid trap codes for oamp traps are 0x401 - 0x4ff */
    int rv;

    if (flag_mc) {
        bcm_gport_t gport;
        bcm_multicast_t mc_group=0x1000;
        BCM_GPORT_MCAST_SET(gport, mc_group);
        rv =  bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID, &mc_group);
        BCM_IF_ERROR_RETURN(rv);

        rv = oam__device_type_get(unit, &device_type);
        if (rv != BCM_E_NONE) {
            printf("Error, in oam__device_type_get\n");
            return rv;
        }

        if (device_type >= device_type_jericho2) {
            flags = BCM_MULTICAST_INGRESS_GROUP;
            bcm_multicast_replication_t_init(&rep_array);
           rep_array.port = in_port_1;
           rep_array.encap1 = in_port_1;
           rv = bcm_multicast_add(unit, mc_group, flags, 1, &rep_array);
        } else {
        	rv =  bcm_multicast_ingress_add(unit, mc_group, in_port_1, in_port_1);
        }
        BCM_IF_ERROR_RETURN(rv);

        if (device_type >= device_type_jericho2) {
            flags = BCM_MULTICAST_INGRESS_GROUP;
            bcm_multicast_replication_t_init(&rep_array);
           rep_array.port = in_port_2;
           rep_array.encap1 = in_port_2;
           rv = bcm_multicast_add(unit, mc_group, flags, 1, &rep_array);
        } else {
        	rv =  bcm_multicast_ingress_add(unit, mc_group, in_port_2, in_port_2);
        }
        BCM_IF_ERROR_RETURN(rv);

        print mc_group;
        trap_config_protection.dest_port = gport;
        printf("MC Group id %x Port-1 %d Port-2 %d\n",gport,in_port_1,in_port_2);
    } else {
        rv = port_to_system_port(unit, dest_port, &trap_config_protection.dest_port);
        if (rv != BCM_E_NONE){
            printf("Error, in port_to_system_port\n");
            return rv;
        }
    }
    
    flags = BCM_RX_TRAP_WITH_ID;
    if (device_type >= device_type_jericho2) {
        /** For DNX, a real trap is used - it must be destroyed */
        rv = bcm_rx_trap_type_get(unit, 0, bcmRxTrapOampRmepStateChange, &trap_code);
        if (rv != BCM_E_NONE) {
           printf("trap get: (%s) \n",bcm_errmsg(rv));
           return rv;
        }
        
        rv = bcm_rx_trap_type_destroy(unit, trap_code);
        if (rv != BCM_E_NONE) {
           printf("trap destroy: (%s) \n",bcm_errmsg(rv));
           return rv;
        }
        
        /** For DNX, it's better to create without ID */
        flags = 0;
    }

    rv = bcm_rx_trap_type_create(unit, flags, bcmRxTrapOampRmepStateChange, &trap_code);
    if (rv != BCM_E_NONE) {
       printf("trap create: (%s) \n",bcm_errmsg(rv));
       return rv;
    }

    rv = bcm_rx_trap_set(unit, trap_code, trap_config_protection);
    if (rv != BCM_E_NONE) {
       printf("trap set: (%s) \n",bcm_errmsg(rv));
       return rv;
    }

    return rv;
}

/*
 *BFD IPv6 test example.
 *  - Test the BFD IPv6 APIs whether works which include bcm_bfd_endpoint_create,
 *     bcm_bfd_endpoint_get and bcm_bfd_endpoint_destroy.
 *  - Create the BFD IPv6 endpoint that will send the BFD IPv6 packet to remote BFD IPv6
 *     device and receive BFD IPv6 packet from remote BFD IPv6 device.
 */
int bfd_ipv6_run_with_defaults(int unit, int endpoint_id, int is_acc, int is_single_hop, bcm_bfd_auth_type_t auth_type, int auth_index, int type, int is_trunk, int punt_rate, int use_my_bfd_dip) {
    int rv;
    bcm_bfd_endpoint_info_t bfd_endpoint_info = {0};
    bcm_bfd_endpoint_info_t bfd_endpoint_test_info = {0};
    int ret;
    bcm_l3_route_t l3rt;
    bcm_ip6_t mask_full = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
    bcm_gport_t remote_gport;
    int trap_code;
    uint8 is_local_dicr_as_session_id = 0;
    int arp_plus_ac_type = 0, bfd_my_dip_destination_read;
 
    if (!is_device_or_above(unit,ARAD_PLUS)) {
        printf("Error BFD IPv6 only support on arad+ / jericho.\n");
        return BCM_E_INTERNAL;
    }

    if (is_device_or_above(unit,JERICHO2)) {
        if(type == 2) {
            arp_plus_ac_type = 2;
        }
        rv = basic_example_ipv6(unit,bfd_in_port[0],bfd_out_port[0],0,0,arp_plus_ac_type);
        if(use_vxlan6_fec) {
            /** Delete conflicting RIF */
            bcm_l3_intf_t rif_to_delete;
            bcm_l3_intf_t_init(&rif_to_delete);
            rif_to_delete.l3a_intf_id = 15;
            bcm_l3_intf_delete(unit, &rif_to_delete);

            /** Create VXLAN FEC */
            vxlan_roo_example(unit, 0 /* is_ipv6 */, bfd_out_port[0], bfd_in_port[0], 1 /* add_routing_table_entry */);
            vxlan6_roo_fec = ip_tunnel_basic_info.encap_native_fec_id[1];
        }
    } else {
        rv = bfd_ipv6_example_init(unit, auth_type, type, is_trunk);
    }
    if (rv != BCM_E_NONE) {
        printf("Error in bfd_ipv6_example_init (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    if (verbose >= 1) {
        printf("Step: bcm_bfd_endpoint_create \n");
    }

    /* Adding BFDoIPV6 one hop endpoint */
    bcm_bfd_endpoint_info_t_init(&bfd_endpoint_info);

    bfd_endpoint_info.flags = BCM_BFD_ENDPOINT_IPV6;
    if (!is_single_hop) {
        bfd_endpoint_info.flags |= BCM_BFD_ENDPOINT_MULTIHOP;
    }
    if (is_acc) {
        bfd_endpoint_info.flags |= BCM_BFD_ENDPOINT_IN_HW;
    }
    if (is_micro_bfd == 1) {
        bfd_endpoint_info.flags |= BCM_BFD_ENDPOINT_MICRO_BFD;
    }


    if(((bfd_endpoint_info.flags &BCM_BFD_ENDPOINT_IN_HW) !=0))
    {
        if (use_invalid_tx_gport)
        {
            bfd_endpoint_info.tx_gport = BCM_GPORT_INVALID;
        }
        else
        {
            if (is_trunk) {
                bfd_endpoint_info.tx_gport = trunk_gport[1];
            } 
            else {
                if(is_device_or_above(unit, JERICHO2)) {
                    BCM_GPORT_SYSTEM_PORT_ID_SET(bfd_endpoint_info.tx_gport, bfd_out_port[0]);
                } else {
                    bcm_stk_sysport_gport_get(unit,bfd_out_port[0], &bfd_endpoint_info.tx_gport);
                }
            }
        }
    }

    if (is_acc || !is_single_hop || is_s_bfd_ipv6_init_ep)
    {
        sal_memcpy(&(bfd_endpoint_info.src_ip6_addr), &src_ipv6, 16);
    }

    if (bfd_remote_port == -1) {
        remote_gport = BCM_GPORT_INVALID;
        if (is_device_or_above(unit, JERICHO2) && is_acc == 0)
        {
            rv = bcm_rx_trap_type_get(unit, 0, bcmRxTrapBfdOamDownMEP, &trap_code);
            if (rv != BCM_E_NONE) {
                printf("(%s) \n",bcm_errmsg(rv));
                return rv;
            }
            BCM_GPORT_TRAP_SET(remote_gport, trap_code, 7, 0);
        }
    }
    else if (bfd_remote_port == -2) 
    {
        /** 
        *  Non accelerated endpoints, but incoming BFD packets are classified by ACC MEP-DB. 
        *  Thus, allowing the oam-id to be set on trapped packet's FHEI.
        */
        if (is_device_or_above(unit, JERICHO2))
        {
            rv = bcm_rx_trap_type_get(unit, 0, bcmRxTrapBfdOamDownMEP, &trap_code);
            if (rv != BCM_E_NONE) {
                printf("(%s) \n",bcm_errmsg(rv));
                return rv;
            }
            BCM_GPORT_TRAP_SET(remote_gport, trap_code, 7, 0);
        }
    } 
    else 
    {
      if (use_trap_with_snoop) {
          BCM_GPORT_TRAP_SET(remote_gport, bfd_remote_port, 7, 3);
      }
      else {
          printf("Setting remote_gport to port 0x%08x with strength %d\n",
                  bfd_remote_port, 7);
          BCM_GPORT_LOCAL_SET(remote_gport, bfd_remote_port);

          /* remote_gport field must be a trap */
          rv = bfd_create_trap_from_gport(unit, remote_gport, &trap_code);
          if (rv != BCM_E_NONE) {
              printf("Error bfd_create_trap_from_gport.\n");
              return rv;
          }
          BCM_GPORT_TRAP_SET(remote_gport, trap_code, 7, 0);
      }
    }

    bfd_endpoint_info.remote_gport = remote_gport;

    if (use_trap_with_snoop)
    {
        BCM_GPORT_TRAP_SET(bfd_endpoint_info.remote_gport, bfd_trap_code, trap_strength, snoop_strength);
    }

    bfd_endpoint_info.local_discr = local_disc;

    /** Various bfd acceleration 
    * Case 1 - Standart non acc
    *   BCM_BFD_ENDPOINT_IN_HW flag is NOT set
    * Case 2 - Standart acc
    *   BCM_BFD_ENDPOINT_IN_HW flag is NOT set
    * Case 3 - Classified by OEM-2, but no OAMP entry
    *   BCM_BFD_ENDPOINT_IN_HW is set, Remote_gport != NULL                      (in case of JER1)
    *   BCM_BFD_ENDPOINT_IN_HW is set, Remote_gport != NULL and tx_gport == NULL (in case of JR2)
    * Case 4 - Classified by OEM-2 to "remote CPU", TX in OAMP
    *   BCM_BFD_ENDPOINT_IN_HW is set, Remote_gport != NULL and tx_gport != NULL
    */
    if (((bfd_endpoint_info.flags &BCM_BFD_ENDPOINT_IN_HW) !=0)  && \
        ((bfd_endpoint_info.remote_gport==BCM_GPORT_INVALID) || ((bfd_endpoint_info.tx_gport!=BCM_GPORT_INVALID) && is_device_or_above(unit, JERICHO2)) \
            || use_trap_with_snoop))
    {
        is_local_dicr_as_session_id = soc_property_get(unit , "bfd_use_local_discriminator_as_session_id", 0);
        if ((is_device_or_above(unit,JERICHO2))&&(is_acc)) {
          /** In JR2 accelerated BFD session must have the same ID as
           *  the local discriminator
           */
           is_local_dicr_as_session_id = 1;
        }
        else {
           bfd_endpoint_info.flags |= BCM_BFD_ENDPOINT_WITH_ID;
        }
        bfd_endpoint_info.id = endpoint_id;
        if (is_local_dicr_as_session_id) {
            bfd_endpoint_info.id = local_disc;
        }
        bfd_endpoint_info.type = bcmBFDTunnelTypeUdp;

        if (is_single_hop) {
            bfd_endpoint_info.ip_ttl = 255;
        } else {
            bfd_endpoint_info.ip_ttl = 200;
        }
        bfd_endpoint_info.ip_tos = 0xab;
        
        sal_memcpy(&(bfd_endpoint_info.dst_ip6_addr), &dst_ipv6, 16);
        if (is_device_or_above(unit, JERICHO2)) {
            bfd_endpoint_info.udp_src_port = 0xc001;
        } else {
            bfd_endpoint_info.udp_src_port = 50000;
        }
        bfd_endpoint_info.auth_index = 0;
        bfd_endpoint_info.auth = bcmBFDAuthTypeNone;
        
        bfd_endpoint_info.local_state = is_bfd_echo ? 0 : bcmBFDStateDown;
        bfd_endpoint_info.local_min_tx = 100000;
        bfd_endpoint_info.local_min_rx = 100000;
        bfd_endpoint_info.local_detect_mult = 3;
        bfd_endpoint_info.remote_discr = is_bfd_echo ? 0 : remote_disc;
        bfd_endpoint_info.bfd_period = bfd_tx_period;

        if (is_bfd_echo == 1) {
            bfd_endpoint_info.flags |= BCM_BFD_ECHO;
            bfd_endpoint_info.local_min_echo = 100000;
            if (is_device_or_above(unit, JERICHO2)) {
                bfd_endpoint_info.ip_ttl = 200;
            }
        }

        if (is_on_demond == 1) {
             bfd_endpoint_info.flags |= BCM_BFD_ENDPOINT_DEMAND;
        }
        if (is_micro_bfd == 1) {
            bfd_endpoint_info.flags |= BCM_BFD_ENDPOINT_MICRO_BFD;
        }
        if (is_device_or_above(unit,JERICHO2)) {
            /* the first entry in bank 8 which is an extra data only bank*/
            bfd_endpoint_info.ipv6_extra_data_index = 1 << 16;
        }


        if (is_device_or_above(unit,JERICHO2)) {
            int itf_id = *dnxc_data_get(unit, "l3", "rif", "nof_rifs", NULL) + 0x384;
            if (bfd_over_ipv6_transmit_into_mpls) {
                /* Create MPLS, FEC objects*/
                bcm_mpls_egress_label_t label_array[2];
                bcm_mpls_egress_label_t_init(&label_array[0]);
                bcm_mpls_egress_label_t_init(&label_array[1]);
                int encap_id, out_port;
                BCM_L3_ITF_SET(encap_id, BCM_L3_ITF_TYPE_LIF, itf_id);
                label_array[0].label = 500;
                label_array[1].label = 501;
                label_array[0].flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
                label_array[1].flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
                label_array[0].l3_intf_id = encap_id ;/* MPLS points to the "next hop mac*/
                label_array[1].l3_intf_id = encap_id ;/* MPLS points to the "next hop mac*/
                BCM_IF_ERROR_RETURN(bcm_mpls_tunnel_initiator_create(unit, 0, 2, label_array));

                BCM_GPORT_SYSTEM_PORT_ID_SET(out_port, bfd_in_port[0]);
                rv = l3__egress_only_fec__create(unit, 52474, 0, label_array[0].tunnel_id, out_port, 0);
                if (rv != BCM_E_NONE) {
                    printf("(%s) mpls_create_initiator_tunnels failed  \n",bcm_errmsg(rv));
                    return rv;
                }
                /* fec from above call points to mpls "tunnel-id" which points to the ARP pointer.
                BFD endpoint's outlif will be the fec-id.*/
                BCM_L3_ITF_SET(bfd_endpoint_info.egress_if, BCM_L3_ITF_TYPE_FEC, 52474);

                /* egress_if will be set to a fec, so tx_gport should be INVALID*/
                bfd_endpoint_info.tx_gport = BCM_GPORT_INVALID;

                /*BFD ipv6 into tunnel - use ttl 1*/
                bfd_endpoint_info.ip_ttl = 1;
            }
            else
            {
                if(use_vxlan6_fec)
                {
                    bfd_endpoint_info.egress_if = vxlan6_roo_fec;

                    /* egress_if will be set to a fec, so tx_gport should be INVALID*/
                    bfd_endpoint_info.tx_gport = BCM_GPORT_INVALID;
                }
                else
                {
                    BCM_L3_ITF_SET(bfd_endpoint_info.egress_if, BCM_L3_ITF_TYPE_LIF, itf_id);
                }
            }
        } else {
            bfd_endpoint_info.egress_if = bfd_encap_id;
        }

        if (is_device_or_above(unit,JERICHO2)) {
            if (int_pri) {
                bfd_endpoint_info.int_pri = int_pri;
            } else {    
                bfd_endpoint_info.int_pri = 1;
            }
        }

        bfd_endpoint_info.auth = auth_type;
        if (auth_type != bcmBFDAuthTypeNone) {
            bfd_endpoint_info.auth_index = auth_index;
        }
        if (punt_rate) {
            bfd_endpoint_info.sampling_ratio = punt_rate;
        }

        if(is_device_or_above(unit,JERICHO2)) {

            bfd_endpoint_info.flags2 = 0;
            if (is_rx_statisitcs_enabled)
            {
                bfd_endpoint_info.flags2 |= BCM_BFD_ENDPOINT_FLAGS2_RX_STATISTICS;
            }

            if (is_tx_statisitcs_enabled)
            {
                bfd_endpoint_info.flags2 |= BCM_BFD_ENDPOINT_FLAGS2_TX_STATISTICS;
            }

            if (use_my_bfd_dip)
            {
                bfd_endpoint_info.flags2 |= BCM_BFD_ENDPOINT_FLAGS2_USE_MY_DIP_DESTINATION;
                /** Set My-BFD-DIP-Destination */
                rv = bcm_switch_control_set(unit, bcmSwitchBfdMyDipDestination, my_bfd_dip_destination);
                if (rv != BCM_E_NONE) {
                    printf("(%s) \n",bcm_errmsg(rv));
                    return rv;
                }

                rv = bcm_switch_control_get(unit, bcmSwitchBfdMyDipDestination, &bfd_my_dip_destination_read);
                if (rv != BCM_E_NONE) {
                    printf("(%s) \n",bcm_errmsg(rv));
                    return rv;
                }

                if(bfd_my_dip_destination_read != my_bfd_dip_destination)
                {
                    printf("Setting BFD-My-DIP-destination failed, value read: 0x%08x, value written 0x%08x\n",
                            my_bfd_dip_destination, bfd_my_dip_destination_read);
                    return BCM_E_INTERNAL;
                }

                /** Set up forwarding from the IP to the destination */
                bcm_l3_route_t_init(&l3rt);
                l3rt.l3a_subnet = bfd_endpoint_info.src_ip_addr;
                l3rt.l3a_ip_mask = 0xFFFFFFFF;
                sal_memcpy(l3rt.l3a_ip6_net, bfd_endpoint_info.src_ip6_addr, 16);
                sal_memcpy(l3rt.l3a_ip6_mask, mask_full, 16);
                l3rt.l3a_intf = my_bfd_dip_destination;
                l3rt.l3a_vrf = 1;
                l3rt.l3a_flags = BCM_L3_IP6;
                l3rt.l3a_flags2 = BCM_L3_FLAGS2_SCALE_ROUTE;
                rv = bcm_l3_route_add(unit, &l3rt);
                if (rv != BCM_E_NONE) {
                    printf("(%s) \n",bcm_errmsg(rv));
                    return rv;
                }
            }
        }
    }

    if (is_s_bfd_ipv6_init_ep) {
        bfd_endpoint_info.flags2 |= BCM_BFD_ENDPOINT_FLAGS2_SEAMLESS_BFD_INITIATOR;
        /* Some number outside 49152 - 65535 range since
         * SBFD does not have limitations on UDP src port range */
        bfd_endpoint_info.udp_src_port = 20020;
        if ((bfd_endpoint_info.flags &BCM_BFD_ENDPOINT_IN_HW) !=0) {
            /* local_min_rx and local_min_echo needs to be zero */
            bfd_endpoint_info.local_min_rx = 0;
            bfd_endpoint_info.local_min_echo = 0;
            /* TTL should be set to 255 */
            bfd_endpoint_info.ip_ttl = 255;
            /* local_state set to UP */
            bfd_endpoint_info.local_state = 3;
            /* Local flag should have demand bit (bit 1) set
             * P | F | C | A | D | M
             * 0 | 0 | 0 | 0 | 1 | 0
             */
            bfd_endpoint_info.local_flags = 0x2;
        }
    }

    bcm_bfd_ipv6_events_register(unit);
    if (!is_device_or_above(unit,JERICHO2)) {
        bfd_endpoint_info.egress_if = bfd_encap_id;
    }
    rv = bcm_bfd_endpoint_create(unit, &bfd_endpoint_info);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_bfd_endpoint_create (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    if (verbose >= 1) {
        printf("Step: bfd endpoint id:0x%x\n", bfd_endpoint_info.id);
    }

    if (verbose >= 1) {
        printf("Step: bfd endpoint get \n");
    }

    bcm_bfd_endpoint_info_t_init(&bfd_endpoint_test_info);
    rv = bcm_bfd_endpoint_get(unit, bfd_endpoint_info.id, &bfd_endpoint_test_info);
    if (rv != BCM_E_NONE) {
      printf("Error in bcm_bfd_endpoint_get (%s) \n",bcm_errmsg(rv));
      return rv;
    }

    if (verbose >= 1) {
        printf("Step: bfd endpoint compare \n");
    }

    /* Here mep_not_acc_info and mep_not_acc_test_info are compared */
    ret = cmp_structs(unit, &bfd_endpoint_info, &bfd_endpoint_test_info,bcmBFDTunnelTypeUdp ) ;
    if (ret != 0) {
        printf("%d different fields in BFDoIPV4\n", ret);
    }

    if (verbose >= 1) {
        printf("Step: bfd endpoint destroy \n");
    }

    rv = bcm_bfd_endpoint_destroy(unit, bfd_endpoint_info.id);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_bfd_endpoint_destroy (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    if (verbose >= 1) {
        printf("Step: bfd endpoint re-create \n");
    }

    rv = bcm_bfd_endpoint_create(unit, &bfd_endpoint_info);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_bfd_endpoint_create (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    if(is_device_or_above(unit, JERICHO2) && is_bfd_echo)
    {
       rv = field_presel_fwd_layer_main(unit,cint_bfd_ipv6_echo_presel_id, bcmFieldStageIngressPMF1, bcmFieldLayerTypeIp6, &cint_bfd_ipv6_echo_context_id_ipmf1, "ipv6 hit");
       if (rv != BCM_E_NONE)
       {
           printf("Error (%d), in cint_field_always_hit_context_main Ingress\n", rv);
           return rv;
        }

        rv = cint_field_bfd_echo_main(unit, cint_bfd_ipv6_echo_context_id_ipmf1, 1);
        if (rv != BCM_E_NONE) {
          printf("Error cint_field_bfd_echo_main.\n");
          return rv;
        }

        rv = cint_field_bfd_echo_entry_add(unit, 0, &src_ipv6, local_disc & 0xFFFF);
        if (rv != BCM_E_NONE) {
          printf("Error cint_field_bfd_echo_entry_add.\n");
          return rv;
        }
    }


    bfd_ep1.id = bfd_endpoint_info.id;

    if (enable_oamp_counter_allocation)
    {
        /** Allocate counters for 1k MEPs with statistics enable, counter base is the first MEP. */
        /* last parameter eviction always 1 for BFD*/
        rv = cint_oam_oamp_statistics_main(unit, bfd_ep1.id, 1024, (enable_oamp_counter_allocation-1),1);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }
    }

    return rv;
}

/*
 *BFD IPv6 test example.
 *  - Test the BFD IPv6 APIs whether works which include bcm_bfd_endpoint_create,
 *     bcm_bfd_endpoint_get and bcm_bfd_endpoint_destroy.
 *  - Create the BFD IPv6 endpoint that will send the BFD IPv6 packet to remote BFD IPv6
 *     device and receive BFD IPv6 packet from remote BFD IPv6 device.
 */
int bfd_ipv6_run_with_defaults_cleanup(int unit) {
    int rv;
    bcm_if_t fec_to_delete;
    bcm_if_t lif_to_delete;

    if (verbose >= 1) {
        printf("Step: bcm_bfd_endpoint_destroy_all \n");
    }

    if (!is_device_or_above(unit, JERICHO2) && (bfd_remote_port != BCM_GPORT_INVALID)) {
         bcm_rx_trap_type_destroy(unit, user_defined_trap_code);
    }

    rv = bcm_bfd_endpoint_destroy_all(unit);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_bfd_endpoint_destroy (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    if (verbose >= 1) {
        printf("Step: bcm_bfd_detach \n");
    }

    if(is_device_or_above(unit, JERICHO2))
    {
        rv = bcm_l3_intf_delete_all(unit);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_l3_intf_delete_all (%s) \n",bcm_errmsg(rv));
            return rv;
        }

        BCM_L3_ITF_SET(fec_to_delete,BCM_L3_ITF_TYPE_FEC,ip_route_fec);
        rv = bcm_l3_egress_destroy(unit, fec_to_delete);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_l3_egress_destroy (%s) \n",bcm_errmsg(rv));
            return rv;
        }

        BCM_L3_ITF_SET(lif_to_delete,BCM_L3_ITF_TYPE_LIF,ip_route_encap);
        rv = bcm_l3_egress_destroy(unit, lif_to_delete);
        {
            printf("Error in bcm_l3_egress_destroy (%s) \n",bcm_errmsg(rv));
            return rv;
        }
    }
    else
    {
        rv = bcm_bfd_detach(unit);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_bfd_detach (%s) \n",bcm_errmsg(rv));
            return rv;
        }
    }

    return rv;
}

/*
 * Clear loc in BFD session Event register
 */
int bfd_ipv6_loc_event_clear(int unit) {
    int rv;

   /* bcm_bfd_ipv6_events_register(unit);*/
    bcm_bfd_event_types_t events;
    BCM_BFD_EVENT_TYPE_CLEAR_ALL(events);

    BCM_BFD_EVENT_TYPE_SET(events, bcmBFDEventStateChange);
    BCM_BFD_EVENT_TYPE_SET(events, bcmBFDEventRemoteStateDiag);
    BCM_BFD_EVENT_TYPE_SET(events, bcmBFDEventSessionDiscriminatorChange);
    BCM_BFD_EVENT_TYPE_SET(events, bcmBFDEventAuthenticationChange);
    BCM_BFD_EVENT_TYPE_SET(events, bcmBFDEventParameterChange);
    BCM_BFD_EVENT_TYPE_SET(events, bcmBFDEventSessionError);

    rv=bcm_bfd_event_register(unit, events, bfd_ipv6_cb, NULL);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_bfd_event_register (%s) \n",bcm_errmsg(rv));
        return rv;
    }
    return rv;
}


/*
* function compares endpoint_info_t created with bfd_mpls_endpoint_info_create() and a struct returned from bfd_endpoint_info_get().
*  type may be bcmBFDTunnelTypePweControlWord,  bcmBFDTunnelTypeMpls or bcmBFDTunnelTypeUdp
*  function returns 0 upon success, number of wrong fields upon failure.
*/
int cmp_structs(int unit, bcm_bfd_endpoint_info_t* endpoint_create,bcm_bfd_endpoint_info_t* endpoint_get, int type  ){
    int rv =0;
    uint32 create_flags, get_flags, ignore_flags;
    create_flags = endpoint_create->flags;
    get_flags = endpoint_get->flags;
    if(is_device_or_above(unit, JERICHO2))
    {
        if((endpoint_create->type == bcmBFDTunnelTypeMplsTpCc) ||
           (endpoint_create->type == bcmBFDTunnelTypePweGal) ||
           (endpoint_create->type == bcmBFDTunnelTypePweControlWord) ||
           (endpoint_create->type == bcmBFDTunnelTypePweRouterAlert) ||
           (endpoint_create->type == bcmBFDTunnelTypePweTtl))
        {
            /** This flag is added for these types */
            create_flags |= BCM_BFD_ENDPOINT_PWE_ACH;
            get_flags |= BCM_BFD_ENDPOINT_PWE_ACH;
        }
    }
    ignore_flags = BCM_BFD_ENDPOINT_WITH_ID | BCM_BFD_ENDPOINT_UPDATE;
    if ((create_flags & (~ignore_flags)) != (get_flags & (~ignore_flags))) {
      printf("Error: endpoint_info_t struct returned from endpoint_info_get not the same as the one created with endpoint_create() - flags fields are different.\n");
      printf("Create flags: 0x%08X\nGet flags: 0x%08X\n", create_flags, get_flags);
      ++rv;
    }
    if (endpoint_create->id != endpoint_get->id ) {
      printf("Error: endpoint_info_t struct returned from endpoint_info_get not the same as the one created with endpoint_create() - id fields are different.\n");
      ++rv;
    }
    if (endpoint_create->type != endpoint_get->type ) {
      printf("Error: endpoint_info_t struct returned from endpoint_info_get not the same as the one created with endpoint_create() - type fields are different.\n");
      ++rv;
    }
    if(!is_device_or_above(unit, JERICHO2))
    {
        if (endpoint_create->gport != endpoint_get->gport ) {
          printf("Error: endpoint_info_t struct returned from endpoint_info_get not the same as the one created with endpoint_create() - gport fields do not match.\n Expected %d, got %d.\n",
                    endpoint_create->gport, endpoint_get->gport);
          ++rv;
        }
    }
    else
    {
        /**
         * In JR2, gport is ignored if the incoming packet is classified by discriminator.
         * Also, the gport's prefix (upper 16 bits) is changed before saving it.
         */
        if((endpoint_create->type != bcmBFDTunnelTypeUdp) && (endpoint_create->type != bcmBFDTunnelTypeMpls) &&
           ((endpoint_create->gport & 0xFFFF) != (endpoint_get->gport & 0xFFFF)))
        {
            printf("Error: endpoint_info_t struct returned from endpoint_info_get not the same as the one created with endpoint_create() - gport fields do not match.\n Expected %d, got %d.\n",
                      endpoint_create->gport, endpoint_get->gport);
        }
    }
    if((!is_device_or_above(unit, JERICHO2) || ((endpoint_create->flags & BCM_BFD_ENDPOINT_IN_HW)==0)) &&
     (endpoint_create->remote_gport != endpoint_get->remote_gport )) {
      printf("Error: endpoint_info_t struct returned from endpoint_info_get not the same as the one created with endpoint_create() - remote_gport fields do not match.\n");
      ++rv;
    }

    if (endpoint_create->src_ip_addr != endpoint_get->src_ip_addr ) {
      printf("Error: endpoint_info_t struct returned from endpoint_info_get not the same as the one created with endpoint_create() - src_ip_addr fields do not match. Expected %d, got %d\n",
                endpoint_create->src_ip_addr, endpoint_get->src_ip_addr );
      ++rv;
    }
    if (!is_device_or_above(unit, JERICHO2) ||
       (((bcmBFDTunnelTypeUdp == endpoint_create->type)
         && ((endpoint_create->flags & BCM_BFD_ENDPOINT_MULTIHOP) != 0))
        || (bcmBFDTunnelTypeMpls == endpoint_create->type)))
    {
        if (endpoint_create->ip_tos != endpoint_get->ip_tos ) {
          printf("Error: endpoint_info_t struct returned from endpoint_info_get not the same as the one created with endpoint_create() - ip_tos fields do not match.\n");
          ++rv;
        }
        if (endpoint_create->ip_ttl != endpoint_get->ip_ttl ) {
          printf("Error: endpoint_info_t struct returned from endpoint_info_get not the same as the one created with endpoint_create() - ip_ttl fields do not match.\n");
         ++rv;
        }
    }
    /** In JR2, udp_src_port is ignored for PWE types */
    if ((!is_device_or_above(unit, JERICHO2) ||
       (bcmBFDTunnelTypeUdp == endpoint_create->type) ||
       (bcmBFDTunnelTypeMpls == endpoint_create->type)) &&
       (endpoint_create->udp_src_port != endpoint_get->udp_src_port) ) {
      printf("Error: endpoint_info_t struct returned from endpoint_info_get not the same as the one created with endpoint_create() - udp_src_port fields do not match.\n");
     ++rv;
    }
    if (endpoint_create->egress_if !=endpoint_get->egress_if ) {
      printf("Error: endpoint_info_t struct returned from endpoint_info_get not the same as the one created with endpoint_create() - egress_if fields do not match.\n");
      ++rv;
    }
    if (endpoint_create->int_pri != endpoint_get->int_pri ) {
      printf("Error: endpoint_info_t struct returned from endpoint_info_get not the same as the one created with endpoint_create() - int_pri fields do not match.\n");
      ++rv;
    }
    if (endpoint_create->local_discr != endpoint_get->local_discr ) {
      printf("Error: endpoint_info_t struct returned from endpoint_info_get not the same as the one created with endpoint_create() - local_discr fields do not match.\n");
      ++rv;
    }
    if (endpoint_create->local_min_tx != endpoint_get->local_min_tx ) {
      printf("Error: endpoint_info_t struct returned from endpoint_info_get not the same as the one created with endpoint_create() - local_min_tx fields do not match.\n");
      ++rv;
    }
    if (endpoint_create->bfd_period - endpoint_get->bfd_period > 10) {
      printf("Error: endpoint_info_t struct returned from endpoint_info_get not the same as the one created with endpoint_create() - bfd_period fields do not match.\n");
      ++rv;
    }
    if (endpoint_create->local_min_rx != endpoint_get->local_min_rx ) {
      printf("Error: endpoint_info_t struct returned from endpoint_info_get not the same as the one created with endpoint_create() - local_min_rx fields do not match.\n");
      ++rv;
    }
    if (endpoint_create->local_detect_mult != endpoint_get->local_detect_mult ) {
      printf("Error: endpoint_info_t struct returned from endpoint_info_get not the same as the one created with endpoint_create() - local_detect_mult fields do not match.\n");
      ++rv;
    }
    if (endpoint_create->local_state !=endpoint_get->local_state ) {
      printf("Error: endpoint_info_t struct returned from endpoint_info_get not the same as the one created with endpoint_create() - local_state fields do not match.\n");
      ++rv;
    }
    if (endpoint_create->remote_discr != endpoint_get->remote_discr) {
      printf("Error: endpoint_info_t struct returned from endpoint_info_get not the same as the one created with endpoint_create() - remote_discr fields do not match.Expected %d, got %d\n",
              endpoint_create->remote_discr, endpoint_get->remote_discr);
      ++rv;
    }
    if (endpoint_create->local_diag != endpoint_get->local_diag) {
      printf("Error: endpoint_info_t struct returned from endpoint_info_get not the same as the one created with endpoint_create() - local_diag fields do not match.\n");
      ++rv;
    }
    if ((type == bcmBFDTunnelTypeUdp && ((endpoint_create->flags & BCM_BFD_ENDPOINT_MULTIHOP)==0)) && !is_device_or_above(unit, JERICHO2)) { /*ip single hop - dest ip should be zero.*/
       if (endpoint_get->dst_ip_addr != 0 ) {
          printf("Error, BFDoIPV4: dst_ip_addr field in endpoint_info_t field returned from endpoint_info_get non zero on single hop ip encapsulation. Dest addr is %d\n",endpoint_get->dst_ip_addr);
          ++rv;
      }
    }
    else{ /* in other cases dest_addresses must match*/
      if (endpoint_get->dst_ip_addr != endpoint_create->dst_ip_addr ) {
          printf("Error: endpoint_info_t struct returned from endpoint_info_get not the same as the one created with endpoint_create() - dst_ip_addr fields do not match.\n");
          ++rv;
      }
    }
    return rv;
}
