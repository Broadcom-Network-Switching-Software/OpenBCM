/*~~~~~~~~~~~~~~~~~~~~~~~~~~Traditional Bridge: Spanning-Tree Groups~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 *  
 * File: cint_stg.c 
 * Purpose: Example of simple spanning-tree group configuration 
 *  
 * Default settings:
 * It is assumed diag_init is executed:
 *  o    All ports set with TPID 0x8100 and all packet formats (tagged/untagged) are accepted.
 *  o    PVID of all ports set to VID = 1, thus untagged packet is associated to VLAN 1.
 *  o    By default, learning is managed by device without interference from the CPU.
 *  o    By default, aging is disabled.
 *  o    All VLANs are mapped to Default STG (0).
 *  o    STP state of all ports in default topology is set to forwarding.
 * 
 * Note: Without running diag_init, ports are not associated with any TPID,
 *       and thus all packets would be considered as untagged.
 *       Besides, only untagged packets are accepted.
 *  
 * Settings include: 
 *  o    Creating one STG. 
 *  o    Creating 2 VLANs. 
 *  o    Adding the VLANs to the stg.
 *  o    Connecting the stg with 2 ports, for first port with stp state forward
 *       and the second one with stp state block.
 *  
 * Application Sequence: 
 *  o    Run application: main()
 *  o    Add mac entries that forward traffic with the two vids from the two ports to a third port.
 *  o    Send packets with one of the VLANs (in our example, vid = 100 or 300) on first port
 *       (in our example, port 1) and check that it is indeed forwarded to the third port.
 *  o    Send packets with one of the VLANs (in our example, vid = 100 or 300) on second port
 *       (in our example, port 2) and see that it is dropped.
 *  o    To delete the VLANs and stg that were created for the test use stg_revert().
 *  o    Set bandwidth limitation of 500 Mbps on scheduler. 
 *  
 * Note: You can modify the ports, the VLANs, and the stp states that the script uses by changing 
 *       the values in the init() function.
 */

struct stg_info_s {
    int vlan_1;
    int vlan_2;
    int port_1;
    int port_2;  
    int stp_state_1;
    int stp_state_2;
    bcm_stg_t stg;  
};
stg_info_s info;

void
init(int unit, stg_info_s *info,int port1,int port2) {    
    int rv;    
    
    info->vlan_1 = 100;
    info->vlan_2 = 300;
    info->port_1 = port1;
    info->port_2 = port2;  
    info->stp_state_1 = BCM_STG_STP_FORWARD;
    info->stp_state_2 = BCM_STG_STP_BLOCK;
    rv = bcm_stg_create(unit,&(info->stg));
}

int
stg(int unit, stg_info_s *info) {
    int rv;
    bcm_pbmp_t pbmp1;
    bcm_pbmp_t pbmp2;
    
    /* create the two vlans*/    
    rv = bcm_vlan_create(unit,info->vlan_1);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_create, vid=%d, \n", info->vlan_1);
        return rv;
    }

    rv = bcm_vlan_create(unit,info->vlan_2);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_create, vid=%d, \n", info->vlan_2);
        return rv;
    }

    BCM_PBMP_CLEAR(pbmp1);
    BCM_PBMP_CLEAR(pbmp2);
    BCM_PBMP_PORT_ADD(pbmp1, info->port_1);
    BCM_PBMP_PORT_ADD(pbmp2, info->port_2);
           
    rv= bcm_vlan_port_add(unit, info->vlan_1, pbmp1, pbmp1);
    if (BCM_FAILURE(rv)) {
        printf("BCM FAIL %d: %s\n", rv, bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_vlan_port_add(unit, info->vlan_2, pbmp2, pbmp2);
    if (BCM_FAILURE(rv)) {
        printf("BCM FAIL %d: %s\n", rv, bcm_errmsg(rv));
        return rv;
    }
      
    /* add the vlans to the stg */
    rv = bcm_stg_vlan_add(unit,info->stg,info->vlan_1);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_stg_vlan_add, vid=%d, \n", info->vlan_1);
        return rv;
    }
    rv = bcm_stg_vlan_add(unit,info->stg,info->vlan_2);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_stg_vlan_add, vid=%d, \n", info->vlan_2);
        return rv;
    }
    
    /* add two ports to the stg and attach stp state for each of them */
    rv = bcm_stg_stp_set(unit,info->stg,info->port_1,info->stp_state_1);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_stg_stp_set, stg=%d, \n", info->stg);
        return rv;
    }
    rv = bcm_stg_stp_set(unit,info->stg,info->port_2,info->stp_state_2);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_stg_stp_set, stg=%d, \n", info->stg);
        return rv;
    }    
    
    return BCM_E_NONE;    
}

/*
 * this function should be called if the user wants to destroy the stg and vlans that were created in main()
 */
