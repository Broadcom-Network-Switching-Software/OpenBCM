/*~~~~~~~~~~~~~~~~~~~~~~~~~~Sracking: MC, 2 TM-domain (1 fap each) ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
 * File: cint_stk_mc.c
 * Purpose: Example Stacking MC Validation
 * Reference: BCM88650 TM Device Driver (Stacking)
 *                        cint_stk_uc.c
 * 
 * Environment:
 *    Assuming UC environment.
 *
 *
 * Adjust to different system configuration:
 *  1. Same as UC adjustments.
 *    2. Multicast Group should contain all Stacking ports, only one copy will pass to other domain, other will be filtered according to LB-Key.
 *  
 * Test Run 1:
 *    1. inject packet from in port. packet format: 
 *         a. eth packet
 *         b. da[4] = mc_id in hex % 256 (mc-id 8000 113 --> 0x0,0x0,0x0,0x0,0x40,0x0)
 *       c. sa changes (for lb-key)
 *       d. vlan tag = 1.
 *  2. Expected Output:
 *       a. according to the configured mc group port members, packet should appear at the output - capture outgoing packet.
 *       b. traffic should be divided on all stacking ports (same lag) - show counters on both TMD.
 *       c. expected output of shell command "show counters" :
 *          (for one packet sent from Tm-domain 0)
 *
 *    on Tm-domain 0:
 *
  *      RX
 *      ---
 *      Port ||       snmpIfHCInUcastPkts |        snmpIfInNUcastPkts |            snmpIfInErrors |  snmpEtherStatsRXNoErrors |
 *      -----------------------------------------------------------------------------------------------------------------------
 *        13 ||                         1 |                         0 |                         0 |                         0 |
 *      TX
 *      ---
 *      Port ||      snmpIfHCOutUcastPkts |       snmpIfOutNUcastPkts |           snmpIfOutErrors |  snmpEtherStatsTXNoErrors |
 *      -----------------------------------------------------------------------------------------------------------------------
 *        16 ||                         1 |                         0 |                         0 |                         1 |
 *        17 ||                         1 |                         0 |                         0 |                         1 |
 *
 *          * packet should leave from port 15 or port 16, selected randomly.
 *
 *   on Tm-domain 1: 
 *
 *      RX
 *      ---
 *      Port ||       snmpIfHCInUcastPkts |        snmpIfInNUcastPkts |            snmpIfInErrors |  snmpEtherStatsRXNoErrors |
 *      -----------------------------------------------------------------------------------------------------------------------
 *        16 ||                         0 |                         1 |                         0 |                         1 |
 *      TX
 *      ---
 *      Port ||      snmpIfHCOutUcastPkts |       snmpIfOutNUcastPkts |           snmpIfOutErrors |  snmpEtherStatsTXNoErrors |
 *      -----------------------------------------------------------------------------------------------------------------------
 *        13 ||                         0 |                         1 |                         0 |                         1 |
 *        17 ||                         1 |                         0 |                         0 |                         1 |
 *
 *           * packet should enter from port 15 or port 16 depend on which port it's leave Tm-domain 0.
 *
 * cd ../../../../src/examples/dpp
 * cint cint_stk_uc.c
 * cint cint_stk_mc.c
 * cint
 * In TMD 0:
 *     stacking_mc_appl(unit, 0);
 * In TMD 1:
 *     stacking_mc_appl(unit, 1);
 *
 *
 * Flooding example:
 *    flooding in Stacking system is done by creating Ingress MC group with mc_id=vlan,
 *    CUD=vlan which contain all the Stacking ports and the ERP port, in all TM-domains.
 *    The packet to ERP port with CUD=vlan will create flooding in the current domain.
 *    Only one of the packets towards the Stacking port will pass to the next TM-domain, 
 *    and will be distributed to Stacking ports (should be filtered by Trap), local ERP port.
 *    This example is based on Stacking UC application, and ERP port bring up.
 *    
 *    Usage:
 *        In TMD 0: stacking_flooding_appl(unit, 0);
 *        In TMD 1: stacking_flooding_appl(unit, 1);
 *
 *
 * Remarks:
 *    1. Read Stacking section in BCM88650 TM Device Driver.
 *    2. The UC application should be called before MC application, as in stacking_mc_appl().
 *    3. Using Load-Balancing Extension should be across TM-Domains.
 *    4. When working without fabric adjust soc property fabric_connect_mode=SINGLE_FAP.
 */
/*#include "cint_stk_uc.c"*/

/*
 * Utility Functions
 */
enum device_type_t { 
    device_type_arad=0,
    device_type_jericho=1
} ;

device_type_t device_type;

/*
 * Get device type
 */
int get_device_type(int unit, device_type_t *device_type)
{
    bcm_info_t info;

    int rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_info_get, rv=%d\n", rv);
        return rv;
    }

    *device_type = ((info.device == 0x8675) || (info.device == 0x8375) || (info.device == 0x8680))? device_type_jericho: device_type_arad;
    return rv;
}


