/* $Id: cint_vpls_split_horizon.c,
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/* **************************************************************************************************************************************************
 * This CINT demonstrates Split Horizon under VPLS mp scenario.                                                                                                   *
 *                                                                                                                                                  *
 * The cint creates a Simple Brigde alike scheme.                                                                                                   * 
 *                                                                                                                                                  *
 * The test configuration:                                                                                                                          *
 *      - two ports, Port1 and Port2.                                                                                                               *
 *      - two symmetric LIFs, one AC and one PWE.                                                                                                      *
 *      - MACT contains port+Out-LIF information                                                                                                    *
 *                                                                                                                                                  *
 * The test has four scenarios:                                                                                                                    *
 *                                                                                                                                                  *
 * 1. Split-Horizon filter is disabled:                                                                                                             *
 *                                                                                                                                                  *
 *      - AC---->PWE traffic passed.                                                                                                           *
 *      - PWE---->AC traffic passed                                                                                                           *
 *                                                                                                                                                  *
 * 2. Split-Horizon filter is enabled:                                                                                                              *
 *                                                                                                                                                  *
 *      - AC---->PWE traffic filtered.                                                                                                           *
 *      - PWE---->AC traffic passed                                                                                                           *
 *                                                                                                                                                  *
 * 3. Split-Horizon filter is enabled:                                                                                                              *
 *                                                                                                                                                  *
 *      - AC---->PWE traffic passed.                                                                                                           *
 *      - PWE---->AC traffic filtered                                                                                                           *
 *                                                                                                                                                  * 
 * 4. Split-Horizon filter is enabled:                                                                                                              *
 *                                                                                                                                                  *
 *      - AC---->PWE filtered.                                                                                                           *
 *      - PWE---->AC filtered                                                                                                           *
 */
/*
 * Test Scenario 
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/dnx/vpls/cint_vpls_split_horizon.c
 * cint ../../src/examples/sand/cint_vpls_mp_basic.c
 * cint ../../src/examples/sand/cint_sand_advanced_vlan_translation_mode.c
 * cint ../../src/examples/dnx/utility/cint_dnx_utils_vpls.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint
 * vpls_split_horizon_main(0,200,201,1,2);
 * exit;
 *
 *  1: pwe -> ac 
 * tx 1 psrc=201 data=0x000c0002000100000000cd1d81000000884700d0504000d80140001100000112000c0002000081000005ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *  Received packets on unit 0 should be: 
 *  Source port: 200, Destination port: 201 
 *  Data: 0x00000112000c0002000081000005ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000 
 *
 *  2: ac -> pwe 
 * tx 1 psrc=200 data=0x000c0002000000110000011281000005ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *  Received packets on unit 0 should be: 
 *  Source port: 200, Destination port: 201 
 *  Data: 0x0000cd1d000c000200018100001e884700d050ff00d801ff000c0002000000110000011281000005ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000 
 *
 *  1: pwe -> ac 
 * tx 1 psrc=201 data=0x000c0002000100000000cd1d81000000884700d0504000d80140001100000112000c0002000081000005ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *  Received packets on unit 0 should be: 
 *
 *  2: ac -> pwe 
 * tx 1 psrc=200 data=0x000c0002000000110000011281000005ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *  Received packets on unit 0 should be: 
 *  Source port: 200, Destination port: 201 
 *  Data: 0x0000cd1d000c000200018100001e884700d050ff00d801ff000c0002000000110000011281000005ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000 
 *
 *  1: pwe -> ac 
 * tx 1 psrc=201 data=0x000c0002000100000000cd1d81000000884700d0504000d80140001100000112000c0002000081000005ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *  Received packets on unit 0 should be: 
 *  Source port: 200, Destination port: 201 
 *  Data: 0x00000112000c0002000081000005ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000 
 *
 *  2: ac -> pwe 
 * tx 1 psrc=200 data=0x000c0002000000110000011281000005ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *  Received packets on unit 0 should be: 
 *
 *  1: pwe -> ac 
 * tx 1 psrc=201 data=0x000c0002000100000000cd1d81000000884700d0504000d80140001100000112000c0002000081000005ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *  Received packets on unit 0 should be: 
 *
 *  2: ac -> pwe 
 * tx 1 psrc=200 data=0x000c0002000000110000011281000005ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *  Received packets on unit 0 should be: 
 */

/* Initialization of main struct
 * Function allows to re-write default values 
 *
 * INPUT: 
 *   ac_nwk_group_id: AC nwk group ID
 *   pwe_nwk_group_id: pwe nwk group ID
 */
