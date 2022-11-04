/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2022 Broadcom Inc. All rights reserved. File: cint_mpls_3_level_protection.c Purpose: Various examples for MPLS. 
 */

/* 
 * File: cint_mpls_3_level_protection.c
 * Purpose: Example of MPLS 3 level fec protecion configuration
 * 
 * Configuration:
 *  
 * Test Scenario - start
 * cint;                                                                  
 * cint_reset();                                                          
 * exit; 
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_global.c  
  cint ../../../../src/examples/dnx/l3/cint_dnx_ip_route_basic.c   
  cint ../../../../src/examples/dnx/mpls/cint_dnx_mpls_3_level_protection.c                                       
  cint                                                                   
  int unit = 0; 
  int rv = 0; 
  int in_port = 200; 
  int out_port = 201;
  int ttl_value = 0;
  rv = mpls_3_level_protection_basic_main(unit,in_port,out_port,ttl_value);
  print rv; 
 * Test Scenario - end
 *
 * Incoming Packet with ip route packet, encapsulated by 3 mpls labels, depending on the super fec protection configuration
 *  
 *  ##############################################################################################
 *  Sending packet from port == 200 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+
 *   |    |      DA      | SA           |  SIP     |  DIP          | TTL  |
 *   |    |0C:00:02:00:00|11:00:00:01:12|160.0.0.17|160.161.161.163| 64   |
 *   |    |              |              |          |               |      |
 *   |    |              |              |          |               |      |
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *  Incoming IPV4 over Ethernet (untagged) packets coming from in_port
 *  Outgoing IPV4oMPLSoMPLSoMPLSoETH(tagged) packet
 *  The mpls labels are determined by the super fec protection configuration
 *  Each fec is configured with a protection path. The fec chosen is determined based on the configurable protection enable
 *  Labels {3333,5555,7777} belong to the primary fecs
 *  Labels {4444,6666,8888} belong to the backup fecs
 * 
 *  Outgoing:
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  | Ethernet                                                           | MPLS                   | MPLS                   | MPLS                   | IPV4                                  | Payload |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  | DA                | SA                  | TPID   | PCP | DEI | VID | Label | TC | BOS | TTL | Label | TC | BOS | TTL | Label | TC | BOS | TTL |  SIP        |  DIP             | TTL  |         |
 *  | 00:0c:00:02:00:00 | 00:0C:00:02:0F:55   | 0x8847 |     |     |     | 3333  |    |     |     | 5555  |    |     |     | 7777  |    |     |     | 160.0.0.17  | 160.161.161.163  |      |         |
 *  |                   |                     |        |     |     |     | 3333  |    |     |     | 6666  |    |     |     | 7777  |    |     |     |             |                  |      |         |
 *  |                   |                     |        |     |     |     | 3333  |    |     |     | 5555  |    |     |     | 8888  |    |     |     |             |                  |      |         |
 *  |                   |                     |        |     |     |     | 3333  |    |     |     | 6666  |    |     |     | 8888  |    |     |     |             |                  |      |         |
 *  |                   |                     |        |     |     |     | 4444  |    |     |     | 5555  |    |     |     | 7777  |    |     |     |             |                  |      |         |
 *  |                   |                     |        |     |     |     | 4444  |    |     |     | 6666  |    |     |     | 7777  |    |     |     |             |                  |      |         |
 *  |                   |                     |        |     |     |     | 4444  |    |     |     | 5555  |    |     |     | 8888  |    |     |     |             |                  |      |         |
 *  |                   |                     |        |     |     |     | 4444  |    |     |     | 6666  |    |     |     | 8888  |    |     |     |             |                  |      |         |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 
 *  Forwarding is done based on the IPV4 header
 *  ILM will provide us with the first fec pointer
 *  First and second level fecs will point to the EEDB (for mpls tunnel) and to the next cascaded fec
 *  Last fec will result in a out port + EEDB entry (for mpls tunnel) and the entry will point to another EEDB entry for ARP
 *                                                  
 *                                                    +-------------------------------------------------------------------+
 *                                                    |                                                                   |
 *                                                    |                         +------------------------------+          |
 *      ILM                       FEC level 1         |      FEC level 2        |      FEC level 3             |  EEDB    |
 *  +-------------+            +--------------+       |   +--------------+      |   +---------------+        +-v----------v-+
 *  |             |            |    Primary   +-------+   |  Primary     +------+   |   Primary     |        |              |
 *  |             |            |    FEC       |           |  FEC         |          |   FEC         |        |              |
 *  |             |            |              |           |              |          |               +-------->              |
 *  |             |            |              |           |              |          |               |        |              |
 *  |             +------------> +---------+  +----------->  +--------+  +----------> +----------+  |        |              |
 *  |             |            |              |           |              |          |               |        |              |
 *  |             |            |    Backup    |           |   Backup     |          |    Backup     |        |              |
 *  |             |            |    FEC       |           |   FEC        |          |    FEC        +-------->              |
 *  |             |            |              |           |              |          |               |        |              |
 *  |             |            |              |           |              |          |               |        |              |
 *  +-------------+            +-------+------+           +------+-------+          +---------------+        +-------^----+-+
 *                                     |                         |                                                   |    ^
 *                                     |                         +---------------------------------------------------+    |
 *                                     |                                                                                  |
 *                                     +----------------------------------------------------------------------------------+
 *
 */

