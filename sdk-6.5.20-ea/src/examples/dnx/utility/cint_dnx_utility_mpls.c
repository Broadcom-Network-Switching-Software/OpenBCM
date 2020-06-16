/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_sand_utility_mpls.c Purpose: Various examples for MPLS. 
 */

/*
 *  
 * Configuration:
 *  
 * cint;                                                                  
 * cint_reset();                                                          
 * exit; 
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c  
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utility_mpls.c                                       
 * cint                                                                   
 * int unit = 0; 
 * int rv = 0; 
 * int in_port = 200; 
 * int out_port = 201;
 * rv = mpls_util_main(unit,in_port,out_port);
 * print rv; 
 * 
 * This cint can be used as MPLS utility cint to configure any MPLS setup.
 * cint usage:
 * 1) call init_default_mpls_params(unit, port1, port2) - defines basic MPLS setup with encapsulation, termination and forwarding MPLS tunnels.
 *          port1 and port2 are used to define the basic scenarios.
 *          other ports can be configured in next step.
 * 2) configure additional parameters and update existing ones if needed.
 * 3) call mpls_main(unit, port1, port2) - configure requested setup
 *
 * Note: is needed MAX_NOF_ parameters can and should be extended permenantly inside this cint.
 *
 *  Scenarios configured in this cint:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Each 'entity' represents a full configuration (both egress and ingress flows) of related ports.
 * In below example 1 port is mapped to each entity.
 * 
 * 
 * Setup configuration:
 *
 *             entity 1                                                                 entity 2
 * +-----------------------------------------+                           +-----------------------------------------+
 * |       INGRESS FLOW                      |                           |       INGRESS FLOW                      |
 * +-----------------------------------------+                           +-----------------------------------------+
 * |                                         |                           |                                         |
 * |   terminate mymac - 00:0c:00:02:00:01   |                           |   terminate mymac - 00:00:00:00:cd:1d   |
 * |                                         |                           |                                         |
 * |   terminate MPLS tunnels (0-4) - NONE   |                           |   terminate MPLS tunnels (0-4) - 3333   |
 * |                                         |                           |                                         |
 * |   perform IP/MPLS forwarding -          |                           |   perform IP/MPLS forwarding -          |
 * |      MPLS - 5555                        |                           |      MPLS - NONE                        |
 * |      IPv4 address - 160.161.161.163     |                           |      IPv4 address - 160.161.161.162     |
 * |                                         |                           |                                         |
 * +-----------------------------------------+                           +-----------------------------------------+
 * |       EGRESS FLOW                       |                           |       EGRESS FLOW                       |
 * +-----------------------------------------+                           +-----------------------------------------+
 * |                                         |                           |                                         |
 * |   encapsulate MPLS tunnels (0-4) - 3333 |                           |   encapsulate MPLS tunnels (0-4) - NONE |
 * |                                         |                           |                                         |
 * |   configure ETH header -                |                           |   configure ETH header -                |
 * |       next hop - 00:11:00:00:01:12      |                           |       next hop - 00:0c:00:02:00:00      |
 * |                                         |                           |                                         |
 * +-----------------------------------------+                           +-----------------------------------------+
 *
 * Transmitted packets between the 'entities':
 *
 *     ENTITY 1                                                    ENTITY 2
 *      port1                          ROUTER                       port2
 *  160.161.161.162                                             160.161.161.163
 * +------------------+           +----------------+
 * | IPv4oMPLSoETH1   |  FWD      |                |
 * |   label = 5555   +----------------------+     |
 * +------------------+           |          |     |           +------------------+
 *                                |    +---> +-----------------> IPv4oMPLSoETH1   |
 *                                |    |       +---------------+   label = 3333   |
 * +------------------+  ENCAP    |    |       |   |   TERM    +------------------+
 * | IPv4oETH1        +----------------+       |   |
 * |                  <------------------------+   |
 * +------------------+           |                |
 *                                +----------------+
 * 
 * Per packet explanation:
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~
 *  1) Basic MPLS forwarding
 *  ~~~~~~~~~~~~~~~~~~~~~~~~
 *  An MPLS label is swapped. Lookup in ILM results in an EEDB entry holding swap information.
 *  The ILM points to the EEDB (outlif) and to a FEC entry (holding the next hop information).
 *
 *
 *  Traffic:
 * 
 *  Send the following traffic:
 *
 *  ##############################################################################################
 *  Sending packet from port == port1 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA         | SA              ||   MPLS   ||  SIP          ||  DIP          ||
 *   |    |00:0C:00:02:00:01|00:0C:00:02:00:00||Label:5555||160.161.161.162||160.161.161.163||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *  Receiving packet on port == port2:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA         | SA              ||   MPLS   ||  SIP          ||  DIP          ||
 *   |    |00:11:00:00:01:12|00:00:00:00:CD:1D||Label:3333||160.161.161.162||160.161.161.163||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ###############################################################################################
 *
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  2) Basic MPLS Encapsulation
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Route into an MPLS core. Exit with a packet including an MPLS label
 *
 *  Traffic:
 * 
 *  Send the following traffic:
 *
 *  ##############################################################################################
 *  Sending packet from port == port1 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA         | SA              ||  SIP          ||  DIP          ||
 *   |    |00:0C:00:02:00:01|00:0C:00:02:00:00||160.161.161.162||160.161.161.163||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *  Receiving packet on port == port2:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA         | SA              ||   MPLS   ||  SIP          ||  DIP          ||
 *   |    |00:11:00:00:01:12|00:00:00:00:CD:1D||Label:3333||160.161.161.162||160.161.161.163||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ###############################################################################################
 * 
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~
 *   3) Basic MPLS Termination
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~
 *  One MPLS label is terminated. Lookup in routing table results in IP forwarding.
 *
 *  Traffic:
 * 
 *  Send the following traffic:
 *
 *  ##############################################################################################
 *  Sending packet from port == port2 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA         | SA              ||   MPLS   ||  SIP          ||  DIP          ||
 *   |    |00:00:00:00:CD:1D|00:11:00:00:01:12||Label:3333||160.161.161.163||160.161.161.162||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *  Receiving packet on port == port1:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA         | SA              ||  SIP          ||  DIP          ||  
 *   |    |00:0C:00:02:00:00|00:0C:00:02:00:01||160.161.161.163||160.161.161.162||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ###############################################################################################
 * Note: All three scenarios use the same EEDB entry for the relevant information. The label
 *       in the entry will swap the label(s) that was (were) matched (the "swapped" label(s)) in the ILM
 *       in case the forwarding context is MPLS. The label in the entry will be pushed in case
 *       the forwarding context is IP.
 */
