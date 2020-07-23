/* 
 *  
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *  
 *  
 *  
 *  
 * Example walkthrough: 
 *  
 * This example shows how to configure a system of a single FE and multiple faps to replicate mc packets in the fabric and the fap's egress. 
 *  
 * The FE function configs the fabric replicate data to all FAPs which have a gport included in the gport array, gport_arr. 
 *  
 * The FAP function configs the FAP to replicate data to all the ports in gport_arr which are in the FAP. 
 *
 * cint_fabric_egress_mc_config shows an example of usage of the functions for 1 FE and 2 FAPs
 *  
 *  NOTEs :
 *      1. gports recived by FE and FAP funcitons are expected to be MODPORTs.
 *      2. The example assumes FE is configure to direct mode.
 *
 *
 *
 * Usage example : 
 * 
    cd ../../../../src/examples/dpp

    cint validate/cint_pp_util.c
    cint ../dfe/cint_fe3200_interop.c

    cd ../dcmn

    cint cint_fabric_egress_mc.c

    c

    print cint_fe3200_interop_example(1);

    int fap_0_unit = 0;
    int fap_0_id = 0;
    int fap_0_in_port = 14;

    int fap_1_unit = 2;
    int fap_1_id = 2;

    int fe_unit = 1;

    int nof_ports_fap_0 = 5;
    int ports_fap_0[nof_ports_fap_0] = {13,14,15,16,17};

    int nof_ports_fap_1 = 5;
    int ports_fap_1[nof_ports_fap_1] = {13,14,15,16,17};

    bcm_multicast_t mc_id=5007;

    bcm_gport_t gport;
    BCM_GPORT_MCAST_SET(gport, mc_id);
    print bcm_port_force_forward_set(fap_0_unit, fap_0_in_port, gport, 1);

    print cint_fabric_egress_mc_config( fap_0_unit,  fap_0_id,  fap_0_in_port,   fap_1_unit,  fap_1_id,  fe_unit, nof_ports_fap_0,  ports_fap_0,  nof_ports_fap_1,  ports_fap_1,   mc_id );
 */

int cint_fabric_egress_mc_config( int fap_0_unit, int fap_0_id, int fap_0_in_port,  int fap_1_unit, int fap_1_id, int fe_unit, 
                                 int nof_ports_fap_0, int *ports_fap_0, int nof_ports_fap_1, int *ports_fap_1, bcm_multicast_t mc_id )
{
    int rv, i, j;
    int nof_gports = nof_ports_fap_0 + nof_ports_fap_1;
    bcm_gport_t garr[nof_gports];

    if (nof_gports == 0) {
        printf("No destination ports defined\n");
        return rv;
    }
    
    for (i=0; i < nof_gports; i++) {
        if (i < nof_ports_fap_0) {
            BCM_GPORT_MODPORT_SET(garr[i], fap_0_id, ports_fap_0[i]); 
        } else {
            BCM_GPORT_MODPORT_SET(garr[i], fap_1_id, ports_fap_1[i - nof_ports_fap_0]); 
        }
    }


    rv =  fabric_egress_mc_fap(fap_0_unit, mc_id, garr, nof_gports);
    if (rv != BCM_E_NONE) {
        printf("Error, fabric_egress_mc_fap (%d)\n",fap_0_unit);
        return rv;
    } 

    rv = fabric_egress_mc_fap(fap_1_unit, mc_id, garr, nof_gports);
    if (rv != BCM_E_NONE) {
        printf("Error, fabric_egress_mc_fap (%d)\n",fap_1_unit);
        return rv;
    } 

    rv = fabric_egress_mc_fe(fe_unit, mc_id,  garr, nof_gports);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_addr_add (%d)\n",fap_0_unit);
        return rv;
    } 


    printf("cint_fabric_egress_mc_config: PASS\n\n");
}


