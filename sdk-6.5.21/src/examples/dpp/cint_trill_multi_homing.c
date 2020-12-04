/*
* 
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 
 
Network diagram 
 
We configure RB1                                       
                                      host30
                                   o  host31
                                   |
                                   | V1,P4,S3 
                               ....|......................................
                              :    |                       TRILL Network  :
                              : +-----+       /\-/\-/\-/\-/\              :
                         +V1,P1-| RB1 |DV3,P3/---+-----+    \             :
                         |    : +-----+    /     | RB3 |    \             :
                         |    :    |      /      +-----+     \            :
                      +-----+ :   V1,P2   /      Transit     \    +-----+ :
       host10---S1 o--| SW1 | :   SW2-S2 |       RBridges     |---| RB4 |---o V1,S4---host40      
       host11         +-----+ :    |      \      Campus      /    +-----+ :           host41      
       host12            |    :  DV2       \                 /            :           host42      
       host13            |    : +-----+    \                /             :           host43      
                         +------| RB2 |-----\              /              :            
                              : +-----+      \/\-/\-/\-/\-/               :            
                              :                                           :
                               ...........................................
 
 
 Hosts connected to S2: host20
                        host21
                        host22
                        host23
 
 
 Traffic RBv <-> RB4: 
 
     #Ingress trill uc: using RBv. S1.host19 to S4.host40
	 #purpose: - check RBv nickname is used as ingress nickname instead of RB1 nickname.
	 #         - use MACT static entry: host40_mac
     #         - Learn how to reach host19: use port1
 
    Send:
             ----------------------------------
        eth: |    DA     |     SA      | VLAN |
             ----------------------------------
             | host40_mac | host19_mac |  v1  |
             ----------------------------------
    Recv:
             -----------------------------------------------------
        eth: |    DA          |     SA  | VLAN  |   ET           |
             -----------------------------------------------------
             | remote_mac_rb4 | mac_rb1 |  dv3  | trill_ethertype |
             ------------------------------------------------------
 
                    ---------------------------------------------------------------
             trill: | multi | hop count |        eRB nickname  | iRB nickname     |
                    ---------------------------------------------------------------
                    |   0   | hopcount  |  remote_nickname_rb4 | virtual_nickname |
                    ---------------------------------------------------------------
 
                         ----------------------------------
                    eth: |    DA      |     SA     | VLAN |
                         ----------------------------------
                         | host40_mac | host19_mac |  v1  |
                         ----------------------------------
 
 
 	#Egress trill uc: using RBv. S4.host40 to S1.host19.
	#purpose: - check RBv nickname is used to terminate trill header.
	#          - use MACT dynamic entry: "host19 learning"
 
    send:
             --------------------------------------------------------------
        eth: |    DA    |     SA         | VLAN         |         ET      |
             --------------------------------------------------------------
             |  mac_rb1 | remote_mac_rb4 |  vid_3_desig | trill_ethertype |
             --------------------------------------------------------------
 
                    -----------------------------------------------------------------
             trill: | multi | hop count |   eRB nickname       | iRB nickname        |
                    ------------------------------------------------------------------
                    |   0   | hopcount  | virtual_nickname     | remote_nickname_rb4 |
                    ------------------------------------------------------------------
 
                         ----------------------------------
                    eth: |    DA     |     SA      | VLAN |
                         ----------------------------------
                         | host19_mac | host40_mac |  v1  |
                         ----------------------------------
     Recv:
              ----------------------------------
         eth: |    DA     |     SA      | VLAN |
              ----------------------------------
              | host19_mac | host40_mac |  v1  |
              ----------------------------------
 
 
	#Egress trill uc: using RBv. S4.host49 to S1.host10.
	#purpose: - check RBv nickname is used to terminate trill header.
	#         - use MACT static entry: host10_mac
    #         - Learn how to reach host49: use trill tunnel: local_nickname_rb1 -> remote_nickname_rb4
 
    send:
             --------------------------------------------------------------
        eth: |    DA    |     SA         | VLAN         |         ET      |
             --------------------------------------------------------------
             |  mac_rb1 | remote_mac_rb4 |  vid_3_desig | trill_ethertype |
             --------------------------------------------------------------
 
                    -----------------------------------------------------------------
             trill: | multi | hop count |   eRB nickname       | iRB nickname        |
                    ------------------------------------------------------------------
                    |   0   | hopcount  | virtual_nickname     | remote_nickname_rb4 |
                    ------------------------------------------------------------------
 
                         ----------------------------------
                    eth: |    DA     |     SA      | VLAN |
                         ----------------------------------
                         | host10_mac | host49_mac |  v1  |
                         ----------------------------------
     Recv:
              ----------------------------------
         eth: |    DA     |     SA      | VLAN |
              ----------------------------------
              | host19_mac | host40_mac |  v1  |
              ---------------------------------- 
 
 
	#Ingress trill MC (flooding): using RBv. S1.host10 to unknown host
	#purpose: - check RBv nickname is used as ingress nickname instead of RB1 nickname for trill tunnel
	#         - use distribution tree nickname as egress nickname for trill tunnel
 
    Send:
             ----------------------------------------
        eth: |    DA            |     SA     | VLAN |
             ----------------------------------------
             | host_unknown_mac | host10_mac |  v1  |
             ----------------------------------------
 
    Recv 2 packets:
             -----------------------------------------------------
        eth: |    DA          |     SA  | VLAN  |   ET           |
             -----------------------------------------------------
             | remote_mac_rb4 | mac_rb1 |  dv3  | trill_ethertype |
             ------------------------------------------------------
 
                    -------------------------------------------------------------
             trill: | multi | hop count |      eRB nickname  | iRB nickname     |
                    -------------------------------------------------------------
                    |   1   | hopcount  | dist_tree_nickname | virtual_nickname |
                    -------------------------------------------------------------
 
                         ---------------------------------------- 
                    eth: |    DA            |     SA     | VLAN | 
                         ---------------------------------------- 
                         | host_unknown_mac | host10_mac |  v1  | 
                         ----------------------------------------
 
             -----------------------------------------            
        eth: |    DA            |     SA      | VLAN |            
             -----------------------------------------             
             | host_unknown_mac | host10_mac  |  v1  |            
             -----------------------------------------            
 
	#Egress trill UC (L2 flooding): using RBv. S4.host40 to unknown host
	#purpose: - check RBv nickname is used to terminate trill header.
	#         - check we are sending to all RBv members 
 
    send:
             --------------------------------------------------------------
        eth: |    DA    |     SA         | VLAN         |         ET      |
             --------------------------------------------------------------
             |  mac_rb1 | remote_mac_rb4 |  vid_3_desig | trill_ethertype |
             --------------------------------------------------------------
 
                    -----------------------------------------------------------------
             trill: | multi | hop count |   eRB nickname       | iRB nickname        |
                    ------------------------------------------------------------------
                    |   0   | hopcount  | virtual_nickname     | remote_nickname_rb4 |
                    ------------------------------------------------------------------
 
                         -----------------------------------------
                    eth: |    DA            |     SA      | VLAN |
                         -----------------------------------------
                         | host_unknown_mac | host49_mac  |  v1  |
                         -----------------------------------------
     Recv:
              ----------------------------------------- 
         eth: |    DA            |     SA      | VLAN | 
              ----------------------------------------- 
              | host_unknown_mac | host49_mac  |  v1  | 
              -----------------------------------------
 
 
    #Ingress trill MC (registered multicast): using RBv. s1.host10 to 'registered MC' mac address
    #purpose: - check RBv nickname is used as ingress nickname instead of RB1 nickname for trill tunnel
    #         - use distribution tree nickname as egress nickname for trill tunnel
    #         - check packet is replicated to all flooding MC members 
 
 
 
	#Egress trill MC (registered multicast): using RBv. S4.host40 to 'registered MC' mac address
	#purpose: - check RBv nickname is used to terminate trill header.
	#         - check packet is replicated to all flooding MC members 
 
    send:
             --------------------------------------------------------------
        eth: |    DA    |     SA         | VLAN         |         ET      |
             --------------------------------------------------------------
             |  mac_rb1 | remote_mac_rb4 |  vid_3_desig | trill_ethertype |
             --------------------------------------------------------------
 
                    -----------------------------------------------------------------
             trill: | multi | hop count |   eRB nickname       | iRB nickname        |
                    ------------------------------------------------------------------
                    |   0   | hopcount  | virtual_nickname     | remote_nickname_rb4 |
                    ------------------------------------------------------------------
 
                         -----------------------------------------
                    eth: |    DA            |     SA      | VLAN |
                         -----------------------------------------
                         | register_mc_mac  | host49_mac  |  v1  |
                         -----------------------------------------
     Recv 2 packets:
              ----------------------------------------- 
         eth: |    DA            |     SA      | VLAN |   (port1)
              ----------------------------------------- 
              | host_unknown_mac | host49_mac  |  v1  | 
              ----------------------------------------- 
 
              ----------------------------------------- 
         eth: |    DA            |     SA      | VLAN |   (port2)
              ----------------------------------------- 
              | host_unknown_mac | host49_mac  |  v1  | 
              -----------------------------------------  
 
 
 
  Traffic RB1 <-> RB4:
 
	#Ingress trill uc: using RB1. S2.host29 to S4.host41
	#purpose: - check trill is still working: use RB1 ingress nickname 
	#         - use MACT static entry: host41_mac
	#         - Learn how to reach host29: use port2
 
    Send:
             ----------------------------------
        eth: |    DA      |     SA     | VLAN |
             ----------------------------------
             | host41_mac | host29_mac |  v1  |
             ----------------------------------
    Recv:
             ------------------------------------------------------
        eth: |    DA          |     SA  | VLAN  |     ET          |
             ------------------------------------------------------
             | remote_mac_rb3 | mac_rb1 |  dv3  | trill_ethertype |
             ------------------------------------------------------
 
                    -----------------------------------------------------------------
             trill: | multi | hop count |        eRB nickname  | iRB nickname       | 
                    -----------------------------------------------------------------
                    |   0   | hopcount  |  remote_nickname_rb4 | local_nickname_rb1 |
                    --------------------------------------------------------------
 
                         ----------------------------------   
                    eth: |    DA      |     SA     | VLAN | 
                         ----------------------------------   
                         | host41_mac | host29_mac |  v1  | 
                         ---------------------------------- 
 
 
	#Egress trill uc: using RB1. S4.host41 to S2.host29
	#purpose: - check trill is still working: terminate with RB1 nickname
	#          - use MACT dynamic entry: "host29 learning"
 
    send:
             ------------------------------------------------------------
        eth: |    DA   |     SA         | VLAN        |         ET      |
             ------------------------------------------------------------
             | mac_rb1 | remote_mac_rb4 | vid_3_desig | trill_ethertype | 
             ------------------------------------------------------------
 
                    ----------------------------------------------------------------
             trill: | multi | hop count |   eRB nickname     | iRB nickname        |
                    ---------------------------------------------------------------
                    |   0   | hopcount  | local_nickname_rb1 | remote_nickname_rb4 |
                    ----------------------------------------------------------------
 
                         ----------------------------------
                    eth: |    DA     |     SA      | VLAN |
                         ----------------------------------
                         | host29_mac | host41_mac |  v1  |
                         ----------------------------------
 
     Recv:
              ---------------------------------- 
         eth: |    DA     |     SA      | VLAN | 
              ---------------------------------- 
              | host29_mac | host41_mac |  v1  | 
              ---------------------------------- 
 
 
	#Egress trill uc: using RB1. S4.host48 to S2.host20
	#purpose: - check trill is still working: terminate with RB1 nickname
	#         - use MACT static entry: host20_mac
	#         - Learn how to reach host48: use trill tunnel: local_nickname_rb1 -> remote_nickname_rb4
 
    send:
             ------------------------------------------------------------
        eth: |    DA   |     SA         | VLAN        |         ET      |
             ------------------------------------------------------------
             | mac_rb1 | remote_mac_rb4 | vid_3_desig | trill_ethertype | 
             ------------------------------------------------------------
 
                    ----------------------------------------------------------------
             trill: | multi | hop count |   eRB nickname     | iRB nickname        |
                    ---------------------------------------------------------------
                    |   0   | hopcount  | local_nickname_rb1 | remote_nickname_rb4 |
                    ----------------------------------------------------------------
 
                         ----------------------------------
                    eth: |    DA     |     SA      | VLAN |
                         ----------------------------------
                         | host20_mac | host48_mac |  v1  |
                         ----------------------------------
 
     Recv:
              ---------------------------------- 
         eth: |    DA     |     SA      | VLAN | 
              ---------------------------------- 
              | host20_mac | host48_mac |  v1  | 
              ----------------------------------  
 
 
	#Ingress trill uc: using RB1. S2.host20 to S4.host48
	#purpose: - check trill is still working: use RB1 ingress nickname to build trill tunnel 
	#         - use MACT dynamic entry: "host48 learning"
 
    Send:
             ----------------------------------
        eth: |    DA      |     SA     | VLAN |
             ----------------------------------
             | host48_mac | host20_mac |  v1  |
             ----------------------------------
    Recv:
             ------------------------------------------------------
        eth: |    DA          |     SA  | VLAN  |     ET          |
             ------------------------------------------------------
             | remote_mac_rb3 | mac_rb1 |  dv3  | trill_ethertype |
             ------------------------------------------------------
 
                    -----------------------------------------------------------------
             trill: | multi | hop count |        eRB nickname  | iRB nickname       | 
                    -----------------------------------------------------------------
                    |   0   | hopcount  |  remote_nickname_rb4 | local_nickname_rb1 |
                    --------------------------------------------------------------
 
                         ----------------------------------   
                    eth: |    DA      |     SA     | VLAN | 
                         ----------------------------------   
                         | host48_mac | host20_mac |  v1  | 
                         ----------------------------------  
 
 
	#Ingress trill MC (flooding):  using RB1. S2.host20 to unknown host
	#purpose: - check trill is still working: use RB1 ingress nickname to build trill tunnel 
    #         - use distribution tree nickname as egress nickname for trill tunnel
 
    Send:
             ----------------------------------------
        eth: |    DA            |     SA     | VLAN |
             ----------------------------------------
             | host_unknown_mac | host20_mac |  v1  |
             ----------------------------------------
 
    Recv 2 packets:
             ------------------------------------------------------
        eth: |    DA          |     SA  | VLAN  |     ET          |
             ------------------------------------------------------
             | remote_mac_rb3 | mac_rb1 |  dv3  | trill_ethertype |
             ------------------------------------------------------
 
                    -----------------------------------------------------------------
             trill: | multi | hop count |        eRB nickname  | iRB nickname       | 
                    -----------------------------------------------------------------
                    |   1   | hopcount  |  dist_tree_nickname  | local_nickname_rb1 |
                    -----------------------------------------------------------------
 
                         ----------------------------------------   
                    eth: |    DA            |     SA     | VLAN | 
                         ----------------------------------------   
                         | host_unknown_mac | host20_mac |  v1  | 
                         ----------------------------------------   
 
            ---------------------------------------- 
       eth: |    DA            |     SA     | VLAN |  
            ----------------------------------------  
            | host_unknown_mac | host20_mac |  v1  |  
            ----------------------------------------
 
 
	#Egress trill UC (L2 flooding): using RB1. S4.host40 to unknown host
    #purpose: - check RBv nickname is used to terminate trill header.
   	#         - check we are sending to all RB1 members 
 
    send:
             ------------------------------------------------------------
        eth: |    DA   |     SA         | VLAN        |         ET      |
             ------------------------------------------------------------
             | mac_rb1 | remote_mac_rb4 | vid_3_desig | trill_ethertype | 
             ------------------------------------------------------------
 
                    ----------------------------------------------------------------
             trill: | multi | hop count |   eRB nickname     | iRB nickname        |
                    ---------------------------------------------------------------
                    |   0   | hopcount  | local_nickname_rb1 | remote_nickname_rb4 |
                    ----------------------------------------------------------------
 
                         ----------------------------------------
                    eth: |    DA            |     SA     | VLAN |
                         ----------------------------------------
                         | host_unknown_mac | host40_mac |  v1  |
                         ----------------------------------------
 
     Recv:
              ----------------------------------------         
         eth: |    DA            |     SA     | VLAN |         
              ----------------------------------------         
              | host_unknown_mac | host40_mac |  v1  |         
              ----------------------------------------
 
 
 
 

 
 
 
 
   
  
 
    - bridging : host2 to host1
      purpose: - check bridging is still working
               - use MACT dynamic entry: "host1 learning"
 
     Send:
             --------------------------------
        eth: |    DA     |     SA    | VLAN |
             --------------------------------
             | host1_mac | host2_mac |  v1  |
             --------------------------------
    Recv:
             --------------------------------
        eth: |    DA     |     SA    | VLAN |
             --------------------------------
             | host1_mac | host2_mac |  v1  |
             --------------------------------
 
    - bridging : host1 to host2
      purpose: - check bridging is still working
               - use MACT static entry
 
     Send:
             --------------------------------
        eth: |    DA     |     SA    | VLAN |
             --------------------------------
             | host2_mac | host1_mac |  v1  |
             --------------------------------
    Recv:
             --------------------------------
        eth: |    DA     |     SA    | VLAN |
             --------------------------------
             | host2_mac | host1_mac |  v1  |
             --------------------------------
 

    
    
 
    - Ingress scenario 2: MC Flooding using RBv TO FIX.....
 
    Send:
             -------------------------------------
        eth: |    DA          |     SA    | VLAN |
             -------------------------------------
             | c_unknown_dmac | c_s1_smac |  v1  |
             -------------------------------------

    Recv 2 packets:
 
    Recv1 (bridging):
             -------------------------------------
        eth: |    DA          |     SA    | VLAN |
             -------------------------------------
             | c_unknown_dmac | c_s1_smac |  v1  |
             -------------------------------------
 
    Recv2 (trill):
             --------------------------------------------------
        eth: |    DA     |     SA   | VLAN  |         ET      |
             --------------------------------------------------
             | rb3_dmac  | rb1_smac |  dv3  | trill_ethertype |
             --------------------------------------------------
 
                    ---------------------------------------------------------------
             trill: | multi | hop count | eRB nickname  | iRB nickname            |
                    ---------------------------------------------------------------
                    |   1   | hopcount  |  remote_nickname_rb4 | virtual_nickname |
                    ---------------------------------------------------------------
 
                         -------------------------------------
                    eth: |    DA          |     SA    | VLAN |
                         -------------------------------------
                         | c_unknown_dmac | c_s1_smac |  v1  |
                         -------------------------------------

 
     Egress scenario 2: UC using RBv: unknown native DA TO FIX*************************
 
    send:
             ----------------------------------------------------
        eth: |    DA      |     SA    | VLAN  |         ET      |
             ----------------------------------------------------
             |  rb1_smac  |  rbx_dmac |  dv3  | trill_ethertype |
             ----------------------------------------------------
 
                    -----------------------------------------------------------
             trill: | multi | hop count |   eRB nickname   | iRB nickname     |
                    -----------------------------------------------------------
                    |   0   | hopcount  | virtual_nickname |   nickname_rbx   |
                    -----------------------------------------------------------
 
                         -------------------------------------
                    eth: |    DA          |     SA    | VLAN |
                         -------------------------------------
                         | c_unknown_dmac | c_sx_dmac |  v1  |
                         --------------------------------------
 
    Recv: Trapped packet
 
 
    
    #Learning 
    #1st packet Egress trill UC : from RBv(S2.host1a) to S3.host30
    #2nd packet Flooding UC: from S3.host30 to host1a
    #purpose: - to check case when we receive packet from RBv - learning should be disable
 
    #1st packet
    send(port2):
             ------------------------------------------------------------
        eth: |    DA   |     SA         | VLAN        |         ET      |
             ------------------------------------------------------------
             | mac_rb1 | remote_mac_rb2 | vid_2_desig | trill_ethertype | 
             ------------------------------------------------------------
 
                    ----------------------------------------------------------------
             trill: | multi | hop count |   eRB nickname     | iRB nickname        |
                    ---------------------------------------------------------------
                    |   0   | hopcount  | local_nickname_rb1 | virtual_nickname    |
                    ----------------------------------------------------------------
 
                         ----------------------------------
                    eth: |    DA     |     SA      | VLAN |
                         ----------------------------------
                         | host30_mac | host1a_mac |  v1  |
                         ----------------------------------
 
     Recv(port4):
              ---------------------------------- 
         eth: |    DA     |     SA      | VLAN | 
              ---------------------------------- 
              | host30_mac | host1a_mac |  v1  | 
              ----------------------------------
 
     2nd packet
     send(port4):
              ---------------------------------- 
         eth: |    DA     |     SA      | VLAN | 
              ---------------------------------- 
              | host1a_mac | host30_mac |  v1  | 
              ----------------------------------
 
     Recv#1(port3):
             --------------------------------------------------------
        eth: |       DA       |  SA | VLAN        |         ET      |
             --------------------------------------------------------
             | remote_mac_rb4 | rb1 | vid_3_desig | trill_ethertype | 
             ------------------------------ -------------------------
 
                    ----------------------------------------------------------------
             trill: | multi | hop count |   eRB nickname     | iRB nickname        |
                    ---------------------------------------------------------------
                    |   0   | hopcount  | dist_tree_nickname | local_nickname_rb1  |
                    ----------------------------------------------------------------
 
                         ----------------------------------
                    eth: |    DA     |     SA      | VLAN |
                         ----------------------------------
                         | host30_mac | host1a_mac |  v1  |
                         ----------------------------------
 
     Recv#2(port2):
             ----------------------------------
        eth: |    DA     |     SA      | VLAN |
             ----------------------------------
             | host30_mac | host1a_mac |  v1  |
             ----------------------------------
     Recv#3(port1):
             ----------------------------------
        eth: |    DA     |     SA      | VLAN |
             ----------------------------------
             | host30_mac | host1a_mac |  v1  |
             ----------------------------------

 
How to run: 
 
BCM> cint ../../../../src/examples/dpp/cint_pmf_trill_2pass_snoop.c 
BCM> cint ../../../../src/examples/dpp/cint_pmf_2pass_snoop.c 
BCM> cint ../../../../src/examples/dpp/cint_trill.c 
BCM> cint ../../../../src/examples/dpp/cint_trill_multi_homing
BCM> trill_multi_homing(unit); 
 
 
*/

