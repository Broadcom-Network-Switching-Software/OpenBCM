/* $Id:
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_rx_trap_programmable_traps.c
 * Purpose: Shows an example for configuration of programmable traps - Matching and Mismatching configuration
 *
 * cint ../../../../src/examples/dnx/traps/cint_rx_trap_programmable_traps.c
 *
 * Main Function:
 *      cint_rx_trap_programmable_traps_main(unit,fwd_strength,prog_config_ipv4_fwd,prog_config_eth_fwd,trap_type,trap_id,is_ipv4_fwd);
 * Destroy Function:
 *      cint_rx_trap_programmable_traps_destroy(unit,trap_id);
 *
 *
 * Example Config:
 * cint ../../../../src/examples/dnx/traps/cint_utils_rx.c
 * cint ../../../../src/examples/dnx/traps/cint_rx_trap_programmable_traps.c
 * cint
 * cint_rx_trap_programmable_traps_main(0,7,NULL,NULL,bcmRxTrapEgTxSplitHorizonFilter,0x120,TRUE);
 */


uint8 cint_rx_trap_programmable_traps_index_eth = 0;
uint8 cint_rx_trap_programmable_traps_index_ipv4 = 1;

/** Get trap ID for test compare */
int cint_rx_trap_programmable_traps_get_trap_id;

bcm_rx_trap_prog_config_t cint_rx_trap_programmable_traps_config_eth = {

    /* Source MAC enabler of Eth Forwarding packet to qualify */
     bcmRxTrapProgEnableMismatch,

    /* Source MAC of Eth Forwarding packet to qualify */
     {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF},

    /* Number of bits from Source MAC of Eth Forwarding packet to qualify */
     48,

    /* Destination MAC enabler of Eth Forwarding packet to qualify */
     bcmRxTrapProgEnableMismatch,

    /* Destination MAC of Eth Forwarding packet to qualify */
     {0x11, 0x22, 0x33, 0x44, 0x55, 0x66},

    /*Number of bits from Destination MAC of Eth Forwarding packet to qualify */
     48,

    /* Ether Type enabler of Eth Forwarding packet to qualify */
     bcmRxTrapProgEnableMatch,

    /* Ether Type of Eth Forwarding packet to qualify */
     0x0800,

    /* Sub Type enabler of Eth Forwarding packet to qualify */
     bcmRxTrapProgDisable,

    /* Sub Type of Eth Forwarding packet to qualify */
     0,

    /* Sub Type mask of Eth Forwarding packet to qualify */
     0,

    /* IP Protocol field enabler for IPv4 Forwarding packet to qualify */
    bcmRxTrapProgDisable,

    /* IP protocol field for IPv4 Forwarding packet to qualify */
     0,

    /* L4 Ports enabler for IPv4 Forwarding packet to qualify */
     bcmRxTrapProgDisable,

    /* L4 Src Port for IPv4 Forwarding packet to qualify */
     0x0,

    /* L4 Src Port Mask for IPv4 Forwarding packet to qualify */
     0x0,

    /* L4 Dest Port for IPv4 Forwarding packet to qualify */
     0x0,

    /* L4 Dest Port Mask for IPv4 Forwarding packet to qualify */
     0x0,

    /* TCP Flags enabler for IPv4 Forwarding packet to qualify */
     bcmRxTrapProgDisable,

    /* TCP Flags for IPv4 Forwarding packet to qualify */
     0x0,

    /* TCP Flags mask for IPv4 Forwarding packet to qualify */
     0x0,

    /* TCP Sequence zero enabler for IPv4 Forwarding packet to qualify */
     bcmRxTrapProgDisable,    

    /* Trap Gport which will connect to the Programmable trap */
     0
};

/*
 * Utility structure for programmable traps,
 * containing only relevant data for IPv4 forwarded packets
 */
struct cint_rx_trap_programmable_traps_config_ipv4_fwd_utils {

    /* IP Protocol field enabler for IPv4 Forwarding packet to qualify */
    bcm_rx_trap_prog_enable_t ip_protocol_enable;

    /* IP protocol field for IPv4 Forwarding packet to qualify */
    uint8 ip_protocol;

    /* L4 Ports enabler for IPv4 Forwarding packet to qualify */
    bcm_rx_trap_prog_enable_t l4_ports_enable;

    /* L4 Src Port for IPv4 Forwarding packet to qualify */
    bcm_port_t src_port;

    /* L4 Src Port Mask for IPv4 Forwarding packet to qualify */
    uint16 src_port_mask;

    /* L4 Dest Port for IPv4 Forwarding packet to qualify */
    bcm_port_t dest_port;

