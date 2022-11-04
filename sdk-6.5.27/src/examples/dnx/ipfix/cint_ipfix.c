/* $Id: cint_ipfix.c
 * 
 * This file contains cints that call all the required APIs to configure an IPFIX entity.
 * Three main cints need to be called for IPFIX configuration.
 * IPFIX configuration sequence:
 * 1. iPMF1: 
 *      Group 1 for received packet:
 *        Qualifier - 5-tupple
 *        Action - Set packet type to Statistical Sampling
 *                 Set Container for iPMF3
 *      Group 2 for exported packet:
 *       Qualifier - UDP dest port == IPFIX
 *                   Source port == eventor
 *       Action - Set counter for IPFIX sequence-number
 *                Set IPFIX sub-type
 * 2. iPMF3:
 *      Qualifier - Container from iPMF1
 *      Action - Copy original packet's source and destination ports to UDH
 * 3. ePMF:
 *      Qualifier - Snooped Copy
 *      Action - Set ASE_Context (Enable correct contexts for IPFIX)
 * 4. Configure IPFIX parameters:
 *              - IPFIX Setup time
 *              - IPFIX Tx Time
 *              - IPFIX Template ID
 *              - IPFIX Eventor Header
 *              - IPFIX Observation Domain
 * 5. Enable IPFIX on eventor port.
 * 6. Setup ESEM lookups (bcm_instru_ipfix_interface_add)
 * 7. Setup IPFIX encap (bcm_instru_ipfix_encap_create) 
 * 8. Configure eventor
 *
 * Main functions:
 * cint_ipfix_general_init - General initializations for IPFIX.
 *                           Gets eventor-rx-port, export-packet-port and eventor-id, number-of-flow-records to aggregate.
 *                           Configures field and mirroring, sets IPFIX virtual registers, and configures the eventor.
 * cint_ipfix_main - Calls subfunctions to configure ingress, egress and the eventor.
 *                   Gets in-port, out-port, eventor-rx-port, export-packet-port and eventor-id.
 *                   Configures input and export packet routes, ingress and egress pipeline, field for export packet.
 * cint_ipfix_eventor_activate - Activates the eventor
 *
*/

int vrf = 20;
int intf_in = 250;           /* Incoming packet ETH-RIF */
int intf_out = 200;          /* Outgoing packet ETH-RIF */

struct cint_ipfix_basic_info_s {
    /* CRPS Properties */
    int ipfix_sequence_counter_id; /* Counter base for OAM counter */
    /* Mirror properties. */
    uint32 sample_rate_dividend; /* Sample rate = rate dividend / rate_divisor. Dividend must always be 1. */
    uint32 sample_rate_divisor;
    bcm_gport_t ipfix_mirror_destination; /* a mirror destination with a sample rate of dividend / divisor */

    /* UDP tunnel properties */
    uint16 udp_src_port;                  /* UDP source port */
    uint16 udp_dst_port;                  /* UDP destination port */
    uint32 kaps_result;                   /* kaps result for the basic route. */
    uint32 uptime;                        /* Random uptime for the cint example. */
    uint32 export_time;                   /* Random export time for the cint example. */
    uint32 observation_domain;            /* Random observation domain for the cint example. */
    int ipfix_encap_id;                   /* IPFIX ENCAP ID - is needed in the mirroring for setting encapsulation size, see cint_ipfix_mirror_create*/

    /* Eventor properties */
    uint32 buffer_size;
    uint32 time_thresh; /* in mSec */
};

cint_ipfix_basic_info_s cint_ipfix_basic_info = {
   /* ipfix_sequence_counter_id */
            111,
    /* Sample rate dividend | divisor */
               1,              1,
   /* ipfix_mirror_destination */
      -1,
    /* UDP SRC | DST ports */
       0, 4739,
    /* kaps result */
       0xA711,
    /* uptime */
       0x12345678,
    /* export_time */
       0x00230614,
    /* observation_domain */
       0xa1f1a1f1,
    /* ipfix_encap_id */
       -1,
    /* buffer_size: in words (4 bytes)
	 * metadata size = 44 bytes
	 * buffer size = 11
     */
       11,
    /* time_thresh */
       2000
};