/* **********************************************************************************
        global variables:
        prefix with "trill_mh_" to differentiate from global variables in cint_trill.c
 */
 
/* RB mac addresses */
bcm_mac_t  trill_mh_mac_rb1        = {0x00, 0x00, 0x00, 0x00, 0x11, 0x11};
bcm_mac_t  trill_mh_remote_mac_rb2 = {0x00, 0x00, 0x00, 0x00, 0x22, 0x22};
bcm_mac_t  trill_mh_remote_mac_rb3 = {0x00, 0x00, 0x00, 0x00, 0x33, 0x33};
bcm_mac_t  trill_mh_remote_mac_rb4 = {0x00, 0x00, 0x00, 0x00, 0x44, 0x44};

/* host mac addresses */
/* on switch 1 */
bcm_mac_t  trill_mh_host10_mac      = {0x00, 0x00, 0x00, 0x00, 0x00, 0x10};
bcm_mac_t  trill_mh_host11_mac      = {0x00, 0x00, 0x00, 0x00, 0x00, 0x11};
/* on switch 2 */
bcm_mac_t  trill_mh_host20_mac      = {0x00, 0x00, 0x00, 0x00, 0x00, 0x20};
bcm_mac_t  trill_mh_host21_mac      = {0x00, 0x00, 0x00, 0x00, 0x00, 0x21};
/*on switch 3 */
bcm_mac_t  trill_mh_host30_mac      = {0x00, 0x00, 0x00, 0x00, 0x00, 0x30};

