/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_field_action_config.c
 * Purpose: Example Usage of egress PMF actions using ACE table
 * 
 * Fuctions:
 * Main function:
 *      stat_config_main()
 * Configuration functions:
 *  L2 config:
 *      create_vlan_and_ports() - to be used for all cases
 *      create_lif() - to be used for out-LIF redirection in addition to create_vlan_and_ports()
 *  PMF config:
 *      field_config()
 * 
 * Result verification functions:
 *      run_verify_redir() - for cases with port redirection
 *      run_verify_newlif() - for cases with out-LIF redirection
 * 
 * Cleanup function:
 *      field_destroy()
 */

int statId; /* Statistics ID for counting */
bcm_gport_t outlif; /* with out-LIF redirection, the new LIF to redirect to */

/* function to check if the device is Jericho and above */
int device_is_jer(int unit,
                  uint8* is_jer) {
    bcm_info_t info;
    char *proc_name;

    proc_name = "device_is_jer";
    printf("%s(): Checking if device is Jericho...\n", proc_name);
    int rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in: bcm_info_get() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf(" info.device = 0x%x...\n", info.device);
    *is_jer = ((info.device == 0x8375) || (info.device == 0x8675) || (info.device == 0x8470) || (info.device == 0x8680)) || (info.device == 0x8270);
    if (*is_jer) {
        printf("%s(): Done. Device is Jericho or above.\n", proc_name);
    } else {
        printf("%s(): Done. Device is below Jericho.\n", proc_name);
    }
    return rv;
}
/* function to check if the device is Qumran AX */
int device_is_qax(int unit,
                  uint8* is_qax) {
    bcm_info_t info;
    char *proc_name;

    proc_name = "device_is_qax";
    printf("%s(): Checking if device is QAX...\n", proc_name);
    int rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in: bcm_info_get() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf(" info.device = 0x%x...\n", info.device);
    *is_qax = (info.device == 0x8470)||(info.device == 0x8270);
    if (*is_qax) {
        printf("%s(): Done. Device is QAX.\n", proc_name);
    } else {
        printf("%s(): Done. Device is not QAX.\n", proc_name);
    }
    return rv;
}


