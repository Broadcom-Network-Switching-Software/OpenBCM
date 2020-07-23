/* $Id: 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 *  
 * File: cint_rx_trap_etpp_visibility.c
 * Purpose: Example for creating and setting ETPP visibility Trap.
 *
 * cint ../../../../src/examples/dnx/traps/cint_rx_trap_etpp_visibility.c
 *
 * Main Function:
 *      cint_rx_trap_etpp_visibility_main(unit, is_pkt_drop, etpp_config_p);
 * Destroy Function:
 *      cint_rx_trap_etpp_visibility_destroy(unit);
 *
 * Example Config:
 * cint ../../../../src/examples/dnx/traps/cint_utils_rx.c
 * cint ../../../../src/examples/dnx/traps/cint_rx_trap_etpp_visibility.c
 * cint
 * cint_rx_trap_etpp_visibility_main(0,TRUE,NULL);
 */

struct cint_rx_trap_etpp_visibility_config_utils_s{
    /* cpu trap gport - configure recycle cmd */
    bcm_gport_t cpu_trap_gport;

    /* recycle high priority indication */
    uint8 is_recycle_high_priority;
    
    /* crop recycle packet indication */
    uint8 is_recycle_crop_pkt;

    /* indication regarding appending the original FTMH to the recycle packet */
    uint8 is_recycle_append_ftmh;

    /** packet processing drop reason */
    int pp_drop_reason;
};

/* default configuration is drop */
cint_rx_trap_etpp_visibility_config_utils_s cint_rx_trap_etpp_visibility_config ={
    /* cpu trap gport */
    0,

    /* recycle high priority indication */
    FALSE,
    
    /* crop recycle packet indication */
    TRUE,

    /* indication regarding appending the original FTMH to the recycle packet */
    TRUE,

    /** packet processing drop reason */
    0
};

int cint_rx_trap_etpp_visibility_trap_id = 0;

/* Configure recycle command trap to redirect packet to CPU */
BCM_GPORT_TRAP_SET(cint_rx_trap_etpp_visibility_config.cpu_trap_gport,0xb3,15,0);

 /**
 * \brief  
 *  Fill trap_config struct with etpp trap configuration
 * \par DIRECT INPUT: 
 *   \param [in] etpp_config   - etpp trap configuration
 *   \param [in] is_pkt_drop   - configure action to drop packet
 *   \param [out] trap_config  - generic trap configuration
 * \par INDIRECT INPUT: 
 *   * None
 * \par DIRECT OUTPUT: 
 *   int -  Error Type
 * \par INDIRECT OUTPUT: 
 *   * None
 * \remark 
 *   * None
 * \see
 *   * None
 */
int trap_config_from_etpp_config_fill(
cint_rx_trap_etpp_visibility_config_utils_s* etpp_config_p,
int is_pkt_drop,
bcm_rx_trap_config_t* trap_config_p)
{
    int rv = BCM_E_NONE;

    /* Check input */
    if(etpp_config_p == NULL || trap_config_p == NULL)
    {
        printf("Error,  NULL param \n");
        return BCM_E_PARAM;
    }

    bcm_rx_trap_config_t_init(trap_config_p);
    if(rv != BCM_E_NONE)
    {
        printf("Error initializing trap_config \n");
        return rv;
    }

    if (is_pkt_drop)
    {
    	trap_config_p->flags = BCM_RX_TRAP_COPY_DROP;
    }
    else
    {
		trap_config_p->cpu_trap_gport = etpp_config_p->cpu_trap_gport;
		trap_config_p->is_recycle_high_priority = etpp_config_p->is_recycle_high_priority;
		trap_config_p->is_recycle_crop_pkt = etpp_config_p->is_recycle_crop_pkt;
		trap_config_p->is_recycle_append_ftmh = etpp_config_p->is_recycle_append_ftmh;
		trap_config_p->pp_drop_reason = etpp_config_p->pp_drop_reason;
    }

    return rv;
}

 /**
 * \brief  
 *  Main function for ETPP visibility trap configuration
 * \par DIRECT INPUT: 
 *   \param [in] unit      -  Unit Id
 *   \param [in] is_pkt_drop -  Configure action to drop packet
 *   \param [in] etpp_trap_config - etpp trap configuration
 * \par INDIRECT INPUT: 
 *   * None
 * \par DIRECT OUTPUT: 
 *   int -  Error Type
 * \par INDIRECT OUTPUT: 
 *   * None
 * \remark 
 *   * None
 * \see
 *   * None
 */
int cint_rx_trap_etpp_visibility_main(
int unit,
int is_pkt_drop,
cint_rx_trap_etpp_visibility_config_utils_s* etpp_config_p)
{
    int rv = BCM_E_NONE;
    bcm_rx_trap_config_t trap_config;

    if (etpp_config_p != NULL)
    {
        sal_memcpy(&cint_rx_trap_etpp_visibility_config, etpp_config_p, sizeof(cint_rx_trap_etpp_visibility_config));
    }

	rv = trap_config_from_etpp_config_fill(&cint_rx_trap_etpp_visibility_config, is_pkt_drop, &trap_config);
	if(rv != BCM_E_NONE)
	{
		printf("Error, with config struct \n");
		return rv;
	}

	/* Set the trap */
	rv = cint_utils_rx_trap_create_and_set(unit, 0, bcmRxTrapEgTxVisibility, &trap_config, &cint_rx_trap_etpp_visibility_trap_id);
	if (rv != BCM_E_NONE)
	{
		printf("Error, in cint_utils_rx_trap_create_and_set \n");
		return rv;
	}

    return rv;    
}

/**
* \brief  
*  Destroy etpp trap
* \par DIRECT INPUT: 
*   \param [in] unit      -  Unit Id
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   int -  Error Type
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
int cint_rx_trap_etpp_visibility_destroy(
int unit)
{
    int rv = BCM_E_NONE;

    rv = bcm_rx_trap_type_destroy(unit, cint_rx_trap_etpp_visibility_trap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error destroying etpp trap, in bcm_rx_trap_type_destroy\n");
        return rv;
    }
    
    return rv;
}
