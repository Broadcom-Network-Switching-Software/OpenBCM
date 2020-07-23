
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 */

/*
 * Set snoop configuration
 */
int set_rx_snoop(int unit,int* snoop_cmnd_handle){

    int rv = BCM_E_NONE;
    int flags = 0x00; 	/*Creat snnop without ID*/
    int snoop_cmnd=0xff; /*Set default value for snoop handle*/
    bcm_rx_snoop_config_t config;

    rv = bcm_rx_snoop_create(unit,flags,&snoop_cmnd); /*Creat snoop handle*/
    printf("snoop_cmnd is $snoop_cmnd \n");
    if (rv != BCM_E_NONE) {
        printf("Error, in snoop create, snoop_cmnd $snoop_cmnd \n");
        return rv;
    }

    *snoop_cmnd_handle = snoop_cmnd; /*Update snoop command handle*/
    /*sal_memset(&config, 0, sizeof(config));*/
    config.flags |= (BCM_RX_SNOOP_UPDATE_DEST | BCM_RX_SNOOP_UPDATE_PRIO | BCM_RX_SNOOP_UPDATE_COLOR | BCM_RX_SNOOP_UPDATE_POLICER
                      | BCM_RX_SNOOP_UPDATE_ETHERNET_POLICER | BCM_RX_SNOOP_UPDATE_COUNTER_1);
    config.dest_port =1; 
    config.prio =1;
    config.color =2;
    config.policer_id =3;
    config.counter_num =5;
    config.size = -1; 
    config.probability= 100000;
    /*Set snoop congiguration*/
    rv = bcm_rx_snoop_set(unit,snoop_cmnd,&config);
    if (rv != BCM_E_NONE) {
        printf("Error, in snoop set\n");
        return rv;
    }
    return rv;
}

/*
 * Set trap configuration
 */
int set_rx_trap(int unit, bcm_rx_trap_t trap_type,int snoop_cmnd,int* trap_id_handle, int port){

    int rv = BCM_E_NONE;
    bcm_rx_trap_config_t config;
    int trap_id=0xff;
    int flags = 0;

    sal_memset(&config, 0, sizeof(config));
    /*for port dest change*/

    config.flags |= (BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_UPDATE_PRIO | BCM_RX_TRAP_UPDATE_COLOR |
                      BCM_RX_TRAP_UPDATE_ETHERNET_POLICER | BCM_RX_TRAP_UPDATE_COUNTER | BCM_RX_TRAP_REPLACE); 

    config.color = 2;
    config.trap_strength = 5; 
    config.counter_num = 4;
    config.dest_port= port;
       
    config.snoop_cmnd = snoop_cmnd; 
    config.snoop_strength = 3; /* Strength of this trap, strongest snoop in processing holds */


    config.flags |= BCM_RX_TRAP_UPDATE_POLICER;
    config.policer_id = 0x4000;
 

    rv = bcm_rx_trap_type_create(unit,flags,trap_type,&trap_id);

    printf("trap id is $trap_id \n");
    if (rv != BCM_E_NONE) {
        printf("Error, in trap create, trap id $trap_id \n");
        return rv;
    }
    
    *trap_id_handle = trap_id; /*Update trap id handle*/
    rv = bcm_rx_trap_set(unit,trap_id,&config);
    if (rv != BCM_E_NONE) {
        printf("Error, in trap set \n");
        return rv;
    }
    return rv;
}


int
l2_addr_to_trap(int unit, int gport_id, bcm_mac_t mac1, bcm_vlan_t v1){
  int rv;
  bcm_gport_t g1;
  bcm_l2_addr_t l2_addr1;
  
  bcm_l2_addr_t_init(&l2_addr1,mac1,v1); 
  l2_addr1.flags = 0x00000020; /* static entry */
  g1 = gport_id;
  l2_addr1.port = g1;
  rv = bcm_l2_addr_add(unit,&l2_addr1);
  
  printf("  0x%08x\n\r",rv);
  return rv;
}


/* 
 *  Trap Example
 *  This example sets trap type code and a snoop command. 
 *  The process: 
 *  1. Create rx snoop. 
 *  2. Set rx snoop. 
 *  3. Create rx trap. 
 *  4. Set rx trap with bcmRxTrapIpv4Ttl0. 
 *  5. Run traffic to be:
 *     packet to send: 
 *  - in port = in_port
 *  - vlan 1.
 *  - DA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}
 *  - DIP = 0x7fffff03 (127.255.255.03)
 *  Config Routing as followed (for bcmRxTrapIpv4Ttl0 only) 
 *  cint ../../../../src/examples/dpp/cint_ip_route.c
 *  cint
 *  int rv;
 *  rv = basic_example(unit,13,13);
 * expected: 
 *  Expectation: see Trapped packets being transfered to CPU.
 *  Example:
 *  Run to destination port 1. Ipv4- ttl = 0.
 *  Example run: run_rx_trap_example(unit,13,bcmRxTrapIpv4Ttl0); 
 *  NOTE: For Egress trap : currently only support dropping, and not re-direction  !!!
 */
int run_rx_trap_example(int unit,int port, bcm_rx_trap_t trap_type){
    int rv = BCM_E_NONE;
    int snoop_cmnd_handle;  /*Holds snoop command handle*/
    int trap_id_handle;    /*Holds trap id  handle*/
    bcm_mac_t mac1;
    bcm_gport_t g;

    /*rv = set_rx_snoop(unit,&snoop_cmnd_handle);
    if (rv != BCM_E_NONE) return rv;*/

    rv = set_rx_trap(unit,trap_type,snoop_cmnd_handle,&trap_id_handle,port);
    if (rv != BCM_E_NONE) return rv;

    BCM_GPORT_TRAP_SET(g, trap_id_handle, 0, 3);
    printf("######### gport is  $g  \n");
    mac1[5] = 0xff;
    rv = l2_addr_to_trap(unit, g, mac1, 1);
    if (rv != BCM_E_NONE) return rv;
        return BCM_E_NONE;
}