/*pm switch 4 */
bcm_mac_t  trill_mh_host40_mac      = {0x00, 0x00, 0x00, 0x00, 0x00, 0x40};
bcm_mac_t  trill_mh_host41_mac      = {0x00, 0x00, 0x00, 0x00, 0x00, 0x41};

/* mac address to register MC group */
bcm_mac_t  trill_mh_register_mc_group_mac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x09};


/* vlans */
/* vlan for bridging between s1 and s2 */
bcm_vlan_t  trill_mh_vid_1       = 100;
/* designated vlan for link: RB1 - RB2 */
bcm_vlan_t  trill_mh_vid_2_desig = 200;
/* designated vlan for link: RB1 - RB4*/
bcm_vlan_t  trill_mh_vid_3_desig = 300;

/* customer vlan for transit.  */
bcm_vlan_t  trill_mh_vid_for_transit  = 400;

/* trill port */
/* for RBV */
bcm_trill_port_t trill_mh_trill_port_rbv;
bcm_trill_port_t trill_mh_trill_port_remote_rbv;
/* remote rb */
bcm_trill_port_t trill_mh_trill_port_rb2;
bcm_trill_port_t trill_mh_trill_port_rb4;

/* ports */

bcm_port_t     trill_mh_port_1 = 13;
bcm_port_t     trill_mh_port_2 = 14;
bcm_port_t     trill_mh_port_3 = 15;
bcm_port_t     trill_mh_port_4 = 16;
bcm_pbmp_t     pbm; /* ports bitmap: ports in use in our application */

/* trap to this port */
bcm_port_t     trill_mh_port_trap_cpu = 0;

/* recycle port: for 2 pass in case of trill multicast */
bcm_port_t     trill_mh_recycle_port[2] = {40, 41};

/* recycle ports config */
bcm_gport_t    trill_mh_snoop_trap_gport_id_2;
bcm_gport_t    trill_mh_snoop_trap_gport_id_3[2];

bcm_field_group_t trill_mh_snoop_group_2;
bcm_field_group_t trill_mh_snoop_group_3;


/* nicknames */
int     trill_mh_virtual_nickname = 0x1122;
int     trill_mh_local_nickname_rb1   = 0x1234;
int     trill_mh_remote_nickname_rb2  = 0xAA22;
int     trill_mh_remote_nickname_rb4  = 0xAA44;

int dist_tree_nickname_index = 0;
/* distribution tree nicknames for flooding: up to 4 (limitation: configure distribution tree nicknames in 2 lsbs from vsi profiles) */
int DISTRIBUTION_TREE_NICKNAMES_SIZE= 4;
int dist_tree_nicknames[DISTRIBUTION_TREE_NICKNAMES_SIZE] = {0xCCCC, 0xCCDD, 0xCCEE, 0xCCFF};

/* distribution tree nickname for registered multicast */
int dist_tree_nickname_registered = 0xDDDD;

/* multicast group */
bcm_multicast_t     trill_mh_mc_group_flooding = 6300; /* used as flooding group and registered multicast group */
bcm_multicast_t     trill_mh_mc_group_transit = 6301; /* contains only trill ports. "RB2 trill port" and "RB4 trill port" */
bcm_multicast_t     trill_mh_mc_group_rb1_members = 6302; /* contains only members of RB1: p2, p3 */
bcm_multicast_t     trill_mh_mc_group_rbv_members = 6303; /* contains only members of RBv: p1 & p3 */
bcm_multicast_t     trill_mh_mc_group_registered = 6304; /* registered multicast: contains p1, p2, "RB2 trill port" */

/* pmf group */
bcm_field_group_t rpf_group = 100;
int qual_id_ing_nickname;
int qual_id_eg_nickname;




/* create and configure trap: send packet to cpu. */
int set_trap_to_cpu(int unit, bcm_port_t cpu_port, bcm_gport_t *trap_gport) {
    int rv = BCM_E_NONE;
    bcm_rx_trap_config_t config;
    int flags = 0;
    int trap_id;

    bcm_rx_trap_config_t_init(&config);
             
    config.flags = (BCM_RX_TRAP_UPDATE_DEST); 
    config.trap_strength = 0;
    config.dest_port=cpu_port; 

    rv = bcm_rx_trap_type_create(unit,flags,bcmRxTrapUserDefine,&trap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in trap create, trap %d \n", trap_id);
        return rv;
    }

    rv = bcm_rx_trap_set(unit,trap_id,&config);
    if (rv != BCM_E_NONE) {
        printf("Error, in trap set \n");
        return rv;
    }

    /* encode trap as gport */   
    BCM_GPORT_TRAP_SET(*trap_gport, trap_id, 7, 0);

    return rv;
}


/* configure ports with default port profile.
 * port profile is used by PMF to find ingress nickname for trill packets.
 * parameters: 
      pbm: contains all ports in use */
int port_profile_default_config(int unit, bcm_pbmp_t pbm) {
    int rv;
    int i;
    for (i=0; i<256; i++) {
        if (BCM_PBMP_MEMBER(pbm, i)) {
            rv = trill_virtual_rbrdige_ing_nickname_index_per_port_set(unit, i, 0);
            BCM_IF_ERROR_RETURN(rv);
        }
    }
}

int _BCM_GPORT_TRILL_VIRTUAL_NICK_NAME_INDEX_MASK = 0x3;
int _BCM_GPORT_TRILL_VIRTUAL_NICK_NAME_INDEX_SHIFT = 16;
int _BCM_GPORT_TRILL_NICK_NAME_MASK                = 0xFFFF;

int _BCM_GPORT_TRILL_VIRTUAL_NICK_NAME_INDEX_GET(int gport_trill_port_id) {
    return (((gport_trill_port_id) >> _BCM_GPORT_TRILL_VIRTUAL_NICK_NAME_INDEX_SHIFT) & _BCM_GPORT_TRILL_VIRTUAL_NICK_NAME_INDEX_MASK);
}

int add_virtual_rbridge_member(int unit, bcm_trill_port_t* trill_port_rbv, uint32 port) {

    int rv = 0;

    /* extract virtual nickname index from trill gport */
    int virtual_nickname_index = _BCM_GPORT_TRILL_VIRTUAL_NICK_NAME_INDEX_GET(trill_port_rbv->trill_port_id);

    /* add  virtual nickname index to port profile */
    rv = trill_virtual_rbrdige_ing_nickname_index_per_port_set(unit, port, virtual_nickname_index);
    if (rv != BCM_E_NONE) {
       printf("Error, in bcm_trill_port_add \n");
       return rv;
    }

    /* setup pmf program to send trill packet using virtual nickname instead of my nickname */
    rv = trill_virtual_rbrdige_ing_nickname_set(unit, 1, 1, pbm);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_trill_port_add \n");
        return rv;
    }

    printf("add virtual rbridge member for trill port_id %x\n", trill_port_rbv->trill_port_id); 

    return BCM_E_NONE;
}

/* 
 * setup pmf program to use 2 bits saved in vsi profile to lookup distribution tree nickname 
 * port: used by PMF to get virtual nickname
 * vsi: to setup vsi profile
 * dist_tree_nickname: used by PMF, result of lookup on dist_tree_nickname_index
 * dist_tree_nickname_index: used by PMF to lookup and get distribution tree nickname */
int add_dist_tree_nickname_flooding(int unit, bcm_vlan_t vsi, int dist_tree_nickname, int dist_tree_nickname_index) {

    int rv = 0;
    int i;
    int group_priority_lookup = 2;
    bcm_field_group_t group_lookup = 2;
    int group_priority_eei = 3;
    bcm_field_group_t group_eei = 3;

    /* save distri tree nickname index in vsi profile */
    bcm_vlan_control_vlan_t vlan_control_vlan;
    bcm_vlan_control_vlan_t_init(&vlan_control_vlan);
    vlan_control_vlan.if_class= dist_tree_nickname_index;
    bcm_vlan_control_vlan_set(unit, vsi, vlan_control_vlan);

    /* setup pmf program in case of flooding: 
     * - send trill packet using virtual nickname instead of my nickname 
     * - send to distribution tree nickname */
    rv = trill_virtual_rbrdige_ing_nickname_flooding_set(unit, group_priority_lookup, group_lookup, group_priority_eei, group_eei, pbm);
    if (rv != BCM_E_NONE) {
        printf("Error, in trill_virtual_rbrdige_ing_nickname_flooding_set \n");
        return rv;
    }

    /* save dist_tree_nickname at index: distri nickname_index for PMF usage */
    rv = trill_virtual_rbrdige_ing_nickname_vsi_profile_set(unit, group_lookup, dist_tree_nickname_index, dist_tree_nickname);
    if (rv != BCM_E_NONE) {
        printf("Error, in trill_virtual_rbrdige_ing_nickname_vsi_profile_set \n");
        return rv;
    }
    return BCM_E_NONE;
}

