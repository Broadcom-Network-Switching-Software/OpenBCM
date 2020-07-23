/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/*
 * Test Scenario: Configure ipv4 tunnel termination and forwarding ttl0/1 traps strengths
 * 
 * how to run the test: 
  cint ../../../../src/examples/dpp/utility/cint_utils_rx.c
  cint ../../../../src/examples/dpp/cint_rx_ipv4_tunnel_termination_forwarding_ttl0_1.c
  cint

  ipv4_ingress__start_run(0,0,NULL);
  ipv4_ingress__start_run_with_params(0,0,BCM_RX_TRAP_UPDATE_TUNNEL_TERMINATION_STRENGTH | BCM_RX_TRAP_UPDATE_DEST,
                                                           BCM_GPORT_BLACK_HOLE, 0, 0, 2, 2);

  for get use:
  ipv4_ingress_trap_get_and_validate(0,0);
  ipv4_ingress_trap_get_global_config(0,&trap_config);

  
  for destroy use:
  ipv4_ingress_trap_destroy(0,0);
 */

/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 */

bcm_rx_trap_config_t g_ipv4_trap_config[2] =
{
    {
        /* flags */ BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_UPDATE_TUNNEL_TERMINATION_STRENGTH,
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
    },
    {
        /* flags */ BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_UPDATE_TUNNEL_TERMINATION_STRENGTH,
        /* dest port */ BCM_GPORT_BLACK_HOLE,
        /* dest_group */ 0,
        /* priority */ 0,
        /* color */ 0,
        /* ethernet policer id */ 0,
        /* general policer id */ 0,
        /* counter */ 0,
        /* trap strength */ 3,
        /* snoop command */ 0,
        /* snoop strength */ 1,
        /* forwarding type value */ 0,
        /* forwarding header */ 0,
        /* encap id */ 0,
        /* mirror options in ETPP traps */ NULL,
        /* destinations information per core */ NULL,
        /* core_config_arr length */ 0,
        /* cos profile handle */ 0,
        /* tunnel termination trap strength */ 4,
        /* tunnel termination snoop strength */ 2
    }
};


int g_ipv4_trap_id[2] = {0};
int g_ipv4_trap_types[2] = {bcmRxTrapIpv4Ttl0, bcmRxTrapIpv4Ttl1};

/*****************************************************************************
* Function:  ipv4_ingress_trap_destroy
* Purpose:   Destroy the created trap
* Params:
* unit (IN)     - Device Number 
* ttl (IN) - Time to leave (0 or 1)
* Return:    (int)
 */
int ipv4_ingress_trap_destroy(int unit, int ttl)
{
    int rv = BCM_E_NONE;

    /*delete trap*/
    rv = rx__trap_destroy(unit, g_ipv4_trap_id[ttl]);
    if (rv != BCM_E_NONE)
    {
        printf("Error in : ipv4_ingress_trap_destroy, returned %d\n", rv);
        return rv;
    }

    printf("Trap destroyed successfully\n");
    return rv;
}

/*****************************************************************************
* Function:  ipv4_ingress_trap_get_and validate
* Purpose:   Get configuration of Ipv4 ttl0/1 Ingress Trap from registers and compare to global
* Params:
* unit (IN) - Device Number
* ttl (IN) - Time to leave (0 or 1)
* Return:    (int)
 */
