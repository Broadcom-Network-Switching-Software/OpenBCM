/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/debug.h>

#include <sal/core/boot.h>

#include <bcm/error.h>
#include <bcm/vlan.h>
#include <bcm/stg.h>
#include <bcm/port.h>
#include <bcm/trunk.h>
#include <bcm/debug.h>
#include <bcm/switch.h>
#include <bcm/stat.h>
#include <bcm/tsn.h>

/*
 * Function:
 *      bcm_vlan_ip_t_init
 * Purpose:
 *      Initialize the vlan_ip_t structure
 * Parameters:
 *      vlan_ip - Pointer to structure which should be initialized
 * Returns:
 *      NONE
*/
void 
bcm_vlan_ip_t_init(bcm_vlan_ip_t *vlan_ip)
{
    if (NULL != vlan_ip) {
        sal_memset(vlan_ip, 0, sizeof (bcm_vlan_ip_t));
    }
    return;
}

/*
 * Function:
 *      bcm_vlan_data_t_init
 * Purpose:
 *      Initialize the bcm_vlan_data_t structure
 * Parameters:
 *      vlan_data - Pointer to vlan data info structure
 * Returns:
 *      NONE
 */
void
bcm_vlan_data_t_init(bcm_vlan_data_t *vlan_data)
{
    if (vlan_data != NULL) {
        sal_memset(vlan_data, 0, sizeof (bcm_vlan_data_t));
    }
    return;
}

/*
 * Function:
 *      bcm_vlan_control_vlan_t_init
 * Purpose:
 *      Initialize the bcm_vlan_control_vlan_t structure
 * Parameters:
 *      data - Pointer to vlan control structure to inititalize
 * Returns:
 *      NONE
 */
void
bcm_vlan_control_vlan_t_init(bcm_vlan_control_vlan_t *data)
{
    if (data != NULL) {
        sal_memset(data, 0, sizeof(bcm_vlan_control_vlan_t));
        data->outer_tpid = 0x8100;
    }
    return;
}

/*
 * Function:
 *      bcm_vlan_block_t_init
 * Purpose:
 *      Initialize the bcm_vlan_block_t structure
 * Parameters:
 *      data - Pointer to vlan block structure to inititalize
 * Returns:
 *      NONE
 */
void
bcm_vlan_block_t_init(bcm_vlan_block_t *data)
{
    if (data != NULL) {
        sal_memset(data, 0, sizeof(bcm_vlan_block_t));
    }
    return;
}


/*
 * Function:
 *      bcm_vlan_action_set_t_init
 * Purpose:
 *      Initialize the bcm_vlan_action_set_t structure
 * Parameters:
 *      data - Pointer to vlan action structure to inititalize
 * Returns:
 *      NONE
 */
void
bcm_vlan_action_set_t_init(bcm_vlan_action_set_t *data)
{
    if (data != NULL) {
        sal_memset(data, 0, sizeof(bcm_vlan_action_set_t));
        data->taf_gate_primap = BCM_TSN_PRI_MAP_INVALID;
        data->inner_qos_map_id = -1;
        data->outer_qos_map_id = -1;
    }
    return;
}

/*
* Function:
*      bcm_vlan_translate_action_class_t_init
* Purpose:
*      Initialize the bcm_vlan_translate_action_class_t structure
* Parameters:
*      info - Pointer to the structure which should be initialized
* Returns:
*      NONE
*/
void 
bcm_vlan_translate_action_class_t_init(bcm_vlan_translate_action_class_t *action_class)
{
    if (NULL != action_class) {
        sal_memset(action_class, 0, sizeof (*action_class));
    }
    return;
}

/*
 * Function:
 *      bcm_vlan_flexctr_config_t_init
 * Purpose:
 *      Initialize flex counter configuration structure in VLAN module
 * Parameters:
 *      config - Pointer to flex counter configuration structure to inititalize
 * Returns:
 *      NONE
 */
void
bcm_vlan_flexctr_config_t_init(bcm_vlan_flexctr_config_t *config)
{
    if (config != NULL) {
        sal_memset(config, 0, sizeof(bcm_vlan_flexctr_config_t));
    }
    return;
}

/*
 * Function:
 *      bcm_vlan_port_t_init
 * Purpose:
 *      Initialize the bcm_vlan_port_t structure
 * Parameters:
 *      data - Pointer to vlan port structure to inititalize
 * Returns:
 *      NONE
 */
void
bcm_vlan_port_t_init(bcm_vlan_port_t *data)
{
    if (data != NULL) {
        sal_memset(data, 0, sizeof(bcm_vlan_port_t));
        data->outlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE;
        data->inlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE;
    }
    return;
}

/*
* Function:
*      bcm_vlan_port_translation_t_init
* Purpose:
*      Initialize the bcm_vlan_port_translation_t structure
* Parameters:
*      info - Pointer to the structure which should be initialized
* Returns:
*      NONE
*/
void 
bcm_vlan_port_translation_t_init(bcm_vlan_port_translation_t *vlan_port_translation)
{
    if (NULL != vlan_port_translation) {
        sal_memset(vlan_port_translation, 0, sizeof (*vlan_port_translation));
    }
    return;
}

/*
 * Function:
 *      bcm_vlan_gport_info_t_init
 * Purpose:
 *      Initialize the bcm_vlan_gport_info_t structure
 * Parameters:
 *      data - Pointer to vlan gport info structure to inititalize
 * Returns:
 *      NONE
 */
void
bcm_vlan_gport_info_t_init(bcm_vlan_gport_info_t *data)
{
    if (data != NULL) {
        sal_memset(data, 0, sizeof(bcm_vlan_gport_info_t));
    }
    return;
}

#if defined(INCLUDE_L3)
/*
 * Function:
 *      bcm_vlan_vpn_config_t_init
 * Purpose:
 *      Initialize VLAN vpn config struct
 * Parameters:
 *      info - Pointer to the struct to be inited
 * Returns:
 *      NONE
 */
void bcm_vlan_vpn_config_t_init(bcm_vlan_vpn_config_t *info)
{
    if (info != NULL) {
        sal_memset(info, 0, sizeof(*info));
    }

    return;
}
#endif

/*
 * Function:
 *      bcm_vlan_match_info_t_init
 * Purpose:
 *      Initialize the vlan match info structure
 * Parameters:
 *      info - Pointer to the struct to be inited
 * Returns:
 *      NONE
 */
void bcm_vlan_match_info_t_init(bcm_vlan_match_info_t *vlan_match_info)
{
    if (vlan_match_info != NULL) {
        sal_memset(vlan_match_info, 0, sizeof(*vlan_match_info));
    }

    return;
}
/*
 * Function:
 *      bcm_vlan_translate_egress_key_config_t_init
 * Purpose:
 *      Initialize the key_config structure
 * Parameters:
 *      info - Pointer to the struct to be inited
 * Returns:
 *      NONE
 */
void bcm_vlan_translate_egress_key_config_t_init(
    bcm_vlan_translate_egress_key_config_t *key_config)
{
    if (key_config != NULL) {
        sal_memset(key_config, 0, sizeof(*key_config));
    }

    return;
}
/*
 * Function:
 *      bcm_vlan_translate_egress_action_set_t_init
 * Purpose:
 *      Initialize the egress_action structure
 * Parameters:
 *      info - Pointer to the struct to be inited
 * Returns:
 *      NONE
 */
void bcm_vlan_translate_egress_action_set_t_init(
    bcm_vlan_translate_egress_action_set_t *action)
{
    if (action != NULL) {
        sal_memset(action, 0, sizeof(*action));
    }

    return;
}
