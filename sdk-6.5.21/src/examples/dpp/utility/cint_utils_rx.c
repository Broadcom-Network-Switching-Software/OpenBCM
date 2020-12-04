/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * This file provides rx basic functionality 
 */

/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization            -----------------
 */


struct rx_etpp_trap_set_utils_s{
    /*
    *  mirror command
    */
    int mirror_cmd;
    /*
    *  mirror strength
    */
    int mirror_strength;
    /*
    *  mirror enable
    */
    int mirror_enable;
    /*
    *  forward strength
    */
    int fwd_strength;
    /*
    *  forward enable
    */
    int fwd_enable;
};

struct rx_lif_mtu_set_utils{
    bcm_port_t gport; /*Gport as Global LIF*/
    uint32 mtu;
};

struct rx_rif_mtu_set_utils{
    bcm_if_t intf;      /* Interface  RIF */
    uint32 mtu;
};




/* ************************************************************************************************** */


/*****************************************************************************
* Function:  rx__etpp_trap__set
* Purpose:   
* Params:
* unit        - 
* trap_type   - 
* etpp_config - 
* trap_id     - 
* Return:    (int)
 */
int rx__etpp_trap__set(int unit, bcm_rx_trap_t trap_type,rx_etpp_trap_set_utils_s *etpp_config, int* trap_id)
{
    int rv = BCM_E_NONE;
    bcm_rx_trap_config_t config ={0};
    bcm_mirror_options_t mirror_cmd = {0};
    int trap_create_flags = 0; /*Trap create flags*/

  
    /*Check input*/
    
    if(etpp_config == NULL || trap_id == NULL)
    {
        printf("Error,  NULL param \n");
        return BCM_E_PARAM;
    }
    
    /*Configure etpp trap*/
    mirror_cmd.recycle_cmd = etpp_config->mirror_cmd;
    mirror_cmd.copy_strength = etpp_config->mirror_strength;
    config.trap_strength = etpp_config->fwd_strength;
    
    if(etpp_config->mirror_enable == TRUE)
    {
        
        mirror_cmd.flags |= BCM_MIRROR_OPTIONS_COPY_DROP;
    }
    if(etpp_config->fwd_enable == FALSE)
    {
       
        config.dest_port = BCM_GPORT_BLACK_HOLE;
        config.flags |= BCM_RX_TRAP_UPDATE_DEST;
    }

    config.mirror_cmd = &mirror_cmd;
    
    /*Create ETTP trap*/
    rv = bcm_rx_trap_type_create(unit,trap_create_flags,trap_type,trap_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error, in trap create, trap id $trap_id \n");
        return rv;
    }

    /*Set the trap */
    rv = bcm_rx_trap_set(unit,(*trap_id),&config);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_rx_trap_set \n");
        return rv;
    } 
    
    return rv;
}


/*****************************************************************************
* Function:  rx__ingress_trap_to_cpu
* Purpose:   Trap packet to CPU
* Params:
* unit      - Device Number
* trap_type - Can be ingress trap predefined of bcmTrapUserDefine
* trap_id   - Will return the trap Id of the created trap
* Return:    (int)
 */
int rx__ingress_trap_to_cpu(int unit, bcm_rx_trap_t trap_type, int trap_strength, int* trap_id)
{
    int rv = BCM_E_NONE;
    bcm_rx_trap_config_t config ={0};

    int trap_create_flags = 0; /*Trap create flags*/

    /*Check input*/
    if(trap_id == NULL)
    {
        printf("Error,  NULL param \n");
        return BCM_E_PARAM;
    }

    config.trap_strength = trap_strength;
    config.flags = (BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP | BCM_RX_TRAP_REPLACE);
    config.dest_port = BCM_GPORT_LOCAL_CPU;

    /*Create Predefind trap*/
    rv = bcm_rx_trap_type_create(unit,trap_create_flags,trap_type,trap_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error, in trap create, trap id $trap_id \n");
        return rv;
    }


    /*Set the trap */
    rv = bcm_rx_trap_set(unit,(*trap_id),&config);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_rx_trap_set \n");
        return rv;
    } 

    return rv;
    
}