/*Create VLAN and add in_port and out_port to this VLAN.*/
int create_vlan_and_ports(int unit,
                          int vsi,
                          int in_port,
                          int out_port,
                          int redir_port,
                          int is_redir) {
    int rv = BCM_E_NONE;
    bcm_vlan_port_t vlan_port;
    bcm_mac_t sa_mac = {0x01,0x02,0x03,0x04,0x05,0x06};
    bcm_mac_t da_mac = {0x06,0x05,0x04,0x03,0x02,0x01};
    char *proc_name;

    proc_name = "create_vlan_and_ports";
    /* Create VSI */
    printf("%s(): Creating VSI on VLAN %d...", proc_name, vsi);
    rv = bcm_vlan_create(unit, vsi);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in: bcm_vlan_create() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf(" Done.\n");

    /* Configure in-port */
    printf("%s(): Adding inport %d to VLAN %d...", proc_name, in_port, vsi);
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN; 
    vlan_port.match_vlan = vsi;
    vlan_port.vsi = vsi;
    vlan_port.port = in_port;
    rv = bcm_vlan_port_create(unit, vlan_port);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in: bcm_vlan_port_create returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf(" Done.\n");
    /* Configure a MAC address for the in-port, to not learn SA MAC. */
    printf("%s(): Adding MACT entry for in-port...", proc_name);
    l2__mact_properties_s mact_entry0 = {vlan_port.vlan_port_id,
                                        sa_mac,
                                        vlan_port.vsi};
    rv = l2__mact_entry_create(unit, &mact_entry0);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in: l2__mact_entry_create() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf(" Done.\n");

    /* Configure out-port */
    printf("%s(): Adding outport %d to VLAN %d...", proc_name, out_port, vsi);
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN; 
    vlan_port.match_vlan = vsi;
    vlan_port.vsi = vsi;
    vlan_port.port = out_port;
    rv = bcm_vlan_port_create(unit, vlan_port);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, bcm_vlan_port_create() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf(" Done.\n");
    /* Configure a MAC address for the out-port, to forward. */
    printf("%s(): Adding MACT entry for out-port...", proc_name);
    l2__mact_properties_s mact_entry1 = {vlan_port.vlan_port_id,
                                        da_mac,
                                        vlan_port.vsi};
    rv = l2__mact_entry_create(unit, &mact_entry1);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in l2__mact_entry_create() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf(" Done.\n");

    if (is_redir) {
        /* Configure redirect port */
        printf("%s(): Adding redirect port %d to VLAN %d...", proc_name, redir_port, vsi);
        bcm_vlan_port_t_init(&vlan_port);
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN; 
        vlan_port.match_vlan = vsi;
        vlan_port.vsi = vsi;
        vlan_port.port = redir_port;
        rv = bcm_vlan_port_create(unit, vlan_port);
        if (rv != BCM_E_NONE) {
            printf("%s(): Error, bcm_vlan_port_create() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
            return rv;
        }
        printf(" Done.\n");
    }

    printf("%s(): Done. Created VLAN %d with ports.\n", proc_name, vsi);
    return rv;
}

/*Create VLAN-port object (for out-LIF redirection)*/
int create_lif(int unit,
               int vsi,
               int redir_port,
               int *newlif) {
    int rv;
    bcm_vlan_port_t vlan_port;
    bcm_gport_t redir_gport;
    bcm_gport_t local_outlif;
    char *proc_name;

    proc_name = "create_lif";
    /* Set gport */
    printf("%s(): Set gport for redirect port %d...", proc_name, redir_port);
    BCM_GPORT_SYSTEM_PORT_ID_SET(redir_gport, redir_port);
    printf(" Done. Gport = 0x%x\n", redir_gport);

    /* Create VLAN port */
    printf("%s(): Adding outport %d (gport=0x%x) to VLAN %d...\r\n", proc_name, redir_port, redir_gport, vsi);
    bcm_vlan_port_t_init (&vlan_port);
    vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
    vlan_port.vsi = vsi;
    vlan_port.port = redir_gport;
    rv = bcm_vlan_port_create(unit, vlan_port);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, bcm_vlan_port_create() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf("%s(): Done. VLAN port ID = 0x%x\n", proc_name, vlan_port.vlan_port_id);

    /* Get LIF value */
    printf("%s(): Getting LIF value from gport 0x%x...", proc_name, vlan_port.vlan_port_id);
    local_outlif = BCM_GPORT_SUB_TYPE_LIF_VAL_GET(vlan_port.vlan_port_id);
    *newlif = local_outlif;
    printf(" Done. LIF = 0x%x\n", local_outlif);

    printf("%s(): Done. Created outlif %d.\n", proc_name, *newlif);
    return rv;
}

/*Create VLAN-port object (for out-LIF on out port to be used for vport range)*/
int create_lif_for_range(int unit,
               int vsi,
               int out_port) {
    int rv;
    bcm_vlan_port_t vlan_port;
    bcm_gport_t out_gport;
    bcm_gport_t local_outlif;
    char *proc_name;

    proc_name = "create_lif_for_range";
    /* Set gport */
    printf("%s(): Set gport for out port %d...", proc_name, out_port);
    BCM_GPORT_SYSTEM_PORT_ID_SET(out_gport, out_port);
    printf(" Done. Gport = 0x%x\n", out_gport);

    /* Create VLAN port */
    printf("%s(): Adding outport %d (gport=0x%x) to VLAN %d...\r\n", proc_name, out_port, out_gport, vsi);
    bcm_vlan_port_t_init (&vlan_port);
    vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
    vlan_port.vsi = vsi;
    vlan_port.port = out_gport;
    rv = bcm_vlan_port_create(unit, vlan_port);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, bcm_vlan_port_create() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf("%s(): Done. VLAN port ID = 0x%x\n", proc_name, vlan_port.vlan_port_id);

    /* Get LIF value */
    printf("%s(): Getting LIF value from gport 0x%x...", proc_name, vlan_port.vlan_port_id);
    local_outlif = BCM_GPORT_SUB_TYPE_LIF_VAL_GET(vlan_port.vlan_port_id);
    printf(" Done. Created outlif = 0x%x (%d)\n", local_outlif, local_outlif);
    /*
     * Load newly retrieved LIF of out_port into a global variable because it is
     * not straighforward to pass arguments by reference from TCL to CINT.
     */
    outlif = local_outlif ;
    return rv;
}

/* Function to configure Field Processor with ActionStat
 *  
 * Can be used in 4 combinations (cases): 
 *  1. To count only, pass arguments:
 *      - redir_port = any
 *      - is_count = 1
 *      - is_redir = 0
 *      - is_newlif = 0
 *  2. To count and redirect, pass arguments:
 *      - redir_port = redirection local port
 *      - is_count = 1
 *      - is_redir = 1
 *      - is_newlif = 0
 *  3. To count, redirect and change out-LIF, pass arguments:
 *      - redir_port = redirection local port
 *      - is_count = 1
 *      - is_redir = 1
 *      - is_newlif = 1
 *  4. To redirect only
 *      - redir_port = redirection local port
 *      - is_count = 0
 *      - is_redir = 1
 *      - is_newlif = 0
 */
int field_config(int unit, 
                 int core, 
                 int counter_id_0, 
                 int counter_proc, 
                 bcm_field_group_t group, 
                 int group_priority, 
                 bcm_port_t out_port,
                 bcm_port_t redir_port, /* redirection local port, pass any if not apply */
                 bcm_gport_t gport, /* out-lif gport id / modport, pass any if not apply */
                 int is_count,
                 int is_redir, 
                 int is_newlif) {
    int rv = BCM_E_NONE;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent;
    int action;
    bcm_gport_t out_gport, redir_gport;
    int modId = 0;
    int cores_num = 1;
    bcm_field_action_core_config_t core_config_arr[2];
    int tcase = 0;
    int i = 0;
    int my_param0, my_param1, my_param2;
    unsigned long my_param0_as_long, my_param1_as_long, my_param2_as_long;
    uint8 is_jer, is_qax;
    char *proc_name;
    
    proc_name = "field_config";
    printf("%s(): Configure field processor.\n", proc_name);

    rv = device_is_jer(unit, &is_jer);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : device_is_jer() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    if(is_jer) {
        /* devices Jericho and above are using bcm_action_config_add API,
         * specifying number of cores: 2 for Jericho and 1 for QAX */
        rv = device_is_qax(unit, &is_qax);
        if (rv != BCM_E_NONE) {
            printf("%s(): Error in : device_is_qax() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
            return rv;
        }
        if(is_qax) {
            cores_num = 1;
        } else {
            cores_num = 2;
        }
    }

    if (is_count) {
        action = bcmFieldActionStat;
        /* Set the Stat ID */
        BCM_FIELD_STAT_ID_SET(&statId, counter_proc, counter_id_0);
        printf("%s(): Counter processor = %d, Counter ID = %d, Stat ID = %d\n", proc_name, counter_proc, counter_id_0, statId);
        if(!is_redir) {
             if (!is_newlif) { /* case 1: count only */
                 tcase = 1;
             }
        } else {
            if(!is_newlif) { /* case 2: count+redirect */
                tcase = 2;
            } else { /* case 3: count+redirect+newlif */
                tcase = 3;
            }
        }
    } else { 
        if(is_redir) {
            if(!is_newlif) {/* case 4: redirect only */
                action = bcmFieldActionRedirect;
                tcase = 4;
            }
        }
    }

    /*Build the qset*/
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageEgress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDstPort);

    /*Build the aset*/
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, action);

    /*Create group with given qset, id and priority*/
    rv = bcm_field_group_create_id(unit, qset, group_priority, group);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_group_create_id returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf("%s(): Group %d created.\n", proc_name, group);

    /*Attach aset to group*/
    rv = bcm_field_group_action_set(unit, group, aset);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_group_action_set returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf("%s(): ASET attached.\n", proc_name);
    /*Create empty entry*/
    rv = bcm_field_entry_create(unit, group, &ent);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_entry_create returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf("%s(): Entry %d created.\n", proc_name, ent);
    /*Set the qualifier value for entry*/
    BCM_GPORT_LOCAL_SET(out_gport, out_port);
    rv = bcm_field_qualify_DstPort(unit, ent, 0, 0, out_gport, 0xffffffff);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_qualify_DstPort returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf("%s(): Added qualifier: bcm_field_qualify_DstPort.\n", proc_name);

    /*Set the action value for entry*/
    printf("%s(): Adding action...\n", proc_name);
    if(is_jer) {
        switch (tcase) {
        case 1:
            printf("%s(): Case '1', count only", proc_name);
            my_param0 = statId;
            my_param1 = BCM_ILLEGAL_ACTION_PARAMETER;
            my_param2 = BCM_ILLEGAL_ACTION_PARAMETER;
            break;
        case 2:
            printf("%s(): Case '2', count + redirect", proc_name);
            BCM_GPORT_LOCAL_SET(redir_gport, redir_port);
            my_param0 = statId;
            my_param1 = redir_gport;
            my_param2 = BCM_ILLEGAL_ACTION_PARAMETER;
            break;
        case 3:
            printf("%s(): Case '3', count +redirect + change LIF", proc_name);
            BCM_GPORT_LOCAL_SET(redir_gport, redir_port);
            my_param0 = statId;
            my_param1 = redir_gport;
            my_param2 = gport;

            /* This new outlif is not programmed, so it should not be filter. Otherwise the packet will be dropped. */ 
            rv = bcm_port_control_set(unit,redir_gport,bcmPortControlEgressFilterDisable,BCM_PORT_CONTROL_FILTER_DISABLE_ALL);
            if (rv != BCM_E_NONE) {
                return rv;
            }

            break;
        case 4:
            printf("%s(): Case '4', redirect only", proc_name);
            BCM_GPORT_LOCAL_SET(redir_gport, redir_port);
            modId = BCM_GPORT_MODPORT_MODID_GET(redir_gport);
            my_param0 = modId;
            my_param1 = redir_gport;
            my_param2 = BCM_ILLEGAL_ACTION_PARAMETER;
            break;
        default:
            printf("%s(): this action combination is not supported\n", proc_name);
            return BCM_E_UNAVAIL;
        }
        my_param0_as_long = my_param0;
        my_param1_as_long = my_param1;
        my_param2_as_long = my_param2;
        printf("  param0 = 0x%08lX\n  param1 = 0x%08lX\n  param2 = 0x%08lX\n\r\n",
               my_param0_as_long, my_param1_as_long, my_param2);

        for(i = 0; i < cores_num; i++) {
            core_config_arr[i].param0 = my_param0;
            core_config_arr[i].param1 = my_param1;
            core_config_arr[i].param2 = my_param2;
        }

        rv = bcm_field_action_config_add(unit, ent, action, cores_num, &core_config_arr[0]);
        if (rv != BCM_E_NONE) {
            printf("%s(): Error in : bcm_field_action_config_add() returned %d(%s)\n", proc_name, rv, bcm_errmsg(rv));
            return rv;
        }
        printf("%s(): bcm_field_action_config_add() done.\n", proc_name);
    } else { /* for ARAD */
        switch (tcase) {
            case 2:
                printf("%s(): Case '2', count + redirect", proc_name);
                BCM_GPORT_LOCAL_SET(redir_gport, redir_port);
                my_param0 = statId;
                my_param1 = redir_gport;
                break;
            case 4:
                printf("%s(): Case '4', redirect only", proc_name);
                BCM_GPORT_LOCAL_SET(redir_gport, redir_port);
                modId = BCM_GPORT_MODPORT_MODID_GET(redir_gport);
                my_param0 = modId;
                my_param1 = redir_gport;
                break;
            default:
                printf("%s(): this action combination is not supported\n", proc_name);
                return BCM_E_UNAVAIL;
        }
        my_param0_as_long = my_param0;
        my_param1_as_long = my_param1;
        printf("  param0 = 0x%08lX\n  param1 = 0x%08lX\n\r\n",my_param0_as_long, my_param1_as_long);

        rv = bcm_field_action_add(unit, ent, action, my_param0, my_param1);
        if (rv != BCM_E_NONE) {
            printf("%s(): Error in : bcm_field_action_add() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
            return rv; 
        }
        printf("%s(): bcm_field_action_add() done.\n", proc_name);
    }

    /*Commit the entire group to hardware.*/
    printf("%s(): Commit group %d to hardware...", proc_name, group);
    rv = bcm_field_group_install(unit, group);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_group_install() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf(" Done.\n");

    printf("%s(): Done. Field processor configured.\n", proc_name);
    return rv;
}



/* Function to configure Field Processor for outlif-range (using,
 * also, ActionStat)
 * NOTE: This procedure is for Jericho (and up) only!
 *  
 * Counting is done using bcm_field_action_config_add(). 
 */
int field_config_for_range(int unit, 
                 int core, 
                 int counter_id_0, 
                 int counter_proc, 
                 bcm_field_group_t group, 
                 int group_priority, 
                 bcm_port_t out_port)
{
    int rv = BCM_E_NONE;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent;
    int action;
    bcm_gport_t out_gport, redir_gport;
    int modId = 0;
    int cores_num = 1;
    bcm_field_action_core_config_t core_config_arr[2];
    int i = 0;
    int my_param0, my_param1, my_param2;
    unsigned long my_param0_as_long, my_param1_as_long, my_param2_as_long;
    uint8 is_jer, is_qax;
    char *proc_name;
    
    proc_name = "field_config_for_range";
    printf("%s(): Configure field processor.\n", proc_name);

    rv = device_is_jer(unit, &is_jer);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error in : device_is_jer() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    if (is_jer)
    {
        /* devices Jericho and above are using bcm_action_config_add API,
         * specifying number of cores: 2 for Jericho and 1 for QAX */
        rv = device_is_qax(unit, &is_qax);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error in : device_is_qax() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
            return rv;
        }
        if(is_qax)
        {
            cores_num = 1;
        }
        else
        {
            cores_num = 2;
        }
    }
    else
    {
        /*
         * Reject for, say,  ARAD
         */
        printf("%s(): this action combination is not supported\n", proc_name);
        return BCM_E_UNAVAIL;
    }

    action = bcmFieldActionStat;
    /* Set the Stat ID */
    BCM_FIELD_STAT_ID_SET(&statId, counter_proc, counter_id_0);
    printf("%s(): Counter processor = %d, Counter ID = %d, Stat ID = %d\n", proc_name, counter_proc, counter_id_0, statId);

    /*Build the qset*/
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageEgress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDstPort);

    /*Build the aset*/
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, action);

    /*Create group with given qset, id and priority*/
    rv = bcm_field_group_create_id(unit, qset, group_priority, group);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_group_create_id returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf("%s(): Group %d created.\n", proc_name, group);

    /*Attach aset to group*/
    rv = bcm_field_group_action_set(unit, group, aset);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_group_action_set returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf("%s(): ASET attached.\n", proc_name);
    /*Create empty entry*/
    rv = bcm_field_entry_create(unit, group, &ent);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_entry_create returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf("%s(): Entry %d created.\n", proc_name, ent);
    /*Set the qualifier value for entry*/
    BCM_GPORT_LOCAL_SET(out_gport, out_port);
    rv = bcm_field_qualify_DstPort(unit, ent, 0, 0, out_gport, 0xffffffff);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_qualify_DstPort returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf("%s(): Added qualifier: bcm_field_qualify_DstPort.\n", proc_name);

    /*Set the action value for entry*/
    printf("%s(): Adding action...\n", proc_name);
    if(is_jer)
    {
        printf("%s(): Case: count only", proc_name);
        my_param0 = statId;
        my_param1 = BCM_ILLEGAL_ACTION_PARAMETER;
        my_param2 = BCM_ILLEGAL_ACTION_PARAMETER;

        my_param0_as_long = my_param0;
        my_param1_as_long = my_param1;
        my_param2_as_long = my_param2;
        printf("  param0 = 0x%08lX\n  param1 = 0x%08lX\n  param2 = 0x%08lX\n\r\n",
               my_param0_as_long, my_param1_as_long, my_param2);

        for(i = 0; i < cores_num; i++)
        {
            core_config_arr[i].param0 = my_param0;
            core_config_arr[i].param1 = my_param1;
            core_config_arr[i].param2 = my_param2;
        }

        rv = bcm_field_action_config_add(unit, ent, action, cores_num, &core_config_arr[0]);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error in : bcm_field_action_config_add() returned %d(%s)\n", proc_name, rv, bcm_errmsg(rv));
            return rv;
        }
        printf("%s(): bcm_field_action_config_add() done.\n", proc_name);
    }

    /*Commit the entire group to hardware.*/
    printf("%s(): Commit group %d to hardware...", proc_name, group);
    rv = bcm_field_group_install(unit, group);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_group_install() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf(" Done.\n");

    printf("%s(): Done. Field processor configured.\n", proc_name);
    return rv;
}




