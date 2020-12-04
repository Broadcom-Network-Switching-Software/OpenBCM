/*~~~~~~~~~~~~~~~~~~~~~~~~~~Stacking: UC, 2 TM-domain (1 fap each) ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
 * File: cint_stk_uc.c
 * Purpose: Example Stacking UC Validation
 * Reference: BCM88650 TM Device Driver (Stacking)
 * 
 * Environment:
 *
 * The following application example matches the following system configuration:
 *
 *                                Tm-domain=0:                         Tm-domain=1:                    
 *                                    fap-id=0                               fap-id=1                       
 *                                    local-stacking ports: 15,16   local-stacking ports: 15,16 (system: 256 + 15, 256 + 16) 
 *                                    in-local-port: 13                    in-local-port: 13          
 *                                    out-ports: 13,17                   out-ports: 256 + 13, 256 + 17
 *                           in_port         --------                              --------        in_port
 *                                    -------|      |         stacking ports       |      |-------
 *                              out_port     |      |----------------------------- |      |        out_port
 *                                    -------|      |----------------------------- |      |-------
 *                                           |      |                              |      |
 *                                           --------                              --------
 *
 * All the configuration example is in stacking_uc_appl().
 * Relevant soc properties configuration:
 * For TM-domain 0:
 *    stacking_enable=1
 *    device_tm_domain=0
 *    tm_port_header_type_15=STACKING
 *    tm_port_header_type_16=STACKING
 *    fabric_ftmh_outlif_extension=ALWAYS
 *    system_ftmh_load_balancing_ext_mode=ENABLED
 *    number_of_trunks=1024
 *
 * For TM-domain 1:                         
 *    stacking_enable=1             
 *    device_tm_domain=1                     
 *    tm_port_header_type_15=STACKING     
 *    tm_port_header_type_16=STACKING            
 *    fabric_ftmh_outlif_extension=ALWAYS
 *    system_ftmh_load_balancing_ext_mode=ENABLED 
 *    number_of_trunks=1024
 *
 *    Local:
 *        diag_disable=1
 *
 * Adjust to different system configuration:
 *  1. Change relevant soc properties.
 *  2. Call stacking_uc_func() with the relevant system configuration:
 *       tmd - tm domain.
 *         stk_member_count - number of stacking ports.
 *         out_port_count - number of out port (uses for eth forwarding - l2).
 *         in_local_port - trafic in port (uses for load-balancing key creation).
 *         stk_port_members - members in stacking lag (array of stacking ports).
 *         out_port_members - members in out port.
 *  3. Application Bring up:
 *       The application used in this cint is called via 'init appl_dpp' Shell command. 
 *     This application can be replaced by any application That configure the following:
 *            create scheduling for all local and system ports on both TMD, open vlan 0x1.
 *         The application should be called after stacking DB creation, but before trunk create/set.
 *
 * Test Run 1:
 *    1. inject packet from in port. packet format: 
 *         a. eth packet
 *         b. da[5] = out port in hex % 256 (port 113 --> 0x0,0x0,0x0,0x0,0x0,0x71)
 *         c. sa changes (for lb-key)
 *         d. vlan tag = 1.
 *    2. Expected Output:
 *         a. according to the configured outpot port, the same packet should appear at the output - capture out going packet.
 *         b. traffic should be divided on all stacking ports (same lag) - show counters on both TMD.
 *         c. expected output of shell command "show counters" :
 *          (for one packet sent from Tm-domain 0)
 *
 *    on Tm-domain 0:
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
 *        15 ||                         0 |                         1 |                         0 |                         1 |
 *
 *          * packet should leave from port 15 or port 16, selected randomly.
 *
 *   on Tm-domain 1: 
 *
 *      RX
 *      ---
 *      Port ||       snmpIfHCInUcastPkts |        snmpIfInNUcastPkts |            snmpIfInErrors |  snmpEtherStatsRXNoErrors |
 *      -----------------------------------------------------------------------------------------------------------------------
 *        15 ||                         0 |                         0 |                         1 |                         0 |
 *      TX
 *      ---
 *      Port ||      snmpIfHCOutUcastPkts |       snmpIfOutNUcastPkts |           snmpIfOutErrors |  snmpEtherStatsTXNoErrors |
 *      -----------------------------------------------------------------------------------------------------------------------
 *        13 ||                         1 |                         0 |                         0 |                         1 |
 *
 *           * packet should enter from port 15 or port 16 depend on which port it's leave Tm-domain 0.
 *
 * cd ../../../../src/examples/dpp
 * cint cint_stk_uc.c
 * cint
 * In TMD 0:
 *     stacking_uc_appl(unit, 0);
 * In TMD 1:
 *     stacking_uc_appl(unit, 1);
 *
 *
 * Trunk API using example:
 *    example for trunk API using on stacking trunk - get, destroy.
 *    notice that tid should be the value which returned from bcm_trunk_create() (tid = peer_tmd + 256 (max lag in system)).
 *  Usage example: 
 *        BCM_TRUNK_STACKING_TID_SET(tid, peer_tmd);
 *       stacking_bcm_trunk_get_destroy(unit, tid, 5);
 *
 *
 * Remarks:
 *    1. Read Stacking section in BCM88650 TM Device Driver.
 *    2. The application should be called after stacking DB creation, but before trunk create/set.
 *    3. Stacking DB should be configured before application bring up (As in cint, In Negev Chassis diag_disable=1).
 *    4. Using Load-Balancing Extension should be across TM-Domains.
 */
 
 