/*****************************************************************************
* Function:  rx__ingress_trap_drop
* Purpose:   Configure ingress trap to drop packets
* Params:
* unit      - Device Number
* trap_type - Can be ingress trap predefined of bcmTrapUserDefine
* trap_id   - Will return the trap Id of the created trap
* Return:    (int)
 */
int rx__ingress_trap_drop(int unit, bcm_rx_trap_t trap_type, int trap_strength, int* trap_id)
{
    int rv = BCM_E_NONE;
    bcm_rx_trap_config_t config = {0};

    int trap_create_flags = 0; /*Trap create flags*/

    /*Check input*/
    if(trap_id == NULL)
    {
        printf("Error,  NULL param \n");
        return BCM_E_PARAM;
    }

    config.trap_strength = trap_strength;
    config.flags = BCM_RX_TRAP_UPDATE_DEST;
    config.dest_port = BCM_GPORT_BLACK_HOLE;

    /*Create Predefind trap*/
    rv = bcm_rx_trap_type_create(unit, trap_create_flags, trap_type, trap_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error, in trap create, trap id $trap_id \n");
        return rv;
    }


    /*Set the trap */
    rv = bcm_rx_trap_set(unit, (*trap_id), &config);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_rx_trap_set \n");
        return rv;
    } 

    return rv;    
}

/*****************************************************************************
* Function:  rx__trap_create_and_set
* Purpose:   Create trap and set the params with config
* Params:
* unit              - Device number
* trap_type         - Trap type to create
* trap_create_flags - Flags for api bcm_rx_trap_type_create
* trap_config       - Config of trap
* trap_id    (OUT)       - Trap id that created 
* Return:    (int)
 */
int rx__trap_create_and_set(int unit,bcm_rx_trap_t trap_type,int trap_create_flags,
                                    bcm_rx_trap_config_t *trap_config,int *trap_id)
{
    int rv = BCM_E_NONE;
    
    rv = bcm_rx_trap_type_create(unit, trap_create_flags, trap_type, trap_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error, in trap create, trap id $trap_id \n");
        return rv;
    }


    /*Set the trap */
    rv = bcm_rx_trap_set(unit, (*trap_id), trap_config);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_rx_trap_set \n");
        return rv;
    } 

    return rv;    
}


/*****************************************************************************
* Function:  rx__lif_mtu__set
* Purpose:   Set an MTU to specific LIF
* Params:
* unit             - Device Nuber
* lif_mtu_set_info - Hold params Which MTU to set To which LIF
* Return:    (int)
 */
int rx__lif_mtu__set(int unit,rx_lif_mtu_set_utils *lif_mtu_set_info)
{
    int rv = BCM_E_NONE;
	bcm_rx_mtu_config_t config = {0};
	config.flags = BCM_RX_MTU_LIF;
	config.gport = lif_mtu_set_info->gport;
	config.mtu = lif_mtu_set_info->mtu;
    rv = bcm_rx_mtu_set(unit, &config);
    
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_rx_lif_mtu_set \n");
        return rv;
    }
    return rv;
}

/*****************************************************************************
* Function:  rx__rif_mtu__set
* Purpose:   Set an MTU to specific RIF
* Params:
* unit             - Device Nuber
* lif_mtu_set_info - Hold params Which MTU to set To which RIF
* Return:    (int)
 */
int rx__rif_mtu__set(int unit,rx_rif_mtu_set_utils *rif_mtu_set_info)
{
    int rv = BCM_E_NONE;
	bcm_rx_mtu_config_t config = {0};
	config.flags = BCM_RX_MTU_RIF;
	config.intf = rif_mtu_set_info->intf;
	config.mtu = rif_mtu_set_info->mtu;
    rv = bcm_rx_mtu_set(unit, &config);
    
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_rx_lif_mtu_set \n");
        return rv;
    }
    return rv;
}


/*****************************************************************************
* Function:  rx__trap_destroy
* Purpose:   Destroy the given trap id
* Params:
* unit    - Device Number
* trap_id - Trap id to Destroy
* Return:    (int)
 */
