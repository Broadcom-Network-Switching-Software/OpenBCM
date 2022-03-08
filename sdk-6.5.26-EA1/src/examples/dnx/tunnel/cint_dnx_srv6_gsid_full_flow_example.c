
/*
 * Examples of Jericho2-only tunnel termination
 *
 * Description:
 * -The Cint is full Demo of traversing an IPv4oETH packet through an SRv6 Tunnel/Overlay.
 * -The Demo includes the following scenarios:
 * -------------------------------------
 * 1.IPv4oETH packet traverses through an SRv6 Ingress Node to add SRv6 and IPv6 Encapsulation,
 *   and output IPv4oSRv6_3oIPv6oETH packet (SRv6_3 means with SRH header with x3 SIDs)
 * 2.Packet from scenario (1) IPv4oSRv6_3oIPv6oETH through an SRv6 Endpoint Node that performs
 *   header editing - SRH (update SL) and IPv6 (update DIP to next SID)
 *
 *   if {SRv6 Egress modes is set to USP} :
 * 3.Packet from scenario (2) IPv4oSRv6_3oIPv6oETH through next SRv6 Endpoint Node that performs
 *   header editing - SRH (update SL) and IPv6 (update DIP to next SID)
 *
 *   if {SRv6 Egress modes is set to PSP} :
 * 3.Packet from scenario (2) IPv4oSRv6_3oIPv6oETH through next SRv6 Egress PSP Node that performs
 *   header editing - Terminate SRv6 header and IPv6 (update DIP to next SID)
 *
 *   if {SRv6 Egress modes is set to USP} :
 * 4.Packet from scenario (3) IPv4oSRv6_3oIPv6oETH through an SRv6 Egress USP Node that performs
 *    on 1st Pass - Termination of SRv6 and IPv6 Header
 *    on 2nd Pass - IPv4 Forwarding
 *    and creates an IPv4oETH packet
 *
 *   if {SRv6 encapsulation mode is set to Reduced} :
 *   All above scenarios are performed whilst not including the 1st SID
 *
 *********************************
 *       Device  Demo Run        *
 *********************************
 *
 * Add/modify following SOC Properties to "config-jr2.bcm":
 * -----------------------------
 * tm_port_header_type_in_0=ETH
 * tm_port_header_type_in_200=ETH
 * tm_port_header_type_in_201=ETH
 * tm_port_header_type_in_202=ETH
 * tm_port_header_type_in_203=ETH
 * tm_port_header_type_in_40=RCH_0
 * tm_port_header_type_out_40=ETH
 * ucode_port_40=RCY.0:core_1.40
 * appl_param_srv6_psp_enable=0
 * appl_param_srv6_reduce_enable=0
 *
 * Shell Commands:
 * ---------------
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/dnx/field/cint_field_utils.c
 * cint ../../../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_vlan_translate.c
 * cint ../../../../src/examples/dnx/utility/cint_dnx_util_srv6.c
 * cint ../../../../src/examples/dnx/tunnel/cint_dnx_srv6_gsid_full_flow_example.c
 *
 * cint;
 * int rv;
 * rv = srv6_basic_gsid_example(0,200,201,201,202,202,203,203,40,200,0);
 * print rv;
 * exit;
 *
 * Following is example of full flow using Egress USP and encapsulation Normal mode, of expected packets:
 *
 ****************************************************
 * NOTE - Send Only Packets Without PTCH for SRv6 ! *
 ****************************************************
 *
 * Srv6 GSID Ingress Node (Encap 4SIs)
 *
 * PacKeT TX raw port=200 DATA=000c000200000000070001000800450000350000000040003a46c08001017fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Received packets on unit 0 should be:
 *
 * Output Packet:
 *        000000cd1d00123456789a86dd
 *        60038ea3007d2b80 123456789abceeffffeecba987654321 0000dd001111dd012222dd023333dd03
 *        0408000303000000 0000aa001111aa012222aa023333aa03 0000bb001111bb012222bb023333bb03 0000cc001111cc012222cc023333cc03 0000dd001111dd012222dd023333dd03
 *        45000035000000003f003b46c08001017fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Srv6 GSID1 End-Point-1 Node (7 SIDs, Segment Left = 3)
 *
 * PacKeT TX raw port=201 DATA=00000000cd1d00123456789a86dd6000000000ad2b7f123456789abceeffffeecba987654321aaaabbbbcccc1111bb01000000000001040e00030600000001001600350070000000db070000000001001600350070000000db07000000010000aa001111aa012222aa023333aa030000bb001111bb012222bb023333bb030000cc001111cc012222cc023333cc0301001600350070000000db070000000501001600350070000000db070000000645000035000000003f003b46c08001017fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Received packets on unit 0 should be:
 *
 * Output Packet:
 *        00000001cd1d00213456789a86dd
 *        6000000000ad2b7e 123456789abceeffffeecba987654321 aaaabbbbcccc0000bb00000000000000
 *        040e000306000000 01001600350070000000db0700000000 01001600350070000000db0700000001 0000aa001111aa012222aa023333aa03 0000bb001111bb012222bb023333bb03 0000cc001111cc012222cc023333cc03 01001600350070000000db0700000005 01001600350070000000db0700000006
 *        45000035000000003f003b46c08001017fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Srv6 GSID0 End-Point-2 Node (7 SIDs, Segment Left = 3)
 *
 * PacKeT TX raw port=202 DATA=00000001cd1d00213456789a86dd6000000000ad2b7e123456789abceeffffeecba987654321aaaabbbbcccc0000bb00000000000000040e00030600000001001600350070000000db070000000001001600350070000000db07000000010000aa001111aa012222aa023333aa030000bb001111bb012222bb023333bb030000cc001111cc012222cc023333cc0301001600350070000000db070000000501001600350070000000db070000000645000035000000003f003b46c08001017fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Received packets on unit 0 should be:
 *
 * Output Packet:
 *        00000002cd1d00223456789a86dd
 *        6000000000ad2b7d 123456789abceeffffeecba987654321 aaaabbbbcccc3333aa03000000000003
 *        040e000206000000 01001600350070000000db0700000000 01001600350070000000db0700000001 0000aa001111aa012222aa023333aa03 0000bb001111bb012222bb023333bb03 0000cc001111cc012222cc023333cc03 01001600350070000000db0700000005 01001600350070000000db0700000006
 *        45000035000000003f003b46c08001017fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Srv6 Egress Node (7 SIDs)
 *
 * PacKeT TX raw port=203 DATA=00000002cd1d00223456789a86dd6000000000ad2b7e123456789abceeffffeecba987654321aaaabbbbcccc3333aa03000000000000040800000600000001001600350070000000db070000000001001600350070000000db07000000010000aa001111aa012222aa023333aa030000bb001111bb012222bb023333bb030000aa001111aa012222aa023333aa030000bb001111bb012222bb023333bb030000cc001111cc012222cc023333cc0345000035000000003f003b46c08001017fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Received packets on unit 0 should be:
 *
 * Output Packet:
 *        00000003cd1d00303456789a0800
 *        45000035000000007d00fd45c08001017fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 */

/** \brief List of SRv6 used instances */
enum srv6_instances_e
{
    INGRESS,
    ENDPOINT_GSID1,
    ENDPOINT_GSID0,
    EGRESS,
    RCH,
    ENDPOINT_GSID3
};

 int NOF_INSTANCES = ENDPOINT_GSID3+1;

/** \brief List of SRv6 used FEC ids */
enum srv6_fec_id_e
{
    INGRESS_SRH_FEC_ID,
    INGRESS_FIRST_SID_FEC_ID,
    END_POINT_1_FEC_ID,
    END_POINT_2_FEC_ID,
    EGRESS_FEC_ID
};

int NOF_FEC_ID = EGRESS_FEC_ID+1;

/** \brief List of SRv6 used VIDs */
enum srv6_vid_e
{
    END_POINT_1_VID,
    END_POINT_2_VID,
    EGRESS_VID,
    EGRESS_OUT_VID
};

int NOF_VID = EGRESS_OUT_VID+1;

/** \brief List of SRv6 used Global LIFs */
enum srv6_global_lifs_e
{
    INGRESS_SRH_GLOBAL_LIF,
    INGRESS_IP_TUNNEL_GLOBAL_LIF
};

int NOF_GLOBAL_LIFS=INGRESS_IP_TUNNEL_GLOBAL_LIF+1;