/* Use this to make combination of actionStat for counting + actionRedirect for change of out-port */
int field_config_2(int unit, 
                 int core, 
                 int counter_id_0, 
                 int counter_proc, 
                 bcm_field_group_t group, 
                 int group_priority, 
                 bcm_port_t out_port,
                 bcm_port_t redir_port) {
    int rv = BCM_E_NONE;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent;
    int action;
    bcm_gport_t out_gport, redir_gport;
    int modId = 0;
    int cores_num = 1;
    bcm_field_action_core_config_t core_config_arr[2];
    int i = 0;
    int my_param0, my_param1, my_param2;
    unsigned long my_param0_as_long, my_param1_as_long, my_param2_as_long;
    uint8 is_jer, is_qax;
    char *proc_name;
    
    proc_name = "field_config";
    printf("%s(): Configure field processor.\n", proc_name);

    rv = device_is_jer(unit, &is_jer);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : device_is_jer() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    if(is_jer) {
        /* devices Jericho and above are using bcm_action_config_add API,
         * specifying number of cores: 2 for Jericho and 1 for QAX */
        rv = device_is_qax(unit, &is_qax);
        if (rv != BCM_E_NONE) {
            printf("%s(): Error in : device_is_qax() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
            return rv;
        }
        if(is_qax) {
            cores_num = 1;
        } else {
            cores_num = 2;
        }
    }
    /* Set the Stat ID */
    BCM_FIELD_STAT_ID_SET(&statId, counter_proc, counter_id_0);
    printf("%s(): Counter processor = %d, Counter ID = %d, Stat ID = %d\n", proc_name, counter_proc, counter_id_0, statId);

    /*Build the qset*/
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageEgress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDstPort);

    /*Build the aset*/
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionStat);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionRedirect);

    /*Create group with given qset, id and priority*/
    rv = bcm_field_group_create_id(unit, qset, group_priority, group);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_group_create_id returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf("%s(): Group %d created.\n", proc_name, group);

    /*Attach aset to group*/
    rv = bcm_field_group_action_set(unit, group, aset);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_group_action_set returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf("%s(): ASET attached.\n", proc_name);
    /*Create empty entry*/
    rv = bcm_field_entry_create(unit, group, &ent);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_entry_create returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf("%s(): Entry %d created.\n", proc_name, ent);
    /*Set the qualifier value for entry*/
    BCM_GPORT_LOCAL_SET(out_gport, out_port);
    rv = bcm_field_qualify_DstPort(unit, ent, 0, 0, out_gport, 0xffffffff);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_qualify_DstPort returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf("%s(): Added qualifier: bcm_field_qualify_DstPort.\n", proc_name);

    /*Set the Stat action value for entry*/
    printf("%s(): Adding action...\n", proc_name);
    if(is_jer) {
        /*for Stat*/
        my_param0 = statId;
        my_param1 = BCM_ILLEGAL_ACTION_PARAMETER;
        my_param2 = BCM_ILLEGAL_ACTION_PARAMETER;
        my_param0_as_long = my_param0;
        my_param1_as_long = my_param1;
        my_param2_as_long = my_param2;
        printf("  param0 = 0x%08lX\n  param1 = 0x%08lX\n  param2 = 0x%08lX\n\r\n",
               my_param0_as_long, my_param1_as_long, my_param2);
        for(i = 0; i < cores_num; i++) {
            core_config_arr[i].param0 = my_param0;
            core_config_arr[i].param1 = my_param1;
            core_config_arr[i].param2 = my_param2;
        }
        rv = bcm_field_action_config_add(unit, ent, bcmFieldActionStat, cores_num, &core_config_arr[0]);
        if (rv != BCM_E_NONE) {
            printf("%s(): Error in : bcm_field_action_config_add() returned %d(%s)\n", proc_name, rv, bcm_errmsg(rv));
            return rv;
        }
        printf("%s(): bcm_field_action_config_add() done for Stat.\n", proc_name);
        /* for Redirect*/
        BCM_GPORT_LOCAL_SET(redir_gport, redir_port);
        modId = BCM_GPORT_MODPORT_MODID_GET(redir_gport);
        my_param0 = modId;
        my_param1 = redir_gport;
        my_param2 = BCM_ILLEGAL_ACTION_PARAMETER;
        my_param0_as_long = my_param0;
        my_param1_as_long = my_param1;
        my_param2_as_long = my_param2;
        printf("  param0 = 0x%08lX\n  param1 = 0x%08lX\n  param2 = 0x%08lX\n\r\n",
               my_param0_as_long, my_param1_as_long, my_param2);
        for(i = 0; i < cores_num; i++) {
            core_config_arr[i].param0 = my_param0;
            core_config_arr[i].param1 = my_param1;
            core_config_arr[i].param2 = my_param2;
        }
        rv = bcm_field_action_config_add(unit, ent, bcmFieldActionRedirect, cores_num, &core_config_arr[0]);
        if (rv != BCM_E_NONE) {
            printf("%s(): Error in : bcm_field_action_config_add() returned %d(%s)\n", proc_name, rv, bcm_errmsg(rv));
            return rv;
        }
        printf("%s(): bcm_field_action_config_add() done for redirect.\n", proc_name);
    } else { /* for ARAD, use only bcmFieldActionStat as counting-only is not supported */
        BCM_GPORT_LOCAL_SET(redir_gport, redir_port);
        modId = BCM_GPORT_MODPORT_MODID_GET(redir_gport);
        rv = bcm_field_action_add(unit, ent, bcmFieldActionStat, statId, redir_gport);
        if (rv != BCM_E_NONE) {
            printf("%s(): Error in : bcm_field_action_add() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
            return rv; 
        }
        printf("%s(): bcm_field_action_add() done for Stat.\n", proc_name);
    }

    /*Commit the entire group to hardware.*/
    printf("%s(): Commit group %d to hardware...", proc_name, group);
    rv = bcm_field_group_install(unit, group);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_group_install() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf(" Done.\n");

    printf("%s(): Done. Field processor configured.\n", proc_name);
    return rv;
}

