/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/*
 * Test Scenario: Configure independent mpls tunnel termination and forwarding ttl0/1 traps
 * 
 * how to run the test: 
  cint ../../../../src/examples/dpp/utility/cint_utils_rx.c
  cint ../../../../src/examples/dpp/cint_rx_mpls_tunnel_termination_forwarding_ttl0_1.c
  cint

  mpls_tunnel_termination_ingress__start_run(0,0,NULL);
  mpls_tunnel_termination_ingress__start_run_with_params(0,0,BCM_RX_TRAP_UPDATE_DEST,BCM_GPORT_BLACK_HOLE,1,1);
  mpls_forwarding_ingress__start_run(0,0,NULL);
  mpls_forwarding_ingress__start_run_with_params(0,0,BCM_RX_TRAP_UPDATE_DEST,BCM_GPORT_BLACK_HOLE,2,2);
  mpls_ingress__start_run(0,0,NULL);
  mpls_ingress__start_run_with_params(0,0,BCM_RX_TRAP_UPDATE_DEST,BCM_GPORT_BLACK_HOLE,3,3);

  for get use:
  mpls_tunnel_termination_ingress_trap_get_and_validate(0,0);
  mpls_tunnel_termination_ingress_trap_get_global_config(0,&trap_config);
  mpls_forwarding_ingress_trap_get_and_validate(0,0);
  mpls_forwarding_ingress_trap_get_global_config(0,&trap_config);
  mpls_ingress_trap_get_and_validate(0,0);
  mpls_ingress_trap_get_global_config(0,&trap_config);

  for destroy use:
  mpls_tunnel_termination_ingress_trap_destroy(0,0);
  mpls_forwarding_ingress_trap_destroy(0,0);
  mpls_ttl0_ingress_trap_destroy(0,0);
 */

/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 */

bcm_rx_trap_config_t g_mpls_tunnel_termination_trap_config[2] =
{
    {
        /* flags */ BCM_RX_TRAP_UPDATE_DEST,
    	/* dest port */ BCM_GPORT_BLACK_HOLE,
        /* dest_group */ 0,
        /* priority */ 0,
        /* color */ 0,
        /* ethernet policer id */ 0,
        /* general policer id */ 0,
        /* counter */ 0,
        /* trap strength */ 1,
        /* snoop command */ 0,
        /* snoop strength */ 1,
        /* forwarding type value */ 0,
        /* forwarding header */ 0,
        /* encap id */ 0,
        /* mirror options in ETPP traps */ NULL,
        /* destinations information per core */ NULL,
        /* core_config_arr length */ 0,
        /* cos profile handle */ 0,
        /* tunnel termination trap strength */ 0,
        /* tunnel termination snoop strength */ 0
    },
    {
        /* flags */ BCM_RX_TRAP_UPDATE_DEST,
        /* dest port */ BCM_GPORT_BLACK_HOLE,
        /* dest_group */ 0,
        /* priority */ 0,
        /* color */ 0,
        /* ethernet policer id */ 0,
        /* general policer id */ 0,
        /* counter */ 0,
        /* trap strength */ 1,
        /* snoop command */ 0,
        /* snoop strength */ 1,
        /* forwarding type value */ 0,
        /* forwarding header */ 0,
        /* encap id */ 0,
        /* mirror options in ETPP traps */ NULL,
        /* destinations information per core */ NULL,
        /* core_config_arr length */ 0,
        /* cos profile handle */ 0,
        /* tunnel termination trap strength */ 0,
        /* tunnel termination snoop strength */ 0
    }
};

bcm_rx_trap_config_t g_mpls_forwarding_trap_config[2] =
{
    {
        /* flags */ BCM_RX_TRAP_UPDATE_DEST,
    	/* dest port */ BCM_GPORT_BLACK_HOLE,
        /* dest_group */ 0,
        /* priority */ 0,
        /* color */ 0,
        /* ethernet policer id */ 0,
        /* general policer id */ 0,
        /* counter */ 0,
        /* trap strength */ 2,
        /* snoop command */ 0,
        /* snoop strength */ 2,
        /* forwarding type value */ 0,
        /* forwarding header */ 0,
        /* encap id */ 0,
        /* mirror options in ETPP traps */ NULL,
        /* destinations information per core */ NULL,
        /* core_config_arr length */ 0,
        /* cos profile handle */ 0,
        /* tunnel termination trap strength */ 0,
        /* tunnel termination snoop strength */ 0
    },
    {
        /* flags */ BCM_RX_TRAP_UPDATE_DEST,
    	/* dest port */ BCM_GPORT_BLACK_HOLE,
        /* dest_group */ 0,
        /* priority */ 0,
        /* color */ 0,
        /* ethernet policer id */ 0,
        /* general policer id */ 0,
        /* counter */ 0,
        /* trap strength */ 2,
        /* snoop command */ 0,
        /* snoop strength */ 2,
        /* forwarding type value */ 0,
        /* forwarding header */ 0,
        /* encap id */ 0,
        /* mirror options in ETPP traps */ NULL,
        /* destinations information per core */ NULL,
        /* core_config_arr length */ 0,
        /* cos profile handle */ 0,
        /* tunnel termination trap strength */ 0,
        /* tunnel termination snoop strength */ 0
    }
};