/* Set DssStacking trap to Drop - for MC bitmap check and prune */
int stacking_discard_config_traps(int unit, bcm_rx_trap_t trap_id) {
    
     int rv = BCM_E_NONE;
    bcm_rx_trap_config_t config;
    int flags = 0;
    int dest_port = 14;
    int redirect_trap_id = 0;


    if (redirect_trap_id == 0) {
        /* set drop to drop packet, this is also the driver default if filter is enabled*/
        rv = bcm_rx_trap_type_create(unit,BCM_RX_TRAP_WITH_ID,trap_id,&redirect_trap_id);
        if (rv != BCM_E_NONE) {
            printf("Error, in trap create, trap id=%d\n", trap_id);
            return rv;
        }
    }
    
    /*change dest port for trap */
  bcm_rx_trap_config_t_init(&config);
    config.flags |= (BCM_RX_TRAP_UPDATE_DEST); 
    config.trap_strength = 5;
    config.dest_port=BCM_GPORT_BLACK_HOLE; 

    rv = bcm_rx_trap_set(unit,redirect_trap_id,&config);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_rx_trap_set(), trap_id=%d\n", trap_id);
        return rv;
    }

  return rv;
}

/* Utility for l2 addr add */
int l2_addr_add(int unit, bcm_mac_t mac, uint16 vid, bcm_gport_t port) {

    int rv;

    bcm_l2_addr_t l2addr;

    bcm_l2_addr_t_init(&l2addr, mac, vid);

    l2addr.port = port;
    l2addr.vid = vid;
    l2addr.flags = BCM_L2_STATIC;
    
    rv = bcm_l2_addr_add(unit, &l2addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add(), rv=%d.\n", rv);
        return rv;
    }

    return BCM_E_NONE;
}

/* Create l2 entries for all out ports, set MAC according to out port */
int stacking_l2_addr_config(int unit, int out_port_count, int *out_port_members)
{
    int 
        rv = BCM_E_NONE,
        i,
        vlan = 0x1;
    bcm_gport_t
        out_gport_members[16];
    bcm_mac_t 
        da  = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        
    for (i = 0; i < out_port_count ; i++) {
        BCM_GPORT_SYSTEM_PORT_ID_SET(out_gport_members[i], out_port_members[i]);
        printf("i=%d, out_gport_members[i]=0x%x, out_port_members[i]=%d,0x%x\n", i, out_gport_members[i], out_port_members[i], out_port_members[i]);
        da[5] = out_port_members[i] % 256;
        rv = l2_addr_add(unit, da, vlan, out_gport_members[i]);
        if (rv != BCM_E_NONE) {
            printf("Error, l2_addr_add(), rv=%d.\n", rv);
            return rv;
        }
    }
    
    return rv;
}