int
vpls_split_horizon_init(
    int unit,
    int ac_nwk_group_id,
    int pwe_nwk_group_id)
{
    int rv;

    ac_port.nwk_group_valid = 1;
    ac_port.ac_nwk_group_id = ac_nwk_group_id;
    pwe_term.nwk_group_valid = 1;
    pwe_term.pwe_nwk_group_id = pwe_nwk_group_id;
    pwe_encap.nwk_group_valid = 1;
    pwe_encap.pwe_nwk_group_id = pwe_nwk_group_id;

    return BCM_E_NONE;
}

/* Configure the different split-horizon scenario
 *
 * INPUT: 
 *   scenario: scenario type
 */
int
vpls_split_horizon_scenario_config(
    int unit,
    int scenario)
{
    int rv=BCM_E_NONE;


    printf("#######\nscenario = %d\n#######\n", scenario);

    switch (scenario)
    {
        case 0:
            /*
             * AC--->PW No filter:
             * PW--->AC No filter:
             */
            printf("scenario = %d, Split horizon Filter = AC--->PW No filter, PW--->AC No filter\n", scenario);

            bcm_switch_network_group_config_t config;

            config.config_flags = 0;

            config.dest_network_group_id = ac_port.ac_nwk_group_id;

            rv = bcm_switch_network_group_config_set(unit,pwe_term.pwe_nwk_group_id,
                                                     &config);

            if (rv != BCM_E_NONE)
            {
                printf("Error, bcm_switch_network_group_config_set(iteration = %d)\n", i);
                return rv;
            }

            config.dest_network_group_id = pwe_encap.pwe_nwk_group_id;

            rv = bcm_switch_network_group_config_set(unit, ac_port.ac_nwk_group_id,
                                                     &config);

            if (rv != BCM_E_NONE)
            {
                printf("Error, bcm_switch_network_group_config_set(iteration = %d)\n", i);
                return rv;
            }

            break;

        case 1:
            /*
             * AC--->PW no filter:
             * PW--->AC filter:
             */
            printf("scenario = %d, Split horizon Filter = AC--->PW no filter, PW--->AC filter\n", scenario);

            bcm_switch_network_group_config_t config;

            config.config_flags = BCM_SWITCH_NETWORK_GROUP_EGRESS_PRUNE_ENABLE;

            config.dest_network_group_id = ac_port.ac_nwk_group_id;

            rv = bcm_switch_network_group_config_set(unit, pwe_term.pwe_nwk_group_id,
                                                     &config);

            if (rv != BCM_E_NONE)
            {
                printf("Error, bcm_switch_network_group_config_set(iteration = %d)\n", i);
                return rv;
            }

            config.config_flags = 0;

            config.dest_network_group_id = pwe_encap.pwe_nwk_group_id;

            rv = bcm_switch_network_group_config_set(unit, ac_port.ac_nwk_group_id,
                                                     &config);

            if (rv != BCM_E_NONE)
            {
                printf("Error, bcm_switch_network_group_config_set(iteration = %d)\n", i);
                return rv;
            }

            break;

        case 2:

            /*
             * AC--->PW filter:
             * PW--->AC no filter:
             */
            printf("scenario = %d, Split horizon Filter = AC--->PW filter, PW--->AC no filter\n", scenario);
        
            bcm_switch_network_group_config_t config;
        
            config.config_flags = 0;
        
            config.dest_network_group_id = ac_port.ac_nwk_group_id;
        
            rv = bcm_switch_network_group_config_set(unit, pwe_term.pwe_nwk_group_id,
                                                 &config);
        
            if (rv != BCM_E_NONE)
            {
                printf("Error, bcm_switch_network_group_config_set(iteration = %d)\n", i);
                return rv;
            }
        
            config.config_flags = BCM_SWITCH_NETWORK_GROUP_EGRESS_PRUNE_ENABLE;
        
            config.dest_network_group_id = pwe_encap.pwe_nwk_group_id;
        
            rv = bcm_switch_network_group_config_set(unit, ac_port.ac_nwk_group_id,
                                                 &config);
        
            if (rv != BCM_E_NONE)
            {
                printf("Error, bcm_switch_network_group_config_set(iteration = %d)\n", i);
                return rv;
            }
        
            break;

        case 3:

            /*
             * AC--->PW filter:
             * PW--->AC filter:
             */
            printf("scenario = %d, Split horizon Filter = AC--->PW filter, PW--->AC filter\n", scenario);
        
            bcm_switch_network_group_config_t config;
        
            config.config_flags = BCM_SWITCH_NETWORK_GROUP_EGRESS_PRUNE_ENABLE;
        
            config.dest_network_group_id = ac_port.ac_nwk_group_id;
        
            rv = bcm_switch_network_group_config_set(unit, pwe_term.pwe_nwk_group_id,
                                                 &config);
        
            if (rv != BCM_E_NONE)
            {
                printf("Error, bcm_switch_network_group_config_set(iteration = %d)\n", i);
                return rv;
            }
        
            config.config_flags = BCM_SWITCH_NETWORK_GROUP_EGRESS_PRUNE_ENABLE;
        
            config.dest_network_group_id = pwe_encap.pwe_nwk_group_id;
        
            rv = bcm_switch_network_group_config_set(unit, ac_port.ac_nwk_group_id,
                                                     &config);
        
            if (rv != BCM_E_NONE)
            {
                printf("Error, bcm_switch_network_group_config_set(iteration = %d)\n", i);
                return rv;
            } 
        
            break;


        default:

            printf("Error, scenario = %d\n", scenario);

            return BCM_E_PARAM;

            break;

    }

    return rv;
}



