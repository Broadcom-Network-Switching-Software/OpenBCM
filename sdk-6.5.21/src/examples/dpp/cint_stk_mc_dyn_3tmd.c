/*~~~~~~~~~~~~~~~~~~~~~~~~~~Sracking: UC, 2 TM-domain (1 fap each) ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*
 * 
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
 * File: cint_stk_mc_dyn_3tmd.c
 * Purpose: Example Stacking MC Validation
 * Reference: BCM88650 TM Device Driver (Stacking)
 * 
 * 
 * Environment:
 *      Assuming UC environment (look at: cint_stk_uc_dyn_3tmd.c Environment description). 
 * 
 *
 * Configuration :
 *     set the same configurations as in UC (look at: cint_stk_uc_dyn_3tmd.c Relevant soc properties configuration). 
 *         
 *
 * Stacking dynamic configuration flow:
 *          1. set all the configurations of the stacking system (look at: cint_stk_uc_dyn_3tmd.c look at Stacking dynamic configuration flow).
 *          2. Set multicast group:
 *              a. open multicast group.
 *              b. set multicast ports replications and flow.
 *                   
 * Run Application:
 *     cd ../../../../src/examples/dpp
 *     cint cint_stk_mc_dyn.c
 *     cint
 * In TMD 0:
 *     stacking_mc_dyn_3tmd_appl(unit, 0);
 * In TMD 1:
 *     stacking_mc_dyn_3tmd_appl(unit, 1); 
 * In TMD 2:
 *     stacking_mc_dyn_3tmd_appl(unit, 2);
 *
 *       
 * Test Run 1:
 *    1. inject packet from in port (of TM-domain 0 or 2). packet format: 
 *         a. eth packet
 *         b. da[5] = out port in hex % 256 ( --> 0x0,0x0,0x0,0x0,0x40,0x0).
 *         c. sa changes (for lb-key)
 *         d. vlan tag = 1.
 *  2. Expected Output:
 *       a. according to the configured output port, the same packet should appear at the output - capture out going packet.
 *       b. traffic should be sent to all stacking ports, and out-port.
 *       c. expected output of shell command "show counters" :
 *          (for one packet sent from Tm-domain 2)
 *
 *       on TM-domain 2:
 *
 *      RX
 *      ---
 *      Port ||       snmpIfHCInUcastPkts |        snmpIfInNUcastPkts |            snmpIfInErrors |  snmpEtherStatsRXNoErrors |
 *      -----------------------------------------------------------------------------------------------------------------------
 *        13 ||                         1 |                         0 |                         0 |                         1 |
 *      TX
 *      ---
 *      Port ||      snmpIfHCOutUcastPkts |       snmpIfOutNUcastPkts |           snmpIfOutErrors |  snmpEtherStatsTXNoErrors |
 *      -----------------------------------------------------------------------------------------------------------------------
 *         1 ||                         1 |                         0 |                         0 |                         1 |
 *        14 ||                         0 |                         1 |                         0 |                         1 |
 *        16 ||                         0 |                         1 |                         0 |                         1 | 
 *
 *          * packet should leave from port 15 or port 14, selected randomly. 
 *
 *       on TM-domain 1:
 *
 *      RX
 *      ---
 *      Port ||       snmpIfHCInUcastPkts |        snmpIfInNUcastPkts |            snmpIfInErrors |  snmpEtherStatsRXNoErrors |
 *      -----------------------------------------------------------------------------------------------------------------------
 *        13 ||                         0 |                         1 |                         0 |                         1 |
 *      TX
 *      ---
 *      Port ||      snmpIfHCOutUcastPkts |       snmpIfOutNUcastPkts |           snmpIfOutErrors |  snmpEtherStatsTXNoErrors |
 *      -----------------------------------------------------------------------------------------------------------------------
 *        15 ||                         0 |                         1 |                         0 |                         1 |
 *        17 ||                         1 |                         0 |                         0 |                         1 |
 *
 *          * packet should enter from port 14 or port 13 depend on which port it's leave Tm-domain 2, and leave from port 15 or port 16, selected randomly.
 *
 *       on TM-domain 0:
 *
 *      RX
 *      ---
 *      Port ||       snmpIfHCInUcastPkts |        snmpIfInNUcastPkts |            snmpIfInErrors |  snmpEtherStatsRXNoErrors |
 *      -----------------------------------------------------------------------------------------------------------------------
 *        15 ||                         0 |                         1 |                         0 |                         1 |
 *      TX
 *      ---
 *      Port ||      snmpIfHCOutUcastPkts |       snmpIfOutNUcastPkts |           snmpIfOutErrors |  snmpEtherStatsTXNoErrors |
 *      -----------------------------------------------------------------------------------------------------------------------
 *         1 ||                         1 |                         0 |                         0 |                         1 |
 *        13 ||                         1 |                         0 |                         0 |                         1 |
 *        17 ||                         1 |                         0 |                         0 |                         1 |  
 *
 *          * packet should enter from port 14 or port 13 depend on which port it's leave Tm-domain 1.
 *  
 *
 *    Flooding example:
 *    flooding in Stacking system is done by creating Ingress MC group with mc_id=vlan, CUD=vlan which contain all the Stacking ports and the ERP port, in all TM-domains.
 *    The packet to ERP port with CUD=vlan will create flooding in the current domain. Only one of the packets towards the Stacking port will pass to the next TM-domain, 
 *    and will be distributed to Stacking ports (should be filtered by Trap), local ERP port.
 *    This example is based on Stacking UC application, and ERP port bring up.
 *    
 *      Usage:
 *          In TMD 0: stacking_flooding_dyn_3tmd_appl(unit, 0);
 *          In TMD 1: stacking_flooding_dyn_3tmd_appl(unit, 1);  
 *          In TMD 2: stacking_flooding_dyn_3tmd_appl(unit, 2);   
 *
 *      to test flooding - injected packet with unknown destination address .
 *      packet should leave through all the ports.
 *
 * Remarks:
 *    1. Read Stacking section in BCM88650 TM Device Driver.
 */