int mpls_util_verbose = 1;
int MPLS_UTIL_INVALID_VALUE = -1;
int MPLS_UTIL_INVALID_GPORT = 0;
int mpls_params_set = 0;             /* indicates if cint parameters were initilized */
int MPLS_UTIL_MAX_NOF_TUNNELS = 10;
int MPLS_UTIL_MAX_NOF_PORTS = 10;
int MPLS_UTIL_MAX_NOF_RIFS = 10;
int MPLS_UTIL_MAX_NOF_ARPS = 10;
int MPLS_UTIL_MAX_NOF_FECS = 10;
int MPLS_UTIL_MAX_NOF_HOST_IPV4 = 10;
int MPLS_UTIL_MAX_NOF_HOST_IPV6 = 10;
int MPLS_UTIL_MAX_NOF_ENTITIES = 10;
int MPLS_UTIL_MIN_LABEL = 16;   /* labels less than 16 are special labels with predefined handling */
int MPLS_UTIL_MAX_LABEL = 0x000FFFFF;
int qos_map_id = 1;
bcm_mac_t mpls_util_mac1 = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x01 };
bcm_mac_t mpls_util_mac2 = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };
bcm_mac_t mpls_util_mac3 = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d };
bcm_mac_t mpls_util_mac4 = { 0x00, 0x11, 0x00, 0x00, 0x01, 0x12 };

bcm_ip6_t mpls_util_ipv6_addr_1 = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x13 };
bcm_ip6_t mpls_util_ipv6_addr_2 = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x44 };

struct mpls_util_port_s
{
    bcm_gport_t port;       /* port */
    int* eth_rif_id;        /* L3 VSI interface */    
};

struct mpls_util_eth_rif_s
{
    int intf;
    bcm_mac_t mac_addr;         /* source mac address of the port - mymac */
    int vrf;                    /* VRF */
    uint32 ingress_flags; /* BCM_L3_INGRESS_XXX */
    int ing_qos_map_id;
    bcm_qos_ingress_model_t ingress_qos_model;
};

struct mpls_util_arp_s
{
    uint32 flags;                /* Interface flags (BCM_L3_TGID, BCM_L3_L2TOCPU). */
    uint32 flags2;               /* See BCM_L3_FLAGS2_xxx flag definitions. */
    int arp;
    bcm_mac_t next_hop;         /* next hop mac address to be used */
    int* intf;                   /* outgoing intf */
    uint32 allocation_flags; /* BCM_L3_XXX - API flags */
};

struct mpls_util_fec_s
{
    int fec_id;                 /* fec id for encapsulation entry - outlif for MPLS entry in EEDB */
    uint32 flags;               /* Interface flags (BCM_L3_TGID, BCM_L3_L2TOCPU). */
    bcm_gport_t* port;           /* destination port */
    bcm_gport_t* fec;           /* destination FEC */
    bcm_if_t* tunnel_gport;      /* Either Tunnel or RIF or ARP */
    bcm_if_t* tunnel_gport2;     /* In case of two pointers, tunnel_gport2 is the second pointer */
    bcm_mpls_label_t mpls_label;        /* MPLS label. */
    bcm_mpls_egress_action_t mpls_action; /* MPLS action. */
    bcm_gport_t hierarchical_gport;     /* Hierarchical TM-Flow. */
};

struct mpls_util_mpls_tunnel_switch_create_s
{
    bcm_mpls_switch_action_t action;    /* MPLS label action. */
    bcm_mpls_label_t label;             /* Incoming label value. */
    bcm_mpls_label_t second_label;      /* second incoming label - used for upsteam assigned */
    uint32 flags;                       /* BCM_MPLS_SWITCH_xxx. */
    bcm_if_t *egress_if;                 /* Outgoing egress object. */
    bcm_gport_t *port;                   /* Incoming port. */
    bcm_mpls_label_t egress_label;      /* Outgoing label. */
    bcm_if_t ingress_if;                /* Ingress Interface object. */
    bcm_gport_t tunnel_id;              /* Tunnel ID. */
    int vrf;
};

struct mpls_util_mpls_tunnel_initiator_create_s
{
    bcm_mpls_egress_action_t action;    /* MPLS label action, relevant when BCM_MPLS_EGRESS_LABEL_ACTION_VALID is set. */
    bcm_mpls_label_t label[2];
    uint32 flags;                       /* BCM_MPLS_EGRESS_LABEL_xxx. */
    bcm_if_t tunnel_id;                 /* Tunnel Interface. */
    bcm_if_t * l3_intf_id;              /* l3 Interface ID. */
    int num_labels;                     /* number of labels to be handled in tunnel (valid values are 1 or 2) */
    bcm_encap_access_t encap_access;    /* Encapsulation Access stage */
    int qos_map_id;                     /* QOS map identifier. */
    bcm_qos_egress_model_t egress_qos_model; /* egress qos and ttl model */
    uint8 exp0; /* exp of label 0 */
    uint8 exp1; /* exp of label 1 */
};

