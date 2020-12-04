/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/* 
 * the cint creates 2 MiM (backbone) ports connected to B-VID 
 * and 2 vlan (access) ports connected to VSI. 
 * for flooding: 
 *  the B-VID MC group contains the 2 MiM ports. 
 *  the VSI MC group contains the 2 vlan ports and the 2 MiM ports.
 *  
 *
 * set following soc properties
 * # Specify the "Auxiliary table" mode. 0: Private VLAN support, 1: Split horizon mode, 2: Mac-In-Mac support.
 * bcm886xx_auxiliary_table_mode=2
 * # Select whether to enable/disable SA authentication on the device
 * sa_auth_enabled=0
 * # Set the ARP table (next Hop MAC address) not to be extended 
 * bcm886xx_next_hop_mac_extension_enable=0  
 * run:
 *
 cd ../../../../src/examples/dpp
 cint cint_mim_mp.c
 cint cint_field_mim_learn_info_set.c
 cint
 int mim_vsi = 5; 
 * main(unit, port1, port2, port3, port4,0,mim_vsi); 
 *  
 * for B-VID flooding: 
 * run packet from <port1>: 
 *      mac-in-mac header with B-DA 001122334455, B-SA abcdef123412 
 *      B-tag: B-VID 5, ethertype 0x81a8
 *      I-tag: I-SID 16777215, ethertype 0x88e7
 * the packet will be flooded to the 2 MiM ports (<port1>,<port3>), as B-DA is unknown
 *  
 * for VSI flooding: 
 * run packet from port <port2>: 
 *      ethernet header with DA 5, SA 1 
 *      Vlan tag: vlan-id 8, vlan tag type 0x8100 
 * the packet will be flooded to all the ports (MiM and customer: <port1>,<port2>,<port3>,<port4>), as DA is unknown
 *  
 * for bridging: 
 * bridging_example(unit); 
 *  
 * run the packet used for B-VID flooding from <port1>: 
 *      mac-in-mac header with B-DA 001122334455, B-SA abcdef123412 
 *      B-tag: B-VID 5, ethertype 0x81a8
 *      I-tag: I-SID 16777215, ethertype 0x88e7 
 * the packet will arrive at <port3>
 * no termination will be done
 *  
 * for termination: 
 * termination_example(unit); 
 *  
 * run a B-VID packet from <port1>:
 *      mac-in-mac header with B-DA abcdef123412, B-SA 010203040506 
 *      B-tag: B-VID 5, ethertype 0x81a8
 *      I-tag: I-SID 16777215, ethertype 0x88e7
 *      ethernet header with DA 5, SA 1
 * the packet will arrive at <port2> with: 
 *      ethernet header with DA 5, SA 1
 * termination of the mac-in-mac header will be made 
 * the packet will be mapped to VSI 8
 *  
 * for encapsulation: 
 * encapsulation_example(unit); 
 *  
 * run the packet used for VSI flooding from <port2>: 
 *      ethernet header with DA 5, SA 1 
 *      Vlan tag: vlan-id 8, vlan tag type 0x8100 
 * the packet will arrive at <port1> with: 
 *      mac-in-mac header with B-DA abcdef123412, B-SA 010203040506 
 *      B-tag: B-VID 5, ethertype 0x81a8
 *      I-tag: I-SID 16777215, ethertype 0x88e7 
 *      ethernet header with DA 5, SA 1, vlan-id 8
 * encapsulation will be made
 * 
 * for ISID-broadcast:
 * for the ISID-broadcast example first set following soc properties:
 * "ucode_port_40.BCM88650=RCY.0" "tm_port_header_type_in_rcy.BCM88650=INJECTED_2_PP" "tm_port_header_type_out_rcy.BCM88650=ETH"
 *
 * description:
 * 1. First pass, BVID flooding bridging. Send packets to backbone copies. Add snoop copy to Recycle.
 * 2. Second pass, VSI flooding (MIM tunnel is terminated). Send packets to backbone and access copies.
 *    Backbone copies are filtered due to Split-Horizon filter.
 *
 * drop_and_continue_2pass_example(<unit>, <recycle_port>, <port_1>);
 *
 * run a B-VID packet from <port1>:
 *      mac-in-mac header with B-DA 011e83ffffff, B-SA 010203040506 
 *      B-tag: B-VID 5, ethertype 0x81a8
 *      I-tag: I-SID 16777215, ethertype 0x88e7
 *      ethernet header with DA 5, SA 1
 * the packet will arrive at <port3> with: 
 *      mac-in-mac header with B-DA 011e83ffffff, B-SA 010203040506 
 *      B-tag: B-VID 5, ethertype 0x81a8
 *      I-tag: I-SID 16777215, ethertype 0x88e7 
 *      ethernet header with DA 5, SA 1, vlan-id 8 
 * the packet will arrive at <port2> and <port4> with: 
 *      ethernet header with DA 5, SA 1 * 
 */

struct mim_info_s {
    /* physical ports: */
    /* MiM (backbone) ports */
    bcm_gport_t port_1;
    bcm_gport_t port_2;
    /* vlan (access) ports */
    bcm_port_t port_3;
    bcm_port_t port_4;

    int vlan_id; /* in VID */
    int bvlan_id; /* in B-VID */

    /* logical vlan ports */
    bcm_gport_t vlan_port_1;
    bcm_gport_t vlan_port_2;
    /* logical MiM ports */
    bcm_mim_port_t mim_port_1;
    bcm_mim_port_t mim_port_2;

    bcm_mim_vpn_config_t bvid; /* B-VID */
    bcm_mim_vpn_config_t vsi; /* VSI */
    
    bcm_mac_t src_bmac; /* out B-SA */
    bcm_mac_t dest_bmac; /* out B-DA = in B-SA */
    bcm_mac_t dest_bmac2; /* in B-DA */
    bcm_mac_t dest_mac; /* DA */

    bcm_l2_addr_t l2addr_1; 
    bcm_l2_addr_t l2addr_2;

    /*bcm_stg_t stg;*/
};

mim_info_s mim_info;