    /* L4 Dest Port Mask for IPv4 Forwarding packet to qualify */
    uint16 dest_port_mask;

    /* TCP Flags enabler for IPv4 Forwarding packet to qualify */
    bcm_rx_trap_prog_enable_t tcp_flags_enable;

    /* TCP Flags for IPv4 Forwarding packet to qualify */
    uint16 tcp_flags;

    /* TCP Flags mask for IPv4 Forwarding packet to qualify */
    uint16 tcp_flags_mask;

    /* TCP Sequence zero enabler for IPv4 Forwarding packet to qualify */
    bcm_rx_trap_prog_enable_t tcp_seq_is_zero_enable;

    /* Trap Gport which will connect to the Programmable trap */
    bcm_gport_t trap_gport;
};


cint_rx_trap_programmable_traps_config_ipv4_fwd_utils cint_rx_trap_programmable_traps_config_ipv4 =  {

    /* IP Protocol field enabler for IPv4 Forwarding packet to qualify */
    bcmRxTrapProgEnableMatch,

    /* IP protocol field for IPv4 Forwarding packet to qualify */
     6,

    /* L4 Ports enabler for IPv4 Forwarding packet to qualify */
     bcmRxTrapProgEnableMismatch,

    /* L4 Src Port for IPv4 Forwarding packet to qualify */
     0x200,

    /* L4 Src Port Mask for IPv4 Forwarding packet to qualify */
     0xFFFF,

    /* L4 Dest Port for IPv4 Forwarding packet to qualify */
     0x100,

    /* L4 Dest Port Mask for IPv4 Forwarding packet to qualify */
     0xFFFF,

    /* TCP Flags enabler for IPv4 Forwarding packet to qualify */
     bcmRxTrapProgDisable,

    /* TCP Flags for IPv4 Forwarding packet to qualify */
     0x32,

    /* TCP Flags mask for IPv4 Forwarding packet to qualify */
     0x1FF,

    /* TCP Sequence zero enabler for IPv4 Forwarding packet to qualify */
     bcmRxTrapProgEnableMatch,

    /* Trap Gport which will connect to the Programmable trap */
     0
};

/**
* \brief
*  Create an ingress UserDefined trap, and set a programmable trap associated with it.
*  This function will only configure filtering for ETH Forwarding packets.
* \par DIRECT INPUT:
*   \param [in] unit                - unit Id
*   \param [in] flags               - Flags - With_ID
*   \param [in] fwd_strength        - Trap Strength
*   \param [in] prog_config_eth_fwd - Eth forwarding structure(If NULL, takes pre-configured)
*   \param [in] trap_id             - The UserDefined trap ID
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   int - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
int
run_rx_programmable_trap_traffic_config_eth_fwd(
    int unit,
    int flags,
    int fwd_strength,
    bcm_rx_trap_prog_config_t *prog_config_eth_fwd,
    int *trap_id)
{

    int rv = 0;
    int snp_strength = 0;

    /* Fwd and Snp Strengths need to be 0 for creation of UserDefined trap */
    rv = cint_utils_rx_trap_ingress_create_and_set_to_drop(unit, BCM_RX_TRAP_WITH_ID, bcmRxTrapUserDefine, 0, trap_id);
    printf("Trap ID: %d \n", *trap_id);
    if (rv != BCM_E_NONE)
    {
      printf("Error in cint_utils_rx_trap_ingress_create_and_set_to_drop\n");
      return rv;
    }

    if(prog_config_eth_fwd != NULL)
    {
      sal_memcpy(&cint_rx_trap_programmable_traps_config_eth, prog_config_eth_fwd, sizeof(cint_rx_trap_programmable_traps_config_eth));
    }


    BCM_GPORT_TRAP_SET(cint_rx_trap_programmable_traps_config_eth.trap_gport, *trap_id, fwd_strength, snp_strength);

    rv = bcm_rx_trap_prog_set(unit, 0, cint_rx_trap_programmable_traps_index_eth, cint_rx_trap_programmable_traps_config_eth);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_rx_trap_prog_set\n");
        return rv;
    }

    return rv;
}

