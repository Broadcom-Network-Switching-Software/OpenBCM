/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/*
 * Test Scenario: Configure mc resrved profile and attch profile to ports
 * 
 * Example how to run the cint configuration: 
 cint ../../../../src/examples/dpp/utility/cint_utils_rx.c
 cint ../../../../src/examples/dpp/cint_rx_reserved_mc.c
 cint

 int unit = 0;
 print reserved_mc_trap_create_and_set(unit, reserved_mc_0, NULL);

 uint8 profile1 = 1;
 uint8 mac1_6lsbs = 0x0;
 print reserved_mc_profile_set(unit, profile1, mac1_6lsbs, reserved_mc_0);

 uint8 port1 = 200;
 uint8 port2 = 201;
 print reserved_mc_port_profile_set (unit, port1, profile1);
 print reserved_mc_port_profile_set (unit, port2, profile1);

 bcm_rx_trap_config_t trap_to_cpu_config;
 bcm_rx_trap_config_t_init(&trap_to_cpu_config);
 trap_to_cpu_config.flags = BCM_RX_TRAP_UPDATE_DEST;
 trap_to_cpu_config.dest_port = BCM_GPORT_LOCAL_CPU;
 trap_to_cpu_config.trap_strength = 4;
 print reserved_mc_trap_create_and_set(unit, reserved_mc_1, &trap_to_cpu_config);

 uint8 profile2 = 2;
 uint8 mac2_6lsbs = 0x02;
 print reserved_mc_profile_set(unit, profile2, mac2_6lsbs, reserved_mc_1);
 uint8 mac3_6lsbs = 0x05;
 print reserved_mc_profile_set(unit, profile2, mac3_6lsbs, reserved_mc_1);

 uint8 port3 = 202;
 print reserved_mc_port_profile_set (unit, port3, profile2);

 * Example how to clean cint configuration:
 print reserved_mc_port_profile_set(unit, port1, 0);
 print reserved_mc_port_profile_set(unit, port2, 0);
 print reserved_mc_port_profile_set(unit, port3, 0);

 print reserved_mc_clear(unit, profile1, mac1_6lsbs);
 print reserved_mc_clear(unit, profile2, mac2_6lsbs);
 print reserved_mc_clear(unit, profile2, mac3_6lsbs);

 print reserved_mc_trap_destroy(unit, reserved_mc_0);
 print reserved_mc_trap_destroy(unit, reserved_mc_1);
 */

/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 */

/* default trap action configuration to drop packet */
bcm_rx_trap_config_t g_reserved_mc_trap_config =
{
        /* flags */ BCM_RX_TRAP_UPDATE_DEST,
    	/* dest port */ BCM_GPORT_BLACK_HOLE,
        /* dest_group */ 0,
        /* priority */ 0,
        /* color */ 0,
        /* ethernet policer id */ 0,
        /* general policer id */ 0,
        /* counter */ 0,
        /* trap strength */ 4,
        /* snoop command */ 0,
        /* snoop strength */ 3,
        /* forwarding type value */ 0,
        /* forwarding header */ 0,
        /* encap id */ 0,
        /* mirror options in ETPP traps */ NULL,
        /* destinations information per core */ NULL,
        /* core_config_arr length */ 0,
        /* cos profile handle */ 0,
        /* tunnel termination trap strength */ 2,
        /* tunnel termination snoop strength */ 1
};

enum rx_reserved_mc_trap_id_e {
    reserved_mc_0 = 24,
    reserved_mc_1 = 25,
    reserved_mc_2 = 26,
    reserved_mc_3 = 27,
    reserved_mc_4 = 28,
    reserved_mc_5 = 29,
    reserved_mc_6 = 30,
    reserved_mc_7 = 31
};

int g_reserved_mc_trap_id = reserved_mc_0;
bcm_gport_t g_trap_gport[8] = {0};

/*****************************************************************************
* Function:  reserved_mc_trap_create_and_set
* Purpose:   Set mc_reserved trap action
* Params:
* unit (IN) - Device Number
* reserved_mc_trap_id (IN) - trap_id of mc_reserved trap
* trap_config (IN) - Trap strengths and action configuration
* Return:    (int)
 */