/* FIELD CONFIG FOR ForwardingTypeNew */
int field_config_ftn(int unit,
                 int core,
                 bcm_field_group_t group,
                 int group_priority,
                 bcm_port_t in_port,
                 bcm_field_ForwardingType_t new_fwd_type,
                 int offset,
                 int fix) {
    int rv = BCM_E_NONE;
    uint8 is_jer;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent;
    bcm_gport_t in_gport;
    int action = bcmFieldActionForwardingTypeNew;
    int cores_num = 1; /* Needs to be configured for exactly one core, all others take the same */
    int i;
    bcm_field_action_core_config_t core_config_arr[1];
    char *proc_name;

    proc_name = "field_config_ftn";
    printf("%s(): Configure field processor to change Forwarding Type.\n", proc_name);

    rv = device_is_jer(unit, &is_jer);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : device_is_jer() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }

    /*Build the qset*/
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifySrcPort);

    /*Build the aset*/
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, action);

    /*Create group with given qset, id and priority*/
    rv = bcm_field_group_create_id(unit, qset, group_priority, group);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_group_create_id returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf("%s(): Group %d created.\n", proc_name, group);

    /*Attach aset to group*/
    rv = bcm_field_group_action_set(unit, group, aset);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_group_action_set returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf("%s(): ASET attached.\n", proc_name);
    /*Create empty entry*/
    rv = bcm_field_entry_create(unit, group, &ent);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_entry_create returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf("%s(): Entry %d created.\n", proc_name, ent);
    /*Set the qualifier value for entry*/
    BCM_GPORT_LOCAL_SET(in_gport, in_port);
    rv = bcm_field_qualify_SrcPort(unit, ent, 0, 0, in_gport, 0xffffffff);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_qualify_SrcPort returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf("%s(): Added qualifier: bcm_field_qualify_SrcPort.\n", proc_name);

    /*Set the action value for entry*/
    printf("%s(): Adding action...\n", proc_name);
    if(is_jer) { /* Use config API for Jericho and above*/
        for(i = 0; i < cores_num; i++) {
            core_config_arr[i].param0 = new_fwd_type;
            core_config_arr[i].param1 = offset;
            core_config_arr[i].param2 = fix;
        }

        rv = bcm_field_action_config_add(unit, ent, action, cores_num, &core_config_arr[0]);
        if (rv != BCM_E_NONE) {
            printf("%s(): Error in : bcm_field_action_config_add() returned %d(%s)\n", proc_name, rv, bcm_errmsg(rv));
            return rv;
        }
        printf("%s(): bcm_field_action_config_add() done.\n", proc_name);
    } else { /* Use old API for Arad */
        rv = bcm_field_action_add(unit, ent, action, new_fwd_type, offset);
        if (rv != BCM_E_NONE) {
            printf("%s(): Error in : bcm_field_action_add() returned %d(%s)\n", proc_name, rv, bcm_errmsg(rv));
            return rv;
        }
        printf("%s(): bcm_field_action_add() done.\n", proc_name);
    }

    /*Commit the entire group to hardware.*/
    printf("%s(): Commit group %d to hardware...", proc_name, group);
    rv = bcm_field_group_install(unit, group);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_group_install() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf(" Done.\n");

    printf("%s(): Done. Field processor configured.\n", proc_name);
    return rv;
}