int
mim_info_init(int unit, int port1, int port2, int port3, int port4) {

    int rv;
    
    /* MiM (backbone) ports */
    mim_info.port_1 = port3;
    mim_info.port_2 = port1;
    /* access ports */
    mim_info.port_3 = port2;
    mim_info.port_4 = port4;

    mim_info.vlan_id = 8; /* in VID */
    mim_info.bvlan_id = 5; /* in B-VID */

    bcm_mim_vpn_config_t_init(&(mim_info.bvid));
    bcm_mim_vpn_config_t_init(&(mim_info.vsi));

    bcm_mim_port_t_init(&(mim_info.mim_port_1));
    bcm_mim_port_t_init(&(mim_info.mim_port_2));

    /* when going out of MiM port: */
    mim_info.dest_bmac[0] = 0xab;
    mim_info.dest_bmac[1] = 0xcd;
    mim_info.dest_bmac[2] = 0xef;
    mim_info.dest_bmac[3] = 0x12;
    mim_info.dest_bmac[4] = 0x34;
    mim_info.dest_bmac[5] = 0x12;

    mim_info.src_bmac[0] = 0x01; 
    mim_info.src_bmac[1] = 0x02;
    mim_info.src_bmac[2] = 0x03;
    mim_info.src_bmac[3] = 0x04;
    mim_info.src_bmac[4] = 0x05;
    mim_info.src_bmac[5] = 0x06; 

    /* when arriving in MiM port: */
    mim_info.dest_bmac2[0] = 0x00;
    mim_info.dest_bmac2[1] = 0x11;
    mim_info.dest_bmac2[2] = 0x22;
    mim_info.dest_bmac2[3] = 0x33;
    mim_info.dest_bmac2[4] = 0x44;
    mim_info.dest_bmac2[5] = 0x55;

    mim_info.dest_mac[0] = 0x0;
    mim_info.dest_mac[1] = 0x0;
    mim_info.dest_mac[2] = 0x0;
    mim_info.dest_mac[3] = 0x0;
    mim_info.dest_mac[4] = 0x0;
    mim_info.dest_mac[5] = 0x5;

    bcm_l2_addr_t_init(&(mim_info.l2addr_1), mim_info.dest_bmac, 0);
    bcm_l2_addr_t_init(&(mim_info.l2addr_2), mim_info.dest_mac, 0);

    /*rv = bcm_stg_create(unit, &(mim_info.stg)); 
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_stg_create\n");
        print rv;
    }*/

    return rv;
}

/* create vlan_port (Logical interface identified by port-vlan) */
int
vlan_port_create(
    int unit,
    bcm_port_t port_id,  
    bcm_gport_t *gport,
    bcm_vlan_t vlan /* incoming outer vlan and also outgoing vlan */
    ){

    int rv;
    bcm_vlan_port_t vp;

    bcm_vlan_port_t_init(&vp);
  
    vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vp.port = port_id;
    vp.match_vlan = vlan; 
    vp.egress_vlan = vlan; /* when forwarded to this port, packet will be set with this out-vlan */
    vp.vsi = 0; /* will be populated when the gport is added to service, using vswitch_port_add */
    vp.flags = BCM_VLAN_PORT_OUTER_VLAN_PRESERVE|BCM_VLAN_PORT_INNER_VLAN_PRESERVE; 
    /*vp.flags = 0;*/
    rv = bcm_vlan_port_create(unit, &vp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n"); 
        print rv;
        return rv;
    }

    *gport = vp.vlan_port_id;  
    return BCM_E_NONE;
}

/* configure bcm_mim_port_t with the given parameters */
void
mim_port_config(
    bcm_mim_port_t *mim_port, 
    int port, /* physical port */
    uint16 vpn, /* in B-VID */
    bcm_mac_t src_mac, /* in B-SA */ 
    uint16 bvid, /* out B-VID */
    bcm_mac_t dest_mac, /* out B-DA */
    uint32 isid /* egress I-SID - relevant for P2P ports only */ 
    ) {

    mim_port->flags = 0;
    mim_port->port = port;
    mim_port->match_tunnel_vlan = vpn;
    sal_memcpy(mim_port->match_tunnel_srcmac, src_mac, 6);
    mim_port->egress_tunnel_vlan = bvid; 
    sal_memcpy(mim_port->egress_tunnel_dstmac, dest_mac, 6);
    mim_port->egress_tunnel_service = isid;
    mim_port->failover_id = 0;
    mim_port->failover_gport_id = BCM_GPORT_TYPE_NONE;
}


/*
 
int unit = 0; 
int vpn = 3000; 
int bvid = 300; 
int port1 = 200; 
int dummy_failover_id = 8093;
bcm_mac_t src_mac = {0,0,0,1,2,3}; 
bcm_mac_t dest_mac = {0,0,0,0xa,0xb,0xc}; 
uint32 mim_gport_id;  
uint32 failover_gport_id; 
uint32 failover_encap_id; 
 
print mim_port_with_reserved_protection(unit,port1,vpn,src_mac,bvid,dest_mac,dummy_failover_id,&mim_gport_id, &failover_gport_id,&failover_encap_id); 
print mim_gport_id; 
print failover_gport_id; 
print failover_encap_id; 
 
int port2 = 202; 
bcm_mac_t src_mac2 = {0,0,0,4,5,6}; 
bcm_mac_t dest_mac2 = {0,0,0,0xd,0xe,0xf}; 
int vpn2 = 3001; 
int bvid2 = 301; 
int act_failover_id = 500;
mim_port_update_protection(unit,failover_gport_id,failover_encap_id,port2,vpn2,src_mac2,bvid2,dest_mac2,act_failover_id); 
 
mim_port_update_failover_id(unit,mim_gport_id,700);
mim_port_update_failover_id(unit,mim_gport_id,800);
  
main: 
main(unit,200,201,202,203,0,400); 
 
*/ 