int global_config_multicast(int unit) {

    int rv = 0;


    /*initialize Multicast RPF: enable RPF Trap */
    rv = rpf_drop_set(unit,10, rpf_group, &qual_id_ing_nickname, &qual_id_eg_nickname);

    /* Global setting for mluticast trill */
    rv = mc_global(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in mc_global \n");
        return rv;
    }
    return BCM_E_NONE;
}



/* add trill entries to multicast group: port + trill tunnel */
int add_trill_port_to_mc(int unit, 
                      int mc_group, 
                      bcm_trill_port_t trill_port, 
                      bcm_port_t port) {
    int rv;
    bcm_if_t trill_encap;
    /* get trill tunnel (or encap or eep)*/
    rv = bcm_multicast_trill_encap_get(unit, -1, trill_port.trill_port_id ,0, &trill_encap); 
    BCM_IF_ERROR_RETURN(rv);
    /* add <port, encap> to MC group */ 
    rv = bcm_multicast_egress_add(unit, mc_group, port, trill_encap);
    BCM_IF_ERROR_RETURN(rv);
    printf ("Added port %d to group. (Trill copy)\n", port);

    return rv;
}


int add_bridge_port_to_mc(int unit, 
                           int mc_group, 
                           bcm_port_t port) {
    int rv;
    /* get default "no encap" value for briding packets */
    bcm_if_t host_encap = get_encap_id_to_bridge(unit);
     /* add port to MC group */
    rv = bcm_multicast_egress_add(unit, mc_group, port, host_encap);
    BCM_IF_ERROR_RETURN(rv);
    printf ("Added port %d to group. (ETH copy)\n", port);

    return rv;
}


/* 
 * create a multicast group
 * for bridging: add ports: port1, port2
 * for trill:    add <port,trill tunnel>:  port2 + encap, port3 + encap
 */   
int
create_mc_group(int unit, int group) {
    int rv;

    /* create new multicast group */
    rv = bcm_multicast_create(unit, 
                              BCM_MULTICAST_TYPE_L2 | BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_WITH_ID, 
                              &group);
    BCM_IF_ERROR_RETURN(rv);
    printf ("Created MC group = %d .\n", group);

   return rv;
}


/* create a flooding multicast group
 * Add trill ports and bridge ports
  */
int create_flooding_group(int unit, int vsi, int mc_group) {
    int rv;

    /* create flooding MC group
     * add bridge port1,
     *     bridge port2,
     *     trill port rb4 */
    rv = create_mc_group(unit, mc_group);
    BCM_IF_ERROR_RETURN(rv);

    /* add trill port rb4 to flooding MC group */
    rv = add_trill_port_to_mc(unit, mc_group, trill_mh_trill_port_rb4, trill_mh_port_3);
    BCM_IF_ERROR_RETURN(rv);

    /* add port1 to flooding MC group */
    add_bridge_port_to_mc(unit, mc_group, trill_mh_port_1);
    BCM_IF_ERROR_RETURN(rv);

    /* add port2 to flooding MC group  */
    add_bridge_port_to_mc(unit, mc_group, trill_mh_port_2);
    BCM_IF_ERROR_RETURN(rv);

    /* add MC group (flooding group) to vsi */
    bcm_vlan_control_vlan_t vsi_control;
    bcm_vlan_control_vlan_t_init(&vsi_control);
    vsi_control.forwarding_vlan = vsi;
    vsi_control.unknown_unicast_group   = mc_group;
    vsi_control.unknown_multicast_group = mc_group;
    vsi_control.broadcast_group         = mc_group;
    
    rv = bcm_vlan_control_vlan_set(unit,vsi,vsi_control);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_control_vlan_set\n");
        return rv;
    }
    printf ("Set flooding to vsi %d .\n", vsi);

   return rv;

}

/* create a transit multicast group
 * Bind multicast and dist tree nickname
 * parameters:
 * vsi: native vlan of transit packet.
 * trill_port_mc: trill port multicast. 
 */
 
int create_transit_group(int unit, 
                          int vsi, 
                          int mc_group_transit, 
                          int dist_tree_nickname, 
                          bcm_trill_port_t* trill_port_mc) {

    int rv;
    bcm_trill_multicast_entry_t trill_mc_entry;

    /* create transit MC group. Contains only trill ports 
     * add trill port rb2,
           trill port rb4 */
    rv = create_mc_group(unit, mc_group_transit);
    BCM_IF_ERROR_RETURN(rv);

    /* add trill entry to transit MC group */
    rv = add_trill_port_to_mc(unit, mc_group_transit, trill_mh_trill_port_rb2, trill_mh_port_2);
    BCM_IF_ERROR_RETURN(rv);

    /* add trill entry to transit MC group */
    rv = add_trill_port_to_mc(unit, mc_group_transit, trill_mh_trill_port_rb4, trill_mh_port_3);
    BCM_IF_ERROR_RETURN(rv);

    /* create "multicast trill port".  */
    rv = add_root_rbridge(unit, dist_tree_nickname,  trill_port_mc, -1); 
    BCM_IF_ERROR_RETURN(rv);
    printf ("Created mc trill port for transit id=0x%x nick=0x%x \n", trill_port_mc->trill_port_id, trill_port_mc->name);

    /* bind MC group to distribution tree 
     * add MC group in HW 
     */
    bcm_trill_multicast_entry_t_init(&trill_mc_entry);
    trill_mc_entry.root_name = dist_tree_nickname;
    trill_mc_entry.group = mc_group_transit;
    trill_mc_entry.c_vlan = vsi;

    rv = bcm_trill_multicast_entry_add(unit, &trill_mc_entry);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_trill_multicast_entry_add\n");
        return rv;
    }
    printf ("Created trill_multicast_entry\n"); 

    return rv;
}

int create_mc_group_registered(int unit, 
                          int vsi, 
                          int mc_group, 
                          int dist_tree_nickname, 
                          bcm_trill_port_t* trill_port_mc) {

    int rv;
    bcm_trill_multicast_entry_t trill_mc_entry;

    rv = create_mc_group(unit, mc_group);
    BCM_IF_ERROR_RETURN(rv);

    /* add trill entry to mc group */
    rv = add_trill_port_to_mc(unit, mc_group, trill_mh_trill_port_rb4, trill_mh_port_3);
    BCM_IF_ERROR_RETURN(rv);

    /* add port1 to mc group */
    rv = add_bridge_port_to_mc(unit, mc_group, trill_mh_port_1);
    BCM_IF_ERROR_RETURN(rv);


    /* add port2 to mc group */
    rv = add_bridge_port_to_mc(unit, mc_group, trill_mh_port_2);
    BCM_IF_ERROR_RETURN(rv);


    /* create "multicast trill port".  */
    rv = add_root_rbridge(unit, dist_tree_nickname,  trill_port_mc, -1); 
    BCM_IF_ERROR_RETURN(rv);
    printf ("Created mc trill port for registered multicast id=0x%x nick=0x%x \n", trill_port_mc->trill_port_id, trill_port_mc->name);

    /* bind MC group to distribution tree 
     * add MC group in HW 
     */
    bcm_trill_multicast_entry_t_init(&trill_mc_entry);
    trill_mc_entry.root_name = dist_tree_nickname;
    trill_mc_entry.group = mc_group;
    trill_mc_entry.c_vlan = vsi;
    trill_mc_entry.flags = BCM_TRILL_MULTICAST_ACCESS_TO_NETWORK;

    rv = bcm_trill_multicast_entry_add(unit, &trill_mc_entry);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_trill_multicast_entry_add\n");
        return rv;
    }

    return rv;
}


/* 
 * add virtual nickname.
 * return gport: trill port. 
   */
int 
config_virtual_rbridge(int unit, int nickname_rb_virtual, bcm_trill_port_t *trill_port_virtual) {

    int rv = 0;

    bcm_trill_port_t_init(trill_port_virtual);

    trill_port_virtual->name = nickname_rb_virtual;
    trill_port_virtual->flags = BCM_TRILL_PORT_VIRTUAL;

    rv = bcm_trill_port_add(unit, trill_port_virtual);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_trill_port_add \n");
        return rv;
    }

    printf("virtual rbridge configured, trill port: 0x%x for my-virtual-nickname 0x%x \n", trill_port_virtual->trill_port_id, nickname_rb_virtual);

    return rv;
}
  


/*
 * replace mact entry: Replace p1 for host10_mac with trill tunnel RB2.
 */
int simulate_link_failure(int unit) {
    int rv;

    bcm_l2_addr_t match_addr, match_addr_mask;
    bcm_l2_addr_t replace_addr, replace_addr_mask;

    int flags = BCM_L2_REPLACE_MATCH_STATIC |BCM_L2_REPLACE_MATCH_MAC | BCM_L2_REPLACE_MATCH_VLAN | BCM_L2_REPLACE_MIGRATE_PORT | BCM_L2_REPLACE_IGNORE_DES_HIT;

    /* build existing l2 addr and its mask */
    bcm_l2_addr_t_init(&match_addr, trill_mh_host10_mac, trill_mh_vid_1);
    match_addr.port = trill_mh_port_1;

    match_addr.flags |= BCM_L2_STATIC;


    rv = bcm_l2_replace(unit, flags, &match_addr, 0,trill_mh_trill_port_rb2.trill_port_id,0);
    if (rv != BCM_E_NONE) {
         printf("Error, bcm_l2_replace_match\n");
         return rv;
    }
    return rv;
}

