/* $Id: 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 *  
 * File: cint_rx_trap_erpp_traps.c
 * Purpose: Example for creating and setting an ERPP application Trap.
 *
 * cint ../../../../src/examples/dnx/traps/cint_rx_trap_erpp_traps.c
 * Main Function:
 *      cint_rx_trap_erpp_traps_main(unit, trap_type, erpp_config_p);
 * Destroy Function:
 *      cint_rx_trap_erpp_traps_destroy(unit, trap_type);
 *
 * Additional Notes: 
 * To configure snoop action need to update following fields:
 * cint_rx_trap_etpp_traps_config.snoop_strength - should be filled with value between 1-7
 * cint_rx_trap_etpp_traps_trap_id - should be filled with snoop profile retrieved from bcm_mirror_destination_create 
 * (snoop_profile field is used for overwriting the snoop profile by fwd action)
 *
 * Example Config:
 * cint ../../../../src/examples/dnx/traps/cint_utils_rx.c
 * cint ../../../../src/examples/dnx/traps/cint_rx_trap_erpp_traps.c
 * cint
 * cint_rx_trap_erpp_traps_main(0, bcmRxTrapEgUnknownDa, NULL);
 */

struct cint_rx_trap_erpp_traps_config_utils_s {
    /* BCM_RX_TRAP_XXX FLAGS. */
    uint32 flags;              

    /* Destination port. 
        * Flag to be set for field use:
        * BCM_RX_TRAP_UPDATE_DEST 
        */
    bcm_gport_t dest_port;

    /* Internal priority of the packet. 
        * Flag to be set for field use:
        * BCM_RX_TRAP_UPDATE_PRIO 
        */ 
    int prio;      

    /* Color of the packet. 
        * Flag to be set for field use:
        * BCM_RX_TRAP_UPDATE_COLOR 
        */
    bcm_color_t color;    

    /* quality of service id.
        * Flag to be set for field use:
        * BCM_RX_TRAP_UPDATE_QOS_MAP_ID 
        */    
    int qos_id;

    /* CUD.
        * Flag to be set for field use:
        * BCM_RX_TRAP_UPDATE_ENCAP_ID
        */      
    int cud;

    /* Snoop Profile.
        * Flag to be set for field use:
        * None, default value is zero.
        */        
    int snoop_profile;

    /* Mirror Profile.
        * Flag to be set for field use:
        * None, default value is zero.
        */        
    int mirror_profile;

    /* Counter 0 profile.
        * Flag to be set for field use:
        * BCM_RX_TRAP_UPDATE_COUNTER
        */      
    int counter_0_profile;

    /* Counter 0 Pointer.
        * Flag to be set for field use:
        * BCM_RX_TRAP_UPDATE_COUNTER
        */    
    int counter_0_ptr;

    /* Counter 1 profile.
        * Flag to be set for field use:
        * BCM_RX_TRAP_UPDATE_COUNTER_2
        */    
    int counter_1_profile;

    /* Counter 1 Pointer.
        * Flag to be set for field use:
        * BCM_RX_TRAP_UPDATE_COUNTER_2
        */    
    int counter_1_ptr;
  
    /* PP Drop Reason */
    int pp_drop_reason;
    
    /* forward trap strength */
    int trap_strength;                
    
    /* snoop strength */
    int snoop_strength;
};

/* default configuration is drop */
cint_rx_trap_erpp_traps_config_utils_s cint_rx_trap_erpp_traps_config = {
    /* BCM_RX_TRAP_XXX FLAGS. */
    BCM_RX_TRAP_UPDATE_DEST,              

    /* Destination port. */
    BCM_GPORT_BLACK_HOLE,

    /* Internal priority of the packet. */
    0,      

    /* Color of the packet. */
    0,   

    /* quality of service id */
    0,

    /* CUD */
    0,

    /* Snoop Profile */
    0,

    /* Mirror Profile */
    0,

    /* Counter 0 profile */
    0,

    /* Counter 0 Pointer */
    0,

    /* Counter 1 profile */
    0,

    /* Counter 1 Pointer */
    0,
  
    /* PP Drop Reason */
    0,
    
    /* forward trap strength */
    15,
    
    /* snoop strength */
    0
};

