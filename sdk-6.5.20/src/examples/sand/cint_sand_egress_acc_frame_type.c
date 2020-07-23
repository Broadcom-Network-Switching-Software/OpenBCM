/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Example of acceptable frame type filter at ERPP and ingress
 *
 * ./bcm.user
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/sand/cint_egress_acceptable_frame_type.c
 * cint
 * egress_acceptable_frame_type_run(0,200,201, 1, 0);
 * exit;
 *
 * please NOTE that acceptable frame type filter at ERPP on JR2 couldn't work currently
 *
 * Example of acceptable frame type filter at ETPP and ingress
 *
 * ./bcm.user
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/sand/cint_egress_acceptable_frame_type.c
 * cint
 * egress_acceptable_frame_type_run(0,200,201, 0, 1);
 * exit;
 *
 * please NOTE that the trap for acceptable frame type filter at ETPP on JR1 are configured at init sequence
 */

/*
 * trap id value which indicated whether corresponding trap has been created or not
 */
int egress_erpp_trap_id = -1;
int egress_etpp_trap_id = -1;
/*
 * hepler function
 */
int is_tpid_invalid(uint32 tpid)
{
    return (tpid == BCM_PORT_TPID_CLASS_TPID_INVALID);
}

/*
 * Create trap and set drop action profile for ERPP application trap type
 */
int cint_acc_frame_type_erpp_trap_create_and_set(int unit)
{
    int rv = BCM_E_NONE;
    bcm_gport_t trap_gport = 0;
    bcm_rx_trap_config_t config;

    bcm_rx_trap_config_t_init(&config);

    config.dest_port = BCM_GPORT_BLACK_HOLE;
    config.flags |= BCM_RX_TRAP_UPDATE_DEST;

    /** Action-Profile of ERPP application traps is configured using bcmRxTrapEgUserDefine */
    rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapEgUserDefine, &egress_erpp_trap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_rx_trap_type_create %d for bcmRxTrapEgUserDefine\n", rv);
        return rv;
    }

    /** Set action of trap */
    rv = bcm_rx_trap_set(unit, egress_erpp_trap_id, &config);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_rx_trap_set %d", rv);
        return rv;
    }

    /** Encode trap_id, forward strength and snoop strength as gport */
    BCM_GPORT_TRAP_SET(trap_gport, egress_erpp_trap_id, 7, 0);

    /** Set the Trap-Action-Profile for application trap type bcmRxTrapEgDiscardFrameTypeFilter */
    rv = bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgDiscardFrameTypeFilter, trap_gport);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_rx_trap_action_profile_set: Couldn't update gprot for  trap %d\n", trap);
        return rv;
    }
    return rv;
}

/*
 * destroy ERPP trap
 */
int cint_acc_frame_type_erpp_trap_destroy(int unit, bcm_rx_trap_t trap_type)
{
    int rv = BCM_E_NONE;
    bcm_gport_t trap_gport = 0;
    int trap_id;

    /** Get the Trap-Action-Profile for trap of specific type */
    rv = bcm_rx_trap_action_profile_get(unit, trap_type, &trap_gport);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_rx_trap_action_profile_get %d\n", rv);
        return rv;
    }

    /** Clear the Trap-Action-Profile for trap of specific type */
    rv = bcm_rx_trap_action_profile_clear(unit, trap_type);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_rx_trap_action_profile_clear %d\n", rv);
        return rv;
    }

    /** Decode trap_id from trap gport */
    trap_id = BCM_GPORT_TRAP_GET_ID(trap_gport);

    /** Destroy trap */
    rv = bcm_rx_trap_type_destroy(unit, trap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_rx_trap_type_destroy %d\n", rv);
        return rv;
    }
    egress_erpp_trap_id = -1;

    return rv;
}

/*
 * Set drop action profile for ETPP application trap type
 */