bcm_rx_trap_config_t g_mpls_trap_config[2] =
{
    {
        /* flags */ BCM_RX_TRAP_UPDATE_DEST,
    	/* dest port */ BCM_GPORT_BLACK_HOLE,
        /* dest_group */ 0,
        /* priority */ 0,
        /* color */ 0,
        /* ethernet policer id */ 0,
        /* general policer id */ 0,
        /* counter */ 0,
        /* trap strength */ 3,
        /* snoop command */ 0,
        /* snoop strength */ 3,
        /* forwarding type value */ 0,
        /* forwarding header */ 0,
        /* encap id */ 0,
        /* mirror options in ETPP traps */ NULL,
        /* destinations information per core */ NULL,
        /* core_config_arr length */ 0,
        /* cos profile handle */ 0,
        /* tunnel termination trap strength */ 0,
        /* tunnel termination snoop strength */ 0
    },
    {
        /* flags */ BCM_RX_TRAP_UPDATE_DEST,
    	/* dest port */ BCM_GPORT_BLACK_HOLE,
        /* dest_group */ 0,
        /* priority */ 0,
        /* color */ 0,
        /* ethernet policer id */ 0,
        /* general policer id */ 0,
        /* counter */ 0,
        /* trap strength */ 3,
        /* snoop command */ 0,
        /* snoop strength */ 3,
        /* forwarding type value */ 0,
        /* forwarding header */ 0,
        /* encap id */ 0,
        /* mirror options in ETPP traps */ NULL,
        /* destinations information per core */ NULL,
        /* core_config_arr length */ 0,
        /* cos profile handle */ 0,
        /* tunnel termination trap strength */ 0,
        /* tunnel termination snoop strength */ 0
    }
};

int g_mpls_tunnel_termination_trap_id[2] = {0};
int g_mpls_forwarding_trap_id[2] = {0};
int g_mpls_trap_id[2] = {0};

int g_mpls_tunnel_termination_trap_types[2] = {bcmRxTrapMplsTunnelTerminationTtl0, bcmRxTrapMplsTunnelTerminationTtl1};
int g_mpls_forwarding_trap_types[2] = {bcmRxTrapMplsForwardingTtl0, bcmRxTrapMplsForwardingTtl1};
int g_mpls_trap_types[2] = {bcmRxTrapMplsTtl0, bcmRxTrapMplsTtl1};

/*****************************************************************************
* Function:  mpls_tunnel_termination_ingress_trap_destroy
* Purpose:   Destroy the created trap
* Params:
* unit (IN)     - Device Number
* ttl (IN) - Time to leave (0 or 1)
* Return:    (int)
 */
int mpls_tunnel_termination_ingress_trap_destroy(int unit, int ttl)
{
    int rv = BCM_E_NONE;

    /*delete trap*/
    rv = rx__trap_destroy(unit, g_mpls_tunnel_termination_trap_id[ttl]);
    if (rv != BCM_E_NONE)
    {
        printf("Error in : mpls_tunnel_termination_ingress_trap_destroy, returned %d\n", rv);
        return rv;
    }

    printf("Trap destroyed successfully\n");
    return rv;
}

/*****************************************************************************
* Function:  mpls_forwarding_ingress_trap_destroy
* Purpose:   Destroy the created trap
* Params:
* unit (IN)     - Device Number
* ttl (IN) - Time to leave (0 or 1)
* Return:    (int)
 */
