/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
int cint_dnxc_nof_interrupts = -1;
int cint_dnxc_max_block_num = 32;
int cint_inter_print_flag = 0;
int cint_test_intr_id = -1;
uint32 cint_intr_cb_data[2];



void cint_dnxc_interrupt_event_cb(int unit, bcm_switch_event_t event,uint32 arg1, uint32 arg2, uint32 arg3, void *data)
{
    uint32 *intr_data;
    soc_interrupt_db_t interrupt_info;
    /* Process only corrected events */
    if (BCM_SWITCH_EVENT_DEVICE_INTERRUPT != event)
    {
        printf("event is not BCM_SWITCH_EVENT_DEVICE_INTERRUPT\n");
        return;
    }

    if (cint_inter_print_flag)
    {
        soc_interrupt_info_get(unit, arg1, &interrupt_info);
        printf("Interrupt %d(%s), block %d\n", arg1, interrupt_info.name, arg2);
    }
    intr_data = data;
    if (intr_data != NULL)
    {
        intr_data[0] = arg1;
        intr_data[1] = arg2;
    }
}

int cint_dnxc_interrupt_register(int unit, void* userdata)
{
    int  rv;
    bcm_switch_event_control_t type;

    /* set bcmSwitchEventCorrActOverride to 1:  Only the user callback will be called */
    type.event_id = BCM_SWITCH_EVENT_CONTROL_ALL;
    type.action = bcmSwitchEventCorrActOverride;
    type.index = 0;
    rv = bcm_switch_event_control_set(unit,BCM_SWITCH_EVENT_DEVICE_INTERRUPT, type, 1);
    if (rv != 0)
    {
       printf("bcm_switch_event_control_set: set bcmSwitchEventCorrActOverride failure\n");
    }

    rv = bcm_switch_event_register(unit, &cint_dnxc_interrupt_event_cb, userdata);
    if (rv != 0)
    {
       printf("bcm_switch_event_register!=OK :( \n");
    }

    return rv;
}

int cint_dnxc_interrupt_unregister(int unit, void* userdata)
{
    int  rv, ii, jj;
    bcm_switch_event_control_t type;

    /* set bcmSwitchEventCorrActOverride to 0:  Only the user callback will be called */

    /*
    0
    ???bcm?call back
    SOC_INTERRUPT_DB_FLAGS_BCM_AND_USR_CB = 0
    SOC_INTERRUPT_DB_FLAGS_CORR_ACT_OVERRIDE_ENABLE = 0
    1
    ??????call back,?bcm?call back????event??,action none?
    SOC_INTERRUPT_DB_FLAGS_BCM_AND_USR_CB = 0
    SOC_INTERRUPT_DB_FLAGS_CORR_ACT_OVERRIDE_ENABLE = 1
    2
    User call back will be called immediately after bcm callback.
    SOC_INTERRUPT_DB_FLAGS_BCM_AND_USR_CB = 1
    SOC_INTERRUPT_DB_FLAGS_CORR_ACT_OVERRIDE_ENABLE = 1
    */
    for (ii=0; ii<cint_dnxc_nof_interrupts; ii++)
    {
        type.event_id = ii;
        type.action = bcmSwitchEventCorrActOverride;
        type.index = 0;
        rv = bcm_switch_event_control_set(unit,BCM_SWITCH_EVENT_DEVICE_INTERRUPT, type, 0);
        if (rv != 0)
        {
           printf("bcm_switch_event_control_set: set bcmSwitchEventCorrActOverride failure\n");
        }
    }

    rv = bcm_switch_event_unregister(unit, &cint_dnxc_interrupt_event_cb, userdata);
    if (rv != 0)
    {
       printf("bcm_switch_event_unregister!=OK \n");
    }

    return rv;
}

int cint_unmask_interrupt(int unit, int start_event_id, int end_event_id)
{
    int rv = 0, event_id, index;
    bcm_switch_event_control_t type;

    if (end_event_id >= cint_dnxc_nof_interrupts)
    {
        end_event_id = cint_dnxc_nof_interrupts - 1;
    }

    for (event_id=start_event_id; event_id<=end_event_id; event_id++)
    {
        /* Disable Log */
        type.event_id = event_id;
        type.index = BCM_CORE_ALL;
        type.action = bcmSwitchEventLog;
        rv = bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, type, 0);
        if (rv != 0)
        {
           printf("bcm_switch_event_control_set: set bcmSwitchEventLog failure\n");
        }

        /* Force Unmask */
        type.event_id = event_id;
        type.index = BCM_CORE_ALL;
        type.action = bcmSwitchEventForceUnmask;
        rv = bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, type, 1);
        if (rv != 0)
        {
           printf("bcm_switch_event_control_set: set bcmSwitchEventForceUnmask failure\n");
        }

        /* unmask the interrupt */
        type.event_id = event_id;
        type.index = BCM_CORE_ALL;
        type.action = bcmSwitchEventMask;
        rv = bcm_switch_event_control_set(unit,BCM_SWITCH_EVENT_DEVICE_INTERRUPT, type, 0);
        if (rv != 0)
        {
           printf("bcm_switch_event_control_set: set bcmSwitchEventMask failure\n");
        }
    }

    return rv;
}