int cint_acc_frame_type_etpp_trap_actions_set(int unit)
{
    int rv = BCM_E_NONE;
    bcm_gport_t trap_gport = 0;

    /*
     * Do not call create and set for pre-defined DROP filters.
     * We just assign the pre-defined DROP profile as trap ID(profile), which is HW created and exists.
     */
    BCM_GPORT_TRAP_SET(trap_gport, BCM_RX_TRAP_EG_TX_TRAP_ID_DROP, 7, 0);

    /** Set the Trap-Action-Profile for application trap type bcmRxTrapEgDiscardFrameTypeFilter */
    rv = bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgTxDiscardFrameTypeFilter, trap_gport);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_rx_trap_action_profile_set: Couldn't update gprot for  trap %d\n", trap);
        return rv;
    }

    /** Indicate etpp trap is configured */
    egress_etpp_trap_id = BCM_RX_TRAP_EG_TX_TRAP_ID_DROP;

    return rv;
}

/*
 * Clear action profile for ETPP application trap type
 */
int cint_acc_frame_type_etpp_trap_actions_destroy(int unit, bcm_rx_trap_t trap_type)
{
    int rv = BCM_E_NONE;

    /** Clear the Trap-Action-Profile for trap of specific type */
    rv = bcm_rx_trap_action_profile_clear(unit, trap_type);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_rx_trap_action_profile_clear %d\n", rv);
        return rv;
    }
    egress_etpp_trap_id = -1;

    return rv;
}


/*
 * Legacy way to create trap and set action for traps of acceptable frame type filter
 */
int cint_acc_frame_type_egress_trap_create_and_set_legacy(int unit, bcm_rx_trap_t trap_type, int drop_pkt)
{
    int rv = BCM_E_NONE;
    bcm_gport_t trap_gport = 0;
    bcm_rx_trap_config_t config;
    bcm_mirror_options_t mirror;
    int trap_id;

    bcm_rx_trap_config_t_init(&config);
    if (drop_pkt)
    {
        config.dest_port = BCM_GPORT_BLACK_HOLE;
        config.flags |= BCM_RX_TRAP_UPDATE_DEST;
        config.trap_strength = 2;
    }

    bcm_mirror_options_t_init(&mirror);
    config.mirror_cmd = &mirror;

    rv = bcm_rx_trap_type_create(unit, 0, trap_type, &trap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_rx_trap_type_create %d \n", rv);
        return rv;
    }

    rv = bcm_rx_trap_set(unit, trap_id, &config);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_rx_trap_set %d", rv);
        return rv;
    }

    if (trap_type == bcmRxTrapEgDiscardFrameTypeFilter)
    {
        egress_erpp_trap_id = trap_id;

    }
    else if (trap_type == bcmRxTrapEgTxDiscardFrameTypeFilter)
    {
        egress_etpp_trap_id = trap_id;

    }
    return rv;
}

/*
 * Legacy way to destroy trap
 */
int cint_acc_frame_type_egress_trap_destroy_legacy(int unit, bcm_rx_trap_t trap_type)
{
    int rv = BCM_E_NONE;

    if (trap_type == bcmRxTrapEgDiscardFrameTypeFilter)
    {
        if (egress_erpp_trap_id != -1)
        {
            rv = bcm_rx_trap_type_destroy(unit, egress_erpp_trap_id);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in bcm_rx_trap_type_destroy %d\n", rv);
                return rv;
            }
            egress_erpp_trap_id = -1;
        }
    }
    else if (trap_type == bcmRxTrapEgTxDiscardFrameTypeFilter)
    {
        if (egress_etpp_trap_id != -1)
        {
            rv = bcm_rx_trap_type_destroy(unit, egress_etpp_trap_id);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in bcm_rx_trap_type_destroy %d\n", rv);
                return rv;
            }
            egress_etpp_trap_id = -1;
        }
    }
    return rv;
}

/*
 * init trap to drop packets of unacceptable frame type
 */
int init_trap_to_drop(int unit)
{
    int rv = BCM_E_NONE;
    int is_jericho2 = 0;
    int drop_pkt = 0;

    rv = is_device_jericho2(unit, &is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in is_device_jericho2 %d\n, rv");
        return rv;
    }

    if (!is_jericho2)
    {
        /*
         * on JR1, bcmRxTrapEgTxDiscardFrameTypeFilter is configured at init sequence
         * via setting register EPNI_CFG_ACC_FRAME_TYPE_TRAP directly
         * create/destory to clear EPNI_CFG_ACC_FRAME_TYPE_TRAP here
         */
        rv = cint_acc_frame_type_egress_trap_create_and_set_legacy(unit, bcmRxTrapEgTxDiscardFrameTypeFilter, drop_pkt);
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
        {
            printf("Error, cint_acc_frame_type_egress_trap_create_and_set_legacy %d\n", rv);
            return rv;
        }
        rv = cint_acc_frame_type_egress_trap_destroy_legacy(unit, bcmRxTrapEgTxDiscardFrameTypeFilter);
        if (rv != BCM_E_NONE)
        {
            printf("Error, cint_acc_frame_type_egress_trap_create_and_set_legacy %d\n", rv);
            return rv;
        }
    }
    return rv;
}

