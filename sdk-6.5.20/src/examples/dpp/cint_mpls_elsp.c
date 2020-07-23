/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~MPLS LSR~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_mpls_elsp.c
 * Purpose: Example of mapping MPLS packets with and without using EXP as a key.
 *
 * Explanation:
 *  *  For incoming packet, the Ethernet header is terminated.
 *  *   Labels are swapped and forwarded to egress port with new lable and Ethernet encapsulations.
 *  *   For in_label 15000, different EXPs would receive different labels at the egress. 
 *
 * Calling Sequence:
 *  *   Make sure soc property mpls_elsp_label_range_min is at most 15000, and soc property mpls_elsp_label_range_max is at least 15000
 *  *   Port TPIDs setting
 *      -   Assuming default TPID setting, that is, all ports configured with TPID 0x8100.
 *  *   Create MPLS L3 interface.
 *      -   Set interface with my-MAC 00:00:00:00:00:11 and VID 200. 
 *      -   Packet routed to this L3 interface will be set with this MAC. 
 *      -   Calls bcm_l3_intf_create().
 *  *   Create egress object points to the above l3-interface.
 *      -   Packet routed to this egress-object will be forwarded to egress port and encapsulated 
 *          with Ethernet Header.
 *          *   DA; next-hop-DA.
 *          *   SA/VID: according to interface definition.
 *      -  Calls bcm_l3_egress_create().
 *  *  Add ILM (ingress label mapping) entry (refer to mpls_add_switch_entry).
 *      -   Maps incoming label (5000) to egress label (8000) and points to egress object created above.
 *           - Or, maps incoming label (15000) and incoming EXP (0 - 7) to egress label (30000 + EXP / 2) and points to egress object created above.
 *      -   Flags.
 *          *   BCM_MPLS_SWITCH_OUTER_TTL|BCM_MPLS_SWITCH_OUTER_EXP: both have to be present or     
 *              absent; indicates Pipe/uniform models.
 *          *   BCM_MPLS_SWITCH_TTL_DECREMENT has to be present.
 *          *   BCM_MPLS_SWITCH_ACTION_SWAP: for LSR functionality.
 *          *   BCM_MPLS_SWITCH_LOOKUP_INT_PRI: For label + EXP as key functionality. Only used with relevant entries.
 *      -   Calls bcm_mpls_tunnel_switch_create().
 *
 * To Activate Above Settings Run (example of call):
 *      BCM> cint cint_qos.c
 *      BCM> cint cint_qos_elsr.c
 *      BCM> cint utility/cint_utils_l3.c   
 *      BCM> cint cint_mpls_lsr.c
 *      BCM> cint cint_mpls_elsp.c
 *      BCM> cint
 *      cint> int rv;
 *      cint> int unit = 0;
 *      cint> int second_unit = -1;
 *      cint> int outP = 13;
 *      cint> rv = mpls_elsp_create_mapping(unit, second_unit, outP);
 * 
 * Note: (For a two-device system call mpls_elsp_create_mapping() with second_unit >= 0.)
 *          This option is currently unavilable.
 *
 * Traffic:
 *
 * * Sending packet from egress port 1. Refer to the table below for releveant prio and labels: 
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+    |
 *   |    | DA |SA||TIPD1 |Prio|VID||   MPLS   || Data |    |
 *   |    |0:11|  ||0x8100| EXP|100||Label:LBL||      |    | 
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+    |
 *   |   +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+   |
 *   |   | Figure 23: Sending Packet from Egress Port 1 |   | 
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  
 *
 * * The packet will be received in egress port 1 with following header
 *   (label swapped, Ethernet header updated). Again, refer to the table below for releveant prio and labels
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+    |
 *   |    |DA| SA ||TIPD1 |Prio|VID||   MPLS   || Data |    |
 *   |    |  |0:11||0x8100| EXP|   ||Label:egLBL||      |    | 
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+    |
 *   |   +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+   |
 *   |   | Figure 24: Packets Received on Egress Port 1 |   | 
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 * 
 *  Label and EXP mapping:
 * 
 *      Ingress Label   \ EXP   \   Egress Label
 *           5000          \   any  \        8000
 *          15000         \ 0 - 1   \       30000
 *          15000         \ 2 - 3   \       30001
 *          15000         \ 4 - 5   \       30002
 *          15000         \ 6 - 7   \       30003
 * 
 * 
 * Remarks:
 *  *
 *  *   You can add more label mappings using mpls_add_switch_entry(), but the system can only support one table of QOS mapping.
 *
 * Script adjustment:
 *  You can adjust the following attributes of the application. Change them from line 5 of mpls_elsp_create_mapping() (The call to mpls_lsr_init). Please refer to cint_mpls_lsr.c to see the relevant function header: 
 *  -  Vid, my_mac:     ethernet attribute, MPLS packets arrived on this VLAN with this MAD as DA, 
 *                      will be L2 terminated and forwarded to MPLS engine
 *  -  in_label:        incoming label to Swap
 *  -  eg_label:        egress label
 *  -  eg_port:         physical port to forward packet to 
 *  -  eg_vid:          VLAN id to stamp in new Ethernet header
 */