/**
* \brief
*  Configuration of programmable trap for ETH forwarding header fields.
* \par DIRECT INPUT:
*   \param [in] unit                - unit Id
*   \param [in] prog_index          - Index of the programmable trap
*   \param [in] prog_config_eth_fwd - Eth parameters only for prog. trap
*   \param [in] prog_config         - BCM struct originally containing GPORT
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   int - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
int
programmable_trap_config_fill_and_set_ipv4(
    int unit,
    uint8 prog_index,
    cint_rx_trap_programmable_traps_config_ipv4_fwd_utils * prog_config_eth_fwd,
    bcm_rx_trap_prog_config_t * prog_config)
{

    int rv = 0;

    /* Fill the prog_config sturct with IPv4/TCP filtering info only */
    prog_config->ip_protocol = prog_config_eth_fwd->ip_protocol;
    prog_config->ip_protocol_enable = prog_config_eth_fwd->ip_protocol_enable;

    prog_config->src_port = prog_config_eth_fwd->src_port;
    prog_config->src_port_mask = prog_config_eth_fwd->src_port_mask;
    prog_config->dest_port = prog_config_eth_fwd->dest_port;
    prog_config->dest_port_mask = prog_config_eth_fwd->dest_port_mask;
    prog_config->l4_ports_enable = prog_config_eth_fwd->l4_ports_enable;


    prog_config->tcp_flags = prog_config_eth_fwd->tcp_flags;
    prog_config->tcp_flags_mask = prog_config_eth_fwd->tcp_flags_mask;
    prog_config->tcp_flags_enable = prog_config_eth_fwd->tcp_flags_enable;

    prog_config->tcp_seq_is_zero_enable = prog_config_eth_fwd->tcp_seq_is_zero_enable;

    rv = bcm_rx_trap_prog_set(unit, 0, prog_index, prog_config);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_rx_trap_prog_set\n");
        return rv;
    }

    return rv;
}

/**
* \brief
*  Create an ingress UserDefined trap, and set a programmable trap associated with it.
 * This function will only configure filtering for IPv4 Forwarding packets.
* \par DIRECT INPUT:
*   \param [in] unit                 - unit Id
*   \param [in] flags                - Flags - With_ID
*   \param [in] fwd_strength         - Trap Strength
*   \param [in] prog_config_ipv4_fwd - IPv4 forwarding structure(If NULL, takes pre-configured)
*   \param [in] trap_id              - The UserDefined trap ID
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   int - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
int
run_rx_programmable_trap_traffic_config_ipv4_fwd(
    int unit,
    int flags,
    int fwd_strength,
    cint_rx_trap_programmable_traps_config_ipv4_fwd_utils *prog_config_ipv4_fwd,
    int *trap_id)
{

    int rv = 0;
    int snp_strength = 0;
    bcm_rx_trap_prog_config_t prog_config;

    bcm_rx_trap_prog_config_t_init(&prog_config);

    /* Fwd and Snp Strengths need to be 0 for creation of UserDefined trap */
    rv = cint_utils_rx_trap_ingress_create_and_set_to_drop(unit, BCM_RX_TRAP_WITH_ID, bcmRxTrapUserDefine, 0, trap_id);
    if (rv != BCM_E_NONE)
    {
      printf("Error in cint_utils_rx_trap_ingress_create_and_set_to_drop\n");
      return rv;
    }

    if(prog_config_ipv4_fwd != NULL)
    {
      sal_memcpy(&cint_rx_trap_programmable_traps_config_ipv4, prog_config_ipv4_fwd, sizeof(cint_rx_trap_programmable_traps_config_ipv4));
    }

    BCM_GPORT_TRAP_SET(prog_config.trap_gport, *trap_id, fwd_strength, snp_strength);

    rv = programmable_trap_config_fill_and_set_ipv4(unit, cint_rx_trap_programmable_traps_index_ipv4, &cint_rx_trap_programmable_traps_config_ipv4, &prog_config);
    if (rv != BCM_E_NONE)
    {
      printf("Error in programmable_trap_config_fill_and_set_ipv4\n");
      return rv;
    }

    return rv;

}