int mpls_forwarding_ingress_trap_destroy(int unit, int ttl)
{
    int rv = BCM_E_NONE;

    /*delete trap*/
    rv = rx__trap_destroy(unit, g_mpls_forwarding_trap_id[ttl]);
    if (rv != BCM_E_NONE)
    {
        printf("Error in : mpls_forwarding_ingress_trap_destroy, returned %d\n", rv);
        return rv;
    }

    printf("Trap destroyed successfully\n");
    return rv;
}

/*****************************************************************************
* Function:  mpls_ingress_trap_destroy
* Purpose:   Destroy the created trap
* Params:
* unit (IN)     - Device Number
* ttl (IN) - Time to leave (0 or 1)
* Return:    (int)
 */
int mpls_ingress_trap_destroy(int unit, int ttl)
{
    int rv = BCM_E_NONE;

    /*delete trap*/
    rv = rx__trap_destroy(unit, g_mpls_trap_id[ttl]);
    if (rv != BCM_E_NONE)
    {
        printf("Error in : mpls_ingress_trap_destroy, returned %d\n", rv);
        return rv;
    }

    printf("Trap destroyed successfully\n");
    return rv;
}

/*****************************************************************************
* Function:  mpls_tunnel_termination_ingress_trap_get
* Purpose:   Get configuration of MPLS tunnel termination ttl0/1 Ingress Trap from registers and
   compare to global configuration
* Params:
* unit (IN) - Device Number
* ttl (IN) - Time to leave (0 or 1)
* Return:    (int)
 */
int mpls_tunnel_termination_ingress_trap_get_and_validate(int unit, int ttl)
{
    int rv = BCM_E_NONE;
    bcm_rx_trap_config_t trap_config = {0};

    rv = rx__trap_get(unit, g_mpls_tunnel_termination_trap_id[ttl], &trap_config);
    if (rv != BCM_E_NONE)
    {
        printf("Error getting trap configuration, in rx__trap_get\n");
        return rv;
    }

    printf("Trap bcmRxTrapMplsTunnelTerminationTtl%d configuration:\n", ttl);
    printf("Trap strength: %d\n", trap_config.trap_strength);
    printf("Snoop strength: %d\n", trap_config.snoop_strength);
    return rx__trap_config_compare(&trap_config, &g_mpls_tunnel_termination_trap_config[ttl]);
}

/*****************************************************************************
* Function:  mpls_tunnel_termination_ingress_trap_get_global_config
* Purpose:   Get configuration of mpls tunnel termination ttl0/1 Ingress Trap from the global configuration
* Params:
* ttl (IN) - Time to leave (0 or 1)
* g_config (OUT) - Configuration retrived
* Return:    (int)
 */
void mpls_tunnel_termination_ingress_trap_get_global_config(int ttl, bcm_rx_trap_config_t* g_config)
{
    sal_memcpy(g_config, &g_mpls_tunnel_termination_trap_config[ttl], sizeof(g_mpls_tunnel_termination_trap_config[ttl]));
}


/*****************************************************************************
* Function:  mpls_forwarding_ingress_trap_get
* Purpose:   Get configuration of MPLS forwarding ttl0/1 Ingress Trap
* Params:
* unit (IN) - Device Number
* ttl (IN) - Time to leave (0 or 1)
* Return:    (int)
 */
int mpls_forwarding_ingress_trap_get_and_validate(int unit, int ttl)
{
    int rv = BCM_E_NONE;
    bcm_rx_trap_config_t trap_config = {0};

    rv = rx__trap_get(unit, g_mpls_forwarding_trap_id[ttl], &trap_config);
    if (rv != BCM_E_NONE)
    {
        printf("Error getting trap configuration, in rx__trap_get\n");
        return rv;
    }

    printf("Trap bcmRxTrapMplsForwardingTtl%d configuration:\n", ttl);
    printf("Trap strength: %d\n", trap_config.trap_strength);
    printf("Snoop strength: %d\n", trap_config.snoop_strength);
    return rx__trap_config_compare(&trap_config, &g_mpls_forwarding_trap_config[ttl]);
}

/*****************************************************************************
* Function:  mpls_forwarding_ingress_trap_get_global_config
* Purpose:   Get configuration of mpls forwarding ttl0/1 Ingress Trap from the global configuration
* Params:
* ttl (IN) - Time to leave (0 or 1)
* g_config (OUT) - Configuration retrived
* Return:    (int)
 */
void mpls_forwarding_ingress_trap_get_global_config(int ttl, bcm_rx_trap_config_t* g_config)
{
    sal_memcpy(g_config, &g_mpls_forwarding_trap_config[ttl], sizeof(g_mpls_forwarding_trap_config[ttl]));
}