/*
 * Multicast Utility Func
 */
int open_multicast_group(int unit, int mc_id, int is_egress) {
    bcm_error_t rv = BCM_E_NONE;
  int         flags;

  if(is_egress == 0) {
      /* Ingress MC */
    flags = BCM_MULTICAST_WITH_ID|BCM_MULTICAST_INGRESS_GROUP;
  } else if (is_egress == 1) {
      /* Egress MC */
    flags = BCM_MULTICAST_WITH_ID|BCM_MULTICAST_EGRESS_GROUP;
  } else {
    flags = BCM_MULTICAST_WITH_ID|BCM_MULTICAST_EGRESS_GROUP|BCM_MULTICAST_INGRESS_GROUP;
  }  

  rv = bcm_multicast_create(unit, flags, &mc_id);
  if (rv != BCM_E_NONE) {
    printf("Error, in _multicast_create, mc_id mc_id=%d\n", mc_id);
    return rv;
  }

  return rv;
}

int close_multicast_group(int unit, int mc_id) {
  bcm_error_t rv = BCM_E_NONE;

  rv = bcm_multicast_destroy(unit, mc_id);

  if (rv != BCM_E_NONE) {
    printf("Error, in _multicast_close, mc_id=%d, rv=%d\n", mc_id, rv);
    return rv;
  }

  return rv;
}

int add_multicast_replication(int unit, int mc_id, bcm_gport_t gport, int cud, int is_egress) {
  bcm_error_t rv = BCM_E_NONE;

  if(is_egress == 0) {
    rv = bcm_multicast_ingress_add(unit, mc_id, gport, cud);
    if (rv != BCM_E_NONE) {
      printf("Error, in bcm_multicast_ingress_add, mc_id mc_id=%d, destination gport=0x%x\n", mc_id, gport);
      return rv;
    }
  } else {
    rv = bcm_multicast_egress_add(unit, mc_id, gport, cud);
    if (rv != BCM_E_NONE) {
      printf("Error, in bcm_multicast_egress_add, mc_id mc_id=%d, destination gport=0x%x\n", mc_id, gport);
      return rv;
    }
  }

  return rv;
}

/* Create l2 entries 'leading' to MC-id, set MAC according to mc-id */
int l2_addr_add_mc(int unit, uint16 vid, int mc_id) {
    int
            rv = BCM_E_NONE;
    bcm_mac_t 
            da  = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    bcm_l2_addr_t 
        l2addr;

        da[4] = mc_id % 256;
    bcm_l2_addr_t_init(&l2addr, da, vid);

        l2addr.flags = 0x00000020 | BCM_L2_MCAST;
        l2addr.l2mc_group = mc_id;
        
        printf("l2_addr_add_mc(): mc_id=%d,0x%x, da[4]=%d,0x%x\n", mc_id, mc_id, da[4] ,da[4]);
        rv = bcm_l2_addr_add(unit, &l2addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add. rv=%d\n", rv);
        return rv;
    }

    return rv;
}

int stacking_mc_func(int unit, int vlan, int mc_type, int mc_id, int *nxt_mc_id, int mc_member_count, int *mc_port_members)
{
    int 
        rv = BCM_E_NONE,
        i;
    bcm_gport_t
        mc_gport_members[16];  
            
    /* Open Multicast Group  */
    printf("stacking_mc_func(): Open Multicast Group, mc_id=%d, mc_type=%d\n", mc_id, mc_type);        
    rv = open_multicast_group(unit, mc_id, mc_type);
    if (!(rv == BCM_E_NONE || rv == BCM_E_EXISTS)) 
    {
        printf("Error, open_multicast_group(). mc_id=%d\n",mc_id);
        return rv;
    } 
    
     
    /* Add MC members */
    printf("stacking_mc_func():Add MC members\n");
    for (i = 0; i < mc_member_count; i++) {
        BCM_GPORT_SYSTEM_PORT_ID_SET(mc_gport_members[i], mc_port_members[i]);
        rv = add_multicast_replication(unit, mc_id, mc_gport_members[i], nxt_mc_id[i], mc_type);
      if (rv != BCM_E_NONE) {
          printf("Error, add_multicast_replication(). mc_gport_members[%d]=0x%x\n", i, mc_gport_members[i]);
          return rv;
        }
    }
    
    /* Config mact to send packet to MC-id */
    printf("stacking_uc_func(): Calling l2_addr_add_mc(), vlan=%d, mc_id=%d\n", vlan, mc_id);
    rv = l2_addr_add_mc(unit, vlan, mc_id);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, l2_addr_add_mc()\n");
        return rv;
    }
    
    return rv;
}

/*
 * Example for MC stacking application
 */