int reserved_mc_trap_create_and_set(
    int unit, 
    rx_reserved_mc_trap_id_e reserved_mc_trap_id,
    bcm_rx_trap_config_t *trap_config)
{
    int trap_create_flags = BCM_RX_TRAP_WITH_ID; /* RESERVED MC traps are created with id */
    uint8 reserved_mc_index = reserved_mc_trap_id - reserved_mc_0;

    if (trap_config != NULL)
    {
        sal_memcpy(&g_reserved_mc_trap_config, trap_config, sizeof(g_reserved_mc_trap_config));
    }

    BCM_GPORT_TRAP_SET(g_trap_gport[reserved_mc_index], reserved_mc_trap_id, g_reserved_mc_trap_config.trap_strength,
                                                                             g_reserved_mc_trap_config.snoop_strength);

    return rx__trap_create_and_set(unit, bcmRxTrapL2Cache, trap_create_flags, &g_reserved_mc_trap_config, 
                                   &reserved_mc_trap_id);
}

/*****************************************************************************
* Function:  reserved_mc_trap_destroy
* Purpose:   Destroy the created trap
* Params:
* unit (IN)     - Device Number
* reserved_mc_trap_id (IN) - trap_id of mc_reserved trap
* Return:    (int)
 */
int reserved_mc_trap_destroy(
    int unit,
    rx_reserved_mc_trap_id_e reserved_mc_trap_id)
{
    int rv = BCM_E_NONE;

    /*delete trap*/
    rv = rx__trap_destroy(unit, reserved_mc_trap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in : reserved_mc_trap_destroy, returned %d\n", rv);
        return rv;
    }

    printf("Trap destroyed successfully\n");
    return rv;
}

/*****************************************************************************
* Function:  reserved_mc_profile_set
* Purpose:   Add mac to profile
* Params:
* unit (IN) - Device Number
* profile (IN) - profile to configure (1-3)
* mac_6lsbs (IN) - 6 LSBs of mac to add (01:80:c2:xx:xx:xx)
* reserved_mc_trap_id (IN) - trap_id of mc_reserved trap
* Return:    (int)
 */
int reserved_mc_profile_set(
    int unit,
    uint8 profile,
    uint8 mac_6lsbs,
    rx_reserved_mc_trap_id_e reserved_mc_trap_id)
{
    bcm_rx_trap_protocol_key_t key;
    uint8 reserved_mc_index = reserved_mc_trap_id - reserved_mc_0;;
    
    key.protocol_type = bcmRxTrapL2cpPeer;
    key.protocol_trap_profile = profile;
    key.trap_args = mac_6lsbs;
    
    return bcm_rx_trap_protocol_set(unit, &key, g_trap_gport[reserved_mc_index]);
}

/*****************************************************************************
* Function:  reserved_mc_port_profile_set
* Purpose:   Set profile to given port
* Params:
* unit (IN) - Device Number
* port (IN) - port (0-255)
* profile (IN) - profile to configure (1-3)
* Return:    (int)
 */
int reserved_mc_port_profile_set(
    int unit,
    bcm_port_t port,
    uint8 profile)
{
    bcm_rx_trap_protocol_profiles_t protocol_profiles;
    
    protocol_profiles.l2cp_trap_profile = profile;
    return bcm_rx_trap_protocol_profiles_set(unit, port, &protocol_profiles);
}

/*****************************************************************************
* Function:  reserved_mc_clear
* Purpose:   Set profile to given port
* Params:
* unit (IN) - Device Number
* Return:    (int)
 */
int reserved_mc_clear(
    int unit,
    uint8 profile,
    uint8 mac_6lsbs)
{
    int rv = BCM_E_NONE;
    bcm_rx_trap_protocol_key_t key;

    key.protocol_type = bcmRxTrapL2cpPeer;
    key.protocol_trap_profile = profile;
    key.trap_args = mac_6lsbs;
    
    rv = bcm_rx_trap_protocol_clear(unit, &key);
    if (rv != BCM_E_NONE)
    {
        printf("Error in : protocol clear - profile=%d, mac_6lsbs = %d\n", profile, mac_6lsbs);
        return rv;
    }

    return rv;
}
