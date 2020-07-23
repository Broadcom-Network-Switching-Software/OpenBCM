/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_instru_ipt.c Purpose: IPT profiles configurations (INT nodes, Tail Edit)
 */

/*
 * File: cint_vpls_statistics.c
 * Purpose: Example of IPT profiles (INT, Tail Edit) congfiguration
 *
 * Configure full metadata for INT (32 bytes) and Tail-Edit (30 bytes) profiles and first header (iOAM) information for INT.
 *
 * The cint includes:
 * - Main function to configure IPT profiles and
 *
 * Usage:
 * ------
 *
 * Test Scenario:
 *--------------
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint
 * cint_instru_ipt_main(0,0xcd, 0x2);
 * exit;
 *
 * After calling this cint, it can be tested using PMF rules that yield these IPT profiles (INT/Tail-Edit)
 *
 * Note, to clear configurations back to default call:
 * cint
 * cint_instru_ipt_clear(0);
 * exit;
 */


/*
 * main function.
 * Calls:
 */

/**
 * \brief IPT profile IDs
 */
int CINT_INSTRU_IPT_TAIL_EDIT_PROFILE              = 1;
int CINT_INSTRU_IPT_INT_INTERMEDIATE_NODE_PROFILE  = 2;
int CINT_INSTRU_IPT_TAIL_EDIT_PROFILE_3            = 3;
int CINT_INSTRU_IPT_INT_FIRST_NODE_PROFILE         = 4;
int CINT_INSTRU_IPT_INT_LAST_NODE_PROFILE          = 6;

/*
 * Default values used for setting back defaults
 */
bcm_instru_ipt_t default_tail_edit_config;
bcm_instru_ipt_t default_int_first_config;
bcm_instru_ipt_t default_int_intermediate_config;
int default_switch_id;