struct mpls_util_host_ipvx_s
{
    int valid_ipv6;
    bcm_ip_t ipv4;                /* destination IPv4 used for forwarding FEC */
    bcm_ip6_t ipv6;                /* destination IPv6 used for forwarding FEC */
    int vrf;                    /* VRF */
    int* fec_id;                /* fec id for encapsulation entry - outlif for MPLS entry in EEDB */
};

struct mpls_util_entity_s
{
    mpls_util_port_s ports[MPLS_UTIL_MAX_NOF_PORTS];
    mpls_util_eth_rif_s rifs[MPLS_UTIL_MAX_NOF_RIFS];
    mpls_util_arp_s arps[MPLS_UTIL_MAX_NOF_ARPS];
    mpls_util_fec_s fecs[MPLS_UTIL_MAX_NOF_FECS];
    mpls_util_host_ipvx_s hosts_ipvx[MPLS_UTIL_MAX_NOF_HOST_IPV4];
    mpls_util_mpls_tunnel_initiator_create_s mpls_encap_tunnel[MPLS_UTIL_MAX_NOF_TUNNELS];
    mpls_util_mpls_tunnel_switch_create_s mpls_switch_tunnel[MPLS_UTIL_MAX_NOF_TUNNELS];
    int skip_fecs;
    int skip_hostss;
};

mpls_util_entity_s mpls_util_entity[MPLS_UTIL_MAX_NOF_ENTITIES];

void 
mpls_util_port_s_init(
    mpls_util_port_s * ports,
    int nof)
{
    int i;

    for (i = 0; i < nof; i++)
    {
        ports[i].port = MPLS_UTIL_INVALID_VALUE;
    }
}

void
mpls_util_eth_rif_s_init(
    mpls_util_eth_rif_s* rifs,
    int nof)
{
    int i;

    for (i = 0; i < nof; i++)
    {
        rifs[i].intf = MPLS_UTIL_INVALID_VALUE;
        rifs[i].vrf = 0;
        rifs[i].ingress_flags = 0;
    }
}

void
mpls_util_arp_s_init(
    mpls_util_arp_s* arps,
    int nof)
{
    int i;

    for (i = 0; i < nof; i++)
    {
        arps[i].flags = 0;
        arps[i].flags2 = 0;
        arps[i].arp = MPLS_UTIL_INVALID_VALUE;
        arps[i].allocation_flags = 0;
    }
}


void
mpls_util_fec_s_init(
    mpls_util_fec_s* fecs,
    int nof)
{
    int i;

    for (i = 0; i < nof; i++)
    {
        fecs[i].fec_id = MPLS_UTIL_INVALID_VALUE;
        fecs[i].flags = 0;
        fecs[i].mpls_action = 0;
        fecs[i].mpls_label = BCM_MPLS_LABEL_INVALID;
        fecs[i].tunnel_gport = &MPLS_UTIL_INVALID_GPORT;
        fecs[i].tunnel_gport2 = &MPLS_UTIL_INVALID_GPORT;
        fecs[i].hierarchical_gport = 0;
    }
}

void
mpls_util_host_ipvx_s_init(
    mpls_util_host_ipvx_s* hosts,
    int nof)
{
    int i;

    for (i = 0; i < nof; i++)
    {
        hosts[i].valid_ipv6 = 0;
        hosts[i].ipv4 = MPLS_UTIL_INVALID_VALUE;
        hosts[i].vrf = 0;
    }
}

void
mpls_tunnel_initiator_create_s_init(
    mpls_util_mpls_tunnel_initiator_create_s * tunnel,
    int nof)
{
    int i;

    for (i = 0; i < nof; i++)
    {
        tunnel[i].action = BCM_MPLS_EGRESS_ACTION_SWAP;
        tunnel[i].flags = 0;
        tunnel[i].num_labels = 0;
        tunnel[i].label[0] = BCM_MPLS_LABEL_INVALID;
        tunnel[i].label[1] = BCM_MPLS_LABEL_INVALID;
        tunnel[i].tunnel_id = 0;
        tunnel[i].encap_access = bcmEncapAccessInvalid;
        tunnel[i].qos_map_id = 0;
        tunnel[i].egress_qos_model.egress_qos = bcmQosEgressModelUniform;
        tunnel[i].egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
    }
}

void
mpls_tunnel_switch_create_s_init(
    mpls_util_mpls_tunnel_switch_create_s * info,
    int nof)
{
    int i;

    for (i = 0; i < nof; i++)
    {
        info[i].label = BCM_MPLS_LABEL_INVALID;
        info[i].second_label = BCM_MPLS_LABEL_INVALID;
        info[i].action = BCM_MPLS_SWITCH_ACTION_INVALID;
        info[i].flags = 0;
        info[i].ingress_if = 0;
        info[i].egress_label = BCM_MPLS_LABEL_INVALID;
        info[i].tunnel_id = 0;
        info[i].vrf = 0;
    }
}