/*
 * create and condfigure trap to drop packets of unacceptable frame type
 */
int set_trap_to_drop(int unit, bcm_rx_trap_t trap_type)
{
    int rv = BCM_E_NONE;
    int is_jericho2 = 0;

    rv = is_device_jericho2(unit, &is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in is_device_jericho2 %d\n, rv");
        return rv;
    }

    if (is_jericho2)
    {
        bcm_gport_t trap_gport = 0;

        if (trap_type == bcmRxTrapEgDiscardFrameTypeFilter)
        {
            rv = cint_acc_frame_type_erpp_trap_create_and_set(unit);
            if (rv != BCM_E_NONE)
            {
                printf("Error, cint_acc_frame_type_erpp_trap_create_and_set: %d\n", rv);
                return rv;
            }

        }
        else if (trap_type == bcmRxTrapEgTxDiscardFrameTypeFilter)
        {
            rv = cint_acc_frame_type_etpp_trap_actions_set(unit);
            if (rv != BCM_E_NONE)
            {
                printf("Error, cint_acc_frame_type_etpp_trap_actions_set: %d\n", rv);
                return rv;
            }
        }
    }
    else
    {
        int drop_pkt = 1;
        rv = cint_acc_frame_type_egress_trap_create_and_set_legacy(unit, trap_type, drop_pkt);
        if (rv != BCM_E_NONE)
        {
            printf("Error, cint_acc_frame_type_egress_trap_create_and_set_legacy: %d\n", rv);
            return rv;
        }

    }
    return rv;
}

/*
 * destory trap to drop packets of unacceptable frame type
 */
int clear_trap_to_drop(int unit, bcm_rx_trap_t trap_type)
{
    int rv = BCM_E_NONE;
    int is_jericho2 = 0;

    rv = is_device_jericho2(unit, &is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in is_device_jericho2 %d\n, rv");
        return rv;
    }

    if (is_jericho2)
    {
            if (trap_type == bcmRxTrapEgDiscardFrameTypeFilter)
            {
                rv = cint_acc_frame_type_erpp_trap_destroy(unit, trap_type);
                if (rv != BCM_E_NONE)
                {
                    printf("Error, in cint_acc_frame_type_erpp_trap_destroy %d\n", rv);
                    return rv;
                }
            }
            else if (trap_type == bcmRxTrapEgTxDiscardFrameTypeFilter)
            {
                rv = cint_acc_frame_type_etpp_trap_actions_destroy(unit, trap_type);
                if (rv != BCM_E_NONE)
                {
                    printf("Error, in cint_acc_frame_type_etpp_trap_actions_destroy %d\n", rv);
                    return rv;
                }

            }
    }
    else
    {
        /** Destroy trap */
        rv = cint_acc_frame_type_egress_trap_destroy_legacy(unit, trap_type);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in cint_acc_frame_type_egress_trap_create_and_set_legacy %d\n", rv);
            return rv;
        }
    }

    return rv;
}

/*
 * 1. ports[0]: configure tpid class: accept all ethernet tags.
 *    ports[1]: configure tpid class: accept all ethernet tags.
 *              Except untags.
 * 2. use basic bridge to forward, add mac entries to ports[0] and ports[1]
 * 3. configure acceptable frame type filter at egress without any trap
 * 4. send tag packet to ports[0], packets should be received
 * 5. send tag packet to ports[1], packets should be received
 * 6. send untag packet to ports[0], packets should be received
 * 7. send untag packet to ports[1], packets should be received
 * 8. configure trap for acceptable frame type at ERPP
 * 9. repeat 4~7, at step 7, packet should be discarded
 * 10. configure trap for acceptable frame type at ETPP
 * 11. repeat 4~7, at step 7, packet should be discarded
 */