bcm_field_context_t context_id = 0;   /* use default context 0*/

/*
 * This function creates the egress pipe configuration for the IPFIX:
 * - Configure IPFIX encap
 * - Configure IPFIX interface with in-port data
 * - Configure IPFIX interface with out-port data
 * - Enable IPFIX on eventor port
 * - Configure egress field
 * in_port [in]- Port that packets are received and sampled
 * out_port [in]- Port that packets are forwarded to.
 * ipfix_destination [out]- Allocated tunnel for IPFIX 
 */
int
cint_ipfix_egress_create(int unit, int in_port, int out_port, int eventor_port, bcm_gport_t *ipfix_destination)
{
    int output_interface;
    char *proc_name;
    bcm_gport_t gport;
    proc_name = "cint_ipfix_egress_create";

    printf("%s: in_port = %d, out_port= %d, \n",
           proc_name, in_port, out_port);

    /*
     * Create the IPFIX ETPS entry.
     */
    bcm_instru_ipfix_encap_info_t ipfix_encap_info;
    sal_memset(&ipfix_encap_info, 0, sizeof(ipfix_encap_info));

    {
        BCM_IF_ERROR_RETURN_MSG(bcm_instru_ipfix_encap_create(unit, &ipfix_encap_info), "");
    }

    bcm_instru_ipfix_interface_info_t ipfix_iterface;
    if (BCM_GPORT_IS_SET(in_port)) {
       gport = in_port;
    }
    else {
       BCM_GPORT_SYSTEM_PORT_ID_SET(gport, in_port);
    }

    ipfix_iterface.flags = BCM_INSTRU_IPFIX_INTERFACE_INPUT;
    ipfix_iterface.port = gport;
    ipfix_iterface.interface = intf_in;
    ipfix_iterface.vrf_id = vrf;
    BCM_IF_ERROR_RETURN_MSG(bcm_instru_ipfix_interface_add(unit, &ipfix_iterface), "");

    ipfix_iterface.flags = BCM_INSTRU_IPFIX_INTERFACE_OUTPUT;
    output_interface = out_port&0xff;
    ipfix_iterface.port = output_interface;
    ipfix_iterface.interface = intf_out;
    ipfix_iterface.vrf_id = vrf;
    BCM_IF_ERROR_RETURN_MSG(bcm_instru_ipfix_interface_add(unit, &ipfix_iterface), "");

    /* Enable IPFIX on eventor port */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_control_set(unit, eventor_port,bcmPortControlIpFixRxEnable,1), "");

    *ipfix_destination = ipfix_encap_info.ipfix_encap_id;
    printf("%s: IPFIX outlif = 0x%x\n", proc_name, ipfix_encap_info.ipfix_encap_id);

    {
        BCM_IF_ERROR_RETURN_MSG(cint_ipfix_field_egress(unit, &ipfix_encap_info), "");
    }

    return BCM_E_NONE;
}

/*
 * This function configures the ingress side of the ipfix, including the PMF selector and mirror
 * destination.
 *
 * in_port [in]- Port that packets are received and sampled
 * out_port [in]- Port that packets are forwarded to.
 * ipfix_out_port [in]- Port that IPFIX export packets are forwarded to (Collector port).
 * ipfix_destination [in]- Tunnel for IPFIX metadata packets
 * 
 */
