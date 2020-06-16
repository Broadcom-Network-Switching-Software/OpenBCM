/* $Id: 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 *  
 * File: cint_rx_trap_ingress_traps.c
 * Purpose: Simple example or creating and setting an Ingress Trap.
 *
 * cint ../../../../src/examples/dnx/traps/cint_rx_trap_ingress_traps.c
 *
 * Main Function:
 *      cint_rx_trap_ingress_traps_main(unit,trap_create_flags,trap_type,ingress_config_p);
 * Destroy Function:
 *      cint_rx_trap_ingress_traps_destroy(unit);
 *
 *
 * Example Config:
 * cint ../../../../src/examples/dnx/traps/cint_utils_rx.c
 * cint ../../../../src/examples/dnx/traps/cint_rx_trap_ingress_traps.c
 * cint
 * cint_rx_trap_ingress_traps_main(0,0,bcmRxTrapTerminatedSaEqualsDa,NULL);
 */

struct cint_rx_trap_ingress_traps_config_utils_s{
    /* BCM_RX_TRAP_XXX FLAGS. 
        * Actions configured by flag only (no field):
        * BCM_RX_TRAP_TRAP - stamp FHEI on packet.
        * BCM_RX_TRAP_BYPASS_FILTERS - disable link layer filters in Egress.
        * BCM_RX_TRAP_LEARN_DISABLE - disable learning.
        * BCM_RX_TRAP_UPDATE_ADD_VLAN - add VLAN to the final destination value.
        */
    uint32 flags;

    /* BCM_RX_TRAP_FLAGS2_XXX FLAGS.
        * Actions configured by flag only (no field):
        * BCM_RX_TRAP_FLAGS2_SNOOP_CODE_CLEAR - Clear the snoop code of the packet(set to 0).
        * BCM_RX_TRAP_FLAGS2_MIRROR_CODE_CLEAR - Clear the mirror code of the packet(set to 0).
        * BCM_RX_TRAP_FLAGS2_STAT_SAMPLING_CODE_CLEAR - Clear the stat sampling code of the packet(set to 0).
        */
    uint32 flags2;

    /* Destination port. 
        * Flag to be set for field use:
        * BCM_RX_TRAP_UPDATE_DEST 
        */
    bcm_gport_t dest_port;

    /* Destination group.
        * Flag to be set for field use:
        * BCM_RX_TRAP_DEST_MULTICAST 
        */
    bcm_multicast_t dest_group; 

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

    /* snoop command.
        * Flag to be set for field use:
        * None, default value is zero.
        */       
    int snoop_cmnd;        

    /* Forwarding header position overridden value.
        * Flag to be set for field use:
        * BCM_RX_TRAP_UPDATE_FORWARDING_HEADER
        */      
    bcm_rx_trap_forwarding_header_t forwarding_header; 

    /* Encap-ID.
        * Flag to be set for field use:
        * BCM_RX_TRAP_UPDATE_ENCAP_ID
        */     
    bcm_if_t encap_id;

    /* Encap-ID2.
        * Flag to be set for field use:
        * BCM_RX_TRAP_UPDATE_ENCAP_ID
        */    
    bcm_if_t encap_id2;    

    /* Destinations information per core.
        * Flag to be set for field use:
        * None, default value is NULL.
        */    
    bcm_rx_trap_core_config_t *core_config_arr; 

    /* core_config_arr length 
        * Flag to be set for field use:
        * None, default value is zero.
        */    
    int core_config_arr_len;            

    /* meter command. 
        * Flag to be set for field use:
        * BCM_RX_TRAP_UPDATE_METER_CMD
        */     
    int meter_cmd;     

    /* mapped trap strength, overwrite trap strength on egress.
        * Flag to be set for field use:
        * BCM_RX_TRAP_UPDATE_MAPPED_STRENGTH
        */    
    uint8 mapped_trap_strength;

