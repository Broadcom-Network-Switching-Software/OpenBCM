/* $Id: cint_sand_utils_tpid.c,
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Utility functions for Switch/Port TPID in Jericho1 and Jericho2 devices.
 */

int
tpid__tpids_clear_all(
    int unit)
{
    int rv = 0, traffic_enable;
    
    if (!is_device_or_above(unit, JERICHO2))
    {
        /*
         * In JR1 run over all ETH ports and run delete all tpids 
         */
        bcm_port_config_t port_config;
        bcm_port_config_t_init(&port_config);
        bcm_port_t port;
        rv = bcm_port_config_get(unit, &port_config);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_port_config_get\n");
            return rv;
        }
        BCM_PBMP_ITER(port_config.e, port)
        {
            rv = bcm_port_tpid_delete_all(unit, port);
            if (rv != BCM_E_NONE)
            {
                printf("Error, bcm_port_tpid_add\n");
                return rv;
            }
        }
    }
    else
    {
       /*
        * In JR2 delete all global tpids 
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
    }

    return rv;
}

int
tpid__tpid_add(
    int unit,
    int tpid)
{
    int rv, traffic_enable;

    if (!is_device_or_above(unit, JERICHO2))
    {
        /*
         * In JR1 run over all ETH ports and add TPID - up to 2 are avaiable 
         */
        bcm_port_config_t port_config;
        bcm_port_config_t_init(&port_config);
        bcm_port_t port;
        rv = bcm_port_config_get(unit, &port_config);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_port_config_get\n");
            return rv;
        }
        BCM_PBMP_ITER(port_config.e, port)
        {
            rv = bcm_port_tpid_add(unit, port, tpid, 0);
            if (rv != BCM_E_NONE)
            {
                printf("Error, bcm_port_tpid_add\n");
                return rv;
            }
        }
    }
    else
    {
        /*
         * In JR2 add TPID to global array 
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
    }

    return rv;
}