void
mpls_util_s_init(
    mpls_util_entity_s* entity,
    int nof)
{
    int i;

    for(i = 0; i < nof; i++)
    {
        mpls_util_port_s_init(entity[i].ports, MPLS_UTIL_MAX_NOF_PORTS);
        mpls_util_eth_rif_s_init(entity[i].rifs, MPLS_UTIL_MAX_NOF_RIFS);
        mpls_util_arp_s_init(entity[i].arps, MPLS_UTIL_MAX_NOF_ARPS);
        mpls_util_fec_s_init(entity[i].fecs, MPLS_UTIL_MAX_NOF_FECS);
        mpls_util_host_ipvx_s_init(entity[i].hosts_ipvx, MPLS_UTIL_MAX_NOF_HOST_IPV4);
        mpls_tunnel_initiator_create_s_init(entity[i].mpls_encap_tunnel, MPLS_UTIL_MAX_NOF_TUNNELS);
        mpls_tunnel_switch_create_s_init(entity[i].mpls_switch_tunnel, MPLS_UTIL_MAX_NOF_TUNNELS);
        entity[i].skip_fecs = 0;
        entity[i].skip_hostss = 0;
    }
}

int
init_default_mpls_params(
    int unit,
    int port1,
    int port2)
{
    int rv = BCM_E_NONE;
    char *proc_name;

    proc_name = "init_default_mpls_params";
    /* printf("%s(): ENTER. port1 %d, port2 %d \r\n", proc_name, port1, port2); */

    mpls_util_s_init(mpls_util_entity, MPLS_UTIL_MAX_NOF_ENTITIES);

    /*
     * l2, mac, arp, fec parameters
     */
    mpls_util_entity[0].ports[0].port = port1;
    mpls_util_entity[0].ports[0].eth_rif_id = &mpls_util_entity[0].rifs[0].intf;

    mpls_util_entity[1].ports[0].port = port2;
    mpls_util_entity[1].ports[0].eth_rif_id = &mpls_util_entity[1].rifs[0].intf;
    /***************************************/
    mpls_util_entity[0].rifs[0].intf = 30;
    mpls_util_entity[0].rifs[0].mac_addr = mpls_util_mac1;
    mpls_util_entity[0].rifs[0].vrf = 1;

    mpls_util_entity[1].rifs[0].intf = 40;
    mpls_util_entity[1].rifs[0].mac_addr = mpls_util_mac3;
    mpls_util_entity[1].rifs[0].vrf = 100;
    /***************************************/
    mpls_util_entity[0].arps[0].arp = 0;
    mpls_util_entity[0].arps[0].next_hop = mpls_util_mac2;
    mpls_util_entity[0].arps[0].intf = &mpls_util_entity[0].rifs[0].intf;

    mpls_util_entity[1].arps[0].arp = 0;
    mpls_util_entity[1].arps[0].next_hop = mpls_util_mac4;
    mpls_util_entity[1].arps[0].intf = &mpls_util_entity[1].rifs[0].intf;
    /***************************************/
    mpls_util_entity[0].fecs[0].fec_id = 0;
    mpls_util_entity[0].fecs[0].port = &mpls_util_entity[1].ports[0].port;
    mpls_util_entity[0].fecs[0].tunnel_gport = &mpls_util_entity[0].mpls_encap_tunnel[0].tunnel_id;
    mpls_util_entity[0].fecs[0].tunnel_gport2 = &MPLS_UTIL_INVALID_GPORT;

    mpls_util_entity[1].fecs[0].fec_id = 0;
    mpls_util_entity[1].fecs[0].port = &mpls_util_entity[0].ports[0].port;
    mpls_util_entity[1].fecs[0].tunnel_gport = &mpls_util_entity[0].rifs[0].intf;
    mpls_util_entity[1].fecs[0].tunnel_gport2 = &mpls_util_entity[0].arps[0].arp;
    /***************************************/
    mpls_util_entity[0].hosts_ipvx[0].ipv4 = 0xA0A1A1A3; /* 160.161.161.163 */
    mpls_util_entity[0].hosts_ipvx[0].fec_id = &mpls_util_entity[0].fecs[0].fec_id;
    mpls_util_entity[0].hosts_ipvx[0].vrf = 1;
    mpls_util_entity[0].hosts_ipvx[0].valid_ipv6 = 1;
    mpls_util_entity[0].hosts_ipvx[0].ipv6 = mpls_util_ipv6_addr_2;

    mpls_util_entity[1].hosts_ipvx[0].ipv4 = 0xA0A1A1A2; /* 160.161.161.162 */
    mpls_util_entity[1].hosts_ipvx[0].fec_id = &mpls_util_entity[1].fecs[0].fec_id;
    mpls_util_entity[1].hosts_ipvx[0].vrf = 100;
    mpls_util_entity[1].hosts_ipvx[0].valid_ipv6 = 1;
    mpls_util_entity[1].hosts_ipvx[0].ipv6 = mpls_util_ipv6_addr_1;

    /*
     * mpls encapsulation tunnel parameters - used for bcm_mpls_tunnel_initiator_create
     */
    /* encapsulation entry */    
    mpls_util_entity[0].mpls_encap_tunnel[0].label[0] = 3333;
    mpls_util_entity[0].mpls_encap_tunnel[0].num_labels = 1;
    mpls_util_entity[0].mpls_encap_tunnel[0].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    mpls_util_entity[0].mpls_encap_tunnel[0].l3_intf_id = &mpls_util_entity[1].arps[0].arp;

    /*
     * mpls tunnel termination/forwarding parameters - used for bcm_mpls_tunnel_switch_create
     */
    /* forwarding entry */
    mpls_util_entity[0].mpls_switch_tunnel[0].label = 5555;
    mpls_util_entity[0].mpls_switch_tunnel[0].egress_if = &mpls_util_entity[0].fecs[0].fec_id;
    mpls_util_entity[0].mpls_switch_tunnel[0].action = BCM_MPLS_SWITCH_ACTION_NOP;
    mpls_util_entity[0].mpls_switch_tunnel[0].flags = BCM_MPLS_SWITCH_TTL_DECREMENT;

    /* termination entry */
    mpls_util_entity[1].mpls_switch_tunnel[0].action = BCM_MPLS_SWITCH_ACTION_POP;
    mpls_util_entity[1].mpls_switch_tunnel[0].label = mpls_util_entity[0].mpls_encap_tunnel[0].label[0];
    mpls_util_entity[1].mpls_switch_tunnel[0].vrf = 100;

    mpls_params_set = 1;

    /* printf("%s(): EXIT. port1 %d, port2 %d \r\n", proc_name, port1, port2); */
    return rv;
}

