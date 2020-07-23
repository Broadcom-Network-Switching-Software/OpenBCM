/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 *  
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~Traditional Bridge: Port Egress filter~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * File: cint_port_eg_filter_example.c 
 * Purpose: Example of how to set the Port Egress filter for Unknown UC / Unknown MC / BC 
 *  
 * Default settings:
 * It is assumed diag_init is executed:
 *  o    All ports set with TPID 0x8100 and all packet formats (tagged/untagged) are accepted.
 *  o    PVID of all ports set to VID = 1, thus untagged packet is associated to VLAN 1.
 *  o    All VLANs are mapped to Default STG (0).
 *  o    STP state of all ports in default topology is set to forwarding.
 * 
 *  
 * Usage: 
 *  o    port_eg_filter_set(unit,eg_port,disable_types): Set disable filter types on Egress port.
 *  disable_types according to port_control types: BCM_PORT_CONTROL_FILTER_DISABLE_*
 *  o    trap_drop_eg_filter_set(unit,enable): Enable , Disable drop of egress filter type.
 *  
 *  o    trap_drop_eg_filter_destroy(unit): Destroy trap handle
 *  
 * Specific example: 
 * Filter Unknown DA on egress port 14 
 *  port_eg_filter_example(unit,14,0); 
 * Send Packet: 
 * Unknown DA packet : VLAN 1 DA : 1 
 * Expects flooding to ethernet ports except 14 
 */

int trap_id_handle = -1;

int
port_eg_filter_set(int unit, int egress_port, int disable_types){
    int rv;
    
    rv = bcm_port_control_set(unit,egress_port,bcmPortControlEgressFilterDisable,disable_types);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_control_set, port=%d, \n", egress_port);
        return rv;
    }    
    
    return rv;
}

int
trap_drop_eg_filter_set(int unit, int enable){
    int rv;
    int trap_id;
    bcm_rx_trap_config_t config;

    bcm_rx_trap_t trap_type = bcmRxTrapEgUnknownDa; /* Trap for Unknown DA / Unknown MC / BC */

    bcm_rx_trap_config_t_init(&config);
    config.flags |= (BCM_RX_TRAP_UPDATE_DEST);
    config.dest_port = BCM_GPORT_BLACK_HOLE;

    if (trap_id_handle == -1) {
      rv = bcm_rx_trap_type_create(unit,0x0,trap_type,&trap_id);
      if (rv != BCM_E_NONE) {
          printf("Error, in bcm_rx_trap_type_create\n");
          return rv;
      }    

       trap_id_handle = trap_id; /*Update trap id handle*/
    } else {
       trap_id = trap_id_handle;
    }
   

    rv = bcm_rx_trap_set(unit,trap_id,&config);
    if (rv != BCM_E_NONE) {
        printf("Error, in trap set \n");
        return rv;
    }
    
    return rv;
}

int trap_drop_eg_filter_destroy(int unit)
{
    int rv;
    int trap_id = trap_id_handle;
    bcm_rx_trap_config_t config;

    bcm_rx_trap_t trap_type = bcmRxTrapEgUnknownDa; /* Trap for Unknown DA / Unknown MC / BC */

    if (trap_id != -1) {
        rv = bcm_rx_trap_type_destroy(unit,trap_id);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_rx_trap_type_create\n");
            return rv;
        } 
    }

    return rv;
}

/* An example of filter according to type */
/* type 0 - enable only Unknown DA , 1 - enable only Unknown MC, 2 - enable only BC */
int port_eg_filter_example(int unit, int eg_port, int type)
{
  int rv;
  int value;

  if (type == 0) {
    value = BCM_PORT_CONTROL_FILTER_DISABLE_UNKNOWN_DA_MC | BCM_PORT_CONTROL_FILTER_DISABLE_DA_BC;
  } else if (type == 1) {
    value = BCM_PORT_CONTROL_FILTER_DISABLE_UNKNOWN_DA_UC | BCM_PORT_CONTROL_FILTER_DISABLE_DA_BC;
  } else {
    value = BCM_PORT_CONTROL_FILTER_DISABLE_UNKNOWN_DA_MC | BCM_PORT_CONTROL_FILTER_DISABLE_UNKNOWN_DA_UC;
  }

  rv = port_eg_filter_set(unit,eg_port,value);
  if (rv != BCM_E_NONE) {
      printf("Error, in port_eg_filter_set\n");
      return rv;
  } 

  rv = trap_drop_eg_filter_set(unit,1);
  if (rv != BCM_E_NONE) {
      printf("Error, in trap_drop_eg_filter_set\n");
      return rv;
  } 

  return rv;
}