uint32
mim_port_with_reserved_protection(
    int unit,
    int port, /* physical port */
    uint16 vpn, /* in B-VID */
    bcm_mac_t src_mac, /* in B-SA */ 
    uint16 bvid, /* out B-VID */
    bcm_mac_t dest_mac, /* out B-DA */
    uint32 dummy_failover_id,
    uint32 *mim_gport_id,
    uint32 *failover_gport_id,
    uint32 *failover_encap_id
    ) {
    uint32 rv;

    bcm_mim_port_t mim_port;
    bcm_mim_port_t_init(&mim_port);

    /* create Dummy protection */
    mim_port.flags = 0;
    mim_port.port = port;
    mim_port.match_tunnel_vlan = vpn;
    sal_memcpy(mim_port.match_tunnel_srcmac, src_mac, 6);
    mim_port.egress_tunnel_vlan = bvid; 
    sal_memcpy(mim_port.egress_tunnel_dstmac, dest_mac, 6);
    mim_port.egress_tunnel_service = 0;
    mim_port.failover_id = dummy_failover_id;
    mim_port.failover_gport_id = 0;
    rv = bcm_mim_port_add(unit, 1, &mim_port); /* use valid VPN (=1) to create MP port */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mim_port_add with mim_port_1\n");
        print rv;
        return rv;
    }
    *failover_gport_id = mim_port.mim_port_id;
    *failover_encap_id = mim_port.encap_id;
    /* create effective MiM port */
    mim_port.failover_gport_id = *failover_gport_id;

    rv = bcm_mim_port_add(unit, 1, &(mim_port)); /* use valid VPN (=1) to create MP port */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mim_port_add with mim_port_2\n");
        print rv;
        return rv;
    }

    *mim_gport_id = mim_port.mim_port_id;
    return rv;
}


uint32
mim_port_update_protection(
    int unit,
    int mim_port_id,
    int failover_encap_id,
    int port, /* physical port */
    uint16 vpn, /* in B-VID */
    bcm_mac_t src_mac, /* in B-SA */ 
    uint16 bvid, /* out B-VID */
    bcm_mac_t dest_mac, /* out B-DA */
    uint32 failover_id
    ) {
    uint32 rv;

    bcm_mim_port_t mim_port;
    bcm_mim_port_t_init(&mim_port);

    /* create Dummy protection */
    mim_port.flags = BCM_MIM_PORT_REPLACE|BCM_MIM_PORT_WITH_ID|BCM_MIM_PORT_ENCAP_WITH_ID;
    mim_port.mim_port_id = mim_port_id;
    mim_port.encap_id = failover_encap_id;
    mim_port.port = port;
    mim_port.match_tunnel_vlan = vpn;
    sal_memcpy(mim_port.match_tunnel_srcmac, src_mac, 6);
    mim_port.egress_tunnel_vlan = bvid; 
    sal_memcpy(mim_port.egress_tunnel_dstmac, dest_mac, 6);
    mim_port.egress_tunnel_service = 0;
    mim_port.failover_id = failover_id;
    mim_port.failover_gport_id = 0;
    rv = bcm_mim_port_add(unit, 1, &mim_port); /* use valid VPN (=1) to create MP port */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mim_port_add with mim_port_1\n");
        print rv;
        return rv;
    }

    return rv;
}



uint32
mim_port_update_failover_id(
    int unit,
    int mim_port_id,
    int new_failover_id
    ) {
    uint32 rv;

    bcm_mim_port_t mim_port;
    bcm_mim_port_t_init(&mim_port);

    mim_port.mim_port_id = mim_port_id;
    rv = bcm_mim_port_get(unit, 1, &mim_port); /* use valid VPN (=1) to create MP port */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mim_port_add with mim_port_1\n");
        print rv;
        return rv;
    }
    print mim_port;
    /* update failover-id protection */

    mim_port.flags = BCM_MIM_PORT_REPLACE|BCM_MIM_PORT_WITH_ID|BCM_MIM_PORT_ENCAP_WITH_ID;
    mim_port.failover_id = new_failover_id;
    rv = bcm_mim_port_add(unit, 1, &mim_port); /* use valid VPN (=1) to create MP port */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mim_port_add with mim_port_1\n");
        print rv;
        return rv;
    }

    return rv;
}

/* configure bcm_mim_vpn_config_t with the given parameters.
   if bvid - this vpn will be B-VID, else - this vpn will be VSI */
int
mim_vpn_config(
    int unit,
    bcm_mim_vpn_config_t *vpn_config, 
    int bvid, /* B-VID or VSI */
    int vpn_id, /* VPN ID. */ 
    int isid_id, /* I-SID */
    int tpid /* TPID of the internal Ethernet header */
    ) {

    int rv;
    bcm_multicast_t mc_group; /* mc_group = bc_group = uc_group */

    vpn_config->flags |= BCM_MIM_VPN_WITH_ID; 
    if (bvid) {
        vpn_config->flags |= BCM_MIM_VPN_BVLAN; /* set vpn as B-VID */
        mc_group = vpn_id + 12*1024;  /* for B-VID mc_group = vpn + 12k */
    }
    else { /* vpn will be VSI */
        mc_group = vpn_id; 
        /* the following fields are relevant only for VSI */
        vpn_config->lookup_id = isid_id; 
        vpn_config->match_service_tpid = tpid; 
    }
    vpn_config->vpn = vpn_id;

    /* open multicast group for the VPN */
    /* destroy before open, to ensure group does not exist */
    rv = bcm_multicast_destroy(unit, mc_group);
    if (rv != BCM_E_NONE && rv != BCM_E_NOT_FOUND) {
        printf("Error, bcm_multicast_destroy, mc_group $mc_group\n");
        print rv;
        return rv;
    }

    /* create MC group of type MiM */
    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_WITH_ID | BCM_MULTICAST_TYPE_MIM, &mc_group);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create, mc_group $mc_group\n");
        print rv; 
        return rv;
    }
    
    vpn_config->broadcast_group = mc_group;
    vpn_config->unknown_unicast_group = mc_group;
    vpn_config->unknown_multicast_group = mc_group;

    return BCM_E_NONE;
}

/* set unknown DA VSI flooding according to the incoming port (instead of per-vlan, by default)
   this will change the unknown DA MC group base (per port) and the actual MC group will be base + VSI */