int
cint_instru_ipt_main(
   int unit,
   int switch_id,
   int shim_type)
{
    int rv;
    uint32 ipt_profile;
    bcm_instru_ipt_t config;
    uint32 supported_flags_per_device = 0;

    /* Following flags shared between all devices:*/
    supported_flags_per_device = BCM_INSTRU_IPT_METADATA_FLAG_INGRESS_TOD | BCM_INSTRU_IPT_METADATA_FLAG_INGRESS_SYS_PORT |
            BCM_INSTRU_IPT_METADATA_FLAG_EGRESS_TOD | BCM_INSTRU_IPT_METADATA_FLAG_EGRESS_SYS_PORT |
            BCM_INSTRU_IPT_METADATA_FLAG_SWITCH_ID | BCM_INSTRU_IPT_METADATA_FLAG_QUEUE_INFO;

    if (*dnxc_data_get (unit,"instru","ipt","advanced_ipt",NULL))
    {
        /* Advanced IPT flags*/
        supported_flags_per_device |= BCM_INSTRU_IPT_METADATA_FLAG_QUEUE_SIZE_FG | BCM_INSTRU_IPT_METADATA_FLAG_LATENCY| BCM_INSTRU_IPT_METADATA_FLAG_PORT_RATE
            | BCM_INSTRU_IPT_METADATA_FLAG_QUEUE_SIZE_CG;

    } else {
        /* Basic IPT flags*/    
        supported_flags_per_device |= BCM_INSTRU_IPT_METADATA_FLAG_INGRESS_DEV_ID |
                BCM_INSTRU_IPT_METADATA_FLAG_EGRESS_RESERVED |  BCM_INSTRU_IPT_METADATA_FLAG_EGRESS_DEV_ID;
    }


    /** set tail-edit profile */
    {
        ipt_profile = CINT_INSTRU_IPT_TAIL_EDIT_PROFILE;

        /** get default */
        rv  = bcm_instru_ipt_profile_get(unit, 0, ipt_profile, &default_tail_edit_config);
        if (rv != BCM_E_NONE)
        {
            printf("bcm_instru_ipt_profile_get failed for Tail Edit\n");
            return rv;
        }

        bcm_instru_ipt_t_init(&config);

        config.node_type = bcmInstruIptNodeTypeTail;
        config.metadata_flags = supported_flags_per_device;

        rv  = bcm_instru_ipt_profile_set(unit, 0, ipt_profile, &config);
        if (rv != BCM_E_NONE)
        {
            printf("bcm_instru_ipt_profile_set failed for Tail Edit\n");
            return rv;
        }
    }

    if (0==*dnxc_data_get (unit,"instru","ipt","advanced_ipt",NULL))
    {
        /* Following configurations also use "ingress resereved" flags (Jer2 devices only)*/
        supported_flags_per_device |= BCM_INSTRU_IPT_METADATA_FLAG_INGRESS_RESERVED;
    }

    /** set INT-first node profile */
    {
        ipt_profile = CINT_INSTRU_IPT_INT_FIRST_NODE_PROFILE;

        /** get default */
        rv  = bcm_instru_ipt_profile_get(unit, 0, ipt_profile, &default_int_first_config);
        if (rv != BCM_E_NONE)
        {
            printf("bcm_instru_ipt_profile_get failed for INT first node\n");
            return rv;
        }

        bcm_instru_ipt_t_init(&config);

        config.node_type = bcmInstruIptNodeTypeIntOverVxlanFirst;
        config.metadata_flags = supported_flags_per_device;


        config.first_node_header.length = 0;
        config.first_node_header.options = 0xff;
        config.first_node_header.type = shim_type;

        rv = bcm_instru_ipt_profile_set(unit, 0, ipt_profile, &config);
        if (rv != BCM_E_NONE)
        {
            printf("bcm_instru_ipt_profile_set failed for INT first node\n");
            return rv;
        }
    }


    /** set INT-Intermediate node profile */
    {
        ipt_profile = CINT_INSTRU_IPT_INT_INTERMEDIATE_NODE_PROFILE;

        /** get default */
        rv  = bcm_instru_ipt_profile_get(unit, 0, ipt_profile, &default_int_intermediate_config);
        if (rv != BCM_E_NONE)
        {
            printf("bcm_instru_ipt_profile_get failed for INT intermediate node\n");
            return rv;
        }

        bcm_instru_ipt_t_init(&config);

        config.node_type = bcmInstruIptNodeTypeIntOverVxlanIntermediate;
        config.metadata_flags = supported_flags_per_device;


        rv = bcm_instru_ipt_profile_set(unit, 0, ipt_profile, &config);
        if (rv != BCM_E_NONE)
        {
            printf("bcm_instru_ipt_profile_set failed for INT intermediate node\n");
            return rv;
        }
    }

    /** get default switch id */
    rv = bcm_instru_control_get(unit, 0, bcmInstruControlIptSwitchId, &default_switch_id);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_instru_control_get failed for setting switch id\n");
        return rv;
    }

    /** set switch ID */
    rv = bcm_instru_control_set(unit, 0, bcmInstruControlIptSwitchId, switch_id);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_instru_control_set failed for setting switch id\n");
        return rv;
    }

    return rv;
}