int cint_force_interrupt(int unit, int start_event_id, int end_event_id)
{
    int rv = 0, event_id, index, asserted;
    bcm_switch_event_control_t type;

    if (end_event_id >= cint_dnxc_nof_interrupts)
    {
        end_event_id = cint_dnxc_nof_interrupts - 1;
    }

    /* Assert interrupt */
    asserted = 0;
    for (event_id=start_event_id; event_id<=end_event_id; event_id++)
    {
        for (index=0; index<cint_dnxc_max_block_num; index++)
        {
            if (soc_interrupt_is_supported(unit, index, event_id))
            {
                asserted = 1;
                cint_intr_cb_data[0] = 0;
                cint_intr_cb_data[1] = 0;

                type.event_id = event_id;
                type.index = index;
                type.action = bcmSwitchEventForce;
                bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, type, 0);
                bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, type, 1);

                sal_usleep(200000);
                if ((cint_intr_cb_data[0] == event_id) && (cint_intr_cb_data[1] == index))
                {
                    printf("    OK for event_id=%d, index=%d\n", event_id, index);
                }
                else
                {
                    printf("    Error for event_id=%d, index=%d, [%d/%d]\n", event_id, index,
                           cint_intr_cb_data[0], cint_intr_cb_data[1]);
                    rv = -1;
                }
            }
        }
    }

    if (asserted)
    {
        return rv;
    }
    else
    {
        printf("No interrupt was asserted!! %d-%d\n", start_event_id, end_event_id);
        return -2;
    }
}

int cint_interrupt_test_init(int unit)
{
    soc_nof_interrupts(unit, &cint_dnxc_nof_interrupts);
    printf("nof interrupt is %d\n", cint_dnxc_nof_interrupts);

    cint_dnxc_max_block_num = 32;
    sal_memset(cint_intr_cb_data, 0, sizeof(cint_intr_cb_data));
    cint_inter_print_flag = 0;

    return 0;
}

int cint_get_valid_rand_intr_id(int unit)
{
    int intr_id;

    if (cint_test_intr_id == -1)
    {
        intr_id = sal_rand();
        intr_id = intr_id % cint_dnxc_nof_interrupts;
        while (!soc_interrupt_is_supported(unit, 0, intr_id))
        {
            printf("intr_id %d is not supported!!\n", intr_id);
            intr_id = sal_rand();
            intr_id = intr_id % cint_dnxc_nof_interrupts;
        }
    }
    else
    {
        intr_id = cint_test_intr_id;
    }

    return intr_id;
}


int cint_interrupt_cb_test(int unit)
{
    int rv;
    bcm_switch_event_control_t type;
    int block_instance = 0;

    soc_nof_interrupts(unit, &cint_dnxc_nof_interrupts);
    printf("nof interrupt is %d\n", cint_dnxc_nof_interrupts);

    /* set bcmSwitchEventCorrActOverride to 1:  Only the user callback will be called */
    type.event_id = BCM_SWITCH_EVENT_CONTROL_ALL;
    type.action = bcmSwitchEventCorrActOverride;
    type.index = 0;
    rv = bcm_switch_event_control_set(unit,BCM_SWITCH_EVENT_DEVICE_INTERRUPT, type, 1);
    if (rv != 0)
    {
       printf("bcm_switch_event_control_set: set bcmSwitchEventCorrActOverride failure\n");
    }

    /* register intr call back */
    rv = bcm_switch_event_register(unit, &cint_dnxc_interrupt_event_cb, cint_intr_cb_data);
    if (rv != 0)
    {
       printf("bcm_switch_event_register!=OK :( \n");
    }

    /* get rand interrupt id */
    if (cint_test_intr_id == -1)
    {
        cint_test_intr_id = sal_rand();
        cint_test_intr_id = cint_test_intr_id % cint_dnxc_nof_interrupts;
        while (!soc_interrupt_is_supported(unit, block_instance, cint_test_intr_id))
        {
            printf("test_intr_id %d is not supported!!\n", cint_test_intr_id);
            cint_test_intr_id = sal_rand();
            cint_test_intr_id = cint_test_intr_id % cint_dnxc_nof_interrupts;
        }
    }
    printf("cint_test_intr_id = %d\n", cint_test_intr_id);

    /* unmask the interrupt */
    type.event_id = cint_test_intr_id;
    type.index = BCM_CORE_ALL;
    type.action = bcmSwitchEventMask;
    rv = bcm_switch_event_control_set(unit,BCM_SWITCH_EVENT_DEVICE_INTERRUPT, type, 0);
    if (rv != 0)
    {
       printf("bcm_switch_event_control_set: set bcmSwitchEventMask failure\n");
    }

    /* Assert interrupt */
    printf("Force interrupt %d, block=%d\n", cint_test_intr_id, block_instance);
    cint_intr_cb_data[0] = 0;
    cint_intr_cb_data[1] = 0;
    type.event_id = cint_test_intr_id;
    type.index = block_instance;
    type.action = bcmSwitchEventForce;
    bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, type, 0);
    bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, type, 1);

    /* Check callback result */
    sal_usleep(200000);
    rv = 0;
    if ((cint_intr_cb_data[0] == cint_test_intr_id) && (cint_intr_cb_data[1] == block_instance))
    {
        printf("    OK for test_intr_id=%d, block_instance=%d\n", cint_test_intr_id, block_instance);
    }
    else
    {
        printf("    Error for test_intr_id=%d, block_instance=%d, [%d/%d]\n", cint_test_intr_id, block_instance,
               cint_intr_cb_data[0], cint_intr_cb_data[1]);
        rv = -1;
    }

    return rv;
}