/**
* \brief
* This function will only configure filtering for IPv4 Forwarding packets.
* It will set the action of the trap to be mapped to the default DROP trap, created on init.
* \par DIRECT INPUT:
*   \param [in] unit                 - unit Id
*   \param [in] fwd_strength         - Trap Strength
*   \param [in] prog_config_ipv4_fwd - IPv4 forwarding structure(If NULL, takes pre-configured)
*   \param [in] trap_type            - BCM Trap type
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   int - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
int
run_rx_programmable_trap_traffic_config_ipv4_default_traps(
    int unit,
    int fwd_strength,
    cint_rx_trap_programmable_traps_config_ipv4_fwd_utils *prog_config_ipv4_fwd,
    bcm_rx_trap_t trap_type)
{

    int rv = 0;
    int snp_strength = 0;
    bcm_rx_trap_prog_config_t prog_config;

    bcm_rx_trap_prog_config_t_init(&prog_config);

    if(prog_config_ipv4_fwd != NULL)
    {
      sal_memcpy(&cint_rx_trap_programmable_traps_config_ipv4, prog_config_ipv4_fwd, sizeof(cint_rx_trap_programmable_traps_config_ipv4));
    }

    cint_rx_trap_programmable_traps_get_trap_id = 0;
    bcm_rx_trap_type_get(unit, 0, trap_type, &cint_rx_trap_programmable_traps_get_trap_id);

    BCM_GPORT_TRAP_SET(prog_config.trap_gport, cint_rx_trap_programmable_traps_get_trap_id, fwd_strength, snp_strength);

    rv = programmable_trap_config_fill_and_set_ipv4(unit, cint_rx_trap_programmable_traps_index_ipv4, &cint_rx_trap_programmable_traps_config_ipv4, &prog_config);
    if (rv != BCM_E_NONE)
    {
      printf("Error in programmable_trap_config_fill_and_set_ipv4\n");
      return rv;
    }

    return rv;

}

/**
* \brief
* This function will only configure filtering for IPv4 Forwarding packets.
* It will set the action of the trap to be mapped to the default DROP trap, created on init.
* \par DIRECT INPUT:
*   \param [in] unit                 - unit Id
*   \param [in] fwd_strength         - Trap Strength
*   \param [in] prog_config_ipv4_fwd - IPv4 forwarding structure(If NULL, takes pre-configured)
*   \param [in] prog_config_eth_fwd  - ETH forwarding structure(If NULL, takes pre-configured)
*   \param [in] trap_type            - BCM Trap type(NULL for Ingress UD traps)
*   \param [in] trap_id              - Trap ID(NULL for Ingress Default traps)
*   \param [in] is_ipv4_fwd          - Boolean flag for Ipv4 forwarding packet/trap
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   int - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
int cint_rx_trap_programmable_traps_main(
    int unit,
    int fwd_strength,
    cint_rx_trap_programmable_traps_config_ipv4_fwd_utils *prog_config_ipv4_fwd,
    bcm_rx_trap_prog_config_t *prog_config_eth_fwd,
    bcm_rx_trap_t *trap_type,
    int *trap_id,
    int is_ipv4_fwd)
{
    int rv = BCM_E_NONE;

    if(trap_type != NULL)
    {
        rv = run_rx_programmable_trap_traffic_config_ipv4_default_traps(unit,
                                                                    fwd_strength,
                                                                    prog_config_ipv4_fwd,
                                                                    *trap_type);
        if (rv != BCM_E_NONE)
        {
          printf("Error in run_rx_programmable_trap_traffic_config_ipv4_default_traps\n");
          return rv;
        }
    }
    else if(trap_id != NULL)
    {
        if(is_ipv4_fwd == TRUE)
        {
            rv = run_rx_programmable_trap_traffic_config_ipv4_fwd(unit,
                                                                 0,
                                                                 fwd_strength,
                                                                 prog_config_ipv4_fwd,
                                                                 trap_id);
            if (rv != BCM_E_NONE)
            {
                printf("Error in run_rx_programmable_trap_traffic_config_ipv4_fwd\n");
                return rv;
            }
        }
        else
        {
            rv = run_rx_programmable_trap_traffic_config_eth_fwd(unit,
                                                                 0,
                                                                 fwd_strength,
                                                                 prog_config_eth_fwd,
                                                                 trap_id);
            if (rv != BCM_E_NONE)
            {
                printf("Error in run_rx_programmable_trap_traffic_config_eth_fwd\n");
                return rv;
            }
        }
    }

    return rv;
}

/**
* \brief
* Destroys the trap associated with the supplied trap_id and
* clears the programmable traps configs set in this cint.
* \par DIRECT INPUT:
*   \param [in] unit     - unit Id
*   \param [in] trap_id  - Trap ID
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   int - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
int cint_rx_trap_programmable_traps_destroy(
    int unit, 
    int trap_id)
{
    int rv;
    bcm_rx_trap_prog_config_t prog_config;

    bcm_rx_trap_prog_config_t_init(&prog_config);

    BCM_GPORT_TRAP_SET(prog_config.trap_gport, trap_id, 0, 0);

    rv = bcm_rx_trap_prog_set(unit, 0, cint_rx_trap_programmable_traps_index_eth, prog_config);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_rx_trap_prog_set ETH Index\n");
        return rv;
    }

    rv = bcm_rx_trap_prog_set(unit, 0, cint_rx_trap_programmable_traps_index_ipv4, prog_config);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_rx_trap_prog_set IPv4 Index\n");
        return rv;
    }

    rv = bcm_rx_trap_type_destroy(unit, trap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_rx_trap_type_destroy \n");
        return rv;
    }

    return rv;
}

