/* 
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * DFE configure FIFOs example:
 *  
 * The test generate four types of fifo_type, 
 * it configure thresholds for each type and attach links 
 * to the fifo_types 
 *  
 * At the end the test validate configuration 
 */


/*create fifo_types*/
int
configure_fifo_types(int unit, int* fifo_type_rx_prim, int* fifo_type_rx_scnd, int* fifo_type_tx, int* fifo_type_all) {
    
    int rv;

    /*create FIFO type for RX Primary pipe*/
    rv = bcm_fabric_link_threshold_add(unit, BCM_FABRIC_LINK_THRESHOLD_RX_FIFO_ONLY | BCM_FABRIC_LINK_TH_PRIM_ONLY, fifo_type_rx_prim);
    if (rv != BCM_E_NONE) {
          printf("Error, in bcm_fabric_link_threshold_add BCM_FABRIC_LINK_THRESHOLD_RX_FIFO_ONLY | BCM_FABRIC_LINK_TH_PRIM_ONLY, rv=%d, \n", rv);
          return rv;
      }

    /*create FIFO type for RX Secondary pipe*/
    rv = bcm_fabric_link_threshold_add(unit, BCM_FABRIC_LINK_THRESHOLD_RX_FIFO_ONLY | BCM_FABRIC_LINK_TH_SCND_ONLY, fifo_type_rx_scnd);

    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_fabric_link_threshold_add BCM_FABRIC_LINK_THRESHOLD_RX_FIFO_ONLY | BCM_FABRIC_LINK_TH_SCND_ONLY, rv=%d, \n", rv);
        return rv;
    }


    /*create FIFO type for TX Primary and Secondary pipes*/
    rv = bcm_fabric_link_threshold_add(unit, BCM_FABRIC_LINK_THRESHOLD_TX_FIFO_ONLY, fifo_type_tx);
    if (rv != BCM_E_NONE) {
          printf("Error, in bcm_fabric_link_threshold_add BCM_FABRIC_LINK_THRESHOLD_TX_FIFO_ONLY, rv=%d, \n", rv);
          return rv;
      }

    /*create FIFO which configure all FIFos*/
    rv = bcm_fabric_link_threshold_add(unit, 0, fifo_type_all);
    if (rv != BCM_E_NONE) {
          printf("Error, in bcm_fabric_link_threshold_add , rv=%d, \n", rv);
          return rv;
      }

    return BCM_E_NONE;
}