void
cint_dnx_test_hard_reset_handle(
    int unit,
    uint32 flags,
    void *userdata)
{
    printf("cint_dnx_test_hard_reset_handle!!!!\n");
}


int cint_interrupt_hard_rst_test(int unit)
{
    int rc;

    rc = bcm_switch_hard_reset_cb_register(unit, 0, cint_dnx_test_hard_reset_handle, NULL);
}



void cint_dnxc_nmg_link_staus_change_event_cb(int unit, bcm_switch_event_t event,uint32 arg1, uint32 arg2, uint32 arg3, void *data)
{
    uint32 *intr_data;
    soc_interrupt_db_t interrupt_info;

    /* Process only corrected events */
    if (BCM_SWITCH_EVENT_DEVICE_INTERRUPT != event)
    {
        printf("event is not BCM_SWITCH_EVENT_DEVICE_INTERRUPT\n");
        return;
    }

    intr_data = data;
    if ((arg1 < intr_data[0]) || (arg1 > intr_data[1]))
    {
        return;
    }

    soc_interrupt_info_get(unit, arg1, &interrupt_info);
    printf("Interrupt %d(%s), block %d, physical, port=%d:\n",
           arg1, interrupt_info.name, arg2, (arg1-intr_data[0]) + arg2*48);
}


int cint_dnxc_nmg_link_change_test(int unit, int start_event_id, int end_event_id)
{
    int  rv;
    bcm_switch_event_control_t type;
    int event_id;

    /* set bcmSwitchEventCorrActOverride to 1:  Only the user callback will be called */
    type.event_id = BCM_SWITCH_EVENT_CONTROL_ALL;
    type.action = bcmSwitchEventCorrActOverride;
    type.index = 0;
    rv = bcm_switch_event_control_set(unit,BCM_SWITCH_EVENT_DEVICE_INTERRUPT, type, 1);
    if (rv != 0)
    {
       printf("bcm_switch_event_control_set: set bcmSwitchEventCorrActOverride failure\n");
    }

    cint_intr_cb_data[0] = start_event_id;
    cint_intr_cb_data[1] = end_event_id;
    rv = bcm_switch_event_register(unit, &cint_dnxc_nmg_link_staus_change_event_cb, cint_intr_cb_data);
    if (rv != 0)
    {
       printf("bcm_switch_event_register!=OK :( \n");
    }


    for (event_id=start_event_id; event_id<=end_event_id; event_id++)
    {
        /* Force Unmask the interrupt */
        type.event_id = event_id;
        type.index = BCM_CORE_ALL;
        type.action = bcmSwitchEventForceUnmask;
        rv = bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, type, 1);
        if (rv != 0)
        {
           printf("bcm_switch_event_control_set: set bcmSwitchEventForceUnmask failure\n");
        }

        /* unmask the interrupt */
        type.event_id = event_id;
        type.index = BCM_CORE_ALL;
        type.action = bcmSwitchEventMask;
        rv = bcm_switch_event_control_set(unit,BCM_SWITCH_EVENT_DEVICE_INTERRUPT, type, 0);
        if (rv != 0)
        {
           printf("bcm_switch_event_control_set: set bcmSwitchEventMask failure\n");
        }
    }
}


int cint_dnxc_interrupt_override_cb_none(int unit)
{
    int rv;
    bcm_switch_event_control_t type;
    int block_instance = 0;
    int nof_interrupts;

    soc_nof_interrupts(unit, &nof_interrupts);
    printf("nof interrupt is %d\n", nof_interrupts);

    /* register intr call back */
    rv = bcm_switch_event_register(unit, soc_interrupt_event_cb_none, NULL);
    if (rv != 0)
    {
       printf("bcm_switch_event_register!=OK :( \n");
    }

    /* set bcmSwitchEventCorrActOverride to 1:  Only the user callback will be called */
    type.event_id = BCM_SWITCH_EVENT_CONTROL_ALL;
    type.action = bcmSwitchEventCorrActOverride;
    type.index = 0;
    rv |= bcm_switch_event_control_set(unit,BCM_SWITCH_EVENT_DEVICE_INTERRUPT, type, 1);
    if (rv != 0)
    {
        printf("bcm_switch_event_control_set: set bcmSwitchEventCorrActOverride failure\n");
    }
    return rv;
}



