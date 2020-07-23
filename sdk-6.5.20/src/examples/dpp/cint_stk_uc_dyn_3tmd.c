/*~~~~~~~~~~~~~~~~~~~~~~~~~~Sracking: UC, 3 TM-domain, dynamic (1 fap each) ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
 * File: cint_stk_uc_dyn_3tmd.c
 * Purpose: Example Stacking UC Validation
 * Reference: BCM88650 TM Device Driver (Stacking)
 * 
 * 
 * Environment:
 *
 * The following application example matches the following system configuration:
 *
 *      Tm-domain=0:                    Tm-domain=1:                                            Tm-domain=2
 *          fap-id=0                        fap-id=1                                                fap-id=2    
 *          local-stacking ports:           local-stacking ports:                                   local-stacking ports:
 *              to Tm-domain 1: 15,16           to Tm-domain 0: 15,16 (system: 256+15, 256+16)          to Tm-domain 1: 14,15 (system: 256*2 + 14, 256*2 + 15)
 *              to Tm-domain 2: 14              to Tm-domain 2: 13,14 (system: 256+13, 256+14)          to Tm-domain 0: 16 (system: 256*2 + 16)
 *          in-local-port: 13               in-local-port: none                                     in-local-port: 256*2+13 
 *          out-ports: 13,17                out-ports: 256 +13,256 +17                              out-port: 256*2 +1, 256*2 +13      
 *  
 *                        in_port        --------                              --------                          --------     in_port
 *                                -------|      |        stacking ports        |      |       stacking port      |      |------
 *                                       |      |----------------------------- |      |--------------------------|      |      
 *                                -------|      |----------------------------- |      |--------------------------|      |-----  
 *                                       |      |                              |      |                          |      |        
 *                                       --------                              --------                          --------
 *                                          |                                                                       |
 *                                          -------------------------------------------------------------------------
 *                                                                          stacking-port
 *
 * All the configuration example is in stacking_uc_dyn_3tmd_appl().
 * Relevant soc properties configuration (For All TM-domains):
 * 
 *    stacking_enable=1
 *    fabric_ftmh_outlif_extension=ALWAYS    
 *    system_ftmh_load_balancing_ext_mode=ENABLED            
 *
 *    Local:
 *        diag_disable=1
 *
 *    Stacking dynamic configuration flow:
 *        1. System bring up: soc property configuration.
 *        2. Bring up local application
 *        3. Adding stacking ports (any order):
 *            a. Create Stacking DB (bcm_stk_modid_domain_add())
 *            b. Set local tm-domain (bcm_switch_control_set(bcmSwitchTMDomain))
 *            c. Configure header type (bcm_switch_control_port_set(bcmSwitchPortHeaderType, BCM_SWITCH_PORT_HEADER_TYPE_STACKING))
 *        4. Create stacking trunk:
 *            a. Create tid
 *            b. Add ports
 *        5. Add remote TMD ports
 *        6. configure reachability: reach tmd via stk_trunk (bcm_stk_domain_stk_trunk_add()) 
 *
 * Run Application:
 *     cd ../../../../src/examples/dpp
 *     cint cint_stk_uc_dyn.c
 *     cint
 * In TMD 0:
 *     stacking_uc_dyn_3tmd_appl(unit, 0);
 * In TMD 1:
 *     stacking_uc_dyn_3tmd_appl(unit, 1); 
 * In TMD 2:
 *     stacking_uc_dyn_3tmd_appl(unit, 2);
 *
 *       
 * Test Run 1:
 *    1. inject packet from in port (of TM-domain 0 or 2). packet format: 
 *         a. eth packet
 *         b. da[5] = out port in hex % 256 (port 256 + 13 --> 0x0,0x0,0x0,0x0,0x0,0x0d)
 *         c. sa changes (for lb-key)
 *         d. vlan tag = 1.
 *  2. Expected Output:
 *       a. according to the configured output port, the same packet should appear at the output - capture out going packet.
 *       b. traffic should be divided on all stacking ports (accept stacking port that connect TM-domain 0 to Tm-domain 2, see application below).
 *       c. expected output of shell command "show counters" :
 *          (for one packet sent from Tm-domain 2)
 *
 *       on TM-domain 2:
 *
 *          RX
 *         ---
 *          Port ||       snmpIfHCInUcastPkts |        snmpIfInNUcastPkts |            snmpIfInErrors |  snmpEtherStatsRXNoErrors |
 *          -----------------------------------------------------------------------------------------------------------------------
 *            13 ||                         1 |                         0 |                         0 |                         1 |
 *          TX
 *          ---
 *          Port ||      snmpIfHCOutUcastPkts |       snmpIfOutNUcastPkts |           snmpIfOutErrors |  snmpEtherStatsTXNoErrors |
 *          -----------------------------------------------------------------------------------------------------------------------
 *            15 ||                         0 |                         1 |                         0 |                         1 |   
 *
 *          * packet should leave from port 15 or port 14, selected randomly. 
 *
 *       on TM-domain 1:
 *
 *          RX
 *         ---
 *          Port ||       snmpIfHCInUcastPkts |        snmpIfInNUcastPkts |            snmpIfInErrors |  snmpEtherStatsRXNoErrors |
 *          -----------------------------------------------------------------------------------------------------------------------
 *            14 ||                         0 |                         0 |                         1 |                         1 |
 *          TX
 *          ---
 *          Port ||      snmpIfHCOutUcastPkts |       snmpIfOutNUcastPkts |           snmpIfOutErrors |  snmpEtherStatsTXNoErrors |
 *          -----------------------------------------------------------------------------------------------------------------------
 *            16 ||                         0 |                         1 |                         0 |                         1 |   
 *
 *          * packet should enter from port 14 or port 13 depend on which port it's leave Tm-domain 2, and leave from port 15 or port 16, selected randomly.
 *
 *       on TM-domain 0:
 *
 *          RX
 *         ---
 *          Port ||       snmpIfHCInUcastPkts |        snmpIfInNUcastPkts |            snmpIfInErrors |  snmpEtherStatsRXNoErrors |
 *          -----------------------------------------------------------------------------------------------------------------------
 *            16 ||                         0 |                         0 |                         1 |                         1 |
 *          TX
 *          ---
 *          Port ||      snmpIfHCOutUcastPkts |       snmpIfOutNUcastPkts |           snmpIfOutErrors |  snmpEtherStatsTXNoErrors |
 *          -----------------------------------------------------------------------------------------------------------------------
 *            13 ||                         1 |                         0 |                         0 |                         1 |   
 *
 *          * packet should enter from port 15 or port 16 depend on which port it's leave Tm-domain 1.
 *  
 *
 *  ECMP-   
 *      to see packets pass through the stacking port that connect TM-domain 0 to Tm-domain 2,
 *      Run (after running the regular application) on TM-domain 0:
 *          cint
 *          stacking_uc_dyn_3tmd_ecmp_appl(unit, 0); 
 *
 *      and inject packets to in-port of TM-domain 0.
 *      the expected output is: traffic divided to all stacking ports.   
 * 
 * Remarks:
 *    1. Read Stacking section in BCM88650 TM Device Driver.
 */