int
cint_ipfix_ingress_create(int unit, int in_port, int ipfix_out_port, bcm_gport_t ipfix_destination)
{
    char *proc_name;

    proc_name = "cint_ipfix_ingress_create";
    /*
     * Create a mirror destination of type stat sampling, including ingress crop, and stat rate,
     * and modified ftmh header.
     */
    BCM_IF_ERROR_RETURN_MSG(cint_ipfix_mirror_create(unit, ipfix_out_port, ipfix_destination,
            cint_ipfix_basic_info.sample_rate_dividend, cint_ipfix_basic_info.sample_rate_divisor,
            &cint_ipfix_basic_info.ipfix_mirror_destination, 0), "");

    printf("%s: cint_ipfix_mirror_create - returned mirror dest %d 0x%x\n", proc_name, cint_ipfix_basic_info.ipfix_mirror_destination, cint_ipfix_basic_info.ipfix_mirror_destination);

    /*
     * Create a field entry for IPoETH
     */

    bcm_field_entry_t field_entry_handle;

    BCM_IF_ERROR_RETURN_MSG(cint_ipfix_field_entry_add(unit, bcmFieldLayerTypeEth, bcmFieldLayerTypeIp4, TRUE, cint_ipfix_basic_info.ipfix_mirror_destination,
                field_entry_handle), "");

    return BCM_E_NONE;
}

/*
 * IPFIX registers initialization 
 */
int
cint_ipfix_registers_init(int unit, int eventor_id)
{



    /*
     * Set the global IPFIX System UP time.
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_instru_control_set(unit, 0, bcmInstruControlIpFixSystemUpTime, cint_ipfix_basic_info.uptime), "");

    /*
     * Set the global IPFIX eventor rx context id.
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_instru_control_set(unit, 0, bcmInstruControlIpFixEventorId, eventor_id), "");

    /*
     * For the purpose of the example cint, we'll use a random Template ID.
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_instru_control_set(unit, 0, bcmInstruControlIpFixTemplateId, 0x14), "");

    {
        /*
         * For the purpose of the example cint, we'll use a random Time.
         */
        BCM_IF_ERROR_RETURN_MSG(bcm_instru_control_set(unit, 0, bcmInstruControlIpFixTxTime, 0x230614), "");
    }

    /*
     * For the purpose of the example cint, we'll use a random Time.
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_instru_control_set(unit, 0, bcmInstruControlIpFixObservationDomain, cint_ipfix_basic_info.observation_domain), "");

    return BCM_E_NONE;
}

/*
 * This function configures the eventor for ipfix.
 * Note:
 * For simplicity - this function sets Eventor RX double buffer to be the pair 
 * eventor_id and eventor_id+1 (modulo 8).
 * Thus, when using multi channel configuration, each channel actually occupies two eventor IDs thus each channel's 
 * eventor_id is +2 from previous channel's eventor_id.
 *
 * eventor_id [in] - exentor rx context
 * nof_samples [in] - number of metadata packets to aggregate into an export packet
 * eventor_port [in] - port that metadata packets are forwarded to.
 * ipfix_out_port [in] - port that IPFIX export packets are forwarded to.
 */