int NUM_FEC_LEVELS = 3;

struct cint_mpls_3_level_protection_basic_info_s
{
    int in_port;                                            /* incoming port */
    int out_port;                                           /* outgoing port */
    int intf_in;                                            /* in RIF */
    int intf_out;                                           /* out RIF */
    bcm_mac_t intf_in_mac_address;                          /* mac for tin RIF */
    bcm_mac_t intf_out_mac_address;                         /* mac for out RIF */
    bcm_mac_t arp_next_hop_mac;                             /* mac for next hop */
    int vrf;                                                /* VRF */
    int arp_id;                                             /* Id for ARP entry */
    bcm_ip_t dip;                                           /* dip 1 */
    int fec_ids_primary[NUM_FEC_LEVELS];                    /* Primary fec ids */
    int fec_ids_backup[NUM_FEC_LEVELS];                     /* Backup fec ids */
    int tunnel_ids_primary[NUM_FEC_LEVELS];                 /* tunnel id for 3_level_protection entry - primary*/
    int tunnel_ids_backup[NUM_FEC_LEVELS];                  /* tunnel id for 3_level_protection entry - backup*/
    bcm_mpls_label_t labels_array_primary[NUM_FEC_LEVELS];  /* pushed labels */
    bcm_mpls_label_t labels_array_backup[NUM_FEC_LEVELS];   /* pushed labels */
    int failover_ids[NUM_FEC_LEVELS];                       /* 3 failover ids, one per super fec */
    int ingress_qos_profile;
};

cint_mpls_3_level_protection_basic_info_s cint_mpls_3_level_protection_basic_info =
{ 
    /*
     * ports : in_port | out_port 
     */
    200, 201,
    /*
     * intf_in | intf_out 
     */
    20, 42,
    /*
     * intf_in_mac_address | intf_out_mac_address | arp_next_hop_mac | 
     */
    {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}, {0x00, 0x0c, 0x00, 0x02, 0x0f, 0x55}, {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d},
    /*
     * vrf 
     */
    1,
    /*
     * arp_id
     */
    4097,
    /*
     * dip
     */
    0xA0A1A1A3 /* 160.161.161.163 */ ,
    /*
     * fec_ids_primary. 
     * Fec values must be even 
     * First level should be between 32k and 64k 
     * Second level should be between 64k and 96k 
     * Third level should be above 96k 
     */
    {0xA000, 0x10000, 0x1E000},
    /*
     * fec_ids_backup. 
     * Fec values must be odd 
     * Fec values should be equal to the primary fec + 1 
     */
    {0xA001, 0x10001, 0x1E001},
    /*
     * tunnel_ids_primary,
     */
    {0,1,2},
    /*
     * tunnel_ids_backup,
     */
    {3,4,5},
    /*
     * labels_array_primary
     */
    {3333, 5555, 7777},
    /*
     * labels_array_backup
     */
    {4444, 6666, 8888},
    /*
     * failover_ids 
     * separated by 16K delta, due to banks limit 
     */
    {100, 16*1024+100, 32*1024+100},
    /*
     * ingress_qos_profile
     */
    3
};