/* Function to verify packet was redirected to out-PP-port */
int run_verify_redir(int unit, int core, bcm_port_t port) {
    int rv = BCM_E_NONE;
    bcm_port_t local_port ;
    uint32 value;
    char *proc_name;

    proc_name = "run_verify_redir";
    printf("%s(): Enter to match expected out-PP-port %d with actual out-PP-port on ETPP\r\n", proc_name, port);
    printf("%s(): Take the output, as given in BCM diagnostic command: \n diag pp sig block=%s from=%s to=%s name=%s\n",
           proc_name, "ETPP", "PRP", "Term", "Out_PP_Port");
    rv = dpp_dsig_read(unit, core, "ETPP", "PRP", "Term", "Out_PP_Port", &value, 1);
    local_port = value;
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in: dpp_dsig_read() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        printf("%s(): Exiting with pass, unable to get signal value to verify\n", proc_name);
        return BCM_E_NONE;
    }
    local_port = value;
    printf("Out_PP_Port value : %d (0x%x)\r\n", local_port, local_port);
    if (local_port != port) {
        printf("%s(): FAIL: Out_PP_Port is expected %d but received %d!\n", proc_name, port, local_port);
        return BCM_E_FAIL;
    }
    printf("%s(): Done. Verified redirection to Out_PP_Port 0x%x\n", proc_name, local_port);
    return BCM_E_NONE;
}

/* Function to verify packet was redirected by ACE type=1 redirection */
int run_verify_redir_ace(int unit, int core, bcm_port_t port) {
    int rv = BCM_E_NONE;
    bcm_port_t local_port;
    uint32 value;
    uint32 cud[2];
    char *proc_name;
    uint8 is_jer;

    proc_name = "run_verify_redir_ace";
    printf("%s(): Enter to match expected signals of the redirection resolution in Egress\r\n", proc_name);
    rv = device_is_jer(unit, &is_jer);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : device_is_jer() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }

    /*ERPP PMF*/
    if(is_jer) {
        /* For ACE redirect, expect ACE-type = 1 */
        printf("%s(): diag pp sig block=%s from=%s to=%s name=%s\n",
               proc_name, "ERPP", "PMF", "TM Action", "ACE_Type");
        rv = dpp_dsig_read(unit, core, "ERPP", "PMF", "TM Action", "ACE_Type", &value, 1);
        printf("%s(): ACE_Type value : %d (0x%x)\r\n", proc_name, value, value);
        if (value != 1) {
            printf("%s(): FAIL: ACE_Type is expected 1 but received %d!\n", proc_name, value);
            return BCM_E_FAIL;
        }
        /*  expect Action_Profile_Valid = 0 */
        printf("%s(): diag pp sig block=%s from=%s to=%s name=%s\n",
               proc_name, "ERPP", "PMF", "TM Action", "Action_Profile_Valid");
        rv = dpp_dsig_read(unit, core, "ERPP", "PMF", "TM Action", "Action_Profile_Valid", &value, 1);
        printf("%s(): Action_Profile_Valid value : %d (0x%x)\r\n", proc_name, value, value);
        if (value != 0) {
            printf("%s(): FAIL: Action_Profile_Valid is expected 0 but received %d!\n", proc_name, value);
            return BCM_E_FAIL;
        }
        /*ERPP TM Action*/
        /*  expect CUD.PP_DSP = -1 */
        /* Remove this check, as the signal is not supported in regression
        printf("%s(): diag pp sig block=%s from=%s to=%s name=%s\n",
               proc_name, "ERPP", "TM Action", "ETM", "CUD");
        rv = dpp_dsig_read(unit, core, "ERPP", "TM Action", "ETM", "CUD", &cud, 7);
        value = (cud[0] >> 18) % 256;
        printf("%s(): CUD[25:18] value : %d (0x%x)\r\n", proc_name, value, value);
        if (value != 255) {
            printf("%s(): FAIL: CUD.PP_DSP is expected -1 (0xFF) but received %d!\n", proc_name, value);
            return BCM_E_FAIL;
        }
        */
        /*ETPP PRP*/
        /* Expect PP_PCT_Index (PP-DSP?) to be as redirection port*/
        /* Remove this check, as the signal is not supported in regression
        printf("%s(): diag pp sig block=%s from=%s to=%s name=%s\n",
               proc_name, "ETPP", "PRP", "", "PP_PCT_Index");
        rv = dpp_dsig_read(unit, core, "ETPP", "PRP", "", "PP_PCT_Index", &value, 1);
        printf("%s(): PP_PCT_Index value : %d (0x%x)\r\n", proc_name, value, value);
        if (value != port) {
            printf("%s(): FAIL: PP_DSP is expected -%d (0x%x) but received %d!\n", proc_name, port, port, value);
            return BCM_E_FAIL;
        } 
        */ 
    }
    /* Expect Out-PP-Port to be as redirection port*/
    printf("%s(): diag pp sig block=%s from=%s to=%s name=%s\n",
           proc_name, "ETPP", "Fwd", "Encap", "Out_PP_Port");
    rv = dpp_dsig_read(unit, core, "ETPP", "PRP", "Term", "Out_PP_Port", &value, 1);
    local_port = value;
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in: dpp_dsig_read() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        printf("%s(): Exiting with pass, unable to get signal value to verify\n", proc_name);
        return BCM_E_NONE;
    }
    local_port = value;
    printf("%s(): Out_PP_Port value : %d (0x%x)\r\n", proc_name, local_port, local_port);
    if (local_port != port) {
        printf("%s(): FAIL: Out_PP_Port is expected %d but received %d!\n", proc_name, port, local_port);
        return BCM_E_FAIL;
    }
    printf("%s(): Out_PP_Port 0x%x is expected.\n", proc_name, local_port);
    /* Print Out-TM-Port, but it cannot change in ETPP (by spec) */
    printf("%s(): diag pp sig block=%s from=%s to=%s name=%s\n",
           proc_name, "ETPP", "Fwd", "Encap", "Out_TM_Port");
    rv = dpp_dsig_read(unit, core, "ETPP", "PRP", "Term", "Out_TM_Port", &value, 1);
    local_port = value;
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in: dpp_dsig_read() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        printf("%s(): Exiting with pass, unable to get signal value to verify\n", proc_name);
        return BCM_E_NONE;
    }
    local_port = value;
    printf("%s(): Out_TM_Port value : %d (0x%x)\r\n", proc_name, local_port, local_port);
    return BCM_E_NONE;
}