int fabric_egress_mc_fe(int fe_unit, bcm_multicast_t mc_id,
                                   bcm_gport_t *gport_arr, int nof_gports)
{
    int rv;
    int i, j, tmp, new_modid, nof_modules = 0;
    bcm_module_t modid;

    bcm_gport_t;

    bcm_module_t modid_array[nof_gports];




    /*gports to modid's - 
      use gport array to create a an array of destenetion modid's */
    if (nof_gports > 0) {
        modid_array[0] = BCM_GPORT_MODPORT_MODID_GET(gport_arr[0]);
        nof_modules++;
    }

    for (i=1; i<nof_gports; i++) {
        modid = BCM_GPORT_MODPORT_MODID_GET(gport_arr[i]);
        new_modid = 1;

        for (j = 0; j < nof_modules; j++) {
            if (modid == modid_array[j]) {
                new_modid = 0;
            }
        }
 
        if (new_modid == 1) {
            modid_array[nof_modules] = modid;
            nof_modules++;
        }
    } 

    /* 
     * Given multicast id, create and replicate multicast id over all requested devices. 
     */

    /* map local modid with modid */
    for (j = 0; j < nof_modules; j++)
    {
        tmp = j;
        BCM_FABRIC_LOCAL_MODID_SET(tmp);
        rv = bcm_fabric_modid_local_mapping_set(fe_unit, tmp, modid_array[j]);
        if(rv != 0) {
            printf("Error, in bcm_fabric_modid_local_mapping_set, rv=%d, \n", rv);
            return rv;
        }
    } 

    /* 
     * Create multicast id, and set fabric to it.
     */

    rv = bcm_multicast_create(fe_unit, BCM_MULTICAST_WITH_ID, &mc_id);
    if(rv != 0) {
        printf("Error, in bcm_multicast_create, rv=%d, \n", rv);
        return rv;
    }

    rv = bcm_fabric_multicast_set(fe_unit, mc_id, 0, nof_modules, modid_array);
    if(rv != 0) {
        printf("Error, in bcm_fabric_multicast_set, rv=%d, \n", rv);
        return rv;
    }

    for (i=0; i<nof_modules ; i++) {
        printf("FE replicating mc id %d to --- FAP ID (%d) ---\n",mc_id,modid_array[i]); 
    }
    
    printf("fabric_egress_mc_fe: PASS\n\n");
    return rv;

}

int is_device_jericho_only(int unit, int *is_jer_only)
{
  bcm_info_t info;

  int rv = bcm_info_get(unit, &info);
  if (rv != BCM_E_NONE) {
      printf("Error in bcm_info_get\n");
      print rv;
      return rv;
  }

  *is_jer_only = (info.device == 0x8675)? 1: 0;
  return rv;
}

int fabric_egress_mc_fap(int fap_unit, bcm_multicast_t mc_id, bcm_gport_t *gport_arr, int nof_gports)
{
    int rv, i, is_jer_only;
    int mymodid, modid;
    int nof_gports_in_fap = 0;
    bcm_if_t cud_array[nof_gports];

    bcm_gport_t gport_in_fap[nof_gports];

    bcm_stk_modid_get(fap_unit, &mymodid);

    rv = is_device_jericho_only(fap_unit, &is_jer_only);
    if (rv != BCM_E_NONE) {
        printf("Error, is_device_jericho_only, (%d)\n", fap_unit);
        return rv;
    }

    /*gports to modid's - 
      use gport array to create a an array of modid's */
    for (i=0; i<nof_gports; i++) {
        modid = BCM_GPORT_MODPORT_MODID_GET(gport_arr[i]);
        printf("\n\n\n----- modid %d-----\n\n\n",modid);
        if ((modid == mymodid) || (modid == mymodid + 1)) {            
            gport_in_fap[nof_gports_in_fap] = gport_arr[i];
            cud_array[nof_gports_in_fap] = (is_jer_only ? -1 : 1);
            nof_gports_in_fap++;
            printf("\n\n\n------ nof_gport_in_fap %d, gport %d -------------\n\n\n", nof_gports_in_fap - 1, gport_in_fap[nof_gports_in_fap - 1]);
        }
        
    } 


    if (nof_gports_in_fap > 0) {
        

        /* mc_id egress fap_0 to gports */

        rv = bcm_multicast_create(fap_unit, BCM_MULTICAST_WITH_ID | BCM_MULTICAST_EGRESS_GROUP, &mc_id);
        if(rv != 0) {
            printf("Error, in bcm_multicast_create, rv=%d, \n", rv);
            return rv;
        }

        rv = bcm_multicast_egress_set(fap_unit, mc_id, nof_gports_in_fap, gport_in_fap, cud_array);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_multicast_egress_set (%d)\n", fap_unit);
            return rv;
        }

        

    }

    for (i=0; i<nof_gports_in_fap ; i++) {
        printf("--- FAP ID (%d) --- replicating mc id %d to --- port (%d) ---\n",mymodid,mc_id,BCM_GPORT_MODPORT_PORT_GET(gport_in_fap[i])); 
    }
        
    
    printf("fabric_egress_mc_fap: PASS\n\n");
    return rv;
}