int
mpls_3_level_protection_basic_main(
    int unit,
    int port1,
    int port2,
    uint8 ttl_value)
{

    cint_mpls_3_level_protection_basic_info.in_port = port1;
    cint_mpls_3_level_protection_basic_info.out_port = port2;

    BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &cint_mpls_3_level_protection_basic_info.fec_ids_primary[0]), "");
    cint_mpls_3_level_protection_basic_info.fec_ids_backup[0] = cint_mpls_3_level_protection_basic_info.fec_ids_primary[0] + 1;
    BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 1, 0, &cint_mpls_3_level_protection_basic_info.fec_ids_primary[1]), "");
    cint_mpls_3_level_protection_basic_info.fec_ids_backup[1] = cint_mpls_3_level_protection_basic_info.fec_ids_primary[1] + 1;
    BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 2, 0, &cint_mpls_3_level_protection_basic_info.fec_ids_primary[2]), "");
    cint_mpls_3_level_protection_basic_info.fec_ids_backup[2] = cint_mpls_3_level_protection_basic_info.fec_ids_primary[2] + 1;

    /*
     * Create AC lif 
     */
    BCM_IF_ERROR_RETURN_MSG(mpls_3_level_protection_basic_configure_port_properties(unit), "");

    /*
     * Create RIF
     */
    BCM_IF_ERROR_RETURN_MSG(mpls_3_level_protection_basic_create_l3_interfaces(unit), "");

    /*
     * Configure an ARP entry in EEDB
     */
    BCM_IF_ERROR_RETURN_MSG(mpls_3_level_protection_basic_create_arp_entry(unit), "");

    /*
     * Configure mpls entries in EEDB, last entry points to ARP
     */
    BCM_IF_ERROR_RETURN_MSG(mpls_3_level_protection_basic_create_all_mpls_tunnels(unit, ttl_value), "");

    /*
     * Configure fecs
     */
    BCM_IF_ERROR_RETURN_MSG(mpls_3_level_protection_basic_create_fec_entries(unit), "");

    /*
     * Create l3 forwarding entry
     */
    BCM_IF_ERROR_RETURN_MSG(mpls_3_level_protection_basic_create_l3_forwarding(unit), "");
    return BCM_E_NONE;
}

int
mpls_3_level_protection_basic_configure_port_properties(
    int unit)
{
    /*
     * Set In-Port to In ETh-RIF 
     */
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, cint_mpls_3_level_protection_basic_info.in_port, cint_mpls_3_level_protection_basic_info.intf_in), "intf_in");

    /*
     * Set Out-Port default properties 
     */
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, cint_mpls_3_level_protection_basic_info.out_port), "");

    return BCM_E_NONE;
}

int
mpls_3_level_protection_basic_create_l3_interfaces(
    int unit)
{

    l3_ingress_intf ingr_itf;
    l3_ingress_intf_init(&ingr_itf);

    /*
     * Create ETH-RIF and set its properties 
     */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_mpls_3_level_protection_basic_info.intf_in,
                             cint_mpls_3_level_protection_basic_info.intf_in_mac_address), "intf_in");

    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_mpls_3_level_protection_basic_info.intf_out,
                             cint_mpls_3_level_protection_basic_info.intf_out_mac_address), "intf_out");

    /*
     * Set Incoming ETH-RIF properties 
     */
    ingr_itf.intf_id = cint_mpls_3_level_protection_basic_info.intf_in;
    bcm_qos_map_id_get_by_profile(unit,BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_REMARK,cint_mpls_3_level_protection_basic_info.ingress_qos_profile, &ingr_itf.qos_map);
    ingr_itf.vrf = cint_mpls_3_level_protection_basic_info.vrf;
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingr_itf), "intf_out");

    return BCM_E_NONE;
}