int eg_intfs[5]; 
int eg_intfs_indx;

int mpls_elsp_create_mapping(int unit, int second_unit, int outp){
    int exp = 0;
    int rv = BCM_E_NONE;
    eg_intfs_indx = 0;
    
    /* Initialise data */
    mpls_lsr_init(outp, outp, 0x11, 0x22, 5000, 8000, 100,200,0);

    rv = mpls_elsp_create_vlan(unit,second_unit);
    if (rv != BCM_E_NONE){
        printf("Error in mpls_elsp_create_vlan.");
        return rv;
    }
    

    /* Create one mapping without QOS*/
    rv = mpls_elsp_config_with_exp(unit,second_unit,-1);
    if (rv != BCM_E_NONE){
        printf("Error in mpls_elsp_config_with_exp.");
        return rv;
    }
   
    /* Create QOS mapping*/
    rv = qos_map_elsr(unit);
    if (rv != BCM_E_NONE){
        printf("Error in qos_map_elsr. Arguments were: %d %d %d\n", unit, second_unit, outp);
        return rv;
    }

    /* Ingress labels between 10000 to 20000 are mapped using QOS as key*/
    mpls_lsr_info_1.in_label = 15000;
    
    /* Create more label mappings in the relevent range*/
    for (exp = 0 ; exp < 4 ; exp++){
        mpls_lsr_info_1.eg_label = 30000 + exp; /* Create a distinction between different mappings */
        rv = mpls_elsp_config_with_exp(unit,second_unit,exp);
        if (rv != BCM_E_NONE){
            printf("Error in mpls_elsp_config_with_exp.");
            return rv;
        }
    }

    printf ("Mapping successfull.\n");
    return rv;
}

int
mpls_elsp_config_with_exp(int unit, int second_unit, int exp){
    int CINT_NO_FLAGS = 0;
    int ingress_intf;
    int egress_intf;
    int encap_id;
    int rv;
    create_l3_intf_s intf;
           
    /* L3 interface for mpls rounting */
    intf.vsi = mpls_lsr_info_1.in_vid;
    intf.my_global_mac = mpls_lsr_info_1.my_mac;
    intf.my_lsb_mac = mpls_lsr_info_1.my_mac;
    intf.skip_mymac = mpls_lsr_info_1.use_multiple_mymac;
    
    rv = l3__intf_rif__create(unit, &intf);
    ingress_intf = intf.rif;
    if (rv != BCM_E_NONE) {
    	printf("Error, in l3__intf_rif__create\n");
    	return rv;
    }
     
    /* create ingress object, packet will be routed to */
    intf.vsi = mpls_lsr_info_1.eg_vid;
    
    rv = l3__intf_rif__create(unit, &intf);
    ingress_intf = intf.rif;
    if (rv != BCM_E_NONE) {
    	printf("Error, in l3__intf_rif__create\n");
    	return rv;
    }
    
    if (second_unit >= 0)
    {
    	rv = l3__intf_rif__create(second_unit, &intf);
    	ingress_intf = intf.rif;
    	if (rv != BCM_E_NONE) {
    		printf("Error, in l3__intf_rif__create\n");
    		return rv;
    	}
    }
    create_l3_egress_s l3eg;
    sal_memcpy(l3eg.next_hop_mac_addr, mpls_lsr_info_1.next_hop_mac , 6);
    l3eg.out_tunnel_or_rif = ingress_intf;
    l3eg.out_gport = mpls_lsr_info_1.eg_port;
    l3eg.vlan = mpls_lsr_info_1.eg_vid;
    l3eg.fec_id = egress_intf;
    l3eg.arp_encap_id = encap_id;

    if (second_unit >= 0)
    {
        /* create egress object */
        rv = l3__egress__create(second_unit,&l3eg);
        if (rv != BCM_E_NONE) {
            printf("Error, l3__egress__create\n");
            return rv;
        }

        egress_intf = l3eg.fec_id;
        encap_id = l3eg.arp_encap_id;
    
        /* create egress object */
        l3eg.allocation_flags = BCM_L3_WITH_ID;
        rv = l3__egress__create(unit,&l3eg);
        if (rv != BCM_E_NONE) {
            printf("Error, l3__egress__create\n");
            return rv;
        }

        egress_intf = l3eg.fec_id;
        encap_id = l3eg.arp_encap_id;
    }
    else
    {
        /* create egress object */
        rv = l3__egress__create(unit,&l3eg);
        if (rv != BCM_E_NONE) {
            printf("Error, l3__egress__create\n");
            return rv;
        }

        egress_intf = l3eg.fec_id;
        encap_id = l3eg.arp_encap_id;
    }  
    
    /* add switch entry to perform LSR */
    /* swap, in-label, with egress label, and point to egress object */
    rv = mpls_add_exp_label_entry(unit,mpls_lsr_info_1.in_label, mpls_lsr_info_1.eg_label,egress_intf, exp);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls_add_switch_entry\n");
        return rv;
    }
    if (second_unit >= 0)
    {
        rv = mpls_add_exp_label_entry(second_unit,mpls_lsr_info_1.in_label, mpls_lsr_info_1.eg_label,egress_intf, exp);
        if (rv != BCM_E_NONE) {
            printf("Error, in mpls_add_switch_entry\n");
            return rv;
        }
    }
    
    return rv;
}