/*set thresholds for fifo_types*/
int
set_thresholds_to_fifo_types(int unit, int fifo_type_rx_prim, int fifo_type_rx_scnd, int fifo_type_tx, int fifo_type_all) {

    int rv;
    bcm_fabric_link_threshold_type_t types[17]; 
    int values[17];

    /*set thresholds for fifo_type_rx_prim*/
    types[0] = bcmFabricLinkRxFifoLLFC;         values[0] = 30;
    types[1] = bcmFabricLinkRxGciLvl1FC;        values[1] = 30;
    types[2] = bcmFabricLinkRxGciLvl2FC;        values[2] = 30;
    types[3] = bcmFabricLinkRxGciLvl3FC;        values[3] = 30;
    types[4] = bcmFabricLinkRxPrio0Drop;        values[4] = 30;
    types[5] = bcmFabricLinkRxPrio1Drop;        values[5] = 30;
    types[6] = bcmFabricLinkRxPrio2Drop;        values[6] = 30;
    types[7] = bcmFabricLinkRxPrio3Drop;        values[7] = 30;
    rv = bcm_fabric_link_thresholds_set(unit, fifo_type_rx_prim, 8, types, values);
    if (rv != BCM_E_NONE) {
          printf("Error, in bcm_fabric_link_thresholds_set for fifo_type_rx_prim, rv=%d, \n", rv);
          return rv;
      }

    /*set thresholds for fifo_type_rx_scnd*/
    types[0] = bcmFabricLinkRxFifoLLFC;         values[0] = 25;
    types[1] = bcmFabricLinkRxGciLvl1FC;        values[1] = 25;
    types[2] = bcmFabricLinkRxGciLvl2FC;        values[2] = 25;
    types[3] = bcmFabricLinkRxGciLvl3FC;        values[3] = 25;
    types[4] = bcmFabricLinkRxPrio0Drop;        values[4] = 25;
    types[5] = bcmFabricLinkRxPrio1Drop;        values[5] = 25;
    types[6] = bcmFabricLinkRxPrio2Drop;        values[6] = 25;
    types[7] = bcmFabricLinkRxPrio3Drop;        values[7] = 25;
    rv = bcm_fabric_link_thresholds_set(unit, fifo_type_rx_scnd, 8, types, values);
    if (rv != BCM_E_NONE) {
          printf("Error, in bcm_fabric_link_thresholds_set for fifo_type_rx_scnd, rv=%d, \n", rv);
          return rv;
      }

    /*set thresholds for fifo_type_tx*/
    types[0] = bcmFabricLinkFE1TxBypassLLFC;    values[0] = 27;
    types[1] = bcmFabricLinkRciFC;              values[1] = 27;
    types[2] = bcmFabricLinkTxGciLvl1FC;        values[2] = 27;
    types[3] = bcmFabricLinkTxGciLvl2FC;        values[3] = 27;
    types[4] = bcmFabricLinkTxGciLvl3FC;        values[4] = 27;
    types[5] = bcmFabricLinkTxPrio0Drop;        values[5] = 27;
    types[6] = bcmFabricLinkTxPrio1Drop;        values[6] = 27;
    types[7] = bcmFabricLinkTxPrio2Drop;        values[7] = 27;
    types[8] = bcmFabricLinkTxPrio3Drop;        values[8] = 27;
    rv = bcm_fabric_link_thresholds_set(unit, fifo_type_tx, 9, types, values);
    if (rv != BCM_E_NONE) {
          printf("Error, in bcm_fabric_link_thresholds_set for fifo_type_tx, rv=%d, \n", rv);
          return rv;
      }

    /*set thresholds for fifo_type_all*/
    types[0] = bcmFabricLinkRxFifoLLFC;         values[0] = 22;
    types[1] = bcmFabricLinkFE1TxBypassLLFC;    values[1] = 22;
    types[2] = bcmFabricLinkRciFC;              values[2] = 22;
    types[3] = bcmFabricLinkTxGciLvl1FC;        values[3] = 22;
    types[4] = bcmFabricLinkTxGciLvl2FC;        values[4] = 22;
    types[5] = bcmFabricLinkTxGciLvl3FC;        values[5] = 22;
    types[6] = bcmFabricLinkRxGciLvl1FC;        values[6] = 22;
    types[7] = bcmFabricLinkRxGciLvl2FC;        values[7] = 22;
    types[8] = bcmFabricLinkRxGciLvl3FC;        values[8] = 22;
    types[9] = bcmFabricLinkTxPrio0Drop;        values[9] = 22;
    types[10] = bcmFabricLinkTxPrio1Drop;       values[10] = 22;
    types[11] = bcmFabricLinkTxPrio2Drop;       values[11] = 22;
    types[12] = bcmFabricLinkTxPrio3Drop;       values[12] = 22;
    rv = bcm_fabric_link_thresholds_set(unit, fifo_type_all, 13, types, values);
    if (rv != BCM_E_NONE) {
          printf("Error, in bcm_fabric_link_thresholds_set for fifo_type_all, rv=%d, \n", rv);
          return rv;
      }

    return BCM_E_NONE;
}

/*attach links to fifo_types*/
int
attach_links_to_fifo_types(int unit, int fifo_type_rx_prim, int fifo_type_rx_scnd, int fifo_type_tx, int fifo_type_all){

    int rv;
    bcm_port_t links[64];
    int i;

    /*attach links 0-63 with fifo_type_rx_prim, fifo_type_rx_scnd, and fifo_type_tx*/
    for(i=0 ; i<64 ; i++) {
        links[i] = i;
    }

    rv = bcm_fabric_link_thresholds_attach(unit, fifo_type_rx_prim, 64, links);
    if (rv != BCM_E_NONE) {
          printf("Error, in bcm_fabric_link_thresholds_attach for fifo_type_rx_prim, rv=%d, \n", rv);
          return rv;
      }

    rv = bcm_fabric_link_thresholds_attach(unit, fifo_type_rx_scnd, 64, links);
    if (rv != BCM_E_NONE) {
          printf("Error, in bcm_fabric_link_thresholds_attach for fifo_type_rx_scnd, rv=%d, \n", rv);
          return rv;
      }

    rv = bcm_fabric_link_thresholds_attach(unit, fifo_type_tx, 64, links);
    if (rv != BCM_E_NONE) {
          printf("Error, in bcm_fabric_link_thresholds_attach for fifo_type_tx, rv=%d, \n", rv);
          return rv;
      }

    /*attach links 64-127 with fifo_type_all*/
    for(i=64 ; i<128 ; i++) {
        links[i-64] = i;
    }

    rv = bcm_fabric_link_thresholds_attach(unit, fifo_type_all, 64, links);
    if (rv != BCM_E_NONE) {
          printf("Error, in bcm_fabric_link_thresholds_attach for fifo_type_all, rv=%d, \n", rv);
          return rv;
      }

    return BCM_E_NONE;    
}