int
mpls_3_level_protection_basic_create_arp_entry(
    int unit)
{
    /*
     * Configure ARP entry for push and swap actions
     */
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create(unit, 0, &(cint_mpls_3_level_protection_basic_info.arp_id),
                                       cint_mpls_3_level_protection_basic_info.arp_next_hop_mac,
                                       cint_mpls_3_level_protection_basic_info.intf_out), "");
    return BCM_E_NONE;
}

int
mpls_3_level_protection_basic_create_fec(
   int unit, 
   int flags,
   int id_idx)
{
    int gport_primary;
    int gport_backup;
    int fec_flags2 = 0;

    /*
     * 3rd hierarchy fec will result in out ports, the rest will point to the following (cascaded) fec
     */
    if (flags != BCM_L3_3RD_HIERARCHY)
    {
        BCM_GPORT_FORWARD_PORT_SET(gport_primary, cint_mpls_3_level_protection_basic_info.fec_ids_primary[id_idx + 1]);
        BCM_GPORT_FORWARD_PORT_SET(gport_backup, cint_mpls_3_level_protection_basic_info.fec_ids_backup[id_idx + 1]);
    }
    else
    {
        gport_primary = cint_mpls_3_level_protection_basic_info.out_port;
        gport_backup = cint_mpls_3_level_protection_basic_info.out_port;
    }

    if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        fec_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }
    /*
     * Create backup fec
     */
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create_inner(unit, 
                                           cint_mpls_3_level_protection_basic_info.fec_ids_backup[id_idx],
                                           0,
                                           cint_mpls_3_level_protection_basic_info.tunnel_ids_backup[id_idx],
                                           gport_backup,
                                           flags,
                                           fec_flags2,
                                           cint_mpls_3_level_protection_basic_info.failover_ids[id_idx],
                                           0,NULL), "");


    /*
     * Create primary fec
     */
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create_inner(unit, 
                                           cint_mpls_3_level_protection_basic_info.fec_ids_primary[id_idx],
                                           0,
                                           cint_mpls_3_level_protection_basic_info.tunnel_ids_primary[id_idx],
                                           gport_primary,
                                           flags,
                                           fec_flags2,
                                           cint_mpls_3_level_protection_basic_info.failover_ids[id_idx],
                                           1,NULL), ""); 

    return BCM_E_NONE;
}

int
mpls_3_level_protection_basic_create_failover_ids(
   int unit)
{
    int i = 0;
    int flags = 0;

    for (i = 0; i < NUM_FEC_LEVELS; i++)
    {
        if (i == 1)
        {
            flags = BCM_FAILOVER_FEC_2ND_HIERARCHY;
        }
        else if (i == 2)
        {
            flags = BCM_FAILOVER_FEC_3RD_HIERARCHY;
        }

        BCM_IF_ERROR_RETURN_MSG(bcm_failover_create(unit, flags | BCM_FAILOVER_WITH_ID | BCM_FAILOVER_FEC, &cint_mpls_3_level_protection_basic_info.failover_ids[i]), "");

    }
    return BCM_E_NONE;
}

int
mpls_3_level_protection_basic_create_fec_entries(
    int unit)
{
    int i = 0;
    int flags  = 0;

    /*
     * Create failover IDs
     */
    BCM_IF_ERROR_RETURN_MSG(mpls_3_level_protection_basic_create_failover_ids(unit), "");

    /*
     * Create first fec - outlif + dest==fec (out port points to cascaded fec) 
     */
    BCM_IF_ERROR_RETURN_MSG(mpls_3_level_protection_basic_create_fec(unit, 0, 0), "");

    /*
     * Create second fec - outlif + dest==fec (out port points to third cascaded fec) 
     */
    BCM_IF_ERROR_RETURN_MSG(mpls_3_level_protection_basic_create_fec(unit, BCM_L3_2ND_HIERARCHY, 1), "");

    /*
     * Create third fec - outlif + port 
     */
    BCM_IF_ERROR_RETURN_MSG(mpls_3_level_protection_basic_create_fec(unit, BCM_L3_3RD_HIERARCHY, 2), "");

    return BCM_E_NONE;
}