int
cint_ipfix_eventor_set(int unit, int eventor_id, int nof_samples, int eventor_port, int ipfix_out_port)
{
    uint32 flags = 0;
	/* Size in bytes of a single metadata packet */
    int buffer_size = cint_ipfix_basic_info.buffer_size;
    int buffer_size_thr = buffer_size * nof_samples;
    int context;
    int builder;
    uint32 header_length;
    uint32 udp_header_offset;
    uint32 ip_length_offset;
	uint32 ipfix_header_length_offset;
    bcm_instru_eventor_context_conf_t context_conf = {0};
    bcm_instru_eventor_builder_conf_t builder_conf = {0};
    uint8 * header_data;
    bcm_port_interface_info_t int_inf;
    bcm_port_mapping_info_t mapping_info;
    char *proc_name;


    proc_name = "cint_ipfix_eventor_set";

    /* TX header data (IPFIX_DATAGRAMoUDPoIPv4oETH0oPTCH2)*/
    uint8 header_data_udp_tunnel[128] = {
            /* PTCH2 - 2 bytes*/
            0x00, 0x00,
            /* ETH0 -  6+6 bytes*/
            0x00, 0x0e, 0x00, 0x02, 0x00, 0x01, /* DMAC should be myMac so IP forwarding will be applied */
            0x00, 0x0e, 0x00, 0x02, 0x00, 0x00, /* SMAC */
            /* Ether type - 2 bytes*/
            0x08, 0x00,
            /* IPV4 header - 4 x 5 bytes */
            0x45, 0x00, 0x06, 0x38,
            0x00, 0x00, 0x00, 0x00,
            0x40, 0x11, 0x38, 0x6B,
            0xA0, 0x00, 0x00, 0x11, /* SIP = 160.0.0.17 */
            0xA1, 0x00, 0x00, 0x11, /* DIP = 161.0.0.17 */
            /* UDP header - 2 x 4 bytes */
            0x17, 0x70, 0x13, 0x88, 
            0x00, 0x59, 0x00, 0x00,
            /* IPFIX DATAGRAM header - 4 x 4 bytes*/
            0x00, 0x0A, 0x01, 0x44, /* IPFIX_VERSION | LENGTH */
            0x00, 0x00, 0x00, 0x00, /* Export Time */
            0x00, 0x00, 0x00, 0x00, /* Sequence Number */
            0x00, 0x00, 0x00, 0x00, /* Observation Domain */
            /* Dont' care*/
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00
    };

    /* Get Eventor PP port */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_get(unit, eventor_port, &flags, &int_inf, &mapping_info), "");
    flags = 0;

    printf("%s: eventor_id = %d, eventor_port = %d, eventor_pp_port = %d, buffer_size = %d, nof_samples = %d, buffer_size_thr = %d, \n", proc_name, eventor_id, eventor_port, mapping_info.pp_port, buffer_size, nof_samples, buffer_size_thr);

    /* in PTCH2, In_PP_Port is 10 bits [9:0]*/
    uint8 eventor_pp_port_high = ((mapping_info.pp_port) >> 8) & 0x3;
    uint8 eventor_pp_port_low = (mapping_info.pp_port) & 0xFF;

    /* Set TX header size and data (IPFIX_DATAGRAMoUDPoIPv4oETH0oPTCH2)*/
    header_data = header_data_udp_tunnel;
    /*
     * PTCH2 = 2 bytes
     * ETH0 = 6+6 bytes
     * EtherType = 2 bytes
     * IPv4 header = 5x4 bytes
     * UDP header = 2x4 bytes 
     * IPFIX header = 1x4 (the header is 4 words, but only 1 word is static) 
     */
     header_length = 2 + (6+6) + 2 + (5*4) + (2*4) + (1*4);
     udp_header_offset = 2 + (6+6) + 2 + (5*4);
     ip_length_offset = 2 + (6+6) + 2 + 2;
     ipfix_header_length_offset = udp_header_offset + 8;

     /* PTCH2 : In_PP_Port [9:0], Parser_Program_Control [15] - '1' - Next header should be deduced from the SSP*/
     header_data[0] = eventor_pp_port_high | 0x80;
     header_data[1] = eventor_pp_port_low;

     /* UDP port for IPFIX = 4739 = 0x1283 */
     header_data[udp_header_offset] = cint_ipfix_basic_info.udp_src_port >> 8;
     header_data[udp_header_offset+1] = cint_ipfix_basic_info.udp_src_port & 0xFF;
     header_data[udp_header_offset+2] = cint_ipfix_basic_info.udp_dst_port >> 8;
     header_data[udp_header_offset+3] = cint_ipfix_basic_info.udp_dst_port & 0xFF;
     /* 
      UDP length =
      UDP header (8 bytes) + payload data
      payload data = IPFIX Datagram header (4 words) + aggregated data (buffer_size_thr is in 4 byte words)
       */
     uint32 udp_header_length = 8 + (16 + buffer_size_thr * 4);
     header_data[udp_header_offset+4] = udp_header_length >> 8;
     header_data[udp_header_offset+5] = udp_header_length & 0xFF;

     header_data[ipfix_header_length_offset+12] = (cint_ipfix_basic_info.observation_domain >>24) & 0xFF;
     header_data[ipfix_header_length_offset+13] = (cint_ipfix_basic_info.observation_domain >> 16) & 0xFF;
     header_data[ipfix_header_length_offset+14] = (cint_ipfix_basic_info.observation_domain >> 8) & 0xFF;
     header_data[ipfix_header_length_offset+15] = cint_ipfix_basic_info.observation_domain & 0xFF;
     /* 
      IP Header - total length =
      UDP header + 20 bytes
       */
     uint32 ip_header_total_length = udp_header_length;
     ip_header_total_length += 20;
     header_data[ip_length_offset] = ip_header_total_length >> 8;
     header_data[ip_length_offset+1] = ip_header_total_length & 0xFF;


    printf("%s: IP LENGTH = 0x%04X, UDP LENGTH = 0x%04X, udp_src_port = 0x%04X, udp_dst_port = 0x%04X \n", proc_name, ip_header_total_length, udp_header_length, cint_ipfix_basic_info.udp_src_port, cint_ipfix_basic_info.udp_dst_port);

    context = builder = eventor_id;
    /* init eventor RX context */

    /*
     * 1. Can simply use different pair of banks per context (but since there are 8 banks, there can be only 4 contexs ...)
     * 2. Can use same pair of banks but with different offset (bank size is 13K bytes)
     * Here, for simplicity, use #1 
     *  
     * Note: performing modulo 8 operation by & 0x7
     */
    context_conf.bank1 = context & 0x7;
    context_conf.bank2 = (context+1) & 0x7;
    printf("%s: bank1 = %d, bank2 = %d\n", proc_name, context_conf.bank1, context_conf.bank2);

    context_conf.buffer_size = buffer_size;
    context_conf.buffer1_start = 0;
    context_conf.buffer2_start = 256;

    /* init builder */
    builder_conf.flags = 0;
    builder_conf.thresh_size = buffer_size_thr;
    builder_conf.thresh_time = cint_ipfix_basic_info.time_thresh; 
    builder_conf.header_data = header_data;
    builder_conf.header_length = header_length;

    /*
     * Set eventor Rx context (Rx buffers)
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_instru_eventor_context_set(unit, flags, context, bcmEventorContextTypeRx, &context_conf), "");
    
    flags = 0;

    /*
     * Set eventor builder (Tx buffers)
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_instru_eventor_builder_set(unit, flags, builder, &builder_conf), "");

    return BCM_E_NONE;
}

/*
 * General intializations for IPFIX. It will init the CRPS, ipfix registers and the field processor qualifiers.
 */