int trill_multi_homing(int unit) {

    int rv = 0;

    bcm_l2_addr_t l2_addr;

    bcm_if_t l3_intf_2_id = 0; 
    bcm_if_t l3_intf_3_id = 0; 

    bcm_trill_port_t trill_port_mc;
    bcm_trill_port_t trill_port_mc_registered;

    bcm_if_t encap_id;

    bcm_field_group_t rpf_group = 100;

    int dist_tree_nickname; 
    int recycle_port_num = 1;

    /* Initialize trill module (SW related database) */
    rv = bcm_trill_init(unit);

    /* global config:
     * - Hop count
     * - EtherType */
    rv = global_config(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in global_config\n");
        return rv;
    }

    /* multicast global config */
    rv = global_config_multicast(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in global_config_multicast \n");
        return rv;
    }

    /* initialize port bitmap: port in use in our application */
    BCM_PBMP_CLEAR(pbm);
    BCM_PBMP_PORT_ADD(pbm, trill_mh_port_1);
    BCM_PBMP_PORT_ADD(pbm, trill_mh_port_2);
    BCM_PBMP_PORT_ADD(pbm, trill_mh_port_3);
    BCM_PBMP_PORT_ADD(pbm, trill_mh_port_4);

    /* default configuration for ports, used by PMF to edit ingress nickname  */
    port_profile_default_config(unit, pbm);


    /* port config:
       - tpid 
       - Acceptable frame type
       - Port -> Vlan domain
       - Designated Vlan */

    /* 3rd param: designated vlan: shouldn't be any designated vlan here... should only set vlan domain.
       Designated vlan has no meaning for port 1 and port 4. */
    rv = port_config(unit, trill_mh_port_1, trill_mh_vid_1);
    if (rv != BCM_E_NONE) {
       printf("Error, in global_config\n");
       return rv;
    }

    rv = port_config(unit, trill_mh_port_4, trill_mh_vid_1);
    if (rv != BCM_E_NONE) {
       printf("Error, in global_config\n");
       return rv;
    }

    rv = port_config(unit, trill_mh_port_2, trill_mh_vid_2_desig);
    if (rv != BCM_E_NONE) {
        printf("Error, in global_config\n");
        return rv;
    }

    rv = port_config(unit, trill_mh_port_3, trill_mh_vid_3_desig);
    if (rv != BCM_E_NONE) {
        printf("Error, in global_config\n");
        return rv;
    }


    /* Create vlan:
       - open VSI (VSI == VID)
       - Add ports
       - Map <VD,vid> -> lif -> VSI */    

    /* vlan for bridging */
    rv = vlan_init(unit, trill_mh_vid_1);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_init\n");
        return rv;
    }

    /* add vlan ports to bridge */
    rv = vlan_port_init(unit, trill_mh_vid_1, trill_mh_port_1);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_port_init\n");
        return rv;
    }

    rv = vlan_port_init(unit, trill_mh_vid_1, trill_mh_port_2);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_port_init\n");
        return rv;
    }

    rv = vlan_port_init(unit, trill_mh_vid_1, trill_mh_port_4);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_port_init\n");
        return rv;
    }

    /* vlan for trill */
    rv = vlan_init(unit, trill_mh_vid_2_desig);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_init\n");
        return rv;
    }

    /* add vlan port to trill */
    rv = vlan_port_init(unit, trill_mh_vid_2_desig, trill_mh_port_2);

    /* vlan for trill */
    rv = vlan_init(unit, trill_mh_vid_3_desig);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_init\n");
        return rv;
    }

    /* add vlan port to trill  */
    rv = vlan_port_init(unit, trill_mh_vid_3_desig, trill_mh_port_3);


    /* Bridging: Add a <VD,vid> -> lif */
    /* No need to add port_2, since it's on the same vlan domain as port_1 */
    rv = vlan_port_set(unit, trill_mh_port_1, trill_mh_vid_1, trill_mh_vid_1);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_port_set: vid=%d, vsi=%d\n",trill_mh_vid_1,trill_mh_vid_1);
        return rv;
    }


    /* Trill: link RB1-RB2 Add a <VD,vid> -> lif */
    rv = vlan_port_set(unit, trill_mh_port_2, trill_mh_vid_2_desig, trill_mh_vid_2_desig);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_port_set: vid=%d, vsi=%d\n",trill_mh_vid_2_desig,trill_mh_vid_2_desig);
        return rv;
    }

    /* Trill: link RB1- RB4 */
    rv = vlan_port_set(unit, trill_mh_port_3, trill_mh_vid_3_desig, trill_mh_vid_3_desig);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_port_set: vid=%d, vsi=%d\n",trill_mh_vid_3_desig,trill_mh_vid_3_desig);
        return rv;
    }


    /* create L3 interface for Trill (link RB1-RB2): configures MyMac */
    rv = l3_intf_create(unit, trill_mh_mac_rb1, trill_mh_vid_2_desig, &l3_intf_2_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in l3_intf_create: vsi=%d\n", vsi_2_desig);
        return rv;
    }

    /* create L3 interface for Trill (link RB1-RB4): configures MyMac */
    rv = l3_intf_create(unit, trill_mh_mac_rb1, trill_mh_vid_3_desig, &l3_intf_3_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in l3_intf_create: vsi=%d\n", trill_mh_vid_3_desig);
        return rv;
    }

    /* configures my nickname */
    rv = config_local_rbridge(unit, trill_mh_local_nickname_rb1, 0);
    BCM_IF_ERROR_RETURN(rv);

    /* add virtual rbridge member (configure virtual nickname) */
    rv = config_virtual_rbridge(unit, trill_mh_virtual_nickname, &trill_mh_trill_port_rbv);
    BCM_IF_ERROR_RETURN(rv);

    /* add virtual rbridge port member (send to trill campus with ingress nickname=virtual nickname) */
    rv = add_virtual_rbridge_member(unit, trill_mh_trill_port_rbv, trill_mh_port_1); 
    BCM_IF_ERROR_RETURN(rv);

    /* add remote rbridges (add trill port) */
    /* add RB2 */
    rv = add_remote_rbridge(unit, trill_mh_remote_mac_rb2, trill_mh_vid_2_desig, trill_mh_port_2, 
                            trill_mh_remote_nickname_rb2, 0, &trill_mh_trill_port_rb2);
    BCM_IF_ERROR_RETURN(rv);

    /* add RB4 */
    rv = add_remote_rbridge(unit, trill_mh_remote_mac_rb4, trill_mh_vid_3_desig, trill_mh_port_3, 
                            trill_mh_remote_nickname_rb4, 0, &trill_mh_trill_port_rb4);
    BCM_IF_ERROR_RETURN(rv);

    /* add RBv: The idea is not to send to that RB.
     * The idea is to disable learning in case we receive packets from RBv.
     * If the switch has receive packets from RBv, sent by host10.
     * it means: 1) host10 does multi homing. 
     *           2) host10 has sent its packet using a member of RBv to send to RB1. 
     *           3) when RB1 receive the packet, it will learn how to send to host10. Using tunnel to RBv.
     *           4) Since host10 does mult homing, RB1 should learn a port for host10.
     * So we define a remote RB as RBv, and we disable learning for it.
     */
     rv = add_remote_rbridge(unit, trill_mh_remote_mac_rb2, trill_mh_vid_2_desig, trill_mh_port_2,
                     trill_mh_virtual_nickname, 0, &trill_mh_trill_port_remote_rbv);      
     if (rv != BCM_E_NONE) {                                                                  
         printf("Error, in add_remote_rbridge\n");                                            
         return rv;                                                                           
     }
    
                                                                                        
     /* disable learning for remote RBv */
     /* To enable learnig use (BCM_PORT_LEARN_FWD | BCM_PORT_LEARN_ARL) flags.
        To disable learning use BCM_PORT_LEARN_FWD flag only. */

      rv = bcm_port_learn_set(unit, trill_mh_trill_port_remote_rbv.trill_port_id,  BCM_PORT_LEARN_FWD);
      if (rv != BCM_E_NONE) {                                                                          
          printf("Error, in bcm_port_learn_set\n");                                                    
          return rv;                                                                                   
      }

    /* bridging: add L2 mac entries for customer packet */

    bcm_l2_addr_t_init(&l2_addr, trill_mh_host10_mac, trill_mh_vid_1);
    l2_addr.port = trill_mh_port_1;
    rv = bcm_l2_addr_add(unit, &l2_addr);
    BCM_IF_ERROR_RETURN(rv);

    bcm_l2_addr_t_init(&l2_addr, trill_mh_host11_mac, trill_mh_vid_1);
    l2_addr.port = trill_mh_port_1;
    rv = bcm_l2_addr_add(unit, &l2_addr);
    BCM_IF_ERROR_RETURN(rv);


    bcm_l2_addr_t_init(&l2_addr, trill_mh_host20_mac, trill_mh_vid_1);
    l2_addr.port = trill_mh_port_2;
    rv = bcm_l2_addr_add(unit, &l2_addr);
    BCM_IF_ERROR_RETURN(rv);

    bcm_l2_addr_t_init(&l2_addr, trill_mh_host21_mac, trill_mh_vid_1);
    l2_addr.port = trill_mh_port_2;
    rv = bcm_l2_addr_add(unit, &l2_addr);
    BCM_IF_ERROR_RETURN(rv);

    bcm_l2_addr_t_init(&l2_addr, trill_mh_host30_mac, trill_mh_vid_1);
    l2_addr.port = trill_mh_port_4;
    rv = bcm_l2_addr_add(unit, &l2_addr);
    BCM_IF_ERROR_RETURN(rv);

    /* trill: add L2 mac entries for trill campus */
    bcm_l2_addr_t_init(&l2_addr, trill_mh_host40_mac, trill_mh_vid_1);
    l2_addr.port = trill_mh_trill_port_rb4.trill_port_id;
    rv = bcm_l2_addr_add(unit, &l2_addr);
    BCM_IF_ERROR_RETURN(rv);

    bcm_l2_addr_t_init(&l2_addr, trill_mh_host41_mac, trill_mh_vid_1);
    l2_addr.port = trill_mh_trill_port_rb4.trill_port_id;
    rv = bcm_l2_addr_add(unit, &l2_addr);
    BCM_IF_ERROR_RETURN(rv);


   /* egress_link_failure_and_egress_trill_no_da_match(unit); */
    rv = egress_link_failure_and_egress_trill_no_da_match(unit, 
                                                          trill_mh_local_nickname_rb1,
                                                          trill_mh_virtual_nickname, 
                                                          trill_mh_mc_group_rb1_members, 
                                                          trill_mh_mc_group_rbv_members, 
                                                          trill_mh_port_trap_cpu);
    BCM_IF_ERROR_RETURN(rv);


    /* MC support */
    if (is_device_or_above(unit, JERICHO)) {
        recycle_port_num = 2;
    }

    /* configure port3 to be recycle port: enable bud & Transit RBridges */
    rv = port_config_recycle_port(unit, trill_mh_recycle_port, recycle_port_num, &trill_mh_port_3, 1, trill_mh_vid_3_desig, 
                                  trill_mh_snoop_trap_gport_id_3, &trill_mh_snoop_group_3, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, in port_config_recycle_port\n");
        return rv;
    }

    dist_tree_nickname = dist_tree_nicknames[dist_tree_nickname_index];

    /* add dist tree nickname for flooding */
    rv = add_dist_tree_nickname_flooding(unit, trill_mh_vid_1, dist_tree_nickname, dist_tree_nickname_index);
    BCM_IF_ERROR_RETURN(rv);

    /* add MC group for flooding:
     * create a mc group and assign it as flooding group for vsi.
     * Contains all ports: p1, p2, p3 and trill ports: trill port RB4
     */
    rv = create_flooding_group(unit, trill_mh_vid_1, trill_mh_mc_group_flooding);
    BCM_IF_ERROR_RETURN(rv);

    /* add MC group for transit:
     * Contains only trill ports: trill port RB2, trill port RB4* 
     * - bind MC group to its distribution tree
     * - for multicast ingress trill packet: <dist tree nickname, native vlan> is resolved as multicast id
     */
    rv = create_transit_group(unit, trill_mh_vid_1, trill_mh_mc_group_transit, dist_tree_nickname, &trill_port_mc);
    BCM_IF_ERROR_RETURN(rv);

    /* add MC group for RB1 members */
    rv = create_mc_group(unit, trill_mh_mc_group_rb1_members);

    /* add trill entry to mc group */
    rv = add_trill_port_to_mc(unit, trill_mh_mc_group_rb1_members, trill_mh_trill_port_rb4, trill_mh_port_3);
    BCM_IF_ERROR_RETURN(rv);

    /* add port2 to MC group */
    add_bridge_port_to_mc(unit, trill_mh_mc_group_rb1_members, trill_mh_port_2);
    BCM_IF_ERROR_RETURN(rv);


    /* add MC group for RBv members */
    rv = create_mc_group(unit, trill_mh_mc_group_rbv_members);

    /* add trill entry to mc group */
    rv = add_trill_port_to_mc(unit, trill_mh_mc_group_rbv_members, trill_mh_trill_port_rb4, trill_mh_port_3);
    BCM_IF_ERROR_RETURN(rv);

    /* add port1 to MC group */
    add_bridge_port_to_mc(unit, trill_mh_mc_group_rbv_members, trill_mh_port_1);
    BCM_IF_ERROR_RETURN(rv);


    /* add MC group for register MC: send to RB3, port1, port2 */
    rv = create_mc_group_registered(unit, 
                                    trill_mh_vid_1, 
                                    trill_mh_mc_group_registered, 
                                    dist_tree_nickname_registered, 
                                    &trill_port_mc_registered);
    BCM_IF_ERROR_RETURN(rv);


    /*register MC group */
    bcm_l2_addr_t_init(&l2_addr, trill_mh_register_mc_group_mac, trill_mh_vid_1);
    l2_addr.l2mc_group = trill_mh_mc_group_registered;
    l2_addr.port =  trill_port_mc_registered.trill_port_id;

    rv = bcm_l2_addr_add(unit, &l2_addr);
    BCM_IF_ERROR_RETURN(rv);


    /* rpf check */


    /* add rpf check for RBv */