/* Function to verify packet was redirected by TM-port redirection */
int run_verify_redir_pmf(int unit, int core, bcm_port_t port) {
    int rv = BCM_E_NONE;
    bcm_port_t local_port;
    uint32 value;
    uint32 cud[7];
    char *proc_name;
    uint8 is_jer;

    proc_name = "run_verify_redir_pmf";
    printf("%s(): Enter to match expected signals of the redirection resolution in Egress\r\n", proc_name);

    rv = device_is_jer(unit, &is_jer);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : device_is_jer() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }

    /*ERPP PMF*/
    /* Print Out-TM-port*/
    printf("%s(): diag pp sig block=%s from=%s to=%s name=%s\n",
           proc_name, "ERPP", "PMF", "TM Action", "Out_TM_Port");
    rv = dpp_dsig_read(unit, core, "ERPP", "PMF", "TM Action", "Out_TM_Port", &value, 1);
    printf("%s(): Out_TM_Port value : %d (0x%x)\r\n", proc_name, value, value);
    if(is_jer) {
        /*Out-TM-port-updated*/
        printf("%s(): diag pp sig block=%s from=%s to=%s name=%s\n",
               proc_name, "ERPP", "PMF", "TM Action", "Out_TM_Port_Updated");
        rv = dpp_dsig_read(unit, core, "ERPP", "PMF", "TM Action", "Out_TM_Port_Updated", &value, 1);
        printf("%s(): Out_TM_Port_Updated value : %d (0x%x)\r\n", proc_name, value, value);
        if (value != 1) {
            printf("%s(): FAIL: Out_TM_Port_Updated is expected 1 but received %d!\n", proc_name, value);
            return BCM_E_FAIL;
        }
        printf("%s(): Out_TM_Port_Updated value 0x%x is expected.\n", proc_name, value);
    }
    /* Expect Out-PP-Port to be as redirection port*/
    printf("%s(): diag pp sig block=%s from=%s to=%s name=%s\n",
           proc_name, "ETPP", "PRP", "Term", "Out_PP_Port");
    rv = dpp_dsig_read(unit, core, "ETPP", "PRP", "Term", "Out_PP_Port", &value, 1);
    local_port = value;
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in: dpp_dsig_read() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        printf("%s(): Exiting with pass, unable to get signal value to verify\n", proc_name);
        return BCM_E_NONE;
    }
    local_port = value;
    printf("%s(): Out_PP_Port value : %d (0x%x)\r\n", proc_name, local_port, local_port);
    if (local_port != port) {
        printf("%s(): FAIL: Out_PP_Port is expected %d but received %d!\n", proc_name, port, local_port);
        return BCM_E_FAIL;
    }
    printf("%s(): Out_PP_Port 0x%x is expected.\n", proc_name, local_port);

    return BCM_E_NONE;
}

/* Function to verify packet was redirected to given outLIF */
int run_verify_newlif(int unit, int core) {
    int rv = BCM_E_NONE;
    bcm_gport_t local_outlif;
    uint32 value;
    char *proc_name = "run_verify_newlif";

    printf("%s(): Enter to match expected outlif %d with actual outlif on %s\r\n", proc_name, outlif, "ETPP");
    printf("%s(): Take the output, as given in BCM diagnostic command: \n diag pp sig block=%s from=%s to=%s name=%s\n",
           proc_name, "ETPP", "Encap", "LL", "Out_LIF");
    rv = dpp_dsig_read(unit, core, "ETPP", "Encap", "LL", "Out_LIF", &value, 1);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in dpp_dsig_read() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        printf("%s(): Exit without fail, because the signal value could not be obtained.\n", proc_name);
        return BCM_E_NONE;
    }
    local_outlif = value;
    printf("Out_LIF value : %d (0x%x)\r\n", local_outlif, local_outlif) ;
    if (local_outlif != outlif) {
        printf("%s(): FAIL: Out_LIF is expected %d but received %d!\n", proc_name, outlif, local_outlif);
        return BCM_E_FAIL;
    }

    printf("%s(): Done. Verified Out_LIF change to 0x%x.\n", proc_name, local_outlif);
    return rv;
}