int
mpls_is_valid_lable(
    bcm_mpls_label_t label)
{
    if ((label >= MPLS_UTIL_MIN_LABEL) && (label <= MPLS_UTIL_MAX_LABEL))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
/*
 * Main function for basic swap and push mpls scnenarios in Jericho 2: 
 * 1) Swap scenario: Do swap with ILM pointing to an EEDB swap entry
 * 2) Push scenario: Forward into an MPLS core with 1 label. 
 */
int
mpls_util_main(
    int unit,
    int port1,
    int port2)
{
    int rv = BCM_E_NONE;
    int i;
    bcm_gport_t vlan_default;
    char *proc_name;

    proc_name = "mpls_util_main";
    /* printf("%s(): ENTER. port1 %d, port2 %d \r\n", proc_name, port1, port2); */
    if(!is_device_or_above(unit, JERICHO2))
    {
        printf("Only Jericho2 is supported\n");
        return BCM_E_CONFIG;
    }

    if (!mpls_params_set)
    {
        rv = init_default_mpls_params(unit, port1, port2);
        if (rv != BCM_E_NONE)
        {
            printf("Error(%d), in init_default_mpls_params\n", rv);
            return rv;
        }
    }

    if (mpls_util_verbose >= 1)
    {
        printf("%s(): ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n", proc_name);
    }
    rv = mpls_util_default_vlan(unit, &vlan_default);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_util_default_vlan\n");
        return rv;
    }
    /** Configure port properties */
    for (i = 0; i < MPLS_UTIL_MAX_NOF_ENTITIES; i++)
    {
        if (mpls_util_verbose >= 1)
        {
            printf("%s(): entity %d: Configure port properties\n", proc_name,  i);
        }
        rv = mpls_configure_port_properties(unit, mpls_util_entity[i].ports, MPLS_UTIL_MAX_NOF_PORTS, vlan_default);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in mpls_configure_port_properties\n");
            return rv;
        }
    }

    if (mpls_util_verbose >= 1)
    {
        printf("%s(): ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n", proc_name);
    }
    if (mpls_util_verbose >= 1)
    {
        printf("%s(): entity 0x%x: Configured Egress Port command \n", proc_name, vlan_default);
    }
    /** Configure ILM POP entries */
    for (i = 0; i < MPLS_UTIL_MAX_NOF_ENTITIES; i++)
    {
        if (mpls_util_verbose >= 1)
        {
            printf("%s(): entity %d: Configure ILM POP entries\n", proc_name, i);
        }
        rv = mpls_create_switch_tunnels(unit, mpls_util_entity[i].mpls_switch_tunnel, MPLS_UTIL_MAX_NOF_TUNNELS, 1);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in mpls_create_switch_tunnels POP\n");
            return rv;
        }
    }

    if (mpls_util_verbose >= 1)
    {
        printf("%s(): ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n", proc_name);
    }

    /** Configure L3 interfaces */
    for (i = 0; i < MPLS_UTIL_MAX_NOF_ENTITIES; i++)
    {
        if (mpls_util_verbose >= 1)
        {
            printf("%s(): entity %d: Configure L3 interfaces\n", proc_name, i);
        }
        rv = mpls_create_l3_interfaces(unit, mpls_util_entity[i].rifs, MPLS_UTIL_MAX_NOF_RIFS);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in mpls_create_l3_interfaces\n");
            return rv;
        }
    }

    if (mpls_util_verbose >= 1)
    {
        printf("%s(): ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n", proc_name);
    }

    /** Configure ARP entries */
    for (i = 0; i < MPLS_UTIL_MAX_NOF_ENTITIES; i++)
    {
        if (mpls_util_verbose >= 1)
        {
            printf("%s(): entity %d: Configure ARP entries\n", proc_name, i);
        }
        rv = mpls_create_arp_entries(unit, mpls_util_entity[i].arps, MPLS_UTIL_MAX_NOF_ARPS);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in mpls_create_arp_entries\n");
            return rv;
        }
    }

    if (mpls_util_verbose >= 1)
    {
        printf("%s(): ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n", proc_name);
    }

    /** Configure push or php entries */
    for (i = 0; i < MPLS_UTIL_MAX_NOF_ENTITIES; i++)
    {
        if (mpls_util_verbose >= 1)
        {
            printf("%s(): entity %d: Configure push or php entries\n", proc_name, i);
        }
        rv = mpls_create_initiator_tunnels(unit, mpls_util_entity[i].mpls_encap_tunnel, MPLS_UTIL_MAX_NOF_TUNNELS);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in mpls_create_initiator_tunnels\n", proc_name);
            return rv;
        }
    }

    if (mpls_util_verbose >= 1)
    {
        printf("%s(): ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n", proc_name);
    }

    /** Configure fec entries for forwarding flows */
    for (i = 0; i < MPLS_UTIL_MAX_NOF_ENTITIES; i++)
    {
        if (mpls_util_verbose >= 1)
        {    
            printf("entity %d: Configure fec entries for forwarding flows\n", i);
        }
        if(mpls_util_entity[i].skip_fecs)
        {
            if (mpls_util_verbose >= 1)
            {    
                printf("    SKIP\n", i);
            }
        }
        else
        {
            rv = mpls_create_fec_entries(unit, mpls_util_entity[i].fecs, MPLS_UTIL_MAX_NOF_FECS);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in mpls_create_fec_entries\n");
                return rv;
            }
        }
    }

    if (mpls_util_verbose >= 1)
    {
        printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    }

    /** Configure ILM PUSH entries */
    for (i = 0; i < MPLS_UTIL_MAX_NOF_ENTITIES; i++)
    {
        if (mpls_util_verbose >= 1)
        {
            printf("entity %d: Configure ILM PUSH entries\n", i);
        }
        rv = mpls_create_switch_tunnels(unit, mpls_util_entity[i].mpls_switch_tunnel, MPLS_UTIL_MAX_NOF_TUNNELS, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in mpls_create_switch_tunnels PUSH\n");
            return rv;
        }
    }

    if (mpls_util_verbose >= 1)
    {
        printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    }

    /** Create l3 forwarding entries for the pushed labels */
    for (i = 0; i < MPLS_UTIL_MAX_NOF_ENTITIES; i++)
    {
        if (mpls_util_verbose >= 1)
        {
            printf("entity %d: Configure l3 forwarding entries\n", i);
        }
        if(mpls_util_entity[i].skip_hostss)
        {
            if (mpls_util_verbose >= 1)
            {    
                printf("    SKIP\n", i);
            }
        }
        else
        {
            rv = mpls_create_l3_forwarding(unit, mpls_util_entity[i].hosts_ipvx, MPLS_UTIL_MAX_NOF_HOST_IPV4);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in mpls_create_l3_forwarding\n");
                return rv;
            }
        }
    }

    /* printf("%s(): EXIT. port1 %d, port2 %d \r\n", proc_name, port1, port2); */
    return rv;
}