int
revert_stg(int unit) {
    int rv;    
    
    rv = bcm_stg_destroy(unit,info.stg);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_stg_destroy\n");
        return rv;
    }
      
    rv = bcm_vlan_destroy(unit,info.vlan_1);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_destroy, vid=%d, \n", info.vlan_1);
        return rv;
    }
      
    rv = bcm_vlan_destroy(unit,info.vlan_2);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_destroy, vid=%d, \n", info.vlan_2);
        return rv;
    }    
}



int main(int unit, int port1,int port2) {
  
  init(unit, &info, port1, port2);
  return stg(unit, &info);
}



/*
 *Add test case to verify Spanning tree port state in certain sequence
 *   step1: create stag and set status to block
 *   step2: delete stag
 *   step3: recreate that stag, the status should be forwarding
*/
int  stg_verify(int unit, int port1,int port2)  
{
     int stg=2; 
     int state; 
     int vlan=100; 
     int port_1=port1; 
     int port_2=port2; 
     int rv= BCM_E_NONE;
     bcm_pbmp_t pbmp; 
     bcm_pbmp_t u_pbmp; 
     
     printf("in stg_verify, port1 =%d, port2 %d, \n", port1, port2);
     
     rv = bcm_vlan_create(unit, vlan); 
     if (rv != BCM_E_NONE) {
         printf("Error, in bcm_vlan_create, rv=%d, \n", rv);
         return rv;
     }
     BCM_PBMP_CLEAR(pbmp); 
     BCM_PBMP_CLEAR(u_pbmp); 
     BCM_PBMP_PORT_ADD(pbmp, port1); 
     BCM_PBMP_PORT_ADD(pbmp, port2); 
     rv = bcm_vlan_port_add(unit, vlan, pbmp, u_pbmp); 
     if (rv != BCM_E_NONE) {
         printf("Error, in bcm_vlan_port_add, rv=%d, \n",rv);
         return rv;
     }
    
    rv = bcm_stg_create_id(unit, stg); 
    if (rv != BCM_E_NONE) {
       printf("Error, in bcm_stg_create_id, rv=%d, \n",rv);
       return rv;
    }
    rv = bcm_stg_vlan_add(unit, stg, vlan); 
    if (rv != BCM_E_NONE) {
       printf("Error, in bcm_stg_vlan_add, rv=%d, \n",rv);
       return rv;
    }
    
    /* Set state to block and check SW and HW config 
    SW = BCM_STG_STP_BLOCK(1), HW=SOC_PPC_PORT_STP_STATE_FLD_VAL_BLOCK(2) */
    rv = bcm_stg_stp_set(unit, stg, port1, BCM_STG_STP_BLOCK); 
    if (rv != BCM_E_NONE) {
       printf("Error, in bcm_stg_stp_set, rv=%d, \n", rv);
       return rv;
    }
    
    /* Set state to block and check SW and HW config after deleting and recreating the STG
    SW and HW should be forwarding status, but it was out of sync in SDK-68531
    In this case, traffic should  be forwarded to another port.*/
    rv = bcm_stg_vlan_remove(unit, stg, vlan); 
    if (rv != BCM_E_NONE) {
       printf("Error, in bcm_stg_vlan_remove, rv=%d, \n", rv);
       return rv;
    }
    
    rv = bcm_stg_destroy(unit, stg); 
    if (rv != BCM_E_NONE) {
      printf("Error, in bcm_stg_destroy, rv=%d, \n",rv);
      return rv;
    }
    
    rv = bcm_stg_create_id(unit, stg); 
    if (rv != BCM_E_NONE) {
       printf("Error, in bcm_stg_create_id, rv=%d, \n",rv);
       return rv;
    }
    
    rv = bcm_stg_vlan_add(unit, stg, vlan); 
    if (rv != BCM_E_NONE) {
       printf("Error, in bcm_stg_vlan_add, rv=%d, \n",rv);
       return rv;
    }
    
    return rv;
}

int  stg_verify_clean(int unit, int port1,int port2)  
{
    int vlan=100; 
    int stg=2; 
    int port_1=port1; 
    int port_2=port2; 
    int rv= BCM_E_NONE;
    bcm_pbmp_t pbmp; 
    bcm_pbmp_t u_pbmp; 
    
    BCM_PBMP_CLEAR(pbmp); 
    BCM_PBMP_CLEAR(u_pbmp); 
    BCM_PBMP_PORT_ADD(pbmp, port1); 
    BCM_PBMP_PORT_ADD(pbmp, port2);
    
    rv = bcm_stg_vlan_remove(unit, stg, vlan); 
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_stg_vlan_remove, rv=%d, \n", rv);
        return rv;
    }
    rv = bcm_vlan_port_remove(unit,vlan,pbmp);
    if (rv != BCM_E_NONE){
        printf("Error, bcm_vlan_port_remove ,rv %d\n", rv);
        return rv;
    }
    rv = bcm_vlan_destroy(unit, vlan);
    if (rv != BCM_E_NONE){
        printf("Error, bcm_vlan_destroy ,rv %d\n", rv);
        return rv;
    }

    rv = bcm_stg_destroy(unit, stg); 
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_stg_destroy, rv=%d, \n",rv);
        return rv;
    }
    return rv;
}