/** \brief List of SRv6 Encapsulated SIDs */
enum srv6_encapsulated_sids_e
{
    SID3,
    SID2,
    SID1,
    SID0
};

int NOF_SIDS = SID0+1;

/** \brief List of IPv6 Terminated DIPs */
enum srv6_terminated_dips_e
{
    DIP_ENDPOINT_GSID2,
    DIP_ENDPOINT_GSID1,
    DIP_ENDPOINT_GSID0,
    DIP_ENDPOINT_CLASSIC,
    DIP_EGRESS,
    DIP_EGRESS_EOC
};
int NOF_MYDIPS = DIP_EGRESS_EOC+1;

enum srv6_gsid_prefixes_e
{
    GSID_PREFIX_48B,
    GSID_PREFIX_64B
};
int NOF_GSID_PREFIXES = GSID_PREFIX_64B+1;

struct cint_srv6_tunnel_info_s
{
    int eth_rif_intf_in[NOF_INSTANCES];                        /* in RIF */
    int eth_rif_intf_out[NOF_INSTANCES-1];                     /* out RIF */
    bcm_mac_t intf_in_mac_address[NOF_INSTANCES];              /* mac for in RIF */
    bcm_mac_t intf_out_mac_address[NOF_INSTANCES];             /* mac for in RIF */
    bcm_mac_t arp_next_hop_mac[NOF_INSTANCES];                 /* mac for next hop */
    int vrf_in[NOF_INSTANCES];                                 /* VRF, resolved as Tunnel-InLif property*/
    int tunnel_arp_id[NOF_INSTANCES-1];                        /* Id for ARP entry */
    int tunnel_fec_id[NOF_FEC_ID];                             /* FEC id */
    int tunnel_global_lif[NOF_GLOBAL_LIFS];                    /* Global LIFs */
    int tunnel_vid[NOF_VID];                                   /* VID */
    bcm_ip_t route_ipv4_dip;                                   /* IPv4 Route DIP as next layer above SRv6 */
    bcm_ip6_t route_ipv6_dip;                                  /* IPv6 Route DIP as next layer above SRv6 */
    bcm_mac_t route_l2_da;                                     /* L2 Route DA as next layer above SRv6 in ISR node */
    bcm_ip6_t sids[NOF_SIDS];                                  /* SRv6 Encapsulated SIDs */
    bcm_ip6_t tunnel_ip6_dip[NOF_MYDIPS];                      /* IPv6 Terminated DIPs */
    bcm_ip6_t tunnel_ip6_dip_64b[NOF_MYDIPS];                  /* IPv6 Terminated DIPs in GSID 64b Prefix mode */
    bcm_ip6_t tunnel_ip6_sip;                                  /* IPv6 Tunnel SIP */
    bcm_ip6_t tunnel_ipv6_dip_mask;                            /* tunnel DIP mask*/
    int srh_next_protocol_ipv4;                                /* IPv4 as next protocol above SRv6, per IANA RFC */
    int srh_next_protocol_ipv6;                                /* IPv6 as next protocol above SRv6, per IANA RFC */
    int srh_next_protocol_eth;                                 /* ETH as next protocol above SRv6, per IANA RFC */
    int l2_termination_vsi;                                    /* When ETHoSRv6, in Extended Termination case, tunnel terminated LIF FODO (VSI) */
    bcm_mac_t l2_termination_mact_fwd_address;                 /* When ETHoSRv6, MACT host entry for Bridging FWD lookup */
    bcm_ip6_t tunnel_ip6_dip_p2p;                              /* IPv6 Tunnel SIP for P2P from cascaded lookup */
    bcm_ip6_t tunnel_ip6_dip_p2p_64b;                          /* IPv6 Tunnel SIP for P2P from cascaded lookup in GSID 64b Prefix mode */
    bcm_ip6_t ipv6_gsid_prefix_locator_mask[NOF_GSID_PREFIXES];/* IPv6 Tunnel MyDIP mask locator for GSID with Prefix 64b and 48b */
};