int stacking_mc_appl(int unit, int tmd)
{
    int 
        rv = BCM_E_NONE,
        vlan,
        mc_type, /* 0=ING, 1=EG, Other=Both */
        mc_id,
        mc_member_count;
    int
        mc_port_members[16],
        nxt_mc_id[16];
    
    if (tmd == 0x0) {
        vlan = 1;
        
        mc_type = 1;
        mc_id = 8000;
        
        mc_member_count = 4;
        mc_port_members[0] = 15; /* Stacking Ports */
        mc_port_members[1] = 16; /* Stacking Ports */
        mc_port_members[2] = 17;
        mc_port_members[3] = 13;

        nxt_mc_id[0] = 8001;
        nxt_mc_id[1] = 8001;
        nxt_mc_id[2] = 0;
        nxt_mc_id[3] = 0;        

    } else if (tmd == 0x1) {
        vlan = 1;
        
        mc_type = 0;
        mc_id = 8001;
        
        mc_member_count = 4;
        mc_port_members[0] = 256 + 13;
        mc_port_members[1] = 256 + 15; /* should be filtered by DSS */
        mc_port_members[2] = 256 + 16; /* should be filtered by DSS */
        mc_port_members[3] = 256 + 17;   
        
        nxt_mc_id[0] = 0;
        nxt_mc_id[1] = 8000;
        nxt_mc_id[2] = 8000;     
        nxt_mc_id[3] = 0;     
    }
    
    rv = stacking_uc_appl(unit, tmd);
    if (rv != BCM_E_NONE) {
        printf("Error, stacking_uc_func(), rv=%d.\n", rv);
        return rv;
    }
    
    rv = stacking_mc_func(unit, vlan, mc_type, mc_id, nxt_mc_id, mc_member_count, mc_port_members);
    if (rv != BCM_E_NONE) {
        printf("Error, stacking_mc_func(), rv=%d.\n", rv);
        return rv;
    }
    
    return rv;
}

/*
 * Example for flooding application
 */
int erp_port_get(int unit, int *erp_sysport)
{
    int 
        rv = BCM_E_NONE,
        count_erp = 0,
        nof_erp_ports,
        i;
    bcm_gport_t 
        erp_local_port_gport[2], erp_sysport_gport;
        
    if (device_type == device_type_jericho) {
        nof_erp_ports = 2;
    } else {
        nof_erp_ports = 1;
    }
    
    rv = bcm_port_internal_get(unit, BCM_PORT_INTERNAL_EGRESS_REPLICATION, nof_erp_ports, &erp_local_port_gport, &count_erp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_internal_get(), rv=%d.\n", rv);
        return rv;
    }

    for (i = 0; i < nof_erp_ports; ++i) {
        printf("count_erp=%d, erp_local_port_gport=0x%x\n", count_erp, erp_local_port_gport[i]);
        
        rv = bcm_stk_gport_sysport_get(unit, erp_local_port_gport[i], &erp_sysport_gport);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_stk_gport_sysport_get(), rv=%d.\n", rv);
            return rv;
        }
        
        erp_sysport[i] = BCM_GPORT_SYSTEM_PORT_ID_GET(erp_sysport_gport);
            
        printf("count_erp=%d, erp_local_port_gport=0x%x, erp_sysport_gport=0x%x, *erp_sysport=0x%x,%d\n", count_erp, erp_local_port_gport[i], erp_sysport_gport, erp_sysport[i], erp_sysport[i]);
    }
    
    return rv;
}

int stacking_flooding_appl(int unit, int tmd)
{
    int 
        rv = BCM_E_NONE,
        vlan,
        mc_type, /* 0=ING, 1=EG, Other=Both */
        mc_id,
        mc_member_count,
        erp_sysport[2];
    int
        mc_port_members[16],
        nxt_mc_id[16];
        
    rv = get_device_type(unit, &device_type);
    if (rv != BCM_E_NONE) {
        printf("Error, get_device_type(), rv=%d.\n", rv);
        return rv;
    }
    
    /* Bring up Stacking UC application */    
    rv = stacking_uc_appl(unit, tmd);
    if (rv != BCM_E_NONE) {
        printf("Error, stacking_uc_func(), rv=%d.\n", rv);
        return rv;
    }
    
    if (tmd == 0x0) {
        vlan = 1;
        
        mc_type = 1;
        mc_id = 1;
        
        mc_member_count = 2;
        mc_port_members[0] = 15; /* Stacking Ports */
        mc_port_members[1] = 16; /* Stacking Ports */

        nxt_mc_id[0] = 1;
        nxt_mc_id[1] = 1;
                
    } else if (tmd == 0x1) {
        vlan = 1;
        
        mc_type = 1;
        mc_id = 1;
        
        mc_member_count = 2;
        mc_port_members[0] = 256 + 15; /* Stacking Ports */
        mc_port_members[1] = 256 + 16; /* Stacking Ports */

        nxt_mc_id[0] = 1;
        nxt_mc_id[1] = 1;
    }
    
    rv = stacking_mc_func(unit, vlan, mc_type, mc_id, nxt_mc_id, mc_member_count, mc_port_members);
    if (rv != BCM_E_NONE) {
        printf("Error, stacking_mc_func(), rv=%d.\n", rv);
        return rv;
    }
    
    return rv;
}