/* Function to verify ForwardingTypeNew action took effect */
int run_verify_ftn(int unit, int core, bcm_field_ForwardingType_t new_fwd_type, int offset, int fix) {
    int rv = BCM_E_NONE;
    uint8 is_jer = 0;
    char *signal_name;
    char *block = "IRPP";
    char *from;
    char *to;
    uint32 signal_value = -1;
    int expected = -1;
    char *proc_name;

    proc_name = "run_verify_ftn";

    printf("%s(): Enter to verify Forwarding type change\n", proc_name);

    rv = device_is_jer(unit, &is_jer);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : device_is_jer() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }

    if(!is_jer) { /* For Arad and below, the index signal is single */
        printf("%s(): 1. Match Forwarding Offset Index\n", proc_name);
        /* Match expected forwarding offset index */
        signal_name = "Fwd_Offset_Index";
        expected = offset;
        from = "FER";
        to = "LBP";
    } else { /* For Jericho and above, the fix value is changed */
        printf("%s(): Match Forwarding Offset Fix\n", proc_name);
        /* Match expected offset local fix */
        signal_name = "Fwd_Offset_Fix";
        expected = fix;
        from = "PMF";
        to = "FER";
    }

    rv =  dpp_dsig_read(unit, core, block, from, to, signal_name, &signal_value, 1);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in dpp_dsig_read() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        printf("%s(): Exit without fail, because the signal value could not be obtained.\n", proc_name);
        return BCM_E_NONE;
    }
    printf("%s(): diag pp sig block=%s from=%s to=%s name=%s ", proc_name, block, from, to, signal_name);
    printf("==> value = %d \n", signal_value);
    if (signal_value != expected) {
        printf("%s(): FAIL: %s is expected %d but received %d!\n", proc_name, signal_name, expected, signal_value);
        return BCM_E_FAIL;
    }
    printf("%s(): Done. Verified %s change to 0x%x.\n", proc_name, signal_name, signal_value);

    return rv;
}

/*
Function to destroy the configured fieldgroup and entry
*/
int field_destroy(int unit, bcm_field_group_t group) {
    int rv = BCM_E_NONE;
    char *proc_name;

    proc_name = "field_destroy";
    /*destroy group and entry*/
    rv = bcm_field_group_flush(unit, group);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_group_flush(), returned %d\n", proc_name, rv);
        return rv;
    }

    printf("%s(): Done. Destroyed field group %d with its entries.\n", proc_name, group);
    return rv;
}