/* configure load-balancing KEY for stack lag */
int stacking_lb_config(int unit, int in_local_port)
{
    int 
        rv = BCM_E_NONE;
    

    rv = bcm_switch_control_port_set(unit, in_local_port, bcmSwitchTrunkHashPktHeaderSelect, BCM_HASH_HEADER_FORWARD);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_port_set(), rv=%d.\n", rv);
        return rv;
    }
    rv = bcm_switch_control_port_set(unit, in_local_port, bcmSwitchTrunkHashPktHeaderCount, 0x2);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_port_set(), rv=%d.\n", rv);
        return rv;
    }
    
    return rv;
}

/* Bring up Stacking: DB, application, trunk */
int configure_tmd_stk_functions(int unit, int tmd, int stk_member_count, int *stk_port_members)
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
        stk_gport_members[16];
    bcm_trunk_member_t 
        trunk_port_members[16];
    bcm_trunk_info_t 
        trunk_info;
    int 
        trunk_member_count = 0x0;
    char 
        appl_str[256];
        
        
    if (tmd == 0x0) {
        peer_tmd = 0x1;
        modid = 0x0;
    } else if (tmd == 0x1) {
        peer_tmd = 0x0;
        modid = 0x2;
    }
            
    /* Create Stacking DB */
    rv = bcm_stk_modid_domain_add(unit, 0 /* modid */, 0 /*tm_domain*/);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_stk_modid_domain_add(), rv=%d.\n", rv);
        return rv;
    }
    rv = bcm_stk_modid_domain_add(unit, 1 /* modid */, 0 /*tm_domain*/);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_stk_modid_domain_add(), rv=%d.\n", rv);
        return rv;
    }
    rv = bcm_stk_modid_domain_add(unit, 2 /* modid */, 1 /*tm_domain*/);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_stk_modid_domain_add(), rv=%d.\n", rv);
        return rv;
    }
    rv = bcm_stk_modid_domain_add(unit, 3 /* modid */, 1 /*tm_domain*/);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_stk_modid_domain_add(), rv=%d.\n", rv);
        return rv;
    }
    for (i = 0; i < stk_member_count ; i++) {
        BCM_GPORT_SYSTEM_PORT_ID_SET(stk_gport_members[i], stk_port_members[i]);
        trunk_port_members[i].gport = stk_gport_members[i];
    }
        
    /*
     *    Main Application
     */
    sprintf(appl_str, "init appl_dpp %d 2 0", modid);
    bshell(unit, appl_str);
    
    
    /* Stacking Trunk creation and Setting */
    BCM_TRUNK_STACKING_TID_SET(tid, peer_tmd);
    flags = BCM_TRUNK_FLAG_WITH_ID;
    rv = bcm_trunk_create(unit, flags, &tid);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_trunk_create_id(), rv=%d.\n", rv);
        return rv;
    }
    
    trunk_info.flags = 0x0;
    trunk_member_count = stk_member_count;
    rv = bcm_trunk_set(unit, tid, &trunk_info, trunk_member_count, trunk_port_members);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_trunk_set(), rv=%d.\n", rv);
        return rv;
    }
    
    return rv;
}

