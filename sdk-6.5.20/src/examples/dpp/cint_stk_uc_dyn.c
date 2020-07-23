/*~~~~~~~~~~~~~~~~~~~~~~~~~~Sracking: UC, 2 TM-domain, dynamic  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
 * File: cint_stk_uc_dyn.c
 * Purpose: Example Stacking UC Validation
 * Reference: BCM88650/BCM88670 TM Device Driver (Stacking)
 * 
 * Environment:
 *
 * The following application example matches the following system configuration(1 fap each):
 *
 *                                Tm-domain=0:                          Tm-domain=1:                    
 *                                    fap-id=0                              fap-id=1                       
 *                                    local-stacking ports: 15,16           local-stacking ports: 15,16 (system: 256 + 15, 256 + 16) 
 *                                    in-local-port: 13                     in-local-port: 13          
 *                                    out-ports: 13,17                      out-ports: 256+13, 256+17
 *
 *                        in_port         --------                              --------        in_port
 *                                 -------|      |        stacking ports        |      |-------
 *                           out_port     |      |----------------------------- |      |        out_port
 *                                 -------|      |----------------------------- |      |-------
 *                                        |      |                              |      |
 *                                        --------                              --------
 *
 * All the configuration example is in stacking_uc_dyn_appl().
 * Relevant soc properties configuration:
 * For TM-domain 0:
 *    stacking_enable=1
 *    fabric_ftmh_outlif_extension=ALWAYS    
 *    system_ftmh_load_balancing_ext_mode=ENABLED 
 *    number_of_trunks=1024           
 *
 * For TM-domain 1:                         
 *    stacking_enable=1             
 *    fabric_ftmh_outlif_extension=ALWAYS
 *    system_ftmh_load_balancing_ext_mode=ENABLED
 *    number_of_trunks.BCM88650=1024
 *
 *    Local:
 *        diag_disable=1
 *
 *    Stacking dynamic configuration flow:
 *        1. System bring up: soc property - stacking_enable
 *        2. Bring up local application
 *        3. Adding stacking ports (any order):
 *            a. Create Stacking DB (bcm_stk_modid_domain_add())
 *            b. Set local tm-domain (bcm_switch_control_set(bcmSwitchTMDomain))
 *            c. Configure header type (bcm_switch_control_port_set(bcmSwitchPortHeaderType, BCM_SWITCH_PORT_HEADER_TYPE_STACKING))
 *        4. Create stacking trunk:
 *            a. Create tid
 *            b. Add ports
 *        5. Add remote TMD ports
 *        6. configure reachability: reach tmd via stk_trunk (bcm_stk_domain_stk_trunk_add()).
 *        
 * Test Run 1:
 *  1. inject packet from in port. packet format: 
 *      a. eth packet
 *      b. da[5] = out port in hex % 256 (port 256 + 13 --> 0x0,0x0,0x0,0x0,0x0,0x0d)
 *      c. sa changes (for lb-key)
 *      d. vlan tag = 1.
 *  2. Expected Output:
 *      a. according to the configured outpot port, the same packet should appear at the output - capture out going packet.
 *      b. traffic should be divided on all stacking ports (same lag) - show counters on both TMD.
  *     c. expected output of shell command "show counters" :
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
 * cint cint_stk_uc_dyn.c
 * cint
 * In TMD 0:
 *     stacking_uc_dyn_appl(unit, 0);
 * In TMD 1:
 *     stacking_uc_dyn_appl(unit, 1);
 *
 * Remarks:
 *    1. Read Stacking section in BCM88650 TM Device Driver.
 *    2. The UC application should be called before MC application, as in stacking_mc_appl().
 */