int
validate_thresholds(int unit, int fifo_type_rx_prim, int fifo_type_rx_scnd, int fifo_type_tx, int fifo_type_all) {
    int rv, i;
    bcm_fabric_link_threshold_type_t types[17]; 
    int values[17];

    /*validate fifo_type_rx_prim*/
    types[0] = bcmFabricLinkRxFifoLLFC;    
    types[1] = bcmFabricLinkRxGciLvl1FC;   
    types[2] = bcmFabricLinkRxGciLvl2FC;     
    types[3] = bcmFabricLinkRxGciLvl3FC;     
    types[4] = bcmFabricLinkRxPrio0Drop;  
    types[5] = bcmFabricLinkRxPrio1Drop; 
    types[6] = bcmFabricLinkRxPrio2Drop;      
    types[7] = bcmFabricLinkRxPrio3Drop;     
    rv = bcm_fabric_link_thresholds_get(unit, fifo_type_rx_prim, 8, types, values);
    if (rv != BCM_E_NONE) {
          printf("Error, in bcm_fabric_link_thresholds_get for fifo_type_rx_prim, rv=%d, \n", rv);
          return rv;
      }

    for(i=0 ; i<8 ; i++) {
        if(values[i] != 30 /*the set value*/) {
            printf("Error, in bcm_fabric_link_thresholds_get for fifo_type_rx_prim return value not match index %d value %d, rv=%d, \n", i, values[i], rv);
            return BCM_E_FAIL;
        }
    }

    /*validate fifo_type_rx_scnd*/
    types[0] = bcmFabricLinkRxFifoLLFC;     
    types[1] = bcmFabricLinkRxGciLvl1FC;      
    types[2] = bcmFabricLinkRxGciLvl2FC;   
    types[3] = bcmFabricLinkRxGciLvl3FC;  
    types[4] = bcmFabricLinkRxPrio0Drop; 
    types[5] = bcmFabricLinkRxPrio1Drop;  
    types[6] = bcmFabricLinkRxPrio2Drop;    
    types[7] = bcmFabricLinkRxPrio3Drop;    
    rv = bcm_fabric_link_thresholds_get(unit, fifo_type_rx_scnd, 8, types, values);
    if (rv != BCM_E_NONE) {
          printf("Error, in bcm_fabric_link_thresholds_get for fifo_type_rx_scnd, rv=%d, \n", rv);
          return rv;
      }

    for(i=0 ; i<8 ; i++) {
        if(values[i] != 25 /*the set value*/) {
            printf("Error, in bcm_fabric_link_thresholds_get for fifo_type_rx_scnd return value not match, rv=%d, \n", rv);
            return BCM_E_FAIL;
        }
    }

    /*validate fifo_type_tx*/
    types[0] = bcmFabricLinkFE1TxBypassLLFC; 
    types[1] = bcmFabricLinkRciFC;         
    types[2] = bcmFabricLinkTxGciLvl1FC;     
    types[3] = bcmFabricLinkTxGciLvl2FC;      
    types[4] = bcmFabricLinkTxGciLvl3FC;        
    types[5] = bcmFabricLinkTxPrio0Drop;       
    types[6] = bcmFabricLinkTxPrio1Drop;    
    types[7] = bcmFabricLinkTxPrio2Drop;       
    types[8] = bcmFabricLinkTxPrio3Drop ;     
    rv = bcm_fabric_link_thresholds_get(unit, fifo_type_tx, 9, types, values);
    if (rv != BCM_E_NONE) {
          printf("Error, in bcm_fabric_link_thresholds_get for fifo_type_tx, rv=%d, \n", rv);
          return rv;
      }

    for(i=0 ; i<9 ; i++) {
        if(values[i] != 27 /*the set value*/) {
            printf("values: actual: %d, expected: %d, i= %d\n", values[i], 27,i);
            printf("Error, in bcm_fabric_link_thresholds_get for fifo_type_tx return value not match, rv=%d, \n", rv);
            return BCM_E_FAIL;
        }
    }

    /*validate fifo_type_all*/
    types[0] = bcmFabricLinkRxFifoLLFC;       
    types[1] = bcmFabricLinkFE1TxBypassLLFC;  
    types[2] = bcmFabricLinkRciFC;           
    types[3] = bcmFabricLinkTxGciLvl1FC;  
    types[4] = bcmFabricLinkTxGciLvl2FC;   
    types[5] = bcmFabricLinkTxGciLvl3FC;        
    types[6] = bcmFabricLinkRxGciLvl1FC;  
    types[7] = bcmFabricLinkRxGciLvl2FC;    
    types[8] = bcmFabricLinkRxGciLvl3FC;     
    types[9] = bcmFabricLinkTxPrio0Drop; 
    types[10] = bcmFabricLinkTxPrio1Drop;     
    types[11] = bcmFabricLinkTxPrio2Drop;    
    types[12] = bcmFabricLinkTxPrio3Drop;            
    rv = bcm_fabric_link_thresholds_get(unit, fifo_type_all, 13, types, values);
    if (rv != BCM_E_NONE) {
          printf("Error, in bcm_fabric_link_thresholds_get for fifo_type_all, rv=%d, \n", rv);
          return rv;
      }

    for(i=0 ; i<13 ; i++) {
        if(values[i] != 22 /*the set value*/) {
            printf("Error, in bcm_fabric_link_thresholds_get for fifo_type_all return value not match, rv=%d, \n", rv);
            return BCM_E_FAIL;
        }
    }

    return BCM_E_NONE;
}