int mpls_add_exp_label_entry(int unit, int in_label, int eg_label, bcm_if_t egress_intf, int exp)
{
   int rv;
   bcm_mpls_tunnel_switch_t entry;

   printf("Enter in mpls_add_exp_label_entry:\n");

   bcm_mpls_tunnel_switch_t_init(&entry);
   entry.action = BCM_MPLS_SWITCH_ACTION_SWAP;

   /* TTL decrement has to be present */
    entry.flags = BCM_MPLS_SWITCH_TTL_DECREMENT;
    /* Uniform: inherit TTL and EXP,
     * in general valid options:
     * both present (uniform) or none of them (Pipe)
     */
    entry.flags |= BCM_MPLS_SWITCH_OUTER_TTL|BCM_MPLS_SWITCH_OUTER_EXP;

    if (exp >= 0){
        /*add In-EXP into the key */
        entry.flags |= BCM_MPLS_SWITCH_LOOKUP_INT_PRI;
    }

    if (exp >= 1){
        /*add In-EXP into the key */
        entry.flags |= BCM_MPLS_SWITCH_REPLACE|BCM_MPLS_SWITCH_WITH_ID;
    }

      /* incoming label */
    entry.label = in_label;

    /* egress attribures*/
    entry.egress_label.label = eg_label;
    entry.egress_if = egress_intf;
    eg_intfs[eg_intfs_indx++] = egress_intf;
    
    /*EXP-map ID*/
    if (exp >= 0) {
        entry.exp_map=exp; /*map ID */
    }

    rv = bcm_mpls_tunnel_switch_create(unit,&entry);
    if (rv != BCM_E_NONE) {
         printf("Error, in bcm_mpls_tunnel_switch_add when insert an Entry in ILM with {EXP,Label}\n");
         return rv;
    }
    return rv;
}

int 
mpls_elsp_create_vlan(int unit, int second_unit){
    int rv= BCM_E_NONE;
    bcm_pbmp_t pbmp;

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, mpls_lsr_info_1.in_port);

    rv = bcm_vlan_destroy(unit,mpls_lsr_info_1.in_vid);
    rv = bcm_vlan_create(unit,mpls_lsr_info_1.in_vid);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_create\n");
        print rv;
        return rv;
    }
    rv = bcm_vlan_port_add(unit, mpls_lsr_info_1.in_vid, pbmp, pbmp);
    if (BCM_FAILURE(rv)) {
        printf("BCM FAIL %d: %s\n", rv, bcm_errmsg(rv));
        return CMD_FAIL;
    }

    rv = bcm_vlan_destroy(unit,mpls_lsr_info_1.eg_vid);
    rv = bcm_vlan_create(unit,mpls_lsr_info_1.eg_vid);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_create\n");
        print rv;
        return rv;
    }

    if (second_unit >= 0)
    {
        rv = bcm_vlan_destroy(second_unit,mpls_lsr_info_1.in_vid);
        rv = bcm_vlan_create(second_unit,mpls_lsr_info_1.in_vid);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_create\n");
            print rv;
            return rv;
        }
        rv = bcm_vlan_destroy(second_unit,mpls_lsr_info_1.eg_vid);
        rv = bcm_vlan_create(second_unit,mpls_lsr_info_1.eg_vid);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_create\n");
            print rv;
            return rv;
        }

        BCM_PBMP_CLEAR(pbmp);
        BCM_PBMP_PORT_ADD(pbmp, BCM_GPORT_MODPORT_PORT_GET(mpls_lsr_info_1.eg_port));

        rv = bcm_vlan_port_add(second_unit, mpls_lsr_info_1.eg_vid, pbmp, pbmp);
        if (BCM_FAILURE(rv)) {
            printf("BCM FAIL %d: %s\n", rv, bcm_errmsg(rv));
            return CMD_FAIL;
        }
    } else {
        BCM_PBMP_CLEAR(pbmp);
        BCM_PBMP_PORT_ADD(pbmp, mpls_lsr_info_1.eg_port);  

        rv = bcm_vlan_port_add(unit, mpls_lsr_info_1.eg_vid, pbmp, pbmp);
        if (BCM_FAILURE(rv)) {
            printf("BCM FAIL %d: %s\n", rv, bcm_errmsg(rv));
            return CMD_FAIL;
        }
    }
    return rv;

}

int eg_intf_get(int index){
    if (index >= 0 && index <5){
        return eg_intfs[index];
    } else {
        return BCM_E_PARAM;
    }
}

int qos_map_id_get(){
    return mpls_in_map_id;
}

int qos_map_entry_get(int unit, int flags, int map_id, int index){
    bcm_qos_map_t maps[8];
    int rv, count;
    
    rv = bcm_qos_map_multi_get(unit, 0, map_id, 8, &maps, &count);

    if (rv != BCM_E_NONE){
        prinf("Error: in bcm_qos_map_multi_get");
        print rv;
        return -1;
    }

    return maps[index].int_pri;
}