cint_srv6_tunnel_info_s cint_srv6_tunnel_info =
{
        /*
         * eth_rif_intf_in (VSI)
         */
         {15, 16, 17, 18, 19, 20},
        /*
         * eth_rif_intf_out --> we don't include eth-rif-out for Ingress node, we use instead VSI with MAC and VID of End-Point-1
         */
         {0, 110, 102, 103},
        /*
         * intf_in_mac_address
         */
         {{ 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 },  { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d }, { 0x00, 0x00, 0x00, 0x01, 0xcd, 0x1d },
          { 0x00, 0x00, 0x00, 0x02, 0xcd, 0x1d },  { 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0x11 }},
        /*
         * intf_out_mac_address
         */
         {{ 0x00, 0x12, 0x34, 0x56, 0x78, 0x9a }, { 0x00, 0x21, 0x34, 0x56, 0x78, 0x9a }, { 0x00, 0x22, 0x34, 0x56, 0x78, 0x9a },
          { 0x00, 0x30, 0x34, 0x56, 0x78, 0x9a }, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
        /*
         * arp_next_hop_mac
         */
         {{ 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d }, { 0x00, 0x00, 0x00, 0x01, 0xcd, 0x1d }, { 0x00, 0x00, 0x00, 0x02, 0xcd, 0x1d },
          { 0x00, 0x00, 0x00, 0x03, 0xcd, 0x1d }, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
        /*
         * In VRF
         */
         {1, 11, 12, 14, 14},
         /*
          * tunnel_arp_id
          */
         {0x6384, 0x6385, 0x6386, 0x6387},
        /*
         * tunnel_fec_id
         */
         {0xA711, 0xD711, 0xA001, 0xA002, 0xA003},
         /*
          * tunnel_global_lifs - SRH Base, IPv6 Tunnel respectively
          */
         {0x2230, 0x4231},
         /*
          * tunnel_vid - VID of End-Point-1 is used to create VSI for Ingress node to have MAC and VID instead of in ETH-RIF-OUT which is not present
          */
         {101, 102, 100, 104},
        /*
         * route_ipv4_dip when IPv4 is above SRv6
         */
        0x7fffff02 /* 127.255.255.02 */,
        /*
         * route_ipv6_dip when IPv6 is above SRv6
         */
        { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x13 },
        /*
         * route_l2_da when L2 is above SRv6
         */
        { 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C },
        /*
         * encapsulated sids
         */
        {{ 0x00, 0x00, 0xDD, 0x00, 0x11, 0x11, 0xDD, 0x01, 0x22, 0x22, 0xDD, 0x02, 0x33, 0x33, 0xDD, 0x03 },
         { 0x00, 0x00, 0xCC, 0x00, 0x11, 0x11, 0xCC, 0x01, 0x22, 0x22, 0xCC, 0x02, 0x33, 0x33, 0xCC, 0x03 },
         { 0x00, 0x00, 0xBB, 0x00, 0x11, 0x11, 0xBB, 0x01, 0x22, 0x22, 0xBB, 0x02, 0x33, 0x33, 0xBB, 0x03 },
         { 0x00, 0x00, 0xAA, 0x00, 0x11, 0x11, 0xAA, 0x01, 0x22, 0x22, 0xAA, 0x02, 0x33, 0x33, 0xAA, 0x03 }},
        /*
         * tunnel_ip6_dip
         */
        {{ 0xAA, 0xAA, 0xBB, 0xBB, 0xCC, 0xCC, 0x22, 0x22, 0xBB, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02 },
         { 0xAA, 0xAA, 0xBB, 0xBB, 0xCC, 0xCC, 0x11, 0x11, 0xBB, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 },
         { 0xAA, 0xAA, 0xBB, 0xBB, 0xCC, 0xCC, 0x00, 0x00, 0xBB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
         { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x13 },
         { 0xAA, 0xAA, 0xBB, 0xBB, 0xCC, 0xCC, 0x33, 0x33, 0xAA, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03 },
         { 0xAA, 0xAA, 0xBB, 0xBB, 0xCC, 0xCC, 0x44, 0x44, 0xDD, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }},
         /*
          * tunnel_ip6_dip_64b
          */
         {{ 0xAA, 0xAA, 0xBB, 0xBB, 0xCC, 0xCC, 0xDD, 0xDD, 0x22, 0x22, 0xBB, 0x02, 0x00, 0x00, 0x00, 0x02 },
          { 0xAA, 0xAA, 0xBB, 0xBB, 0xCC, 0xCC, 0xDD, 0xDD, 0x11, 0x11, 0xBB, 0x01, 0x00, 0x00, 0x00, 0x01 },
          { 0xAA, 0xAA, 0xBB, 0xBB, 0xCC, 0xCC, 0xDD, 0xDD, 0x00, 0x00, 0xBB, 0x00, 0x00, 0x00, 0x00, 0x00 },
          { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x13 },
          { 0xAA, 0xAA, 0xBB, 0xBB, 0xCC, 0xCC, 0xDD, 0xDD, 0x33, 0x33, 0xAA, 0x03, 0x00, 0x00, 0x00, 0x03 },
          { 0xAA, 0xAA, 0xBB, 0xBB, 0xCC, 0xCC, 0xDD, 0xDD, 0x44, 0x44, 0xDD, 0x04, 0x00, 0x00, 0x00, 0x00 }},
         /*
          * tunnel_ip6_sip
          */
        { 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xee, 0xff, 0xff, 0xee, 0xcb, 0xa9, 0x87, 0x65, 0x43, 0x21 },
        /*
         * GSID locator mask is Prefix 48b + GSIDs MSB 16b = 64b
         */
        { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
       /*
        * next protocol above SRv6 which is IPv4
        */
        4,
        /*
         * next protocol above SRv6 which is IPv6
         */
         41,
         /*
          * next protocol above SRv6 which is L2
          */
          143,
         /*
          * l2_termination_vsi - when ETHoSRv6 tunnel terminated LIF FODO (VSI)
          */
         2,
         /*
          * when ETHoSRv6, MACT host entry for Bridging FWD lookup
          */
         { 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c },
         /*
          * tunnel_ip6_dip_p2p
          */
         { 0xAA, 0xAA, 0xBB, 0xBB, 0xCC, 0xCC, 0x11, 0x11, 0xBB, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
         /*
          * tunnel_ip6_dip_p2p_64b
          */
         { 0xAA, 0xAA, 0xBB, 0xBB, 0xCC, 0xCC, 0xDD, 0xDD, 0x11, 0x11, 0xBB, 0x02, 0x00, 0x00, 0x00, 0x00 },
         /*
          * ipv6_gsid_prefix_locator_mask
          *   - in case of Prefix 48b, locator is 48+16 = 64b
          *   - in case of Prefix 64b, locator is 64+16 = 80b
          */
         {
             {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
             {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00}
         }

};

int default_tunnel_for_gsid_cascading = 0;
/*
 * Set In-Port default ETH-RIF:
 * - in_port: Incoming port ID
 * - in_port: Incoming VLAN ID
 * - eth_rif: ETH-RIF
 */
int
in_port_vid_intf_set(
    int unit,
    int in_port,
    int in_vid,
    int eth_rif)
{
    bcm_vlan_port_t vlan_port;
    int rc;
    char *proc_name;

    proc_name = "in_port_vid_intf_set";
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.port = in_port;
    vlan_port.match_vlan = in_vid;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    vlan_port.vsi = eth_rif;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;

    rc = bcm_vlan_port_create(unit, vlan_port);
    if (rc != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_vlan_port_create\n", proc_name);
        return rc;
    }

    printf("%s(): port = %d, in_lif = 0x%08X\n", proc_name, vlan_port.port, vlan_port.vlan_port_id);
    rc = bcm_vlan_gport_add(unit, in_vid, in_port, 0);
    if (rc != BCM_E_NONE)
    {
        printf("Error in %s(): bcm_vlan_gport_add \n", proc_name);
        return rc;
    }

    return rc;
}

/*
 * Configures MyDip of an IPv6 address in an SRv6 tunnel
 */
int
srv6_tunnel_termination(
                    int unit,
                    int end_point_id,
                    int is_egress_tunnel,
                    int vrf,
                    int vsi,          /** if not 0, after Tunnel Terminate, configure the Termination Lif's FODO to VSI and not VRF*/
                    int is_gsid,
                    int is_gsid_eoc,
                    int default_lif_for_cascading,
                    int is_gsid_prefix_64b)
{

    bcm_ip6_t ip6_classic_mask = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    /** GSID locator mask is Prefix size (48/6b) + GSIDs MSB 16b = 64/80b */
    bcm_ip6_t ip6_gsid_prefix_locator_mask = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    /** GSID function mask*/
    bcm_ip6_t ip6_gsid_prefix_function_mask = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00};
    bcm_ip6_t ip6_gsid_prefix_64b_function_mask = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00};

    bcm_ip6_t ip6_dip  = {0};
    bcm_ip6_t ip6_sip  = {0};

    print(cint_srv6_tunnel_info);

    sal_memcpy(ip6_sip, cint_srv6_tunnel_info.tunnel_ip6_sip, 16);
    if (is_gsid_prefix_64b)
    {
        sal_memcpy(ip6_dip, cint_srv6_tunnel_info.tunnel_ip6_dip_64b[end_point_id], 16);
    }
    else
    {
        sal_memcpy(ip6_dip, cint_srv6_tunnel_info.tunnel_ip6_dip[end_point_id], 16);
    }

    if(default_lif_for_cascading)
    {
        if (is_gsid_prefix_64b)
        {
            sal_memcpy(ip6_dip, cint_srv6_tunnel_info.tunnel_ip6_dip_p2p_64b, 16);
        }
        else
        {
            sal_memcpy(ip6_dip, cint_srv6_tunnel_info.tunnel_ip6_dip_p2p, 16);
        }
    }

    bcm_tunnel_terminator_t tunnel_term_set;
    l3_ingress_intf ingress_rif;
    int rv;

    /*
     * Configure the keys for the resulting LIF on termination (SIP, DIP, VRF)
     */
    bcm_tunnel_terminator_t_init(&tunnel_term_set);
    tunnel_term_set.type = bcmTunnelTypeSR6;
    sal_memcpy(tunnel_term_set.dip6, ip6_dip, 16);
    sal_memcpy(tunnel_term_set.sip6, ip6_sip, 16);

    sal_memcpy(tunnel_term_set.dip6_mask, ip6_classic_mask, 16);
    /** if GSID, use a 64b locator mask on DIP, apply GSID flags, and check if GSID is EOC */
    if (is_gsid)
    {
        if (is_gsid_prefix_64b)
        {
            sal_memcpy(tunnel_term_set.dip6_mask, cint_srv6_tunnel_info.ipv6_gsid_prefix_locator_mask[GSID_PREFIX_64B], 16);
        }
        else
        {
            sal_memcpy(tunnel_term_set.dip6_mask, cint_srv6_tunnel_info.ipv6_gsid_prefix_locator_mask[GSID_PREFIX_48B], 16);
        }

        tunnel_term_set.flags = BCM_TUNNEL_TERM_GENERALIZED_SEGMENT_ID;

    }

    if(default_lif_for_cascading)
    {
        if (is_gsid_prefix_64b)
        {
            sal_memcpy(tunnel_term_set.dip6_mask, ip6_gsid_prefix_64b_function_mask, 16);
        }
        else
        {
            sal_memcpy(tunnel_term_set.dip6_mask, ip6_gsid_prefix_function_mask, 16);
        }
    }

    sal_memcpy(tunnel_term_set.sip6_mask, ip6_classic_mask, 16);
    tunnel_term_set.vrf = vrf;
    /* Following is for configuring for the IPv6 LIF, to increase the strength of its TTL
     * so that terminated IPv6 header's TTL would get to sysh, from which the IPv6 TTL decrease
     * would decrease from
     */
    tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelPipe;

    if(is_egress_tunnel)
    {
        tunnel_term_set.flags |= BCM_TUNNEL_TERM_EXTENDED_TERMINATION;
        /* If it's ESR USP, shortpipe and uniform are supported
         * Uniform: take QoS info from SRv6 tunnel
         * Shortpipe: take QoS info from fwd layer
         */
        tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelUniform;
        tunnel_term_set.ingress_qos_model.ingress_phb = bcmQosIngressModelUniform;
        tunnel_term_set.ingress_qos_model.ingress_remark = bcmQosIngressModelUniform;
    }

    /** 'vsi' - if configured, it sets the termination LIF property FODO to be this VLAN value */
    if (vsi)
    {
        tunnel_term_set.type = bcmTunnelTypeEthSR6;
        tunnel_term_set.vlan = vsi;
    }

    if(default_lif_for_cascading)
    {
        tunnel_term_set.type = bcmTunnelTypeCascadedFunct;
        tunnel_term_set.default_tunnel_id = default_lif_for_cascading;
        tunnel_term_set.flags &= ~(BCM_TUNNEL_TERM_GENERALIZED_SEGMENT_ID);
        tunnel_term_set.flags |= BCM_TUNNEL_TERM_CROSS_CONNECT;
    }

    if (is_gsid_eoc)
    {
        tunnel_term_set.flags |= BCM_TUNNEL_TERM_GENERALIZED_SEGMENT_ID_END_OF_COMPRESSION;
    }


    rv = bcm_tunnel_terminator_create(unit, &tunnel_term_set);
    if(rv != BCM_E_NONE)
    {
        int end_idx =  end_point_id;
        printf("Error bcm_tunnel_terminator_create. rv = %d, end-point = %d\n", rv, end_idx);
        return rv;
    }

    if(default_lif_for_cascading)
    {
        /** add cross connect info on the Cascaded LIF */
        bcm_vswitch_cross_connect_t gports;
        bcm_vswitch_cross_connect_t_init(&gports);
        gports.port1 = tunnel_term_set.tunnel_id;
        BCM_GPORT_FORWARD_PORT_SET(gports.port2, cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_1_FEC_ID]);
        gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
        rv = bcm_vswitch_cross_connect_add(unit, &gports);
        if(rv != BCM_E_NONE)
        {
            printf("Error bcm_vswitch_cross_connect_add. rv = %d \n", rv);
            return rv;
        }

        return BCM_E_NONE;

    }

    if(end_point_id == DIP_ENDPOINT_GSID1)
    {
        default_tunnel_for_gsid_cascading = tunnel_term_set.tunnel_id;
    }
    /*
     * Configure the termination resulting LIF's FODO to VRF
     *  - not needed if resulting LIF's FODO is to be VSI (when ETHoSRv6), because
     *    in this case it's set by 'bcm_tunnel_terminator_create' above with 'vlan' property
     */
    if (!vsi)
    {
        l3_ingress_intf_init(&ingress_rif);
        ingress_rif.vrf = vrf;
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(ingress_rif.intf_id, tunnel_term_set.tunnel_id);
        rv = intf_ingress_rif_set(unit, &ingress_rif);
        if (rv != BCM_E_NONE)
        {
            printf("Error intf_ingress_rif_set. rv = %d \n", rv);
            return rv;
        }
    }
    return rv;
}


int
dnx_srv6_update_fecs_id(
        int unit)
{
    int first_fec;
    int rv = BCM_E_NONE;
    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &first_fec);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }
    cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID] = first_fec++;
    cint_srv6_tunnel_info.tunnel_fec_id[EGRESS_FEC_ID] = first_fec++;
    cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_1_FEC_ID] = first_fec++;
    cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_2_FEC_ID]=first_fec;
    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 1, 0, &first_fec);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }
    cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_FIRST_SID_FEC_ID] = first_fec;
    return rv;
}