int 
validate_links(int unit, int fifo_type_rx_prim, int fifo_type_rx_scnd, int fifo_type_tx, int fifo_type_all){

    int rv, i;
    bcm_port_t links[128];
    uint32     links_count;


    /*retrive fifo_type_rx_prim links*/
    rv = bcm_fabric_link_thresholds_retrieve(unit, fifo_type_rx_prim, 128, links, &links_count);
    if (rv != BCM_E_NONE) {
          printf("Error, in bcm_fabric_link_thresholds_retrieve for fifo_type_rx_prim, rv=%d, \n", rv);
          return rv;
      }
    if(links_count != 64) {
        printf("Error, in bcm_fabric_link_thresholds_retrieve fifo_type_rx_prim wrong links_count %d, rv=%d, \n", links_count, rv);
          return BCM_E_FAIL;
    }
    for(i=0 ; i<links_count ; i++) {
        if(links[i] > 63 || links[i]<0) {
            printf("Error, in bcm_fabric_link_thresholds_retrieve fifo_type_rx_prim wrong link %d, rv=%d, \n", links[i] , rv);
            return BCM_E_FAIL;
        }
    }

    /*retrive fifo_type_rx_scnd links*/
    rv = bcm_fabric_link_thresholds_retrieve(unit, fifo_type_rx_scnd, 128, links, &links_count);
    if (rv != BCM_E_NONE) {
          printf("Error, in bcm_fabric_link_thresholds_retrieve for fifo_type_rx_scnd, rv=%d, \n", rv);
          return rv;
      }
    if(links_count != 64) {
        printf("Error, in bcm_fabric_link_thresholds_retrieve fifo_type_rx_scnd wrong links_count %d, rv=%d, \n", links_count, rv);
          return BCM_E_FAIL;
    }
    for(i=0 ; i<links_count ; i++) {
        if(links[i] > 63 || links[i]<0) {
            printf("Error, in bcm_fabric_link_thresholds_retrieve fifo_type_rx_scnd wrong link %d, rv=%d, \n", links[i] , rv);
            return BCM_E_FAIL;
        }
    }

    /*retrive fifo_type_tx links*/
    rv = bcm_fabric_link_thresholds_retrieve(unit, fifo_type_tx, 128, links, &links_count);
    if (rv != BCM_E_NONE) {
          printf("Error, in bcm_fabric_link_thresholds_retrieve for fifo_type_tx, rv=%d, \n", rv);
          return rv;
      }
    if(links_count != 64) {
        printf("Error, in bcm_fabric_link_thresholds_retrieve fifo_type_tx wrong links_count %d, rv=%d, \n", links_count, rv);
          return BCM_E_FAIL;
    }
    for(i=0 ; i<links_count ; i++) {
        if(links[i] > 63 || links[i]<0) {
            printf("Error, in bcm_fabric_link_thresholds_retrieve fifo_type_tx link %d, rv=%d, \n", links[i] , rv);
            return BCM_E_FAIL;
        }
    }

    /*retrive fifo_type_all links*/
    rv = bcm_fabric_link_thresholds_retrieve(unit, fifo_type_all, 128, links, &links_count);
    if (rv != BCM_E_NONE) {
          printf("Error, in bcm_fabric_link_thresholds_retrieve for fifo_type_all, rv=%d, \n", rv);
          return rv;
      }
    if(links_count != 64) {
        printf("Error, in bcm_fabric_link_thresholds_retrieve fifo_type_all wrong links_count %d, rv=%d, \n", links_count, rv);
          return BCM_E_FAIL;
    }
    for(i=0 ; i<links_count ; i++) {
        if(links[i] > 127 || links[i]<64) {
            printf("Error, in bcm_fabric_link_thresholds_retrieve fifo_type_all link %d, rv=%d, \n", links[i] , rv);
            return BCM_E_FAIL;
        }
    }

    return BCM_E_NONE;

}

