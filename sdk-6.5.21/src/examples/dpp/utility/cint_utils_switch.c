/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * This file provides switch basic functionality 
 */

/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization            -----------------
 */



/* ************************************************************************************************** */

/*****************************************************************************
* Function:  switch__link_layer_mtu_header_trigger
* Purpose:   Function trigger mtu layer filter check on input header
* Params:
* unit - Device number
* header - header type
* enable - TRUE for enable FALSE for disable

* Return:    (int)
 */
int switch__link_layer_mtu_header_trigger(int unit,int header,int enable)
{
    
    int rv = BCM_E_NONE;
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t value;

    key.type = bcmSwitchLinkLayerMtuFilter;
    key.index = header;
    value.value = enable;
    
    rv = bcm_switch_control_indexed_set(unit,key,value);
    if (rv != BCM_E_NONE) {
        printf("Error, in trap set \n");
        return rv;
    } 
    return rv;
}


/*****************************************************************************
* Function:  switch__link_layer_mtu_value_per_profile_set
* Purpose:   Link layer in ETPP has 3 MTU profiles, this function set given MTU value to given MTU profile
* Params:
* unit - Device number
* mtu_profile -  MTU profile to set the value to (1-3)
* mtu_value -  MTU value set for given profile
* Return:    (int)
 */

int switch__link_layer_mtu_value_per_profile_set(int unit,int mtu_profile,int mtu_value)
{
    int rv = BCM_E_NONE;
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t value;

    key.type = bcmSwitchLinkLayerMtuSize;
    key.index = mtu_profile;
    value.value = mtu_value;

    if(mtu_profile < 1 || mtu_profile > 3 ) 
    {
        printf("MTU profile should be in range 1-3 \n");
        return BCM_E_PARAM;
    }
    
    rv = bcm_switch_control_indexed_set(unit,key,value);
    if (rv != BCM_E_NONE) {
        printf("Error, in trap set \n");
        return rv;
    } 
    return rv;
}