#include "cint_stk_uc.c"
 

/* Bring up Stacking: DB, application, trunk */
int configure_tmd_stk_dyn_functions(int unit, int tmd, int stk_member_count, int *stk_port_members)
{
    int
        rv = BCM_E_NONE,
        i,
        peer_tmd,
        modid,
        tid;
    uint32
        flags;
    bcm_gport_t
        stk_gport_members[16],
        stk_gport_modport_members[16];
    bcm_trunk_member_t 
        trunk_port_members[16];
    bcm_trunk_info_t 
        trunk_info;
    int 
        trunk_member_count = 0x0;
    char 
        appl_str[256];
    bcm_port_t
        tmp_local_port;
        
        
    if (tmd == 0x0) {
        peer_tmd = 0x1;
        modid = 0x0;
    } else if (tmd == 0x1) {
        modid = 0x2;
    } else if (tmd == 0x2) {
        peer_tmd = 0x1;
        modid = 0x4;
    }
    
    for (i = 0; i < stk_member_count ; i++) {                
        BCM_GPORT_SYSTEM_PORT_ID_SET(stk_gport_members[i], stk_port_members[i]);
        trunk_port_members[i].gport = stk_gport_members[i];
    }
    
    /* configure self TM-domian */
    rv = bcm_switch_control_set(unit, bcmSwitchTMDomain, tmd);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set(), rv=%d.\n", rv);
        return rv;
    }
            
    /* Create Stacking DB */
    rv = bcm_stk_modid_domain_add(unit, 0 /* modid */, 0 /*tm_domain*/);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_stk_modid_domain_add(), rv=%d.\n", rv);
        return rv;
    }
    rv = bcm_stk_modid_domain_add(unit, 2 /* modid */, 1 /*tm_domain*/);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_stk_modid_domain_add(), rv=%d.\n", rv);
        return rv;
    }
    rv = bcm_stk_modid_domain_add(unit, 4 /* modid */, 2 /*tm_domain*/);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_stk_modid_domain_add(), rv=%d.\n", rv);
        return rv;
    }
            
    /*
     *    Main Application
     */
    sprintf(appl_str, "init appl_dpp %d 3 0", modid);
    bshell(unit, appl_str);
    
    /* Config Header type */
    for (i = 0; i < stk_member_count ; i++) {
        rv = bcm_stk_sysport_gport_get(unit, stk_gport_members[i], &stk_gport_modport_members);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_stk_sysport_gport_get(), rv=%d.\n", rv);
            return rv;
        }
        tmp_local_port = BCM_GPORT_MODPORT_PORT_GET(stk_gport_modport_members);
        
        printf("configure_tmd_stk_dyn_functions(): tmp_local_port=%d\n", tmp_local_port);
        rv = bcm_switch_control_port_set(unit, tmp_local_port, bcmSwitchPortHeaderType, BCM_SWITCH_PORT_HEADER_TYPE_STACKING);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_switch_control_port_set(), rv=%d.\n", rv);
            return rv;
        }
    }
        

    if (tmd != 0x1) {
        /* Stacking Trunk creation and Setting */
        BCM_TRUNK_STACKING_TID_SET(tid, peer_tmd);
        flags = BCM_TRUNK_FLAG_WITH_ID;
        rv = bcm_trunk_create(unit, flags, &tid);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_trunk_create_id(), rv=%d.\n", rv);
            return rv;
        }
        
        for (i = 0; i < 2 ; i++) {     
            rv = bcm_trunk_member_add(unit, tid, &trunk_port_members[i]);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_trunk_member_add(), rv=%d.\n", rv);
                return rv;
            }
        }


            
        /* configure reachability: reach tmd via stk_trunk */
        rv = bcm_stk_domain_stk_trunk_add(unit, peer_tmd /* domain */, tid /* stk_trunk*/);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_stk_domain_stk_trunk_add(), rv=%d.\n", rv);
            return rv;
        }
        if (tmd == 0x0) {
            rv = bcm_stk_domain_stk_trunk_add(unit,  2 /* domain */, tid /* stk_trunk*/);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_stk_domain_stk_trunk_add(), rv=%d.\n", rv);
                return rv;
            }
        }else {
            rv = bcm_stk_domain_stk_trunk_add(unit,  0 /* domain */, tid /* stk_trunk*/);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_stk_domain_stk_trunk_add(), rv=%d.\n", rv);
                return rv;
            }              
        }

        /* Stacking Trunk creation and Setting */
        if (tmd == 0) {
            peer_tmd = 2;
        }else {
            peer_tmd = 0;
        }

        BCM_TRUNK_STACKING_TID_SET(tid, peer_tmd);
        flags = BCM_TRUNK_FLAG_WITH_ID;
        rv = bcm_trunk_create(unit, flags, &tid);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_trunk_create_id(), rv=%d.\n", rv);
            return rv;
        }
        
        for (i = 2; i < 3 ; i++) {        
            rv = bcm_trunk_member_add(unit, tid, &trunk_port_members[i]);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_trunk_member_add(), rv=%d.\n", rv);
                return rv;
            }
        }

    }else if (tmd == 0x1) {
        /* Stacking Trunk creation and Setting */
        BCM_TRUNK_STACKING_TID_SET(tid, 0);
        flags = BCM_TRUNK_FLAG_WITH_ID;
        rv = bcm_trunk_create(unit, flags, &tid);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_trunk_create_id(), rv=%d.\n", rv);
            return rv;
        }
        
        for (i = 0; i < 2 ; i++) {        
            rv = bcm_trunk_member_add(unit, tid, &trunk_port_members[i]);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_trunk_member_add(), rv=%d.\n", rv);
                return rv;
            }
        }
            
        /* configure reachability: reach tmd via stk_trunk */
        rv = bcm_stk_domain_stk_trunk_add(unit, 0 /* domain */, tid /* stk_trunk*/);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_stk_domain_stk_trunk_add(), rv=%d.\n", rv);
            return rv;
        }
        
        /* Stacking Trunk creation and Setting */
        BCM_TRUNK_STACKING_TID_SET(tid, 2);
        flags = BCM_TRUNK_FLAG_WITH_ID;
        rv = bcm_trunk_create(unit, flags, &tid);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_trunk_create_id(), rv=%d.\n", rv);
            return rv;
        }
        
        for (i = 2; i < 4 ; i++) {        
            rv = bcm_trunk_member_add(unit, tid, &trunk_port_members[i]);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_trunk_member_add(), rv=%d.\n", rv);
                return rv;
            }
        }
            
        /* configure reachability: reach tmd via stk_trunk */
        rv = bcm_stk_domain_stk_trunk_add(unit, 2 /* domain */, tid /* stk_trunk*/);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_stk_domain_stk_trunk_add(), rv=%d.\n", rv);
            return rv;
        }          
      }
        
    return rv;
}