/*#include "cint_stk_uc.c"*/
 

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
        peer_tmd = 0x0;
        modid = 0x2;
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
            
    /*
     *    Main Application
     */
    sprintf(appl_str, "init appl_dpp %d 2 0", modid);
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
        
    /* Stacking Trunk creation and Setting */
    BCM_TRUNK_STACKING_TID_SET(tid, peer_tmd);
    flags = BCM_TRUNK_FLAG_WITH_ID;
    rv = bcm_trunk_create(unit, flags, &tid);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_trunk_create_id(), rv=%d.\n", rv);
        return rv;
    }
    
    for (i = 0; i < stk_member_count ; i++) {        
        rv = bcm_trunk_member_add(unit, tid, &trunk_port_members[i]);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_trunk_member_add(), rv=%d.\n", rv);
            return rv;
        }
    }
        
    /* configure reachability: reach tmd via stk_trunk */
    printf("configure_tmd_stk_dyn_functions(), tmd=%d, peer_tmd=%d, tid=%d.\n", tmd, peer_tmd, tid);
    rv = bcm_stk_domain_stk_trunk_add(unit, peer_tmd /* reached domain */, tid /* via stk_trunk*/);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_stk_domain_stk_trunk_add(), rv=%d.\n", rv);
        return rv;
    }
        
    return rv;
}

/* Bringup Staking Application: Stacking, Trap, LB-Key, l2 */
int stacking_uc_dyn_func(int unit, int tmd, int stk_member_count, int out_port_count, int in_local_port, int *stk_port_members, int *out_port_members)
{
    int 
        rv = BCM_E_NONE;

    printf("stacking_uc_func(), unit=%d, tmd=%d: Calling configure_tmd_stk_functions()\n", unit, tmd);
    rv = configure_tmd_stk_dyn_functions(unit, tmd, stk_member_count, stk_port_members);
    if (rv != BCM_E_NONE) {
        printf("Error, configure_tmd_stk_functions(), rv=%d.\n", rv);
        return rv;
    }
        
    /* Configure Load-Balancing Key */
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
int stacking_uc_dyn_appl(int unit, int tmd)
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
        stk_member_count = 2;
        stk_port_members[0] = 15;
        stk_port_members[1] = 16;
                
        in_local_port = 13;
        
        out_port_count = 2;
        out_port_members[0] = 256 + 13;
        out_port_members[1] = 256 + 17;
        
    } else if (tmd == 0x1) {
        
        stk_member_count = 2;
        stk_port_members[0] = 256 + 15;
        stk_port_members[1] = 256 + 16;
                
        in_local_port = 13;
        
        out_port_count = 2;
        out_port_members[0] = 13;
        out_port_members[1] = 17;
        
    }
    
    rv = stacking_uc_dyn_func(unit, tmd, stk_member_count, out_port_count, in_local_port, stk_port_members, out_port_members);
    if (rv != BCM_E_NONE) {
        printf("Error, stacking_uc_func(), rv=%d.\n", rv);
        return rv;
    }
    
    return rv;
}