/* Configure default egress vlan */
int
mpls_util_default_vlan(
    int unit,
    bcm_gport_t* vlan_default)
{
    int rv = BCM_E_NONE;

    
    bcm_vlan_port_t vlan_port;
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY;
    if (is_device_or_above(unit, JERICHO2))
    {
        vlan_port.flags |= BCM_VLAN_PORT_DEFAULT;
        vlan_port.flags |= BCM_VLAN_PORT_VLAN_TRANSLATION;
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
    }
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_port_create\n");
        return rv;
    }
    *vlan_default = vlan_port.vlan_port_id;

    return rv;
}

/* Configure ports entries */
int
mpls_configure_port_properties(
    int unit,
    mpls_util_port_s * l2_port,
    int nof_ports,
    bcm_gport_t vlan_default)
{
    int rv = BCM_E_NONE, port_idx;
    sand_utils_l3_port_s l3_port;

    for (port_idx = 0; port_idx < nof_ports; port_idx++)
    {
        if (l2_port[port_idx].port != BCM_PORT_INVALID)
        {
            sand_utils_l3_port_s_common_init(unit, 0, &l3_port, l2_port[port_idx].port, *l2_port[port_idx].eth_rif_id);
            l3_port.eg_vlan_port_id = vlan_default;
            rv = sand_utils_l3_port_set(unit, l3_port);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in sand_utils_l3_port_set for index %d\n", port_idx);
                return rv;
            }
        }
    }
    return rv;
}

/* Configure RIF entries */
int
mpls_create_l3_interfaces(
    int unit,
    mpls_util_eth_rif_s * rifs,
    int nof_rifs)
{
    int rv = BCM_E_NONE, rif_idx;
    sand_utils_l3_eth_rif_s l3_eth_rif;

    for (rif_idx = 0; rif_idx < nof_rifs; rif_idx++)
    {
        if (rifs[rif_idx].intf != MPLS_UTIL_INVALID_VALUE)
        {
            sand_utils_l3_eth_rif_s_common_init(unit, 0, &l3_eth_rif, rifs[rif_idx].intf, 0, rifs[rif_idx].ingress_flags, rifs[rif_idx].mac_addr, rifs[rif_idx].vrf);
            rv = sand_utils_l3_eth_rif_create(unit, l3_eth_rif);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in sand_utils_l3_eth_rif_create for index %d\n", rif_idx);
                return rv;
            }
        }
    }
    return rv;
}

/* Configure ARP entries */
int
mpls_create_arp_entries(
    int unit,
    mpls_util_arp_s* arps,
    int nof_arps)
{
    int rv = BCM_E_NONE, arp_idx;
    sand_utils_l3_arp_s arp_entry;

    for (arp_idx = 0; arp_idx < nof_arps; arp_idx++)
    {
        if (arps[arp_idx].arp != MPLS_UTIL_INVALID_VALUE)
        {
            sand_utils_l3_arp_s_common_init(unit, arps[arp_idx].allocation_flags, &arp_entry, arps[arp_idx].arp, arps[arp_idx].flags, arps[arp_idx].flags2, arps[arp_idx].next_hop, *arps[arp_idx].intf);
            rv = sand_utils_l3_arp_create(unit, arp_entry);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in sand_utils_l3_arp_create for index %d\n", arp_idx);
                return rv;
            }
            arps[arp_idx].arp = arp_entry.l3eg_arp_id;
        }
    }
    return rv;
}