int 
per_port_vsi_flooding_set(
    int unit, 
    bcm_port_t port,
    bcm_multicast_t *mc_group, /* mc_group id to be opened */
    int open_group, /* is this group a new one (that should be opened first) */
    int vsi
    ) {

    int rv;
    bcm_multicast_t base_mc;

    if (open_group) {

        /* destroy before open, to ensure group does not exist */
        rv = bcm_multicast_destroy(unit, *mc_group);
        if (rv != BCM_E_NONE && rv != BCM_E_NOT_FOUND) {
            printf("Error, bcm_multicast_destroy, mc_group %d\n", *mc_group);
            print rv;
            return rv;
        }

        /* open multicast group */
        rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_WITH_ID, mc_group);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_multicast_create for MC group %d\n", *mc_group);
            print rv; 
            return rv;
        }
    }
   
    /* bcm_port_control_set should get the base MC group and the actual MC group will be the base + VSI */
    base_mc = *mc_group - vsi;
    /* bcmPortControlFloodUnknownUcastGroup is used because the C-DA of the mim packet is UC */
    rv = bcm_port_control_set(unit, port, bcmPortControlFloodUnknownUcastGroup, base_mc);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_control_set of type bcmPortControlFloodUnknownUcastGroup with port %d\n", port);
        print rv; 
    }
    return rv;
}

int
mim_bvid_create(
    int unit,
    bcm_mim_vpn_config_t *vpn_config,
    int vpn_id, /* VPN ID */ 
    bcm_pbmp_t pbmp, /* port bit map */
    bcm_pbmp_t ubmp /* port bit map for untagged ports */
    /*bcm_stg_t stg*/
    ) {

    int rv;

    rv = mim_vpn_config(unit, vpn_config, 1 /* for B-VID */, vpn_id, 0, 0); 
    if (rv != BCM_E_NONE) {
        printf("Error, mim_vpn_config\n");
        return rv;
    }

    /* create the B-VID */
    rv = bcm_mim_vpn_create(unit, vpn_config);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mim_vpn_create\n");
        print rv;
        return rv;
    }
  
    /* set the B-VID port membership
     * add the ports to the replication list of the B-VID's MC group, which will be used for flooding
     * set B-VSI = B-VID 
     */ 
    rv = bcm_vlan_port_add(unit, vpn_config->vpn, pbmp, ubmp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_add for vlan %d\n", vpn_config->vpn);
        print rv;
        return rv;
    }

    /* put the B-VID in a spanning tree group */
    /*rv = bcm_vlan_stg_set(unit, vpn_config->vpn, stg);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_stg_set\n");
        print rv;
        return rv;
    }*/

    return BCM_E_NONE;
}

int
mim_vsi_create(
    int unit,
    bcm_mim_vpn_config_t *vpn_config,
    int vpn_id, /* VPN ID */ 
    int isid_id /* I-SID */
    ) {

    int rv;

    rv = mim_vpn_config(unit, vpn_config, 0 /* for VSI */, vpn_id, isid_id, 0x8100 /* internal Ethernet header TPID */); 
    if (rv != BCM_E_NONE) {
        printf("Error, mim_vpn_config\n");
        return rv;
    }

    /* create the VPN */
    rv = bcm_mim_vpn_create(unit, vpn_config);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mim_vpn_create\n");
        print rv;
        return rv;
    }

    return BCM_E_NONE;
}

int
mim_l2_addr_add(
    int unit, 
    bcm_l2_addr_t *l2addr,
    bcm_mac_t mac,
    bcm_gport_t port, 
    uint16 vid
    ) {

    int rv;

    bcm_l2_addr_t_init(l2addr, mac, vid);

    l2addr->port = port;
    l2addr->flags |= BCM_L2_STATIC;

    rv = bcm_l2_addr_add(unit, l2addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add\n");
        print rv;
        return rv;
    }

    return BCM_E_NONE;
}

int 
mim_l2_station_add(
    int unit,
    int msb, /* configure my-B-MAC MSB/LSB */
    int port, /* if port == -1 then port is invalid */
    bcm_mac_t mac,
    int *station_id
    ) {
    int rv;
    bcm_l2_station_t station; 

    bcm_l2_station_t_init(&station); 

    station.flags = BCM_L2_STATION_MIM;
    sal_memcpy(station.dst_mac, mac, 6);

    if (msb) { /* configure my-B-Mac global MSB */
        station.src_port_mask = 0; /* port is not valid */
        station.dst_mac_mask[0] = 0xff; /* dst_mac MSB should be used */
        station.dst_mac_mask[1] = 0xff;
        station.dst_mac_mask[2] = 0xff;
        station.dst_mac_mask[3] = 0xff;
        station.dst_mac_mask[4] = 0xff;
        station.dst_mac_mask[5] = 0x0;
    }
    else { /* configure my-B-Mac global or per-port LSB */
        station.dst_mac_mask[0] = 0x0; /* in both cases dst_mac LSB should be used */
        station.dst_mac_mask[1] = 0x0;
        station.dst_mac_mask[2] = 0x0;
        station.dst_mac_mask[3] = 0x0;
        station.dst_mac_mask[4] = 0x0;
        station.dst_mac_mask[5] = 0xff;

        if (port == -1) { /* port is not valid - set ingress global LSB, for termination */
            station.src_port_mask = 0; /* port is not valid */
        }
        else { /* port is valid - set egress per-port LSB, for encapsultaion */
            station.src_port_mask = -1; /* port is valid */
            station.src_port = port; /* src port (access port) */
        }
    }

    rv = bcm_l2_station_add(unit, station_id, &station);
    if (rv != BCM_E_NONE) {
        printf("bcm_l2_station_add  %d \n", *station_id);
        print rv;
    }
    return BCM_E_NONE;
}

 
int advanced_vlan_edit_ingress(int unit, int *action_id, bcm_vlan_action_t action_type,bcm_vlan_action_set_t *action, int flags, int mim_port1, int mim_port2, uint32 tpid){

    int rv = BCM_E_NONE;

    rv = port_outer_tpid_set(unit,mim_port1, tpid,0, *action_id); /* remove outer tag in case of single tagged packet in avt mode */
    if (rv != BCM_E_NONE) {
        printf("Error, port_outer_tpid_set,mim_info.port_1 \n");
        return rv;
    }
    rv = port_outer_tpid_set(unit,mim_port2, tpid,0, *action_id);
    if (rv != BCM_E_NONE) {
        printf("Error, port_outer_tpid_set,mim_info.port_2 \n");
        return rv;
    }

    rv = bcm_vlan_translate_action_id_create(unit,flags,action_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_create\n");
        return rv;
    }
        
    action->action_id = *action_id;
    action->dt_outer = action_type;
    action->outer_tpid = tpid; 

    rv = bcm_vlan_translate_action_id_set(unit,flags,*action_id,action); 
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_set\n");
        return rv;
    }

    return rv;


}