/* Bringup Staking Application: Stacking, Trap, LB-Key, l2 */
int stacking_uc_dyn_func(int unit, int tmd, int stk_member_count, int out_port_count, int in_local_port_count, int *in_local_port, int *stk_port_members, int *out_port_members)
{
    int 
        i,
        rv = BCM_E_NONE;

    printf("stacking_uc_dyn_func(), unit=%d, tmd=%d: Calling configure_tmd_stk_functions()\n", unit, tmd);
    rv = configure_tmd_stk_dyn_functions(unit, tmd, stk_member_count, stk_port_members);
    if (rv != BCM_E_NONE) {
        printf("Error, configure_tmd_stk_functions(), rv=%d.\n", rv);
        return rv;
    }
        
    /* Configure Load-Balancing Key */
     for (i = 0; i < in_local_port_count ; i++) {
        printf("stacking_uc_dyn_func(): Calling stacking_lb_config(), in_local_port[%d]=%d\n", i, in_local_port[i]);
        rv = stacking_lb_config(unit, in_local_port[i]);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_switch_control_set(), rv=%d.\n", rv);
            return rv;
        }    
      }
    
    /* Add relevant Stacking Trap */
    printf("stacking_uc_dyn_func(): Calling stacking_discard_config_traps()\n");
    rv = stacking_discard_config_traps(unit, bcmRxTrapDssStacking);
    if (rv != BCM_E_NONE) {
        printf("Error, stacking_discard_config_traps(), rv=%d.\n", rv);
        return rv;
    }    
        
    /* config mact to send packet to remote sysport */
    printf("stacking_uc_dyn_func(): Calling stacking_l2_addr_config(), out_port_count=%d\n", out_port_count);
    rv = stacking_l2_addr_config(unit, out_port_count, out_port_members);
    if (rv != BCM_E_NONE) {
        printf("Error, stacking_l2_addr_config(), rv=%d.\n", rv);
        return rv;
    }
    
    return rv;
}