/*  rv = bcm_multicast_trill_encap_get(unit, trill_mh_mc_group_flooding, trill_mh_trill_port_rb4.trill_port_id, 0, &encap_id);*/
/*  BCM_IF_ERROR_RETURN(rv);                                                                                                  */
/*                                                                                                                            */
/*  rv = trill_multicast_source_add(unit, dist_tree_nickname, trill_mh_virtual_nickname, trill_mh_port_3,                 */
/*                                  encap_id, rpf_group, qual_id_ing_nickname, qual_id_eg_nickname);                          */
/*  BCM_IF_ERROR_RETURN(rv);                                                                                                  */

    /*add rpf check for RB1: flooding multicast */
/*  rv = bcm_multicast_trill_encap_get(unit, trill_mh_mc_group_flooding, trill_mh_trill_port_rb4.trill_port_id, 0, &encap_id);*/
/*                                                                                                                            */
/*  BCM_IF_ERROR_RETURN(rv);                                                                                                  */
/*                                                                                                                            */
/*  rv = trill_multicast_source_add(unit, dist_tree_nickname, trill_mh_local_nickname_rb1, trill_mh_port_3,                   */
/*                                  encap_id, rpf_group, qual_id_ing_nickname, qual_id_eg_nickname);                          */
/*  BCM_IF_ERROR_RETURN(rv);                                                                                                  */


    /* rpf check: we know how to send uc packet to "multicast packet ingress RB"
     * rpf check is a way to validate the multicast (dist_tree_nickname and its linked MC group) */

    /* rpf check for multicast traffic from RB4 to RB1 */
    /* get encap_id: tunnel to RB4 (eep) */
    rv = bcm_multicast_trill_encap_get(unit, 
                                       trill_mh_mc_group_transit, /* unused to get encap */
                                       trill_mh_trill_port_rb4.trill_port_id,  
                                       0, 
                                       &encap_id);
    BCM_IF_ERROR_RETURN(rv);
    /* for distribution tree nickname, we know how to get to nickname rb4: encap_id, port */
    rv = trill_multicast_source_add(unit, dist_tree_nickname, trill_mh_remote_nickname_rb4, trill_mh_port_3, encap_id, 
                                    /* pmf*/ rpf_group, qual_id_ing_nickname, qual_id_eg_nickname);
    BCM_IF_ERROR_RETURN(rv);


    return BCM_E_NONE;
}


int
test_trill_multi_homing(int unit, 
                        int p1, int p2, int p3,int p4, int trap_port, int recycle_port, 
                        int recycle_port_core1, int trill_ether_type) {
    trill_mh_port_1 =  p1;
    trill_mh_port_2  = p2;
    trill_mh_port_3 = p3;
    trill_mh_port_4 = p4;
    trill_mh_port_trap_cpu = trap_port;
    trill_mh_recycle_port[0] = recycle_port;
    trill_mh_recycle_port[1] = recycle_port_core1;
    /* trill_ethertype is defined and initialized in cint_trill.c */
    trill_ethertype = trill_ether_type;
    
    return trill_multi_homing(unit);
}


/* 
 * Set the In-Port profile[22:16] to be 
 * ['1010' in 22:19 to indicate EEI-Type is Trill | '0' in 18 for Reserved | virtual_nickname_index in 17:16 ] 
 */
int trill_virtual_rbrdige_ing_nickname_index_per_port_set(/* in */ int unit,
                                                           /* in */ int in_port,
                                                           /* in */ int virtual_nickname_index) {
  int rv =0;
  int result;
  uint32 value;

  result = bcm_port_class_get(unit, in_port, bcmPortClassFieldIngressPacketProcessing, &value);
  if (BCM_FAILURE(rv)) {
      printf("Error in bcm_port_class_get\n");
      return result;
  }
  value = (value & 0xFF80FFFF /* Zero 22:16 */) + (10 << 19) + (virtual_nickname_index << 16);
  result = bcm_port_class_set(unit, in_port, bcmPortClassFieldIngressPacketProcessing, value);
  if (BCM_FAILURE(rv)) {
      printf("Error in bcm_port_class_set\n");
      return result;
  }

  return result;
}



/*
 *  Create Field group such that for bridged packets coming
 *  - from in_port (virtual port member),
 *  - when the L2 destination is found,
 *  set (via index) the virtual nickname
 *  
 *  In practice, rebuild the EEI:
 *  [ In-Port-Profile[22:16] as new-EEI[23:17] | EEI[16:0] as new-EEI[16:0] ]
 */
int trill_virtual_rbrdige_ing_nickname_set(/* in */ int unit,
                                           /* in */ int group_priority,
                                           /* in */ bcm_field_group_t group,
                                           /* in */ bcm_pbmp_t pbm) {
  int result;
  int auxRes;
  int i;
  bcm_field_qset_t qset;
  bcm_field_aset_t aset;
  bcm_field_entry_t ent;
  /* In large Direct Extraction Databases, the qualifier length is maximally 16 bits */
  bcm_field_data_qualifier_t dq_trill[3]; /* 0 - EEI[15:0], 1 - EEI[16], 2 - In-PP-Port-Profile[22:16] */
  uint16 dq_offset[3] = {0, 16, 16};
  uint8 dq_length[3] = {16, 1, 7};
  bcm_field_qualify_t dq_qualifier[3] = {bcmFieldQualifyTrillEgressRbridge, 
      bcmFieldQualifyTrillEgressRbridge, bcmFieldQualifyInterfaceClassProcessingPort};
  uint32 dq_ndx, nof_dqs = 3;
  bcm_pbmp_t pbm_mask;
  int presel_id = 0;
  int presel_flags = 0;
  bcm_field_presel_set_t psset;
  bcm_field_extraction_field_t ext[3];
  bcm_field_extraction_action_t extact;
  bcm_field_group_config_t grp_config;


  /* 
   * Set the preselector to be on this port
   */
  for(i=0; i<256; i++) { BCM_PBMP_PORT_ADD(pbm_mask, i); } /* Mask on all the ports */

  /* Create a presel entity */
  if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
        presel_flags = BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
        result = bcm_field_presel_create_stage_id(unit, bcmFieldStageIngress, presel_id);
  } else {
        result = bcm_field_presel_create(unit, &presel_id);
  }
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_presel_create\n");
      return result;
  }

  result = bcm_field_qualify_Stage(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldStageIngress);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qualify_Stage\n");
      return result;
  }

  /* Select on this port */
  result = bcm_field_qualify_InPorts(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, pbm, pbm_mask);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qualify_InPorts\n");
      return result;
  }

  /* Ethernet forwarding */
  result = bcm_field_qualify_ForwardingType(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldForwardingTypeL2);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qualify_ForwardingType\n");
      return result;
  }

  /* Ethernet forward lookup is found */
  result = bcm_field_qualify_L2DestHit(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, 0x1, 0x1);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qualify_L2DestHit\n");
      return result;
  }

  /* Define the preselector-set */
  BCM_FIELD_PRESEL_INIT(psset);
  BCM_FIELD_PRESEL_ADD(psset, presel_id);


  /* 
   * Create the 3 data qualifiers
   */
  for (dq_ndx = 0; dq_ndx < nof_dqs; dq_ndx++) {
      bcm_field_data_qualifier_t_init(&dq_trill[dq_ndx]); 
      dq_trill[dq_ndx].flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES; 
      dq_trill[dq_ndx].qualifier = dq_qualifier[dq_ndx]; 
      dq_trill[dq_ndx].offset = dq_offset[dq_ndx]; 
      dq_trill[dq_ndx].length = dq_length[dq_ndx]; 
      result = bcm_field_data_qualifier_create(unit, &dq_trill[dq_ndx]);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_data_qualifier_create\n");
          return result;
      }      
  }

  /* 
   * Create the Field group   
   */
  bcm_field_group_config_t_init(&grp_config);
  grp_config.group = group;

  /* Define the QSET */
  BCM_FIELD_QSET_INIT(grp_config.qset);
  BCM_FIELD_QSET_ADD(grp_config.qset, bcmFieldQualifyStageIngress);
  for (dq_ndx = 0; dq_ndx < nof_dqs; dq_ndx++) {
      /* Add the Data qualifier to the QSET */
      result = bcm_field_qset_data_qualifier_add(unit, &grp_config.qset, dq_trill[dq_ndx].qual_id);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_qset_data_qualifier_add\n");
          return result;
      }
  }

  /*
   *  This Field Group can change the EEI value
   */
  BCM_FIELD_ASET_INIT(grp_config.aset);
  BCM_FIELD_ASET_ADD(grp_config.aset, bcmFieldActionSystemHeaderSet);

  /*  Create the Field group */
  grp_config.priority = group_priority;
  grp_config.flags = BCM_FIELD_GROUP_CREATE_WITH_ID | BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
  grp_config.mode = bcmFieldGroupModeDirectExtraction;
  grp_config.preselset = psset;
  result = bcm_field_group_config_create(unit, &grp_config);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_create\n");
      return result;
  }

  /*
   *  Add an entry to the group.
   */
  result = bcm_field_entry_create(unit, grp_config.group, &ent);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_entry_create\n");
      return result;
  }

  bcm_field_extraction_action_t_init(&extact);
  extact.action = bcmFieldActionSystemHeaderSet;
  extact.bias = 0;
  for (dq_ndx = 0; dq_ndx < nof_dqs; dq_ndx++) {
      bcm_field_extraction_field_t_init(&(ext[dq_ndx]));
      ext[dq_ndx].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
      ext[dq_ndx].qualifier = dq_trill[dq_ndx].qual_id;
      ext[dq_ndx].lsb = 0;
      ext[dq_ndx].bits = dq_length[dq_ndx];
  }
  result = bcm_field_direct_extraction_action_add(unit,
                                                  ent,
                                                  extact,
                                                  nof_dqs /* count */,
                                                  ext);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_direct_extraction_action_add\n");
      return result;
  }

  /* Install all to the HW */
  result = bcm_field_group_install(unit, grp_config.group);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_install\n");
      return result;
  }
  
  return result;
}