/* Configure FEC entries */
int
mpls_create_fec_entries(
    int unit,
    mpls_util_fec_s* fecs,
    int nof_fecs)
{
    int rv = BCM_E_NONE, fec_idx;
    sand_utils_l3_fec_s fec_entry;
    uint32 allocation_flags = 0;
    bcm_gport_t destination = 0;
    bcm_if_t tunnel_gport_lif;

    allocation_flags |= BCM_L3_INGRESS_ONLY;

    for (fec_idx = 0; fec_idx < nof_fecs; fec_idx++)
    {
        if (fecs[fec_idx].fec_id != MPLS_UTIL_INVALID_VALUE)
        {
            if (fecs[fec_idx].fec)
            {
                BCM_GPORT_FORWARD_PORT_SET(destination, *fecs[fec_idx].fec);
            }
            else if (fecs[fec_idx].port)
            {
                destination = *fecs[fec_idx].port;
            }

            if(BCM_GPORT_SUB_TYPE_IS_LIF(*fecs[fec_idx].tunnel_gport))
            {
                tunnel_gport_lif = BCM_GPORT_SUB_TYPE_LIF_VAL_GET(*fecs[fec_idx].tunnel_gport);
                BCM_L3_ITF_SET(*fecs[fec_idx].tunnel_gport,BCM_L3_ITF_TYPE_LIF,tunnel_gport_lif);
            }

            sand_utils_l3_fec_s_common_init(unit, allocation_flags, 0, &fec_entry, fecs[fec_idx].fec_id, fecs[fec_idx].flags, 0, destination, *fecs[fec_idx].tunnel_gport, *fecs[fec_idx].tunnel_gport2);
            fec_entry.mpls_action = fecs[fec_idx].mpls_action;
            fec_entry.mpls_label = fecs[fec_idx].mpls_label;
            fec_entry.hierarchical_gport = fecs[fec_idx].hierarchical_gport;
            rv = sand_utils_l3_fec_create(unit, fec_entry);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in sand_utils_l3_fec_create for index %d\n", fec_idx);
                return rv;
            }

            fecs[fec_idx].fec_id = fec_entry.l3eg_fec_id;
        }
    }

    return rv;
}

/* Configure MPLS egress tunnels */
int
mpls_create_initiator_tunnels(
    int unit,
    mpls_util_mpls_tunnel_initiator_create_s * mpls_tunnel,
    int nof_tunnels)
{
    bcm_mpls_egress_label_t label_array[2];
    int rv = BCM_E_NONE, tunnel_idx;
    char *proc_name;

    proc_name = "mpls_create_initiator_tunnels";
    /* printf("%s(): ENTER. nof_tunnels %d, label[0] %d \r\n", proc_name, nof_tunnels, mpls_tunnel->label[0]); */

    bcm_mpls_egress_label_t_init(&label_array[0]);

    for (tunnel_idx = nof_tunnels - 1; tunnel_idx >= 0; tunnel_idx--)
    {
        if (mpls_is_valid_lable(mpls_tunnel[tunnel_idx].label[0]) || (mpls_tunnel[tunnel_idx].action == BCM_MPLS_EGRESS_ACTION_PHP))
        {
            label_array[0].label = mpls_tunnel[tunnel_idx].label[0];
            label_array[0].flags = mpls_tunnel[tunnel_idx].flags;
            BCM_L3_ITF_SET(label_array[0].tunnel_id, BCM_L3_ITF_TYPE_LIF, mpls_tunnel[tunnel_idx].tunnel_id);
            if (mpls_tunnel[tunnel_idx].l3_intf_id)
            {
                label_array[0].l3_intf_id = *mpls_tunnel[tunnel_idx].l3_intf_id;
            }
            label_array[0].action = mpls_tunnel[tunnel_idx].action;
            label_array[0].encap_access = mpls_tunnel[tunnel_idx].encap_access;
            label_array[0].qos_map_id = mpls_tunnel[tunnel_idx].qos_map_id;
            label_array[0].egress_qos_model = mpls_tunnel[tunnel_idx].egress_qos_model;
            label_array[0].exp = mpls_tunnel[tunnel_idx].exp0;


            label_array[1].label = mpls_tunnel[tunnel_idx].label[1];
            /* Set the flags not including LOSS which should be set only on first label */
            label_array[1].flags = mpls_tunnel[tunnel_idx].flags & (~BCM_MPLS_EGRESS_LABEL_ALTERNATE_MARKING_LOSS_SET);
            BCM_L3_ITF_SET(label_array[1].tunnel_id, BCM_L3_ITF_TYPE_LIF, mpls_tunnel[tunnel_idx].tunnel_id);
            if (mpls_tunnel[tunnel_idx].l3_intf_id)
            {
                label_array[1].l3_intf_id = *mpls_tunnel[tunnel_idx].l3_intf_id;
            }
            label_array[1].action = mpls_tunnel[tunnel_idx].action;
            label_array[1].encap_access = mpls_tunnel[tunnel_idx].encap_access;
            label_array[1].qos_map_id = mpls_tunnel[tunnel_idx].qos_map_id;
            label_array[1].egress_qos_model = mpls_tunnel[tunnel_idx].egress_qos_model;
            label_array[1].exp = mpls_tunnel[tunnel_idx].exp1;

            rv = bcm_mpls_tunnel_initiator_create(unit, 0, mpls_tunnel[tunnel_idx].num_labels, label_array);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in bcm_mpls_tunnel_initiator_create for index %d\n", tunnel_idx);
                return rv;
            }

            mpls_tunnel[tunnel_idx].tunnel_id = label_array[0].tunnel_id;

            if(verbose >= 1) {
                printf("Created MPLS tunnel: id = 0x%08x with: label_1 = 0x%x, label_2 = 0x%x", mpls_tunnel[tunnel_idx].tunnel_id, mpls_tunnel[tunnel_idx].label[0], mpls_tunnel[tunnel_idx].label[1]);
                if(mpls_tunnel[tunnel_idx].l3_intf_id)
                {
                    printf(", l3_intf_id = 0x%x", *mpls_tunnel[tunnel_idx].l3_intf_id);
                }
                printf("\n");
            }
        }
    }

    /* printf("%s(): EXIT. nof_tunnels %d, label[0] %d \r\n", proc_name, nof_tunnels, mpls_tunnel->label[0]); */
    return rv;
}

