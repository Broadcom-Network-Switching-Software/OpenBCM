
/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 *  mask FMAC/FSRD interrupts examples
 */

int mask_fmac_interrupts(int unit, int index)
{
    int rv, i;
    bcm_switch_event_t switch_event;
    bcm_switch_event_control_t type;
    uint32 value = 0;
    switch_event = BCM_SWITCH_EVENT_DEVICE_INTERRUPT;
    type.index = index;
    type.action = bcmSwitchEventMask;

    /*
     * JER_INT_FMAC_ERROR_ECC = 501
     * JER_INT_FMAC_ERROR_ECC = 501,
     * JER_INT_FMAC_INT_REG_1 = 502,
     * JER_INT_FMAC_INT_REG_2 = 503,
     * JER_INT_FMAC_INT_REG_3 = 504,
     * JER_INT_FMAC_INT_REG_4 = 505,
     * JER_INT_FMAC_INT_REG_5 = 506,
     * JER_INT_FMAC_INT_REG_6 = 507,
     * JER_INT_FMAC_INT_REG_7 = 508,
     * JER_INT_FMAC_INT_REG_8 = 509,
     * JER_INT_FMAC_ECC_PARITY_ERR_INT = 510,
     * JER_INT_FMAC_ECC_ECC_1B_ERR_INT = 511,
     * JER_INT_FMAC_ECC_ECC_2B_ERR_INT = 512,
     */
    int event_ids[12] = {501, 502, 503, 504, 505, 506, 507, 508, 509, 510, 511, 512};
    int event_num = 12;

    for (i=0 ; i < event_num ; i++)
    {
        type.event_id = event_ids[i];
        rv = bcm_switch_event_control_set(unit, switch_event, type, value);
        if(rv != 0) {
            printf("Error, in init_fmac_interrupts, FMAC %d\n", index);
            return rv;
        }
    }

    return 0;
}

int mask_fsrd_interrupts(int unit, int index)
{
    int rv, i;
    bcm_switch_event_t switch_event;
    bcm_switch_event_control_t type;
    uint32 value = 0;
    switch_event = BCM_SWITCH_EVENT_DEVICE_INTERRUPT;
    type.index = index;
    type.action = bcmSwitchEventMask;

    /*
     * JER_INT_FSRD_ERROR_ECC = 1344
     * JER_INT_FSRD_ECC_PARITY_ERR_INT = 1348,
     * JER_INT_FSRD_ECC_ECC_1B_ERR_INT = 1349,
     * JER_INT_FSRD_ECC_ECC_2B_ERR_INT = 1350,
 */
    int event_ids[4] = {1344, 1348, 1349, 1350};
    int event_num = 4;

    for (i=0 ; i < event_num ; i++)
    {
        type.event_id = event_ids[i];
        rv = bcm_switch_event_control_set(unit, switch_event, type, value);
        if(rv != 0) {
            printf("Error, in init_fmac_interrupts, FMAC %d\n", index);
            return rv;
        }
    }

    return 0;
}