#include "cint_stk_mc.c"
#include "cint_stk_uc_dyn_3tmd.c"
/*
 * Example for MC stacking application
 */
int stacking_mc_dyn_3tmd_appl(int unit, int tmd)
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
        
        mc_type = 0;
        mc_id = 8000;
        
        mc_member_count = 5;
        mc_port_members[0] = 1;
        mc_port_members[1] = 13;
        mc_port_members[2] = 15; /* Stacking Ports */
        mc_port_members[3] = 16; /* Stacking Ports */
        mc_port_members[4] = 17;

        nxt_mc_id[0] = 0;
        nxt_mc_id[1] = 0;
        nxt_mc_id[2] = 8001;
        nxt_mc_id[3] = 8001;
        nxt_mc_id[4] = 0;
        
    } else if (tmd == 0x1) {
        vlan = 1;
        
        mc_type = 1;
        mc_id = 8001;
        
        mc_member_count = 5;
        mc_port_members[0] = 256 + 13; /* Stacking Ports */
        mc_port_members[1] = 256 + 14; /* Stacking Ports */
        mc_port_members[2] = 256 + 15; /* Stacking Ports */
        mc_port_members[3] = 256 + 16; /* Stacking Ports */
        mc_port_members[4] = 256 + 17;   
        
        nxt_mc_id[0] = 8002;
        nxt_mc_id[1] = 8002;
        nxt_mc_id[2] = 8000;     
        nxt_mc_id[3] = 8000; 
        nxt_mc_id[4] = 0; 
           
    } else if (tmd == 0x2) {
        vlan = 1;

        mc_type = 0;
        mc_id = 8002;

        mc_member_count = 5;
        mc_port_members[0] = 256*2 + 1; 
        mc_port_members[1] = 256*2 + 13;  
        mc_port_members[2] = 256*2 + 14;  /* Stacking Ports */
        mc_port_members[3] = 256*2 + 15;  /* Stacking Ports */
        mc_port_members[4] = 256*2 + 16;  /* Stacking Ports */

        nxt_mc_id[0] = 0;
        nxt_mc_id[1] = 0;
        nxt_mc_id[2] = 8001;     
        nxt_mc_id[3] = 8001; 
        nxt_mc_id[4] = 8000;
    }
    
    rv = stacking_uc_dyn_3tmd_appl(unit, tmd);
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

int stacking_flooding_dyn_3tmd_appl(int unit, int tmd)
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
    rv = stacking_uc_dyn_3tmd_appl(unit, tmd);
    if (rv != BCM_E_NONE) {
        printf("Error, stacking_uc_func(), rv=%d.\n", rv);
        return rv;
    }

    /* Get ERP system port */    
    rv = erp_port_get(unit, erp_sysport);
        if (rv != BCM_E_NONE) {
        printf("Error, erp_port_get(), rv=%d.\n", rv);
        return rv;
    }
    
    if (tmd == 0x0) {
        vlan = 1;
        
        mc_type = 0;
        mc_id = 1;
        
        mc_member_count = 4;
        mc_port_members[0] = 15; /* Stacking Ports */
        mc_port_members[1] = 16; /* Stacking Ports */
        mc_port_members[2] = 14; /* Stacking Ports */
        mc_port_members[3] = erp_sysport[0];
        if (device_type == device_type_jericho) {
            mc_port_members[4] = erp_sysport[1];
            ++mc_member_count;
        }

        nxt_mc_id[0] = 1;
        nxt_mc_id[1] = 1;
        nxt_mc_id[2] = 1;
        nxt_mc_id[3] = 1;     
        
    } else if (tmd == 0x1) {
        vlan = 1;
        
        mc_type = 0;
        mc_id = 1;
        
        mc_member_count = 5;
        mc_port_members[0] = 256 + 13; /* Stacking Ports */
        mc_port_members[1] = 256 + 14; /* Stacking Ports */
        mc_port_members[2] = 256 + 15; /* Stacking Ports */
        mc_port_members[3] = 256 + 16; /* Stacking Ports */
        mc_port_members[4] = erp_sysport[0];        
        if (device_type == device_type_jericho) {
            mc_port_members[5] = erp_sysport[1];
            ++mc_member_count;
        }

        nxt_mc_id[0] = 1;
        nxt_mc_id[1] = 1;
        nxt_mc_id[2] = 1;
        nxt_mc_id[3] = 1;
        nxt_mc_id[4] = 1;
             
    } else if (tmd == 0x2) {
        vlan = 1;
        
        mc_type = 0;
        mc_id = 1;
        
        mc_member_count = 4;
        mc_port_members[0] = 256*2 + 14; /* Stacking Ports */
        mc_port_members[1] = 256*2 + 15; /* Stacking Ports */
        mc_port_members[2] = 256*2 + 16; /* Stacking Ports */
        mc_port_members[3] = erp_sysport[0];
        if (device_type == device_type_jericho) {
            mc_port_members[4] = erp_sysport[1];
            ++mc_member_count;
        }

        nxt_mc_id[0] = 1;
        nxt_mc_id[1] = 1;
        nxt_mc_id[2] = 1;
        nxt_mc_id[3] = 1;    
    }
    
    rv = stacking_mc_func(unit, vlan, mc_type, mc_id, nxt_mc_id, mc_member_count, mc_port_members);
    if (rv != BCM_E_NONE) {
        printf("Error, stacking_mc_func(), rv=%d.\n", rv);
        return rv;
    }
    
    return rv;
}