/*delete fifo_types*/
int
delete_fifo_types(int unit, int fifo_type_rx_prim, int fifo_type_rx_scnd, int fifo_type_tx, int fifo_type_all){

    int rv;

    /*delete fifo_type_rx_prim*/
    rv = bcm_fabric_link_threshold_delete(unit, fifo_type_rx_prim);
    if (rv != BCM_E_NONE) {
          printf("Error, in bcm_fabric_link_threshold_delete for fifo_type_rx_prim, rv=%d, \n", rv);
          return rv;
      }

    /*delete fifo_type_rx_scnd*/
    rv = bcm_fabric_link_threshold_delete(unit, fifo_type_rx_scnd);
    if (rv != BCM_E_NONE) {
          printf("Error, in bcm_fabric_link_threshold_delete for fifo_type_rx_scnd, rv=%d, \n", rv);
          return rv;
      }

    /*delete fifo_type_all*/
    rv = bcm_fabric_link_threshold_delete(unit, fifo_type_tx);
    if (rv != BCM_E_NONE) {
          printf("Error, in bcm_fabric_link_threshold_delete for fifo_type_tx, rv=%d, \n", rv);
          return rv;
      }

    /*delete fifo_type_all*/
    rv = bcm_fabric_link_threshold_delete(unit, fifo_type_all);
    if (rv != BCM_E_NONE) {
          printf("Error, in bcm_fabric_link_threshold_delete for fifo_type_all, rv=%d, \n", rv);
          return rv;
      }

    return BCM_E_NONE;

    
}

int main(int unit) {
  int fifo_type_rx_prim = 0,
        fifo_type_rx_scnd = 0,
        fifo_type_tx = 0, 
        fifo_type_all = 0,
        rv;
  int fail = 0;

  rv = configure_fifo_types(unit, &fifo_type_rx_prim, &fifo_type_rx_scnd, &fifo_type_tx, &fifo_type_all);
  if (rv!=BCM_E_NONE) {
      printf("ERROR: cint_fifos - configure_fifo_types failed\n");
      fail = 1;
  }
  rv = set_thresholds_to_fifo_types(unit, fifo_type_rx_prim, fifo_type_rx_scnd, fifo_type_tx, fifo_type_all);
  if (rv!=BCM_E_NONE) {
      printf("ERROR: cint_fifos - set_thresholds_to_fifo_types failed\n");
      fail = 1;
  }
  rv = attach_links_to_fifo_types(unit, fifo_type_rx_prim, fifo_type_rx_scnd, fifo_type_tx, fifo_type_all);
  if (rv!=BCM_E_NONE) {
      printf("ERROR: cint_fifos - attach_links_to_fifo_types failed\n");
      fail = 1;
  }
  rv = validate_thresholds(unit, fifo_type_rx_prim, fifo_type_rx_scnd, fifo_type_tx, fifo_type_all);
  if (rv!=BCM_E_NONE) {
      printf("ERROR: cint_fifos - validate_thresholds failed\n");
      fail = 1;
  }
  rv = validate_links(unit, fifo_type_rx_prim, fifo_type_rx_scnd, fifo_type_tx, fifo_type_all);
  if (rv!=BCM_E_NONE) {
      printf("ERROR: cint_fifos - validate_thresholds failed\n");
      fail = 1;
  }

  rv = delete_fifo_types(unit, fifo_type_rx_prim, fifo_type_rx_scnd, fifo_type_tx, fifo_type_all);
  if (rv!=BCM_E_NONE) {
      printf("ERROR: cint_fifos - delete_fifo_types failed\n");
      fail = 1;
  }

  if (fail) {
      printf("cint_fifos: FAILED\n");
      return BCM_E_FAIL;
  }
  printf("cint_fifos: PASS\n");

  return BCM_E_NONE;
}