int
cint_ipfix_general_init(int unit, int ipfix_out_port, int eventor_port, int eventor_id, int nof_samples)
{
    char *proc_name;

    proc_name = "cint_ipfix_general_init";

    /*
     * Configure the CRPS for IPFIX.
     */
    /* See set_counter_resource:
     * For couner_if 0,1,2:
     * Ingress command_id are 7,8,9
	 * In this example counter_if 0 and command_id 7 are being used
	 */
    BCM_IF_ERROR_RETURN_MSG(set_counter_resource(unit, eventor_port, 0, 0, &cint_ipfix_basic_info.ipfix_sequence_counter_id), "");

    printf("%s: counter (counter_if = %d, counter_base = %d) created.\n", proc_name, 0, cint_ipfix_basic_info.ipfix_sequence_counter_id);

    /*
     * Intialize the ipfix field configuration, then map the flow's tuple to the mirror destination.
     */
    BCM_IF_ERROR_RETURN_MSG(cint_ipfix_field_main(unit, context_id), "");

    /*
     * Set the global registers for IPFIX.
     */
    BCM_IF_ERROR_RETURN_MSG(cint_ipfix_registers_init(unit, eventor_id), "");

    BCM_IF_ERROR_RETURN_MSG(cint_ipfix_eventor_set(unit, eventor_id, nof_samples, eventor_port, ipfix_out_port), "");

    return BCM_E_NONE;
}

