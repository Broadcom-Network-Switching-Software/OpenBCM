/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:       subport.c
 * Purpose:    SUBPORT common APIs
 */

#include <soc/drv.h>
#include <bcm/subport.h>

/*
 * Function:
 *     bcm_subport_group_config_t_init
 * Description:
 *     Initialize an SUBPORT group config structure
 * Parameters:
 *     config - pointer to SUBPORT group config structure
 * Return: none
 */
void
bcm_subport_group_config_t_init(bcm_subport_group_config_t *config)
{
    sal_memset(config, 0, sizeof(bcm_subport_group_config_t));
}

/*
 * Function:
 *     bcm_subport_config_t_init
 * Description:
 *     Initialize an SUBPORT config structure
 * Parameters:
 *     config - pointer to SUBPORT config structure
 * Return: none
 */
void
bcm_subport_config_t_init(bcm_subport_config_t *config)
{
    sal_memset(config, 0, sizeof(bcm_subport_config_t));
}

/**
 * Function:
 *      bcm_subport_group_linkphy_config_t_init()
 * Purpose:
 *      Initialize port linkPHY config control struct
 *      
 * Parameters:
 *      unit - Unit
 *      linkphy_config - port linkPHY configuration
 */
void bcm_subport_group_linkphy_config_t_init(
        bcm_subport_group_linkphy_config_t *linkphy_config) 
{
    if (linkphy_config != NULL) {
        sal_memset(linkphy_config, 0,
            sizeof(bcm_subport_group_linkphy_config_t));
    }
    return;
}

/**
 * Function:
 *      bcm_subport_linkphy_rx_error_reg_info_t_init()
 * Purpose:
 *      Initialize linkPHY RX error registration struct
 *      
 * Parameters:
 *      reg_info - Linkphy error reg info
 */
void bcm_subport_linkphy_rx_error_reg_info_t_init(
        bcm_subport_linkphy_rx_error_reg_info_t *reg_info) 
{
    if (reg_info != NULL) {
        sal_memset(reg_info, 0,
            sizeof(bcm_subport_linkphy_rx_error_reg_info_t));
    }
    return;
}