/* Add entry per VSI-Profile: which nickname to use */
int trill_virtual_rbrdige_ing_nickname_vsi_profile_set(/* in */ int unit,
                                                    /* in */ bcm_field_group_t group_lookup,
                                                    /* in */ int vsi_profile,
                                                    /* in */ int nickname) {
  int result;
  int auxRes;
  bcm_field_entry_t ent;

  /*
   *  Add an entry to the group.
   */
  result = bcm_field_entry_create(unit, group_lookup, &ent);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_entry_create\n");
      return result;
  }

  /* Qualify on the VSO-Profile */
  result = bcm_field_qualify_InterfaceClassL2(unit, ent, vsi_profile, 0x3 /* 2 bits mask */);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qualify_InterfaceClassL2\n");
      return result;
  }

  /* Change the cascaded value to be the nickname */
  result = bcm_field_action_add(unit, ent, bcmFieldActionCascadedKeyValueSet, nickname, 0);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_action_add\n");
      return result;
  }

  /* Install all to the HW */
  result = bcm_field_group_install(unit, group_lookup);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_install\n");
      return result;
  }

  return result;
}


/*
 *  Create Field group such that for bridged packets coming
 *  - from pbm (for ports where is configured port profile with nickname_index),
 *  - when the L2 destination is NOT found,
 *  set (via index) the virtual nickname according to the VSI profile
 *  
 *  In practice,
 *  1. Lookup with the VSI-profile into Direct Table to get the new Nickname distribution tree
 *  2. rebuild the EEI:
 *  [ In-Port-Profile[22:16] as new-EEI[23:17] | Constant '1' as new-EEI[16]
 *       | Lookup-Result of VSI-Profile lookup as new-EEI[15:0] ]
 */
int trill_virtual_rbrdige_ing_nickname_flooding_set(/* in */ int unit,
                                                    /* in */ int group_priority_lookup,
                                                    /* in */ bcm_field_group_t group_lookup,
                                                    /* in */ int group_priority_eei,
                                                    /* in */ bcm_field_group_t group_eei,
                                                    /* in */ bcm_pbmp_t pbm) {
  int result;
  int auxRes;
  int i;
  bcm_field_qset_t qset;
  bcm_field_aset_t aset;
  bcm_field_entry_t ent;
  /* In large Direct Extraction Databases, the qualifier length is maximally 16 bits */
  bcm_field_data_qualifier_t dq_trill[2]; /* LSB - Lookup-Result[15:0] = Cascaded Value, 0 - M-bit, 1 - In-PP-Port-Profile[22:16] */
  uint16 dq_offset[2] = {0, 16};
  uint8 dq_length[2] = {1, 7};
  bcm_field_qualify_t dq_qualifier[2] = {bcmFieldQualifyConstantOne, bcmFieldQualifyInterfaceClassProcessingPort};
  uint32 fg_ndx, nof_fgs = 2, dq_ndx, nof_dqs = 2;
  bcm_pbmp_t pbm_mask;
  int presel_id = 1;
  int presel_flags = 0;
  bcm_field_presel_set_t psset;
  bcm_field_extraction_field_t ext[3];
  bcm_field_extraction_action_t extact;
  bcm_field_group_config_t grp_config;
  int value;

  /* 
   * Set the preselector to be on this port
   */
  BCM_PBMP_CLEAR(pbm_mask);
  for(i=0; i<256; i++) { BCM_PBMP_PORT_ADD(pbm_mask, i); } /* Mask on all the ports */

  /* Create a presel entity */
    if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
            presel_flags = BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
            result = bcm_field_presel_create_stage_id(unit, bcmFieldStageIngress, presel_id);
    } else {
            result = bcm_field_presel_create(unit, &presel_id);
    }
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_presel_create\n");
      return result;
  }

  result = bcm_field_qualify_Stage(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldStageIngress);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qualify_Stage\n");
      return result;
  }

  /* Select on this port */
  result = bcm_field_qualify_InPorts(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, pbm, pbm_mask);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qualify_InPorts\n");
      return result;
  }

  /* Ethernet forwarding */
  result = bcm_field_qualify_ForwardingType(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldForwardingTypeL2);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qualify_ForwardingType\n");
      return result;
  }

  /* Ethernet forward lookup is NOT found */
  result = bcm_field_qualify_L2DestHit(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, 0x0, 0x1);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qualify_L2DestHit\n");
      return result;
  }

  /* Define the preselector-set */
  BCM_FIELD_PRESEL_INIT(psset);
  BCM_FIELD_PRESEL_ADD(psset, presel_id);

  /*
   *  Set the cascaded key length to 16 bits
   */
  result = bcm_field_control_set(unit, bcmFieldControlCascadedKeyWidth, 16 /* bits in cascaded key */);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_control_set\n");
      return result;
  }

  /* 
   * Create the 3 data qualifiers
   */
  for (dq_ndx = 0; dq_ndx < nof_dqs; dq_ndx++) {
      bcm_field_data_qualifier_t_init(&dq_trill[dq_ndx]); 
      dq_trill[dq_ndx].flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES; 
      dq_trill[dq_ndx].qualifier = dq_qualifier[dq_ndx]; 
      dq_trill[dq_ndx].offset = dq_offset[dq_ndx]; 
      dq_trill[dq_ndx].length = dq_length[dq_ndx]; 
      result = bcm_field_data_qualifier_create(unit, &dq_trill[dq_ndx]);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_data_qualifier_create\n");
          return result;
      }      
  }


  /* 
   * Create the Field groups   
   */
  for (fg_ndx = 0; fg_ndx < nof_fgs; fg_ndx++) {
      bcm_field_group_config_t_init(&grp_config);
      grp_config.group = (fg_ndx == 0)? group_eei: group_lookup;

      /* Define the QSET */
      BCM_FIELD_QSET_INIT(grp_config.qset);
      BCM_FIELD_QSET_ADD(grp_config.qset, bcmFieldQualifyStageIngress);
      if (fg_ndx == 0) {
          BCM_FIELD_QSET_ADD(grp_config.qset, bcmFieldQualifyCascadedKeyValue);
          for (dq_ndx = 0; dq_ndx < nof_dqs; dq_ndx++) {
              /* Add the Data qualifier to the QSET */
              result = bcm_field_qset_data_qualifier_add(unit, &grp_config.qset, dq_trill[dq_ndx].qual_id);
              if (BCM_E_NONE != result) {
                  printf("Error in bcm_field_qset_data_qualifier_add\n");
                  return result;
              }
          }
      }
      else {
          BCM_FIELD_QSET_ADD(grp_config.qset, bcmFieldQualifyInterfaceClassL2);
      }

      /*
       *  This Field Group can change the EEI value
       */
      BCM_FIELD_ASET_INIT(grp_config.aset);
      if (fg_ndx == 0) {
          BCM_FIELD_ASET_ADD(grp_config.aset, bcmFieldActionSystemHeaderSet);
      } else {
          BCM_FIELD_ASET_ADD(grp_config.aset, bcmFieldActionCascadedKeyValueSet);
      }

      /*  Create the Field group */
      grp_config.priority = (fg_ndx == 0)? group_priority_eei: group_priority_lookup;
      grp_config.flags = BCM_FIELD_GROUP_CREATE_WITH_ID | BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
      if (fg_ndx == 0) {
          grp_config.mode = bcmFieldGroupModeDirectExtraction;
      } else {
          /* use Direct table for the first lookup field group */
          grp_config.mode = bcmFieldGroupModeAuto;
      }
      grp_config.preselset = psset;
      result = bcm_field_group_config_create(unit, &grp_config);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_group_create\n");
          return result;
      }

      if (fg_ndx == 0) {
          /*
           *  Add an entry to the group.
           */
          result = bcm_field_entry_create(unit, grp_config.group, &ent);
          if (BCM_E_NONE != result) {
              printf("Error in bcm_field_entry_create\n");
              return result;
          }

          bcm_field_extraction_action_t_init(&extact);
          extact.action = bcmFieldActionSystemHeaderSet;
          extact.bias = 0;
          bcm_field_extraction_field_t_init(&(ext[0]));
          ext[0].qualifier = bcmFieldQualifyCascadedKeyValue;
          ext[0].lsb = 0;
          ext[0].bits = 16;
          for (dq_ndx = 0; dq_ndx < nof_dqs; dq_ndx++) {
              bcm_field_extraction_field_t_init(&(ext[1 + dq_ndx]));
              ext[1 + dq_ndx].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
              ext[1 + dq_ndx].qualifier = dq_trill[dq_ndx].qual_id;
              ext[1 + dq_ndx].lsb = 0;
              ext[1 + dq_ndx].bits = dq_length[dq_ndx];
          }
          result = bcm_field_direct_extraction_action_add(unit,
                                                          ent,
                                                          extact,
                                                          1 + nof_dqs /* count */,
                                                          ext);
          if (BCM_E_NONE != result) {
              printf("Error in bcm_field_direct_extraction_action_add\n");
              return result;
          }
      } /* Add entry to the direct extraction field group */

      /* Install all to the HW */
      result = bcm_field_group_install(unit, grp_config.group);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_group_install\n");
          return result;
      }
  }

  return result;
}


