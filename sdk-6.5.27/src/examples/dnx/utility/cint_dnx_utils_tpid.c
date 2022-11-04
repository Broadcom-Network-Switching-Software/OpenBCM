/*
 * Utility functions for Switch/Port TPID in Jericho2 devices.
 */

int
tpid__tpids_clear_all(
    int unit)
{
    int traffic_enable;

    /*
     * Note: Traffic should be halted when configuring a TPID
     * This was will delete all the default settings (see appl_dnx_vlan_init).
     */

    traffic_enable = 0;
    BCM_IF_ERROR_RETURN_MSG(bcm_stk_module_enable(unit, 0, 0, traffic_enable), "Failed to disable traffic");
    BCM_IF_ERROR_RETURN_MSG(bcm_switch_tpid_delete_all(unit), "");

    traffic_enable = 1;
    BCM_IF_ERROR_RETURN_MSG(bcm_stk_module_enable(unit, 0, 0, traffic_enable), "Failed to enable traffic");

    return BCM_E_NONE;
}

int
tpid__tpid_add(
    int unit,
    int tpid)
{
    int traffic_enable;

    /*
     * Note: Traffic should be halted when configuring a TPID 
     */
    traffic_enable = 0;
    BCM_IF_ERROR_RETURN_MSG(bcm_stk_module_enable(unit, 0, 0, traffic_enable), "Failed to disable traffic");

    bcm_switch_tpid_info_t tpid_info;
    int options = 0;
    tpid_info.tpid_value = tpid;
    BCM_IF_ERROR_RETURN_MSG(bcm_switch_tpid_add(unit, options, &tpid_info), "");

    traffic_enable = 1;
    BCM_IF_ERROR_RETURN_MSG(bcm_stk_module_enable(unit, 0, 0, traffic_enable), "Failed to enable traffic");

    return BCM_E_NONE;
}