int advanced_vlan_edit_egress(int unit, int *action_id, bcm_vlan_port_translation_t *vlan_port_translation, bcm_gport_t gport, bcm_vlan_translate_action_class_t *action_class,  int vlan_edit_profile,  bcm_vlan_action_t action_type, bcm_vlan_action_set_t *action,  int flags, uint32 tpid){

    int rv = BCM_E_NONE;

    bcm_vlan_port_translation_t_init(vlan_port_translation);
    vlan_port_translation->flags = BCM_VLAN_ACTION_SET_EGRESS;
    vlan_port_translation->gport = gport; 
    vlan_port_translation->vlan_edit_class_id = vlan_edit_profile;
  
    rv = bcm_vlan_port_translation_set(unit, vlan_port_translation);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_translation_set\n");
        return rv;
    }

    /* Map the VLAN edit profile and tag format 0 (untagged) to an action ID */
    bcm_vlan_translate_action_class_t_init(action_class);
    action_class->flags = BCM_VLAN_ACTION_SET_EGRESS;
    action_class->vlan_edit_class_id = vlan_edit_profile;
    action_class->tag_format_class_id = 0;
    action_class->vlan_translation_action_id = *action_id;
    rv = bcm_vlan_translate_action_class_set(unit, action_class);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_class_set\n");
        return rv;
    }

    bcm_vlan_action_set_t_init(action);
    action->action_id = *action_id;

    printf("action_id: %d\n", *action_id);

    rv = bcm_vlan_translate_action_id_create(unit,flags,action_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_create\n");
        return rv;
    }

    action->dt_outer = action_type; /* performing AVT on the lif. It will add the vsi, in this case: B-VID*/
    action->outer_tpid = tpid;
    rv = bcm_vlan_translate_action_id_set(unit,flags,*action_id,action);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_set\n");
        return rv;
    }

    return rv;
}


int create_alternative_default_mim_ac(int unit, bcm_vlan_port_t *vp, int mim_port, bcm_port_match_info_t *match_info, bcm_gport_t *gport ){

    int rv = BCM_E_NONE;

    bcm_port_match_info_t_init(match_info);
    bcm_vlan_port_t_init(vp); 
      
    vp->criteria = BCM_VLAN_PORT_MATCH_NONE;
    vp->port = mim_port;
    vp->match_vlan = 0; 
    vp->vsi = 0; 
    vp->flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY; 
    rv = bcm_vlan_port_create(unit, vp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n"); 
        print rv;
        return rv;
    }

    *gport = vp->vlan_port_id;

    match_info->match = BCM_PORT_MATCH_PORT;
    match_info->port = mim_port; 
    match_info->match_vlan = -1;
    match_info->flags = BCM_PORT_MATCH_EGRESS_ONLY;

   
    
    rv = bcm_port_match_add(unit, *gport ,match_info); /* associating the port to lif */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_match_add\n"); 
        print rv;
        return rv;
    }

    return rv;
}