/* setup pmf program:
 * - for link failure when egress trill to virtual rbridge
 * - for egress trill and no DA match, send to all member ports of RBv (or to RB1)
 * params:
 * mc_rb1_members: mc id for rb1 members
 * mc_rbv_members: mc id for rbc members
 */
int egress_link_failure_and_egress_trill_no_da_match(int unit, 
                                                     int my_nickname,
                                                     int virtual_nickname,
                                                     int mc_rb1_members, 
                                                     int mc_rbv_members,
                                                     bcm_port_t port_trap_cpu) {
    int rv;
    bcm_gport_t trap_gport;
    bcm_gport_t gport_mc_rb1_members;
    bcm_gport_t gport_mc_id_rbv_members;

    /* pmf group */
    int group_priority = 4;
    bcm_field_group_t group = 4;


    /* configure trap */
    rv = set_trap_to_cpu(unit, port_trap_cpu, &trap_gport);
    BCM_IF_ERROR_RETURN(rv);

    /*get gport from MC-id*/
    BCM_GPORT_MCAST_SET(gport_mc_rb1_members, mc_rb1_members);
    BCM_GPORT_MCAST_SET(gport_mc_id_rbv_members, mc_rbv_members);

    /* link failure mgmt: for virtual rbridge, if da is fec, trap */
    /* if flooding and RBv -> send to RBv port members (MC1)
     * if floodign and RB1 -> send to RB1 port members (MC2) */
    rv = trill_virtual_rbrdige_eg_unicast_set(unit, 
                                         group_priority,  /* pmf */
                                         group,           /* pmf */
                                         gport_mc_id_rbv_members,
                                         gport_mc_rb1_members,
                                         trap_gport,
                                         virtual_nickname, 
                                         my_nickname);  
    BCM_IF_ERROR_RETURN(rv);

    printf("after trill_virtual_rbrdige_eg_unicast_set\n");

    return rv;
}


/*
 *  Create Field group such that for Trill-forwarded (Egress Trill)
 *  unicast packets:
 *  1. Trap packet whose destination is FEC and DA-Match
 *  2. Change the destination to Multicast (to all members of the
 *  virtual Rbridge) if DA-Not-Matchedegr rbridge => 0xcccc
 */
int trill_virtual_rbrdige_eg_unicast_set(/* in */ int unit,
                                         /* in */ int group_priority,
                                         /* in */ bcm_field_group_t group,
                                         /* in */ int mc_id_rbv_members_gport,
                                         /* in */ int mc_id_rb_members_gport,
                                         /* in */ int trap_gport,
                                         /* in */ int virtual_nickname, 
                                         /* in */ int my_nickname) {
  int result;
  int auxRes;
  bcm_field_qset_t qset;
  bcm_field_aset_t aset;
  bcm_field_entry_t ent;
  bcm_field_group_config_t grp_config;
  bcm_field_data_qualifier_t dq_trill[3]; /* 0 - Trill M-Bit, 1 - Egress NickName, 2 - Destination[18:15] = FEC if '0100', take the whole destination */
  uint16 dq_offset[3] = {0, 2, 0}; /* In bytes */
  uint8 dq_length[3] = {8, 16, 19};
  uint8 is_qualifier[3] = {0, 0, 1};
  bcm_field_qualify_t dq_qualifier[3] = {0, 0, bcmFieldQualifyDstPort};
  bcm_field_data_offset_base_t offset_base[3] = {bcmFieldDataOffsetBaseFirstHeader, 
      bcmFieldDataOffsetBaseFirstHeader, 0};
  uint32 ent_ndx, nof_entries = 3;
  uint32 dq_ndx, nof_dqs = 3;
  uint8 mbit_data[3], mbit_mask[3];
  uint8 data, mask;
  int presel_id = 2;
  int presel_flags = 0;
  bcm_field_presel_set_t psset;

  /* Create a presel entity */
    if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
            presel_flags = BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
            result = bcm_field_presel_create_stage_id(unit, bcmFieldStageIngress, presel_id);
    } else {
            result = bcm_field_presel_create(unit, &presel_id);
    }
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_presel_create\n");
      return result;
  }

  result = bcm_field_qualify_Stage(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldStageIngress);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qualify_Stage\n");
      return result;
  }

  /* Trill forwarding */
  result = bcm_field_qualify_ForwardingType(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldForwardingTypeL2);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qualify_ForwardingType\n");
      return result;
  }

  /* Define the preselector-set */
  BCM_FIELD_PRESEL_INIT(psset);
  BCM_FIELD_PRESEL_ADD(psset, presel_id);

  /* 
   * Create the 2 data qualifiers
   */
  for (dq_ndx = 0; dq_ndx < nof_dqs; dq_ndx++) {
      bcm_field_data_qualifier_t_init(&dq_trill[dq_ndx]); 
      dq_trill[dq_ndx].flags = BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES; 
      if (is_qualifier[dq_ndx]) {
          dq_trill[dq_ndx].flags |= BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED; 
          dq_trill[dq_ndx].qualifier = dq_qualifier[dq_ndx]; 
      }
      else {
          dq_trill[dq_ndx].offset_base = offset_base[dq_ndx]; 
      }
      dq_trill[dq_ndx].offset = dq_offset[dq_ndx]; 
      dq_trill[dq_ndx].length = dq_length[dq_ndx]; 
      result = bcm_field_data_qualifier_create(unit, &dq_trill[dq_ndx]);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_data_qualifier_create\n");
          return result;
      }      
  }

  /* 
   * Create the Field group   
   */
  bcm_field_group_config_t_init(&grp_config);
  grp_config.group = group;

  /* Define the QSET */
  BCM_FIELD_QSET_INIT(grp_config.qset);
  BCM_FIELD_QSET_ADD(grp_config.qset, bcmFieldQualifyStageIngress);
  BCM_FIELD_QSET_ADD(grp_config.qset, bcmFieldQualifyL2DestHit);
  BCM_FIELD_QSET_ADD(grp_config.qset, bcmFieldQualifyHeaderFormat);
  for (dq_ndx = 0; dq_ndx < nof_dqs; dq_ndx++) {
      /* Add the Data qualifier to the QSET */
      result = bcm_field_qset_data_qualifier_add(unit, &grp_config.qset, dq_trill[dq_ndx].qual_id);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_qset_data_qualifier_add\n");
          return result;
      }
  }

  /*
   *  This Field Group can change the EEI value
   */
  BCM_FIELD_ASET_INIT(grp_config.aset);
  BCM_FIELD_ASET_ADD(grp_config.aset, bcmFieldActionRedirect);
  BCM_FIELD_ASET_ADD(grp_config.aset, bcmFieldActionTrap);

  /*  Create the Field group */
  grp_config.priority = group_priority;
  grp_config.flags = BCM_FIELD_GROUP_CREATE_WITH_ID | BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
  grp_config.preselset = psset;
  result = bcm_field_group_config_create(unit, &grp_config);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_create\n");
      return result;
  }

  /*
   *  Add entries to the group.
   */
  for (ent_ndx = 0; ent_ndx < nof_entries; ent_ndx++) {
      result = bcm_field_entry_create(unit, grp_config.group, &ent);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_entry_create\n");
          return result;
      }

      /* L2 lookup */
      result = bcm_field_qualify_L2DestHit(unit, ent, ((ent_ndx == 0)?0x1: 0x0), 0x1);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_qualify_L2DestHit\n");
          return result;
      }

      /* Trill packet */
      result = bcm_field_qualify_HeaderFormat(unit, ent, bcmFieldHeaderFormatEthTrillEth);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_qualify_HeaderFormat\n");
          return result;
      }

      /* qualifier for m-bit NOT set */
      mbit_data[0] = 0x0;
      mbit_mask[0] = 0x8;
      result = bcm_field_qualify_data(unit, ent, dq_trill[0 /* M-bit */].qual_id, mbit_data, mbit_mask, 1 /* len here always in bytes */);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_qualify_data1\n");
          return result;
      }

      /* qualifier for correct egress nickname */
      mbit_data[0] = (((ent_ndx <= 1)? virtual_nickname : my_nickname) >> 8) & 0xFF;
      mbit_mask[0] = 0xFF;
      mbit_data[1] = ((ent_ndx <= 1)? virtual_nickname : my_nickname ) & 0xFF;
      mbit_mask[1] = 0xFF;
      result = bcm_field_qualify_data(unit, ent, dq_trill[1 /* Egress Nickname */].qual_id, mbit_data, mbit_mask, 2 /* len here always in bytes */);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_qualify_data2\n");
          return result;
      }

      /* FEC forwarding for first entry  */
      if (ent_ndx == 0) {
          /* 0100 for FEC destination in destination HW encoding */
          mbit_data[2] = 0x00; 
          mbit_mask[2] = 0x00;
          mbit_data[1] = 0x00;
          mbit_mask[1] = 0x80;
          mbit_data[0] = 0x02;
          mbit_mask[0] = 0x07;
          result = bcm_field_qualify_data(unit, ent, dq_trill[2 /* Destination */].qual_id, mbit_data, mbit_mask, 3 /* len here always in bytes */);
          if (BCM_E_NONE != result) {
              printf("Error in bcm_field_qualify_data3\n");
              return result;
          }
      }

      if (ent_ndx == 0) {
          /* Trap the packet */
          result = bcm_field_action_add(unit, ent, bcmFieldActionTrap, trap_gport, 0);
          if (BCM_E_NONE != result) {
              printf("Error in bcm_field_action_add\n");
              return result;
          }
      } else if (ent_ndx == 1){
          /* Redirect to Multicast-Id */
          result = bcm_field_action_add(unit, ent, bcmFieldActionRedirect, 0, mc_id_rbv_members_gport);
          if (BCM_E_NONE != result) {
              printf("Error in bcm_field_action_add\n");
              return result;
          }
      } else if (ent_ndx == 2){
          /* Redirect to Multicast-Id */
          result = bcm_field_action_add(unit, ent, bcmFieldActionRedirect, 0, mc_id_rb_members_gport);
          if (BCM_E_NONE != result) {
              printf("Error in bcm_field_action_add\n");
              return result;
          }
      }
  }

  /* Install all to the HW */
  result = bcm_field_group_install(unit, grp_config.group);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_install\n");
      return result;
  }
  
  return result;
}