int rx__trap_destroy(int unit,int trap_id)
{
     int rv = BCM_E_NONE;

    /*delete trap*/
    rv = bcm_rx_trap_type_destroy(unit, trap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in : bcm_rx_trap_type_destroy, returned %d\n", rv);
        return rv;
    }

    return rv;
}

/*****************************************************************************
* Function:  rx__trap_get
* Purpose:   Get the given trap id configuration
* Params:
* unit    - Device Number
* trap_id - Trap id to retrieve
* Return:    (int)
 */
int rx__trap_get(int unit, int trap_id, bcm_rx_trap_config_t *config)
{
     int rv = BCM_E_NONE;
     sal_memset(config, 0, sizeof(*config));

    /* get trap configuration */
    rv = bcm_rx_trap_get(unit, trap_id, config);
    if (rv != BCM_E_NONE)
    {
        printf("Error in : bcm_rx_trap_get, returned %d\n", rv);
        return rv;
    }

    return rv;
}

/*****************************************************************************
* Function:  rx__trap_config_compare
* Purpose:   Compare 2 trap configurations
* Params:
* x (IN)   - Trap configuration struct
* y (IN)   - Trap configguration struct
* Return:    (int)
 */
int rx__trap_config_compare(bcm_rx_trap_config_t *x, bcm_rx_trap_config_t *y)
{    
    return ((x->flags != y->flags) ||
            (x->dest_port != y->dest_port) ||
            (x->dest_group != y->dest_group) ||
            (x->prio != y->prio) ||
            (x->color != y->color) ||
            (x->ether_policer_id != y->ether_policer_id) ||
            (x->policer_id != y->policer_id) ||
            (x->counter_num != y->counter_num) ||
            (x->trap_strength != y->trap_strength) ||
            (x->snoop_cmnd != y->snoop_cmnd) ||
            (x->snoop_strength != y->snoop_strength) ||
            (x->forwarding_type != y->forwarding_type) ||
            (x->forwarding_header != y->forwarding_header) ||
            (x->encap_id != y->encap_id) ||
            (rx__trap_mirror_cmd_compare(x->mirror_cmd, y->mirror_cmd)) ||
            (rx__trap_core_config_arr_compare(x->core_config_arr, x->core_config_arr_len,
                                              y->core_config_arr, y->core_config_arr_len)) || 
            (x->qos_map_id != y->qos_map_id) ||
            (x->tunnel_termination_trap_strength != y->tunnel_termination_trap_strength) ||
            (x->tunnel_termination_snoop_strength != y->tunnel_termination_snoop_strength));
}

/*****************************************************************************
* Function:  rx__trap_mirror_cmd_compare
* Purpose:   Compare 2 structures of type bcm_mirror_options_t
* Params:
* x (IN)   - bcm_mirror_options_t struct
* y (IN)   - bcm_mirror_options_t struct
* Return:    (int)
 */
int rx__trap_mirror_cmd_compare(bcm_mirror_options_t *x, bcm_mirror_options_t *y)
{    
    if (x != NULL && y != NULL)
    {
        return ((x->flags != y->flags) ||
                (x->forward_strength != y->forward_strength) ||
                (x->copy_strength != y->copy_strength) ||
                (x->recycle_cmd != y->recycle_cmd));
    }
    else if (x == NULL && y == NULL)
    {
        return 0;
    }

    /* if x is NULL and y not or vise versa, return error (not 0) */
    return 1;
}

/*****************************************************************************
* Function:  rx__trap_core_config_arr_compare
* Purpose:   Compare 2 core configuration arrays
* Params:
* x (IN)   - core configuration array
* x_len (IN) -  x array length
* y (IN)   - core configuration array
* y_len (IN) -  y array length
* Return:    (int)
 */
int rx__trap_core_config_arr_compare(bcm_rx_trap_core_config_t *x, int x_len, bcm_rx_trap_core_config_t *y, int y_len)
{
    int core_num;
    
    if (x_len != y_len)
    {
        return 1;
    }

    for (core_num = 0; core_num < x_len; ++core_num)
    {
        if ((x[core_num]->dest_port != y[core_num]->dest_port) ||
            (x[core_num]->encap_id != y[core_num]->encap_id))
        {
            return 1;
        }
    }

    /* return 0 if all is equal */
    return 0;
}