    /** ECN value.
        * Flag to be set for field use:
        * BCM_RX_TRAP_UPDATE_ECN_VALUE
        */    
    uint8 ecn_value;

    /** VSQ pointer.
        * Flag to be set for field use:
        * BCM_RX_TRAP_UPDATE_VSQ
        */    
    uint16 vsq;

    /** Latency flow id clear. TRUE/FALSE value.
        * Flag to be set for field use:
        * None, the default value is FALSE(0).
        */    
    uint8 latency_flow_id_clear;

    /** Visibility value.
        * Flag to be set for field use:
        * BCM_RX_TRAP_UPDATE_VISIBILITY
        */     
    uint8 visibility_value;

    /** Bitmap indicating which statistical objects needs to be cleared.
        * Flag to be set for field use:
        * None, default value is zero.
        */    
    uint32 stat_clear_bitmap;

    /** Statistical objects configuration array.
        * Flag to be set for field use:
        * None, default value is empty array (all zeros content).
        */      
    bcm_rx_trap_stat_obj_config_t stat_obj_config_arr[BCM_RX_TRAP_MAX_STAT_OBJ_ARR_LEN];

    /** Statistical objects configuration array length.
        * Flag to be set for field use:
        * None, default value is zero.
        */     
    uint32 stat_obj_config_len;

    /* Statistical metadata mask.
        * Flag to be set for field use:
        * None, default value is zero.
        */     
    uint32 stat_metadata_mask;

    /* Egress parsing index.
        * Flag to be set for field use:
        * BCM_RX_TRAP_UPDATE_EGRESS_FWD_INDEX
        */           
    uint32 egress_forwarding_index;    
    
    /* forward trap strength */
    int trap_strength;                
    
    /* snoop strength */
    int snoop_strength;                 
};

/* default configuration is drop */
cint_rx_trap_ingress_traps_config_utils_s cint_rx_trap_ingress_traps_config ={
    /* BCM_RX_TRAP_XXX FLAGS. */
    BCM_RX_TRAP_UPDATE_DEST,

    /* Flags 2 */
    0,

    /* Destination port. */
    BCM_GPORT_BLACK_HOLE,

    /* Destination group. */
    0, 

    /* Internal priority of the packet. */
    0,      

    /* Color of the packet. */
    0,   

    /* snoop command. */
    0,                 

    /* Forwarding header position overridden value */
    0, 

    /* Encap-ID */
    0,

    /* Encap-ID2 */
    0,

    /* Destinations information per core */
    NULL, 

    /* core_config_arr length */
    0,            

    /* meter command. */
    0,     

    /* mapped trap strength */
    0,

    /** ECN value */
    0,

    /** VSQ pointer */
    0,

    /** Latency flow id clear */
    0,

    /** Visibility value */
    0,

    /** Statistical objects are to clear */
    0,

    /** Statistical objects configuration array */
    {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},

    /** Statistical objects configuration array length */
    0,

    /* Statistical metadata mask */
    0,       

    /* Egress parsing index */
    0,
    
    /* forward trap strength */
    15,                
    
    /* snoop strength */
    0       
};