int cint_rx_trap_erpp_traps_trap_id = BCM_RX_TRAP_EG_TRAP_ID_DEFAULT;

 /**
 * \brief  
 *  Fill trap_config struct with erpp trap configuration
 * \par DIRECT INPUT: 
 *   \param [in] erpp_config   -  erpp trap configuration
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
int trap_config_from_erpp_config_fill(
cint_rx_trap_erpp_traps_config_utils_s* erpp_config_p,
bcm_rx_trap_config_t* trap_config_p)
{
    int rv = BCM_E_NONE;

    /* Check input */
    if(erpp_config_p == NULL || trap_config_p == NULL)
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

    trap_config_p->flags = erpp_config_p->flags;
    trap_config_p->dest_port = erpp_config_p->dest_port;
    trap_config_p->prio = erpp_config_p->prio;
    trap_config_p->color = erpp_config_p->color;
    trap_config_p->qos_map_id = erpp_config_p->qos_id;
    trap_config_p->encap_id = erpp_config_p->cud;
    trap_config_p->snoop_cmnd = erpp_config_p->snoop_profile;
    trap_config_p->mirror_profile = erpp_config_p->mirror_profile;
    trap_config_p->stat_obj_config_arr[0].stat_profile = erpp_config_p->counter_0_profile;
    trap_config_p->stat_obj_config_arr[0].stat_id = erpp_config_p->counter_0_ptr;
    trap_config_p->stat_obj_config_arr[1].stat_profile = erpp_config_p->counter_1_profile;
    trap_config_p->stat_obj_config_arr[1].stat_id = erpp_config_p->counter_1_ptr;
    trap_config_p->trap_strength = erpp_config_p->trap_strength;
    trap_config_p->snoop_strength = erpp_config_p->snoop_strength;
    trap_config_p->pp_drop_reason = erpp_config_p->pp_drop_reason;

    return rv;
}
 
 /**
 * \brief  
 *  Main function for generic erpp trap configuration
 * \par DIRECT INPUT: 
 *   \param [in] unit      -  Unit Id
 *   \param [in] appl_trap_type   -  application trap type to configure
 *   \param [in] trap_config - erpp trap configuration
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
int cint_rx_trap_erpp_traps_main(
int unit,
bcm_rx_trap_t appl_trap_type,
cint_rx_trap_erpp_traps_config_utils_s* erpp_config_p)
{
    int rv = BCM_E_NONE;
    bcm_rx_trap_config_t trap_config;

    if (erpp_config_p != NULL)
    {
        sal_memcpy(&cint_rx_trap_erpp_traps_config, erpp_config_p, sizeof(cint_rx_trap_erpp_traps_config));
    }
    
    rv = trap_config_from_erpp_config_fill(&cint_rx_trap_erpp_traps_config, &trap_config);
    if(rv != BCM_E_NONE)
    {
        printf("Error, with config struct \n");
        return rv;
    }

    /* Set the trap */
    rv = cint_utils_rx_trap_appl_config(unit, 0, TRUE, appl_trap_type, &trap_config, &cint_rx_trap_erpp_traps_trap_id);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, in cint_utils_rx_trap_create_and_set \n");
        return rv;
    } 

    return rv;    
}

/**
* \brief  
*  Destroy erpp trap
* \par DIRECT INPUT: 
*   \param [in] unit      -  Unit Id
*   \param [in] appl_trap_type -  application BCM Trap Type
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
int cint_rx_trap_erpp_traps_destroy(
int unit,
bcm_rx_trap_t appl_trap_type)
{
    int rv = BCM_E_NONE;

    rv = cint_utils_rx_trap_appl_destroy(unit, appl_trap_type);
    if (rv != BCM_E_NONE)
    {
        printf("Error destroying erpp trap, in cint_utils_rx_trap_appl_destroy\n");
        return rv;
    }

    return rv;
}

int cint_rx_trap_erpp_basic_tcam_fg(
int unit,
int inP)
{
    field_util_basic_tcam_fg_t tcam_fg_util;
    int rv = BCM_E_NONE;

    field_util_basic_tcam_fg_t_init(unit, &tcam_fg_util);

    tcam_fg_util.fg_id = 30;
    tcam_fg_util.stage = bcmFieldStageIngressPMF1;
    tcam_fg_util.bcm_qual[0] = bcmFieldQualifyInPort;
    tcam_fg_util.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    tcam_fg_util.qual_info[0].input_arg = 0;
    tcam_fg_util.qual_info[0].offset = 0;
    tcam_fg_util.qual_values[0].value[0] = inP;
    tcam_fg_util.bcm_actions[0] = bcmFieldActionStackingRouteNew;
    tcam_fg_util.action_values[0].value[0] = 0x3;

    rv = field_util_basic_tcam_fg_full(unit, tcam_fg_util);

    return rv;

}