int cint_fabric_egress_mc_sanke( int fap_0_unit, int fap_0_id, int fap_0_in_port,  int fap_1_unit, int *fap_1_id, int fe_unit, 
                                 int nof_ports_fap_1, int *ports_fap_1, bcm_multicast_t mc_id_in, bcm_multicast_t mc_id_out, bcm_mac_t mac_address )
{
    int rv, i, j;
    int nof_gports = nof_ports_fap_1;
    bcm_gport_t garr[nof_gports];
    bcm_gport_t gport_fap_0_in;
    int vid_0 = BASE_VID + fap_0_in_port;
    int vid_1;

    BCM_GPORT_MODPORT_SET(gport_fap_0_in, fap_0_id, fap_0_in_port); 

    /* ingress fap_0 to mc_id*/
    rv = l2_addr_add(fap_0_unit, mac_address, vid_0, 0, mc_id_in);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_addr_add (%d)\n",fap_0_unit);
        return rv;
    }




    if (nof_gports == 0) {
        printf("No destination ports defined\n");
        return rv;
    }
    
    for (i=0; i < nof_gports; i++) {

        BCM_GPORT_MODPORT_SET(garr[i], fap_1_id[i], ports_fap_1[i]); 

        vid_1 = BASE_VID + ports_fap_1[i];

        /* fap_1 to mc_id*/
        rv = egr_vlan_edit(fap_1_unit, fap_0_in_port, ports_fap_1[i], vid_0, vid_1);
        if (rv != BCM_E_NONE) {
            printf("Error, egr_vlan_edit (%d)\n",fap_1_unit);
            return rv;
        }
        rv = l2_addr_add(fap_1_unit, mac_address, vid_1, 0, mc_id_out + i);
        if (rv != BCM_E_NONE) {
            printf("Error, l2_addr_add (%d)\n",fap_1_unit);
            return rv;
        }
        /* fap_1 loopback */
        rv = bcm_port_loopback_set(fap_1_unit, ports_fap_1[i], 2);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_loopback_set (%d)\n",fap_1_unit);
            return rv;
        }

        rv =  fabric_egress_mc_fap(fap_0_unit, mc_id_out + i, gport_fap_0_in, 1);
        if (rv != BCM_E_NONE) {
            printf("Error, fabric_egress_mc_fap (%d)\n",fap_0_unit);
            return rv;
        } 

        rv = fabric_egress_mc_fe(fe_unit, mc_id_out + i,  gport_fap_0_in, 1);
        if (rv != BCM_E_NONE) {
             printf("Error, l2_addr_add (%d)\n",fap_0_unit);
             return rv;
        } 

    }




    rv = fabric_egress_mc_fap(fap_1_unit, mc_id_in, garr, nof_gports);
    if (rv != BCM_E_NONE) {
        printf("Error, fabric_egress_mc_fap (%d)\n",fap_1_unit);
        return rv;
    } 

    rv = fabric_egress_mc_fe(fe_unit, mc_id_in,  garr, nof_gports);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_addr_add (%d)\n",fap_0_unit);
        return rv;
    } 


    printf("cint_fabric_egress_mc_sanke: PASS\n\n");
}