int 
main(int unit, int port1, int port2, int port3, int port4, int differential_flooding, int mim_vsi) {

    int rv, flags, ingress_action_id, egress_action_id, flags_ingress, flags_egress, vlan_edit_profile_1, vlan_edit_profile_2;
    uint32 advanced_vlan_translation_mode;
    bcm_pbmp_t pbmp, ubmp;
    bcm_if_t encap_id_1, encap_id_2;
    bcm_multicast_t mc_group, mc_group_1, mc_group_2;
    bcm_vlan_action_set_t action;
    bcm_vlan_port_translation_t vlan_port_translation;
    bcm_vlan_translate_action_class_t action_class;
    bcm_port_tpid_class_t tpid_class, tpid_class_2;
    bcm_gport_t gport;
    bcm_vlan_port_t vp;
    bcm_port_match_info_t match_info;
    int station_id;
    

    /* Configurations for advanced vlan editing*/
    sal_srand(sal_time());
    ingress_action_id =  (sal_rand() % 60) + 4; /* we deal only with action ids above 3*/

    vlan_edit_profile_1 = (sal_rand() % 7) + 1; /* we deal only with vlan_edit_profile between 1 and 7*/
    vlan_edit_profile_2 = vlan_edit_profile_1;
    while (vlan_edit_profile_1 == vlan_edit_profile_2) {vlan_edit_profile_2 =  (sal_rand() % 7) + 1;}
    flags_ingress = BCM_VLAN_ACTION_SET_INGRESS | BCM_VLAN_ACTION_SET_WITH_ID;
    flags_egress = BCM_VLAN_ACTION_SET_EGRESS | BCM_VLAN_ACTION_SET_WITH_ID;
    advanced_vlan_translation_mode = soc_property_get(unit , "bcm886xx_vlan_translate_mode",0);

    rv = mim_info_init(unit, port1, port2, port3, port4);
    if (rv != BCM_E_NONE) {
        printf("Error, mim_info_init\n");
        return rv;
    }
    /* init MiM on the device */
    rv = bcm_mim_init(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mim_init\n");
        print rv;
        return rv;
    }
    /* FP : Set learning information. (used in case of termination) */
    rv = mim_learn_info_example(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, mim_learn_info_example\n");
        print rv;
        return rv;
    }
    /* set the PBB-TE B-VID ranges */
    rv = bcm_switch_control_set(unit, bcmSwitchMiMTeBvidLowerRange, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set for lower range\n");
        print rv;
        return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchMiMTeBvidUpperRange, 11);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set for upper range\n");
        print rv;
        return rv;
    }

    /* set 2 physical ports as MiM (backbone) ports */
    rv = bcm_port_control_set(unit, mim_info.port_1, bcmPortControlMacInMac, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_control_set with port_1\n");
        print rv;
        return rv;
    }

    rv = bcm_port_control_set(unit, mim_info.port_2, bcmPortControlMacInMac, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_control_set with port_2\n");
        print rv;
        return rv;
    }

    /* make the MiM physical port recognize the B-tag ethertype */
    /* Set drop to all packets without I-tag. */
    port_tpid_init(mim_info.port_1, 1, 0);
    if (advanced_vlan_translation_mode) { /* configuring the default action id for this port*/
        port_tpid_info1.vlan_transation_profile_id = ingress_action_id; 
    }
    port_tpid_info1.outer_tpids[0] = 0x81a8;
    port_tpid_info1.discard_frames = BCM_PORT_DISCARD_TAG;
    rv = port_tpid_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, port_tpid_set with port_1\n");
        print rv;
        return rv;
    }

    /* Set drop to all packets without I-tag. */
    port_tpid_init(mim_info.port_2, 1, 0);
    port_tpid_info1.outer_tpids[0] = 0x81a8;
    port_tpid_info1.discard_frames = BCM_PORT_DISCARD_TAG;
    rv = port_tpid_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, port_tpid_set with port_2\n");
        print rv;
        return rv;
    }

    /* set MiM ports ingress vlan-editing to remove B-tag if exists */
    bcm_vlan_action_set_t_init(&action);

    if (advanced_vlan_translation_mode) {
          
        /* remove outer tag in case of single tagged packet in avt mode */  
        rv = advanced_vlan_edit_ingress(unit, &ingress_action_id,bcmVlanActionDelete ,&action, flags_ingress, mim_info.port_1, mim_info.port_2, 0x81a8);
        if (rv != BCM_E_NONE) {
            printf("Error, advanced_vlan_edit_ingress\n"); 
            print rv;
            return rv;
        }

        /* creating an alternative default out ac for AVT in MIM*/
        /*bcm_vlan_port_t_init(&vp); */
      
        
        rv = create_alternative_default_mim_ac(unit, &vp, mim_info.port_1, &match_info, &gport);
        if (rv != BCM_E_NONE) {
            printf("Error, create_alternative_default_mim_ac\n"); 
            print rv;
            return rv;
        }

        
        /* Attach the MIM vlan_port_ids to a VLAN edit profile */
        egress_action_id = ingress_action_id;

        while (egress_action_id == ingress_action_id) {egress_action_id =  (sal_rand() % 252) + 4;} /* we deal only with action ids above 3*/
        printf("egress_action_id: %d\n",egress_action_id );

        rv = advanced_vlan_edit_egress(unit, &egress_action_id, &vlan_port_translation, gport, &action_class, vlan_edit_profile_1,  bcmVlanActionMappedAdd, &action,  flags_egress, 0x81a8);
        if (rv != BCM_E_NONE) {
            printf("Error, advanced_vlan_edit_egress\n");
            return rv;
        }

        /*bcm_vlan_port_t_init(&vp);*/
        gport = 0; 


        rv = create_alternative_default_mim_ac(unit, &vp, mim_info.port_2, &match_info, &gport);
        if (rv != BCM_E_NONE) {
            printf("Error, create_alternative_default_mim_ac\n"); 
            print rv;
            return rv;
        }

        bcm_vlan_action_set_t_init(&action);
        ingress_action_id = egress_action_id;

        while (egress_action_id == ingress_action_id) {egress_action_id =  (sal_rand() % 252) + 4;} 
        printf("egress_action_id: %d\n",egress_action_id );

        rv = advanced_vlan_edit_egress(unit, &egress_action_id, &vlan_port_translation, gport, &action_class, vlan_edit_profile_2,  bcmVlanActionMappedAdd, &action,  flags_egress, 0x81a8);
        if (rv != BCM_E_NONE) {
            printf("Error, advanced_vlan_edit_egress\n");
            return rv;
        }

        
    }
    else{
        action.ot_outer = bcmVlanActionDelete; /* remove outer tag in case of single tagged packet */

        rv = bcm_vlan_port_default_action_set(unit, mim_info.port_1, &action);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_tpid_set with mim_info.port_1\n");
            print rv;
            return rv;
        }

        rv = bcm_vlan_port_default_action_set(unit, mim_info.port_2, &action);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_tpid_set with mim_info.port_2\n");
            print rv;
            return rv;
        }
    }

    /* prepare the port bit maps to set the B-VID port membership, with the 2 backbone ports (used for filtering) */ 
    BCM_PBMP_CLEAR(pbmp); /* port bit map for tagged ports */
    BCM_PBMP_CLEAR(ubmp); /* port bit map for untagged ports */
    BCM_PBMP_PORT_ADD(pbmp, mim_info.port_1);
    BCM_PBMP_PORT_ADD(pbmp, mim_info.port_2);

    /* create B-VID */
    rv = mim_bvid_create(unit, &(mim_info.bvid), mim_info.bvlan_id, pbmp, ubmp); 
    if (rv != BCM_E_NONE) {
        printf("Error, mim_vpn_create with bvid\n");
        return rv;
    }

    /* create 2 MiM (backbone) MP ports */
    mim_port_config(&(mim_info.mim_port_1), mim_info.port_1, mim_info.bvid.vpn, mim_info.src_bmac,
                    mim_info.bvid.vpn, mim_info.dest_bmac, 0); 
    rv = bcm_mim_port_add(unit, 1, &(mim_info.mim_port_1)); /* use valid VPN (=1) to create MP port */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mim_port_add with mim_port_1\n");
        print rv;
        return rv;
    }

    mim_port_config(&(mim_info.mim_port_2), mim_info.port_2, mim_info.bvid.vpn, mim_info.src_bmac, 
                    mim_info.bvid.vpn, mim_info.dest_bmac, 0); 
    rv = bcm_mim_port_add(unit, 1, &(mim_info.mim_port_2));
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mim_port_add with mim_port_2\n");
        print rv;
        return rv;
    }
  
    /* create VSI */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(ubmp);

    /* prepare the port bit maps to set the VSI port membership, with the 2 access ports (used for filtering) */ 
    BCM_PBMP_PORT_ADD(pbmp, mim_info.port_3);
    BCM_PBMP_PORT_ADD(pbmp, mim_info.port_4);

    /* VSI should be < 4K */
    rv = mim_vsi_create(unit, &(mim_info.vsi), mim_vsi, 16777215);
    if (rv != BCM_E_NONE) {
        printf("Error, mim_vpn_create with vsi\n");
        return rv;
    }

    /* create 2 access (vlan) ports: */
    rv = vlan_port_create(unit, mim_info.port_3, &(mim_info.vlan_port_1), mim_info.vlan_id);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan_port_create with port_3\n");
        return rv;
    }

    rv = vlan_port_create(unit, mim_info.port_4, &(mim_info.vlan_port_2), mim_info.vlan_id);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan_port_create with port_4\n");
        return rv;
    }

    /* add the vlan-ports to the vswitch, both will point at the same VSI */
    rv = bcm_vswitch_port_add(unit, mim_info.vsi.vpn, mim_info.vlan_port_1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_port_add\n");
        return rv;
    }

    rv = bcm_vswitch_port_add(unit, mim_info.vsi.vpn, mim_info.vlan_port_2);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_port_add\n");
        return rv;
    }

    /* set Vlan port membership */ 
    rv = bcm_vlan_port_add(unit, mim_info.vlan_id, pbmp, ubmp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_add for vlan %d\n", mim_info.vsi.vpn);
        print rv;
    }  

    if (differential_flooding) { /* 2 different MC groups for VSI flooding, one for access ports and one for backbone ports */

        /* existing group, will be used for flooding of packets coming from customer (access) ports
           flooding will send packets to backbone and customer ports */
        mc_group_1 = mim_info.vsi.vpn; /* bc = uc = mc */

        /* new group, will be used for flooding of packets coming from backbone ports
           flooding will send packets to customer (access) ports only */
        mc_group_2 = 2*1024 + mim_info.vsi.vpn;

        /* use physical port to set per-port flooding */
        rv = per_port_vsi_flooding_set(unit, mim_info.mim_port_1.port, &mc_group_2, 1 /* this is a new group that needs to be opened */, mim_info.vsi.vpn);
        if (rv != BCM_E_NONE) {
            printf("Error, per_lif_flooding_set for port %d\n", mim_info.mim_port_1.port);
            return rv;
        }

        rv = per_port_vsi_flooding_set(unit, mim_info.mim_port_2.port, &mc_group_2, 0 /* this group is already opened */, mim_info.vsi.vpn);
        if (rv != BCM_E_NONE) {
            printf("Error, per_lif_flooding_set for port %d\n", mim_info.mim_port_2.port);
            return rv;
        }

        /* this changes the unknown-DA MC group of all packets coming from the backbone ports,
           on isid-lif (VSI flooding after termination) and ac-lif (B-VSI flooding) */
    }
    else { /* single MC group for VSI */

        /* add members to the VSI multicast group (for flooding) */
        mc_group_1 = mim_info.vsi.vpn; /* bc = uc = mc */
        mc_group_2 = mc_group_1;
    }

    /* get the access ports' encap_id */
    rv = bcm_multicast_vlan_encap_get(unit, 0, mim_info.port_3, mim_info.vlan_port_1, &encap_id_1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_mim_encap_get with port_3\n");
        print rv;
        return rv;
    }
    printf("encap_id: %d\n", encap_id_1);

    rv = bcm_multicast_vlan_encap_get(unit, 0, mim_info.port_4, mim_info.vlan_port_2, &encap_id_2);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_mim_encap_get with port_4\n");
        print rv;
        return rv;
    }
    printf("encap_id: %d\n", encap_id_2);

    /* add the 2 access (vlan) ports to the VSI multicast groups */
    rv = bcm_multicast_ingress_add(unit, mc_group_2, mim_info.port_3, encap_id_1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_ingress_add with encap_id_1\n");
        print rv;
        return rv;
    }

    rv = bcm_multicast_ingress_add(unit, mc_group_2, mim_info.port_4, encap_id_2);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_ingress_add with encap_id_2\n");
        print rv;
        return rv;
    }

    if (differential_flooding) { /* mc_group_1 != mc_group_2 */

        rv = bcm_multicast_ingress_add(unit, mc_group_1, mim_info.port_3, encap_id_1);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_multicast_ingress_add with encap_id_1\n");
            print rv;
            return rv;
        }

        rv = bcm_multicast_ingress_add(unit, mc_group_1, mim_info.port_4, encap_id_2);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_multicast_ingress_add with encap_id_2\n");
            print rv;
            return rv;
        }
    }

    /* get the MiM ports' encap_id */
    rv = bcm_multicast_mim_encap_get(unit, 0, 0, mim_info.mim_port_1.mim_port_id, &encap_id_1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_mim_encap_get with mim_port_1\n");
        print rv;
        return rv;
    }

    rv = bcm_multicast_mim_encap_get(unit, 0, 0, mim_info.mim_port_2.mim_port_id, &encap_id_2);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_mim_encap_get with mim_port_2\n");
        print rv;
        return rv;
    }

    /* add the 2 MiM (backbone) ports to the VSI multicast group */
    rv = bcm_multicast_ingress_add(unit, mc_group_1, mim_info.mim_port_1.port, encap_id_1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_ingress_add with encap_id_1\n");
        print rv;
        return rv;
    }

    rv = bcm_multicast_ingress_add(unit, mc_group_1, mim_info.mim_port_2.port, encap_id_2);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_ingress_add with encap_id_2\n");
        print rv;
        return rv;
    }

    /* add the 2 MiM (backbone) ports to the B-VSI multicast group */
    mc_group = mim_info.bvlan_id + 12*1024; /* bc = uc = mc, for B-VID mc_group = B-Vlan + 12k */
    if (differential_flooding) { 
        /* port unknown-DA MC group of all packets coming from the backbone ports has been incremented by 2K */
        mc_group += 2*1024;

        /* destroy before open, to ensure group does not exist */
        rv = bcm_multicast_destroy(unit, mc_group);
        if (rv != BCM_E_NONE && rv != BCM_E_NOT_FOUND) {
            printf("Error, bcm_multicast_destroy, mc_group %d\n", mc_group);
            print rv;
            return rv;
        }

        /* open multicast group */
        rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_WITH_ID, &mc_group);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_multicast_create for MC group %d\n", mc_group);
            print rv; 
            return rv;
        }
    }

    rv = bcm_multicast_ingress_add(unit, mc_group, mim_info.mim_port_1.port, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_ingress_add with encap_id_1\n");
        print rv;
        return rv;
    }

    rv = bcm_multicast_ingress_add(unit, mc_group, mim_info.mim_port_2.port, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_ingress_add with encap_id_2\n");
        print rv;
        return rv;
    }

    /* for the packet to arrive at MiM ports with the correct B-SA: */
    /* set global B-SA MSB */
    rv = mim_l2_station_add(unit, 1 /* MSB */, 0, mim_info.src_bmac, &station_id);
    if (rv != BCM_E_NONE) {
        printf("Error, mim_l2_station_add (global)\n");
        print rv;
        return rv;
    }
    /* set per-src-access-port B-SA LSB */
    rv = mim_l2_station_add(unit, 0 /* LSB */, mim_info.port_3, mim_info.src_bmac, &station_id);
    if (rv != BCM_E_NONE) {
        printf("Error, mim_l2_station_add (per port)\n");
        print rv;
        return rv;
    }

    return BCM_E_NONE;
}