/*****************************************************************************
* Function:  mpls_ingress_trap_get
* Purpose:   Get configuration of MPLS ttl0/1 Ingress Trap
* Params:
* unit (IN) - Device Number
* ttl (IN) - Time to leave (0 or 1)
* Return:    (int)
 */
int mpls_ingress_trap_get_and_validate(int unit, int ttl)
{
    int rv = BCM_E_NONE;
    bcm_rx_trap_config_t trap_config = {0};

    rv = rx__trap_get(unit, g_mpls_trap_id[ttl], &trap_config);
    if (rv != BCM_E_NONE)
    {
        printf("Error getting trap configuration, in rx__trap_get\n");
        return rv;
    }

    printf("Trap bcmRxTrapMplsTtl%d configuration:\n", ttl);
    printf("Trap strength: %d\n", trap_config.trap_strength);
    printf("Snoop strength: %d\n", trap_config.snoop_strength);
    return rx__trap_config_compare(&trap_config, &g_mpls_trap_config[ttl]);
}

/*****************************************************************************
* Function:  mpls_ingress_trap_get_global_config
* Purpose:   Get configuration of mpls ttl0/1 Ingress Trap from the global configuration
* Params:
* ttl (IN) - Time to leave (0 or 1)
* g_config (OUT) - Configuration retrived
* Return:    (int)
 */
void mpls_ingress_trap_get_global_config(int ttl, bcm_rx_trap_config_t* g_config)
{
    sal_memcpy(g_config, &g_mpls_trap_config[ttl], sizeof(g_mpls_trap_config[ttl]));
}

/*Main Functions*/

/*****************************************************************************
* Function:  mpls_tunnel_termination_ingress__start_run
* Purpose:   Main function run default configuration for setting all needed values for MPLS
*                 tunnel termination ttl0/1 Ingress Trap
* Params:
* unit (IN) - Device Number
* ttl (IN) - Time to leave (0 or 1)
* params (IN) - Trap configuration parameters
* Return:    (int)
 */
int mpls_tunnel_termination_ingress__start_run(int unit, int ttl, bcm_rx_trap_config_t *params)
{
    int rv = BCM_E_NONE;
    int trap_create_flags = 0; /*Trap create flags*/

    if (params != NULL)
    {
        sal_memcpy(&g_mpls_tunnel_termination_trap_config[ttl], params, sizeof(g_mpls_tunnel_termination_trap_config[ttl]));
    }

    rv = rx__trap_create_and_set(unit, g_mpls_tunnel_termination_trap_types[ttl], trap_create_flags,
                                        &g_mpls_tunnel_termination_trap_config[ttl], &g_mpls_tunnel_termination_trap_id[ttl]);
    if (rv != BCM_E_NONE)
    {
        printf("Error configuring bcmRxTrapMplsTunnelTerminationTtl%d, in rx__trap_create_and_set\n", ttl);
        return rv;
    }

    printf("Trap bcmRxTrapMplsTunnelTerminationTtl%d was created and set \n", ttl);
    return rv;
}

/*****************************************************************************
* Function: mpls_tunnel_termination_ingress__start_run_with_params
* Purpose: Main function run default configuration for setting all needed values for MPLS tunnel 
*               termination ttl0/1 Ingress Trap with parameters
* Params:
* unit (IN) - Device Number
* ttl (IN) - Time to leave (0 or 1)
* flags (IN)     - Configuration flags
* dest_port (IN) - Destination port
* trap_strength (IN) - Trap strength
* snoop_strength (IN) - Snoop strength
* Return:    (int)
 */

int mpls_tunnel_termination_ingress__start_run_with_params(int unit, int ttl, uint32 flags, int dest_port, int trap_strength, int snoop_strength)
{
    bcm_rx_trap_config_t trap_config;

    mpls_tunnel_termination_ingress_trap_get_global_config(ttl, &trap_config);
    trap_config.flags = flags;
    trap_config.dest_port = dest_port;
    trap_config.trap_strength = trap_strength;
    trap_config.snoop_strength = snoop_strength;

    return mpls_tunnel_termination_ingress__start_run(unit, ttl, &trap_config);
}

/*****************************************************************************
* Function:  mpls_forwarding_ingress__start_run
* Purpose:   Main function run default configuration for setting all needed values for MPLS 
*                 forwarding ttl0/1 Ingress Trap
* Params:
* unit (IN) - Device Number
* ttl (IN) - Time to leave (0 or 1)
* params (IN) - Trap configuration parameters
* Return:    (int)
 */