/* Bringup Staking Application: Stacking, Trap, LB-Key, l2 */
int stacking_uc_func(int unit, int tmd, int stk_member_count, int out_port_count, int in_local_port, int *stk_port_members, int *out_port_members)
{
    int 
        rv = BCM_E_NONE;

    printf("stacking_uc_func(), unit=%d, tmd=%d: Calling configure_tmd_stk_functions()\n", unit, tmd);
    /* Setting stacking trunk, this function can handle 2 TMDs  (TMD-0 and TMD-1) */
    rv = configure_tmd_stk_functions(unit, tmd, stk_member_count, stk_port_members);
    if (rv != BCM_E_NONE) {
        printf("Error, configure_tmd_stk_functions(), rv=%d.\n", rv);
        return rv;
    }
        
    /* Configure Load-Balancing Key - this is not a needed configuration for the stacking trunk, but needed if we want to send traffic through this port */
    printf("stacking_uc_func(): Calling stacking_lb_config(), in_local_port=%d\n", in_local_port);
    rv = stacking_lb_config(unit, in_local_port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set(), rv=%d.\n", rv);
        return rv;
    }    
    
    /* Add relevant Stacking Trap */
    printf("stacking_uc_func(): Calling stacking_discard_config_traps()\n");
    rv = stacking_discard_config_traps(unit, bcmRxTrapDssStacking);
    if (rv != BCM_E_NONE) {
        printf("Error, stacking_discard_config_traps(), rv=%d.\n", rv);
        return rv;
    }    
        
    /* config mact to send packet to remote sysport */
    printf("stacking_uc_func(): Calling stacking_l2_addr_config(), out_port_count=%d\n", out_port_count);
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
int stacking_uc_appl(int unit, int tmd)
{
    int 
        rv = BCM_E_NONE,
        stk_member_count,
        out_port_count,
        in_local_port;
    int
        stk_port_members[16],
        out_port_members[16];
    
    if (tmd == 0x0) {
        /* In TMD-0 ports 15 and 16 are defined as stacking ports */
        stk_member_count = 2;
        stk_port_members[0] = 15;
        stk_port_members[1] = 16;
                
        in_local_port = 13;
        
        out_port_count = 2;
        out_port_members[0] = 256 + 13;
        out_port_members[1] = 256 + 17;
        
    } else if (tmd == 0x1) {
        /* In TMD-1 ports 256+15 and 256+16 are defined as stacking ports */
        stk_member_count = 2;
        stk_port_members[0] = 256 + 15;
        stk_port_members[1] = 256 + 16;
                
        in_local_port = 13;
        
        out_port_count = 2;
        out_port_members[0] = 13;
        out_port_members[1] = 17;
        
    }
    
    rv = stacking_uc_func(unit, tmd, stk_member_count, out_port_count, in_local_port, stk_port_members, out_port_members);
    if (rv != BCM_E_NONE) {
        printf("Error, stacking_uc_func(), rv=%d.\n", rv);
        return rv;
    }
    
    return rv;
}

/*
 *    Trunk API using example
 */
int stacking_bcm_trunk_get_destroy(int unit, int tid, int max_member_count)
{
    int 
        rv = BCM_E_NONE,
        i, 
        member_count = 0;
    uint32 
        flags;
    bcm_trunk_info_t  
        trunk_info;
    bcm_trunk_member_t 
        trunk_port_members[256];
        
    /* Get current trunk */
    trunk_info.flags = 0x0;
    rv = bcm_trunk_get(unit, tid, &trunk_info, max_member_count, trunk_port_members, &member_count);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_trunk_get(), rv=%d.\n", rv);
        return rv;
    }
    
    printf("tid=%d, max_member_count=%d, member_count=%d\n", tid, max_member_count, member_count);
    for (i=0;i < member_count; i ++) {
        printf("gport=0x%x, modid=%d, port=%d\n", trunk_port_members[i].gport, BCM_GPORT_MODPORT_PORT_GET(trunk_port_members[i].gport), BCM_GPORT_MODPORT_MODID_GET(trunk_port_members[i].gport));
    }
    
    /* Destroy current trunk */
    rv = bcm_trunk_destroy(unit, tid);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_trunk_destroy(), rv=%d.\n", rv);
        return rv;
    }    
    return rv;
}