int ipv4_ingress_trap_get_and_validate(int unit, int ttl)
{
    int rv = BCM_E_NONE;
    bcm_rx_trap_config_t trap_config = {0};

    rv = rx__trap_get(unit, g_ipv4_trap_id[ttl], &trap_config);
    if (rv != BCM_E_NONE)
    {
        printf("Error getting trap configuration, in rx__trap_get\n");
        return rv;
    }

    printf("Trap bcmRxTrapIpv4Ttl%d configuration:\n", ttl);
    printf("Trap strength: %d\n", trap_config.trap_strength);
    printf("Snoop strength: %d\n", trap_config.snoop_strength);

    if (trap_config.flags & BCM_RX_TRAP_UPDATE_TUNNEL_TERMINATION_STRENGTH)
    {
        printf("Tunnel termination strengths are configured\n");
        printf("Tunnel termination trap strength: %d\n", trap_config.tunnel_termination_trap_strength);
        printf("Tunnel termination snoop strength: %d\n", trap_config.tunnel_termination_snoop_strength);
    }
    else
    {
        printf("Tunnel termination strengths are not configured!\n");
    }

    return rx__trap_config_compare(&trap_config, &g_ipv4_trap_config[ttl]);
}

/*****************************************************************************
* Function:  ipv4_ingress_trap_get_global_config
* Purpose:   Get configuration of Ipv4 ttl0/1 Ingress Trap from the global configuration
* Params:
* ttl (IN) - Time to leave (0 or 1)
* g_config (OUT) - configuration retrived
* Return:    (int)
 */
void ipv4_ingress_trap_get_global_config(int ttl, bcm_rx_trap_config_t* g_config)
{
    sal_memcpy(g_config, &g_ipv4_trap_config[ttl], sizeof(g_ipv4_trap_config[ttl]));
}

/*Main Functions*/

/*****************************************************************************
* Function:  ipv4_ingress__start_run
* Purpose:   Main function run default configuration for setting all needed
* values for Ipv4 ttl0/1 Ingress Trap
* Params:
* unit (IN) - Device Number
* ttl (IN) - Time to leave (0 or 1)
* Return:    (int)
 */
int ipv4_ingress__start_run(int unit, int ttl, bcm_rx_trap_config_t *params)
{
    int rv = BCM_E_NONE;
    int trap_create_flags = 0; /*Trap create flags*/

    if (params != NULL)
    {
        sal_memcpy(&g_ipv4_trap_config[ttl], params, sizeof(g_ipv4_trap_config[ttl]));
    }

    rv = rx__trap_create_and_set(unit, g_ipv4_trap_types[ttl], trap_create_flags,
                                        &g_ipv4_trap_config[ttl], &g_ipv4_trap_id[ttl]);
    if (rv != BCM_E_NONE)
    {
        printf("Error configuring bcmRxTrapIpv4Ttl%d, in rx__trap_create_and_set\n", ttl);
        return rv;
    }

    printf("Trap bcmRxTrapIpv4Ttl%d was created and set \n", ttl);
    return rv;
}

/*****************************************************************************
* Function:  ipv4_ingress__start_run_with_parmas
* Purpose:   Main function run inserted configuration for setting all needed
*                 values for Ipv4 ttl0/1 Ingress Trap
* Params:
* unit (IN) - Device Number
* ttl (IN) - Time to leave (0 or 1)
* flags (IN)     - Configuration flags
* dest_port (IN) - Destination port
* trap_strength (IN) - Trap strength
* snoop_strength (IN) - Snoop strength
* tunnel_termination_trap_strength (IN) - Trap strength
* tunnel_termination_snoop_strength (IN) - Snoop strength
* Return:    (int)
 */

int ipv4_ingress__start_run_with_params(int unit, int ttl, uint32 flags, int dest_port,
                                                    int trap_strength, int snoop_strength,
                                                    int tunnel_termination_trap_strength, int tunnel_termination_snoop_strength)
{
    bcm_rx_trap_config_t trap_config;

    ipv4_ingress_trap_get_global_config(ttl, &trap_config);
    trap_config.flags = flags;
    trap_config.dest_port = dest_port;
    trap_config.trap_strength = trap_strength;
    trap_config.snoop_strength = snoop_strength;
    trap_config.tunnel_termination_trap_strength = tunnel_termination_trap_strength;
    trap_config.tunnel_termination_snoop_strength = tunnel_termination_snoop_strength;

    return ipv4_ingress__start_run(unit, ttl, &trap_config);
}