/*
 * Example for stacking application
 */
int stacking_uc_dyn_3tmd_appl(int unit, int tmd)
{
    int 
        rv = BCM_E_NONE,
        stk_member_count,
        out_port_count,
        in_local_port_count;
    int
        stk_port_members[16],
        out_port_members[16],
        in_local_port[16];
    
    if (tmd == 0x0) {
        stk_member_count = 3;
        stk_port_members[0] = 15;
        stk_port_members[1] = 16;
        stk_port_members[2] = 14;
                
        in_local_port_count = 1;
        in_local_port[0] = 13;
        
        out_port_count = 2;
        out_port_members[0] = (2 * 256) + 13;
        out_port_members[1] = (2 * 256) + 17;
        
    } else if (tmd == 0x1) {
        
        stk_member_count = 4;
        stk_port_members[0] = 256 + 15;
        stk_port_members[1] = 256 + 16;
        stk_port_members[2] = 256 + 13;
        stk_port_members[3] = 256 + 14;
                
        in_local_port_count = 4;
        in_local_port[0] = 13;
        in_local_port[1] = 14;
        in_local_port[2] = 15;
        in_local_port[3] = 16;
        
        out_port_count = 4;
        out_port_members[0] = 1;
        out_port_members[1] = 17;
        out_port_members[2] = (2 * 256) + 13;
        out_port_members[3] = (2 * 256) + 17;
        
    } else if (tmd == 0x2) {
        
        stk_member_count = 3;
        stk_port_members[0] = (2 * 256) + 14;
        stk_port_members[1] = (2 * 256) + 15;
        stk_port_members[2] = (2 * 256) + 16;
                
        in_local_port_count = 1;   
        in_local_port[0] = 13;
        
        out_port_count = 2;
        out_port_members[0] = 13;
        out_port_members[1] = 17;
        
    }
    
    rv = stacking_uc_dyn_func(unit, tmd, stk_member_count, out_port_count, in_local_port_count, in_local_port, stk_port_members, out_port_members);
    if (rv != BCM_E_NONE) {
        printf("Error, stacking_uc_dyn_func(), rv=%d.\n", rv);
        return rv;
    }
    
    return rv;
}


/*
 * Example for ecmp
 */


int stacking_uc_dyn_3tmd_ecmp_appl(int unit, int tmd)
{
    int rv = BCM_E_NONE;

    if (tmd != 0) {
        printf("Error, ECMP test relevant only to tmd=0. current tmd=%d\n", tmd);
        rv = -1;
        return rv;
    }

    rv = bcm_stk_domain_stk_trunk_add(unit, 2 /* domain */, 2 /* stk_trunk*/);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_stk_domain_stk_trunk_add(), rv=%d.\n", rv);
        return rv;
    }

    return rv;  

}