int 
bridging_example(int unit) {

    int rv;

    /* add the in B-DA and send to B-VID and port_1 */
    rv = mim_l2_addr_add(unit, &(mim_info.l2addr_1), mim_info.dest_bmac2, mim_info.mim_port_1.port, mim_info.bvid.vpn);
    if (rv != BCM_E_NONE) {
        printf("Error, mim_l2_addr_add with l2addr_1\n");
        return rv;
    }

    return BCM_E_NONE;
}

int 
termination_example(int unit) {
    int station_id;
    int rv;

    /* MiM port with B-SA and out B-VID was already created in main() */

    /* set global my-B-DA MSB */
    rv = mim_l2_station_add(unit, 1 /* MSB */, 0, mim_info.dest_bmac, &station_id);
    if (rv != BCM_E_NONE) {
        printf("Error, mim_l2_station_add (global MSB)\n");
        print rv;
        return rv;
    }

    /* set global my-B-DA LSB */
    rv = mim_l2_station_add(unit, 0 /* LSB */, -1 /* invalid port for global configuration */, mim_info.dest_bmac, &station_id);
    if (rv != BCM_E_NONE) {
        printf("Error, mim_l2_station_add (global LSB)\n");
        print rv;
        return rv;
    }

    /* add L2 address (DA) and send to access port */
    rv = mim_l2_addr_add(unit, &(mim_info.l2addr_2), mim_info.dest_mac, mim_info.vlan_port_1, mim_info.vsi.vpn);
    if (rv != BCM_E_NONE) {
        printf("Error, mim_l2_addr_add with l2addr_1\n");
        return rv;
    }

    return BCM_E_NONE;
}