/*
* Configure MPLS switch tunnels
* This function must be called separately to handle BCM_MPLS_SWITCH_ACTION_POP and other actions
* 'action_pop' must be set for BCM_MPLS_SWITCH_ACTION_POP
*/
int
mpls_create_switch_tunnels(
    int unit,
    mpls_util_mpls_tunnel_switch_create_s * mpls_switch,
    int nof_switches,
    int action_pop)
{
    bcm_mpls_tunnel_switch_t entry;
    int rv = BCM_E_NONE, switch_idx;
    bcm_l3_ingress_t l3_ing_if;
    bcm_if_t l3_intf_id;
    bcm_if_t egress_if = 0;

    for (switch_idx = 0; switch_idx < nof_switches; switch_idx++)
    {
        if (mpls_is_valid_lable(mpls_switch[switch_idx].label) &&
            ((action_pop & (mpls_switch[switch_idx].action == BCM_MPLS_SWITCH_ACTION_POP)) ||
            (!action_pop & (mpls_switch[switch_idx].action != BCM_MPLS_SWITCH_ACTION_POP))))
        {
            bcm_mpls_tunnel_switch_t_init(&entry);

            entry.action = mpls_switch[switch_idx].action;
            entry.label = mpls_switch[switch_idx].label;
            entry.second_label = mpls_switch[switch_idx].second_label;
            entry.flags = mpls_switch[switch_idx].flags;
            if (mpls_switch[switch_idx].egress_if)
            {
                egress_if = *mpls_switch[switch_idx].egress_if;
                BCM_L3_ITF_SET(entry.egress_if, BCM_L3_ITF_TYPE_FEC, egress_if);
            }
            entry.ingress_if = mpls_switch[switch_idx].ingress_if;
            if (mpls_switch[switch_idx].port)
            {
                BCM_GPORT_FORWARD_PORT_SET(entry.port, *mpls_switch[switch_idx].port);
            }
            entry.egress_label.label = mpls_switch[switch_idx].egress_label;
            entry.tunnel_id = mpls_switch[switch_idx].tunnel_id;

            rv = bcm_mpls_tunnel_switch_create(unit, &entry);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in bcm_mpls_tunnel_switch_create for index %d\n", switch_idx);
                return rv;
            }

            mpls_switch[switch_idx].tunnel_id = entry.tunnel_id;

            if(mpls_util_verbose >= 1) {
                printf("Created MPLS switch: label_1 = 0x%x, label_2 = 0x%x, egress_label = 0x%x, ingress_if = 0x%x, egress_if = 0x%x, tunnel_id = 0x%x\n", mpls_switch[switch_idx].label, mpls_switch[switch_idx].second_label, mpls_switch[switch_idx].egress_label, mpls_switch[switch_idx].ingress_if, egress_if, mpls_switch[switch_idx].tunnel_id);
            }

            if (mpls_switch[switch_idx].action == BCM_MPLS_SWITCH_ACTION_POP)
            {
                /* RIF is the mpls tunnel id */
                bcm_l3_ingress_t_init(&l3_ing_if);
                BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(l3_intf_id, entry.tunnel_id);
                l3_ing_if.vrf = mpls_switch[switch_idx].vrf;
                l3_ing_if.flags = BCM_L3_INGRESS_WITH_ID; /* must, as we update exist RIF */
                rv = bcm_l3_ingress_create(unit, &l3_ing_if, l3_intf_id);
                if (rv != BCM_E_NONE)
                {
                    printf("Error, intf_ingress_rif_set for index %d\n", switch_idx);
                    return rv;
                }
            }
        }
    }

    return rv;
}

/* Create l3 forwarding entries binding {destination(IPv4/IPv6), VRF} with FEC */
int
mpls_create_l3_forwarding(
    int unit,
    mpls_util_host_ipvx_s* host,
    int nof_hosts)
{
    int rv = BCM_E_NONE, host_idx;
    sand_utils_l3_host_ipv4_s l3_host_ipv4;
    sand_utils_l3_host_ipv6_s l3_host_ipv6;

    for (host_idx = 0; host_idx < nof_hosts; host_idx++)
    {
        if (host[host_idx].ipv4 != MPLS_UTIL_INVALID_VALUE)
        {
            sand_utils_l3_host_ipv4_s_common_init(unit, 0, &l3_host_ipv4, host[host_idx].ipv4, host[host_idx].vrf, 0, 0, *host[host_idx].fec_id);
            rv = sand_utils_l3_host_ipv4_add(unit, l3_host_ipv4);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in sand_utils_l3_host_ipv4_add for index %d\n", host_idx);
                return rv;
            }
        }
        
        if (host[host_idx].valid_ipv6)
        {
            sand_utils_l3_host_ipv6_s_common_init(unit, 0, &l3_host_ipv6, host[host_idx].ipv6, host[host_idx].vrf, 0, 0, *host[host_idx].fec_id);
            rv = sand_utils_l3_host_ipv6_add(unit, l3_host_ipv6);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in sand_utils_l3_host_ipv6_add for index %d\n", host_idx);
                return rv;
            }
        }
    }

    return rv;
}