/*
* This function create route setup for the IPFIX export packet. 
*/
int
cint_ipfix_export_route(int unit, int in_port, int ipfix_out_port, int kaps_result)
{
    char *proc_name;

    int fec = kaps_result;
    int vlan = 200;
    bcm_tunnel_type_t tunnel_type = bcmTunnelTypeUdp;

    bcm_mac_t intf_in_mac_address =  { 0x00, 0x0e, 0x00, 0x02, 0x00, 0x01 };   /* my-MAC */
    bcm_mac_t intf_out_mac_address = { 0x00, 0x0e, 0x00, 0x02, 0x00, 0x02 };   
    bcm_mac_t arp_next_hop_mac =     { 0x00, 0x0e, 0x00, 0x02, 0x00, 0x03 };   /* next_hop_mac */

    
    bcm_vlan_port_t vlan_port;
    bcm_port_match_info_t match_info;

    bcm_l3_intf_t l3if;
    bcm_l3_ingress_t l3_ing_if;
    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid_null;      /* not intersting */
    int arp_encap_id = 8195;   /* ARP-Link-layer (needs to be higher than 8192 for Jer Plus) */
    bcm_gport_t gport;
    bcm_if_t encoded_fec;

    bcm_l3_host_t host_info;
    int l3_itf;
    bcm_l3_route_t l3rt;

    uint32 ipv4_host  = 0xA1000011; /* 161.0.0.17*/
    uint32 ipv4_route = 0xA1000000; /* 161.0.0.xx*/
    uint32 ipv4_mask  = 0xffffff00;

    proc_name = "cint_ipfix_export_route";

    /*
     * 1. Set in-port to ETH-RIF 
     */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.port = in_port;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vlan_port.vsi = intf_in; /* ETH-RIF */
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;

    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, vlan_port), "ETH-RIF");

    printf("1. %s: port = %d, vsi = %d, vlan_port_id = 0x%08X\n", proc_name, vlan_port.port, vlan_port.vsi, vlan_port.vlan_port_id);

    /*
     * 2. Set out-port to default AC LIF
     */
    bcm_vlan_port_t_init(&vlan_port);

    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_DEFAULT | BCM_VLAN_PORT_VLAN_TRANSLATION;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), "OUT-RIF");

    bcm_port_match_info_t_init(&match_info);
    match_info.match = BCM_PORT_MATCH_PORT;
    match_info.flags = BCM_PORT_MATCH_EGRESS_ONLY;
    match_info.port = ipfix_out_port;

    BCM_IF_ERROR_RETURN_MSG(bcm_port_match_add(unit, vlan_port.vlan_port_id, &match_info), "");

    printf("2. %s: port = %d, vlan_port_id = 0x%08X\n", proc_name, ipfix_out_port, vlan_port.vlan_port_id);

    /*
     * 3. Create ETH-RIF/OUT-RIF and set its properties
     */
    bcm_l3_intf_t_init(&l3if);
    l3if.l3a_flags = BCM_L3_WITH_ID;
    l3if.l3a_mac_addr = intf_in_mac_address;
    l3if.l3a_intf_id = intf_in;
    l3if.l3a_vid = intf_in;
    /* set qos map id to 0 as default */
    l3if.dscp_qos.qos_map_id = 0;

    BCM_IF_ERROR_RETURN_MSG(bcm_l3_intf_create(unit, &l3if), "ETH-RIF");

    bcm_l3_intf_t_init(&l3if);
    l3if.l3a_flags = BCM_L3_WITH_ID;
    l3if.l3a_mac_addr = intf_out_mac_address;
    l3if.l3a_intf_id = intf_out;
    l3if.l3a_vid = intf_out;
    /* set qos map id to 0 as default */
    l3if.dscp_qos.qos_map_id = 0;

    BCM_IF_ERROR_RETURN_MSG(bcm_l3_intf_create(unit, &l3if), "OUT-RIF");

    /*
     * 4. Set Incoming ETH-RIF properties
     */
    bcm_l3_ingress_t_init(&l3_ing_if);
    l3_ing_if.flags = BCM_L3_INGRESS_WITH_ID;      /* must, as we update exist RIF */
    l3_ing_if.vrf = vrf;
    l3_ing_if.qos_map_id = 0;

    BCM_IF_ERROR_RETURN_MSG(bcm_l3_ingress_create(unit, &l3_ing_if, intf_in), "");
 
    /*
     * 5. Create ARP and set its properties
     */
    bcm_l3_egress_t_init(&l3eg);

    l3eg.mac_addr = arp_next_hop_mac;
    l3eg.encap_id = arp_encap_id;
    l3eg.vlan = vlan;
    l3eg.flags = 0;

    BCM_IF_ERROR_RETURN_MSG(bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY, &l3eg, &l3egid_null), "");
 
    arp_encap_id = l3eg.encap_id;

    printf("5. %s: arp_encap_id = 0x%08X\n", proc_name, arp_encap_id);


    /*
     * 6. Create FEC and set its properties
     */
    if (BCM_GPORT_IS_SET(ipfix_out_port)) {
        gport = ipfix_out_port;
    }
    else {
        BCM_GPORT_LOCAL_SET(gport, ipfix_out_port);
    }
 
    bcm_l3_egress_t_init(&l3eg);
    l3eg.intf = intf_out;
    l3eg.encap_id = arp_encap_id;
    l3eg.failover_id = 0;
    l3eg.port = gport;
    l3eg.flags = 0;
    encoded_fec = fec;
    if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        l3eg.flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }


    BCM_IF_ERROR_RETURN_MSG(bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY | BCM_L3_WITH_ID, &l3eg, &encoded_fec), "");

     printf("6: %s: fec = 0x%08X, encoded_fec = 0x%08X \n",proc_name, fec, encoded_fec);

    /*
     * 7. Add Route entry
     */
     bcm_l3_route_t_init(l3rt);
     l3rt.l3a_vrf = vrf;
     l3rt.l3a_intf = fec;
     if (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
     {
         l3rt.l3a_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
     }

     if (tunnel_type == bcmTunnelTypeUdp)
     {
         l3rt.l3a_subnet = ipv4_route;
         l3rt.l3a_ip_mask = ipv4_mask;
     }
     else
     {
         printf("%s: tunnel_type = %d is not supported! Only support bcmTunnelTypeUdp (%d) \n", 
              proc_name, tunnel_type, bcmTunnelTypeUdp);
         return BCM_E_PARAM;
     }

     BCM_IF_ERROR_RETURN_MSG(bcm_l3_route_add(unit, &l3rt), "");

    /*
     * 8. Add host entry
     */

    BCM_L3_ITF_SET(&l3_itf, BCM_L3_ITF_TYPE_FEC, fec);

    if (!*dnxc_data_get(unit, "l3", "fwd", "host_entry_support", NULL))
    {
        if (tunnel_type == bcmTunnelTypeUdp)
        {
            BCM_IF_ERROR_RETURN_MSG(add_route_ipv4(unit, ipv4_host, 0xffffffff, vrf, l3_itf), "");
        }
        else if (tunnel_type == bcmTunnelTypeUdp6)
        {
            BCM_IF_ERROR_RETURN_MSG(add_route_ipv6(unit, ipv6_host, 0xffffffff, vrf, l3_itf), "");
        }
        else
        {
            printf("%s: tunnel_type = %d is not supported! Only support bcmTunnelTypeUdp (%d) and bcmTunnelTypeUdp6 (%d) \n",
                 proc_name, tunnel_type, bcmTunnelTypeUdp, bcmTunnelTypeUdp6);
            return BCM_E_PARAM;
        }
    }
    else
    {
        bcm_l3_host_t_init(&host_info);
        host_info.l3a_vrf = vrf;
        host_info.l3a_intf = l3_itf;

        if (tunnel_type == bcmTunnelTypeUdp)
        {
            host_info.l3a_ip_addr = ipv4_host;
        }
        else if (tunnel_type == bcmTunnelTypeUdp6)
        {
            host_info.l3a_ip6_addr = ipv6_host;
            host_info.l3a_flags = BCM_L3_IP6;
        }
        else
        {
            printf("%s: tunnel_type = %d is not supported! Only support bcmTunnelTypeUdp (%d) and bcmTunnelTypeUdp6 (%d) \n",
                proc_name, tunnel_type, bcmTunnelTypeUdp, bcmTunnelTypeUdp6);
            return BCM_E_PARAM;
       }

       BCM_IF_ERROR_RETURN_MSG(bcm_l3_host_add(unit, &host_info), "");
    }

    return BCM_E_NONE;
}