int 
encapsulation_example(int unit) {
    
    int rv;
    int station_id;

    /* add L2 address (DA) and send to MiM port */
    rv = mim_l2_addr_add(unit, &(mim_info.l2addr_2), mim_info.dest_mac, mim_info.mim_port_2.mim_port_id, mim_info.vsi.vpn);
    if (rv != BCM_E_NONE) {
        printf("Error, mim_l2_addr_add with l2addr_1\n");
        return rv;
    }

    /* set global B-SA MSB: */
    rv = mim_l2_station_add(unit, 1 /* MSB */, 0, mim_info.src_bmac, &station_id);
    if (rv != BCM_E_NONE) {
        printf("Error, mim_l2_station_add (global)\n");
        print rv;
        return rv;
    }

    /* set per-port B-SA LSB:
       port should be an access port, because encapsulation B-SA is determined by the src port */
    rv = mim_l2_station_add(unit, 0 /* LSB */, mim_info.port_3, mim_info.src_bmac, &station_id);
    if (rv != BCM_E_NONE) {
        printf("Error, mim_l2_station_add (per port)\n");
        print rv;
        return rv;
    }

    return BCM_E_NONE;
}

/* set configuration for ISID-broadcast 2 pass functionality.
   1. snoop action for first pass - copy packet to recycle port
   2. PTCH addition to packet transmitted from recycle port */
int
drop_and_continue_2pass_example(int unit, int recycle_port, int port_1) {
    bcm_field_group_t snoop_group;
    bcm_gport_t snoop_trap_gport_id;
    int rv;

    /* set snooping to recycle port in case of B-MAC MC */
    rv = two_pass_snoop_set(unit, bcmRxTrapMimMyBmacMulticastContinue, recycle_port, &snoop_trap_gport_id);
    BCM_IF_ERROR_RETURN(rv);

    /* set egress PMF to add PTCH of port_1 when transmitting from recycle port*/
    rv = system_port_lookup_ptch(unit, 20, 17, port_1, recycle_port); 
    BCM_IF_ERROR_RETURN(rv);

    rv = bcm_port_control_set(unit, recycle_port, bcmPortControlOverlayRecycle, 1);
    BCM_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;
}

int 
test_mim_l2_station_add(
    int unit,
    int msb, 
    int port,
    int mac_low,
    int *station_id
    ) {
        bcm_mac_t   mac;
        int         rv;

        mac[0] = mac_low;
        mac[1] = 0x11;
        mac[2] = 0x22;
        mac[3] = 0x33;
        mac[4] = 0x44;
        mac[5] = 0x55;

        rv = mim_l2_station_add(unit, msb, port, mac, station_id);
        printf("test_mim_l2_station_add rv=%d\n", rv);

        return rv;
}

int bvlan = 0;

int
mim_bvid_create_example(
    int unit,
    int vpn_id
    ) {
    int rv = BCM_E_NONE;
    bcm_mim_vpn_config_t vpn_config;
    bcm_pbmp_t pbmp;
    bcm_pbmp_t ubmp;

    /* init MiM on the device */
    rv = bcm_mim_init(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mim_init.\n");
        return rv;
    }

    rv = mim_bvid_create(unit, &vpn_config, vpn_id, pbmp, ubmp);
    if (rv != BCM_E_NONE) {
        printf("Error, mim_bvid_create.\n");
        return rv;
    }
    bvlan = vpn_config.vpn;

    return BCM_E_NONE;
}