/* Main function to configure working L2 and field */
int stat_config_main (int unit, 
                      int core, 
                      int counter_id_0, 
                      int counter_proc, 
                      bcm_field_group_t group, 
                      int group_priority,
                      bcm_vlan_t vsi,
                      bcm_port_t in_port,
                      bcm_port_t out_port,
                      bcm_port_t redir_port, /* redirection local port, pass any if not apply */
                      bcm_gport_t gport, /*new out-LIF or modport; pass any if not apply */
                      int is_count,
                      int is_redir, 
                      int is_newlif) {
    int rv = BCM_E_NONE;
    bcm_gport_t newlif;
    char *proc_name;

    proc_name = "stat_config_main";
    rv = create_vlan_and_ports(unit, vsi, in_port, out_port, redir_port, is_redir);
    if (rv != BCM_E_NONE) {
        printf("%s():Error in function create_vlan_and_ports(), returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf("%s(): VLAN AND PORTS CREATED.\n", proc_name);
    if(is_newlif) {
        rv = create_lif(unit, vsi, redir_port, &newlif);
        if (rv != BCM_E_NONE) {
            printf("%s(): Error in function create_lif(), returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
            return rv;
        }
        printf("%s(): OUTLIF CREATED. newlif = 0x%x\n", proc_name, newlif);
        outlif = newlif; /* Fill file-global variable to later use for verify function */
        gport = newlif; /* Substitute the gport value to use for out-LIF redirection */
    }
    
    rv = field_config(unit, core, counter_id_0, counter_proc, group, group_priority, out_port, redir_port, gport, is_count, is_redir, is_newlif);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in function field_config(), returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf("%s(): FIELD CONFIGURATION CREATED.\n", proc_name);

    return rv;
}



/* Main function to configure working L2 and field for range qualifier testing */
int config_for_range_main (int unit, 
                      bcm_vlan_t vsi,
                      bcm_port_t in_port,
                      bcm_port_t out_port) {
    int rv = BCM_E_NONE;
    bcm_gport_t newlif;
    char *proc_name;

    proc_name = "config_for_range_main";
    rv = create_vlan_and_ports(unit, vsi, in_port, out_port, 0, 0);
    if (rv != BCM_E_NONE) {
        printf("%s():Error in function create_vlan_and_ports(), returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf("%s(): VLAN AND PORTS CREATED.\n", proc_name) ;
    rv = create_lif_for_range(unit, vsi, out_port) ;
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error in function create_lif_for_range(), returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf("%s(): OUTLIF CREATED. newlif = 0x%x\n", proc_name, outlif) ;

    return rv;
}
/* Small utility to get the value of the global 'outlif' variable */
int get_out_lif (void)
{
    return (outlif) ;
}

int main_ftn (int unit, 
                      int core, 
                      bcm_field_group_t group, 
                      int group_priority,
                      bcm_vlan_t vsi,
                      bcm_port_t in_port,
                      bcm_port_t out_port,
                      bcm_field_ForwardingType_t new_fwd_type,
                      int offset,
                      int fix) {
    int rv = BCM_E_NONE;
    bcm_gport_t newlif;
    char *proc_name;

    proc_name = "stat_config_ftn";
    rv = create_vlan_and_ports(unit, vsi, in_port, out_port, 0, 0);
    if (rv != BCM_E_NONE) {
        printf("%s():Error in function create_vlan_and_ports(), returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf("%s(): VLAN AND PORTS CREATED.\n", proc_name);

    rv = field_config_ftn(unit, core, group, group_priority, in_port, new_fwd_type, offset, fix);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in function field_config_ftn(), returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf("%s(): FIELD CONFIGURATION CREATED.\n", proc_name);

    return rv;
}

int main_in_modport_drop(int unit,
                         bcm_field_group_t group,
                         int group_priority,
                         bcm_gport_t modport) {
    int rv = BCM_E_NONE;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent;
    int action = bcmFieldActionDrop;
    char *proc_name;

    proc_name = "main_in_modport_drop";
    printf("%s(): Qualify on InPort given as ModPort and Drop.\n", proc_name);

    /*Build the qset*/
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);

    /*Build the aset*/
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, action);

    /*Create group with given qset, id and priority*/
    rv = bcm_field_group_create_id(unit, qset, group_priority, group);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_group_create_id returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf("%s(): Group %d created.\n", proc_name, group);

    /*Attach aset to group*/
    rv = bcm_field_group_action_set(unit, group, aset);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_group_action_set returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf("%s(): ASET attached.\n", proc_name);
    /*Create empty entry*/
    rv = bcm_field_entry_create(unit, group, &ent);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_entry_create returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf("%s(): Entry %d created.\n", proc_name, ent);
    /*Set the qualifier value for entry*/
    rv = bcm_field_qualify_InPort(unit, ent, modport, 0xffffffff);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_qualify_SrcPort returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf("%s(): Added qualifier: bcm_field_qualify_InPort.\n", proc_name);

    /*Set the action value for entry*/
    printf("%s(): Adding action...\n", proc_name);
    rv = bcm_field_action_add(unit, ent, action, 0, 0);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_action_add() returned %d(%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf("%s(): bcm_field_action_add() done.\n", proc_name);

    /*Commit the entire group to hardware.*/
    printf("%s(): Commit group %d to hardware...", proc_name, group);
    rv = bcm_field_group_install(unit, group);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_group_install() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf(" Done.\n");

    printf("%s(): Done. Field processor configured.\n", proc_name);

    return rv;
}

bcm_gport_t mirror_gport;

/* Main function to add  1025 entries to a field group with ActionStat and Action MirrorEgress */
int action_1k_entries(int unit, int group, int prio, int vid, bcm_port_t mirror_dest) {
    int rv = BCM_E_NONE;
    bcm_field_qset_t qset;
    bcm_field_entry_t ent;
    bcm_field_aset_t aset;
    bcm_gport_t local_gport;
    bcm_field_stat_t stats[2];
    int i = 0;
    uint32 flags;
    bcm_field_action_core_config_t action_config[2];
    bcm_mirror_options_t options;
    bcm_mirror_destination_t dest = {0};
    int core, modid;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    int pmf_mirror_profile = 0;
    uint8 is_jer, is_qax;
    int nof_cores = 0;
    char *proc_name = "action_1k_entries";

    rv = egress_mirror_check_oam_bfd_soc_init(unit, mirror_dest);
    if (rv != BCM_E_NONE) {
        printf("Error, egress_mirror_check_oam_bfd_soc_init\n");
        return rv;
    }

    rv = device_is_jer(unit, &is_jer);
    rv = device_is_qax(unit, &is_qax);
    if(is_jer && !is_qax) {
        nof_cores = 2;
    } else {
        nof_cores = 1;
    }
    BCM_FIELD_STAT_ID_SET(statId, 7, 0);
    printf("statId = %d \n", statId);

    rv = bcm_stk_my_modid_get(unit, &modid);
    if (rv != BCM_E_NONE) {
        printf("%s(): error(%d) in bcm_stk_my_modid_get() for unit(%d)\r\n",proc_name, rv,unit);
        return rv;
    }
    rv = bcm_port_get(unit,mirror_dest,&flags,&interface_info,&mapping_info);
    if (rv != BCM_E_NONE) {
        printf("%s(): error(%d) in bcm_port_get() for unit(%d)\r\n",proc_name, rv,unit);
        return rv;
    }
    core = mapping_info.core;
    modid += core;
    printf("%s(): For mirror destination port (%d), core is %d, modid is %d\r\n",proc_name, mirror_dest, core, modid);

    dest.flags = BCM_MIRROR_DEST_WITH_ID; 
  
    BCM_GPORT_MIRROR_SET(dest.mirror_dest_id, 1); 
    BCM_GPORT_MODPORT_SET(dest.gport,1024,10); 
    rv = bcm_mirror_destination_create(unit, &dest); 
 
    flags = BCM_MIRROR_PORT_EGRESS_ACL;
    bcm_mirror_options_t_init(&options);
    /*
    * Note that 'strength' is a two bits variable so
    * the range is 0->3
    */
    options.forward_strength = 2;
    options.copy_strength = 1;
    options.recycle_cmd = pmf_mirror_profile;

    printf("%s(): Invoke -- bcm_mirror_port_destination_add(unit %d, mirror_dest %d , flags 0x%08lX, dest.gport 0x%08lX, options)\r\n",
           proc_name, unit, mirror_dest, flags, dest.gport);

    rv = bcm_mirror_port_destination_add(unit, mirror_dest, flags, dest.mirror_dest_id, options);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mirror_port_destination_add\n");
        return rv;
    }
    BCM_GPORT_MIRROR_SET(mirror_gport, dest.mirror_dest_id);
    printf("mirror_gport = %d \n",mirror_gport);

    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageEgress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyOuterVlanId);
  
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionMirrorEgress);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionStat);

    rv = bcm_field_group_create_id(unit, qset, prio, group);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_group_create_id returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    /*Attach aset to group*/
    rv = bcm_field_group_action_set(unit, group, aset);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in : bcm_field_group_action_set returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
	action_config[0].param1 = -1;
	action_config[0].param2 = -1;
	action_config[1].param1 = -1;
    action_config[1].param2 = -1;

    for(i = 0; i < 1025; i++){
        action_config[0].param0 = statId + i;
        action_config[1].param0 = statId + i;
        rv=bcm_field_entry_create(unit, group, &ent); 
        if(rv != BCM_E_NONE){
            printf ("entry number=0x%x, bcm_field_entry_create return error\n",i);
            return rv;
        }
        rv = bcm_field_qualify_OuterVlanId(unit, ent, i, 0xfff);
	    if(rv != BCM_E_NONE){
    	    printf ("entry number=0x%x, bcm_field_qualify_OuterVlan return error\n", i);
    	    return rv;
	    }
        rv = bcm_field_action_config_add(unit, ent,  bcmFieldActionStat, nof_cores, &action_config[0]);
        if(rv != BCM_E_NONE){
    	    printf ("entry number=0x%x, bcm_field_action_config_add bcmFieldActionStat return error\n",i);
    	    return rv;
	    }

	    rv = bcm_field_action_add(unit, ent, bcmFieldActionMirrorEgress, modid, mirror_gport);  	  
	    if(rv != BCM_E_NONE){
            printf ("entry number=0x%x, bcm_field_action_add bcmFieldActionMirrorEgress return error\n",i);
            return rv;
	    }

	    rv = bcm_field_entry_install(unit, ent);
	    if(rv != BCM_E_NONE){
    	    printf ("entry number=0x%x, bcm_field_entry_install return error\n",i);
            return rv;
	    }
    }
    return rv;
}


/*
* Destroy mirror configuration.
*/
int egress_mirror_teardown(int unit, bcm_port_t mirror_dest) 
{
    int rv;
    bcm_mirror_destination_t dest;
    bcm_gport_t mirror_gport_destroy;

    BCM_GPORT_MIRROR_SET(mirror_gport_destroy, BCM_GPORT_MIRROR_GET(mirror_gport));

    rv = bcm_mirror_destination_get(unit, mirror_gport_destroy, &dest);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_mirror_destination_get\n");
        return rv;
    }

    rv = bcm_mirror_port_dest_delete(unit, mirror_dest, BCM_MIRROR_PORT_EGRESS_ACL, dest.mirror_dest_id);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_mirror_port_dest_delete\n");
        return rv;
    }

    rv = bcm_mirror_destination_destroy(unit, dest.mirror_dest_id);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_mirror_destination_destroy\n");
        return rv;
    }
    return rv;
}


