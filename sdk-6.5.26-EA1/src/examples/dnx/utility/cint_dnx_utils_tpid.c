/*
 * Utility functions for Switch/Port TPID in Jericho2 devices.
 */

int
tpid__tpids_clear_all(
    int unit)
{
    int rv = 0, traffic_enable;

    /*
     * Note: Traffic should be halted when configuring a TPID
     * This was will delete all the default settings (see appl_dnx_vlan_init).
     */

    traffic_enable = 0;
    rv = bcm_stk_module_enable(unit, 0, 0, traffic_enable);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_stk_module_enable() Failed to disable traffic, rv = %d\n", rv);
        return rv;
    }
    rv = bcm_switch_tpid_delete_all(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_switch_tpid_delete_all\n");
        return rv;
    }

    traffic_enable = 1;
    rv = bcm_stk_module_enable(unit, 0, 0, traffic_enable);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_stk_module_enable() Failed to enable traffic, rv = %d\n", rv);
        return rv;
    }

    return rv;
}

int
tpid__tpid_add(
    int unit,
    int tpid)
{
    int rv, traffic_enable;

    /*
     * Note: Traffic should be halted when configuring a TPID 
     */
    traffic_enable = 0;
    rv = bcm_stk_module_enable(unit, 0, 0, traffic_enable);
    if (rv != BCM_E_NONE)
    {
       printf("Error, bcm_stk_module_enable() Failed to disable traffic, rv = %d\n", rv);
       return rv;
    }

    bcm_switch_tpid_info_t tpid_info;
    int options = 0;
    tpid_info.tpid_value = tpid;
    rv = bcm_switch_tpid_add(unit, options, &tpid_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_switch_tpid_add\n");
        return rv;
    }

    traffic_enable = 1;
    rv = bcm_stk_module_enable(unit, 0, 0, traffic_enable);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_stk_module_enable() Failed to enable traffic, rv = %d\n", rv);
        return rv;
    }

    return rv;
}