int cint_rx_trap_ingress_traps_trap_id = 0;
bcm_rx_trap_core_config_t cint_rx_trap_ingress_traps_core_config[2];

 /**
 * \brief  
 *  Fill trap_config struct with ingress trap configuration
 * \par DIRECT INPUT: 
 *   \param [in] ingress_config   -  ingress trap configuration
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
int trap_config_from_ingress_config_fill(
cint_rx_trap_ingress_traps_config_utils_s* ingress_config_p,
bcm_rx_trap_config_t* trap_config_p)
{
    int rv = BCM_E_NONE;

    /* Check input */
    if(ingress_config_p == NULL || trap_config_p == NULL)
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

    trap_config_p->flags = ingress_config_p->flags;
    trap_config_p->flags2 = ingress_config_p->flags2;
    trap_config_p->dest_port = ingress_config_p->dest_port;
    trap_config_p->dest_group = ingress_config_p->dest_group;
    trap_config_p->prio = ingress_config_p->prio;
    trap_config_p->color = ingress_config_p->color;
    trap_config_p->snoop_cmnd = ingress_config_p->snoop_cmnd;
    trap_config_p->forwarding_header = ingress_config_p->forwarding_header;
    trap_config_p->encap_id = ingress_config_p->encap_id;
    trap_config_p->encap_id2 = ingress_config_p->encap_id2;
    trap_config_p->meter_cmd = ingress_config_p->meter_cmd;
    trap_config_p->mapped_trap_strength = ingress_config_p->mapped_trap_strength;
    trap_config_p->ecn_value = ingress_config_p->ecn_value;
    trap_config_p->vsq = ingress_config_p->vsq;
    trap_config_p->latency_flow_id_clear = ingress_config_p->latency_flow_id_clear;
    trap_config_p->visibility_value = ingress_config_p->visibility_value;
    trap_config_p->stat_clear_bitmap = ingress_config_p->stat_clear_bitmap;
    trap_config_p->stat_obj_config_arr = ingress_config_p->stat_obj_config_arr;
    trap_config_p->stat_obj_config_len = ingress_config_p->stat_obj_config_len;
    trap_config_p->stat_metadata_mask = ingress_config_p->stat_metadata_mask;
    trap_config_p->egress_forwarding_index = ingress_config_p->egress_forwarding_index;
    trap_config_p->trap_strength = ingress_config_p->trap_strength;
    trap_config_p->snoop_strength = ingress_config_p->snoop_strength;
    trap_config_p->core_config_arr_len = ingress_config_p->core_config_arr_len; 
    if ((ingress_config_p->core_config_arr_len > 0) && (ingress_config_p->core_config_arr == NULL))
    {
        trap_config_p->core_config_arr = cint_rx_trap_ingress_traps_core_config;
    }
    else
    {
        trap_config_p->core_config_arr = ingress_config_p->core_config_arr;
    }

    return rv;
}
 
 /**
 * \brief  
 *  Main function for generic ingress trap configuration
 * \par DIRECT INPUT: 
 *   \param [in] unit      -  Unit Id
 *   \param [in] trap_create_flags   -  flags for trap creation (BCM_RX_TRAP_WITH_ID)
 *   \param [in] trap_type   -  trap type to configure
 *   \param [in] trap_config - ingress trap configuration
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
int cint_rx_trap_ingress_traps_main(
int unit,
int trap_create_flags,
bcm_rx_trap_t trap_type,
cint_rx_trap_ingress_traps_config_utils_s* ingress_config_p)
{
    int rv = BCM_E_NONE;
    bcm_rx_trap_config_t trap_config;

    if (ingress_config_p != NULL)
    {
        sal_memcpy(&cint_rx_trap_ingress_traps_config, ingress_config_p, sizeof(cint_rx_trap_ingress_traps_config));
    }
    
    rv = trap_config_from_ingress_config_fill(&cint_rx_trap_ingress_traps_config, &trap_config);
    if(rv != BCM_E_NONE)
    {
        printf("Error, with config struct \n");
        return rv;
    }

    /* Set the trap */
    rv = cint_utils_rx_trap_create_and_set(unit, trap_create_flags, trap_type, trap_config, &cint_rx_trap_ingress_traps_trap_id);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, in cint_utils_rx_trap_create_and_set \n");
        return rv;
    } 

    return rv;    
}

/**
* \brief  
*  Destroy ingress trap
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
int cint_rx_trap_ingress_traps_destroy(
int unit)
{
    int rv = BCM_E_NONE;

    rv = bcm_rx_trap_type_destroy(unit, cint_rx_trap_ingress_traps_trap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error destroying ingress trap, in bcm_rx_trap_type_destroy\n");
        return rv;
    }

    return rv;
}