/* Environment:
 *
 * The following application example matches the following system configuration(2 faps each TMD):
 *
 *                                Tm-domain=0:                                                             Tm-domain=1
 *                                    fap-id=0,1  (Jericho is 0 and 1;
 *                                                       Arad/QAX/QUX is only 0)
 *                                    local-stacking ports: 15
 *                                    system-stacking ports: 15
 *                                    in-local-port: 13
 *                                    out-ports: 13,17
 *
 *                        in_port(13)  -----------                                                        ---------
 *                             -------|           |        stacking ports (15)                           |         |
 *                                    |   DNX     |------------------------------------------------------|   DNX   |
 *                             -------|  unit 0   |                                                      | unit 0  |
 *                      out_port(17)  |           |                                                      |         |
 *                                     ----------                                                         ---------
 *                                         |                                                                 |
 *                                     ---------                                                         ----------
 *                                    |FE unit 1|                                                        |FE unit 1|
 *                                     ---------                                                         ----------
 *                                         |                                                                 |
 *        in_port (256 + 13)           -----------                                                        ---------
 *                             -------|           |       stacking ports(256 + 15 )                      |         |
 *                                    |   DNX     |----------------------------- ------------------------|   DNX   |
 *                             -------|  unit 2   |                                                      | unit 2  |
 *        out_port (256 + 17)         |           |                                                      |         |
 *                                     ----------                                                         ---------
 *
 *                                Tm-domain=0:
 *                                    fap-id=2,3 (Jericho is 2 and 3 ;
 *                                                     Arad/QAX/QUX is only 2)
 *                                    local-stacking ports: 15
 *                                    system-stacking ports: 256 + 15
 *                                    in-local-port: 13
 *                                    out-ports: 256 + 13, 256 + 17
 *
 *
 * All the configuration example is in stacking_uc_dyn_appl2().
 * Relevant soc properties configuration:
 * For TM-domain 0:
 *    stacking_enable=1
 *    fabric_ftmh_outlif_extension=ALWAYS
 *    system_ftmh_load_balancing_ext_mode=ENABLED
 *    number_of_trunks=1024
 *
 *    Local:
 *        diag_disable=1
 *
 *    Stacking dynamic configuration flow:
 *        1. System bring up: soc property - stacking_enable
 *        2. Bring up local application
 *        3. Adding stacking ports (any order):
 *            a. Create Stacking DB (bcm_stk_modid_domain_add())
 *            b. Set local tm-domain (bcm_switch_control_set(bcmSwitchTMDomain))
 *            c. Configure header type (bcm_switch_control_port_set(bcmSwitchPortHeaderType, BCM_SWITCH_PORT_HEADER_TYPE_STACKING))
 *        4. Create stacking trunk:
 *            a. Create tid
 *            b. Add ports
 *        5. Add remote TMD ports
 *        6. configure reachability: reach tmd via stk_trunk (bcm_stk_domain_stk_trunk_add()).
 *
 *
 * cd ../../../../src/examples/dpp
 * cint cint_stk_uc.c
 * cint cint_stk_uc_dyn.c
 * cint
 * In TMD 0:
 *     stacking_uc_dyn_appl2(0, 0);
 *     stacking_uc_dyn_appl2(2, 0);
 *
 * Remarks:
 *    1. Read Stacking section in BCM88650 TM Device Driver.
 *    2. The UC application should be called before MC application, as in stacking_mc_appl().
 */

/* Bring up Stacking: DB, application, trunk */
int configure_tmd_stk_dyn_functions2(int unit, int tmd, int stk_member_count, int *stk_port_members)
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
    } else if (tmd == 0x1) {
        peer_tmd = 0x0;
    }

    if (unit == 0x0) {
        modid = 0x0;
    } else if (unit == 0x2) {
        modid = 0x2;
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
    rv = bcm_stk_modid_domain_add(unit, 0 /* modid */, tmd /*tm_domain*/);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_stk_modid_domain_add(), rv=%d.\n", rv);
        return rv;
    }
    rv = bcm_stk_modid_domain_add(unit, 1 /* modid */, tmd /*tm_domain*/);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_stk_modid_domain_add(), rv=%d.\n", rv);
        return rv;
    }
    rv = bcm_stk_modid_domain_add(unit, 2 /* modid */, tmd /*tm_domain*/);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_stk_modid_domain_add(), rv=%d.\n", rv);
        return rv;
    }
    rv = bcm_stk_modid_domain_add(unit, 3 /* modid */, tmd /*tm_domain*/);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_stk_modid_domain_add(), rv=%d.\n", rv);
        return rv;
    }

    /*
     *    Main Application
     */
    sprintf(appl_str, "init appl_dpp %d 2 0", modid);
    bshell(unit, appl_str);

    /* Config Header type */
    for (i = 0; i < stk_member_count ; i++) {
        rv = bcm_stk_sysport_gport_get(unit, stk_gport_members[i], &stk_gport_modport_members);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_stk_sysport_gport_get(), rv=%d.\n", rv);
            return rv;
        }
        tmp_local_port = BCM_GPORT_MODPORT_PORT_GET(stk_gport_modport_members);

        printf("configure_tmd_stk_dyn_functions2(): tmp_local_port=%d\n", tmp_local_port);
        rv = bcm_switch_control_port_set(unit, tmp_local_port, bcmSwitchPortHeaderType, BCM_SWITCH_PORT_HEADER_TYPE_STACKING);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_switch_control_port_set(), rv=%d.\n", rv);
            return rv;
        }
    }

    /* Stacking Trunk creation and Setting */
    BCM_TRUNK_STACKING_TID_SET(tid, peer_tmd);
    flags = BCM_TRUNK_FLAG_WITH_ID;
    rv = bcm_trunk_create(unit, flags, &tid);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_trunk_create_id(), rv=%d.\n", rv);
        return rv;
    }

    for (i = 0; i < stk_member_count ; i++) {
        rv = bcm_trunk_member_add(unit, tid, &trunk_port_members[i]);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_trunk_member_add(), rv=%d.\n", rv);
            return rv;
        }
    }

    /* configure reachability: reach tmd via stk_trunk */
    printf("configure_tmd_stk_dyn_functions2(), tmd=%d, peer_tmd=%d, tid=%d.\n", tmd, peer_tmd, tid);
    rv = bcm_stk_domain_stk_trunk_add(unit, peer_tmd /* reached domain */, tid /* via stk_trunk*/);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_stk_domain_stk_trunk_add(), rv=%d.\n", rv);
        return rv;
    }

    return rv;
}