int
mpls_3_level_protection_basic_create_mpls_tunnels(
    int unit,
    uint8 ttl_value,
    int *tunnel_ids,
    bcm_mpls_label_t *label_array)
{
    bcm_mpls_egress_label_t label[1];
    int i = 0;

    bcm_mpls_egress_label_t_init(label);

    /*
     * Each eedb entry should be added in a different phase
     */
    for (i = 0; i < NUM_FEC_LEVELS; i++){
        label[0].label = label_array[i];
        switch (i) {
        case 0:
            label[0].encap_access = bcmEncapAccessTunnel3;
            break;
        case 1:
            label[0].encap_access = bcmEncapAccessTunnel2;
            break;
        case 2:
            label[0].encap_access = bcmEncapAccessTunnel1;
            break;
        default:
            break;
        }
        
        if (i == 2)
        {
            /*   
             * Next pointer of the last entry should be the arp 
             */
            label[0].l3_intf_id = cint_mpls_3_level_protection_basic_info.arp_id;
        }
        label[0].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
        label[0].egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
        label[0].egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
        label[0].ttl = ttl_value;

        BCM_IF_ERROR_RETURN_MSG(bcm_mpls_tunnel_initiator_create(unit, 0, 1, label), "");
        tunnel_ids[i] = label[0].tunnel_id;
    }

    return BCM_E_NONE;
}

int
mpls_3_level_protection_basic_create_all_mpls_tunnels(
    int unit,
    uint8 ttl_value)
{

    /*
     * Create primary mpls tunnels
     */
    BCM_IF_ERROR_RETURN_MSG(mpls_3_level_protection_basic_create_mpls_tunnels(unit,
                                                           ttl_value,
                                                           cint_mpls_3_level_protection_basic_info.tunnel_ids_primary,
                                                           cint_mpls_3_level_protection_basic_info.labels_array_primary), "");

    /*
     * Create secondary mpls tunnels
     */
    BCM_IF_ERROR_RETURN_MSG(mpls_3_level_protection_basic_create_mpls_tunnels(unit,
                                                           ttl_value,
                                                           cint_mpls_3_level_protection_basic_info.tunnel_ids_backup,
                                                           cint_mpls_3_level_protection_basic_info.labels_array_backup), "");


    return BCM_E_NONE;
}

int
mpls_3_level_protection_basic_create_l3_forwarding(
    int unit)
{

    /*
     * Create a routing entry in lem with DIP, vrf and first primary fec which points to it
     */
    BCM_IF_ERROR_RETURN_MSG(add_host_ipv4(unit, cint_mpls_3_level_protection_basic_info.dip, cint_mpls_3_level_protection_basic_info.vrf,
                       cint_mpls_3_level_protection_basic_info.fec_ids_primary[0], 0, 0), "");

    return BCM_E_NONE;
}

/*
 * Set the protection path for the fec level through the failover id set in bcm_failover_create
 */
int
mpls_3_level_protection_basic_set_failover(
   int unit,
   int fec_level,
   int protection_enable)
{

    printf("Received fec_level %d, protection_enable %d\n", fec_level, protection_enable);
    if ((fec_level < 1) || (fec_level > 3))
    {
        printf("Error, in mpls_3_level_protection_basic_set_failover - illegal fec level id %d\n", fec_level);
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_failover_set(unit, cint_mpls_3_level_protection_basic_info.failover_ids[fec_level - 1], protection_enable), "");
    return BCM_E_NONE;
}