int
cint_instru_ipt_clear(
   int unit)
{
    int rv;
    int ipt_profile;

    /** set tail-edit profile */
    {
        ipt_profile = CINT_INSTRU_IPT_TAIL_EDIT_PROFILE;

        rv  = bcm_instru_ipt_profile_set(unit, 0, ipt_profile, default_tail_edit_config);
        if (rv != BCM_E_NONE)
        {
            printf("bcm_instru_ipt_profile_set failed for default Tail Edit\n");
            return rv;
        }
    }

    /** set INT-first node profile */
    {
        ipt_profile = CINT_INSTRU_IPT_INT_FIRST_NODE_PROFILE;

        rv = bcm_instru_ipt_profile_set(unit, 0, ipt_profile, default_int_first_config);
        if (rv != BCM_E_NONE)
        {
            printf("bcm_instru_ipt_profile_set failed for default INT first node\n");
            return rv;
        }
    }


    /** set INT-Intermediate node profile */
    {
        ipt_profile = CINT_INSTRU_IPT_INT_INTERMEDIATE_NODE_PROFILE;

        rv = bcm_instru_ipt_profile_set(unit, 0, ipt_profile, default_int_intermediate_config);
        if (rv != BCM_E_NONE)
        {
            printf("bcm_instru_ipt_profile_set failed for default INT intermediate node\n");
            return rv;
        }
    }

    /** set switch ID */
    rv = bcm_instru_control_set(unit, 0, bcmInstruControlIptSwitchId, default_switch_id);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_instru_control_set failed for setting default switch id\n");
        return rv;
    }

    return rv;
}


/**
 * \brief - Enable ETPP trap to recycle for INT packets and forward the packet at 2nd path to new destination.
 *          The original packet (1st path) was destined to dst_port_first_path, the new destination is dst_port_second_path.
 *          Note - Forwarding is set using a simple trap -bcm_port_force_forward_set(), in practice it can be a more complex PMF rule.
 *
 * \param [in] unit -  Unit-ID
 * \param [in] dst_port_first_path -  original destination port (first path)
 * \param [in] dst_port_second_path -  new destination (after trap to recycle)
 */
int
cint_instru_ipt_trap_set(
   int unit,
   bcm_port_t dst_port_first_path,
   bcm_port_t dst_port_second_path)
{
    int rv;
    bcm_mirror_port_to_rcy_map_info_t rcy_map_info;

    /** enable ETPP trap for INT packets on original destination port */
    rv = bcm_instru_gport_control_set(unit, dst_port_first_path, 0, bcmInstruGportControlIptTrapToRcyEnable, 1);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_instru_gport_control_set failed for setting ETPP trap to recycle\n");
        return rv;
    }

    /** get mapped mirror recycle port of the original destination port */
    rv = bcm_mirror_port_to_rcy_port_map_get(unit , 0, dst_port_first_path, &rcy_map_info);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_mirror_port_to_rcy_port_map_get failed\n");
        return rv;
    }

    /** force forward the packet to new destination port */
    rv = bcm_port_force_forward_set(unit, rcy_map_info.rcy_mirror_port, dst_port_second_path, 1);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_port_force_forward_set failed\n");
        return rv;
    }

    return rv;
}


/**
 * \brief - Clear all configurations that were set in cint_instru_ipt_trap_set
 *
 * \param [in] unit -  Unit-ID
 * \param [in] dst_port_first_path -  original destination port (first path)
 * \param [in] dst_port_second_path -  new destination (after trap to recycle)
 */
int
cint_instru_ipt_trap_clear(
   int unit,
   bcm_port_t dst_port_first_path,
   bcm_port_t dst_port_second_path)
{
    int rv;
    bcm_mirror_port_to_rcy_map_info_t rcy_map_info;

    /** disable ETPP trap for INT packets on original destination port */
    rv = bcm_instru_gport_control_set(unit, dst_port_first_path, 0, bcmInstruGportControlIptTrapToRcyEnable, 0);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_instru_gport_control_set failed for setting ETPP trap to recycle\n");
        return rv;
    }

    /** get mapped mirror recycle port of the original destination port */
    rv = bcm_mirror_port_to_rcy_port_map_get(unit , 0, dst_port_first_path, &rcy_map_info);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_mirror_port_to_rcy_port_map_get failed\n");
        return rv;
    }

    /** disable force forward for the mapped mirror recycle port */
    rv = bcm_port_force_forward_set(unit, rcy_map_info.rcy_mirror_port, dst_port_second_path, 0);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_port_force_forward_set failed\n");
        return rv;
    }

    return rv;
}