int egress_acceptable_frame_type_run(
    int unit,
    int in_port,
    int out_port_1,
    int out_port_2,
    int enable_egress_erpp_trap,
    int enable_egress_etpp_trap)
{
    int rv = BCM_E_NONE;
    uint8 is_outer_prio;
    int vid = 20;

    /** ports */
    int port_nbr = 2;
    bcm_port_t ports[port_nbr] = {out_port_1, out_port_2};
    int port_index;
    bcm_port_tpid_class_t tpid_class;

    /** tpids */
    int tpid_nbr = 2;
    uint16 tpids[tpid_nbr] = {0x8100, 0x9100};
    int tpid_index;

    /** tpids + special tpid value: untag */
    int port_tpid_class_tpids_nbr = tpid_nbr + 1;
    uint32 port_tpid_class_tpids[port_tpid_class_tpids_nbr] = {0x8100, 0x9100, BCM_PORT_TPID_CLASS_TPID_INVALID};
    int port_tpid_class_tpid_index1, port_tpid_class_tpid_index2;

    /** mac table variables */
    bcm_l2_addr_t l2_addr;
    bcm_mac_t mac;
    int fid_untag = 1;
    int fid_tag = 20;

    /** temp shlomi, configure action id 0 to do nothing */
    bcm_vlan_action_set_t action;
    int action_id;

    int is_jericho2 = 0;

    rv = is_device_jericho2(unit, &is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in is_device_jericho2 %d\n, rv");
        return rv;
    }

    if (!is_jericho2)
    {
        for (port_index = 0; port_index < port_nbr; port_index++)
        {
            bcm_port_tpid_delete_all(unit, ports[port_index]);
        }
        /** add tpids to ports */
        for (port_index = 0; port_index < port_nbr; port_index++)
        {
            for (tpid_index= 0; tpid_index < tpid_nbr; tpid_index++)
            {
                rv = bcm_port_tpid_add(unit, ports[port_index], tpids[tpid_index], 0);
                if (rv != BCM_E_NONE)
                {
                    printf("Error, in bcm_port_tpid_add, port=%d, \n", ports[port_index]);
                    return rv;
                }
            }
        }
        /** setup tpid class for each port, for each allowed tpids */
        for (port_tpid_class_tpid_index1 = 0; port_tpid_class_tpid_index1 < port_tpid_class_tpids_nbr; port_tpid_class_tpid_index1++)
        {
            for (port_tpid_class_tpid_index2 = 0; port_tpid_class_tpid_index2 < port_tpid_class_tpids_nbr; port_tpid_class_tpid_index2++)
            {
                for (is_outer_prio = FALSE; is_outer_prio < TRUE; is_outer_prio++)
                {
                    /** loop on each port */
                    for (port_index = 0;port_index < port_nbr;port_index++)
                    {
                        /* configure tpid class for a tag structure */
                        bcm_port_tpid_class_t_init(&tpid_class);
                        /* tpid 1*/
                        tpid_class.tpid1 = port_tpid_class_tpids[port_tpid_class_tpid_index1];
                        /* tpid 2*/
                        tpid_class.tpid2 = port_tpid_class_tpids[port_tpid_class_tpid_index2];
                        /* priority */
                        if (is_outer_prio) {
                            tpid_class.flags |= BCM_PORT_TPID_CLASS_OUTER_IS_PRIO;
                        }

                        /** tag format: if untag: 0 */
                        if (is_tpid_invalid(tpid_class.tpid1) && is_tpid_invalid(tpid_class.tpid2))
                        {
                            tpid_class.tag_format_class_id = 0;
                        }
                        /** tag format: if 1 tag: 7 else 3 */
                        else if (!is_tpid_invalid(tpid_class.tpid1) && is_tpid_invalid(tpid_class.tpid2))
                        {
                            tpid_class.tag_format_class_id = 7;
                        }
                        else
                        {
                            tpid_class.tag_format_class_id = 3;
                        }

                        /** port */
                        tpid_class.port = ports[port_index];

                        /** set tpid class for class structure*/
                        bcm_port_tpid_class_set(unit, tpid_class);

                    }
                }
            }
        }
    }
    /*
     * Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, in_port, vid);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set vid\n");
        return rv;
    }

    /** discard untagged packets for both ingress and egress */
    bcm_port_tpid_class_t_init(&tpid_class);
    tpid_class.port = ports[1];
    tpid_class.tpid1 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    tpid_class.tag_format_class_id = 0;
    tpid_class.flags = BCM_PORT_TPID_CLASS_DISCARD;
    tpid_class.vlan_translation_action_id = 0;
    bcm_port_tpid_class_set(unit, &tpid_class);

    /** set erpp trap */
    if (enable_egress_erpp_trap)
    {
        /** if bcmRxTrapEgDiscardFrameTypeFilter has not been created, create it */
        if (egress_erpp_trap_id == -1)
        {
            set_trap_to_drop(unit, bcmRxTrapEgDiscardFrameTypeFilter);
        }
    }
    else
    {
        /** if bcmRxTrapEgDiscardFrameTypeFilter has been created, destory it */
        if (egress_erpp_trap_id != -1)
        {
            clear_trap_to_drop(unit, bcmRxTrapEgDiscardFrameTypeFilter);
        }
    }

    /** set etpp trap */
    if (enable_egress_etpp_trap)
    {
        /** if bcmRxTrapEgTxDiscardFrameTypeFilter has not been created, create it */
        if (egress_etpp_trap_id == -1)
        {
            set_trap_to_drop(unit, bcmRxTrapEgTxDiscardFrameTypeFilter);
        }
    }
    else
    {
        /** if bcmRxTrapEgTxDiscardFrameTypeFilter has been created, destory it */
        if (egress_etpp_trap_id != -1)
        {
            clear_trap_to_drop(unit, bcmRxTrapEgTxDiscardFrameTypeFilter);
        }
    }
    /*
     * add mact entries
     * fid      mac                port
     *   1    00:00:00:00:02       port[0]
     *  20    00:00:00:00:02       port[0]
     *   1    00:00:00:00:03       port[1]
     *  20    00:00:00:00:03       port[1]
     */
    mac[0] = mac[1] = mac[2] = mac[3] = mac[4] = 0;
    mac[5] = 2;

    /* Add l2 address */
    bcm_l2_addr_t_init(&l2_addr, mac, fid_untag);
    l2_addr.flags = BCM_L2_STATIC;
    l2_addr.port = ports[0];
    bcm_l2_addr_add(unit,&l2_addr);
    bcm_l2_addr_t_init(&l2_addr, mac, fid_tag);
    l2_addr.flags = BCM_L2_STATIC;
    l2_addr.port = ports[0];
    bcm_l2_addr_add(unit,&l2_addr);

    mac[5] = 3;
    bcm_l2_addr_t_init(&l2_addr, mac, fid_untag);
    l2_addr.flags = BCM_L2_STATIC;
    l2_addr.port = ports[1];
    bcm_l2_addr_add(unit,&l2_addr);
    bcm_l2_addr_t_init(&l2_addr, mac, fid_tag);
    l2_addr.flags = BCM_L2_STATIC;
    l2_addr.port = ports[1];
    bcm_l2_addr_add(unit,&l2_addr);

    return rv;
}



int egress_acceptable_frame_type_clear(
    int unit,
    int out_port_1,
    int out_port_2)
{
    int rv = BCM_E_NONE;
    bcm_port_tpid_class_t tpid_class;

    /** ports */
    bcm_port_t ports[2] = {out_port_1, out_port_2};

    /** remove the discard of untagged packets */
    bcm_port_tpid_class_t_init(&tpid_class);
    tpid_class.port = ports[1];
    tpid_class.tpid1 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    tpid_class.tag_format_class_id = 0;
    tpid_class.vlan_translation_action_id = 0;
    bcm_port_tpid_class_set(unit, &tpid_class);

    /** if bcmRxTrapEgDiscardFrameTypeFilter has been created, destory it */
    if (egress_erpp_trap_id != -1)
    {
        clear_trap_to_drop(unit, bcmRxTrapEgDiscardFrameTypeFilter);
    }
    /** if bcmRxTrapEgTxDiscardFrameTypeFilter has been created, destory it */
    if (egress_etpp_trap_id != -1)
    {
        clear_trap_to_drop(unit, bcmRxTrapEgTxDiscardFrameTypeFilter);
    }

    return rv;
}