/* JR2 dedicated routing:Inherit the network group property of PWE to its valid native AC LIF
 *
 * INPUT: 
 *   scenario: scenario type
 */
int 
vpls_split_horizon_inherit_pwe_nwk_group_to_native_ac(int unit)
{
    int rv = BCM_E_NONE;
    bcm_vlan_port_t default_native_ac;

    /*
     * In JR2,PWE split-horizon perperty is resolved from the native AC related with the PWE.
     * Here, we use the default natvie AC for inheritance because there is no specific native
     * AC is binded to the PWE.
     * If the PWE has its dedicated native AC, its split-horizon property should be configured
     * on that AC.
     */
    bcm_vlan_port_t_init(&default_native_ac);

    default_native_ac.criteria = BCM_VLAN_PORT_MATCH_NONE;
    default_native_ac.flags = BCM_VLAN_PORT_DEFAULT |BCM_VLAN_PORT_NATIVE
                             |BCM_VLAN_PORT_CREATE_EGRESS_ONLY|BCM_VLAN_PORT_VLAN_TRANSLATION;

    default_native_ac.egress_network_group_id = pwe_encap.pwe_nwk_group_id;

    rv = bcm_vlan_port_create(unit, &default_native_ac);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_port_create\n");
        return rv;
    }

    bcm_port_match_info_t match_info;
    match_info.match = BCM_PORT_MATCH_PORT;
    match_info.flags |= BCM_PORT_MATCH_NATIVE|BCM_PORT_MATCH_EGRESS_ONLY;
    match_info.port = pwe_encap.mpls_port_id;
    rv = bcm_port_match_add(unit,default_native_ac.vlan_port_id,&match_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_port_match_add\n");
        return rv;
    }    
    return BCM_E_NONE;
}


int vpls_split_horizon_main(
    int unit,
    int ac_port,
    int pwe_port,
    int ac_nwk_group_id,
    int pwe_nwk_group_id)
{
    int rv = BCM_E_NONE;

    rv = vpls_split_horizon_init(unit, ac_nwk_group_id,pwe_nwk_group_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vpls_split_horizon_init\n");
        return rv;
    }

    /*
     * In JR mode, mpls tunnel is decoded as EEI
     */
    if (soc_property_get(unit, spn_SYSTEM_HEADERS_MODE, 1) == 0)
    {
       mpls_tunnel_in_eei = 1; 
    }    
    rv = vpls_main(unit, ac_port,pwe_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vpls_mp_basic_main\n");
        return rv;
    }

    /* Split-Horizon need to be taken from ETPS TOS , need to configure native ac as well  */ 
    int nwk_grp_from_esem = *dnxc_data_get(unit, "esem", "feature", "etps_properties_assignment", NULL);
	if (nwk_grp_from_esem)
	{
        rv = vpls_split_horizon_inherit_pwe_nwk_group_to_native_ac(unit);
        if (rv != BCM_E_NONE)
        {
            printf("Error, vpls_split_horizon_inherit_pwe_nwk_to_native_ac\n");
            return rv;
        }
    }

    return rv;
}