/* Bringup Staking Application: Stacking, Trap, LB-Key, l2 */
int stacking_uc_dyn_func2(int unit, int tmd, int stk_member_count, int out_port_count, int in_local_port, int *stk_port_members, int *out_port_members)
{
    int
        rv = BCM_E_NONE;

    printf("stacking_uc_dyn_func2(), unit=%d, tmd=%d: Calling configure_tmd_stk_dyn_functions2()\n", unit, tmd);
    rv = configure_tmd_stk_dyn_functions2(unit, tmd, stk_member_count, stk_port_members);
    if (rv != BCM_E_NONE) {
        printf("Error, configure_tmd_stk_dyn_functions2(), rv=%d.\n", rv);
        return rv;
    }

    /* Configure Load-Balancing Key */
    printf("stacking_uc_dyn_func2(): Calling stacking_lb_config(), in_local_port=%d\n", in_local_port);
    rv = stacking_lb_config(unit, in_local_port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set(), rv=%d.\n", rv);
        return rv;
    }

    /* Add relevant Stacking Trap */
    printf("stacking_uc_dyn_func2(): Calling stacking_discard_config_traps()\n");
    rv = stacking_discard_config_traps(unit, bcmRxTrapDssStacking);
    if (rv != BCM_E_NONE) {
        printf("Error, stacking_discard_config_traps(), rv=%d.\n", rv);
        return rv;
    }

    /* config mact to send packet to remote sysport */
    printf("stacking_uc_dyn_func2(): Calling stacking_l2_addr_config(), out_port_count=%d\n", out_port_count);
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
int stacking_uc_dyn_appl2(int unit, int tmd)
{
    int
        rv = BCM_E_NONE,
        stk_member_count,
        out_port_count,
        in_local_port,
        stk_port_members[16],
        out_port_members[16];

    stk_member_count = 2;
    stk_port_members[0] = 15;
    stk_port_members[1] = 256 + 15;

    in_local_port = 13;

    out_port_count = 2;
    out_port_members[0] = 256 + 13;
    out_port_members[1] = 256 + 17;

    rv = stacking_uc_dyn_func2(unit, tmd, stk_member_count, out_port_count, in_local_port, stk_port_members, out_port_members);
    if (rv != BCM_E_NONE) {
        printf("Error, stacking_uc_dyn_func2(), rv=%d.\n", rv);
    }

    return rv;
}