int
set_eth_arp_ac_untagged(int unit,
                        int arp_id)
{
    int rv;
    /** convert 1st SID global-LIF id to GPORT */
    bcm_gport_t gport_arp_id;
    BCM_GPORT_SUB_TYPE_L3_VLAN_TRANSLATION_SET(gport_arp_id, arp_id);
    BCM_GPORT_VLAN_PORT_ID_SET(gport_arp_id, gport_arp_id);

    rv = vlan_translate_ive_eve_translation_set(unit, gport_arp_id,
                                                0,                                0,
                                                bcmVlanActionNone,                bcmVlanActionNone,
                                                0,                                0,
                                                0, 0, 0);
                                                /* vlan tag-format as per enum in  appl_ref_vlan_init.c
                                                 * dnx_pp_vlan_tag_format_default_t struct:
                                                 * DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_C_TAG = 8 */

    return rv;
}

int
srv6_basic_gsid_example(
                        int unit,
                        int in_port,              /** Incoming port of SRV6 Ingress Tunnel */
                        int out_port,             /** Outgoing port of SRV6 Ingress Tunnel */
                        int in_port_end1,         /** Incoming port of SRV6 End Point 1 */
                        int out_port_end1,        /** Outgoing port of SRV6 End Point 1 */
                        int in_port_end2,         /** Incoming port of SRV6 End Point 2 */
                        int out_port_end2,        /** Outgoing port of SRV6 End Point 2 */
                        int in_port_egress,       /** Incoming port of SRV6 Egress */
                        int rch_port,             /** Rycling port number for 1st Pass USP in Egress */
                        int out_port_egress,      /** Outgoing port of SRV6 Egress */
                        uint8 next_header_is_eth, /** if set, header above SRv6 is ETH */
                        uint8 is_gsid_eoc,        /** if set, the last GSID in SID is EOC */
                        uint8 is_gsid_prefix_64b) /** if set, the last GSID Prefix is 64b, otherwise 48b */
{

    int rv;
    char *proc_name = "dnx_basic_example_srv6_full_flow";


    /*
     * 0. Set legal FEC IDs
     */
    rv = dnx_srv6_update_fecs_id(unit);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, dnx_srv6_update_fecs_id\n",proc_name);
        return rv;
    }

    /*
     * 1. Set In-Port to In ETh-RIF
     */

    /** SRV6 Ingress Tunnel Eth RIF based on Port only - configuring Port match and resulting VSI of the IN_LIF */
    rv = in_port_intf_set(unit, in_port, cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set intf_in\n",proc_name);
        return rv;
    }

    /** End-Point-1 Eth RIF based on Port only - configuring Port match and resolting VSI of the IN_LIF */
    rv = in_port_intf_set(unit, in_port_end1, cint_srv6_tunnel_info.eth_rif_intf_in[ENDPOINT_GSID1]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set in_port_end1\n",proc_name);
        return rv;
    }

    /** End-Point-2 Eth RIF based on Port only - configuring Port match and resolting VSI of the IN_LIF */
    rv = in_port_intf_set(unit, in_port_end2, cint_srv6_tunnel_info.eth_rif_intf_in[ENDPOINT_GSID0]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set in_port_end2\n",proc_name);
        return rv;
    }

    /** Egress Eth RIF based on Port only - configuring Port match and resolting VSI of the IN_LIF */
    rv = in_port_intf_set(unit, in_port_egress, cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set in_port = %d, intf_in = %d, err_id = %d\n", proc_name, in_port_egress, cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS],
               rv);
        return rv;
    }

    /** Egress to emulate GSID3 with VID to reuse Egress Node (with SI==3) with Port + VID */
    rv = in_port_vid_intf_set(unit, in_port_egress, cint_srv6_tunnel_info.tunnel_vid[EGRESS_VID], cint_srv6_tunnel_info.eth_rif_intf_in[ENDPOINT_GSID3]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set in_port = %d, intf_in = %d, err_id = %d\n", proc_name, in_port_egress, cint_srv6_tunnel_info.eth_rif_intf_in[ENDPOINT_GSID3],
               rv);
        return rv;
    }

    /*
     * 2. Set Out-Port default properties, in case of ARP+AC no need
     */

    /** In SRV6 Ingress and End-Point-1 use the same out_port, (End-Point-1 and End-Point-2 receive from same Port, with different VLAN) */
    rv = out_port_set(unit, out_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set intf_out out_port of SRV6 Ingress Tunnel %d\n",proc_name,out_port);
        return rv;
    }

    /** End-Point-2 out_port is different from above, to initiate Egress Tunnel */
    rv = out_port_set(unit, out_port_end2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set intf_out out_port of End-Point-2 %d\n",proc_name,out_port_end2);
        return rv;
    }

    /** Egress out_port */
    rv = out_port_set(unit, out_port_egress);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_intf_set out_port = %d, err_id = %d\n", proc_name, out_port_egress, rv);
        return rv;
    }

    /*
     * 3. Create ETH-RIF and set its properties
     */

    /** SRV6 Ingress Tunnel My-MAC */
    rv = intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS], cint_srv6_tunnel_info.intf_in_mac_address[INGRESS]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in %d\n",proc_name,cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS]);
        return rv;
    }

    /** SRV6 Ingress Tunnel out-port SA - Here creating not Out-RIF, we don't need it SRv6 Tunnel, but VSI (with SA MAC) and its VID of END-Point1, to tie it to ARP+AC  */
    rv = intf_eth_rif_create(unit, cint_srv6_tunnel_info.tunnel_vid[END_POINT_1_VID], cint_srv6_tunnel_info.intf_out_mac_address[INGRESS]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
        return rv;
    }

    /** End-Point-1 My-MAC - set to SRV6 Ingress Tunnel next hop MAC */
    rv = intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_in[ENDPOINT_GSID1], cint_srv6_tunnel_info.intf_in_mac_address[ENDPOINT_GSID1]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in %d\n",proc_name,cint_srv6_tunnel_info.eth_rif_intf_in[ENDPOINT_GSID1]);
        return rv;
    }

    /** End-Point-1 out-port SA  */
    rv = intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_out[ENDPOINT_GSID1], cint_srv6_tunnel_info.intf_out_mac_address[ENDPOINT_GSID1]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
        return rv;
    }

    /** End-Point-2 My-MAC - set to End-Point-1 next hop MAC */
    rv = intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_in[ENDPOINT_GSID0], cint_srv6_tunnel_info.intf_in_mac_address[ENDPOINT_GSID0]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in %d\n",proc_name,cint_srv6_tunnel_info.eth_rif_intf_in[ENDPOINT_GSID0]);
        return rv;
    }

    /** End-Point-2 out-port SA  */
    rv = intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_out[ENDPOINT_GSID0], cint_srv6_tunnel_info.intf_out_mac_address[ENDPOINT_GSID0]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
        return rv;
    }

    /** Egress My-MAC - set to End-Point-2 next hop MAC */
    rv = intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS], cint_srv6_tunnel_info.intf_in_mac_address[EGRESS]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in = %d, err_id = %d\n", proc_name, cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS], rv);
        return rv;
    }

    /** RCH Eth_Rif My MAC  */
    rv = intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_in[RCH], cint_srv6_tunnel_info.intf_in_mac_address[RCH]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in %d\n",proc_name,cint_srv6_tunnel_info.eth_rif_intf_in[RCH]);
        return rv;
    }

    /** Egress out-port SA  */
    rv = intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_out[EGRESS], cint_srv6_tunnel_info.intf_out_mac_address[EGRESS]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out = %d, err_id = %d\n", proc_name, cint_srv6_tunnel_info.eth_rif_intf_out[EGRESS], rv);
        return rv;
    }

    /** EndPoint GSID3, as Egress My-MAC */
    rv = intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_in[ENDPOINT_GSID3], cint_srv6_tunnel_info.intf_in_mac_address[EGRESS]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in = %d, err_id = %d\n", proc_name, cint_srv6_tunnel_info.eth_rif_intf_in[ENDPOINT_GSID3], rv);
        return rv;
    }

    /*
     * 4. Set Incoming ETH-RIF properties
     */

    l3_ingress_intf ingress_rif;

    /** SRV6 Ingress Tunnel VRF in case of MyMac */
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = cint_srv6_tunnel_info.vrf_in[INGRESS];
    ingress_rif.intf_id = cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS];
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create SRV6 Ingress Tunnel\n",proc_name);
        return rv;
    }

    /** End-Point-1 VRF in case of MyMac */
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = cint_srv6_tunnel_info.vrf_in[ENDPOINT_GSID1];
    ingress_rif.intf_id = cint_srv6_tunnel_info.eth_rif_intf_in[ENDPOINT_GSID1];
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create End-Point-1\n",proc_name);
        return rv;
    }

    /** End-Point-2 VRF in case of MyMac */
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = cint_srv6_tunnel_info.vrf_in[ENDPOINT_GSID0];
    ingress_rif.intf_id = cint_srv6_tunnel_info.eth_rif_intf_in[ENDPOINT_GSID0];
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create End-Point-2\n",proc_name);
        return rv;
    }

    /** Egress Tunnel - 1st pass VRF in case of MyMac */
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = cint_srv6_tunnel_info.vrf_in[EGRESS];
    ingress_rif.intf_id = cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS];
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in = %d, vrf = %d, err_id = %d\n", proc_name, cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS], cint_srv6_tunnel_info.vrf_in[EGRESS], rv);
        return rv;
    }

    /** Egress Tunnel - RCH - 2nd pass VRF  */
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = cint_srv6_tunnel_info.vrf_in[RCH];
    ingress_rif.intf_id = cint_srv6_tunnel_info.eth_rif_intf_in[RCH];
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
        return rv;
    }

    /** GSID3 Endpoint VRF as in Engress Node in case of MyMac */
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = cint_srv6_tunnel_info.vrf_in[EGRESS];
    ingress_rif.intf_id = cint_srv6_tunnel_info.eth_rif_intf_in[ENDPOINT_GSID3];
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in = %d, vrf = %d, err_id = %d\n", proc_name, cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS], cint_srv6_tunnel_info.vrf_in[EGRESS], rv);
        return rv;
    }

    /*
     * 5. Create ARP and set its properties - ARP is Assumed be + AC. Create VLAN ID editing for each case.
     *    We use same port out for the End-Points, therefore the VLAN ID is what separates these cases.
     */

    uint32 flags2 = 0;

    flags2 |= BCM_L3_FLAGS2_VLAN_TRANSLATION;

    /*
     * SRV6 Ingress Tunnel ARP create, with VLAN ID of expected in End-Point-1 - adding to ARP+AC the VSI (created of tunne_vid of end-point-1) which includes the SA MAC,
     * cause in Ingress node, ETH-RIF out is not present, and VSI will come from ARP+AC
     * -using no VLAN
     */


    rv = l3__egress_only_encap__create_inner(unit, 0, &cint_srv6_tunnel_info.tunnel_arp_id[INGRESS], cint_srv6_tunnel_info.arp_next_hop_mac[INGRESS], cint_srv6_tunnel_info.tunnel_vid[END_POINT_1_VID], 0, flags2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object ARP only, in SRV6 Ingress Tunnel\n",proc_name);
        return rv;
    }

    /** Set ETH encap to Untagged */
    rv = set_eth_arp_ac_untagged(unit, cint_srv6_tunnel_info.tunnel_arp_id[INGRESS]);


    /** End-Point-1 ARP create, with VLAN ID of expected in End-Point-2*/
    rv = l3__egress_only_encap__create_inner(unit, 0, &cint_srv6_tunnel_info.tunnel_arp_id[ENDPOINT_GSID1], cint_srv6_tunnel_info.arp_next_hop_mac[ENDPOINT_GSID1], cint_srv6_tunnel_info.tunnel_vid[END_POINT_2_VID], 0, flags2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object ARP only, in End-Point-1\n",proc_name);
        return rv;
    }

    /** Set ETH encap to Untagged */
    rv = set_eth_arp_ac_untagged(unit, cint_srv6_tunnel_info.tunnel_arp_id[ENDPOINT_GSID1]);

    /** End-Point-2 ARP create, with VLAN ID of Egress Tunnel, however there we will look on only the Port */
    rv = l3__egress_only_encap__create_inner(unit, 0, &cint_srv6_tunnel_info.tunnel_arp_id[ENDPOINT_GSID0], cint_srv6_tunnel_info.arp_next_hop_mac[ENDPOINT_GSID0], cint_srv6_tunnel_info.tunnel_vid[EGRESS_VID], 0, flags2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object ARP only, in End-Point-2\n",proc_name);
        return rv;
    }

    /** Set ETH encap to Untagged */
    rv = set_eth_arp_ac_untagged(unit, cint_srv6_tunnel_info.tunnel_arp_id[ENDPOINT_GSID0]);

    /** Egress ARP create, with VLAN ID of Egress Tunnel, however there we will look on only the Port */
    rv = l3__egress_only_encap__create_inner(unit, 0, &cint_srv6_tunnel_info.tunnel_arp_id[EGRESS], cint_srv6_tunnel_info.arp_next_hop_mac[EGRESS], cint_srv6_tunnel_info.tunnel_vid[EGRESS_OUT_VID], 0,flags2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object ARP only: encap_id = %d, vsi = %d, err_id = %d\n", proc_name,
                cint_srv6_tunnel_info.tunnel_arp_id[EGRESS], cint_srv6_tunnel_info.tunnel_vid[EGRESS_OUT_VID], rv);
        return rv;
    }

    /** Set ETH encap to Untagged */
    rv = set_eth_arp_ac_untagged(unit, cint_srv6_tunnel_info.tunnel_arp_id[EGRESS]);

    /*
     * 6. SRV6 FEC Entries
     */

    bcm_gport_t gport;

    int srv6_basic_lif_encode;
    int encoded_fec1;
    BCM_L3_ITF_SET(srv6_basic_lif_encode, BCM_L3_ITF_TYPE_LIF, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_SRH_GLOBAL_LIF]);
    BCM_GPORT_FORWARD_PORT_SET(gport, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_FIRST_SID_FEC_ID]);

    rv = l3__egress_only_fec__create_inner(unit, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID], 0 , srv6_basic_lif_encode, gport, 0, 0,
                                           0, 0,&encoded_fec1);

    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object FEC_1 only\n",proc_name);
        return rv;
    }

    int srv6_first_lif_encode;
    int encoded_fec2;
    BCM_L3_ITF_SET(srv6_first_lif_encode, BCM_L3_ITF_TYPE_LIF, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_IP_TUNNEL_GLOBAL_LIF]);
    BCM_GPORT_LOCAL_SET(gport, out_port);
    rv = l3__egress_only_fec__create_inner(unit, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_FIRST_SID_FEC_ID], 0 , srv6_first_lif_encode, gport, BCM_L3_2ND_HIERARCHY, 0,
                                           0, 0,&encoded_fec2);

    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC_2 only, fec\n");
        return rv;
    }

    /** End-Point-1 FEC Entry - tying end-point-1 OUT-RIF with its OUT_Port, (ARP - DA and VLAN ID) */
    BCM_GPORT_LOCAL_SET(gport, out_port_end1);
    rv = l3__egress_only_fec__create(unit, cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_1_FEC_ID], cint_srv6_tunnel_info.eth_rif_intf_out[ENDPOINT_GSID1], cint_srv6_tunnel_info.tunnel_arp_id[ENDPOINT_GSID1], gport, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf
            ("%s(): Error, create egress object FEC only for End-Point-1: fec = %d, intf_out = %d, encap_id = %d, out_port = %d, err_id = %d\n",
             proc_name, cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_1_FEC_ID], cint_srv6_tunnel_info.eth_rif_intf_out[ENDPOINT_GSID1], cint_srv6_tunnel_info.tunnel_arp_id[ENDPOINT_GSID1], out_port_end1, rv);
        return rv;
    }

    /** End-Point-2 FEC Entry - tying end-point-2 OUT-RIF with its OUT_Port, (ARP - DA and VLAN ID) */
    BCM_GPORT_LOCAL_SET(gport, out_port_end2);
    rv = l3__egress_only_fec__create(unit, cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_2_FEC_ID], cint_srv6_tunnel_info.eth_rif_intf_out[ENDPOINT_GSID0], cint_srv6_tunnel_info.tunnel_arp_id[ENDPOINT_GSID0], gport, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf
            ("%s(): Error, create egress object FEC only for End-Point-2: fec = %d, intf_out = %d, encap_id = %d, out_port = %d, err_id = %d\n",
             proc_name, cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_2_FEC_ID], cint_srv6_tunnel_info.eth_rif_intf_out[ENDPOINT_GSID0], cint_srv6_tunnel_info.tunnel_arp_id[ENDPOINT_GSID0], out_port_end2, rv);
        return rv;
    }

    /*
     * On 1st Pass Egress, no FEC entry  because no more SIDs left to enter FWD on and then FEC,
     * in last termination stage, there's LIF P2P which gives the destination right away
     */


    /** Egress 2nd Pass FEC Entry - due to Forwarding on IPv4 Layer above SRv6  */
    int egress_encoded_fec1;
    BCM_GPORT_LOCAL_SET(gport, out_port_egress);
    rv = l3__egress_only_fec__create_inner(unit, cint_srv6_tunnel_info.tunnel_fec_id[EGRESS_FEC_ID], 0 , cint_srv6_tunnel_info.tunnel_arp_id[EGRESS], gport, 0, 0, 0, 0,&egress_encoded_fec1);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object FEC only\n",proc_name);
        return rv;
    }

    /*
     * 7. Add route entry - we work with specific DIP addresses, so we don't need to add best match route entries.
     */

    /*
     * 8. Add host entry
     */

    /** SRV6 Tunnel Ingress Forwarding for IPv4 as next header over SRv6 to FEC Hierarchy to bring out IPV6, SRH, SIDs - not including OUT-ETH-RIF entry */
    BCM_GPORT_FORWARD_PORT_SET(gport, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID]);
    rv = add_host_ipv4(unit, cint_srv6_tunnel_info.route_ipv4_dip, cint_srv6_tunnel_info.vrf_in[INGRESS], 0, cint_srv6_tunnel_info.tunnel_arp_id[INGRESS], gport);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function add_host_ipv4() for SRV6 Ingress Tunnel\n",proc_name);
        return rv;
    }

    /** SRV6 Tunnel Ingress Forwarding for IPv6 as next header over SRv6 to FEC Hierarchy to bring out IPV6, SRH, SIDs - not including OUT-ETH-RIF entry */
    rv = add_host_ipv6_encap_dest(unit, cint_srv6_tunnel_info.route_ipv6_dip, cint_srv6_tunnel_info.vrf_in[INGRESS], 0, cint_srv6_tunnel_info.tunnel_arp_id[INGRESS], gport);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function add_host_ipv6_encap_dest() for SRV6 Ingress Tunnel\n",proc_name);
        return rv;
    }

    /** SRV6 Tunnel Ingress Forwarding in MACT for L2 as next header over SRv6 to FEC Hierarchy to bring out IPV6, SRH, SIDs - not including OUT-ETH-RIF entry */
    bcm_l2_addr_t l2_addr_ing;

    bcm_l2_addr_t_init(&l2_addr_ing, cint_srv6_tunnel_info.route_l2_da, cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS]); /* configuring DA address and VSI as keys in the MACT */
    l2_addr_ing.flags = BCM_L2_STATIC;      /* static entry */
    l2_addr_ing.port = gport;               /* destination of 1st FEC */
    /** encap id of ARP+AC of ingress node - making sure that the encap_id is set as OUTLIF and not EEI (difference is bit 30 - below macro sets as OUTLIF) */
    BCM_FORWARD_ENCAP_ID_VAL_SET(l2_addr_ing.encap_id, BCM_FORWARD_ENCAP_ID_TYPE_OUTLIF, BCM_FORWARD_ENCAP_ID_OUTLIF_USAGE_GENERAL, cint_srv6_tunnel_info.tunnel_arp_id[INGRESS]);
    rv = bcm_l2_addr_add(unit, &l2_addr_ing);

    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l2_addr_add\n");
        return rv;
    }

    /** add End-Point-2's as host to do forwarding on to FEC */
    if (is_gsid_prefix_64b)
    {
        rv = add_host_ipv6(unit, cint_srv6_tunnel_info.tunnel_ip6_dip_64b[DIP_ENDPOINT_GSID0], cint_srv6_tunnel_info.vrf_in[ENDPOINT_GSID1], cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_1_FEC_ID]);
    }
    else
    {
        rv = add_host_ipv6(unit, cint_srv6_tunnel_info.tunnel_ip6_dip[DIP_ENDPOINT_GSID0], cint_srv6_tunnel_info.vrf_in[ENDPOINT_GSID1], cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_1_FEC_ID]);
    }

    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function add_host_ipv6 for GSID1: vrf = %d, fec = %d, err_id = %d\n", proc_name, cint_srv6_tunnel_info.vrf_in[ENDPOINT_GSID1], cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_1_FEC_ID], rv);
        return rv;
    }

    /* if last GSID is EOC, from last termination stage the flow is classic Endpoint, so lookup of Classic Endpoint,
     * else Egress dip (which is next SID's 1st GSID)
     */
    if (is_gsid_eoc)
    {
        /** add Egress as host to do forwarding on to FEC */
        if (is_gsid_prefix_64b)
        {
            rv = add_host_ipv6(unit, cint_srv6_tunnel_info.tunnel_ip6_dip_64b[DIP_ENDPOINT_CLASSIC], cint_srv6_tunnel_info.vrf_in[ENDPOINT_GSID0], cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_2_FEC_ID]);
        }
        else
        {
            rv = add_host_ipv6(unit, cint_srv6_tunnel_info.tunnel_ip6_dip[DIP_ENDPOINT_CLASSIC], cint_srv6_tunnel_info.vrf_in[ENDPOINT_GSID0], cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_2_FEC_ID]);
        }

        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in function add_host_ipv6 for GSID0 is EOC: vrf = %d, fec = %d, err_id = %d\n", proc_name, cint_srv6_tunnel_info.vrf_in[ENDPOINT_GSID0], cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_2_FEC_ID], rv);
            return rv;
        }
    }
    else
    {
        /** add Egress as host to do forwarding on to FEC */
        if (is_gsid_prefix_64b)
        {
            rv = add_host_ipv6(unit, cint_srv6_tunnel_info.tunnel_ip6_dip_64b[DIP_EGRESS], cint_srv6_tunnel_info.vrf_in[ENDPOINT_GSID0], cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_2_FEC_ID]);
        }
        else
        {
            rv = add_host_ipv6(unit, cint_srv6_tunnel_info.tunnel_ip6_dip[DIP_EGRESS], cint_srv6_tunnel_info.vrf_in[ENDPOINT_GSID0], cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_2_FEC_ID]);
        }

        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in function add_host_ipv6 for GSID0 none EOC: vrf = %d, fec = %d, err_id = %d\n", proc_name, cint_srv6_tunnel_info.vrf_in[ENDPOINT_GSID0], cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_2_FEC_ID], rv);
            return rv;
        }
    }

    /** just to add Endpoint GSID3 case of SI==3 to SI==2 with VRF of "Egress Node SI==3" & FEC of ENDPOINT_GSID1 */
    if (is_gsid_prefix_64b)
    {
        rv = add_host_ipv6(unit, cint_srv6_tunnel_info.tunnel_ip6_dip_64b[DIP_ENDPOINT_GSID2], cint_srv6_tunnel_info.vrf_in[EGRESS], cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_1_FEC_ID]);
    }
    else
    {
        rv = add_host_ipv6(unit, cint_srv6_tunnel_info.tunnel_ip6_dip[DIP_ENDPOINT_GSID2], cint_srv6_tunnel_info.vrf_in[EGRESS], cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_1_FEC_ID]);
    }

    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function add_host_ipv6 for GSID1: vrf = %d, fec = %d, err_id = %d\n", proc_name, cint_srv6_tunnel_info.vrf_in[EGRESS], cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_1_FEC_ID], rv);
        return rv;
    }

    /** No Egress 1-Pass host, because no more SIDs left to enter FWD on, in last termination stage there's LIF P2P which gives the destination right away */

    int _l3_itf;
    BCM_L3_ITF_SET(&_l3_itf, BCM_L3_ITF_TYPE_FEC, cint_srv6_tunnel_info.tunnel_fec_id[EGRESS_FEC_ID]);

    /** add Egress 2nd-Pass, IPv4 Packet DIP as host to do forwarding on to FEC when IPv4 is next header over SRv6*/
    rv = add_host_ipv4(unit, cint_srv6_tunnel_info.route_ipv4_dip, cint_srv6_tunnel_info.vrf_in[RCH], _l3_itf, cint_srv6_tunnel_info.eth_rif_intf_out[EGRESS], 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function add_host_ipv4(), \n",proc_name);
        return rv;
    }

    /** add Egress 2nd-Pass, IPv6 Packet DIP as host to do forwarding on to FEC when IPv6 is next header over SRv6 */
    rv = add_host_ipv6_encap_dest(unit, cint_srv6_tunnel_info.route_ipv6_dip, cint_srv6_tunnel_info.vrf_in[RCH], _l3_itf, cint_srv6_tunnel_info.eth_rif_intf_out[EGRESS], 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function add_host_ipv6_encap_dest(), \n",proc_name);
        return rv;
    }

    /*
     * L2 o SRv6 case, configure MAC-T address
     */
    bcm_l2_addr_t l2_addr_egr;

    bcm_l2_addr_t_init(&l2_addr_egr, cint_srv6_tunnel_info.l2_termination_mact_fwd_address, cint_srv6_tunnel_info.l2_termination_vsi);
    l2_addr_egr.flags = BCM_L2_STATIC;      /* static entry */
    l2_addr_egr.port = out_port_egress;
    rv = bcm_l2_addr_add(unit, &l2_addr_egr);

    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l2_addr_add for Egress node\n");
        return rv;
    }


    /*
     * 9. Configuring ISR SRv6 Tunnel
     */

   {
       /*
        * define the x4 SIDs  EEDB entries (linked to one another) and then link to the SRv6 Transport EEDB entry
        */

       /** define the SID structure which holds info on the SID address, EEDB entry order, next EEDB pointer */
       bcm_srv6_sid_initiator_info_t  sid_info;

       /** to store next SID GPORT */
       bcm_gport_t next_sid_tunnel_id;

       /*
        * Create SID0 EEDB entry and link to IPv6 Tunnel EEDB entry
        */

       /** don't use any special flags */
       sid_info.flags = 0;

       /** must set the tunnel id to 0 if not using WITH_ID flag */
       sid_info.tunnel_id = 0;

       /** set SID address to be last SID0 */
       sal_memcpy(sid_info.sid, cint_srv6_tunnel_info.sids[SID0], 16);

       /** set to EEDB entry of SID0 */
       sid_info.encap_access = bcmEncapAccessTunnel4;

       /** call the sid_initiator API to create the SID EEDB Entry and return its GPORT Tunnel-id */
       rv = bcm_srv6_sid_initiator_create(unit, &sid_info);

       if (rv != BCM_E_NONE)
       {
          printf("Error, bcm_dnx_srv6_sid_initiator_create for SID0 \n");
          return rv;
       }

       /*
        * Create SID1 EEDB entry and link to SID0 EEDB entry
        */

       /** don't use any special flags */
       sid_info.flags = 0;

       /** store the SID0 GPORT */
       next_sid_tunnel_id = sid_info.tunnel_id;

       /** must set the tunnel id to 0 if not using WITH_ID flag */
       sid_info.tunnel_id = 0;

       /** set SID address to be SID1 */
       sal_memcpy(sid_info.sid, cint_srv6_tunnel_info.sids[SID1], 16);

       /** set to EEDB entry of SID1 */
       sid_info.encap_access = bcmEncapAccessTunnel3;

       /** next_encap_id set to SID1 interface - sid_info.tunnel_id stores the next SID GPORT */
       BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(sid_info.next_encap_id, next_sid_tunnel_id);

       /** call the sid_initiator API to create the SID EEDB Entry and return its GPORT Tunnel-id */
       rv = bcm_srv6_sid_initiator_create(unit, &sid_info);

       if (rv != BCM_E_NONE)
       {
          printf("Error, bcm_dnx_srv6_sid_initiator_create for SID1 \n");
          return rv;
       }

       /*
        * Create SID2 EEDB entry and link to SID1 EEDB entry
        */

       /** store SID1 GPORT */
       next_sid_tunnel_id = sid_info.tunnel_id;

       sid_info.tunnel_id = 0;

       /** set SID address to be SID2 */
       sal_memcpy(sid_info.sid, cint_srv6_tunnel_info.sids[SID2], 16);

       /** set to EEDB entry of SID2 */
       sid_info.encap_access = bcmEncapAccessTunnel2;

       /** next_encap_id set to SID1 interface - sid_info.tunnel_id stores the next SID GPORT */
       BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(sid_info.next_encap_id, next_sid_tunnel_id);

       /** call the sid_initiator API to create the SID EEDB Entry and return its GPORT Tunnel-id */
       rv = bcm_srv6_sid_initiator_create(unit, &sid_info);

       if (rv != BCM_E_NONE)
       {
          printf("Error, bcm_dnx_srv6_sid_initiator_create for SID2 \n");
          return rv;
       }

       /*
        * Create SID3 EEDB entry and link to SID2 EEDB entry
        */

       /** store SID2 GPORT */
       next_sid_tunnel_id = sid_info.tunnel_id;

       sid_info.tunnel_id = 0;

       /** set SID address to be SID3 */
       sal_memcpy(sid_info.sid, cint_srv6_tunnel_info.sids[SID3], 16);

       /** set to EEDB entry of SID3 */
       sid_info.encap_access = bcmEncapAccessTunnel1;

       /** next_encap_id set to SID2 interface - sid_info.tunnel_id stores the next SID GPORT */
       BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(sid_info.next_encap_id, next_sid_tunnel_id);

       /** call the sid_initiator API to create the SID EEDB Entry and return its GPORT Tunnel-id */
       rv = bcm_srv6_sid_initiator_create(unit, &sid_info);

       if (rv != BCM_E_NONE)
       {
          printf("Error, bcm_dnx_srv6_sid_initiator_create for SID3 \n");
          return rv;
       }

       /*
        * define the SRH Base EEDB entry
        */

       /** define the SRH Base structure which holds nof_sids, QOS, and SRH Base GPORT */
       bcm_srv6_srh_base_initiator_info_t srh_base_info;
       bcm_srv6_srh_base_initiator_info_t_init(&srh_base_info);
       /** we will pass the global-out-LIF id*/
       srh_base_info.flags = BCM_SRV6_SRH_BASE_INITIATOR_WITH_ID;

       /** convert SRH global-LIF id to GPORT */
       BCM_GPORT_TUNNEL_ID_SET(srh_base_info.tunnel_id, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_SRH_GLOBAL_LIF]);

       /** we have 4 SIDs*/
       srh_base_info.nof_sids = 4;

       /** set TTL and QOS modes */
       srh_base_info.egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
       srh_base_info.egress_qos_model.egress_qos = bcmQosEgressModelUniform;
       srh_base_info.egress_qos_model.egress_ecn = bcmQosEgressEcnModelInvalid;
       srh_base_info.ttl = 0x0; /** needs to be 0 cause we don't use Pipe mode */
       BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(srh_base_info.next_encap_id, sid_info.tunnel_id);

       /** call the srh_base_initiator API to create the SRH EEDB Entry and local-out-LIF for it */
       rv = bcm_srv6_srh_base_initiator_create(unit, &srh_base_info);

       if (rv != BCM_E_NONE)
       {
          printf("Error, bcm_dnx_srv6_srh_base_initiator_create \n");
          return rv;
       }

       /*
        * create IPv6 Tunnel of SRv6 Type
        */
       bcm_tunnel_initiator_t tunnel_init_set;
       bcm_l3_intf_t intf;

       bcm_tunnel_initiator_t_init(&tunnel_init_set);
       bcm_l3_intf_t_init(&intf);
       tunnel_init_set.type = bcmTunnelTypeSR6;
       /** DIP is not required for this IPv6 Tunnel, which serves the SRv6 layer*/
       sal_memcpy(tunnel_init_set.sip6, cint_srv6_tunnel_info.tunnel_ip6_sip, 16);

       tunnel_init_set.ttl = 128;
       tunnel_init_set.egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
       tunnel_init_set.flags = BCM_TUNNEL_WITH_ID;
       BCM_GPORT_TUNNEL_ID_SET(tunnel_init_set.tunnel_id, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_IP_TUNNEL_GLOBAL_LIF]);

       rv = bcm_tunnel_initiator_create(unit, &intf, &tunnel_init_set);

       if (rv != BCM_E_NONE)
       {
          printf("Error, bcm_tunnel_initiator_create for IPv6 Tunnel \n");
          return rv;
       }
   }

    /*
     * 10. Configure the SRv6 ESR USP 1st Pass P2P IN_LIF into RCH Port and Ethernet Encapsulation
     */
    {

        /** Set Out-Port default properties for the recycled packets*/
        rv = out_port_set(unit, rch_port);
        if (rv != BCM_E_NONE)
        {
            printf("Error, out_port_set recycle_port \n");
            return rv;
        }

        /** Configure RCH port of Extended Termination type */
        rv = bcm_port_control_set(unit, rch_port, bcmPortControlRecycleApp, bcmPortControlRecycleAppExtendedTerm);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, bcm_port_control_set for rch_port %d\n",proc_name, rch_port);
            return rv;
        }

        /** define the structure which holds the mapping between nof_sids to RCH port and encap_id */
        bcm_srv6_extension_terminator_mapping_t extension_terminator_info;

        int sid_idx; /** sid_idx is used to fill LIF table for all possible number of SIDs in an SRv6 packet */

        extension_terminator_info.flags = 0;
        /** Convert Port number to GPORT */
        BCM_GPORT_LOCAL_SET(extension_terminator_info.port, rch_port);


        int max_sid_idx;
        max_sid_idx = *dnxc_data_get(unit, "srv6", "termination", "max_nof_terminated_sids_usp", NULL);
        int max_nof_terminated_usp_sids_1pass = *(dnxc_data_get (unit, "srv6", "termination", "max_nof_terminated_sids_usd_1pass", NULL));
        int nof_2pass_recycle_entries = max_sid_idx - max_nof_terminated_usp_sids_1pass;
        /** create LIF entries for all possible nof_sids in an SRv6 packet that would lead to RCY port + RCH Encapsulation*/
        for (sid_idx = 0; sid_idx < nof_2pass_recycle_entries; sid_idx++)
        {
            /*
             * Create RCH encapsulation per each NOF SIDs, to take into account relevant:
             * -recycle_header_extension_length (bytes to remove on 2nd pass at RCH termination)
             * -additional_egress_termination_size (additional bytes to remove on 1st pass egress - additional to parsing start offset value)
             */
            int recycle_entry_encap_id;
            /** Create entry, which will be built the recycle header for the copy going on the 2nd pass*/
            rv = srv6_create_extended_termination_recycle_entry(unit, sid_idx, &recycle_entry_encap_id);
            if (rv != BCM_E_NONE)
            {
                printf("Error, srv6_create_extended_termination_recycle_entry \n");
                return rv;
            }

            /** Convert from L3_ITF to GPORT */
            BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(extension_terminator_info.encap_id , recycle_entry_encap_id);

            extension_terminator_info.nof_sids = sid_idx + max_nof_terminated_usp_sids_1pass + 1;

            /** call the nof_sids API to map to RCH port and encapsulation */
            rv = bcm_srv6_extension_terminator_add(unit, &extension_terminator_info);

            if (rv != BCM_E_NONE)
            {
                 printf("Error, bcm_srv6_extension_terminator_add for nof_sids:%d\n", sid_idx);
                 return rv;
            }

         } /** of for sid_idx*/
    }

    /*
     * 11. End-Point configurations
     */
    {
        /** End-Points: Add MyDIP according to the SIDs, because we need to terminate the IPV6 header with the current SID */
        srv6_tunnel_termination(unit, DIP_ENDPOINT_GSID1, 0, cint_srv6_tunnel_info.vrf_in[ENDPOINT_GSID1], 0, 1, 0, 0, is_gsid_prefix_64b);
        srv6_tunnel_termination(unit, DIP_ENDPOINT_GSID0, 0, cint_srv6_tunnel_info.vrf_in[ENDPOINT_GSID0], 0, 1, is_gsid_eoc, 0, is_gsid_prefix_64b);

        /*
         * Egress: Add MyDIP according to the SIDs, because we need to terminate the IPV6 header with the current SID
         * -If ETHoSRv6 case, set the tunnel termination LIF FODO to be VSI, instead of VRF
         */
        if (next_header_is_eth)
        {
            /** to allow termination of GSID because of SL == 0 && SI==0 */
            srv6_tunnel_termination(unit, DIP_EGRESS, 1, cint_srv6_tunnel_info.vrf_in[EGRESS], cint_srv6_tunnel_info.l2_termination_vsi, 1, 0, 0, is_gsid_prefix_64b);
            /** to allow termination of GSID because of SL == 0 && EOC */
            srv6_tunnel_termination(unit, DIP_EGRESS_EOC, 1, cint_srv6_tunnel_info.vrf_in[EGRESS], cint_srv6_tunnel_info.l2_termination_vsi, 1, is_gsid_eoc, 0, is_gsid_prefix_64b);
        }
        else
        {
            /** to allow termination of GSID because of SL == 0 && SI==0 */
            srv6_tunnel_termination(unit, DIP_EGRESS, 1, cint_srv6_tunnel_info.vrf_in[EGRESS], 0, 1, 0, 0, is_gsid_prefix_64b);
            /** to allow termination of GSID because of SL == 0 && EOC */
            srv6_tunnel_termination(unit, DIP_EGRESS_EOC, 1, cint_srv6_tunnel_info.vrf_in[EGRESS], 0, 1, is_gsid_eoc, 0, is_gsid_prefix_64b);
        }
        srv6_tunnel_termination(unit, DIP_ENDPOINT_GSID1, 0, cint_srv6_tunnel_info.vrf_in[ENDPOINT_GSID1], 0, 1, is_gsid_eoc, default_tunnel_for_gsid_cascading, is_gsid_prefix_64b);
    }

    printf("%s(): Exit\r\n",proc_name);
    return rv;
}