/*
 * IPFIX Main Function
 * -------------------
 * 1. Configure basic route.
 * 2. Configure egress pipeline, including crps, ipfix_encap and field.
 * 3. Configure ingress pipeline, including field and mirroring.
 * 4. Configure ipfix export packet route (ipfix export traffic coming from eventor).
 * 5. Configure ipfix export packet field.
 */
int cint_ipfix_main(int unit, int in_port, int route_out_port, int ipfix_out_port, int eventor_port, int eventor_id)
{
    bcm_gport_t ipfix_destination;
    int fec_id;

    {
        BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit,0, &fec_id), "");
    }
    cint_ipfix_basic_info.kaps_result = fec_id;

    /*
     * Create some traffic flow.
     * In this example we use ip route basic, but it could be anything.
     */
    BCM_IF_ERROR_RETURN_MSG(dnx_basic_example(unit, in_port, route_out_port, fec_id), "");

    /*
     * Configure the IPFIX egress pipeline.
     */
    BCM_IF_ERROR_RETURN_MSG(cint_ipfix_egress_create(unit, in_port, route_out_port, eventor_port, &ipfix_destination), "");
    cint_ipfix_basic_info.ipfix_encap_id = ipfix_destination;

    /*
     * Configure the IPFIX ingress pipeline
     */
    BCM_IF_ERROR_RETURN_MSG(cint_ipfix_ingress_create(unit, in_port, eventor_port, ipfix_destination), "");

    /*
     * Set ip route for ipfix export packet. eventor_port ==> ipfix_out_port
     */
    BCM_IF_ERROR_RETURN_MSG(cint_ipfix_export_route(unit, eventor_port, ipfix_out_port, fec_id +1), "");

    /*
     * Create a field entry for preventing snooping of the IPFIX export packet coming from the Eventor port
     */
    bcm_field_entry_t field_entry_handle;

    BCM_IF_ERROR_RETURN_MSG(cint_ipfix_field_entry_add_eventor_port(unit, eventor_port, cint_ipfix_field_fg_id, &field_entry_handle), "");

    /*
     * Configure IPFIX datagram SN and timestamp update
     */
    BCM_IF_ERROR_RETURN_MSG(cint_ipfix_field_export_packet(unit, eventor_port, ipfix_out_port, context_id), "");

    return BCM_E_NONE;
}

/*
 * This functions activates the eventor for ipfix.
 */
int
cint_ipfix_eventor_activate(int unit)
{
    uint32 flags = 0;
    char *proc_name;

    proc_name = "cint_ipfix_eventor_activate";

    /*
     * Activate eventor
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_instru_eventor_active_set(unit, flags, 1), "");

    printf("%s: Eventor is activated.\n", proc_name);

    return BCM_E_NONE;
}