int mpls_forwarding_ingress__start_run(int unit, int ttl, bcm_rx_trap_config_t *params)
{
    int rv = BCM_E_NONE;
    int trap_create_flags = 0; /*Trap create flags*/

    if (params != NULL)
    {
        sal_memcpy(&g_mpls_forwarding_trap_config[ttl], params, sizeof(g_mpls_forwarding_trap_config[ttl]));
    }

    rv = rx__trap_create_and_set(unit, g_mpls_forwarding_trap_types[ttl], trap_create_flags,
                                        &g_mpls_forwarding_trap_config[ttl], &g_mpls_forwarding_trap_id[ttl]);
    if (rv != BCM_E_NONE)
    {
        printf("Error configuring bcmRxTrapMplsForwardingTtl%d, in rx__trap_create_and_set\n", ttl);
        return rv;
    }

    printf("Trap bcmRxTrapMplsForwardingTtl%d was created and set \n", ttl);
    return rv;
}

/*****************************************************************************
* Function: mpls_forwarding_ingress__start_run_with_params
* Purpose: Main function run default configuration for setting all needed values for MPLS
*               forwarding ttl0/1 Ingress Trap with parameters
* Params:
* unit (IN) - Device Number
* ttl (IN) - Time to leave (0 or 1)
* flags (IN)     - Configuration flags
* dest_port (IN) - Destination port
* trap_strength (IN) - Trap strength
* snoop_strength (IN) - Snoop strength
* Return:    (int)
 */

int mpls_forwarding_ingress__start_run_with_params(int unit, int ttl, uint32 flags, int dest_port, int trap_strength, int snoop_strength)
{
    bcm_rx_trap_config_t trap_config;

    mpls_forwarding_ingress_trap_get_global_config(ttl, &trap_config);
    trap_config.flags = flags;
    trap_config.dest_port = dest_port;
    trap_config.trap_strength = trap_strength;
    trap_config.snoop_strength = snoop_strength;

    return mpls_forwarding_ingress__start_run(unit, ttl, &trap_config);
}

/*****************************************************************************
* Function:  mpls_ingress__start_run
* Purpose:   Main function run default configuration for setting all needed values for MPLS 
*                 ttl0/1 Ingress Trap
* Params:
* unit (IN) - Device Number
* ttl (IN) - Time to leave (0 or 1)
* params (IN) - Trap configuration parameters
* Return:    (int)
 */
int mpls_ingress__start_run(int unit, int ttl, bcm_rx_trap_config_t *params)
{
    int rv = BCM_E_NONE;
    int trap_create_flags = 0; /*Trap create flags*/

    if (params != NULL)
    {
        sal_memcpy(&g_mpls_trap_config[ttl], params, sizeof(g_mpls_forwarding_trap_config[ttl]));
    }

    rv = rx__trap_create_and_set(unit, g_mpls_trap_types[ttl], trap_create_flags,
                                        &g_mpls_trap_config[ttl], &g_mpls_trap_id[ttl]);
    if (rv != BCM_E_NONE)
    {
        printf("Error configuring bcmRxTrapMplsTtl%d, in rx__trap_create_and_set\n", ttl);
        return rv;
    }

    printf("Trap bcmRxTrapMplsTtl%d was created and set \n", ttl);
    return rv;
}

/*****************************************************************************
* Function: mpls_ingress__start_run_with_params
* Purpose: Main function run default configuration for setting all needed values for MPLS 
*               ttl0/1 Ingress Trap with parameters
* Params:
* unit (IN) - Device Number
* ttl (IN) - Time to leave (0 or 1)
* flags (IN)     - Configuration flags
* dest_port (IN) - Destination port
* trap_strength (IN) - Trap strength
* snoop_strength (IN) - Snoop strength
* Return:    (int)
 */

int mpls_ingress__start_run_with_params(int unit, int ttl, uint32 flags, int dest_port, int trap_strength, int snoop_strength)
{
    bcm_rx_trap_config_t trap_config;

    mpls_ingress_trap_get_global_config(ttl, &trap_config);
    trap_config.flags = flags;
    trap_config.dest_port = dest_port;
    trap_config.trap_strength = trap_strength;
    trap_config.snoop_strength = snoop_strength;

    return mpls_ingress__start_run(unit, ttl, &trap_config);
}
